function [x,fs] = bin2audio(infile,outfile)
if(nargin == 1)
	outfile = [infile(1:max(strfind(infile,'.'))),'wav'];
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

x = reshape(x,nchan,dim0).'; % reshape audio
fs = dim1;

audiowrite(outfile,x,fs);

return;