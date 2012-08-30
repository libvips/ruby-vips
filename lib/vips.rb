# Vips will print warnings to stdout unless this is set
ENV['IM_WARNING'] = "0"

require 'vips_ext'
require 'vips/version'
require 'vips/reader'
require 'vips/writer'

module VIPS
  class << self
    def sequential_mode_supported?
      comp = VIPS::LIB_VERSION_ARRAY <=> [7,28,0]
      comp >= 0
    end
  end
end
