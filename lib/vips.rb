# This module provides a set of overrides for the vips image processing library
# used via the gobject-introspection gem. 
#
# Author::    John Cupitt  (mailto:jcupitt@gmail.com)
# License::   MIT

# @private
def log str 
    if $vips_debug
        puts str
    end
end

# copied from ruby-gnome2/gstreamer/lib/gst.rb without much understanding

require 'pathname'
require 'gobject-introspection'

# pick up a local girepository/lib in preference to the system one 
base_dir = Pathname.new(__FILE__).dirname.dirname.expand_path
vendor_dir = base_dir + "vendor" + "local"
vendor_bin_dir = vendor_dir + "bin"
GLib.prepend_dll_path(vendor_bin_dir)
vendor_girepository_dir = vendor_dir + "lib" + "girepository-1.0"
GObjectIntrospection.prepend_typelib_path(vendor_girepository_dir)

module Vips
    # @private
    LOG_DOMAIN = "VIPS"
    GLib::Log.set_log_domain(LOG_DOMAIN)

    # about as crude as you could get
    $vips_debug = false

    # allow to skip GI autoload
    $vips_skip_autoload ||= false

    # Turn debug logging on and off.
    #
    # @param dbg [Boolean] Set true to print debug log messages
    def self.set_debug dbg 
        $vips_debug = dbg
    end

    class << self
        # @private
        def load_gi_module(*argv)
            log "Vips::load_gi_module: #{argv}"

            loader = Loader.new(self, argv)
            begin
                loader.load("Vips")
            rescue 
                puts "Unable to load Vips"
                puts "  Check that the vips library has been installed and is"
                puts "  on your library path."
                puts "  Check that the typelib `Vips-8.0.typelib` has been "
                puts "  installed, and that it is on your GI_TYPELIB_PATH."
                raise
            end

            require 'vips/error'
            require 'vips/argument'
            require 'vips/operation'
            require 'vips/call'
            require 'vips/image'
            require 'vips/version'

            # Make sure we only get called once.
            def self.load_gi_module; false; end

            true
        end
    end

    # @private
    class Loader < GObjectIntrospection::Loader
        def initialize(base_module, init_arguments)
            log "Vips::Loader.initialize: #{base_module}, #{init_arguments}"

            super(base_module)
            @init_arguments = init_arguments
        end

        private
        def pre_load(repository, namespace)
            log "Vips::Loader.pre_load: #{repository}, #{namespace}"

            call_init_function(repository, namespace)
            define_value_modules
        end

        def call_init_function(repository, namespace)
            log "Vips::Loader.call_init_function: #{repository}, #{namespace}"

            # call Vips::init
            init = repository.find(namespace, "init")
            succeeded, argv, error = init.invoke([$PROGRAM_NAME])

            # TODO get the vips error buffer
            raise error unless succeeded

            log "Vips::Loader.call_init_function: argv = #{argv}"
        end

        def define_value_modules
            @value_functions_module = Module.new
            @value_methods_module = Module.new
            @base_module.const_set("ValueFunctions", @value_functions_module)
            @base_module.const_set("ValueMethods",   @value_methods_module)
        end

        def post_load(repository, namespace)
            log "Vips::Loader.post_load:"
        end

    end
end

at_exit {
    Vips::shutdown if Vips.respond_to? :shutdown
}

# this makes vips keep a list of all active objects which we can print out
Vips::leak_set true if $vips_debug

# Initialize GI
Vips::load_gi_module unless $vips_skip_autoload

# @private
def showall 
    if $vips_debug
        GC.start 
        Vips::Object::print_all 
    end
end

