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

    # we have a set of things we need to inherit in different ways:
    #
    # - we want to be able to subclass GObject in a simple way
    # - the layouts of the nested structs
    # - casting between structs which share a base
    # - gobject refcounting
    #
    # the solution is to split the class into four areas which we treat
    # differently:
    #
    # - we have a "wrapper" Ruby class to allow easy subclassing ... this has a
    #   @struct member which holds the actual pointer
    # - we use "forwardable" to forward the various ffi methods on to the
    #   @struct member ... we arrange things so that subclasses do not need to
    #   do the forwarding themselves
    # - we have two versions of the struct: a plain one which we can use for
    #   casting that will not change the refcounts
    # - and a managed one with an unref which we just use for .new
    # - we separate the struct layout into a separate module to avoid repeating
    #   ourselves

    class GObject
        extend Forwardable
        extend SingleForwardable

        def_instance_delegators :@struct, :[], :to_ptr
        def_single_delegators :ffi_struct, :ptr

        # the layout of the GObject struct
        module GObjectLayout
            def self.included(base)
                base.class_eval do
                    layout :g_type_instance, :pointer,
                           :ref_count, :uint,
                           :qdata, :pointer
                end
            end
        end

        # the struct with unref ... manage object lifetime with this
        class ManagedStruct < FFI::ManagedStruct
            include GObjectLayout

            def initialize(ptr)
                log "GLib::GObject::ManagedStruct.new: #{ptr}"
                super
            end

            def self.release(ptr)
                log "GLib::GObject::ManagedStruct.release: unreffing #{ptr}"
                GLib::g_object_unref(ptr) unless ptr.null?
            end
        end

        # the plain struct ... cast with this
        class Struct < FFI::Struct
            include GObjectLayout

            def initialize(ptr)
                log "GLib::GObject::Struct.new: #{ptr}"
                super
            end

        end

        # don't allow ptr == nil, we never want to allocate a GObject struct
        # ourselves, we just want to wrap GLib-allocated GObjects
        #
        # here we use ManagedStruct, not Struct, since this is the ref that will
        # need the unref
        def initialize(ptr)
            log "GLib::GObject.initialize: ptr = #{ptr}"
            @struct = ffi_managed_struct.new(ptr)
        end

        # access to the casting struct for this class
        def ffi_struct
            self.class.ffi_struct
        end

        class << self
            def ffi_struct
                self.const_get(:Struct)
            end
        end

        # access to the lifetime managed struct for this class
        def ffi_managed_struct
            self.class.ffi_managed_struct
        end

        class << self
            def ffi_managed_struct
                self.const_get(:ManagedStruct)
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

    VIPS_ARGUMENT_FLAGS = {
        :none => VIPS_ARGUMENT_NONE,
        :required => VIPS_ARGUMENT_REQUIRED,
        :construct => VIPS_ARGUMENT_CONSTRUCT,
        :set_once => VIPS_ARGUMENT_SET_ONCE,
        :set_always => VIPS_ARGUMENT_SET_ALWAYS,
        :input => VIPS_ARGUMENT_INPUT,
        :output => VIPS_ARGUMENT_OUTPUT,
        :deprecated => VIPS_ARGUMENT_DEPRECATED,
        :modify => VIPS_ARGUMENT_MODIFY
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

    class VipsOperation < VipsObject

        # the layout of the VipsImage struct
        module VipsOperationLayout
            def self.included(base)
                base.class_eval do
                    layout :parent, VipsObject::Struct
                    # rest opaque
                end
            end
        end

        class Struct < VipsObject::Struct
            include VipsOperationLayout

            def initialize(ptr)
                log "Vips::VipsOperation::Struct.new: #{ptr}"
                super
            end

        end

        class ManagedStruct < VipsObject::ManagedStruct
            include VipsOperationLayout

            def initialize(ptr)
                log "Vips::VipsOperation::ManagedStruct.new: #{ptr}"
                super
            end

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

        # not quick! try to call this infrequently
        def get_construct_args
            args = []

            argument_map do |pspec, argument_class, argument_instance|
                flags = argument_class[:flags]
                if (flags & VIPS_ARGUMENT_CONSTRUCT) != 0 
                    args << [pspec[:name], flags] 
                end
            end

            args
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

    attach_function :vips_operation_new, [:string], :pointer

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
            VipsImage.new(Vips::vips_image_new)
        end

    end

    attach_function :vips_image_new, [], :pointer

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
puts "x[:parent] = #{x[:parent]}"
puts "x[:parent][:description] = #{x[:parent][:description]}"
x = nil
GC.start
Vips::showall
puts ""

def show_flags(table, flags)
    set = []
    table.each do |key, value|
        set << key.to_s if (flags & value) != 0
    end

    set.join " " 
end

puts "creating operation"
x = Vips::VipsOperation.new_from_name "perlin"
puts "x = #{x}"
x.argument_map do |pspec, argument_class, argument_instance|
    puts "in arg_map fn"
    puts "   pspec[:name] = #{pspec[:name]}"
    puts "   argument_class = #{argument_class}"
    puts "   argument_instance = #{argument_instance}"
    puts "   flags = #{show_flags(Vips::VIPS_ARGUMENT_FLAGS, argument_class[:flags])}"
end
args = x.get_construct_args
puts "x.get_construct_args = #{args}"
args.each do |name, flags| 
    puts "#{name} = #{flags}"
end
x.set("width", 100)
x.set("height", 100)
x = nil
puts ""

