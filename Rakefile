# encoding: utf-8

require 'rubygems'
require 'bundler'

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
  # gem is a Gem::Specification... see http://guides.rubygems.org/specification-reference/ for more options
  gem.name = "ruby-vips"
  gem.homepage = "http://github.com/jcupitt/ruby-vips"
  gem.license = "MIT"
  gem.summary = %Q{Ruby extension for the vips image processing library.}
  gem.description = %Q{ruby-vips is a ruby extension for vips. It is extremely fast and it can process huge images without requiring the entire image to be loaded into memory.}
  gem.email = "jcupitt@gmail.com"
  gem.authors = ["John Cupitt"]
  # dependencies defined in Gemfile
end
Jeweler::RubygemsDotOrgTasks.new

require 'rspec/core'
require 'rspec/core/rake_task'
RSpec::Core::RakeTask.new(:spec) do |spec|
  spec.pattern = FileList['spec/**/*_spec.rb']
end

task :default => :spec

require "github/markup"
require "redcarpet"
require "yard"
require "yard/rake/yardoc_task"

YARD::Rake::YardocTask.new do |yard|
end
