
require_relative "lib/vips/version"

Gem::Specification.new do |spec|
	spec.name          = "vips"
	spec.version       = Vips::VERSION
	spec.authors       = ["John Cupitt", "Samuel Williams"]
	spec.email         = ["jcupitt@gmail.com", "samuel.williams@oriontransfer.co.nz"]

	spec.summary       = "Vips is a high-performance image manipulation library."
	spec.description   = "Provides pre-compiled binaries for libvips."
	spec.homepage      = "https://github.com/ioquatix/vips"
	spec.license       = "MIT"

	spec.files         = `git ls-files -z`.split("\x0").reject do |f|
		f.match(%r{^(test|spec|features)/})
	end
	spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
	spec.require_paths = ["lib"]
	
	spec.required_ruby_version = ">= 2.1"

	spec.extensions = %w[ext/Rakefile]

	spec.add_runtime_dependency "ffi", ["~> 1.9"]

	spec.add_development_dependency "pry"

	spec.add_development_dependency "yard", "~> 0.8"
	spec.add_development_dependency "redcarpet", "~> 3.3"
	spec.add_development_dependency "github-markup", "~> 1.4"

	spec.add_development_dependency "bundler"
	spec.add_development_dependency "rspec", "~> 3.7"
	spec.add_development_dependency "rake"
end
