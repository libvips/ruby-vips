# coding: utf-8

lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'vips/version'

Gem::Specification.new do |spec|
  spec.name = "ruby-vips"
  spec.version = Vips::VERSION
  spec.authors = ["John Cupitt"]
  spec.email = "jcupitt@gmail.com"

  spec.summary = "Ruby extension for the vips image processing library."
  spec.description = "ruby-vips is a binding for the vips image processing library. It is extremely fast and it can process huge images without loading the whole image in memory."
  spec.homepage = "http://github.com/libvips/ruby-vips"
  spec.licenses = ["MIT"]

  spec.require_paths = ["lib"]
  spec.extra_rdoc_files = [
    "LICENSE.txt",
    "README.md",
    "TODO"
  ]

  spec.files = `git ls-files -z`.split("\x0").reject do |f|
    f.match(%r{^(test|spec|features)/})
  end

  spec.add_runtime_dependency "ffi", ["~> 1.9"]

  spec.add_development_dependency "rake", ["~> 11.0"]
  spec.add_development_dependency "rspec", ["~> 3.3"]
  spec.add_development_dependency "yard", ["~> 0.9.11"]
  spec.add_development_dependency "redcarpet", ["~> 3.3"]
  spec.add_development_dependency "github-markup", ["~> 1.4"]
  spec.add_development_dependency "bundler", [">= 1.0", "< 3"]

  # RuboCop requires Ruby >= 2.2
  if Gem.ruby_version >= Gem::Version.new("2.2")
    spec.add_development_dependency "rubocop", ["~> 0.64"]
  end
end
