#!/usr/bin/ruby

require 'ffi'

module Vips
    extend FFI::Library
    ffi_lib 'vips'

    attach_function :vips_init, [ :string ], :int
    attach_function :vips_init, [ :string ], :int
end

Vips.vips_init 'banana'


