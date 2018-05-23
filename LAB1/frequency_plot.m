[x,sample_rate] = audioread('audiofile.ext'); % change audiofile.ext to audio file
t1 = 1.00; % seconds
t2 = 1.01; % seconds
i1 = round(t1*sample_rate); % convert time to index
i2 = round(t2*sample_rate); % convert time to index

nfft = 2^12; % FFT size
freq = ([0:nfft-1]/nfft-0.5)*sample_rate ; % frequency [Hz]
X = fft(x(i1:i2),nfft); % compute the discrete - Fourier transform
plot(freq,20*log10(abs(fftshift(X))));
% plot with accurately scaled frequency axis

xlabel('frequency [Hz]','FontSize',18);
ylabel('magnitude [dB]','FontSize',18);
set(gca,'FontSize',16)
grid on;
print -dpdf frequency_plot.pdf