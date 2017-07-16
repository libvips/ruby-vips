module Vips

    # The type of complex projection operation to perform on an image. See 
    # {Vips::Image.complexget}.
    #
    # *     ':real' get real part
    #
    # *     ':imag' get imaginary part

    class OperationComplexget < Symbol
    end
end
