ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require "mkmf"

File::unlink("Makefile") if (File::exist?("Makefile"))

pkg_config("vips-7.23") || pkg_config("vips-7.22") || pkg_config("vips-7.20")

have_header('vips/vips.h')
create_makefile('vips_ext')
