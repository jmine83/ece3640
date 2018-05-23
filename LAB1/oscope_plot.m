[x,fs] = audioread('audiofile.ext'); % change audiofile.ext to audio file
t = [0:length(x)-1]/fs; % time [seconds]
%plot(t,x); % this plots whole waveform, gives big picture, but cannot see detail

win_sec = 0.01; % window length [seconds]
win_sam = round(win_sec*fs); % window length [samples]
step_sec = 0.0002; % step length [seconds]
step_sam = round(step_sec*fs); % step length [samples]

han = plot(t(1:win_sam),x(1:win_sam)); drawnow;
ylim(0.1*[-1 1]);
for i=win_sam:step_sam:length (x)
ind = [i-win_sam+1:i];
set(han,'XData',t(ind),'YData',x(ind));
xlim(t(ind([1,end]))); drawnow ;
% pause; % if this runs too fast, include a pause
end