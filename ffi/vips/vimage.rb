# This module provides an interface to the top level bits of GLib
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
    extend FFI::Library
    ffi_lib 'vips'

    class VipsImage < VipsObject

        # the layout of the VipsImage struct
        module VipsImageLayout
            def self.included(base)
                base.class_eval do
                    layout :parent, VipsObject::Struct
                    # rest opaque
                end
            end
        end

        class Struct < VipsObject::Struct
            include VipsImageLayout

            def initialize(ptr)
                log "Vips::VipsImage::Struct.new: #{ptr}"
                super
            end

        end

        class ManagedStruct < VipsObject::ManagedStruct
            include VipsImageLayout

            def initialize(ptr)
                log "Vips::VipsImage::ManagedStruct.new: #{ptr}"
                super
            end

        end

        def self.new_partial
            VipsImage.new(Vips::vips_image_new)
        end

    end

    attach_function :vips_image_new, [], :pointer

end
