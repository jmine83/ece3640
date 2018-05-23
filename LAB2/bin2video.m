function x = bin2video(infile,outfile)
if(nargin == 1)
	outfile = [infile(1:max(strfind(infile,'.'))),'mp4'];
end
fprintf('Input  file = %s\n',infile);
fprintf('Output file = %s\n',outfile);

fid = fopen(infile,'rb'); % read binary
ndim  = fread(fid,1,'int'); % read number of dimensions
nchan = fread(fid,1,'int'); % read number of channels
dim0  = fread(fid,1,'int'); % read first dimension
dim1  = fread(fid,1,'int'); % read second dimension
dim2  = fread(fid,1,'int'); % read third dimension
[x,cnt]=fread(fid,inf,'float'); % read data
fclose(fid);

obj = VideoWriter(outfile,'MPEG-4'); % create video file
open(obj); % open video file
j = 1; % beginning of frame
k = dim0*dim1*nchan-1; % data length of frame
for i=1:dim2
    y = x(j:j+k); % isolate image from video
    if(nchan == 1) % grayscale image
        y = reshape(y,[dim1 dim0]).'; % reshape image
        y = double(y)/255; % convert to K percentage
    end
    if(nchan == 3) % color image
        R = y(1:3:end); G = y(2:3:end); B = y(3:3:end); % isolate channels
        R = reshape(R,[dim1 dim0]).'; % reshape RED channel
        G = reshape(G,[dim1 dim0]).'; % reshape GREEN channel
        B = reshape(B,[dim1 dim0]).'; % reshape BLUE channel
        y = cat(3,R,G,B); % concatenate along the third dimension
        y = double(y)/255; % convert to RGB percentages
    end
    writeVideo(obj,y); % write the video frame
    j = j+k+1; % move to next frame
end
close(obj); % close video file

return;