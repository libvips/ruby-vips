# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
  if Vips::at_least_libvips?(8, 9)
    attach_function :vips_source_custom_new, [], :pointer
  end

  # A source you can attach action signal handlers to to implememt 
  # custom input types.
  #
  # ```ruby
  # source = Vips::SourceCustom.new
  # source.on_read do |len|
  #   # return up to len bytes 
  # end
  # ```
  class SourceCustom < Vips::Source
    module SourceCustomLayout
      def self.included(base)
        base.class_eval do
          layout :parent, Vips::Source::Struct
          # rest opaque
        end
      end
    end

    class Struct < Vips::Source::Struct
      include SourceCustomLayout
    end

    class ManagedStruct < Vips::Source::ManagedStruct
      include SourceCustomLayout
    end

    def initialize
      pointer = Vips::vips_source_custom_new
      raise Vips::Error if pointer.null?

      super pointer
    end

    # The block is executed to read data from the source. The interface is
    # exactly as IO::read, ie. it takes a maximum number of bytes to read and
    # returns a string of bytes from the source, or nil if the source is already
    # at end of file.
    def on_read &block
      signal_connect "read" do |buf, len|
        chunk = block.call len
        return 0 if chunk == nil
        bytes_read = chunk.bytesize
        buf.put_bytes(0, chunk, 0, bytes_read)
        chunk.clear

        bytes_read
      end
    end

    # The block is executed to seek the source. The interface is exactly as
    # IO::seek, ie. it should take an offset and whence, and return the new read
    # position.
    #
    # This handler is optional -- if you do not attach a seek handler,
    # ruby-vips will treat your source as an unseekable pipe-like object, and
    # do extra caching.
    def on_seek &block
      signal_connect "seek" do |offset, whence|
        block.call offset, whence
      end
    end

  end
end
