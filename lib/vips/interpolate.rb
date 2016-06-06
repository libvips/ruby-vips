module Vips

    # An interpolator. One of these can be given to operations like 
    # {Vips::affine} or {Vips::mapim} to select the type of pixel interpolation
    # to use.
    #
    # To see all interpolators supported by your
    # libvips, try
    #
    # ```
    # $ vips -l interpolate
    # ```
    #
    # But at least these should be available:
    #
    # *   `:nearest` Nearest-neighbour interpolation.
    # *   `:bilinear` Bilinear interpolation.
    # *   `:bicubic` Bicubic interpolation.
    # *   `:lbb` Reduced halo bicubic interpolation.
    # *   `:nohalo` Edge sharpening resampler with halo reduction.
    # *   `:vsqbs` B-Splines with antialiasing smoothing.
    #
    #  For example:
    #
    #  ```ruby
    #  im = im.affine :interpolate => Vips::Interpolate.new :bicubic
    #  ```

    class Interpolate

        # @!method self.new(name, opts = {})
        #   @param name [Symbol] interpolator to create
        #   @param [Hash] opts Set of options
        #   @return [Interpolate] constructed interpolator

    end
end
