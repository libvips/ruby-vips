require "bundler/gem_tasks"
require "rspec/core/rake_task"

RSpec::Core::RakeTask.new(:spec)

task :default => :spec

task :build do
	Dir.chdir "ext" do
		sh "rake"
	end
end

task :console do
	require 'pry'
	
	require_relative 'lib/vips'
	
	Pry.start
end

begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end

require "github/markup"
require "redcarpet"
require "yard"
require "yard/rake/yardoc_task"

YARD::Rake::YardocTask.new do |yard|
end
