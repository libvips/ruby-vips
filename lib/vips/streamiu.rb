# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
  if Vips::at_least_libvips?(8, 9)
    attach_function :vips_streamiu_new, [:void], :pointer
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
      super Vips::vips_streamiu_new
    end

  end
end
