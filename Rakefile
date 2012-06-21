require "rubygems"

def gemspec
  @gemspec ||= begin
    file = File.expand_path('../ruby-vips.gemspec', __FILE__)
    eval(File.read(file), binding, file)
  end
end

desc "Build the gem"
task :build => "#{gemspec.full_name}.gem"

file "#{gemspec.full_name}.gem" => gemspec.files + ["ruby-vips.gemspec"] do
  system "gem build ruby-vips.gemspec"
end

