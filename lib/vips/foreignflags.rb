module Vips

    # Some hints about the image loader.
    #
    # *   `:partial` means that the image can be read directly from the
    #     file without needing to be unpacked to a temporary image first. 
    #
    # *   `:sequential` means that the loader supports lazy reading, but
    #     only top-to-bottom (sequential) access. Formats like PNG can read 
    #     sets of scanlines, for example, but only in order. 
    #
    #     If neither partial` or sequential` is set, the loader only supports 
    #     whole image read. Setting both partial` and sequential` is an error.
    #
    # *   `:bigendian` means that image pixels are most-significant byte
    #     first. Depending on the native byte order of the host machine, you may
    #     need to swap bytes. See vips_copy().
    class ForeignFlags
    end
end
