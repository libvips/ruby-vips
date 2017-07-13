# This module provides an interface to the top level bits of GLib
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
    extend FFI::Library
    ffi_lib 'vips'

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

end
