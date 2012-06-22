# ruby-vips : A fast image processing extension for Ruby.

note: this fork aims to track the latest stable vips. There are no plans to
add more features, just to keep it working.

ruby-vips is a ruby extension for [vips](http://www.vips.ecs.soton.ac.uk). It is
extremely fast and it can process huge images without requiring the entire image
to be loaded into memory.

ruby-vips allows you to set up pipelines that don't get executed until you
output the image to disk or to a string. This means you can create,
manipulate, and pass around Image objects without incurring any memory or CPU
costs. The image is not actually processed until you write the image to memory
or to disk.

## Requirements.

  * OS X or Linux
  * MRI 1.8.6, 1.8.7, 1.9.2 or Rubinius 1.0.1
  * libvips 7.12.x, 7.20.x, 7.22.x, or 7.23.x

## Installation.

### Ubuntu Prerequisites.

```bash
$ apt-get install libvips-dev
```

The above installs libvips 7.20.x in Ubuntu 10.04 and 7.12.x in Ubuntu 8.04.
Though ruby-vips will work with libvips 7.12.x, many features are disabled, and
there are a few caveats.

### OS X Prerequisites.

    $ brew install vips -HEAD

The above installs libvips 7.22.x

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

ruby-vips has [rdoc documentation](http://rubyvips.holymonkey.com). Also
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

# handy for leak checking, but not actually a complete shrinker. It takes 
# no account of things like colour profiles, for example
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
# with ruby-vips 0.1.1 and vips-7.28.7 I see a steady ~50mb of memuse as 
# this program runs (watching RES in top)
#
# on my laptop (2009 macbook running ubuntu 12.04):
#
# $ time ./soak.rb t_*.jpg
# real  0m47.809s
# user  0m33.626s
# sys   0m1.516s
#
# ie. about 0.5s real time to do a high-quality shrink of a 10k x 10k 
# rgb jpg

# target size we shrink to ... the image will fit inside a size x size box
size = 100

# we apply a slight sharpen to thumbnails to make then "pop" a bit
mask = [
[-1, -1,  -1],
[-1,  32, -1,],
[-1, -1,  -1]
]
m = Mask.new mask, 24, 0 

# show what we do
# verbose = true
verbose = false

ARGV.each do |filename|
  puts "loop #{filename} ..."

  # get the image size ... ,new() only decompresses pixels on access, 
  # just opening and getting properties is fast
  # 
  # this will decode small images to a memory buffer, large images to a
  # temporary disc file which is then mapped
  #
  # vips also supports sequential mode access, where the image is 
  # directly streamed from the source, through the decode library, to 
  # the destination, but ruby-vips does not yet expose this, 
  # unfortunately
  #
  # see 
  #
  #   http://libvips.blogspot.co.uk/2012/02/sequential-mode-read.html
  #
  # enabling this mode would help speed up tiff and png thumbnailing and
  # reduce disc traffic, must get around to adding it to ruby-vips
  a = Image.new(filename)

  # largest dimension
  d = [a.x_size, a.y_size].max

  shrink = d / size.to_f
  puts "shrink of #{shrink}" if verbose

  # jpeg images can be shrunk very quickly during load, by a factor of 2, 
  # 4 or 8
  #
  # if this is a jpeg, turn on shrink-on-load 
  #
  # a better file type test would be good here :-( vips has a nice one, 
  # but it's not exposed in ruby-vips yet
  if filename.end_with? ".jpg"
    if shrink >= 8
        load_shrink = 8
    elsif shrink >= 4
        load_shrink = 4
    elsif shrink >= 2
        load_shrink = 4
    end

    puts "jpeg shrink on load of #{load_shrink}" if verbose

    a = Image.jpeg(filename, :shrink_factor => load_shrink)

    # and recalculate the shrink we need, since the dimensions have changed
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

  # this will look a little "soft", apply a gentle sharpen
  a = a.conv(m)

  # finally ... write to the output
  #
  # this call will run the pipeline we have built above across all your 
  # CPUs, though for a simple pipeline like this you'll be spending 
  # most of your time in the file import / export libraries, which 
  # are generally single-threaded
  a = JPEGWriter.new(a, {:quality => 50})
  a.write('test.jpg')

  # force the GC to run and free up any memory vips is hanging on to
  #
  # without this you'll see memuse slowly climb until ruby runs the GC 
  # for you
  # 
  # something to make ruby-vips drop refs to vips objects explicitly 
  # would be nice 
  GC.start
end
```

## Gotchas

### Contain memuse
ruby-vips only finalises vips images on GC. In other words:

a = Image.new(filename)
a = nil

will not release the resources associated with the image, you have to
either request a GC explicitly, or wait for Ruby to GC for you. This can
be a problem if you're processing many images.

ruby-vips uses a simple mark system to link images together. Instead, we
should rely on vips8 to link images for us, then dropping 

... finish

## Why use ruby-vips?

  - It supports over 250 low-level image and color manipulation operations.
  - Operations are chainable, and do not get executed until the image is sent to
    an output.
  - Fastest ruby library for resizing large images. See [benchmarks at the
    official libvips website](http://www.vips.ecs.soton.ac.uk/index.php?title=Speed_and_Memory_Use).
