# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
  if Vips::at_least_libvips?(8, 9)
    attach_function :vips_streamou_new, [], :pointer
  end

  # A user output stream you can attach action signal handlers to to implememt 
  # custom output types.
  #
  # ```ruby
  # stream = Vips::Streamou.new
  # stream.signal_connect "write" do |buf, len|
  #   # write up to len bytes from buf, return the nuber of bytes 
  #   # actually written
  # end
  # ```
  class Streamou < Vips::Streamo
    module StreamouLayout
      def self.included(base)
        base.class_eval do
          layout :parent, Vips::Streamo::Struct
          # rest opaque
        end
      end
    end

    class Struct < Vips::Streamo::Struct
      include StreamouLayout
    end

    class ManagedStruct < Vips::Streamo::ManagedStruct
      include StreamouLayout
    end

    def initialize
      pointer = Vips::vips_streamou_new
      raise Vips::Error if pointer.null?

      super pointer
    end

  end
end
