# This module provides an interface to the vips image processing library
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

# how crude!
# @private
def log str 
    if $vips_debug
        puts str
    end
end

module GLib
    extend FFI::Library
    ffi_lib 'gobject-2.0'

    # nil being the default
    glib_log_domain = nil

    def self.set_log_domain(domain)
        glib_log_domain = domain
    end

    attach_function :g_malloc, [:size_t], :pointer
    attach_function :g_free, [:pointer], :void

    # :gtype will usually be 64-bit, but will be 32-bit on 32-bit Windows
    typedef :ulong, :GType

    attach_function :g_type_name, [:GType], :string
    attach_function :g_type_from_name, [:string], :GType
    attach_function :g_type_fundamental, [:GType], :GType

    # look up some common gtypes
    GBOOL_TYPE = g_type_from_name("gboolean")
    GINT_TYPE = g_type_from_name("gint")
    GDOUBLE_TYPE = g_type_from_name("gdouble")
    GENUM_TYPE = g_type_from_name("GEnum")
    GFLAGS_TYPE = g_type_from_name("GFlags")
    GSTR_TYPE = g_type_from_name("gchararray")
end

require_relative 'vips/gobject'
require_relative 'vips/gvalue'

module Vips
    extend FFI::Library
    ffi_lib 'vips'

    # @private
    LOG_DOMAIN = "VIPS"
    GLib::set_log_domain(LOG_DOMAIN)

    $vips_debug = false

    # Turn debug logging on and off.
    #
    # @param dbg [Boolean] Set true to print debug log messages
    def self.set_debug dbg 
        $vips_debug = dbg
    end

    # need to repeat this
    typedef :ulong, :GType

    attach_function :vips_init, [:string], :int
    attach_function :vips_shutdown, [], :void

    attach_function :vips_error_buffer, [], :string
    attach_function :vips_error_clear, [], :void

    def self.get_error
        errstr = Vips::vips_error_buffer
        Vips::vips_error_clear
        errstr
    end

    if Vips::vips_init($0) != 0
        throw Vips::get_error
    end

    at_exit {
        Vips::vips_shutdown
    }

    attach_function :vips_object_print_all, [], :void
    attach_function :vips_leak_set, [:int], :void

    def self.showall
        if $vips_debug
            GC.start
            vips_object_print_all
        end
    end

    if $vips_debug
        vips_leak_set 1
    end

end

require_relative 'vips/vobject'
require_relative 'vips/voperation'
require_relative 'vips/vimage'


