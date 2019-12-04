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
      puts "Mystreami read: #{bytes_available} bytes_available"
      puts "Mystreami read: #{bytes_to_copy} bytes_to_copy"
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

class Mystreamo < Vips::Streamou
  def initialize(filename)
    puts "Mystreamo: initialize"

    @filename = filename
    @f = File.open(@filename, "wb")

    super()

    signal_connect "write" do |buf, len|
      puts "Mystreami write: #{len} bytes"
      @f.write(buf)
      len
    end

    signal_connect "finish" do 
      @f.close
      @f = nil
    end

  end
end

streamiu = Mystreami.new(ARGV[0])
image = Vips::Image.new_from_stream(streamiu, "", access: "sequential")
image.avg

#streamio = Mystreamo.new(ARGV[1])
#image.write_to_stream(streamio, ".png")

