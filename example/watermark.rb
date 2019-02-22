#!/usr/bin/ruby

require 'vips'

im = Vips::Image.new_from_file ARGV[0], access: :sequential

# make the text overlay mask
text = Vips::Image.text ARGV[2], width: 500, dpi: 300, font: 'sans bold'
text = text.rotate(-45)
# make the text transparent
text = (text * 0.3).cast(:uchar)
text = text.gravity :centre, 200, 200
text = text.replicate 1 + im.width / text.width, 1 + im.height / text.height
text = text.crop 0, 0, im.width, im.height

# we blend in RGB colourspace, so no CMYK etc.
im = im.colourspace :srgb

# we want to split the image into alpha and non-alpha so we can blend the
# text into the main part of the image and leave any alpha untouched
if im.has_alpha?
  alpha = im[im.bands - 1]
  im = im[0...im.bands - 1]
else
  alpha = nil
end

im = text.ifthenelse [255, 128, 128], im, blend: true

# reattach alpha
im = im.bandjoin alpha if alpha

im.write_to_file ARGV[1]
