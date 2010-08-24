require "rubygems"

def gemspec
  @gemspec ||= begin
    file = File.expand_path('../ruby-vips.gemspec', __FILE__)
    eval(File.read(file), binding, file)
  end
end

begin
  require "spec"
  require "spec/rake/spectask"
rescue LoadError
  raise 'Run `gem install rspec` to be able to run specs.'
else
  task :clear_tmp do
    FileUtils.rm_rf(File.expand_path("../tmp", __FILE__))
  end

  Spec::Rake::SpecTask.new
end

desc "Build the gem"
task :build => "#{gemspec.full_name}.gem"

file "#{gemspec.full_name}.gem" => gemspec.files + ["ruby-vips.gemspec"] do
  system "gem build ruby-vips.gemspec"
end

