# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
  if Vips::at_least_libvips?(8, 9)
    attach_function :vips_stream_filename, [:pointer], :string
    attach_function :vips_stream_nick, [:pointer], :string
  end

  # Abstract base class for streams.
  class Stream < Vips::Object
    # The layout of the VipsRegion struct.
    module StreamLayout
      def self.included(base)
        base.class_eval do
          layout :parent, Vips::Object::Struct
          # rest opaque
        end
      end
    end

    class Struct < Vips::Object::Struct
      include StreamLayout
    end

    class ManagedStruct < Vips::Object::ManagedStruct
      include StreamLayout
    end

    def filename
      Vips::vips_stream_filename self
    end

    def nick
      Vips::vips_stream_nick self
    end
  end
end
