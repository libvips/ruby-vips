#!/usr/bin/ruby

require 'ffi'

module Libc
    extend FFI::Library
    ffi_lib FFI::Library::LIBC

    attach_function :malloc, [:size_t], :pointer
    attach_function :free, [:pointer], :void
end

module Libgobject
    extend FFI::Library
    ffi_lib 'gobject-2.0'

    attach_function :g_malloc, [:size_t], :pointer
    attach_function :g_free, [:pointer], :void

    class GObject < FFI::Struct
        layout :g_type_instance, :pointer,
               :ref_count, :uint,
               :qdata, :pointer
    end

    # :gtype will usually be 64-bit, but will be 32-bit on 32-bit Windows
    typedef :ulong, :GType

    class GValue < FFI::Struct
        layout :gtype, :GType, 
               :data, [:ulong_long, 2]
    end

    attach_function :g_value_init, [GValue.ptr, :GType], :void
    attach_function :g_value_unset, [GValue.ptr], :void
    attach_function :g_type_name, [:GType], :string
    attach_function :g_type_from_name, [:string], :GType
    attach_function :g_type_fundamental, [:GType], :GType

    attach_function :g_value_set_boolean, [GValue.ptr, :int], :void
    attach_function :g_value_set_int, [GValue.ptr, :int], :void
    attach_function :g_value_set_double, [GValue.ptr, :double], :void
    attach_function :g_value_set_enum, [GValue.ptr, :int], :void
    attach_function :g_value_set_flags, [GValue.ptr, :flags], :void
    attach_function :g_value_set_string, [GValue.ptr, :string], :void
    attach_function :g_value_set_object, [GValue.ptr, GObject.ptr], :void

    class GParamSpec < FFI::Struct
        # the first few public fields
        layout :g_type_instance, :pointer,
               :name, :string,
               :flags, :uint,
               :value_type, :GType,
               :owner_type, :GType
    end

    attach_function :g_object_ref, [:pointer], :void
    attach_function :g_object_unref, [:pointer], :void

    attach_function :g_object_set_property, 
        [GObject.ptr, :string, GValue.ptr], :void
    attach_function :g_object_get_property, 
        [GObject.ptr, :string, GValue.ptr], :void

    class GParamSpecPtr < FFI::Struct
        layout :value, GParamSpec.ptr
    end

end

module Libvips
    extend FFI::Library
    ffi_lib 'vips'

    attach_function :vips_init, [:string], :int

    attach_function :vips_error_buffer, [], :string
    attach_function :vips_error_clear, [], :void

    attach_function :vips_enum_from_nick, [:string, :GType, :string], :int
    attach_function :vips_enum_nick, [:GType, :int], :string

    attach_function :vips_value_set_array_double, 
        [Libgobject::GValue.ptr, [:double], :int], :void


    void vips_value_set_array_double (GValue* value, 
                                              const double* array, int n );
        void vips_value_set_array_int (GValue* value, 
                                               const int* array, int n );
            void vips_value_set_array_image (GValue *value, int n);
                void vips_value_set_blob (GValue* value,
                                                      void (*free_fn)(void* data), void* data, size_t length);


    class VipsObject < FFI::Struct
        # don't actually need most of these, remove them later
        layout :parent_object, Libgobject::GObject,
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

    class VipsObjectClass < FFI::Struct
        # opaque
    end

    class VipsArgument < FFI::Struct
        layout :pspec, Libgobject::GParamSpec.ptr
    end

    class VipsArgumentInstance < FFI::Struct
        layout :parent, VipsArgument
        # rest opaque
    end

    # enum VipsArgumentFlags 
    VIPS_ARGUMENT_NONE = 0
    VIPS_ARGUMENT_REQUIRED = 1
    VIPS_ARGUMENT_CONSTRUCT = 2
    VIPS_ARGUMENT_SET_ONCE = 4
    VIPS_ARGUMENT_SET_ALWAYS = 8
    VIPS_ARGUMENT_INPUT = 16
    VIPS_ARGUMENT_OUTPUT = 32
    VIPS_ARGUMENT_DEPRECATED = 64
    VIPS_ARGUMENT_MODIFY = 128

    class VipsArgumentClass < FFI::Struct
        layout :parent, VipsArgument,
               :object_class, VipsObjectClass.ptr,
               :flags, :uint,
               :priority, :int,
               :offset, :ulong_long
    end

    class GParamSpecPtr < FFI::Struct
        layout :value, Libgobject::GParamSpec.ptr
    end

    class VipsArgumentClassPtr < FFI::Struct
        layout :value, VipsArgumentClass.ptr
    end

    class VipsArgumentInstancePtr < FFI::Struct
        layout :value, VipsArgumentInstance.ptr
    end

    attach_function :vips_object_get_argument, 
        [VipsObject.ptr, :string, 
         Libgobject::GParamSpecPtr.ptr, VipsArgumentClassPtr.ptr, 
         VipsArgumentInstancePtr.ptr], :int

    attach_function :vips_object_print_all, [], :void

    class VipsImage < FFI::Struct
        layout :parent, VipsObject
    end

end

if Libvips.vips_init($0) != 0
    puts "unable to init vips: #{Libvips.vips_error_buffer}"
    Libvips.vips_error_clear
    exit -1
end

x = Libc.malloc 1000
puts "x = #{x}"


