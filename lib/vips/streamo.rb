# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
  if Vips::at_least_libvips?(8, 9)
    attach_function :vips_streamo_new_to_descriptor, [:int], :pointer
    attach_function :vips_streamo_new_to_file, [:string], :pointer
    attach_function :vips_streamo_new_to_memory, [], :pointer
  end

  # An output stream. For example:
  #
  # ```ruby
  # stream = Vips::Streamo.new_to_file('k2.jpg')
  # image.write_to_stream(stream, '.jpg')
  # ```
  class Streamo < Vips::Stream
    # The layout of the VipsRegion struct.
    module StreamoLayout
      def self.included(base)
        base.class_eval do
          layout :parent, Vips::Stream::Struct
          # rest opaque
        end
      end
    end

    class Struct < Vips::Stream::Struct
      include StreamoLayout
    end

    class ManagedStruct < Vips::Stream::ManagedStruct
      include StreamoLayout
    end

    # Create a new output stream to a file descriptor. File descriptors are
    # small integers, for example 1 is stdout.
    #
    # Pass output streams to Vips::Image.write_to_stream to write images to
    # them.
    # 
    # @param descriptor [Integer] the file descriptor
    # @return [Streamo] the new Vips::Streamo
    def self.new_to_descriptor(descriptor)
      ptr = Vips::vips_streamo_new_to_descriptor descriptor
      raise Vips::Error if ptr.null?

      Vips::Streamo.new ptr
    end

    # Create a new output stream to a file name. 
    #
    # Pass output streams to Vips::Image.write_to_stream to write images to
    # them.
    # 
    # @param filename [String] the name of the file
    # @return [Streamo] the new Vips::Streamo
    def self.new_to_file(filename)
      raise Vips::Error, 'filename is nil' if filename.nil?
      ptr = Vips::vips_streamo_new_to_file filename
      raise Vips::Error if ptr.null?

      Vips::Streamo.new ptr
    end

    # Create a new output stream to an area of memory. 
    #
    # Pass output streams to Vips::Image.write_to_stream to write images to
    # them.
    #
    # Once the image has been written, use `streamo.get("blob")` to read out the
    # data.
    # 
    # @return [Streamo] the new Vips::Streamo
    def self.new_to_memory
      ptr = Vips::vips_streamo_new_to_memory 

      Vips::Streamo.new ptr
    end

  end
end
