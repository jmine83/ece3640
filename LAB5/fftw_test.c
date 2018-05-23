#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>

/* To compile at the command line:											  */
/* gcc -o fftw_test fftw_test.c -lfftw3 -I/usr/local/include -L/usr/local/lib */

int main(int argc, char *argv[])
{
	fftw_plan p_fft, p_ifft;

	int nfft = 4;
	int i;

	double *x, *y;
	fftw_complex *X, *Y;
	x = (double*)calloc(sizeof(double),nfft);
	y = (double*)calloc(sizeof(double),nfft);
	X = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*nfft);
	Y = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*nfft);
	p_fft  = fftw_plan_dft_r2c_1d(nfft, x, X, FFTW_MEASURE);
	p_ifft = fftw_plan_dft_c2r_1d(nfft, Y, y, FFTW_MEASURE);

	x[0] =  4.3;
	x[1] = -2.8;
	x[2] = -1.6;
	x[3] = -0.6;

	fftw_execute(p_fft); // X = fft(x) in MATLAB

	for (i=0; i<nfft; i++)
	{
		Y[i][0] = X[i][0];
		Y[i][1] = X[i][1];
	}

	fftw_execute(p_ifft); // y = ifft(Y) in MATLAB

	for (i=0; i<nfft; i++)
	{
		printf("x[%d] = %20.10f, y[%d] = %20.10f\n",i,x[i],i,y[i]/nfft);
	}

	fftw_destroy_plan(p_fft);
	fftw_destroy_plan(p_ifft);
	fftw_free(X);
	fftw_free(Y);
	free(x);
	free(y);

	return 1;
}