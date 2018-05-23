#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>

// This code uses single precision floating point arrays.
// FFTW must be configured to use floats by typing:
// ./configure --enable-float && make
// sudo make install
//
// To compile at the command line:
// gcc -o fftwf_test fftwf_test.c -lfftw3 -I/usr/local/include -L/usr/local/lib

int main(int argc, char *argv[])
{
	fftwf_plan p_fft, p_ifft;

	int nfft = 4;
	int i;

	double *x, *y;
	fftw_complex *X, *Y;
	x = (float*)calloc(sizeof(float),nfft);
	y = (float*)calloc(sizeof(float),nfft);
	X = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*nfft);
	Y = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*nfft);
	p_fft  = fftwf_plan_dft_r2c_1d(nfft, x, X, FFTW_MEASURE);
	p_ifft = fftwf_plan_dft_c2r_1d(nfft, Y, y, FFTW_MEASURE);

	x[0] =  4.3;
	x[1] = -2.8;
	x[2] = -1.6;
	x[3] = -0.6;

	fftwf_execute(p_fft); // X = fft(x) in MATLAB

	for (i=0; i<nfft; i++)
	{
		Y[i][0] = X[i][0];
		Y[i][1] = X[i][1];
	}

	fftwf_execute(p_ifft); // y = ifft(Y) in MATLAB

	for (i=0; i<nfft; i++)
	{
		printf("x[%d] = %20.10f, y[%d] = %20.10f\n",i,x[i],i,y[i]/nfft);
	}

	fftwf_destroy_plan(p_fft);
	fftwf_destroy_plan(p_ifft);
	fftwf_free(X);
	fftwf_free(Y);
	free(x);
	free(y);

	return 1;
}