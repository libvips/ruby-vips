#!/usr/bin/ruby

require 'vips'

class Mystreami < Vips::Streamiu
  def initialize(filename)
    puts "Mystreami: initialize"

    @filename = filename
    @contents = File.read(@filename)
    @length = @contents.length
    @read_point = 0

    super()

    signal_connect "read" do |buf, len|
      puts "Mystreami read: #{len} bytes"

      bytes_available = @length - @read_point
      bytes_to_copy = [bytes_available, len].min
      buf.put_bytes(0, @contents, @read_point, bytes_to_copy)
      @read_point += bytes_to_copy

      bytes_to_copy
    end

    signal_connect "seek" do |offset, whence|
      puts "Mystreami seek: offset #{offset}, whence #{whence}"
      -1
    end

  end
end

streamiu = Mystreami.new(ARGV[0])
image = Vips::Image.new_from_stream(streamiu, "", access: "sequential")

