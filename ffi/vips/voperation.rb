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

        def self.call(name, option_string, supplied)
            op = new_from_name name
            throw get_error if op == nil

            # find and classify all the arguments the operator can take
            args = get_construct_args
            required_input = [] 
            optional_input = {}
            required_output = [] 
            optional_output = {}
            args.each do |name, flags|
                next if (flags & ARGUMENT_DEPRECATED) != 0

                if (flags & ARGUMENT_INPUT) != 0 
                    if (flags & ARGUMENT_REQUIRED) != 0 and
                        required_input << [name, flags]
                    else
                        optional_input[name] = flags
                    end
                end

                if (flags & ARGUMENT_OUTPUT) != 0 
                    if (flags & ARGUMENT_REQUIRED) != 0 and
                        required_output << [name, flags]
                    else
                        optional_output[name] = flags
                    end
                end
            end

            # so we should have been supplied with n_required_input values, or
            # n_required_input + 1 if there's a hash of options at the end
            if supplied.length == required_input.length 
                supplied_optional = nil
            elsif supplied.length == required_input.length + 1 and
                supplied.last.is_a? Hash
                supplied_optional = supplied.last
                supplied.delete_at -1
            else
                throw "unable to call #{name}: " + 
                    "you supplied #{supplied.length} arguments, " +
                    "but operation needs #{required_input.length}."
            end

            # FIXME .. need to verify that all supplied_optional keys are in
            # either optional_input or optional_output

            # the first image arg is the thing we expand constants to match ...
            # we need to find it
            #
            # look inside array and hash arguments, since we may be passing an
            # array of images
            match_image = find_inside(supplied) do |value|
                value.is_a VipsImage
            end

            # set any string args first so they can't be overridden
            if option_string != nil
                if Vips::vips_object_set_from_string(op, option_string) != 0
                    throw "unable to call #{name}: #{Vips::get_error}"
                end
            end

            # set all required inputs
            (0...required_input.length) do |i|
                arg_name = required_input[i][0]
                flags = required_input[i][1]
                value = supplied[i]

                # FIXME ... need to use match_image
                # FIXME ... need to use flags
                op.set arg_name, value
            end

            # set all optional inputs
            if supplied_optional
                supplied_optional.each do |arg_name, value|
                    if optional_input.has_key? arg_name
                        flags = optional_input[arg_name]

                        # FIXME ... need to use match_image
                        # FIXME ... need to use flags for MODIFY etc.
                        op.set arg_name, value
                    end
                end
            end

            # build and adjust counts
            op2 = Vips::vips_cache_operation_build op 
            if op2 == nil
                throw "unable to call #{name}: #{Vips::get_error}"
            end
            op2 = VipsOperation.new op2
            op = op2
            op2 = nil

            # get all required results, then all optional ones
            # FIXME ... need to fetch modified inputs too
            result = []
            required_output.each do |arg_name, flags|
                result << op.get arg_name
            end

            optional_results = {}
            if supplied_optional
                supplied_optional.each do |arg_name, value|
                    if optional_output.has_key? arg_name
                        flags = optional_output[arg_name]

                        optional_results[arg_name] = op.get arg_name
                    end
                end
            end

            result << optional_results if optional_results != {}

            if result.length == 1
                result = result.first
            elsif result.length == 0
                result = nil
            end

            return result
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

    attach_function :vips_cache_operation_build, [:pointer], :pointer

end
