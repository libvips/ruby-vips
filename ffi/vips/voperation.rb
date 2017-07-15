# This module provides an interface to the top level bits of GLib
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips

    attach_function :vips_operation_new, [:string], :pointer

    attach_function :vips_cache_operation_build, [:pointer], :pointer
    attach_function :vips_object_unref_outputs, [:pointer], :void

    class VipsOperation < VipsObject

        # the layout of the VipsOperation struct
        module VipsOperationLayout
            def self.included base
                base.class_eval do
                    layout :parent, VipsObject::Struct
                    # rest opaque
                end
            end
        end

        class Struct < VipsObject::Struct
            include VipsOperationLayout

            def initialize ptr
                log "Vips::VipsOperation::Struct.new: #{ptr}"
                super
            end

        end

        class ManagedStruct < VipsObject::ManagedStruct
            include VipsOperationLayout

            def initialize ptr
                log "Vips::VipsOperation::ManagedStruct.new: #{ptr}"
                super
            end

        end

        def self.new_from_name name
            VipsOperation.new Vips::vips_operation_new(name)
        end

        def build 
            op = Vips::vips_cache_operation_build self 
            if op == nil
                raise Vips::Error
            end

            return VipsOperation.new(op)
        end

        def argument_map &block
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
                if (flags & ARGUMENT_CONSTRUCT) != 0 
                    args << [pspec[:name], flags] 
                end
            end

            return args
        end

        # search array for the first element to match a predicate ...
        # search inside subarrays and sub-hashes
        def self.find_inside(object, &block)
            return object if block.call(object)

            if object.is_a? Enumerable
                object.each do |value|
                    return value if block.call(value, block)
                end
            end

            return nil
        end

        # expand a constant into an image
        def self.imageize match_image, value
            return value if match_image == nil
            return value if value.is_a? Vips::VipsImage

            # 2D array values become tiny 2D images
            if value.is_a? Array and value[0].is_a? Array
                return Vips::VipsImage.new_from_array value
            end

            # if there's nothing to match to, we also make a 2D image
            if match_image == nil
                return Vips::VipsImage.new_from_array value
            end

            # we have a 1D array ... use that as a pixel constant and
            # expand to match match_image
            match_image.new_from_image(value)
        end

        # set an operation argument, expanding constants and copying images as
        # required
        def set name, value, match_image = nil, flags = 0
            if match_image 
                gtype = get_typeof name

                if gtype == Vips::IMAGE_TYPE 
                    value = imageize match_image, value
                elsif gtype == Vips::IMAGE_ARRAY_TYPE
                    value = value.map {|x| imageize match_image, x}
                end
            end

            if (flags & ARGUMENT_MODIFY) != 0
                # make sure we have a unique copy
                value = value.copy.copy_memory
            end

            super name, value
        end

        def self.call name, supplied, option_string = ""
            log "Vips::VipsOperation.call: name = #{name}, " + 
                "supplied = #{supplied} option_string = #{option_string}"

            op = new_from_name name
            raise Vips::Error if op == nil

            # find and classify all the arguments the operator can take
            log "Vips::VipsOperation.call: analyzing args..."
            args = op.get_construct_args
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

                # MODIFY INPUT args count as OUTPUT as well
                if (flags & ARGUMENT_OUTPUT) != 0 or
                    ((flags & ARGUMENT_INPUT) != 0 and
                     (flags & ARGUMENT_MODIFY) != 0)
                    if (flags & ARGUMENT_REQUIRED) != 0 and
                        required_output << [name, flags]
                    else
                        optional_output[name] = flags
                    end
                end

            end

            # so we should have been supplied with n_required_input values, or
            # n_required_input + 1 if there's a hash of options at the end
            log "Vips::VipsOperation.call: checking supplied args ..."
            if not supplied.is_a? Array
                raise Vips::Error, "unable to call #{name}: " + 
                    "argument array is not an array"
            elsif supplied.length == required_input.length 
                supplied_optional = nil
            elsif supplied.length == required_input.length + 1 and
                supplied.last.is_a? Hash
                supplied_optional = supplied.last
                supplied.delete_at -1
            else
                raise Vips::Error, "unable to call #{name}: " + 
                    "you supplied #{supplied.length} arguments, " +
                    "but operation needs #{required_input.length}."
            end

            # very that all supplied_optional keys are in optional_input or
            # optional_output
            if supplied_optional
                supplied_optional.each do |key, value|
                    if not optional_input.has_key? key and
                        not optional_output.has_key? key 
                        raise Vips::Error, "unable to call #{name}: " + 
                            "unknown option #{key}"
                    end
                end
            end

            # the first image arg is the thing we expand constants to match ...
            # we need to find it
            #
            # look inside array and hash arguments, since we may be passing an
            # array of images
            match_image = find_inside(supplied) do |value|
                value.is_a? VipsImage
            end

            log "Vips::VipsOperation.call: match_image = #{match_image}"

            # set any string args first so they can't be overridden
            if option_string != nil
                log "Vips::VipsOperation.call: setting string args ..."
                if Vips::vips_object_set_from_string(op, option_string) != 0
                    raise Vips::Error
                end
            end

            # set all required inputs
            log "Vips::VipsOperation.call: setting required inputs ..."
            required_input.each_index do |i|
                arg_name = required_input[i][0]
                flags = required_input[i][1]
                value = supplied[i]

                op.set arg_name, value, match_image, flags
            end

            # set all optional inputs
            if supplied_optional
                log "Vips::VipsOperation.call: setting optional inputs ..."
                supplied_optional.each do |arg_name, value|
                    if optional_input.has_key? arg_name
                        flags = optional_input[arg_name]

                        op.set arg_name, value, matgch_image, flags
                    end
                end
            end

            log "Vips::VipsOperation.call: building ..."
            op = op.build

            # get all required results, then all optional ones
            log "Vips::VipsOperation.call: fetching output ..."
            result = []
            required_output.each do |arg_name, flags|
                result << op.get(arg_name)
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

            log "Vips::VipsOperation.call: result = #{result}"

            Vips::vips_object_unref_outputs op

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


end
