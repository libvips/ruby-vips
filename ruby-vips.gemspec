# Generated by jeweler
# DO NOT EDIT THIS FILE DIRECTLY
# Instead, edit Jeweler::Tasks in Rakefile, and run 'rake gemspec'
# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = "ruby-vips"
  s.version = "0.3.6"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Timothy Elliott", "John Cupitt"]
  s.date = "2014-02-04"
  s.description = "Ruby extension for the vips image processing library."
  s.email = "jcupitt@gmail.com"
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = [
    "LICENSE",
    "README.md",
    "TODO"
  ]
  s.files = [
    "CHANGELOG.md",
    "Gemfile.lock",
    "README.md",
    "ext/extconf.rb",
    "ext/header.c",
    "ext/header.h",
    "ext/image.c",
    "ext/image.h",
    "ext/image_arithmetic.c",
    "ext/image_arithmetic.h",
    "ext/image_boolean.c",
    "ext/image_boolean.h",
    "ext/image_colour.c",
    "ext/image_colour.h",
    "ext/image_conversion.c",
    "ext/image_conversion.h",
    "ext/image_convolution.c",
    "ext/image_convolution.h",
    "ext/image_freq_filt.c",
    "ext/image_freq_filt.h",
    "ext/image_histograms_lut.c",
    "ext/image_histograms_lut.h",
    "ext/image_morphology.c",
    "ext/image_morphology.h",
    "ext/image_mosaicing.c",
    "ext/image_mosaicing.h",
    "ext/image_relational.c",
    "ext/image_relational.h",
    "ext/image_resample.c",
    "ext/image_resample.h",
    "ext/interpolator.c",
    "ext/interpolator.h",
    "ext/mask.c",
    "ext/mask.h",
    "ext/reader.c",
    "ext/reader.h",
    "ext/ruby_vips.c",
    "ext/ruby_vips.h",
    "ext/writer.c",
    "ext/writer.h",
    "lib/vips.rb",
    "lib/vips/reader.rb",
    "lib/vips/version.rb",
    "lib/vips/writer.rb",
    "ruby-vips.gemspec",
    "ruby.supp"
  ]
  s.homepage = "http://github.com/jcupitt/ruby-vips"
  s.licenses = ["MIT"]
  s.rdoc_options = ["--title", "ruby-vips \#{version}", "ext"]
  s.require_paths = ["lib"]
  s.rubygems_version = "2.0.3"
  s.summary = "ruby-vips is a ruby extension for vips. It is extremely fast and it can process huge images without requiring the entire image to be loaded into memory."

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<rdoc>, ["~> 3.12"])
      s.add_development_dependency(%q<bundler>, [">= 0"])
      s.add_development_dependency(%q<jeweler>, [">= 2.0.1"])
      s.add_development_dependency(%q<rspec>, [">= 0"])
    else
      s.add_dependency(%q<rdoc>, ["~> 3.12"])
      s.add_dependency(%q<bundler>, [">= 0"])
      s.add_dependency(%q<jeweler>, [">= 2.0.1"])
      s.add_dependency(%q<rspec>, [">= 0"])
    end
  else
    s.add_dependency(%q<rdoc>, ["~> 3.12"])
    s.add_dependency(%q<bundler>, [">= 0"])
    s.add_dependency(%q<jeweler>, [">= 2.0.1"])
    s.add_dependency(%q<rspec>, [">= 0"])
  end
end

