# Vips will print warnings to stdout unless this is set
ENV['IM_WARNING'] = "0"

require 'vips_ext'
require 'vips/version'
require 'vips/reader'
require 'vips/writer'

module Vips
  class << self
    def sequential_mode_supported?
      VIPS::LIB_VERSION_ARRAY[0] >= 7 and VIPS::LIB_VERSION_ARRAY[1] >= 29
    end
  end
end
