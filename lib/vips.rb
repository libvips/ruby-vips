# This module provides an interface to the vips image processing library
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

# This module uses FFI to make a simple layer over the glib and gobject 
# libraries. 

module GLib
    extend FFI::Library
    ffi_lib 'gobject-2.0'

    # nil being the default
    glib_log_domain = nil

    def self.set_log_domain domain
        glib_log_domain = domain
    end

    attach_function :g_malloc, [:size_t], :pointer

    # save the FFI::Function that attach will return ... we can use it directly
    # as a param for callbacks
    G_FREE = attach_function :g_free, [:pointer], :void

    # :gtype will usually be 64-bit, but will be 32-bit on 32-bit Windows
    typedef :ulong, :GType

    attach_function :g_type_name, [:GType], :string
    attach_function :g_type_from_name, [:string], :GType
    attach_function :g_type_fundamental, [:GType], :GType

    # look up some common gtypes
    GBOOL_TYPE = g_type_from_name "gboolean"
    GINT_TYPE = g_type_from_name "gint"
    GDOUBLE_TYPE = g_type_from_name "gdouble"
    GENUM_TYPE = g_type_from_name "GEnum"
    GFLAGS_TYPE = g_type_from_name "GFlags"
    GSTR_TYPE = g_type_from_name "gchararray"
    GOBJECT_TYPE = g_type_from_name "GObject"

end

require 'vips/gobject'
require 'vips/gvalue'

# This module provides a binding for the [vips image processing 
# library](https://jcupitt.github.io/libvips/).
#
# It needs vips-8.2 or later to be installed. 
#
# # Example
#
# ```ruby
# require 'vips'
#
# if ARGV.length < 2
#     raise "usage: #{$PROGRAM_NAME}: input-file output-file"
# end
#
# im = Vips::Image.new_from_file ARGV[0], :access => :sequential
#
# im *= [1, 2, 1]
#
# mask = Vips::Image.new_from_array [
#         [-1, -1, -1],
#         [-1, 16, -1],
#         [-1, -1, -1]], 8
# im = im.conv mask
#
# im.write_to_file ARGV[1]
# ```
#
# This example loads a file, boosts the green channel (I'm not sure why), 
# sharpens the image, and saves it back to disc again. 
#
# Reading this example line by line, we have:
#
# ```ruby
# im = Vips::Image.new_from_file ARGV[0], :access => :sequential
# ```
#
# {Image.new_from_file} can load any image file supported by vips. In this
# example, we will be accessing pixels top-to-bottom as we sweep through the
# image reading and writing, so `:sequential` access mode is best for us. The
# default mode is `:random`, this allows for full random access to image pixels,
# but is slower and needs more memory. See {Access}
# for full details
# on the various modes available. You can also load formatted images from 
# memory buffers, create images that wrap C-style memory arrays, or make images
# from constants.
#
# The next line:
#
# ```ruby
# im *= [1, 2, 1]
# ```
#
# Multiplying the image by an array constant uses one array element for each
# image band. This line assumes that the input image has three bands and will
# double the middle band. For RGB images, that's doubling green.
#
# Next we have:
#
# ```ruby
# mask = Vips::Image.new_from_array [
#         [-1, -1, -1],
#         [-1, 16, -1],
#         [-1, -1, -1]], 8
# im = im.conv mask
# ```
#
# {Image.new_from_array} creates an image from an array constant. The 8 at
# the end sets the scale: the amount to divide the image by after 
# integer convolution. See the libvips API docs for `vips_conv()` (the operation
# invoked by {Image#conv}) for details on the convolution operator. 
#
# Finally:
#
# ```ruby
# im.write_to_file ARGV[1]
# ```
#
# {Image#write_to_file} writes an image back to the filesystem. It can 
# write any format supported by vips: the file type is set from the filename 
# suffix. You can also write formatted images to memory buffers, or dump 
# image data to a raw memory array. 
#
# # How it works
#
# The binding uses [ruby-ffi](https://github.com/ffi/ffi) to open the libvips
# shared library. When you call a method on the image class, it searches the
# library for an operation of that name, transforms the arguments to a form
# libvips can digest, and runs the operation. 
#
# This means ruby-vips always presents the API implemented by the libvips shared
# library. It should update itself as new features are added. 
#
# # Automatic wrapping
#
# `ruby-vips` adds a {Image.method_missing} handler to {Image} and uses
# it to look up vips operations. For example, the libvips operation `add`, which
# appears in C as `vips_add()`, appears in Ruby as {Image#add}. 
#
# The operation's list of required arguments is searched and the first input 
# image is set to the value of `self`. Operations which do not take an input 
# image, such as {Image.black}, appear as class methods. The remainder of
# the arguments you supply in the function call are used to set the other
# required input arguments. If the final supplied argument is a hash, it is used
# to set any optional input arguments. The result is the required output 
# argument if there is only one result, or an array of values if the operation
# produces several results. If the operation has optional output objects, they
# are returned as a final hash.
#
# For example, {Image#min}, the vips operation that searches an image for 
# the minimum value, has a large number of optional arguments. You can use it to
# find the minimum value like this:
#
# ```ruby
# min_value = image.min
# ```
#
# You can ask it to return the position of the minimum with `:x` and `:y`.
#   
# ```ruby
# min_value, opts = min :x => true, :y => true
# x_pos = opts['x']
# y_pos = opts['y']
# ```
#
# Now `x_pos` and `y_pos` will have the coordinates of the minimum value. 
# There's actually a convenience method for this, {Image#minpos}.
#
# You can also ask for the top *n* minimum, for example:
#
# ```ruby
# min_value, opts = min :size => 10, :x_array => true, :y_array => true
# x_pos = opts['x_array']
# y_pos = opts['y_array']
# ```
#
# Now `x_pos` and `y_pos` will be 10-element arrays. 
#
# Because operations are member functions and return the result image, you can
# chain them. For example, you can write:
#
# ```ruby
# result_image = image.real.cos
# ```
#
# to calculate the cosine of the real part of a complex image. 
# There are also a full set
# of arithmetic operator overloads, see below.
#
# libvips types are also automatically wrapped. The override looks at the type 
# of argument required by the operation and converts the value you supply, 
# when it can. For example, {Image#linear} takes a `VipsArrayDouble` as 
# an argument 
# for the set of constants to use for multiplication. You can supply this 
# value as an integer, a float, or some kind of compound object and it 
# will be converted for you. You can write:
#
# ```ruby
# result_image = image.linear 1, 3 
# result_image = image.linear 12.4, 13.9 
# result_image = image.linear [1, 2, 3], [4, 5, 6] 
# result_image = image.linear 1, [4, 5, 6] 
# ```
#
# And so on. A set of overloads are defined for {Image#linear}, see below.
#
# It does a couple of more ambitious conversions. It will automatically convert
# to and from the various vips types, like `VipsBlob` and `VipsArrayImage`. For
# example, you can read the ICC profile out of an image like this: 
#
# ```ruby
# profile = im.get_value "icc-profile-data"
# ```
#
# and profile will be a byte array.
#
# If an operation takes several input images, you can use a constant for all but
# one of them and the wrapper will expand the constant to an image for you. For
# example, {Image#ifthenelse} uses a condition image to pick pixels 
# between a then and an else image:
#
# ```ruby
# result_image = condition_image.ifthenelse then_image, else_image
# ```
#
# You can use a constant instead of either the then or the else parts and it
# will be expanded to an image for you. If you use a constant for both then and
# else, it will be expanded to match the condition image. For example:
#
# ```ruby
# result_image = condition_image.ifthenelse [0, 255, 0], [255, 0, 0]
# ```
#
# Will make an image where true pixels are green and false pixels are red.
#
# This is useful for {Image#bandjoin}, the thing to join two or more 
# images up bandwise. You can write:
#
# ```ruby
# rgba = rgb.bandjoin 255
# ```
#
# to append a constant 255 band to an image, perhaps to add an alpha channel. Of
# course you can also write:
#
# ```ruby
# result_image = image1.bandjoin image2
# result_image = image1.bandjoin [image2, image3]
# result_image = Vips::Image.bandjoin [image1, image2, image3]
# result_image = image1.bandjoin [image2, 255]
# ```
#
# and so on. 
# 
# # Automatic YARD documentation
#
# The bulk of these API docs are generated automatically by 
# {Vips::generate_yard}. It examines
# libvips and writes a summary of each operation and the arguments and options
# that that operation expects. 
# 
# Use the [C API 
# docs](https://jcupitt.github.io/libvips/API/current) 
# for more detail.
#
# # Exceptions
#
# The wrapper spots errors from vips operations and raises the {Vips::Error}
# exception. You can catch it in the usual way. 
#
# # Enums
#
# The libvips enums, such as `VipsBandFormat` appear in ruby-vips as Symbols
# like `:uchar`. They are documented as a set of classes for convenience, see
# the class list. 
# 
# # Draw operations
#
# Paint operations like {Image#draw_circle} and {Image#draw_line}
# modify their input image. This
# makes them hard to use with the rest of libvips: you need to be very careful
# about the order in which operations execute or you can get nasty crashes.
#
# The wrapper spots operations of this type and makes a private copy of the
# image in memory before calling the operation. This stops crashes, but it does
# make it inefficient. If you draw 100 lines on an image, for example, you'll
# copy the image 100 times. The wrapper does make sure that memory is recycled
# where possible, so you won't have 100 copies in memory. 
#
# If you want to avoid the copies, you'll need to call drawing operations
# yourself.
#
# # Overloads
#
# The wrapper defines the usual set of arithmetic, boolean and relational
# overloads on image. You can mix images, constants and lists of constants
# (almost) freely. For example, you can write:
#
# ```ruby
# result_image = ((image * [1, 2, 3]).abs < 128) | 4
# ```
#
# # Expansions
#
# Some vips operators take an enum to select an action, for example 
# {Image#math} can be used to calculate sine of every pixel like this:
#
# ```ruby
# result_image = image.math :sin
# ```
#
# This is annoying, so the wrapper expands all these enums into separate members
# named after the enum. So you can write:
#
# ```ruby
# result_image = image.sin
# ```
#
# # Convenience functions
#
# The wrapper defines a few extra useful utility functions: 
# {Image#get_value}, {Image#set_value}, {Image#bandsplit}, 
# {Image#maxpos}, {Image#minpos}, 
# {Image#median}.

module Vips
    extend FFI::Library
    ffi_lib 'vips'

    LOG_DOMAIN = "VIPS"
    GLib::set_log_domain LOG_DOMAIN

    @@debug = false

    # Turn debug logging on and off.
    #
    # @param dbg [Boolean] Set true to print debug log messages
    def self.set_debug dbg 
        @@debug = dbg
    end

    # Print a log message to the output. See {Vips::set_debug}.
    def self.log str 
        if @@debug
            puts str
        end
    end

    typedef :ulong, :GType

    attach_function :vips_error_buffer, [], :string
    attach_function :vips_error_clear, [], :void

    # The ruby-vips error class. 
    class Error < RuntimeError
        # @param msg [String] The error message. If this is not supplied, grab
        #   and clear the vips error buffer and use that. 
        def initialize msg = nil
            if msg
                @details = msg
            elsif Vips::vips_error_buffer != ""
                @details = Vips::vips_error_buffer
                Vips::vips_error_clear
            else 
                @details = nil
            end
        end

        # Pretty-print a {Vips::Error}.
        #
        # @return [String] The error message
        def to_s
            if @details != nil
                @details
            else
                super.to_s
            end
        end
    end

    attach_function :vips_init, [:string], :int

    if Vips::vips_init($0) != 0
        throw Vips::get_error
    end

    # don't use at_exit to call vips_shutdown, it causes problems with fork, and
    # in any case libvips does this for us

    attach_function :vips_object_print_all, [], :void
    attach_function :vips_leak_set, [:int], :void

    def self.showall
        if @@debug
            GC.start
            vips_object_print_all
        end
    end

    if @@debug
        vips_leak_set 1
    end

    attach_function :version, :vips_version, [:int], :int
    attach_function :version_string, :vips_version_string, [], :string

    LIBRARY_VERSION = Vips::version_string

end

require 'vips/object'
require 'vips/operation'
require 'vips/image'
require 'vips/interpolate'
require 'vips/version'


