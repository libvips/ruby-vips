# This module provides an interface GValue via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module GLib

    class GValue < FFI::ManagedStruct
        layout :gtype, :GType, 
               :data, [:ulong_long, 2]

        def self.release ptr
            # Vips::log "GLib::GValue::release ptr = #{ptr}"
            GLib::g_value_unset ptr 
        end

        def self.alloc
            # allocate memory
            memory = FFI::MemoryPointer.new GValue

            # make this alloc autorelease ... we mustn't release in 
            # GValue::release, since we are used to wrap GValue pointers 
            # made by other people
            pointer = FFI::Pointer.new GValue, memory

            # ... and wrap in a GValue
            GValue.new pointer
        end

        def init gtype
            GLib::g_value_init self, gtype
        end

        def set value
            # Vips::log "GLib::GValue.set: value = #{value}"

            gtype = self[:gtype]
            fundamental = GLib::g_type_fundamental gtype

            case gtype
            when GBOOL_TYPE
                GLib::g_value_set_boolean self, (value ? 1 : 0)

            when GINT_TYPE
                GLib::g_value_set_int self, value

            when GDOUBLE_TYPE
                GLib::g_value_set_double self, value

            when GSTR_TYPE
                # set_string takes a copy, no lifetime worries
                GLib::g_value_set_string self, value

            when Vips::ARRAY_INT_TYPE
                value = [value] if not value.is_a? Array

                Vips::vips_value_set_array_int self, nil, value.length
                ptr = Vips::vips_value_get_array_int self, nil
                ptr.write_array_of_int32 value

            when Vips::ARRAY_DOUBLE_TYPE
                value = [value] if not value.is_a? Array

                # this will allocate an array in the gvalue
                Vips::vips_value_set_array_double self, nil, value.length

                # pull the array out and fill it
                ptr = Vips::vips_value_get_array_double self, nil

                ptr.write_array_of_double value

            when Vips::ARRAY_IMAGE_TYPE
                value = [value] if not value.is_a? Array

                Vips::vips_value_set_array_image self, value.length
                ptr = Vips::vips_value_get_array_image self, nil
                ptr.write_array_of_pointer value

                # the gvalue needs a ref on each of the images
                value.each {|image| GLib::g_object_ref image}

            when Vips::BLOB_TYPE
                len = value.length
                ptr = GLib::g_malloc len
                ptr.write_bytes value
                Vips::vips_value_set_blob self, G_FREE_CALLBACK, ptr, len

            else
                case fundamental
                when GFLAGS_TYPE
                    GLib::g_value_set_flags self, value

                when GENUM_TYPE
                    value = value.to_s if value.is_a? Symbol

                    if value.is_a? String
                        value = Vips::vips_enum_from_nick "ruby-vips", 
                            self[:gtype], value
                        if value == -1
                            raise Vips::Error
                        end
                    end

                    GLib::g_value_set_enum self, value

                when GOBJECT_TYPE
                    # g_value_set_object() will add an extra ref
                    GLib::g_value_set_object self, value
                    GLib::g_object_unref value

                else
                    raise Vips::Error, "unimplemented gtype for set: #{gtype}"
                end
            end
        end

        def get
            gtype = self[:gtype]
            fundamental = GLib::g_type_fundamental gtype
            result = nil

            case gtype
            when GBOOL_TYPE
                result = GLib::g_value_get_boolean(self) != 0 ? true : false

            when GINT_TYPE
                result = GLib::g_value_get_int self

            when GDOUBLE_TYPE
                result = GLib::g_value_get_double self

            when GSTR_TYPE
                # FIXME do we need to strdup here?
                result = GLib::g_value_get_string self

            when Vips::ARRAY_INT_TYPE
                len = Vips::IntStruct.new
                array = Vips::vips_value_get_array_int self, len
                result = array.get_array_of_int32 0, len[:value]

            when Vips::ARRAY_DOUBLE_TYPE
                len = Vips::IntStruct.new
                array = Vips::vips_value_get_array_double self, len
                result = array.get_array_of_double 0, len[:value]

            when Vips::ARRAY_IMAGE_TYPE
                len = Vips::IntStruct.new
                array = Vips::vips_value_get_array_image self, len
                result = array.get_array_of_pointer 0, len[:value]
                result.map! do |pointer| 
                    GLib::g_object_ref pointer
                    Vips::Image.new pointer
                end

            when Vips::BLOB_TYPE
                len = Vips::SizeStruct.new
                array = Vips::vips_value_get_blob self, len
                result = array.get_bytes 0, len[:value]

            else
                case fundamental
                when GFLAGS_TYPE
                    result = GLib::g_value_get_flags self

                when GENUM_TYPE
                    enum_value = GLib::g_value_get_enum self
                    # this returns a static string, no need to worry about 
                    # lifetime
                    enum_name = Vips::vips_enum_nick self[:gtype], enum_value
                    if enum_name == nil
                        raise Vips::Error
                    end
                    result = enum_name.to_sym

                when GOBJECT_TYPE
                    # g_value_get_object() does not add a ref
                    obj = GLib::g_value_get_object self
                    result = Vips::Image.new obj

                else
                    raise Vips::Error, "unimplemented gtype for get: #{gtype}"

                end
            end

            # Vips::log "GLib::GValue.get: result = #{result}"

            return result
        end

    end

    attach_function :g_value_init, [GValue.ptr, :GType], :void
    attach_function :g_value_unset, [GValue.ptr], :void

    attach_function :g_value_set_boolean, [GValue.ptr, :int], :void
    attach_function :g_value_set_int, [GValue.ptr, :int], :void
    attach_function :g_value_set_double, [GValue.ptr, :double], :void
    attach_function :g_value_set_enum, [GValue.ptr, :int], :void
    attach_function :g_value_set_flags, [GValue.ptr, :uint], :void
    attach_function :g_value_set_string, [GValue.ptr, :string], :void
    attach_function :g_value_set_object, [GValue.ptr, :pointer], :void

    attach_function :g_value_get_boolean, [GValue.ptr], :int
    attach_function :g_value_get_int, [GValue.ptr], :int
    attach_function :g_value_get_double, [GValue.ptr], :double
    attach_function :g_value_get_enum, [GValue.ptr], :int
    attach_function :g_value_get_flags, [GValue.ptr], :int
    attach_function :g_value_get_string, [GValue.ptr], :string
    attach_function :g_value_get_object, [GValue.ptr], :pointer

    # use :pointer rather than GObject.ptr to avoid casting later
    attach_function :g_object_set_property, 
        [:pointer, :string, GValue.ptr], :void
    attach_function :g_object_get_property, 
        [:pointer, :string, GValue.ptr], :void

end
