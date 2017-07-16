module Vips

    # The type of rounding to perform on an image. See {Vips::Image.round}.
    #
    # *     ':ceil' round up
    #
    # *     ':floor' round down
    #
    # *     ':rint' round to nearest integer

    class OperationRound < Symbol
    end
end
