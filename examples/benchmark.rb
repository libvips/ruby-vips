#!/usr/bin/ruby

# this is the benchmark program from
#
# https://github.com/stanislaw/vips-benchmarks
#
# it loads an image, crops 100 pixels off every edge, shrinks by 10%, sharpens
# and saves again

require 'rubygems'
require 'vips'

include VIPS

im = Image.new(ARGV[0])

im = im.extract_area(100, 100, im.x_size - 200, im.y_size - 200)
im = im.affinei_resize(:bilinear, 0.9)
mask = [
    [-1, -1,  -1],
    [-1,  16, -1,],
    [-1, -1,  -1]
]
m = Mask.new mask, 8, 0 
im = im.conv(m)

im.write(ARGV[1])
