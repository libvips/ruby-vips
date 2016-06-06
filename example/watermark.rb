#!/usr/bin/ruby

require 'vips'
 
im = Vips::Image.new_from_file ARGV[0], :access => :sequential
 
text = Vips::Image.text ARGV[2], :width => 500, :dpi => 300
text = (text * 0.3).cast(:uchar)
text = text.embed 100, 100, text.width + 200, text.width + 200
text = text.replicate 1 + im.width / text.width, 1 + im.height / text.height
text = text.crop 0, 0, im.width, im.height

# we want to blend into the visible part of the image and leave any alpha
# channels untouched ... we need to split im into two parts

# guess how many bands from the start of im contain visible colour information
if im.bands >= 4 and im.interpretation == :cmyk
    # cmyk image
    n_visible_bands = 4
    text_colour = [0, 255, 0, 0]
elsif im.bands >= 3
    # rgb image
    n_visible_bands = 3
    text_colour = [255, 0, 0]
else
    # mono image
    n_visible_bands = 1
    text_colour = 255
end

# split into image and alpha
if im.bands - n_visible_bands > 0
    alpha = im.extract_band n_visible_bands, :n => im.bands - n_visible_bands
    im = im.extract_band 0, :n => n_visible_bands
else
    alpha = nil
end

marked = text.ifthenelse text_colour, im, :blend => true

# reattach alpha
marked = marked.bandjoin alpha  if alpha
 
marked.write_to_file ARGV[1]
