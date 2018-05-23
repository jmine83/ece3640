// Up Sampling

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define IOBUFFSIZE 2048

typedef struct
{
	int ndim; /* Number of Dimensions; audio=1 */
	int nchan; /* Number of Channels; monaural=1 stereo=2 */
	int dim0; /* Length of First Dimension; audio=L L=length */
	int dim1; /* Length of Second Dimension; audio=Fs Fs=sample rate */
	int dim2; /* Length of Third Dimension; audio=null */
} dsp_file_header;

int main(int argc,char *argv[])
{
	// Open Files
	FILE *fx,*fy,*fh;
	if(NULL == (fx = fopen("input.bin","rb")))
	{
		printf("ERROR: Cannot open input.bin for input.\n");
		return 0;
	}
	if(NULL == (fy = fopen("output.bin","wb")))
	{
		printf("ERROR: Cannot open output.bin for output.\n");
		return 0;
	}
	if(NULL == (fh = fopen("impulse.bin","rb")))
	{
		printf("ERROR: Cannot open impulse file impulse.bin.\n");
		return 0;
	}

	// Read In & Write Out Headers
	int Lx,Lh,Ly,U,L,M;
	dsp_file_header headx,heady,headh;
	fread(&headx,sizeof(dsp_file_header),1,fx); // Read in Input Header
	fread(&headh,sizeof(dsp_file_header),1,fh); // Read in Impulse Header
	Lx = headx.dim0;	// Length of Input
	Lh = headh.dim0;	// Length of Impulse Response
	U = headh.dim2;		// Up Sample Multiplier
	L = Lh;				// Length of Impulse Response
	M = ceil(L/U);		// Length of Impulse Response (Modified)
	memcpy(&heady,&headx,sizeof(dsp_file_header)); // Copy Input Header to Output Header
	Ly = heady.dim0;	// Length of Output
	fwrite(&heady,sizeof(dsp_file_header),1,fy); // Write out Output Header

    // Error Checking
	if(headx.nchan > 1)
	{
		printf("ERROR: This program only processes single channel signals.\n");
		return 0;
	}
	
	// Memory Allocation
	float *h = (float*)calloc(L,sizeof(float));	// Dynamic Array for Impulse Response
	float *g = (float*)calloc(L,sizeof(float));	// Dynamic Array for Circular Buffer
	float x[IOBUFFSIZE], y[IOBUFFSIZE];			// Static Arrays for I/O

	// Read Impulse Response (natural order in file; reverse in memory)
	fread(h,sizeof(float),Lh,fh);

	// Processing
	float t;			// Variable for accumulating convolution result
	int xlen,ylen=0;	// Indexes for input and output buffers
	int i;				// Index for input data buffer
	int j;				// Index for up sampling loop
	int k=0;			// Index for circular data buffer
	int m;				// Convolution loop index for filter coefficients
	int n;				// Convolution loop index for circular data buffer
	xlen = fread(x,sizeof(float),IOBUFFSIZE,fx); // Read in first chunk of input samples
	while (xlen > 0)
	{ // While there are samples to be processed, keep processing
		for (i=0; i<xlen; i++)
		{ // Process each of the input samples
			k = (k+M-1) % M;	// Update circular index of filter circular data buffer
			g[k] = x[i];		// Put each sample into the filter circular data buffer
			for (j=0; j<U; j++)
			{ // Loop over the up sampled outputs
				for (t=0.0, m=0, n=0; n<M; n++, m+=U)
				{ // Convolution loop
					t += h[m+j]*g[(k+n) % M]; // Multiply and accumulate into local variable
				} // End convolution loop
				y[ylen] = t;	// Save result into output buffer
				ylen++;			// Increment the index for the output buffer
				if (ylen==IOBUFFSIZE)
				{ // If output buffer is full, then save it to output file
					fwrite(y,sizeof(float),ylen,fy); // Write the output buffer
					ylen = 0;	// Reset the inex for the output buffer
				}
			} // End loop over up sampled outputs
		} // End loop over inputs samples
		xlen = fread(x,sizeof(float),IOBUFFSIZE,fx); // Read in next chunk of input samples
	}
	if(ylen>0) // Finish writing the last chunk of output samples
	{ // If output buffer is full, then save it to output file
		fwrite(y,sizeof(float),ylen,fy); // Write the output buffer
		ylen = 0; // Reset the index for the output buffer
	}

	// Numer of I/O samples
	printf("Number input samples = %d\n",Lx);
	printf("Number output samples = %d\n",Ly);

	// Close files and free memory
	fclose(fx);
	fclose(fy);
	fclose(fh);
	free(g);
	free(h);

	system("pause>nul");
	return 1;
}