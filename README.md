# ruby-vips : A fast image processing extension for Ruby.

Note: ruby-vips git master is the development version and uses features from
git master of libvips (the unreleased 7.29) as well. You may prefer the stable 
0.1 branch of ruby-vips.

ruby-vips is a ruby extension for [vips](http://www.vips.ecs.soton.ac.uk). 
It is extremely fast and it can process huge images without requiring the 
entire image to be loaded into memory. For example, the benchmark at 
[vips-benchmarks](https://github.com/stanislaw/vips-benchmarks) loads a large
image, crops, shrinks, sharpens and saves again:

[![Build Status](https://secure.travis-ci.org/stanislaw/ruby-vips.png?branch=master)](http://travis-ci.org/stanislaw/ruby-vips)

<pre>
real time in seconds, fastest of three runs
benchmark       tiff    jpeg
ruby-vips.rb    0.45    0.56    
rmagick.rb      1.69    1.90    
netpbm.sh       1.74    1.63    
image-magick    2.87    3.02    
image_sci.rb    3.19    2.90    

peak memory use in kilobytes
benchmark       peak RSS
ruby-vips.rb    160400
image_sci.rb    546992
rmagick.rb      1370064
</pre>

See also [benchmarks at the official libvips
website](http://www.vips.ecs.soton.ac.uk/index.php?title=Speed_and_Memory_Use).

ruby-vips allows you to set up pipelines that don't get executed until you
output the image to disk or to a string. This means you can create,
manipulate, and pass around Image objects without incurring any memory or CPU
costs. The image is not actually processed until you write the image to memory
or to disk.

## Requirements.

  * OS X or Linux
  * MRI 1.8.7, 1.9.2
  * libvips 7.29 and later (use the stable 0.1 branch of ruby-vips for earlier
    versions of libvips)

## Installation.

### Ubuntu Prerequisites.

```bash
$ apt-get install libvips-dev
```

### OS X Prerequisites.

```bash
$ brew install vips -HEAD
```

TODO: Describe & test with macports.

### Other platforms

See [Installiation on various platforms](https://github.com/jcupitt/ruby-vips/wiki/installiation-on-various-platforms).

### Installing the gem.

```bash
$ gem install ruby-vips
```

or include it in Gemfile:

```ruby
gem 'ruby-vips'
```

## Documentation.

ruby-vips has [rdoc
documentation](http://rubydoc.info/gems/ruby-vips/0.1.1/frames). Also
see [Wiki page](https://github.com/jcupitt/ruby-vips/wiki)

## Examples

```ruby
require 'rubygems'
require 'vips'

include VIPS

# Create an image object. It will not actually load the image until needed.
im = Image.jpeg('mypic.jpg')

# Shrink the jpeg by a factor of four when loading -- huge speed and CPU
# improvements on large images.
im = Image.jpeg('mypic.jpg', :shrink_factor => 4)

# Add a shrink by a factor of two to the pipeline. This will not actually be
# executed yet.
im_shrink_by_two = im.shrink(2)

# Write out the shrunk image to a PNG file. This is where the image is
# actually loaded and resized. With images that allow for random access from
# the hard drive (VIPS native format, tiled OpenEXR, ppm/pbm/pgm/pfm, tiled
# tiff, and RAW images), the entire image is never read into memory.
im_shrink_by_two.png('out.png', :interlace => true)

# All ruby-vips image commands can be chained, so the above sequence could
# be written as:
Image.jpeg('mypic.jpg', :shrink_factor => 4).shrink(2).png('out.png')

# The statement above will load the jpeg (pre-shrunk by a factor of four),
# shrink the image again by a factor of two, and then save as a png image.

# If you want to let vips determine file formats, you can use the generic
# reader and writer:
Image.new('mypic.jpg').shrink(2).write('out.png')
```

### Longer example

```ruby
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
# http://libvips.blogspot.co.uk/2012/02/sequential-mode-read.html
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
                load_shrink = 4
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

        # vips has other interpolators, eg. :nohalo ... see the output of 
        # "vips list classes" at the command-line
        #
        # :bicubic is well-known and mostly good enough
        a = a.shrink(ishrink).affinei_resize(:bicubic, rscale)

	# the convolution will break sequential access: we need to cache a few
	# scanlines
        a = a.tile_cache(a.x_size, 1, 30)

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
```

## Gotchas

### Contain memuse

ruby-vips only finalises vips images on GC. In other words:

```ruby
a = Image.new(filename)
a = nil
```

will not release the resources associated with a, you have to
either request a GC explicitly or wait for Ruby to GC for you. This can
be a problem if you're processing many images.

The growth in memory consumption is rather slow, about 200kb per iteration
for the longer example above. It's more of a problem that file descriptors
are not released until GC.

Scheduling a GC every 100 images processed would be enough.

## Why use ruby-vips?

  - It supports over 250 low-level image and color manipulation operations.
  - Operations are chainable, and do not get executed until the image is sent to an output.
  - Memory use is low, even for very, very large images.
  - Fastest ruby library for resizing large images. See [benchmarks at the official libvips website](http://www.vips.ecs.soton.ac.uk/index.php?title=Speed_and_Memory_Use) and [vips-benchmarks](https://github.com/stanislaw/vips-benchmarks)
