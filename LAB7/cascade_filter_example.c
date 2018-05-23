#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IOBUFFSIZE 4096

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
	float h0[5] = { 0.0439, 0.4578, 1.0000, 0.4578, 0.0439 };		// Impulse response, filter 0
	float h1[5] = { -0.0598, -0.2992, 1.6197, -0.2992, -0.0598 };	// Impulse response, filter 1
	float h2[5] = { -1, 2, -1 };									// Impulse response, filter 2
	float x0[5] = { 0, 0, 0, 0, 0 };			// Init. data buffer for filter 0
	float x1[5] = { 0, 0, 0, 0, 0 };			// Init. data buffer for filter 1
	float x2[5] = { 0, 0, 0, 0, 0 };			// Init. data buffer for filter 2
	int L0 = 5, L1 = 5, L2 = 3;					// Set filter lengths
	int i, n, i0 = L0-1, i1 = L1-1, i2 = L2-1;	// Initialize indexes for circular buffers
	float t0, t1, t2;							// Accumulators for filters

	// Open files
	float x[IOBUFFSIZE], y[IOBUFFSIZE];
	FILE* fx = fopen("fireflyintro.bin","rb");		// Input file
	FILE* fy = fopen("fireflyintro-2.bin","rb");	// Output file
	FILE* fh = fopen("hilbert_filter.bin", "rb");	// Hilbert filter

	int Lx, Lh, Ly;
	dsp_file_header headx, heady, headh;
	fread(&headx,sizeof(dsp_file_header),1,fx);
	fread(&headh,sizeof(dsp_file_header),1,fh);
	Lh = headh.dim0;		// Length of Hilbert filter
	Lx = headx.dim0;		// Length of input sequence
	Ly = Lx;				// Output is the same length as input
	memcpy(&heady,&headx,sizeof(dsp_file_header));
	heady.dim0 = Ly;
	fwrite(&heady,sizeof(dsp_file_header),1,fy);
	
	// Processing
	int len = fread(x,sizeof(float),IOBUFFSIZE,fx);	// Read in first chunk into input buffer
	while(len>0)
	{
		for(i=0; i<len; i++)
		{
			// Run parallel combination of filters h0 and h1
			x0[i0] = x[i];	// Same input into both filters
			x1[i1] = x[i];	// Same input into both filters
			t0 = 0.0; t1 = 0.0;	// Initialize accumulators
			for (n=0; n<L0; n++)
			{
				t0 += h0[n]*x0[(i0+n) % L0];
				t1 += h1[n]*x1[(i1+n) % L1];
			}
			i0--;					// Update circular index
			i0 = (i0 + L0) % L0;	// Update circular index
			i1--;					// Update circular index
			i1 = (i1 + L1) % L1;	// Update circular index

			// x2[i2] = t0 + t1;		// Sum outputs of h0 and h1 and pass to h2
			// Replace above with arctan function

			// Insert phase unwrapping and instantaneous frequency calculation
			
			
			
			t2 = 0.0;				// Initialize accumulator
			for(n=0; n<L2; n++)
			{
				t2 += h2[n]*x2[(i2+n) % L2];
			}

			y[i] = t2;	// Save output
		}

		fwrite(y,sizeof(float),len,fy);				// Write output buffer
		len = fread(x,sizeof(float),IOBUFFSIZE,fx);	// Read in next chunk into input buffer
	}

	fclose(fx);
	fclose(fy);
}