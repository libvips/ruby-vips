#!/usr/bin/ruby

require "vips"
# gem install down
require "down"

# byte_source = File.open ARGV[0], "rb"
# eg. https://images.unsplash.com/photo-1491933382434-500287f9b54b
byte_source = Down::open(ARGV[0])

source = Vips::SourceCustom.new
source.on_read do |length|
  puts "source: reading #{length} bytes ..."
  byte_source.read length
end
source.on_seek do |offset, whence|
  puts "source: seeking to #{offset}, #{whence}"
  byte_source.seek(offset, whence)
end

byte_target = File.open ARGV[1], "w+b"

target = Vips::TargetCustom.new
target.on_write do |chunk| 
  puts "target: writing #{chunk.length} bytes ..."
  byte_target.write(chunk) 
end
target.on_read do |length| 
  puts "target: reading #{length} bytes ..."
  byte_target.read length 
end
target.on_seek do |offset, whence| 
  puts "target: seeking to #{offset}, #{whence}"
  byte_target.seek(offset, whence) 
end
target.on_end do 
  puts "target: ending"
  byte_target.close 
end

image = Vips::Image.new_from_source source, "", access: :sequential
image.write_to_target target, ARGV[2]
