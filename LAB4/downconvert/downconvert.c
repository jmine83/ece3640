// Down Sampling

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	int Lx,Lh,Ly,D,L;
	dsp_file_header headx,heady,headh;
	fread(&headx,sizeof(dsp_file_header),1,fx); // Read in Input Header
	fread(&headh,sizeof(dsp_file_header),1,fh); // Read in Impulse Header
	Lx = headx.dim0;		// Length of Input
	Lh = headh.dim0;		// Length of Impulse Response
	D = headh.dim2;			// Down Sample Divisor
	L = Lh;					// Length of Impulse Response
	memcpy(&heady,&headx,sizeof(dsp_file_header)); // Copy Input Header to Output Header
	Ly = heady.dim0;		// Length of Output
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
	float x[IOBUFFSIZE],y[IOBUFFSIZE];				// Static Arrays for I/O

	// Read Impulse Response (natural order in file; reverse in memory)
	fread(h,sizeof(float),Lh,fh);

	// Processing
	float t;			// Variable for accumulating convolution result
	int xlen, ylen=0;	// Indexes for input and output buffers
	int i;		// Index for input data buffer
	int l=1;	// Down sampling counter
	int k=0;	// Index for circular data buffer
	int n;		// Convolution loop index for filter coefficients and circular data buffer
	xlen = fread(x,sizeof(float),IOBUFFSIZE,fx); // Read in first chunk of input samples
	while(xlen>0)
	{ // While there are samples to be processed, keep processing
		for(i=0; i<xlen; i++)
		{ // Process each of the input samples
			k = (k+L-1) % L;		// Update circular index of filter circular data buffer
			g[k] = x[i];			// Put each sample into the filter circular data buffer
			l = (l+D-1) % D;		// Update downsampling index
			if(l==0)
			{ // Down sampling condition: Only compute convolution result when needed
				l = D;				// Reset the down sampling counter
				for(t=0.0,n=0; n<L; n++)
				{ // Convolution loop
					t += h[n]*g[(k+n) % L]; // Multiply and accumulate into local variable
				} // End convolution loop
				y[ylen] = t;		// Save result into output buffer
				ylen++;				// Increment the index for the output buffer
				if(ylen==IOBUFFSIZE)
				{ // If output buffer is full, then save it to output file
					fwrite(y,sizeof(float),ylen,fy); // Write the output buffer
					ylen = 0;		// Reset the inex for the output buffer
				}
			} // End down sampling condition
		} // End loop over input samples
		xlen = fread(x,sizeof(float),IOBUFFSIZE,fx); // Read in next chunk of input samples
	} // Finish writing the last chunk of output samples
	if(ylen>0) 
	{ // If output buffer is full, then save it to output file
		fwrite(y,sizeof(float),ylen,fy); // Write the output buffer
		ylen = 0;		// Reset the index for the output buffer
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