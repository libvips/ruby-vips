## ruby-vips : A fast image processing extension for Ruby.

ruby-vips is a ruby extension for vips (http://www.vips.ecs.soton.ac.uk). It is
extremely fast and it can process huge images without requiring the entire image
to be loaded into memory.

ruby-vips allows you to set up pipelines that don't get executed until you
output the image to disk or to a string. This means you can create,
manipulate, and pass around Image objects without incurring any memory or CPU
costs. The image is not actually processed until you call e.g. Image#write.

## Requirements

  * OS X or Linux
  * MRI 1.8.6, 1.8.7, 1.9.2 or Rubinius 1.0.1
  * libvips 7.12.x, 7.20.x, 7.22.x, or 7.23.x

## Installation.

### Ubuntu Prerequisites.

    $ apt-get install libvips-dev

The above installs libvips 7.20.x in Ubuntu 10.04 and 7.12.x in Ubuntu 8.04.
Though ruby-vips will work with libvips 7.12.x, many features are disabled, and
there are a few caveats.

TODO: I hope to provide updated debs. Describe issues with 7.12.x.

### OS X Prerequisites.

    $ brew install vips -HEAD

The above installs libvips 7.22.x

TODO: Describe & test with macports.

### Installing the gem.

gem install ruby-vips

## Example.

    require 'rubygems'
    require 'vips'

    # Create an image object. It will not actually load the image until needed.
    im = VIPS::Image.new('mypic.jpg')

    # Shrink the jpeg by a factor of three when loading -- huge speed and CPU
    # improvements on large images.
    im_shrink_on_load = VIPS::Image.new('mypic.jpg:4')

    # Add a shrink by a factor of two to the pipeline. This will not actually be
    # executed yet.
    im_shrink_by_two = im.shrink(2, 2)

    # Write out the shrunk image to a PNG file. This is where the image is
    # actually loaded and resized. With images that allow for random access from
    # the hard drive (VIPS native format and certain TIFF images) the entire
    # image does not even have to be read into memory.
    im_shrink_by_two.to_png.write('out.png')

## Why use ruby-vips?

  - It supports over 250 low-level image and color manipulation operations.
  - Operations are chainable, and do not get executed until the image is sent to
    an output.
  - Fastest ruby library for resizing images (TODO: prove it).

