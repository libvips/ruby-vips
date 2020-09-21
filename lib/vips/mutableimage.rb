# This module provides an interface to the vips image processing library
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'
require 'forwardable'

module Vips
  # This class represents a libvips image. See the {Vips} module documentation
  # for an introduction to using this class.
  class MutableImage < Vips::Object
    extend Forwardable
    alias_method :parent_get_typeof, :get_typeof
    def_delegators :@image, :width, :height, :bands, :format, :interpretation, 
      :filename, :xoffset, :yoffset, :xres, :yres, :size, :get, :get_typeof,
      :get_fields

    private

    # the layout of the VipsImage struct
    module ImageLayout
      def self.included base
        base.class_eval do
          layout :parent, Vips::Object::Struct
          # rest opaque
        end
      end
    end

    class Struct < Vips::Object::Struct
      include ImageLayout
    end

    class ManagedStruct < Vips::Object::ManagedStruct
      include ImageLayout
    end

    class GenericPtr < FFI::Struct
      layout :value, :pointer
    end

    def image= image
      @image = image
    end

    public

    # get the #Image this #MutableImage is modifying. Only use this once you
    # have finished all modifications.
    def image 
      @image
    end

    # make a mutable image from an image ... copy the VipsImage so that we
    # have a unique image to modify
    def self.new_from_image image
      new_image = image.copy
      mutable = Vips::MutableImage.new new_image.pointer
      mutable.image = image 
      mutable
    end

    def inspect
      "#<Image #{width}x#{height} #{format}, #{bands} bands, #{interpretation}>"
    end

    def respond_to? name, include_all = false
      # To support keyword args, we need to tell Ruby that final image
      # arguments cannot be hashes of keywords.
      #
      # https://makandracards.com/makandra/
      #   36013-heads-up-ruby-implicitly-converts-a-hash-to-keyword-arguments
      return false if name == :to_hash

      super
    end

    def respond_to_missing? name, include_all = false
      # respond to all vips operations by nickname
      return true if Vips::type_find("VipsOperation", name.to_s) != 0

      super
    end

    def self.respond_to_missing? name, include_all = false
      # respond to all vips operations by nickname
      return true if Vips::type_find("VipsOperation", name.to_s) != 0

      super
    end

    # Invoke a vips operation with {Vips::Operation.call}, using self as
    # the first input argument.
    #
    # @param name [String] vips operation to call
    # @return result of vips operation
    def method_missing name, *args, **options
      Vips::Operation.call name.to_s, [self, *args], options
    end

    # Create a metadata item on an image of the specifed type. Ruby types
    # are automatically transformed into the matching `GType`, if possible.
    #
    # For example, you can use this to set an image's ICC profile:
    #
    # ```
    # x = y.set_type Vips::BLOB_TYPE, "icc-profile-data", profile
    # ```
    #
    # where `profile` is an ICC profile held as a binary string object.
    #
    # @see set
    # @param gtype [Integer] GType of item
    # @param name [String] Metadata field to set
    # @param value [Object] Value to set
    def set_type! gtype, name, value
      gvalue = GObject::GValue.alloc
      gvalue.init gtype
      gvalue.set value
      Vips::vips_image_set self, name, gvalue
      gvalue.unset
    end

    # Set the value of a metadata item on an image. The metadata item must
    # already exist. Ruby types are automatically transformed into the
    # matching `GValue`, if possible.
    #
    # For example, you can use this to set an image's ICC profile:
    #
    # ```
    # x = y.set "icc-profile-data", profile
    # ```
    #
    # where `profile` is an ICC profile held as a binary string object.
    #
    # @see set_type
    # @param name [String] Metadata field to set
    # @param value [Object] Value to set
    def set! name, value
      set_type get_typeof(name), name, value
    end

    # Remove a metadata item from an image.
    #
    # @param name [String] Metadata field to remove
    def remove! name
      Vips::vips_image_remove self, name
    end

  end
end
