[x,sample_rate] = audioread('audiofile.ext'); % change audiofile.ext to audio file

nfft = 2^8; % FFT size
overlap = round(0.8*nfft);
window = hamming(nfft);
y = (x(:,1)+x(:,2))/2; % use y if audiofile.ext is stereo
spectrogram(y,window,overlap,nfft,sample_rate); % x = mono, y = stereo

set(gca,'FontSize',16);
grid on;
print -dpdf spectrogram_plot.pdf