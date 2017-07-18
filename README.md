# ruby-vips

[![Gem Version](https://badge.fury.io/rb/ruby-vips.svg)](https://badge.fury.io/rb/ruby-vips)
[![Build Status](https://travis-ci.org/jcupitt/ruby-vips.svg?branch=master)](https://travis-ci.org/jcupitt/ruby-vips)

This gem provides a Ruby binding for the [vips image processing
library](https://jcupitt.github.io/libvips).

Programs that use `ruby-vips` don't
manipulate images directly, instead they create pipelines of image processing
operations building on a source image. When the end of the pipe is connected
to a destination, the whole pipeline executes at once, streaming the image
in parallel from source to destination a section at a time. 

Because `ruby-vips` is parallel, it's quick, and because it doesn't need to
keep entire images in memory, it's light.  For example, the benchmark at
[vips-benchmarks](https://github.com/stanislaw/vips-benchmarks) loads a
large image, crops, shrinks, sharpens and saves again, and repeats 10 times.

```text
real time in seconds, fastest of three runs
benchmark       tiff    jpeg
ruby-vips.rb    0.66    0.44
image-magick    1.10    1.50
rmagick.rb      1.63    2.16

peak memory use in bytes
benchmark       peak RSS
ruby-vips.rb    58696
rmagick.rb      787564
```

See also [benchmarks at the official libvips
website](https://github.com/jcupitt/libvips/wiki/Speed-and-memory-use).
There's a handy blog post explaining [how libvips opens
files](http://libvips.blogspot.co.uk/2012/06/how-libvips-opens-file.html)
which gives some more background.

## Requirements

  * OS X and Linux tested, Windows should work
  * libvips 8.2 or later
  * [ruby-ffi](https://github.com/ffi/ffi) 1.9 or later
  * Ruby or JRuby

## Installation prerequisites

### OS X 

Install [homebrew](https://brew.sh/) and enter:

```bash
$ brew install vips
```

To verify that your vips install is working, try:

```bash
$ vips --version
vips-8.2.1
```

### Other platforms

Check your package manager and see if the libvips on your system is new enough.
`ruby-vips` needs libvips 8.2 or later. 

If it's too old, you'll need to build libvips from source. 
Download a tarball from the 
[libvips website](https://jcupitt.github.io/libvips), or build
from [the git repository](https://github.com/jcupitt/libvips) and see the
README.

## Installing the gem.

```bash
$ gem install ruby-vips
```

or include it in Gemfile:

```ruby
gem 'ruby-vips'
```

And take a look in `examples/`. There is full yard documentation, take a look
there too.

# Example

```ruby
require 'vips'

im = Vips::Image.new_from_file filename

# put im at position (100, 100) in a 3000 x 3000 pixel image, 
# make the other pixels in the image by mirroring im up / down / 
# left / right, see
# https://jcupitt.github.io/libvips/API/current/libvips-conversion.html#vips-embed
im = im.embed 100, 100, 3000, 3000, :extend => :mirror

# multiply the green (middle) band by 2, leave the other two alone
im *= [1, 2, 1]

# make an image from an array constant, convolve with it
mask = Vips::Image.new_from_array [
    [-1, -1, -1],
    [-1, 16, -1],
    [-1, -1, -1]], 8
im = im.conv mask

# finally, write the result back to a file on disk
im.write_to_file output_filename
```

# Older versions

There are two older versions of this gem.

The `0.3-stable` branch is written in C, and supports a different API. It still
works, but is only supported for compatibility.

The `1.0-stable` branch is based on `gobject-introspection`. It supports the
same API as the current version, but is harder to install, slower, and less
stable. 

# Why the API break?

There's been a `ruby-vips` for a few years now. 
It was written by a Ruby
expert, it works well, it includes a test-suite, and has pretty full
documentation. Why rewrite?

`ruby-vips` 0.3 was based on the old vips7 API. There's now vips8, 
which adds several
very useful new features:

* [GObject](https://developer.gnome.org/gobject/stable/)-based API with full
  introspection. You can discover the vips8 API at runtime. This means that if
  libvips gets a new operator, any binding that goes via vips8 will 
  get the new thing immediately. With vips7, whenever libvips was changed, all
  the bindings needed to be changed too.

* vips7 probably won't get new features. vips7 doesn't really exist any more:
  the API is still there, but now just a thin compatibility layer over vips8.
  New features may well not get added to the vips7 API.

* No C required. Thanks to
  [ffi](https://github.com/ffi/ffi)
  the whole binding is now written in Ruby. This
  makes it a lot smaller and more portable. 

There are some more minor pluses as well:

* Named and optional arguments. vips8 lets you have optional and required
  arguments, both input and output, and optional arguments can have default
  values. 

* Operation cache. vips8 keeps track of the last 1,000 or so operations and
  will automatically reuse results when it can. This can give a huge speedup
  in some cases.

* vips8 is much simpler and more regular. For example, 
  ruby-vips had to work hard to offer a nice loader system, but that's all
  built into vips8. It can do things like load and save formatted images to 
  and from memory buffers as well, which just wasn't possible before. 

This binding adds some extra useful features over the old `ruby-vips` binding.

* Full set of arithmetic operator overloads.

* Automatic constant expansion. You can write things like
  `image.bandjoin(255)` and the 255 will be automatically expanded to an image 
  and attached as an extra band. You can mix int, float, scalar, vector and
  image constants freely.

# How to update your code

* `VIPS::` becomes `Vips::`

* `VIPS::Image.new(filename)` becomes `Vips::Image.new_from_file(filename)`. 
  `VIPS::Image.jpeg(filename)` also becomes 
  `Vips::Image.new_from_file(filename)`, similarly for other formats.  

* `#write(filename)` becomes `#write_to_file(filename)`. `#png(filename)` also
  becomes `#write_to_file(filename)`, same for all other
  formats.

* Most member functions are unchanged, but check the yard docs. You can also
  use the C docs directly, since `ruby-vips` is now a very thin layer over the
  C API. See the docs for the `Vips` class for guidance. 

* There are lots of nice new features, see the docs and examples.
