# This module provides an interface to the top level bits of GLib
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
end
