# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
  private

  # debugging support
  attach_function :vips_object_print_all, [], :void

  # we must init these by hand, since they are usually made on first image
  # create
  attach_function :vips_band_format_get_type, [], :GType
  attach_function :vips_interpretation_get_type, [], :GType
  attach_function :vips_coding_get_type, [], :GType

  public

  # some handy gtypes
  IMAGE_TYPE = GObject::g_type_from_name "VipsImage"
  ARRAY_INT_TYPE = GObject::g_type_from_name "VipsArrayInt"
  ARRAY_DOUBLE_TYPE = GObject::g_type_from_name "VipsArrayDouble"
  ARRAY_IMAGE_TYPE = GObject::g_type_from_name "VipsArrayImage"
  REFSTR_TYPE = GObject::g_type_from_name "VipsRefString"
  BLOB_TYPE = GObject::g_type_from_name "VipsBlob"

  BAND_FORMAT_TYPE = Vips::vips_band_format_get_type
  INTERPRETATION_TYPE = Vips::vips_interpretation_get_type
  CODING_TYPE = Vips::vips_coding_get_type

  if Vips::at_least_libvips?(8, 6)
    attach_function :vips_blend_mode_get_type, [], :GType
    BLEND_MODE_TYPE = Vips::vips_blend_mode_get_type
  else
    BLEND_MODE_TYPE = nil
  end

  private

  attach_function :vips_enum_from_nick, [:string, :GType, :string], :int
  attach_function :vips_enum_nick, [:GType, :int], :string

  attach_function :vips_value_set_ref_string,
      [GObject::GValue.ptr, :string], :void
  attach_function :vips_value_set_array_double,
      [GObject::GValue.ptr, :pointer, :int], :void
  attach_function :vips_value_set_array_int,
      [GObject::GValue.ptr, :pointer, :int], :void
  attach_function :vips_value_set_array_image,
      [GObject::GValue.ptr, :int], :void
  callback :free_fn, [:pointer], :void
  attach_function :vips_value_set_blob,
      [GObject::GValue.ptr, :free_fn, :pointer, :size_t], :void

  class SizeStruct < FFI::Struct
    layout :value, :size_t
  end

  class IntStruct < FFI::Struct
    layout :value, :int
  end

  attach_function :vips_value_get_ref_string,
      [GObject::GValue.ptr, SizeStruct.ptr], :string
  attach_function :vips_value_get_array_double,
      [GObject::GValue.ptr, IntStruct.ptr], :pointer
  attach_function :vips_value_get_array_int,
      [GObject::GValue.ptr, IntStruct.ptr], :pointer
  attach_function :vips_value_get_array_image,
      [GObject::GValue.ptr, IntStruct.ptr], :pointer
  attach_function :vips_value_get_blob,
      [GObject::GValue.ptr, SizeStruct.ptr], :pointer

  attach_function :type_find, :vips_type_find, [:string, :string], :GType

  class Object < GObject::GObject

    # print all active VipsObjects, with their reference counts. Handy for
    # debugging ruby-vips.
    def self.print_all
      GC.start
      Vips::vips_object_print_all
    end

    # the layout of the VipsObject struct
    module ObjectLayout
      def self.included base
        base.class_eval do
          # don't actually need most of these
          layout :parent, GObject::GObject::Struct,
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

    class Struct < GObject::GObject::Struct
      include ObjectLayout

    end

    class ManagedStruct < GObject::GObject::ManagedStruct
      include ObjectLayout

    end

    # return a pspec, or nil ... nil wil leave a message in the error log
    # which you must clear
    def get_pspec name
      pspec = GObject::GParamSpecPtr.new
      argument_class = Vips::ArgumentClassPtr.new
      argument_instance = Vips::ArgumentInstancePtr.new

      result = Vips::vips_object_get_argument self, name,
          pspec, argument_class, argument_instance
      return nil if result != 0

      pspec
    end

    # return a gtype, raise an error on not found
    def get_typeof_error name
      pspec = get_pspec name
      raise Vips::Error unless pspec

      pspec[:value][:value_type]
    end

    # return a gtype, 0 on not found
    def get_typeof name
      pspec = get_pspec name
      unless pspec
        Vips::vips_error_clear
        return 0
      end

      pspec[:value][:value_type]
    end

    def get name
      gtype = get_typeof_error name
      gvalue = GObject::GValue.alloc
      gvalue.init gtype
      GObject::g_object_get_property self, name, gvalue
      result = gvalue.get

      GLib::logger.debug("Vips::Object.get") {"#{name} == #{result}"}

      return result
    end

    def set name, value
      GLib::logger.debug("Vips::Object.set") {"#{name} = #{value}"}

      gtype = get_typeof_error name
      gvalue = GObject::GValue.alloc
      gvalue.init gtype
      gvalue.set value
      GObject::g_object_set_property self, name, gvalue
    end

  end

  class ObjectClass < FFI::Struct
    # opaque
  end

  class Argument < FFI::Struct
    layout :pspec, GObject::GParamSpec.ptr
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
       GObject::GParamSpecPtr.ptr,
       ArgumentClassPtr.ptr, ArgumentInstancePtr.ptr],
      :int

  attach_function :vips_object_print_all, [], :void

  attach_function :vips_object_set_from_string, [:pointer, :string], :int

  callback :type_map_fn, [:GType, :pointer], :pointer
  attach_function :vips_type_map, [:GType, :type_map_fn, :pointer], :pointer

  attach_function :vips_object_get_description, [:pointer], :string

end


