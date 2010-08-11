module VIPS
  class Writer
    def write(path)
      write_internal path
    end
  end

  class CSVWriter < Writer
    attr_accessor :separator

    def initialize(image, options={})
      super image
      @separator = "\t"

      self.separator = options[:separator] if options.has_key?(:separator)
    end

    def write(path)
      write_internal "#{path}:sep:#{@separator}"
    end
  end

    class JPEGWriter < Writer
    attr_reader :quality

    def initialize(image, options={})
      super image

      @quality = 75

      self.quality = options[:quality] if options.has_key?(:quality)
    end

    def write(path)
      write_internal "#{path}:#{@quality}"
    end

    def to_memory
      buf_internal @quality
    end

    def quality=(quality_v)
      unless (0..100).include?(quality_v)
        raise ArgumentError, 'quality must be a numeric value between 0 and 100'
      end

      @quality = quality_v
    end
  end

  class PNGWriter < Writer
    attr_reader :compression
    attr_accessor :interlace

    def initialize(image, options={})
      super image

      @compression = 6
      @interlace = false

      self.compression = options[:compression] if options.has_key?(:compression)
      self.interlace = options[:interlace] if options.has_key?(:interlace)
    end

    def write(path)
      write_internal "#{path}:#{@compression},#{@interlace ? 1 : 0}"
    end

    def to_memory
      buf_internal @compression, (@interlace ? 1 : 0)
    end

    def compression=(compression_v)
      unless (0..9).include?(compression_v)
        raise ArgumentError, 'compression must be a numeric value between 0 and 9'
      end

      @compression = compression_v
    end
  end

  class PPMWriter < Writer
    attr_reader :format

    FORMAT = [:binary, :ascii]

    def initialize(image, options={})
      super image

      @format = :binary

      self.format = options[:format] if options.has_key?(:format)
    end

    def write(path)
      write_internal "#{path}:#{@format}"
    end

    def format=(format_v)
      unless FORMAT.include?(format_v)
        raise ArgumentError, "format must be one of #{FORMAT.join(', ')}"
      end

      @format = format_v
    end
  end

  class TIFFWriter < Writer
    attr_reader :compression, :layout, :multi_res, :format, :resolution_units,
      :resolution, :predictor, :quality, :tile_size

    COMPRESSION = [:none, :jpeg, :deflate, :packbits, :ccittfax4, :lzw]
    PREDICTOR = [:none, :horizontal_differencing, :floating_point]
    LAYOUT = [:strip, :tile]
    MULTI_RES = [:flat, :pyramid]
    FORMAT = [:manybit, :onebit]
    RESOLUTION_UNITS = [:cm, :inch]

    def initialize(image, options={})
      super image

      @compression = :none
      @quality = 75
      @predictor = :none
      @layout = :strip
      @tile_size = [128, 128]
      @multi_res = :flat
      @format = :manybit
      @resolution_units = :cm

      [ :compression, :layout, :multi_res, :format, :resolution_units,
        :resolution, :predictor, :quality, :tile_size ].each do |att|
        self.send "#{att}=".to_sym, options[att] if options.has_key?(att)
      end
    end

    def write(path)
      opts = [compression_str, layout_str, @multi_res, @format, resolution_str].join ','
      write_internal "#{path}:#{opts}"
    end

    def compression_str
      case @compression
      when :jpeg then "#{@compression}:#{@quality}"
      when :lzw, :deflate then "#{@compression}:#{@predictor}"
      else @compression
      end
    end

    def layout_str
      s = @layout
      s << ":#{@tile_size.join 'x'}" if @layout == :tile
      s
    end

    def resolution_str
      s = "res_#{@resolution_units}"
      s << ":#{@resolution.join 'x'}" if @resolution
      s
    end

    def compression=(compression_v)
      unless COMPRESSION.include?(compression_v)
        raise ArgumentError, "compression must be one of #{COMPRESSION.join(', ')}"
      end

      @compression = compression_v
    end

    def quality=(quality_v)
      unless (0..100).include?(quality_v)
        raise ArgumentError, 'quality must be a numeric value between 0 and 100'
      end

      @quality = quality_v
    end

    def predictor=(predictor_v)
      unless PREDICTOR.include?(predictor_v)
        raise ArgumentError, "predictor must be one of #{PREDICTOR.join(', ')}"
      end

      @predictor = predictor_v
    end

    def layout=(layout_v)
      unless LAYOUT.include?(layout_v)
        raise ArgumentError, "layout must be one of #{LAYOUT.join(', ')}"
      end

      @layout = layout_v
    end

    def tile_size=(tile_size_v)
      unless tile_size_v[0] > 1 && tile_size_v[1] > 1
        raise ArgumentError, "tile sizes must be larger than 1"
      end

      @tile_size = tile_size_v
    end

    def multi_res=(multi_res_v)
      unless MULTI_RES.include?(multi_res_v)
        raise ArgumentError, "multi res must be one of #{MULTI_RES.join(', ')}"
      end

      @multi_res = multi_res_v
    end

    def format=(format_v)
      unless FORMAT.include?(format_v)
        raise ArgumentError, "format must be one of #{FORMAT.join(', ')}"
      end

      @format = format_v
    end

    def resolution_units=(resolution_units_v)
      unless RESOLUTION_UNITS.include?(resolution_units_v)
        raise ArgumentError, "Resolution units must be one of #{RESOLUTION_UNITS.join(', ')}"
      end

      @resolution_units = resolution_units_v
    end

    def resolution=(resolution_v)
      unless resolution_v[0] > 0 && resolution_v[1] > 0
        raise ArgumentError, "Resolution must have a height and width larger than 0"
      end

      @resolution = resolution_v
    end
  end

  class Image
    def csv(*args)
      invoke_writer CSVWriter, *args
    end

    def jpeg(*args)
      invoke_writer JPEGWriter, *args
    end

    def png(*args)
      invoke_writer PNGWriter, *args
    end

    def ppm(*args)
      invoke_writer PPMWriter, *args
    end

    def tiff(*args)
      invoke_writer TIFFWriter, *args
    end

    def vips(*args)
      invoke_writer VIPSWriter, *args
    end

    private

    def invoke_writer(writer_class, path=nil, options={})
      w = writer_class.new self, options
      if path
        w.write path
      else
        w
      end
    end
  end
end
