# This module provides an interface to the top level bits of GLib
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips

    attach_function :vips_image_new, [], :pointer

    attach_function :vips_filename_get_filename, [:string], :string
    attach_function :vips_filename_get_options, [:string], :string
    attach_function :vips_filename_get_options, [:string], :string

    attach_function :vips_foreign_find_load, [:string], :string
    attach_function :vips_foreign_find_save, [:string], :string
    attach_function :vips_foreign_find_load_buffer, [:pointer, :size_t], :string
    attach_function :vips_foreign_find_save_buffer, [:pointer, :size_t], :string

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
            VipsImage.new Vips::vips_image_new
        end

        def self.new_from_file(name, opts = {})
            # very common, and Vips::vips_filename_get_filename will segv if we 
            # pass this
            if name == nil
                raise Vips::Error, "filename is nil"
            end
            filename = Vips::vips_filename_get_filename name
            option_string = Vips::vips_filename_get_options name
            loader = Vips::vips_foreign_find_load filename
            if loader == nil
                raise Vips::Error
            end

            Vips::VipsOperation::call loader, [filename, opts], option_string
        end

    end

end
