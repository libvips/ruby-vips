require "bundler/gem_tasks"

begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  warn e.message
  warn "Run `bundle install` to install missing gems"
  exit e.status_code
end

require "rake"

require "rspec/core"
require "rspec/core/rake_task"
RSpec::Core::RakeTask.new(:spec) do |spec|
  spec.pattern = FileList["spec/**/*_spec.rb"]
end

task default: :spec

unless RUBY_PLATFORM.include?("java")
  require "github/markup"
  require "yard"
  require "yard/rake/yardoc_task"

  YARD::Rake::YardocTask.new
end

# RuboCop requires Ruby >= 2.2
if Gem.ruby_version >= Gem::Version.new("2.2.0")
  require "rubocop/rake_task"
  RuboCop::RakeTask.new
end
