# coding: utf-8

lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'vips/version'

Gem::Specification.new do |spec|
  spec.name = "ruby-vips"
  spec.version = Vips::VERSION
  spec.authors = ["John Cupitt"]
  spec.date = "2016-06-07"
  spec.email = "jcupitt@gmail.com"

  spec.summary = "Ruby extension for the vips image processing library."
  spec.description = "ruby-vips is a ruby extension for vips. It is extremely fast and it can process huge images without requiring the entire image to be loaded into memory."
  spec.homepage = "http://github.com/jcupitt/ruby-vips"
  spec.licenses = ["MIT"]

  spec.required_ruby_version = ">= 2.1"
  spec.rubygems_version = "2.5.1"
  spec.require_paths = ["lib"]
  spec.extra_rdoc_files = [
    "LICENSE.txt",
    "README.md",
    "TODO"
  ]

  spec.files = `git ls-files -z`.split("\x0").reject do |f|
    f.match(%r{^(test|spec|features)/})
  end

  spec.add_runtime_dependency "gobject-introspection", ["~> 3.0"]

  spec.add_development_dependency "rake", ["~> 11.0"]
  spec.add_development_dependency "rspec", ["~> 3.3"]
  spec.add_development_dependency "yard", ["~> 0.8"]
  spec.add_development_dependency "redcarpet", ["~> 3.3"]
  spec.add_development_dependency "github-markup", ["~> 1.4"]
  spec.add_development_dependency "bundler", ["~> 1.0"]

end

