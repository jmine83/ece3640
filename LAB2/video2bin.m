function x = video2bin(infile,outfile)
if(nargin == 1)
    outfile = [infile(1:max(strfind(infile,'.'))),'bin'];
end
fprintf('Input  file = %s\n',infile);
fprintf('Output file = %s\n',outfile);

obj=VideoReader(infile,'tag','myreader1');
x = read(obj); % read in all the frames

fid = fopen(outfile,'wb'); % write binary
fwrite(fid,[3 size(x,3) size(x,1) size(x,2) size(x,4)],'int'); % video header
nframes = obj.NumberOfFrames;
for i=1:nframes
    y = read(obj,i); % read video frame
    y = permute(y,[2 1 3]); % permutate
    y = reshape(y,[size(y,1)*size(y,2) size(y,3)]).'; % reshape & transpose
    fwrite(fid,y(:),'float'); % vectorize then write data
end
fclose(fid);

return;