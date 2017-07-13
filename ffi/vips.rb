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

require 'vips/glib'
require 'vips/gvalue'
require 'vips/gobject'

module Vips
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


