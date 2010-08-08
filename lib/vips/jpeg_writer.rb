module VIPS
  class JPEGWriter < Writer
    attr_reader :quality

    def initialize(image)
      @quality = 75
      super image
    end

    def write(path)
      jpeg_write "#{path}:#{@quality}"
    end

    def to_memory
      jpeg_buf @quality
    end

    def quality=(quality_v)
      unless (0..100).include?(quality_v)
        raise ArgumentError, 'quality must be a numeric value between 0 and 100'
      end

      @quality = quality_v
    end
  end

  class Image
    def to_jpeg(options = {})
      writer = JPEGWriter.new self
      writer.quality = options[:quality] if options.has_key?(:quality)
      writer
    end
  end
end
