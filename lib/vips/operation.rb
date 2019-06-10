# This module provides an interface to the top level bits of libvips
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

module Vips
  private

  attach_function :vips_operation_new, [:string], :pointer

  attach_function :vips_cache_operation_build, [:pointer], :pointer
  attach_function :vips_object_unref_outputs, [:pointer], :void

  callback :argument_map_fn, [:pointer,
                              GObject::GParamSpec.ptr,
                              ArgumentClass.ptr,
                              ArgumentInstance.ptr,
                              :pointer, :pointer], :pointer
  attach_function :vips_argument_map, [:pointer,
                                       :argument_map_fn,
                                       :pointer, :pointer], :pointer

  OPERATION_SEQUENTIAL = 1
  OPERATION_NOCACHE = 4
  OPERATION_DEPRECATED = 8

  OPERATION_FLAGS = {
      :sequential => OPERATION_SEQUENTIAL,
      :nocache => OPERATION_NOCACHE,
      :deprecated => OPERATION_DEPRECATED
  }

  attach_function :vips_operation_get_flags, [:pointer], :int

  class Operation < Object

    # the layout of the VipsOperation struct
    module OperationLayout
      def self.included base
        base.class_eval do
          layout :parent, Object::Struct
          # rest opaque
        end
      end
    end

    class Struct < Object::Struct
      include OperationLayout

    end

    class ManagedStruct < Object::ManagedStruct
      include OperationLayout

    end

    def initialize value
      # allow init with a pointer so we can wrap the return values from
      # things like _build
      if value.is_a? String
        value = Vips::vips_operation_new value
        raise Vips::Error if value == nil
      end

      super value
    end

    def build
      op = Vips::vips_cache_operation_build self
      if op == nil
        raise Vips::Error
      end

      return Operation.new op
    end

    def argument_map &block
      fn = Proc.new do |op, pspec, argument_class, argument_instance, a, b|
        block.call pspec, argument_class, argument_instance
      end

      Vips::vips_argument_map self, fn, nil, nil
    end

    def get_flags
      Vips::vips_operation_get_flags self
    end

    # not quick! try to call this infrequently
    def get_construct_args
      args = []

      argument_map do |pspec, argument_class, argument_instance|
        flags = argument_class[:flags]
        if (flags & ARGUMENT_CONSTRUCT) != 0
          # names can include - as punctuation, but we always use _ in
          # Ruby
          name = pspec[:name].tr("-", "_")

          args << [name, flags]
        end
      end

      return args
    end

    # search array for the first element to match a predicate ...
    # search inside subarrays and sub-hashes
    def self.find_inside object, &block
      return object if block.call object

      if object.is_a? Enumerable
        object.find {|value| block.call value, block}
      end

      return nil
    end

    # expand a constant into an image
    def self.imageize match_image, value
      return value if value.is_a? Image

      # 2D array values become tiny 2D images
      # if there's nothing to match to, we also make a 2D image
      if (value.is_a?(Array) && value[0].is_a?(Array)) ||
          match_image == nil
        return Image.new_from_array value
      else
        # we have a 1D array ... use that as a pixel constant and
        # expand to match match_image
        return match_image.new_from_image value
      end
    end

    # set an operation argument, expanding constants and copying images as
    # required
    def set name, value, match_image = nil, flags = 0
      gtype = get_typeof name

      if gtype == IMAGE_TYPE
        value = Operation::imageize match_image, value

        if (flags & ARGUMENT_MODIFY) != 0
          # make sure we have a unique copy
          value = value.copy.copy_memory
        end
      elsif gtype == ARRAY_IMAGE_TYPE
        value = value.map {|x| Operation::imageize match_image, x}
      end

      super name, value
    end

    public

    # This is the public entry point for the vips binding. {call} will run
    # any vips operation, for example:
    #
    # ```ruby
    # out = Vips::Operation.call "black", [100, 100], {:bands => 12}
    # ```
    #
    # will call the C function
    #
    # ```C
    # vips_black( &out, 100, 100, "bands", 12, NULL );
    # ```
    #
    # There are {Image#method_missing} hooks which will run {call} for you
    # on {Image} for undefined instance or class methods. So you can also
    # write:
    #
    # ```ruby
    # out = Vips::Image.black 100, 100, bands: 12
    # ```
    #
    # Or perhaps:
    #
    # ```ruby
    # x = Vips::Image.black 100, 100
    # y = x.invert
    # ```
    #
    # to run the `vips_invert()` operator.
    #
    # There are also a set of operator overloads and some convenience
    # functions, see {Image}.
    #
    # If the operator needs a vector constant, {call} will turn a scalar
    # into a
    # vector for you. So for `x.linear a, b`, which calculates
    # `x * a + b` where `a` and `b` are vector constants, you can write:
    #
    # ```ruby
    # x = Vips::Image.black 100, 100, bands: 3
    # y = x.linear 1, 2
    # y = x.linear [1], 4
    # y = x.linear [1, 2, 3], 4
    # ```
    #
    # or any other combination. The operator overloads use this facility to
    # support all the variations on:
    #
    # ```ruby
    # x = Vips::Image.black 100, 100, bands: 3
    # y = x * 2
    # y = x + [1,2,3]
    # y = x % [1]
    # ```
    #
    # Similarly, wherever an image is required, you can use a constant. The
    # constant will be expanded to an image matching the first input image
    # argument. For example, you can write:
    #
    # ```
    # x = Vips::Image.black 100, 100, bands: 3
    # y = x.bandjoin 255
    # ```
    #
    # to add an extra band to the image where each pixel in the new band has
    # the constant value 255.

    def self.call name, supplied, optional = {}, option_string = ""
      GLib::logger.debug("Vips::VipsOperation.call") {
        "name = #{name}, supplied = #{supplied}, " +
        "optional = #{optional}, option_string = #{option_string}"
      }

      op = Operation.new name

      # find and classify all the arguments the operator can take
      args = op.get_construct_args
      required_input = []
      optional_input = {}
      required_output = []
      optional_output = {}
      args.each do |name, flags|
        next if (flags & ARGUMENT_DEPRECATED) != 0

        if (flags & ARGUMENT_INPUT) != 0
          if (flags & ARGUMENT_REQUIRED) != 0
            required_input << [name, flags]
          else
            optional_input[name] = flags
          end
        end

        # MODIFY INPUT args count as OUTPUT as well
        if (flags & ARGUMENT_OUTPUT) != 0 ||
            ((flags & ARGUMENT_INPUT) != 0 &&
             (flags & ARGUMENT_MODIFY) != 0)
          if (flags & ARGUMENT_REQUIRED) != 0
            required_output << [name, flags]
          else
            optional_output[name] = flags
          end
        end

      end

      # so we should have been supplied with n_required_input values, or
      # n_required_input + 1 if there's a hash of options at the end
      unless supplied.is_a? Array
        raise Vips::Error, "unable to call #{name}: " +
            "argument array is not an array"
      end
      unless optional.is_a? Hash
        raise Vips::Error, "unable to call #{name}: " +
            "optional arguments are not a hash"
      end
      if supplied.length != required_input.length
        raise Vips::Error, "unable to call #{name}: " +
            "you supplied #{supplied.length} arguments, " +
            "but operation needs #{required_input.length}."
      end

      # very that all supplied_optional keys are in optional_input or
      # optional_output
      optional.each do |key, value|
        arg_name = key.to_s

        unless optional_input.has_key?(arg_name) ||
            optional_output.has_key?(arg_name)
          raise Vips::Error, "unable to call #{name}: " +
              "unknown option #{arg_name}"
        end
      end

      # the first image arg is the thing we expand constants to match ...
      # we need to find it
      #
      # look inside array and hash arguments, since we may be passing an
      # array of images
      match_image = find_inside(supplied) do |value|
        value.is_a? Image
      end

      # set any string args first so they can't be overridden
      if option_string != nil
        if Vips::vips_object_set_from_string(op, option_string) != 0
          raise Vips::Error
        end
      end

      # set all required inputs
      required_input.each_index do |i|
        arg_name = required_input[i][0]
        flags = required_input[i][1]
        value = supplied[i]

        op.set arg_name, value, match_image, flags
      end

      # set all optional inputs
      optional.each do |key, value|
        next if value.nil?

        arg_name = key.to_s

        if optional_input.has_key? arg_name
          flags = optional_input[arg_name]

          op.set arg_name, value, match_image, flags
        end
      end

      op = op.build

      # get all required results
      result = []
      required_output.each do |arg_name, flags|
        result << op.get(arg_name)
      end

      # fetch all optional ones
      optional_results = {}
      optional.each do |key, value|
        arg_name = key.to_s

        if optional_output.has_key? arg_name
          flags = optional_output[arg_name]

          optional_results[arg_name] = op.get arg_name
        end
      end

      result << optional_results if optional_results != {}

      if result.length == 1
        result = result.first
      elsif result.length == 0
        result = nil
      end

      GLib::logger.debug("Vips::Operation.call") {"result = #{result}"}

      Vips::vips_object_unref_outputs op

      return result
    end

  end

end
