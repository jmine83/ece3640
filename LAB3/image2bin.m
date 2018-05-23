function x = image2bin(infile,outfile)
if(nargin == 1)
    outfile = [infile(1:max(strfind(infile,'.'))),'bin'];
end
fprintf('Input  file = %s\n',infile);
fprintf('Output file = %s\n',outfile);

x=imread(infile);

fid = fopen(outfile,'wb'); % write binary
fwrite(fid,[2 size(x,3) size(x,1) size(x,2) 0],'int'); % image header
x = permute(x,[2 1 3]); % permutate
x = reshape(x,[size(x,1)*size(x,2) size(x,3)]).'; % reshape & transpose
fwrite(fid,x(:),'float'); % vectorize then write data
fclose(fid);

return;