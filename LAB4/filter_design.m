U = 0; % Up Sample Multiplier
D = 0; % Down Sample Divisor
N = max([U,D]);
fpass = 0.9/(2*N);
fstop = 1.1/(2*N);
f1 = (fstop + fpass)/2;
f2 = (fstop - fpass)/2;
L = 100;
n = [-L:L].';
h = (1/N)*sinc(2*f1*n).*sinc(2*f2*n);
win = hamming(2*L+1);
hw = h.*win;

NFFT = 2^14;
freq = [0:NFFT-1]/NFFT;
subplot(211);
plot(freq,20*log10(abs(fft([h hw],NFFT))),'LineWidth',2);
xlabel('Frequency'); ylabel('dB');
hold on;
ax = axis();
plot(fpass*[1 1],ax(3:4),'r');
plot(1-fpass*[1 1],ax(3:4),'r');
plot(fstop*[1 1],ax(3:4),'r');
plot(1-fstop*[1 1],ax(3:4),'r');
hold off;
grid on;
ylim([-100 10]);
subplot(212);
plot(freq,abs(fft([h hw],NFFT)),'LineWidth',2);
xlabel('Frequency'); ylabel('Linear');
hold on;
ax = axis();
plot(fpass*[1 1],ax(3:4),'r');
plot(1-fpass*[1 1],ax(3:4),'r');
plot(fstop*[1 1],ax(3:4),'r');
plot(1-fstop*[1 1],ax(3:4),'r');
hold off;
grid on;

file_name = sprintf('lpf_U%d_D%d.bin',U,D);
fid = fopen(file_name,'wb');
fwrite(fid,[1 1 length(h) 1 N],'int');
fwrite(fid,h,'float');
fclose(fid);