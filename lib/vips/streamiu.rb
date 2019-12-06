# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
  if Vips::at_least_libvips?(8, 9)
    attach_function :vips_streamiu_new, [], :pointer
  end

  # A user input stream you can attach action signal handlers to to implememt 
  # custom input types.
  #
  # ```ruby
  # stream = Vips::Streamiu.new
  # stream.signal_connect "read" do |buf, len|
  #   # read up to len bytes into buf, return the nuber of bytes 
  #   # actually read
  # end
  # ```
  class Streamiu < Vips::Streami
    module StreamiuLayout
      def self.included(base)
        base.class_eval do
          layout :parent, Vips::Streami::Struct
          # rest opaque
        end
      end
    end

    class Struct < Vips::Streami::Struct
      include StreamiuLayout
    end

    class ManagedStruct < Vips::Streami::ManagedStruct
      include StreamiuLayout
    end

    def initialize
      pointer = Vips::vips_streamiu_new
      raise Vips::Error if pointer.null?

      super pointer
    end

    # The block is executed to read data from the source. The interface is
    # exactly as IO::seek, ie. it takes a maximum number of bytes to read and
    # returns a string of bytes from the source, or nil if the source is already
    # at eod of file.
    def on_read &block
      signal_connect "read" do |buf, len|
        chunk = block.call len
        return 0 if chunk == nil

        buf.put_bytes(0, chunk, 0, chunk.bytesize)
        chunk.bytesize
      end
    end

    # The block is executed to seek the source. The interface is exactly as
    # IO::seek, ie. it should take an offset and whence, and return the new read
    # position.
    def on_seek &block
      signal_connect "seek" do |offset, whence|
        block.call offset, whence
      end
    end

  end
end
