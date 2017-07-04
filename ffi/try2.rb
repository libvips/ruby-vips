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

    class GValue < FFI::Struct
        layout :gtype, :GType, 
               :data, [:ulong_long, 2]

        def self.release(ptr)
            Libgobject::g_value_unset(ptr) unless ptr.null?
        end

        def init(gtype)
            Libgobject::g_value_init(self, gtype)
        end

        def set(value)
            gtype = self[:gtype]
            fundamental = Libgobject::g_type_fundamental gtype

            case gtype
            when GBOOL_TYPE
                Libgobject::g_value_set_boolean self, (value ? 1 : 0)
            when GINT_TYPE
                Libgobject::g_value_set_int self, value
            when GDOUBLE_TYPE
                Libgobject::g_value_set_double self, value
            when GSTR_TYPE
                # set_string takes a copy, no lifetime worries
                Libgobject::g_value_set_string self, value
            when Libvips::IMAGE_TYPE
                # this will add a ref, held by the gvalue ... it will be 
                # dropped by g_value_unset() when the gvalue is GC'd
                Libgobject::g_value_set_object self, value
            when Libvips::ARRAY_INT_TYPE
            when Libvips::ARRAY_DOUBLE_TYPE
            when Libvips::ARRAY_IMAGE_TYPE
            when Libvips::BLOB_TYPE
            else
                case fundamental
                when GFLAGS_TYPE
                    Libgobject::g_value_set_flags self, value
                when GENUM_TYPE
                    if value.is_a? Symbol
                        value = value.to_s
                    end

                    if value.is_a? String
                        value = Libvips::vips_enum_from_nick "ruby-vips", 
                            self[:gtype], value
                        if value == -1
                            throw Libvips::get_error
                        end
                    end

                    Libgobject::g_value_set_enum self, value
                else
                    puts "unimplemented gtype for set: #{gtype}"
                end
            end
        end

        def get
            gtype = self[:gtype]
            fundamental = Libgobject::g_type_fundamental gtype

            case gtype
            when GBOOL_TYPE
                Libgobject::g_value_get_boolean(self) != 0 ? true : false
            when GINT_TYPE
                Libgobject::g_value_get_int(self)
            when GDOUBLE_TYPE
                Libgobject::g_value_get_double(self)
            when GSTR_TYPE
                # FIXME do we need to strdup here?
                Libvips::g_value_get_string self
            when Libvips::IMAGE_TYPE
                # FIXME ... cast to VipsImage
                Libvips::g_value_get_object self
            when Libvips::ARRAY_INT_TYPE
            when Libvips::ARRAY_DOUBLE_TYPE
            when Libvips::ARRAY_IMAGE_TYPE
            when Libvips::BLOB_TYPE
            else
                case fundamental
                when GFLAGS_TYPE
                    Libgobject::g_value_get_flags(self)
                when GENUM_TYPE
                    enum_value = Libgobject::g_value_get_enum(self)
                    # this returns a static string, no need to worry about 
                    # lifetime
                    enum_name = Libvips::vips_enum_nick self[:gtype], enum_value
                    if enum_name == nil
                        throw Libvips::get_error
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

    # need to repeat this
    typedef :ulong, :GType

    attach_function :vips_init, [:string], :int

    attach_function :vips_error_buffer, [], :string
    attach_function :vips_error_clear, [], :void

    def Libvips::get_error
        errstr = Libvips::vips_error_buffer
        Libvips::vips_error_clear
        errstr
    end

    if Libvips::vips_init($0) != 0
        puts Libvips::get_error
        exit 1
    end

    # some handy gtypes
    IMAGE_TYPE = Libgobject::g_type_from_name("VipsImage")
    ARRAY_INT_TYPE = Libgobject::g_type_from_name("VipsArrayInt")
    ARRAY_DOUBLE_TYPE = Libgobject::g_type_from_name("VipsArrayDouble")
    ARRAY_IMAGE_TYPE = Libgobject::g_type_from_name("VipsArrayImage")
    REFSTR_TYPE = Libgobject::g_type_from_name("VipsRefString")
    BLOB_TYPE = Libgobject::g_type_from_name("VipsBlob")

    attach_function :vips_enum_from_nick, [:string, :GType, :string], :int
    attach_function :vips_enum_nick, [:GType, :int], :string

    attach_function :vips_value_set_array_double, 
        [Libgobject::GValue.ptr, :pointer, :int], :void
    attach_function :vips_value_set_array_int, 
        [Libgobject::GValue.ptr, :pointer, :int], :void
    attach_function :vips_value_set_array_image, 
        [Libgobject::GValue.ptr, :int], :void
    callback :free_fn, [:pointer], :void
    attach_function :vips_value_set_blob, 
        [Libgobject::GValue.ptr, :free_fn, :pointer, :size_t], :void

    attach_function :vips_value_get_ref_string, 
        [Libgobject::GValue.ptr, :pointer], :string
    attach_function :vips_value_get_array_double, 
        [Libgobject::GValue.ptr, :pointer], :pointer
    attach_function :vips_value_get_array_int, 
        [Libgobject::GValue.ptr, :pointer], :pointer
    attach_function :vips_value_get_array_image, 
        [Libgobject::GValue.ptr, :pointer], :pointer
    attach_function :vips_value_get_blob, 
        [Libgobject::GValue.ptr, :pointer], :pointer

    # init by hand for testing 
    attach_function :vips_interpretation_get_type, [], :GType

    class VipsObject < Libgobject::GObject
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

        def get_typeof(name)
            pspec = Libgobject::GParamSpecPtr.new
            argument_class = Libvips::VipsArgumentClassPtr.new
            argument_instance = Libvips::VipsArgumentInstancePtr.new

            result = Libvips::vips_object_get_argument self, name,
                pspec, argument_class, argument_instance

            if result != 0 
                throw Libvips::get_error
            end

            pspec[:value][:value_type]
        end

        def get(name)
            gtype = get_typeof name
            gvalue = Libgobject::GValue.new 
            gvalue.init gtype
            Libgobject::g_object_get_property self, name, gvalue
            gvalue.get
        end

        def set(name, value)
            gtype = get_typeof name
            gvalue = Libgobject::GValue.new 
            gvalue.init gtype
            gvalue.set value
            Libgobject::g_object_set_property self, name, gvalue
        end

    end

    class VipsObjectClass < FFI::Struct
        # opaque
    end

    class VipsArgument < FFI::Struct
        layout :pspec, Libgobject::GParamSpec.ptr
    end

    class VipsArgumentInstance < VipsArgument
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

    attach_function :vips_object_get_argument, 
        [VipsObject.ptr, :string, 
         Libgobject::GParamSpecPtr.ptr, VipsArgumentClassPtr.ptr, 
         VipsArgumentInstancePtr.ptr], :int

    attach_function :vips_object_print_all, [], :void

    class VipsOperation < VipsObject
        # rest opaque
        layout :parent, VipsObject

        def self.new_from_name(name)
            VipsOperation.new (Libvips::vips_operation_new name)
        end

        def argument_map(&block)
            fn = Proc.new do |op, pspec, argument_class, argument_instance, a, b|
                # not sure why we have to cast these ... sad!
                pspec = Libgobject::GParamSpec.new pspec
                argument_class = Libvips::VipsArgumentClass.new argument_class
                argument_instance = 
                    Libvips::VipsArgumentInstance.new argument_instance

                block.call(pspec, argument_class, argument_instance)
            end

            Libvips::vips_argument_map(self, fn, nil, nil)
        end

    end

    callback :argument_map_fn, [VipsOperation, 
                                Libgobject::GParamSpec, 
                                VipsArgumentClass, 
                                VipsArgumentInstance, 
                                :pointer, :pointer], :pointer
    attach_function :vips_argument_map, [VipsOperation, 
                                         :argument_map_fn, 
                                         :pointer, :pointer], :pointer

    attach_function :vips_operation_new, [:string], VipsOperation

    class VipsImage < VipsObject
        # rest opaque
        layout :parent, VipsObject
    end

    attach_function :vips_image_new, [], VipsImage.ptr

end

x = Libc::malloc 1000
puts "x = #{x}"
puts ""

puts "creating gvalue with Libgobject::GValue"
x = Libgobject::GValue.new 
gtype = Libgobject::g_type_from_name "gboolean"
puts "Libgobject::g_type_from_name 'gboolean' = #{gtype}"
x.init gtype
x.set true
puts "x = #{x}"
puts "x.get = #{x.get}"
puts ""

puts "creating enum"
# need to init this by hand, it's not created normally until the first image is
# made
Libvips::vips_interpretation_get_type
x = Libgobject::GValue.new 
gtype = Libgobject::g_type_from_name "VipsInterpretation"
puts "Libgobject::g_type_from_name 'VipsInterpretation' = #{gtype}"
x.init gtype
x.set :lab
puts "x = #{x}"
puts "x.get = #{x.get}"
puts ""

puts "creating image"
x = Libvips::vips_image_new
puts "x = #{x}"
puts "x.get_typeof('width') = #{x.get_typeof('width')}"
puts "Libvips::GINT_TYPE = #{Libgobject::GINT_TYPE}"
puts "x.get('width') = #{x.get('width')}"
puts "x.set('width', 99)"
x.set('width', 99)
puts "x.get('width') = #{x.get('width')}"
puts "x[:parent][:description] = #{x[:parent][:description]}"
puts ""

puts "creating operation"
x = Libvips::VipsOperation.new_from_name "invert"
puts "x = #{x}"
x.argument_map do |pspec, argument_class, argument_instance|
    puts "in arg_map fn"
    puts "   pspec[:name] = #{pspec[:name]}"
    puts "   argument_class = #{argument_class}"
    puts "   argument_instance = #{argument_instance}"
end
puts ""

