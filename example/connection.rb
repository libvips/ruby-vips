#!/usr/bin/ruby

require 'vips'

file = File.open ARGV[0], "rb"
source = Vips::SourceCustom.new
source.on_read { |length| file.read length }
# this method is optional
# source.on_seek { |offset, whence| file.seek(offset, whence) }

dest = File.open ARGV[1], "wb"
target = Vips::TargetCustom.new
target.on_write { |chunk| dest.write(chunk) }
target.on_finish { dest.close }

image = Vips::Image.new_from_source source, "", access: :sequential
image.write_to_target target, ".png"
