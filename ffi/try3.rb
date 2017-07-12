#!/usr/bin/ruby

require 'ffi'
require 'forwardable'

# this is really very crude logging

# @private
$vips_debug = true

# @private
def log str
    if $vips_debug
        puts str
    end
end

def set_debug debug
    $vips_debug = debug
end

module Libc
    extend FFI::Library
    ffi_lib FFI::Library::LIBC

    attach_function :malloc, [:size_t], :pointer
    attach_function :free, [:pointer], :void
end

module GLib
    extend FFI::Library
    ffi_lib 'gobject-2.0'

    # nil being the default
    glib_log_domain = nil

    def self.set_log_domain(domain)
        glib_log_domain = domain
    end

    attach_function :g_malloc, [:size_t], :pointer
    attach_function :g_free, [:pointer], :void

    # instead of having a GObject inheriting directly from FFI::Struct, we have
    # a wrapper class with a @struct member ... this lets us inherit from
    # GObject correctly in Vips:: below
    class GObject
        extend Forwardable
        extend SingleForwardable

        def_instance_delegators :@struct, :[], :to_ptr
        def_single_delegators :ffi_struct, :ptr

        # A struct with unref ... inherit from this in subclasses 
        class Struct < FFI::ManagedStruct
            layout :g_type_instance, :pointer,
                   :ref_count, :uint,
                   :qdata, :pointer

            def self.release(ptr)
                log "GLib::GObject::Struct.release: unreffing #{ptr}"
                GLib::g_object_unref(ptr) unless ptr.null?
            end
        end

        # don't allow ptr == nil, we never want to allocate a GObject struct
        # ourselves, we just want to wrap GLib-allocated GObjects
        def initialize(ptr)
            puts "GLib::GObject.initialize: ptr = #{ptr}"
            pointer = FFI::Pointer.new Struct, ptr.pointer
            @struct = self.ffi_struct.new(pointer)
        end

        def ffi_struct
            self.class.ffi_struct
        end

        class << self
            def ffi_struct
                self.const_get(:Struct)
            end
        end

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

    # use :pointer rather than GObject.ptr to avoid casting later
    attach_function :g_object_set_property, 
        [:pointer, :string, GValue.ptr], :void
    attach_function :g_object_get_property, 
        [:pointer, :string, GValue.ptr], :void

    class GParamSpecPtr < FFI::Struct
        layout :value, GParamSpec.ptr
    end

end

module Vips
    extend FFI::Library
    ffi_lib 'vips'

    LOG_DOMAIN = "VIPS"
    GLib::set_log_domain(LOG_DOMAIN)

    # need to repeat this
    typedef :ulong, :GType

    attach_function :vips_init, [:string], :int
    attach_function :vips_shutdown, [], :void

    attach_function :vips_error_buffer, [], :string
    attach_function :vips_error_clear, [], :void

    def self.get_error
        errstr = Vips::vips_error_buffer
        Vips::vips_error_clear
        errstr
    end

    if Vips::vips_init($0) != 0
        puts Vips::get_error
        exit 1
    end

    at_exit {
        Vips::vips_shutdown
    }

    attach_function :vips_object_print_all, [], :void
    attach_function :vips_leak_set, [:int], :void

    def self.showall
        if $vips_debug
            GC.start
            vips_object_print_all
        end
    end

    if $vips_debug
        vips_leak_set 1
    end

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

        class Struct < GLib::GObject.ffi_struct
            # don't actually need most of these, remove them later
            layout :parent, GLib::GObject.ffi_struct, 
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
            gvalue.get
        end

        def set(name, value)
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

    # just use :pointer, not VipsObject.ptr, to avoid casting gobject
    # subclasses
    attach_function :vips_object_get_argument, 
        [:pointer, :string, 
         GLib::GParamSpecPtr.ptr, VipsArgumentClassPtr.ptr, 
         VipsArgumentInstancePtr.ptr], :int

    attach_function :vips_object_print_all, [], :void

    class VipsOperation < VipsObject

        class Struct < VipsObject.ffi_struct
            layout :parent, VipsObject.ffi_struct
            # rest opaque
        end

        def self.new_from_name(name)
            VipsOperation.new (Vips::vips_operation_new(name))
        end

        def argument_map(&block)
            fn = Proc.new do |op, pspec, argument_class, argument_instance, a, b|
                block.call(pspec, argument_class, argument_instance)
            end

            Vips::vips_argument_map(self, fn, nil, nil)
        end

    end

    callback :argument_map_fn, [:pointer,
                                GLib::GParamSpec.ptr, 
                                VipsArgumentClass.ptr, 
                                VipsArgumentInstance.ptr, 
                                :pointer, :pointer], :pointer
    attach_function :vips_argument_map, [:pointer,
                                         :argument_map_fn, 
                                         :pointer, :pointer], :pointer

    attach_function :vips_operation_new, [:string], VipsOperation.ptr

    class VipsImage < VipsObject

        class Struct < VipsObject.ffi_struct
            layout :parent, VipsObject.ffi_struct
            # rest opaque
        end

        def self.new_partial
            VipsImage.new(Vips::vips_image_new())
        end

    end

    attach_function :vips_image_new, [], VipsImage.ptr

end

puts "creating gvalue with GLib::GValue"
x = GLib::GValue.alloc 
gtype = GLib::g_type_from_name "gboolean"
puts "GLib::g_type_from_name 'gboolean' = #{gtype}"
x.init gtype
x.set true
puts "x = #{x}"
puts "x.get = #{x.get}"
puts ""

puts "creating enum"
# need to init this by hand, it's not created normally until the first image is
# made
Vips::vips_interpretation_get_type
x = GLib::GValue.alloc 
gtype = GLib::g_type_from_name "VipsInterpretation"
puts "GLib::g_type_from_name 'VipsInterpretation' = #{gtype}"
x.init gtype
x.set :lab
puts "x = #{x}"
puts "x.get = #{x.get}"
puts ""

puts "creating image"
x = Vips::VipsImage.new_partial
Vips::showall
puts "x = #{x}"
puts "x.get_typeof('width') = #{x.get_typeof('width')}"
puts "Vips::GINT_TYPE = #{GLib::GINT_TYPE}"
puts "x.get('width') = #{x.get('width')}"
puts "x.set('width', 99)"
x.set('width', 99)
puts "x.get('width') = #{x.get('width')}"
puts "x[:parent][:description] = #{x[:parent][:description]}"
x = nil
GC.start
Vips::showall
puts ""

puts "creating operation"
x = Vips::VipsOperation.new_from_name "invert"
puts "x = #{x}"
x.argument_map do |pspec, argument_class, argument_instance|
    puts "in arg_map fn"
    puts "   pspec[:name] = #{pspec[:name]}"
    puts "   argument_class = #{argument_class}"
    puts "   argument_instance = #{argument_instance}"
end
x = nil
puts ""

