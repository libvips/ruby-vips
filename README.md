# ruby-vips : A fast image processing extension for Ruby.

[![Build Status](https://secure.travis-ci.org/jcupitt/ruby-vips.png)](http://travis-ci.org/jcupitt/ruby-vips)

ruby-vips is a ruby extension for [vips](http://www.vips.ecs.soton.ac.uk). 
It is fast and it can process images without requiring the 
entire image to be loaded into memory. For example, the benchmark at 
[vips-benchmarks](https://github.com/stanislaw/vips-benchmarks) loads a large
image, crops, shrinks, sharpens and saves again:

```text
real time in seconds, fastest of three runs
benchmark       tiff    jpeg
ruby-vips.rb    0.45    0.56    
rmagick.rb      1.69    1.90    
netpbm.sh       1.74    1.63    
image-magick.sh 2.87    3.02    
image_sci.rb    3.19    2.90    

peak memory use in kilobytes
benchmark       peak RSS
ruby-vips.rb    160400
image_sci.rb    546992
rmagick.rb      1370064
```

See also [benchmarks at the official libvips
website](http://www.vips.ecs.soton.ac.uk/index.php?title=Speed_and_Memory_Use).
There's a handy blog post explaining [how libvips opens
files](http://libvips.blogspot.co.uk/2012/06/how-libvips-opens-file.html)
which gives some more background.

ruby-vips allows you to set up pipelines that don't get executed until you
output the image to disk or to a string. This means you can create,
manipulate, and pass around Image objects without incurring any memory or CPU
costs. The image is not actually processed until you write the image to memory
or to disk.

*note*: ruby-vips will work with versions of libvips as far back as 7.12, but
with missing features and with reduced performance. For best results, use the 
latest libvips you can.

## Requirements.

  * OS X or Linux
  * MRI 1.8.7, 1.9.3
  * libvips 7.24 and later (it will work with earlier libvips, but some
    features may not be functional)

## Installation.

### Ubuntu Prerequisites.

```bash
$ apt-get install libvips-dev
```

### OS X Prerequisites.

```bash
$ brew tap homebrew/science
$ brew install --use-llvm vips
```

You may need to set your PKG_CONFIG_PATH before vips can see your libpng. Try
something like:

```bash
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/usr/lib/pkgconfig:/usr/X11/lib/pkgconfig
```

See the Mac OS X Lion section in [Installation on various
platforms](https://github.com/jcupitt/ruby-vips/wiki/installation-on-various-platforms).

TODO: Describe & test with macports.

### Other platforms

See [Installation on various
platforms](https://github.com/jcupitt/ruby-vips/wiki/installation-on-various-platforms).

### Installing the gem.

```bash
$ gem install ruby-vips
```

Alternatively, for a debug build:

```bash
$ gem install ruby-vips -- --enable-debug
```

or include it in Gemfile:

```ruby
gem 'ruby-vips'
```

## Documentation.

ruby-vips has [rdoc
documentation](http://rubydoc.info/gems/ruby-vips/frames). Also
see [Wiki page](https://github.com/jcupitt/ruby-vips/wiki)

## Small example

See also the
[examples](https://github.com/jcupitt/ruby-vips/tree/master/examples)
directory.

```ruby
require 'rubygems'
require 'vips'

include VIPS

# Create an image object. It will not actually load the pixel data until 
# needed. 
im = Image.jpeg('mypic.jpg')

# You can read all the header fields without triggering a pixel load.
puts "it's #{im.x_size} pixels across!"

# Shrink the jpeg by a factor of four when loading -- huge speed and CPU
# improvements on large images.
im = Image.jpeg('mypic.jpg', :shrink_factor => 4)
puts "but only #{im.x_size} pixels when we shrink on load"

# Add a shrink by a factor of two to the pipeline. This will not actually be
# executed yet.
im_shrink_by_two = im.shrink(2)

# Write out the shrunk image to a PNG file. This is where the image is
# actually loaded and resized. With images that allow for random access from
# the hard drive (VIPS native format, tiled OpenEXR, ppm/pbm/pgm/pfm, tiled
# tiff, and RAW images), the entire image is never read into memory.
# For other formats, the image is either decompressed to a temporary disc 
# file and then processed from there, or, if you give the :sequential hint, 
# streamed directly from the file.
im_shrink_by_two.png('out.png', :interlace => true)

# All ruby-vips image commands can be chained, so the above sequence could
# be written as:
Image.jpeg('mypic.jpg', :shrink_factor => 4).shrink(2).png('out.png')

# You hint sequential mode in the loader, so this will stream directly from
# the source image:
Image.jpeg('large.png', :sequential => true).shrink(2).png('out.png')

# The statement above will load the jpeg (pre-shrunk by a factor of four),
# shrink the image again by a factor of two, and then save as a png image.

# If you want to let vips determine file formats, you can use the generic
# reader and writer:
Image.new('mypic.jpg').shrink(2).write('out.png')
```

## Why use ruby-vips?

  - It supports over 250 low-level image and color manipulation operations.
  - Operations are chainable and do not get executed until the image is sent to an output.
  - Memory use is low, even for very, very large images.
  - Fastest ruby library for resizing large images. See [benchmarks at the official libvips website](http://www.vips.ecs.soton.ac.uk/index.php?title=Speed_and_Memory_Use) and [vips-benchmarks](https://github.com/stanislaw/vips-benchmarks)
