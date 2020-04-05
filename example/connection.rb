#!/usr/bin/ruby

require 'vips'
require "down/http"

# byte_source = File.open ARGV[0], "rb"
byte_source = Down::Http.open(ARGV[0])

source = Vips::SourceCustom.new
source.on_read { |length| byte_source.read length }
source.on_seek { |offset, whence| puts "seeking to #{offset}, #{whence}"; byte_source.seek(offset, whence) }

byte_target = File.open ARGV[1], "wb"
target = Vips::TargetCustom.new
target.on_write { |chunk| byte_target.write(chunk) }
target.on_finish { byte_target.close }

image = Vips::Image.new_from_source source, "", access: :sequential
image.write_to_target target, ".jpg"
