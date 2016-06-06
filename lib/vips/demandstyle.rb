module Vips

    # Operations can hint to the VIPS image IO 
    # system about the kind of demand geometry they prefer. 
    #   
    # These demand styles are given below in order of increasing
    # restrictiveness.  When demanding output from a pipeline, 
    # vips_image_generate()
    # will use the most restrictive of the styles requested by the operations 
    # in the pipeline.
    #   
    # *   `:thinstrip` --- This operation would like to output strips 
    #     the width of the image and a few pels high. This is option suitable 
    #     for point-to-point operations, such as those in the arithmetic 
    #     package.
    #    
    #     This option is only efficient for cases where each output pel depends 
    #     upon the pel in the corresponding position in the input image.
    # 
    # * `:fatstrip` --- This operation would like to output strips 
    #     the width of the image and as high as possible. This option is 
    #     suitable for area operations which do not violently transform 
    #     coordinates, such as vips_conv(). 
    #   
    # * `:smalltile` --- This is the most general demand format.
    #     Output is demanded in small (around 100x100 pel) sections. This style 
    #     works reasonably efficiently, even for bizzare operations like 45 
    #     degree rotate.
    #   
    # * `:any` --- This image is not being demand-read from a disc 
    #     file (even indirectly) so any demand style is OK. It's used for 
    #     things like vips_black() where the pixels are calculated.
    class DemandStyle
    end
end
