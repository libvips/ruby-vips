# encoding: utf-8

$:.unshift File.expand_path('lib', File.dirname(__FILE__))

require "rubygems"
require 'bundler'

require 'vips/version'

begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end

require 'rake'
require 'jeweler'

Jeweler::Tasks.new do |gem|
  # gem is a Gem::Specification... see http://docs.rubygems.org/read/chapter/20 for more options

  gem.name = "ruby-vips"
  gem.homepage = "http://github.com/jcupitt/ruby-vips"
  gem.license = "MIT"
  
  gem.summary = %Q{ruby-vips is a ruby extension for vips. It is extremely fast and it can process huge images without requiring the entire image to be loaded into memory.}
  
  gem.description = %Q{Ruby extension for the vips image processing library.}
  
  gem.email = "jcupitt@gmail.com"
  gem.authors = ["Timothy Elliott", "John Cupitt"]

  gem.version = VIPS::VERSION
  
  gem.files = [
    "*.*",
    "*.",
    "lib/**/*",
    "ext/**/*"
  ]

  gem.rdoc_options << 
    '--title' << 'ruby-vips #{version}' <<
    'ext'

  # dependencies defined in Gemfile
end
Jeweler::RubygemsDotOrgTasks.new

require 'rspec/core'
require 'rspec/core/rake_task'
RSpec::Core::RakeTask.new(:spec) do |spec|
  spec.pattern = FileList['spec/**/*_spec.rb']
end

task :default => :spec

require 'rdoc/task'

Rake::RDocTask.new do |rdoc|
  version = File.exist?('VERSION') ? File.read('VERSION') : ""

  rdoc.rdoc_dir = 'rdoc'
  rdoc.title = "ruby-vips #{version}"
  rdoc.rdoc_files.include('README*')
  rdoc.rdoc_files.include('lib/**/*.rb')
  rdoc.rdoc_files.include('ext/**/*.c')
end
