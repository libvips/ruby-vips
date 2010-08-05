$:.unshift File.expand_path('../../ext', __FILE__)

require "rspec/core"
require "vips"
require 'tempfile'

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

  config.before :each do
    reset!
  end

  config.filter_run_excluding :vips_lib_version => lambda{ |ver|
    return !Spec::Helpers.match_vips_version(ver)
  }
end

