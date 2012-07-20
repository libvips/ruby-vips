ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require "mkmf"

File::unlink("Makefile") if (File::exist?("Makefile"))

# override normal build configuration to build debug friendly library
# if installed via 'gem install ruby-vips -- --enable-debug'
# see: http://jonforums.github.com/ruby/2011/01/27/debugging-native-gems-1.html
if enable_config('debug')
    puts '[INFO] enabling debug library build configuration.'
    if RUBY_VERSION < '1.9'
        $CFLAGS = CONFIG['CFLAGS'].gsub(/\s\-O\d?\s/, ' -O0 ')
        $CFLAGS.gsub!(/\s?\-g\w*\s/, ' -ggdb3 ')
        CONFIG['LDSHARED'] = CONFIG['LDSHARED'].gsub(/\s\-s(\s|\z)/, ' ')
    else
        CONFIG['debugflags'] << ' -ggdb3 -O0'
    end
end

# vips-7.30 and later use plain "vips" for the pkg-config name: look for that
# first
if not pkg_config("vips") 
    VIPS_VERSIONS = %w[7.29 7.28 7.27 7.26 7.24 7.23 7.22 7.20]

    if not VIPS_VERSIONS.detect {|x| pkg_config("vips-#{x}") }
        raise("no pkg_config for any of following libvips versions: #{VIPS_VERSIONS.join(', ')}") 
    end
end

create_makefile('vips_ext')
