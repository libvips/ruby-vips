# master 

### Changed

* Don't use :sequential option for older libvipses [John Cupitt]

# Version 0.2.0

Release date: 2012-06-29

### Added

* Add tile_cache [John Cupitt]
* Add :sequential option to tiff, jpeg and png readers [John Cupitt]
* Add raise if suitable pkg_config for libvips is not found, thanks to Pierre
  Chapuis [Stanislaw Pankevich]
* Add backward compatibility of 0.1.x ruby-vips with libvips versions less than 7.28 [John Cupitt]
* Add Travis. ruby-vips now is being tested on travis-ci.org. [Stanislaw Pankevich]

### Changed

* Disable the vips8 operation cache to save some memory [John Cupitt]
* Update example shrinker [John Cupitt]

### Fixed

* #8: Memory allocation-free issues [Grigoriy Chudnov]

# Version 0.1.1

Release date: 2012-06-22

### Changed

* Upgrade spec/* code to latest RSpec  [Stanislaw Pankevich]

### Added

* Added CHANGELOG.md file (thanks to jnicklas/capybara - using the layout of their History.txt) [Stanislaw Pankevich]
* Added Gemfile with the only 'rspec' dependency. [Stanislaw Pankevich]
* Added Jeweler Rakefile contents to release ruby-vips as a gem. [Stanislaw Pankevich]

# Before (initial unreleased version 0.1.0)

Long-long history here undocumented...
