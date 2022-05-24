# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require "ffi"

module Vips
  if Vips.at_least_libvips?(8, 9)
    attach_function :vips_target_custom_new, [], :pointer
  end

  # A target you can attach action signal handlers to to implememt
  # custom output types.
  #
  # For example:
  #
  # ```ruby
  # file = File.open "some/file/name", "wb"
  # target = Vips::TargetCustom.new
  # target.on_write { |bytes| file.write bytes }
  # image.write_to_target target, ".png"
  # ```
  #
  # (just an example -- of course in practice you'd use {Target#new_to_file}
  # to write to a named file)
  class TargetCustom < Vips::Target
    module TargetCustomLayout
      def self.included(base)
        base.class_eval do
          layout :parent, Vips::Target::Struct
          # rest opaque
        end
      end
    end

    class Struct < Vips::Target::Struct
      include TargetCustomLayout
    end

    class ManagedStruct < Vips::Target::ManagedStruct
      include TargetCustomLayout
    end

    def initialize
      pointer = Vips.vips_target_custom_new
      raise Vips::Error if pointer.null?

      super pointer
    end

    # The block is executed to write data to the source. The interface is
    # exactly as IO::write, ie. it should write the string and return the
    # number of bytes written.
    #
    # @yieldparam bytes [String] Write these bytes to the file
    # @yieldreturn [Integer] The number of bytes written, or -1 on error
    def on_write &block
      signal_connect "write" do |p, len|
        chunk = p.get_bytes(0, len)
        bytes_written = block.call chunk
        chunk.clear

        bytes_written
      end
    end

    # The block is executed to read data from the target. The interface is
    # exactly as IO::read, ie. it takes a maximum number of bytes to read and
    # returns a string of bytes from the target, or nil if the target is already
    # at end of file.
    #
    # This handler is optional and only needed for image formats which have
    # to be able to read their own output, like TIFF.
    #
    # @yieldparam length [Integer] Read and return up to this many bytes
    # @yieldreturn [String] Up to length bytes of data, or nil for EOF
    def on_read &block
      # target read added in 8.13
      if Vips.at_least_libvips?(8, 13)
        signal_connect "read" do |buf, len|
          chunk = block.call len
          return 0 if chunk.nil?
          bytes_read = chunk.bytesize
          buf.put_bytes(0, chunk, 0, bytes_read)
          chunk.clear

          bytes_read
        end
      end
    end

    # The block is executed to seek the target. The interface is exactly as
    # IO::seek, ie. it should take an offset and whence, and return the
    # new read position.
    #
    # This handler is optional and only needed for image formats which have
    # to be able to read their own output, like TIFF.
    #
    # @yieldparam offset [Integer] Seek offset
    # @yieldparam whence [Integer] Seek whence
    # @yieldreturn [Integer] the new read position, or -1 on error
    def on_seek &block
      # target seek added in 8.13
      if Vips.at_least_libvips?(8, 13)
        signal_connect "seek" do |offset, whence|
          block.call offset, whence
        end
      end
    end

    # The block is executed at the end of write. It should do any necessary
    # finishing action, such as closing a file, and return 0 on sucess and -1
    # on error
    #
    # @yieldreturn [Integer] 0 on sucess, or -1 on error
    def on_end &block
      signal_name = Vips.at_least_libvips?(8, 13) ? "end" : "finish"
      signal_connect signal_name do
        block.call
      end
    end

    # Deprecated name for libvips before 8.13
    def on_finish &block
      signal_connect "finish" do
        block.call
      end
    end
  end
end
