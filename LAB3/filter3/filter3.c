#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

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

int main(int argc,char *argv[])
{
	// Open Files
	FILE *fx,*fy;
	if(NULL == (fx = fopen("cameraman.bin","rb")))
	{
		printf("ERROR: Cannot open cameraman.bin for input.\n");
		return 0;
	}
	if(NULL == (fy = fopen("cameraman-3.bin","wb")))
	{
		printf("ERROR: Cannot open cameraman-3.bin for output.\n");
		return 0;
	}

	// Read In & Write Out Headers
	int Rx,Rh,Cx,Ch,Ry,Cy,Rz,Cz;
	dsp_file_header headx,heady;
	fread(&headx,sizeof(dsp_file_header),1,fx); // Read In Header
	Rx = headx.dim0;		// Number of Rows in Input Image
	Rh = 3;					// Number of Rows in Impulse Filter
	Cx = headx.dim1;		// Number of Columns in Input Image
	Ch = 3;					// Number of Columns in Impulse Filter
	Ry = Rx + (Rh-1);		// Length of Convolution Result for Rows
	Cy = Cx + (Ch-1);		// Length of Convolution Result for Columns
	Rz = Rx + 2*(Rh-1);		// Length of Double Zero Padded Image for Rows
	Cz = Cx + 2*(Ch-1);		// Length of Double Zero Padded Image for Columns
	memcpy(&heady,&headx,sizeof(dsp_file_header)); // Copy Input Header to Output Header
	heady.dim0 = Ry;		// Number of Rows in Output Image
	heady.dim1 = Cy;		// Number of Columns in Output Image
	fwrite(&heady,sizeof(dsp_file_header),1,fy);	// Write Out Header

	// Error Checking
	if(headx.nchan > 1)
	{
		printf("ERROR: This program only processes single channel signals.\n");
		return 0;
	}

	// Memory Allocation
	float *x = (float*)calloc(sizeof(float),Rz*Cz);
	float *y = (float*)calloc(sizeof(float),Ry*Cy);
	float *yx = (float*)calloc(sizeof(float),Ry*Cy);
	float *yy = (float*)calloc(sizeof(float),Ry*Cy);

	// Read Data into Input Array
	fread(x+(Ry*Cy-Rx*Cx),sizeof(float),Rx*Cx,fx);

	// Impulse Filter
	int hx[3][3]; // Row Mask
	int hy[3][3]; // Columns Mask
	// Row Mask Matrix (Reversed)
	hx[0][0] = 1; hx[0][1] = 0; hx[0][2] = -1;
	hx[1][0] = 2; hx[1][1] = 0; hx[1][2] = -2;
	hx[2][0] = 1; hx[2][1] = 0; hx[2][2] = -1;
	// Columns Mask Matrix (Reversed)
	hy[0][0] = 1; hy[0][1] = 2; hy[0][2] = 1;
	hy[1][0] = 0; hy[1][1] = 0; hy[1][2] = 0;
	hy[2][0] = -1; hy[2][1] = -2; hy[2][2] = -1;

	// Processing
	int k,l,i,j;
	float Rtmp,Ctmp;
	for(k=0; k<Ry; k++) // Loop over rows in output image.
	{
		for(l=0; l<Cy; l++) // Loop over columns in output image.
		{
			for(Rtmp=0.0,i=0; i<Rh; i++) // Loop over rows in impulse filter.
			{
				for(Ctmp=0.0,j=0; j<Ch; j++) // Loop over columns in impulse filter.
				{
					Rtmp += hx[i][j]*x[(k+i)*Cz+(l+j)]; // Multiply and Accumulate the Rows
					Ctmp += hy[i][j]*x[(k+i)*Cz+(l+j)]; // Multiply and Accumulate the Columns
				}
			}
			yx[k*Cy+l] = Rtmp; // Save result to output image for rows.
			yy[k*Cy+l] = Ctmp; // Save result to output image for columns.
		}
	}
	y = sqrt(pow(yx,2)+pow(yy,2)); // Combine the rows output image with the columns output image.

	// Write Data into Output Array
	fwrite(y,sizeof(float),Ry*Cy,fy);

	// Close Files and Clear Memory
	fclose(fx);
	fclose(fy);
	free(x);
	free(y);
	free(yx);
	free(yy);
}