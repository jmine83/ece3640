#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// Program description:
// - Does convolution
// - Loads the portion of input into memory
// - Computes convolution tails
// - Assumes an FIR filter
// - Assumes a single channel audio file for input (exists if two channels)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IOBUFFSIZE 2048

typedef struct
{
	int ndim; /* number of dimensions; audio=1 */
	int nchan; /* number of channels; monaural=1 stereo=2 */
	int dim0; /* length of first dimension; audio=L L=length */
	int dim1; /* length of second dimension; audio=Fs Fs=sample rate */
	int dim2; /* length of third dimension; audio=null */
} dsp_file_header;

int main(int argc,char *argv[])
{
	// Open files
	FILE *fx,*fy,*fh;
	if(NULL == (fx = fopen("fireflyintro.bin","rb")))
	{
		printf("ERROR: Cannot open fireflyintro.bin for input.\n");
		return 0;
	}
	if(NULL == (fy = fopen("fireflyintro-2.bin","wb")))
	{
		printf("ERROR: Cannot open fireflyintro-2.bin for output.\n");
		return 0;
	}
	if(NULL == (fh = fopen("lpf_260_400_44100_80dB.bin","rb")))
	{
		printf("ERROR: Cannot open impulse file lpf_260_400_44100_80dB.bin.\n");
		return 0;
	}

	// Open files and read in/write out headers
	int Lx,Lh,Ly;
	dsp_file_header headx,heady,headh;
	fread(&headx,sizeof(dsp_file_header),1,fx);
	fread(&headh,sizeof(dsp_file_header),1,fh);
	Lh = headh.dim0;		// Length of impulse response
	Lx = headx.dim0;		// Length of input sequence
	Ly = Lx;				// Output is the same length as input
	memcpy(&heady,&headx,sizeof(dsp_file_header));
	heady.dim0 = Ly;
	fwrite(&heady,sizeof(dsp_file_header),1,fy);

	// Error checking
	if(headx.nchan > 1)
	{
		printf("ERROR: This program only processes single channel signals.\n");
		return 0;
	}

	// Memory allocation
	float *h = (float*)calloc(Lh,sizeof(float)); // Dynamic array for impulse response
	float *g = (float*)calloc(Lh,sizeof(float)); // Dynamic array for circular buffer
	float x[IOBUFFSIZE], y[IOBUFFSIZE]; // Static I/O arrays

	// Read data into input array
	fread(x+Lh-1,sizeof(float),Lx,fx);

	// Read impulse response (natural order in file, reverse in memory)
	int n;
	float t;
	fread(h,sizeof(float),Lh,fh); // Read in the impulse response in natural order
	for(n=0; n<Lh/2; n++) // Time reverse the impulse response in place
	{
		t = h[n];
		h[n] = h[Lh-1-n];
		h[Lh-1-n] = t;
	}

	// Processing
	int len,i,k=Lh-1;
	len = fread(x,sizeof(float),IOBUFFSIZE,fx);
	while(len>0)
	{
		for(i=0; i<len; i++) // Loop over all the samples read in
		{
			g[k] = x[i]; // Put each sample into the circular buffer
			t = 0.0; // Initialize the accumulator
			for(n=0; n<Lh; n++)
			{
				t += h[n]*g[(n+k) % Lh]; // Mulitply and accumulate into local variable
			}
			y[i] = t; // Save result into output array
			k--;	// Decrement circular index
			k = (k+Lh) % Lh;	// Takes care of the negative mod thing
		}
		fwrite(y,sizeof(float),len,fy);
		len = fread(x,sizeof(float),IOBUFFSIZE,fx);
	}

	// Close files and free memory
	fclose(fx);
	fclose(fy);
	fclose(fh);
	free(g);
	free(h);

	return 1;
}