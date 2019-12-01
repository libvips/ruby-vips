# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
  if Vips::at_least_libvips?(8, 9)
    attach_function :vips_streami_new_from_descriptor, [:int], :pointer
    attach_function :vips_streami_new_from_file, [:pointer], :pointer
    attach_function :vips_streami_new_from_memory, [:pointer, :size_t], :pointer
  end

  # An input stream. For example:
  #
  # ```ruby
  # streami = Vips::Streami.new_from_file("k2.jpg")
  # image = Vips::Image.new_from_stream(streami)
  # ```
  class Streami < Vips::Stream
    # The layout of the VipsRegion struct.
    module StreamiLayout
      def self.included(base)
        base.class_eval do
          layout :parent, Vips::Stream::Struct
          # rest opaque
        end
      end
    end

    class Struct < Vips::Stream::Struct
      include StreamiLayout
    end

    class ManagedStruct < Vips::Stream::ManagedStruct
      include StreamiLayout
    end

    # Create a new input stream from a file descriptor. File descriptors are
    # small integers, for example 0 is stdin.
    #
    # Pass input streams to Vips::Image.new_from_stream to load images from
    # them.
    # 
    # @param descriptor [Integer] the file descriptor
    # @return [Streami] the new Vips::Streami
    def self.new_from_descriptor(descriptor)
      ptr = Vips::vips_streami_new_from_descriptor descriptor
      raise Vips::Error if ptr.null?

      Vips::Streami.new ptr
    end

    # Create a new input stream from a file name. 
    #
    # Pass input streams to Vips::Image.new_from_stream to load images from
    # them.
    # 
    # @param filename [String] the name of the file
    # @return [Streami] the new Vips::Streami
    def self.new_from_file(filename)
      raise Vips::Error, "filename is nil" if filename.nil?
      ptr = Vips::vips_streami_new_from_file filename
      raise Vips::Error if ptr.null?

      Vips::Streami.new ptr
    end

    # Create a new input stream from an area of memory. Memory areas can be
    # string, arrays and so forth -- anything that supports bytesize.
    #
    # Pass input streams to Vips::Image.new_from_stream to load images from
    # them.
    # 
    # @param descriptor [String] memory area 
    # @return [Streami] the new Vips::Streami
    def self.new_from_memory(data)
      ptr = Vips::vips_streami_new_from_memory data, data.bytesize
      raise Vips::Error if ptr.null?

      # FIXME do we need to keep a ref to the underlying memory area? what abut
      # Image.new_from_buffer? Does that need a secret ref too?

      Vips::Streami.new ptr
    end

  end
end
