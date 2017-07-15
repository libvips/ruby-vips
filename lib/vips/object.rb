# This module provides an interface to the top level bits of GLib
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
    private

    # we must init these by hand, since they are usually made on first image
    # create
    attach_function :vips_band_format_get_type, [], :GType
    attach_function :vips_interpretation_get_type, [], :GType
    attach_function :vips_coding_get_type, [], :GType

    public

    # some handy gtypes
    IMAGE_TYPE = GLib::g_type_from_name("VipsImage")
    ARRAY_INT_TYPE = GLib::g_type_from_name("VipsArrayInt")
    ARRAY_DOUBLE_TYPE = GLib::g_type_from_name("VipsArrayDouble")
    ARRAY_IMAGE_TYPE = GLib::g_type_from_name("VipsArrayImage")
    REFSTR_TYPE = GLib::g_type_from_name("VipsRefString")
    BLOB_TYPE = GLib::g_type_from_name("VipsBlob")

    BAND_FORMAT_TYPE = Vips::vips_band_format_get_type
    INTERPRETATION_TYPE = Vips::vips_interpretation_get_type
    CODING_TYPE = Vips::vips_coding_get_type

    private

    attach_function :vips_enum_from_nick, [:string, :GType, :string], :int
    attach_function :vips_enum_nick, [:GType, :int], :string

    attach_function :vips_value_set_array_double, 
        [GLib::GValue.ptr, :pointer, :int], :void
    attach_function :vips_value_set_array_int, 
        [GLib::GValue.ptr, :pointer, :int], :void
    attach_function :vips_value_set_array_image, 
        [GLib::GValue.ptr, :int], :void
    callback :free_fn, [:pointer], :void
    attach_function :vips_value_set_blob, 
        [GLib::GValue.ptr, :free_fn, :pointer, :size_t], :void

    class SizeStruct < FFI::Struct
        layout :value, :size_t
    end

    class IntStruct < FFI::Struct
        layout :value, :int
    end

    attach_function :vips_value_get_ref_string, 
        [GLib::GValue.ptr, SizeStruct.ptr], :string
    attach_function :vips_value_get_array_double, 
        [GLib::GValue.ptr, IntStruct.ptr], :pointer
    attach_function :vips_value_get_array_int, 
        [GLib::GValue.ptr, IntStruct.ptr], :pointer
    attach_function :vips_value_get_array_image, 
        [GLib::GValue.ptr, IntStruct.ptr], :pointer
    attach_function :vips_value_get_blob, 
        [GLib::GValue.ptr, SizeStruct.ptr], :pointer

    attach_function :type_find, :vips_type_find, [:string, :string], :GType

    class Object < GLib::GObject

        # the layout of the VipsObject struct
        module ObjectLayout
            def self.included(base)
                base.class_eval do
                    # don't actually need most of these
                    layout :parent, GLib::GObject::Struct, 
                       :constructed, :int,
                       :static_object, :int,
                       :argument_table, :pointer,
                       :nickname, :string,
                       :description, :string,
                       :preclose, :int,
                       :close, :int,
                       :postclose, :int,
                       :local_memory, :size_t
                end
            end
        end

        class Struct < GLib::GObject::Struct
            include ObjectLayout

            def initialize(ptr)
                Vips::log "Vips::Object::Struct.new: #{ptr}"
                super
            end

        end

        class ManagedStruct < GLib::GObject::ManagedStruct
            include ObjectLayout

            def initialize(ptr)
                Vips::log "Vips::Object::ManagedStruct.new: #{ptr}"
                super
            end

        end

        def get_typeof(name)
            pspec = GLib::GParamSpecPtr.new
            argument_class = Vips::ArgumentClassPtr.new
            argument_instance = Vips::ArgumentInstancePtr.new

            result = Vips::vips_object_get_argument self, name,
                pspec, argument_class, argument_instance
            if result != 0 
                raise Vips::Error
            end

            pspec[:value][:value_type]
        end

        def get(name)
            gtype = get_typeof name
            gvalue = GLib::GValue.alloc 
            gvalue.init gtype
            GLib::g_object_get_property self, name, gvalue
            result = gvalue.get

            Vips::log "Vips::Object.get(\"#{name}\"): result = #{result}"

            return result
        end

        def set(name, value)
            Vips::log "Vips::Object.set: #{name} = #{value}"

            gtype = get_typeof name
            gvalue = GLib::GValue.alloc 
            gvalue.init gtype
            gvalue.set value
            GLib::g_object_set_property self, name, gvalue
        end

    end

    class ObjectClass < FFI::Struct
        # opaque
    end

    class Argument < FFI::Struct
        layout :pspec, GLib::GParamSpec.ptr
    end

    class ArgumentInstance < Argument
        layout :parent, Argument
        # rest opaque
    end

    # enum VipsArgumentFlags 
    ARGUMENT_REQUIRED = 1
    ARGUMENT_CONSTRUCT = 2
    ARGUMENT_SET_ONCE = 4
    ARGUMENT_SET_ALWAYS = 8
    ARGUMENT_INPUT = 16
    ARGUMENT_OUTPUT = 32
    ARGUMENT_DEPRECATED = 64
    ARGUMENT_MODIFY = 128

    ARGUMENT_FLAGS = {
        :required => ARGUMENT_REQUIRED,
        :construct => ARGUMENT_CONSTRUCT,
        :set_once => ARGUMENT_SET_ONCE,
        :set_always => ARGUMENT_SET_ALWAYS,
        :input => ARGUMENT_INPUT,
        :output => ARGUMENT_OUTPUT,
        :deprecated => ARGUMENT_DEPRECATED,
        :modify => ARGUMENT_MODIFY
    }

    class ArgumentClass < Argument
        layout :parent, Argument,
               :object_class, ObjectClass.ptr,
               :flags, :uint,
               :priority, :int,
               :offset, :ulong_long
    end

    class ArgumentClassPtr < FFI::Struct
        layout :value, ArgumentClass.ptr
    end

    class ArgumentInstancePtr < FFI::Struct
        layout :value, ArgumentInstance.ptr
    end

    # just use :pointer, not VipsObject.ptr, to avoid casting gobject
    # subclasses
    attach_function :vips_object_get_argument, 
        [:pointer, :string, 
         GLib::GParamSpecPtr.ptr, ArgumentClassPtr.ptr, ArgumentInstancePtr.ptr],
        :int

    attach_function :vips_object_print_all, [], :void

    attach_function :vips_object_set_from_string, [:pointer, :string], :int

end


