fid = fopen('lpf_260_400_44100_80db.bin','rb');
head = fread(fid,5,'int'); % Read in header
h = fread(fid,inf,'float'); % Read in impulse response
fclose(fid);
stem(h); % Take a look to make sure we pulled in the right stuff

% Now make magnitude response plot
N = 2^14; % FFT size
f = [0:N-1]*44100/N; % Make a frequency vector for plotting
H = abs(fft(h,N)).^2; % Compute the magnitude response

figure(1);
plot(f,10*log10(H));
grid on;
xlim([0 44100/2]);
ylim([-100 10]);
xlabel('frequency [Hz]');

figure(2);
semilogx(f,10*log10(H));
grid on;
xlim([10 44100/2]);
ylim([-100 10]);
xlabel('log(frequency) [Hz]');