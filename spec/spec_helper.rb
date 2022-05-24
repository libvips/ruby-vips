# Set default concurrency so we can check against it later. Must be set
# before Vips.init sets concurrency to the default.
DEFAULT_VIPS_CONCURRENCY = 5
ENV["VIPS_CONCURRENCY"] = DEFAULT_VIPS_CONCURRENCY.to_s

# Disable stderr output since we purposefully trigger warn-able behavior.
ENV["VIPS_WARNING"] = "1"

require "vips"

require "tempfile"
require "pathname"

Vips.set_debug ENV["DEBUG"]
# Vips.leak_set true

def simg(name)
  File.join(__dir__, "samples", name)
end

def timg(name)
  File.join(@temp_dir, name)
end

RSpec.configure do |config|
  config.around do |example|
    Dir.mktmpdir("ruby-vips-spec-") do |dir|
      @temp_dir = dir
      example.run
    end
  end

  config.before(:example, jpeg: true) do
    skip "required jpegload for this spec" unless has_jpeg?
  end

  config.before(:example, :version) do |example|
    required_version = example.metadata[:version]
    unless Vips.at_least_libvips?(*required_version)
      skip "required at least #{required_version.join(".")} version of the libvips"
    end
  end
end
