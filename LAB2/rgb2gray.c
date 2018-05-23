#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	int ndim;      /* number of dimensions; video=3 */
	int nchan;     /* number of channels; gray=1 RGB=3 */
	int d0;        /* length of first dimension; video=M M=number of rows */
	int d1;        /* length of second dimension; video=N N=number of columns */
	int d2;        /* length of third dimension; video=T T=number of frames */
} dsp_file_header;

int main(int argc, char *argv[])
{
	/* binary file read/write */
	FILE *I, *O; /* I=input O=output */
	if (NULL == (I = fopen("xylophone.bin","rb")))
	{
		printf("ERROR: Can't open xylophone.bin for input.\n");
		return 0;
	}
	if (NULL == (O = fopen("xylophone-G.bin","wb")))
	{
		printf("ERROR: Can't open xylophone-G.bin for output.\n");
		return 0;
	}

	/* header read/write */
	dsp_file_header hI, hO; /* hI=header input hO=header output */
	fread(&hI,sizeof(dsp_file_header),1,I);
	memcpy(&hO,&hI,sizeof(dsp_file_header));
	hO.nchan = 1; /* output file; gray=1 RGB=3 */
	fwrite(&hO,sizeof(dsp_file_header),1,O);

	/* RGB to gray */
	float *x = (float*)calloc(sizeof(float),hI.d2*hI.d0*hI.d1*hI.nchan); /* input memory */
	float *y = (float*)calloc(sizeof(float),hO.d2*hO.d0*hO.d1*hO.nchan); /* output memory */

	int t=0, m, n, p=0; /* t=frame m=row n=column p=pixel */
	float R, G, B; /* value of each color channel of a pixel */
	int fin = fread(x,sizeof(float),hI.d0*hI.d1*hI.nchan,I); /* read in one frame of RGB video */
	while (t<hI.d2)
	{
		for (m=0; m<hI.d0; m++)
		{
			for (n=0; n<hI.d1; n++)
			{
				/* convert to grayscale using the formula */
				R = 0.2989*x[((t*hI.d0+m)*hI.d1+n)*hI.nchan+0]; /* 0=red */
				G = 0.5870*x[((t*hI.d0+m)*hI.d1+n)*hI.nchan+1]; /* 1=green */
				B = 0.1140*x[((t*hI.d0+m)*hI.d1+n)*hI.nchan+2]; /* 2=blue */
				y[p] = R + G + B; /* store gray value to output */
				p++; /* advance to next pixel */
				if (p == hI.d0*hI.d1-1)
				{
					fwrite(y,sizeof(float),p,O); /* write out the grayscale frame */
					/* read in next frame of RGB video */
					fin = fread(x,sizeof(float),hI.d0*hI.d1*hI.nchan,I); 
					p = 0; /* reset pixel count */
					t++; /* increment frame count */
				}
			}
		}
		
	}

	/* close binary files */
	fclose(I);
	fclose(O);
	return 1;
}