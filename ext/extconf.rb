ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require "mkmf"

File::unlink("Makefile") if (File::exist?("Makefile"))

VIPS_VERSIONS = %w[7.29 7.28 7.27 7.26 7.24 7.23 7.22 7.20]

raise("There is no pkg_config for any of following libvips versions: #{VIPS_VERSIONS.join(', ')}") unless VIPS_VERSIONS.detect {|x| pkg_config("vips-#{x}") }

create_makefile('vips_ext')
