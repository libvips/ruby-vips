module Vips
    # The type of access an operation has to supply. 
    # 
    # *   `:random` means requests can come in any order. 
    # 
    # *   `:sequential` means requests will be top-to-bottom, but with some
    #     amount of buffering behind the read point for small non-local
    #     accesses. 
    #
    # *   `:sequential_unbuffered` means requests will be strictly
    #     top-to-bottom with no read-behind. This can save some memory.
    class Access
    end
end
