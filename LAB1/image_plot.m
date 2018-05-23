infile = 'imagefile.ext'; % change imagefile.ext to image file
x = imread(infile);
imagesc(x); % show scaled image
axis image; % make the pixels square
title(infile);
colormap jet; % gray = grayscale image, jet = color image
colorbar; % show the colorbar
print -dpng image_plot.png