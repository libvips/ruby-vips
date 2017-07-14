# This module provides an interface to the top level bits of GLib
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
    # some handy gtypes
    IMAGE_TYPE = GLib::g_type_from_name("VipsImage")
    ARRAY_INT_TYPE = GLib::g_type_from_name("VipsArrayInt")
    ARRAY_DOUBLE_TYPE = GLib::g_type_from_name("VipsArrayDouble")
    ARRAY_IMAGE_TYPE = GLib::g_type_from_name("VipsArrayImage")
    REFSTR_TYPE = GLib::g_type_from_name("VipsRefString")
    BLOB_TYPE = GLib::g_type_from_name("VipsBlob")

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

    attach_function :vips_value_get_ref_string, 
        [GLib::GValue.ptr, :pointer], :string
    attach_function :vips_value_get_array_double, 
        [GLib::GValue.ptr, :pointer], :pointer
    attach_function :vips_value_get_array_int, 
        [GLib::GValue.ptr, :pointer], :pointer
    attach_function :vips_value_get_array_image, 
        [GLib::GValue.ptr, :pointer], :pointer
    attach_function :vips_value_get_blob, 
        [GLib::GValue.ptr, :pointer], :pointer

    # init by hand for testing 
    attach_function :vips_interpretation_get_type, [], :GType

    class VipsObject < GLib::GObject

        # the layout of the VipsObject struct
        module VipsObjectLayout
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
            include VipsObjectLayout

            def initialize(ptr)
                log "Vips::VipsObject::Struct.new: #{ptr}"
                super
            end

        end

        class ManagedStruct < GLib::GObject::ManagedStruct
            include VipsObjectLayout

            def initialize(ptr)
                log "Vips::VipsObject::ManagedStruct.new: #{ptr}"
                super
            end

        end

        def get_typeof(name)
            pspec = GLib::GParamSpecPtr.new
            argument_class = Vips::VipsArgumentClassPtr.new
            argument_instance = Vips::VipsArgumentInstancePtr.new

            result = Vips::vips_object_get_argument self, name,
                pspec, argument_class, argument_instance

            if result != 0 
                throw Vips::get_error
            end

            pspec[:value][:value_type]
        end

        def get(name)
            gtype = get_typeof name
            gvalue = GLib::GValue.alloc 
            gvalue.init gtype
            GLib::g_object_get_property self, name, gvalue
            result = gvalue.get

            log "Vips::VipsObject.get(\"#{name}\"): result = #{result}"

            return result
        end

        def set(name, value)
            log "Vips::VipsObject.set: #{name} = #{value}"

            gtype = get_typeof name
            gvalue = GLib::GValue.alloc 
            gvalue.init gtype
            gvalue.set value
            GLib::g_object_set_property self, name, gvalue
        end

    end

    class VipsObjectClass < FFI::Struct
        # opaque
    end

    class VipsArgument < FFI::Struct
        layout :pspec, GLib::GParamSpec.ptr
    end

    class VipsArgumentInstance < VipsArgument
        layout :parent, VipsArgument
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

    VIPS_ARGUMENT_FLAGS = {
        :required => ARGUMENT_REQUIRED,
        :construct => ARGUMENT_CONSTRUCT,
        :set_once => ARGUMENT_SET_ONCE,
        :set_always => ARGUMENT_SET_ALWAYS,
        :input => ARGUMENT_INPUT,
        :output => ARGUMENT_OUTPUT,
        :deprecated => ARGUMENT_DEPRECATED,
        :modify => ARGUMENT_MODIFY
    }

    class VipsArgumentClass < VipsArgument
        layout :parent, VipsArgument,
               :object_class, VipsObjectClass.ptr,
               :flags, :uint,
               :priority, :int,
               :offset, :ulong_long
    end

    class VipsArgumentClassPtr < FFI::Struct
        layout :value, VipsArgumentClass.ptr
    end

    class VipsArgumentInstancePtr < FFI::Struct
        layout :value, VipsArgumentInstance.ptr
    end

    # just use :pointer, not VipsObject.ptr, to avoid casting gobject
    # subclasses
    attach_function :vips_object_get_argument, 
        [:pointer, :string, 
         GLib::GParamSpecPtr.ptr, VipsArgumentClassPtr.ptr, 
         VipsArgumentInstancePtr.ptr], :int

    attach_function :vips_object_print_all, [], :void

    attach_function :vips_object_set_from_string, [:pointer, :string], :int

end


