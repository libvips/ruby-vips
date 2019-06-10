require "bundler/gem_tasks"
require "rspec/core/rake_task"

RSpec::Core::RakeTask.new(:test)

task :default => :test

task :ext do
	Dir.chdir "ext" do
		sh "rake"
	end
end

task :console do
	require 'pry'
	
	require_relative 'lib/vips'
	
	Pry.start
end

require "yard/rake/yardoc_task"

YARD::Rake::YardocTask.new do |yard|
	require "yard"
	require "github/markup"
	require "redcarpet"
end
