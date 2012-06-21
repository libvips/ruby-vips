# encoding: utf-8

$:.unshift File.expand_path('lib', File.dirname(__FILE__))

require "rubygems"
require 'bundler'

require 'vips/version'

# Obsolete task. It should be removed later. Now Jeweler does this.
#
# def gemspec
#   @gemspec ||= begin
#     file = File.expand_path('../ruby-vips.gemspec', __FILE__)
#     eval(File.read(file), binding, file)
#   end
# end

# desc "Build the gem"
# task :build => "#{gemspec.full_name}.gem"

# file "#{gemspec.full_name}.gem" => gemspec.files + ["ruby-vips.gemspec"] do
#   system "gem build ruby-vips.gemspec"
# end

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

  # dependencies defined in Gemfile
end
Jeweler::RubygemsDotOrgTasks.new

# Adapt to it rspec later
#
# require 'rake/testtask'

# Rake::TestTask.new(:test) do |test|
#   test.libs << 'lib' << 'test'
#   test.pattern = 'test/**/test_*.rb'
#   test.verbose = true
# end

# require 'rcov/rcovtask'
# Rcov::RcovTask.new do |test|
#   test.libs << 'test'
#   test.pattern = 'test/**/test_*.rb'
#   test.verbose = true
#   test.rcov_opts << '--exclude "gems/*"'
# end

task :default => :test

require 'rdoc/task'
Rake::RDocTask.new do |rdoc|
  version = File.exist?('VERSION') ? File.read('VERSION') : ""

  rdoc.rdoc_dir = 'rdoc'
  rdoc.title = "ruby-vips #{version}"
  rdoc.rdoc_files.include('README*')
  rdoc.rdoc_files.include('lib/**/*.rb')
end
