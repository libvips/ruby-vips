# master 

# Version 0.3.1

### Fixed

* PNG writer no longer changes the filename argument [John Cupitt]
* Workaround helps ruby-vips compile against 7.26.3 [John Cupitt]
* Image read now runs GC and retries on fail [John Cupitt]

# Version 0.3.0

Release date: 2012-07-20

### Added

* More rspec tests [John Cupitt]
* Updated to libvips-7.30 [John Cupitt]

### Changed

* Reworked Reader class offers better performance and compatibility [John
  Cupitt]
* Don't use :sequential option for older libvipses [John Cupitt]
* Rename "tone_analyze" as "tone_analyse" for consistency with the rest of
  vips [John  CUpitt]

### Fixed

* Now passes rspec test suite cleanly in valgrind [John Cupitt]
* Fixed check of sequential mode support [Stanislaw Pankevich]

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
