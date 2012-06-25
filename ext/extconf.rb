ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require "mkmf"

File::unlink("Makefile") if (File::exist?("Makefile"))

found = false
%w{7.29 7.28 7.27 7.26 7.24 7.23 7.22 7.20}.each do |x| 
    if pkg_config("vips-#{x}")
        found = true
        break
    end
end

if not found
    puts "no compatible libvips found, check your pkg-config settings"
else
    create_makefile('vips_ext')
end
