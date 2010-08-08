module VIPS
  class PNGWriter < Writer
    attr_reader :compression
    attr_accessor :interlace

    def initialize(*args)
      @compression = 6
      @interlace = false
      super *args
    end

    def write(path)
      png_write "#{path}:#{@compression},#{@interlace ? 1 : 0}"
    end

    def to_memory
      png_buf @compression, (@interlace ? 1 : 0)
    end

    def compression=(compression_v)
      unless (0..9).include?(compression_v)
        raise ArgumentError, 'compression must be a numeric value between 0 and 9'
      end

      @compression = compression_v
    end
  end

  class Image
    def to_png(options = {})
      writer = PNGWriter.new self
      writer.compression = options[:compression] if options.has_key?(:compression)
      writer.interlace = options[:interlace] if options.has_key?(:interlace)
      writer
    end
  end
end
