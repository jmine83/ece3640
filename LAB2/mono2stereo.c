#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IOBUFFSIZE 1024

typedef struct
{
	int ndim;      /* number of dimensions; audio=1 */
	int nchan;     /* number of channels; monaural=1 stereo=2 */
	int d0;        /* length of first dimension; audio=L L=length */
	int d1;        /* length of second dimension; audio=Fs Fs=sample rate */
	int d2;        /* length of third dimension; audio=null */
} dsp_file_header;

int main(int argc, char *argv[])
{
	/* binary file read/write */
	FILE *I1, *I2, *O; /* I1=input 1 I2=input 2 O=output */
	if (NULL == (I1 = fopen("f1.bin","rb")))
	{
		printf("ERROR: Can't open f1.bin for input.\n");
		return 0;
	}
	if (NULL == (I2 = fopen("f2.bin","rb")))
	{
		printf("ERROR: Can't open f2.bin for input.\n");
		return 0;
	}
	if (NULL == (O = fopen("f.bin","wb")))
	{
		printf("ERROR: Can't open f.bin for output.\n");
		return 0;
	}

	/* header read/write */
	dsp_file_header h1, h2, h; /* h1=header input 1 h2=header input 2 h=header output */
	fread(&h1,sizeof(dsp_file_header),1,I1);
	fread(&h2,sizeof(dsp_file_header),1,I2);
	memcpy(&h,&h1,sizeof(dsp_file_header));
	h.nchan = 2; /* output file; monaural=1 stereo=2 */
	fwrite(&h,sizeof(dsp_file_header),1,O);

	/* monaural to stereo */
	float x1[IOBUFFSIZE];	/* input memory 1 */
	float x2[IOBUFFSIZE];	/* input memory 2 */
	float y[2*IOBUFFSIZE];	/* output memory */

	int m, n;
	int c0 = fread(x1,sizeof(float),IOBUFFSIZE,I1);	/* read in one buffer of audio from input 1 */
	int c1 = fread(x2,sizeof(float),IOBUFFSIZE,I2);	/* read in one buffer of audio from input 2 */
	int cin = (c0<c1?c0:c1);
	while (cin>0)
	{
		for (m=0, n=0; n<cin; n++)
		{
			/* interleave samples in output buffer */
			y[m] = x1[n]; m++;	/* store input 1 to output */
			y[m] = x2[n]; m++;	/* store input 2 to output */
		}
		fwrite(y,sizeof(float),m,O); /* write out the output buffer */
		c0 = fread(x1,sizeof(float),IOBUFFSIZE,I1); /* read in next sample of audio from input 1 */
		c1 = fread(x2,sizeof(float),IOBUFFSIZE,I2); /* read in next sample of audio from input 2 */
		cin = (c0<c1?c0:c1);
	}

	/* close binary files */
	fclose(I1);
	fclose(I2);
	fclose(O);
	return 1;
}