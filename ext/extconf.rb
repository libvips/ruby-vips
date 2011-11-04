ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require "mkmf"

File::unlink("Makefile") if (File::exist?("Makefile"))

%w{7.27 7.26 7.24 7.23 7.22 7.20}.each{|x| break if pkg_config("vips-#{x}")}

have_header('vips/vips.h')
create_makefile('vips_ext')
