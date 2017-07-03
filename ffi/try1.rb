#!/usr/bin/ruby

require 'ffi'

module MyLib
    extend FFI::Library
    ffi_lib 'c'
    attach_function :puts, [ :string ], :int
end

MyLib.puts 'Hello, World using libc!'


