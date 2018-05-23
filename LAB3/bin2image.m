function x = bin2image(infile,outfile)
if(nargin == 1)
	outfile = [infile(1:max(strfind(infile,'.'))),'png'];
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

if(nchan == 1) % grayscale image
    x = reshape(x,[dim1 dim0]).'; % reshape image
    x = double(x)/255; % convert to K percentage
end
if(nchan == 3) % color image
    R = x(1:3:end); G = x(2:3:end); B = x(3:3:end); % isolate channels
    R = reshape(R,[dim1 dim0]).'; % reshape RED channel
    G = reshape(G,[dim1 dim0]).'; % reshape GREEN channel
    B = reshape(B,[dim1 dim0]).'; % reshape BLUE channel
    x = cat(3,R,G,B); % concatenate along the third dimension
    x = double(x)/255; % convert to RGB percentages
end

imwrite(x,outfile,'PNG');

return;