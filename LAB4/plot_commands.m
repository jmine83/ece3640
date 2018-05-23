fid = fopen(‘*.bin','rb');
head = fread(fid,5,'int');
x = fread(fid,inf,'float');
fclose(fid);
figure;
plot(x)

NFFT = 1024; spectrogram(x,hamming(NFFT),round(0.95*NFFT),NFFT,head(4));

soundsc(x(1:99225),head(4))