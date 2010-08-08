module VIPS
  class PPMWriter < Writer
    attr_reader :format

    FORMAT = [:binary, :ascii]

    def initialize(*args)
      @format = :binary
      super *args
    end

    def write(path)
      ppm_write "#{path}:#{@format}"
    end

    def format=(format_v)
      unless FORMAT.include?(format_v)
        raise ArgumentError, "format must be one of #{FORMAT.join(', ')}"
      end

      @format = format_v
    end
  end

  class Image
    def to_ppm(options = {})
      writer = PPMWriter.new self
      writer.format = options[:format] if options.has_key?(:format)
      writer
    end
  end
end
