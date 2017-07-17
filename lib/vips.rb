# This module provides an interface to the vips image processing library
# via ruby-ffi.
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

require 'ffi'

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
    GOBJECT_TYPE = g_type_from_name("GObject")
end

require 'vips/gobject'
require 'vips/gvalue'

module Vips
    extend FFI::Library
    ffi_lib 'vips'

    # @private
    LOG_DOMAIN = "VIPS"
    GLib::set_log_domain(LOG_DOMAIN)

    @@debug = false

    # Turn debug logging on and off.
    #
    # @param dbg [Boolean] Set true to print debug log messages
    def self.set_debug dbg 
        @@debug = dbg
    end

    # how crude!
    # @private
    def self.log str 
        if @@debug
            puts str
        end
    end

    # need to repeat this
    typedef :ulong, :GType

    attach_function :vips_error_buffer, [], :string
    attach_function :vips_error_clear, [], :void

    # The ruby-vips error class. 
    class Error < RuntimeError
        # @param msg [String] The error message. If this is not supplied, grab
        #   and clear the vips error buffer and use that. 
        def initialize(msg = nil)
            if msg
                @details = msg
            elsif Vips::vips_error_buffer != ""
                @details = Vips::vips_error_buffer
                Vips::vips_error_clear
            else 
                @details = nil
            end
        end

        # Pretty-print a {Vips::Error}.
        #
        # @return [String] The error message
        def to_s
            if @details != nil
                @details
            else
                super.to_s
            end
        end
    end

    attach_function :vips_init, [:string], :int

    if Vips::vips_init($0) != 0
        throw Vips::get_error
    end

    # don't use at_exit to call vips_shutdown, it causes problems with fork, and
    # in any case libvips does this for us

    attach_function :vips_object_print_all, [], :void
    attach_function :vips_leak_set, [:int], :void

    def self.showall
        if @@debug
            GC.start
            vips_object_print_all
        end
    end

    if @@debug
        vips_leak_set 1
    end

    attach_function :version, :vips_version, [:int], :int
    attach_function :version_string, :vips_version_string, [], :string

    LIBRARY_VERSION = Vips::version_string

end

require 'vips/object'
require 'vips/operation'
require 'vips/image'
require 'vips/interpolate'
require 'vips/version'


