#!/usr/bin/ruby

require 'vips'

class Mystreami < Vips::Streamiu
  def initialize(filename, pipe_mode=false)
    @filename = filename
    @contents = File.open(@filename, "rb").read
    @length = @contents.length
    @pipe_mode = pipe_mode
    @read_point = 0

    puts "Mystreami: loaded #{@length} bytes"

    super()

    signal_connect "read" do |buf, len|
      puts "read: #{len} bytes from #{@read_point}"

      bytes_available = @length - @read_point
      bytes_to_copy = [bytes_available, len].min
      buf.put_bytes(0, @contents, @read_point, bytes_to_copy)
      @read_point += bytes_to_copy

      puts "    transferred #{bytes_to_copy} bytes"

      bytes_to_copy
    end

    signal_connect "seek" do |offset, whence|
      puts "seek: offset #{offset}, whence #{whence}"
      if @pipe_mode
        puts "    new_position == -1 (pipe mode)"
        -1 
      else
        case whence 
        when 0
          # SEEK_SET
          new_read_point = offset
        when 1
          # SEEK_CUR
          new_read_point = self.read_point + offset
        when 2
          # SEEK_END
          new_read_point = self.length + offset
        else
          raise "bad whence #{whence}"
        end

        @read_point = [0, [@length, new_read_point].min].max

        puts "    new_position = #{@read_point}"

        @read_point
      end
    end
  end
end

class Mystreamo < Vips::Streamou
  def initialize(filename)
    @filename = filename
    @f = File.open(@filename, "wb")

    super()

    signal_connect "write" do |buf, len|
      @f.write(buf.get_bytes(0, len))
      len
    end

    signal_connect "finish" do 
      @f.close
      @f = nil
    end

  end
end

streamiu = Mystreami.new ARGV[0]
image = Vips::Image.new_from_stream streamiu, "", access: "sequential" 
#puts "avg = #{image.avg}"

streamio = Mystreamo.new ARGV[1]
image.write_to_stream streamio, ".png" 

