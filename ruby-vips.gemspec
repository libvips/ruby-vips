require File.expand_path("../lib/vips/version", __FILE__)

Gem::Specification.new do |s|
  s.name        = "ruby-vips"
  s.version     = VIPS::VERSION
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Timothy Elliott"]
  s.email       = ["tle@holymonkey.com"]
  s.homepage    = "http://github.com/ender672/ruby-vips"
  s.summary     = "Ruby extension for the vips image processing library."
  s.description = "VIPS is an image processing library. It is fast and memory efficient."
  s.required_rubygems_version = ">= 1.3.6"
  s.rubyforge_project         = "ruby-vips"
  s.files        = Dir["{lib}/**/*.rb", "bin/*", "LICENSE", "*.md", "ext/**/*.[ch]"]
  s.extensions = "ext/extconf.rb"
end

