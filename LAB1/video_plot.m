% load the frames of a video one at a time and display each one as an image

obj = VideoReader('imagefile.ext','tag','myreader1');
% change imagefile.ext to video file

z = read(obj); % color video
sz = size(z)
r = round(sz(1)/2);
y = z(r,:,:,:);
sy = size(y);
x = squeeze(y); % squeeze removes singleton dimensions
sx = size(x);
w = permute(x,[1 3 2]); % permutes the dimensions
    % need RGB in 3rd dimension of image (see also ipermute)
sw = size(w);
imagesc(w); axis image;

nframes = obj.NumberOfFrames ;
for i=1:nframes
    yi = read(obj,i); % read the next video frame
    if(i==1)
        imhan = image(yi); axis image; drawnow;
    else
        set(imhan,'CData',yi); drawnow ;
    end
end