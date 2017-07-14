# This module provides an interface to the top level bits of GLib
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips

    class VipsOperation < VipsObject

        # the layout of the VipsOperation struct
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

        # search array for the first element to match a predicate ...
        # search inside subarrays and sub-hashes
        def self.find_inside(object, &block)
            return object if block.call(object)

            if object.is_a? Enumerable
                object.each do |value|
                    result = block.call(value, block)
                    return result if result != nil
                end
            end

            return nil
        end

        def self.call(name, option_string, supplied_values)
            op = new_from_name name
            throw get_error if op == nil

            args = get_construct_args

            # count required input args
            n_required_input = 0 
            args.each do |name, flags|
                if (flags & ARGUMENT_REQUIRED) != 0 and
                    (flags & ARGUMENT_INPUT) != 0 and
                    (flags & ARGUMENT_DEPRECATED) == 0 
                    n_required_input += 1 
                end
            end

            # so we should have been supplied with n_required_input values, or
            # n_required_input + 1 if there's a hash of options at the end
            if supplied_values.length == n_required_input 
                optional_values = nil
            elsif supplied_values.length == n_required_input + 1 and
                supplied_values.last.is_a? Hash
                optional_values = supplied_values.last
                supplied_values.delete_at -1
            else
                throw "unable to call #{name}: " + 
                    "you supplied #{supplied_values.length} arguments, " +
                    "but operation needs #{n_required_input}."
            end

            # the first image arg is the thing we expand constants to match ...
            # we need to find it
            #
            # look inside array and hash arguments, since we may be passing an
            # array of images
            match_image = find_inside(supplied_values) do |value|
                value.is_a VipsImage
            end

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
