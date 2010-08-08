# Vips will print warnings to stdout unless this is set
ENV['IM_WARNING'] = "0"

require 'vips_ext'
require 'vips/version'
require 'vips/jpeg_writer'
require 'vips/png_writer'
require 'vips/tiff_writer'
require 'vips/ppm_writer'
require 'vips/csv_writer'
