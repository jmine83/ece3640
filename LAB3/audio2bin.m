function [x,fs] = audio2bin(infile,outfile)
if(nargin == 1)
    outfile = [infile(1:max(strfind(infile,'.'))),'bin'];
end
fprintf('Input  file = %s\n',infile);
fprintf('Output file = %s\n',outfile);

[x,fs]=audioread(infile);

fid = fopen(outfile,'wb'); % write binary
fwrite(fid,[1 size(x,2) size(x,1) fs 0],'int'); % audio header
x = x.'; % transpose
fwrite(fid,x(:),'float'); % vectorize then write data
fclose(fid);

return;