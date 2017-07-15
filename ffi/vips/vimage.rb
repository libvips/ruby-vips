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

    class Image < Vips::Object

        # the layout of the VipsImage struct
        module ImageLayout
            def self.included(base)
                base.class_eval do
                    layout :parent, Vips::Object::Struct
                    # rest opaque
                end
            end
        end

        class Struct < Vips::Object::Struct
            include ImageLayout

            def initialize(ptr)
                log "Vips::Image::Struct.new: #{ptr}"
                super
            end

        end

        class ManagedStruct < Vips::Object::ManagedStruct
            include ImageLayout

            def initialize(ptr)
                log "Vips::Image::ManagedStruct.new: #{ptr}"
                super
            end

        end

        def self.new_partial
            Image.new Vips::vips_image_new
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

            Operation::call loader, [filename, opts], option_string
        end

    end

end
