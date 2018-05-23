[x,sample_rate] = audioread('audiofile.ext'); % change audiofile.ext to audio file
time = [0:length(x)-1]/sample_rate ; % time [seconds]

subplot(1,2,1)
plot(time,x); % plot with accurately scaled time axis
xlabel('time [seconds]','FontSize' ,18);
ylabel('amplitude','FontSize',18);
set(gca,'FontSize',16)
grid on;

subplot(1,2,2);
plot(time,x); % plot with accurately scaled time axis
% adjust the view
t1 = 1.00; % seconds
t2 = 1.01; % seconds
xlim([t1 t2]); % view from t1 to t2 seconds
xlabel('time [seconds]','FontSize',18);
ylabel('amplitude','FontSize',18);
set(gca,'FontSize',16)
grid on;

orient landscape;
print -dpdf time_plot.pdf