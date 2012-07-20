#!/usr/bin/env ruby

require 'rubygems'
require 'vips'

include VIPS

# sample image shrinker for ruby-vips

# handy for leak checking, but not actually a complete shrinker. It takes no
# account of things like colour profiles, for example
#
# see vipsthumbnail, part of the standard vips distrubution, for a complete
# shrink program:
#
# https://github.com/jcupitt/libvips/blob/master/tools/vipsthumbnail.c
#
# this demo ought to be worked up to include all that stuff

# test with:
#
# $ for i in {1..100}; do cp ~/pics/wtc.jpg t_$i.jpg; done
# $ soak.rb t_*.jpg
#
# where wtc.jpg is a 10,000 x 10,000 pixel RGB image
#
# with ruby-vips 0.1.1 and vips-7.28.7 I see a steady ~50mb of memuse as this
# program runs (watching RES in top)
#
# on my laptop (2009 macbook running ubuntu 12.04):
#
# $ time ./soak.rb t_*.jpg
# real  0m47.809s
# user  0m33.626s
# sys   0m1.516s
#
# ie. about 0.5s real time to do a high-quality shrink of a 10k x 10k rgb jpg

# target size we shrink to ... the image will fit inside a size x size box
size = 128

# we apply a slight sharpen to thumbnails to make then "pop" a bit
mask = [
    [-1, -1,  -1],
    [-1,  32, -1],
    [-1, -1,  -1]
]
m = Mask.new mask, 24, 0 

# show what we do
verbose = true
# verbose = false

# repeat everything this many times for a serious soak test
repeat = 1

# we want to open tiff, png and jpg images in sequential mode -- see
#
# http://libvips.blogspot.co.uk/2012/02/sequential-mode-read.html
#
# for more background, see also
#
# http://libvips.blogspot.co.uk/2012/06/how-libvips-opens-file.html
#
# this is very ugly! vips8 has a better way to give options to generic loaders,
# this chunk of ruby-vips should get better in the next version
#
# formats other than tif/png/jpg will be opened with the default loader -- 
# this will decompress small images to memory, large images via a temporary 
# disc file
def thumb_open(filename, shrink = 1)
    if filename.end_with? ".jpg"
        return Image.jpeg filename, 
                      :shrink_factor => shrink,
                      :sequential => true
    elsif filename.end_with? ".tif"
        return Image.tiff filename, 
                      :sequential => true
    elsif filename.end_with? ".png"
        return Image.png filename, 
                      :sequential => true
    else 
        return Image.new filename 
    end
end

repeat.times do 
    ARGV.each do |filename|
        # you can give vips argument flags to ruby-vips programs, eg.
        # --vips-progress, make sure we don't try to load those
        next if filename =~ /^-/

        puts "loop #{filename} ..."

        # get the image size ... this will only decompresses pixels on access, 
        # just opening and getting properties is fast
        a = thumb_open filename

        # largest dimension
        d = [a.x_size, a.y_size].max

        shrink = d / size.to_f
        puts "shrink of #{shrink}" if verbose

        # jpeg images can be shrunk very quickly during load by a factor of 2, 
        # 4 or 8
        #
        # if this is a jpeg, turn on shrink-on-load 
        if filename.end_with? ".jpg"
            if shrink >= 8
                load_shrink = 8
            elsif shrink >= 4
                load_shrink = 4
            elsif shrink >= 2
                load_shrink = 2
            end

            puts "jpeg shrink on load of #{load_shrink}" if verbose

            a = thumb_open filename, load_shrink

            # and recalculate the shrink we need, since the dimensions have 
            # changed
            d = [a.x_size, a.y_size].max
            shrink = d / size.to_f
        end

        # we shrink in two stages: we use a box filter (each pixel in output 
        # is the average of a m x n box of pixels in the input) to shrink by 
        # the largest integer factor we can, then an affine transform to get 
        # down to the exact size we need
        #
        # if you just shrink with the affine, you'll get bad aliasing for large
        # shrink factors (more than x2)

        ishrink = shrink.to_i

        # size after int shrink
        id = (d / ishrink).to_i

        # therefore residual float scale (note! not shrink)
        rscale = size.to_f / id

        puts "block shrink by #{ishrink}" if verbose
        puts "residual scale by #{rscale}" if verbose

        a = a.shrink(ishrink)

        # the convolution will break sequential access: we need to cache a few
        # scanlines
        a = a.tile_cache(a.x_size, 1, 30)

        # vips has other interpolators, eg. :nohalo ... see the output of 
        # "vips list classes" at the command-line
        #
        # :bicubic is well-known and mostly good enough
        a = a.affinei_resize(:bicubic, rscale)

        # this will look a little "soft", apply a gentle sharpen
        a = a.conv(m)

        # finally ... write to the output
        #
        # this call will run the pipeline we have built above across all your 
        # CPUs, though for a simple pipeline like this you'll be spending 
        # most of your time in the file import / export libraries, which are 
        # generally single-threaded
        a = JPEGWriter.new(a, {:quality => 50})

        puts "starting write test.jpg ..."
        a.write('test.jpg')
    end
end
