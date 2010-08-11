$:.unshift File.expand_path('../../ext', __FILE__)

require "rubygems"
require "vips"
require 'tempfile'
require "rspec"

Dir["#{File.expand_path('../support', __FILE__)}/*.rb"].each do |file|
  require file
end

def sample_join(*args)
  pth = File.join '..', 'samples', *args
  File.expand_path pth, __FILE__
end

Rspec.configure do |config|
  config.include Spec::Path
  config.include Spec::Helpers

  config.before :all do
    reset_formats!
    reset_working!
  end

  config.before :each do
    reset_working!
  end

  config.filter_run_excluding :vips_lib_version => lambda{ |ver|
    return !Spec::Helpers.match_vips_version(ver)
  }
end

