#!/usr/bin/env ruby
# Convert an image to JPEG XL using ruby-vips when its libvips was built
# with libjxl, falling back to cjxl (the only backend that can transcode
# JPEG losslessly, bit-exact), ffmpeg/libjxl, then ImageMagick.
# The result is only reported as success if it starts with a JPEG XL
# codestream (FF 0A) or container signature.
#
#   ruby convert_to_jxl.rb [--lossless] [--distance 1.0] [--effort 7] in.png [out.jxl]
require "open3"

CODESTREAM = "\xFF\x0A".b
CONTAINER = "\x00\x00\x00\x0CJXL \x0D\x0A\x87\x0A".b

def jpeg_xl?(path)
  return false unless File.file?(path)

  head = File.binread(path, 12)
  head.start_with?(CODESTREAM) || head.start_with?(CONTAINER)
rescue SystemCallError
  false
end

def convert(src, dst, lossless:, distance:, effort:)
  begin
    require "vips"
    quality = lossless ? 100 : [[100 - (distance * 10).round, 10].max, 100].min
    Vips::Image.new_from_file(src, access: :sequential)
               .jxlsave(dst, "lossless" => lossless, "Q" => quality, "effort" => effort)
    return dst if jpeg_xl?(dst)
  rescue LoadError, StandardError
    File.delete(dst) if File.exist?(dst)
  end

  cjxl = ["cjxl", src, dst, "--effort=#{effort}",
          lossless ? "--lossless=1" : "--distance=#{distance}"]
  ffmpeg = ["ffmpeg", "-y", "-i", src, "-c:v", "libjxl", "-effort", effort.to_s,
            *(lossless ? ["-lossless", "1"] : ["-distance", distance.to_s]), dst]
  [cjxl, ffmpeg, ["magick", src, dst]].each do |cmd|
    ok = begin
      _o, _e, s = Open3.capture3(*cmd)
      s.success?
    rescue Errno::ENOENT
      false
    end
    return dst if ok && jpeg_xl?(dst)

    File.delete(dst) if File.exist?(dst)
  end
  raise "no working JPEG XL backend (install libvips with libjxl, cjxl, " \
        "ffmpeg --enable-libjxl, or ImageMagick with jxl delegate)"
end

if __FILE__ == $PROGRAM_NAME
  lossless, distance, effort = false, 1.0, 7
  positional = []
  args = ARGV.dup
  until args.empty?
    case (a = args.shift)
    when "--lossless" then lossless = true
    when "--distance" then distance = Float(args.shift)
    when "--effort" then effort = Integer(args.shift)
    else positional << a
    end
  end
  abort "usage: convert_to_jxl.rb [--lossless] [--distance 1.0] [--effort 7] in [out.jxl]" if positional.empty? || positional.size > 2

  src = positional[0]
  dst = positional[1] || src.sub(/\.[^.]+\z/, ".jxl")
  puts "#{src} -> #{convert(src, dst, lossless: lossless, distance: distance, effort: effort)}"
end
