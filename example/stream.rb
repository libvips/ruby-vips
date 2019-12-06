#!/usr/bin/ruby

require 'vips'

source = File.open ARGV[0], "rb"
input_stream = Vips::Streamiu.new
input_stream.on_read { |length| source.read length }
input_stream.on_seek { |offset, whence| source.seek(offset, whence) }

dest = File.open ARGV[1], "w"
output_stream = Vips::Streamou.new
output_stream.on_write { |chunk| dest.write(chunk) }
output_stream.on_finish { dest.close }

image = Vips::Image.new_from_stream input_stream, "", access: "sequential"
image.write_to_stream output_stream, ".png"
