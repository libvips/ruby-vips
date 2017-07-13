# This module provides an interface GValue via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module GLib
    extend FFI::Library
    ffi_lib 'gobject-2.0'

    attach_function :g_malloc, [:size_t], :pointer
    attach_function :g_free, [:pointer], :void

    # :gtype will usually be 64-bit, but will be 32-bit on 32-bit Windows
    typedef :ulong, :GType

    attach_function :g_type_name, [:GType], :string
    attach_function :g_type_from_name, [:string], :GType
    attach_function :g_type_fundamental, [:GType], :GType

    # look up some common gtypes
    GBOOL_TYPE = g_type_from_name("gboolean")
    GINT_TYPE = g_type_from_name("gint")
    GDOUBLE_TYPE = g_type_from_name("gdouble")
    GENUM_TYPE = g_type_from_name("GEnum")
    GFLAGS_TYPE = g_type_from_name("GFlags")
    GSTR_TYPE = g_type_from_name("gchararray")

    class GValue < FFI::ManagedStruct
        layout :gtype, :GType, 
               :data, [:ulong_long, 2]

        def self.release(ptr)
            puts "GLib::GValue::release ptr = #{ptr}"
            GLib::g_value_unset(ptr) 
        end

        def self.alloc
            # we seem to need the extra cast, I'm not sure why
            memory = FFI::MemoryPointer.new GValue
            pointer = FFI::Pointer.new GValue, memory
            GValue.new(pointer)
        end

        def init(gtype)
            GLib::g_value_init(self, gtype)
        end

        def set(value)
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
            when Vips::IMAGE_TYPE
                # this will add a ref, held by the gvalue ... it will be 
                # dropped by g_value_unset() when the gvalue is GC'd
                GLib::g_value_set_object self, value
            when Vips::ARRAY_INT_TYPE
            when Vips::ARRAY_DOUBLE_TYPE
            when Vips::ARRAY_IMAGE_TYPE
            when Vips::BLOB_TYPE
            else
                case fundamental
                when GFLAGS_TYPE
                    GLib::g_value_set_flags self, value
                when GENUM_TYPE
                    if value.is_a? Symbol
                        value = value.to_s
                    end

                    if value.is_a? String
                        value = Vips::vips_enum_from_nick "ruby-vips", 
                            self[:gtype], value
                        if value == -1
                            throw Vips::get_error
                        end
                    end

                    GLib::g_value_set_enum self, value
                else
                    puts "unimplemented gtype for set: #{gtype}"
                end
            end
        end

        def get
            gtype = self[:gtype]
            fundamental = GLib::g_type_fundamental gtype

            case gtype
            when GBOOL_TYPE
                GLib::g_value_get_boolean(self) != 0 ? true : false
            when GINT_TYPE
                GLib::g_value_get_int(self)
            when GDOUBLE_TYPE
                GLib::g_value_get_double(self)
            when GSTR_TYPE
                # FIXME do we need to strdup here?
                Vips::g_value_get_string self
            when Vips::IMAGE_TYPE
                # FIXME ... cast to VipsImage
                Vips::g_value_get_object self
            when Vips::ARRAY_INT_TYPE
            when Vips::ARRAY_DOUBLE_TYPE
            when Vips::ARRAY_IMAGE_TYPE
            when Vips::BLOB_TYPE
            else
                case fundamental
                when GFLAGS_TYPE
                    GLib::g_value_get_flags(self)
                when GENUM_TYPE
                    enum_value = GLib::g_value_get_enum(self)
                    # this returns a static string, no need to worry about 
                    # lifetime
                    enum_name = Vips::vips_enum_nick self[:gtype], enum_value
                    if enum_name == nil
                        throw Vips::get_error
                    end
                    enum_name.to_sym
                else
                    puts "unimplemented gtype for set: #{gtype}"
                end
            end
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
    attach_function :g_value_set_object, [GValue.ptr, GObject.ptr], :void

    attach_function :g_value_get_boolean, [GValue.ptr], :int
    attach_function :g_value_get_int, [GValue.ptr], :int
    attach_function :g_value_get_double, [GValue.ptr], :double
    attach_function :g_value_get_enum, [GValue.ptr], :int
    attach_function :g_value_get_flags, [GValue.ptr], :int
    attach_function :g_value_get_string, [GValue.ptr], :string
    attach_function :g_value_get_object, [GValue.ptr], GObject.ptr

end
