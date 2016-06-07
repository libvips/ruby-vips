# ruby-vips : A fast image processing extension for Ruby.

[![Build Status](https://secure.travis-ci.org/jcupitt/ruby-vips.png)](http://travis-ci.org/jcupitt/ruby-vips)

**This branch (0.3-stable) is still being maintained, but for new projects you should look at
the master branch, which use the newer vips-8 under the hood.**

This is `ruby-vips`, a gem for the 
[libvips](http://www.vips.ecs.soton.ac.uk) image processing library. 

ruby-vips is fast and it can work without needing the 
entire image to be loaded into memory. For example, the benchmark at 
[vips-benchmarks](https://github.com/stanislaw/vips-benchmarks) loads a large
image, crops, shrinks, sharpens and saves again:

```text
real time in seconds, fastest of three runs
benchmark       tiff	jpeg
ruby-vips.rb    2.77	2.98	
ruby-vips8.rb   2.97	3.29	
image-magick    8.18	9.71	
rmagick.rb      9.22	10.06	
image_sci.rb    9.39	7.20	

peak memory use in bytes
benchmark       peak RSS
ruby-vips.rb    107340
ruby-vips8.rb   117604
image_sci.rb    146536
rmagick.rb      3352020
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

## Installation prerequisites

### Ubuntu 

```bash
$ apt-get install libvips-dev
```

### OS X 

Install [homebrew](http://mxcl.github.com/homebrew) and enter:

```bash
$ brew tap homebrew/science
$ brew install vips 
```

You may see some harmless warnings. 

To verify that your vips install is working, try:

```bash
$ vips --version
vips-7.42.1-Sat Dec 27 12:01:43 GMT 2014
```

libvips has a lot of optional dependencies. You
may not need all of them.  True `brew info vips` to see what you have enabled
and what is disabled. See also [the notes
here](http://www.vips.ecs.soton.ac.uk/index.php?title=Build_on_OS_X) for
more information.

If you want to build things outside homebrew which depend on vips,
such as ruby-vips, your pkg-config will need to be working. To test
`pkg-config`, try:

```bash
$ pkg-config vips --modversion
```

If you see a version number, you are OK. If you don't, either update your
homebrew, or try adjusting `PKG_CONFIG_PATH`. At various times homebrew has
needed various settings. You might need to point it at homebrew or even at
libxml2. 

To verify that your `pkg-config` is working correctly with vips, try:

```bash
$ pkg-config vips --libs
-L/usr/local/Cellar/vips/7.42.1/lib ... a lot of stuff
```

TODO: Describe & test with macports.

### Other platforms

See [Installation on various
platforms](https://github.com/jcupitt/ruby-vips/wiki/installation-on-various-platforms).

### Installing the gem.

```bash
$ gem install ruby-vips
```

or include it in Gemfile:

```ruby
gem 'ruby-vips'
```

For a debug build:

```bash
$ gem install ruby-vips -- --enable-debug
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

# You can also read and write images from memory areas. For example:

jpeg_data = IO.read('mypic.jpg')
reader = JPEGReader.new(jpeg_data, :shrink_factor => 2, :fail_on_warn => true)
im = reader.read_buffer

# As above, the image will not be processed until the .to_memory() method 
# is called, and then will only decompress the section being processed. 
# You will need to have all of the compressed data in memory at once though. 

# Note that this means you will get a coredump if you free the compressed
# image buffer (jpeg_data above) before the write has completed.

writer = PNGWriter.new(im, :compression => 2, :interlace => false)
png_data = writer.to_memory
IO.write('out.png', png_data)

# Only JPEG, PNG and uncompressed memory images are supported at the moment,
# and png memory read is only in vips-7.34 and later.

# We hope to add other formats in future. 

png_data = IO.read('mypic.png')
reader = PNGReader.new(png_data)
im = reader.read_buffer

writer = JPEGWriter.new(im, :quality => 50)
jpeg_data = writer.to_memory
IO.write('out.jpg', jpeg_data)

```

## Why use ruby-vips?

  - It supports over 250 low-level image and color manipulation operations.
  - Operations are chainable and do not get executed until the image is sent to an output.
  - Memory use is low, even for very, very large images.
  - Fastest ruby library for resizing large images. See [benchmarks at the official libvips website](http://www.vips.ecs.soton.ac.uk/index.php?title=Speed_and_Memory_Use) and [vips-benchmarks](https://github.com/stanislaw/vips-benchmarks)
