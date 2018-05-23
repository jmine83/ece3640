#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// Program description:
// - Does convolution
// - Loads the entire input into memory
// - Computes convolution tails
// - Assumes an FIR filter
// - Assumes a single channel audio file for input (exists if two channels)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	int ndim; /* number of dimensions; audio=1 */
	int nchan; /* number of channels; monaural=1 stereo=2 */
	int dim0; /* length of first dimension; audio=L L=length */
	int dim1; /* length of second dimension; audio=Fs Fs=sample rate */
	int dim2; /* length of third dimension; audio=null */
} dsp_file_header;

int main(int argc, char *argv[])
{
	// Open files
	FILE *fx, *fy, *fh;
	if(NULL == (fx = fopen("fireflyintro.bin","rb")))
	{
		printf("ERROR: Cannot open fireflyintro.bin for input.\n");
		return 0;
	}
	if (NULL == (fy = fopen("fireflyintro-1.bin","wb")))
	{
		printf("ERROR: Cannot open fireflyintro-1.bin for output.\n");
		return 0;
	}
	if (NULL == (fh = fopen("lpf_260_400_44100_80dB.bin","rb")))
	{
		printf("ERROR: Cannot open impulse file lpf_260_400_44100_80dB.bin.\n");
		return 0;
	}

// Open files and read in/write out headers
	int Lx, Lh, Ly, Lz;
	dsp_file_header headx, heady, headh;
	fread(&headx,sizeof(dsp_file_header),1,fx);
	fread(&headh,sizeof(dsp_file_header),1,fh);
	Lh = headh.dim0;		// Length of impulse response
	Lx = headx.dim0;		// Length of input sequence
	Ly = Lx + (Lh-1);		// Compute convolution length (output sequence length)
	Lz = Lx + 2*(Lh-1);		// Compute length of input padded by Lh-1 zeros on both ends
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
	float *x = (float*)calloc(Lz,sizeof(float));
	float *h = (float*)calloc(Lh,sizeof(float));
	float *y = (float*)calloc(Ly,sizeof(float));

	// Read data into input array
	fread(x+Lh-1,sizeof(float),Lx,fx);

	// Read impulse response (natural order in file, reverse in memory)
	int n, k;
	float t;
	fread(h,sizeof(float),Lh,fh); // Read in the impulse response in natural order
	for(n=0; n<Lh/2; n++)
	{
		t = h[n];
		h[n] = h[Lh-1-n];
		h[Lh-1-n] = t;
	}

	// Processing
	for(n=0; n<Ly; n++)
	{
		t = 0.0; // Initialize
		for(k=0; k<Lh; k++)
		{
			t += h[k]*x[k+n]; // Mulitply and accumulate into local variable
		}
		y[n] = t; // Save result in output array
	}

	// Write convolution result to output file
	fwrite(y,sizeof(float),Ly,fy);

	// Close files and free memory
	fclose(fx);
	fclose(fy);
	fclose(fh);
	free(x);
	free(y);
	free(h);
}