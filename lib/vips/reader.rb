module VIPS
  class Reader
    attr_reader :path

    def initialize(path, options={})
      @path = path
      read_header path unless options[:skip_header]
    end

    def self.read(path, options={})
      options[:skip_header] = true
      reader = new path, options
      reader.read
    end

    def read
      read_internal @path
    end
  end

  class CSVReader < Reader
    attr_reader :line_skip, :line_limit
    attr_accessor :whitespace, :separator

    def initialize(path, options={})
      @line_skip = 0
      @whitespace = ' "'
      @separator = ";,\t"
      @line_limit = 0

      self.line_skip = options[:line_skip] if options.has_key?(:line_skip)
      self.whitespace = options[:whitespace] if options.has_key?(:whitespace)
      self.separator = options[:separator] if options.has_key?(:separator)
      self.line_limit = options[:line_limit] if options.has_key?(:line_limit)

      super path, options={}
    end

    def read
      str = "#{@path}:ski:#{@line_skip},whi:#{@whitespace}"
      str << ",lin:#{@line_limit == 0 ? -1 : @line_limit}"

      # VIPS magic open path limitation/bug -- we cannot specify the comma char
      str << ",sep:#{@separator}" unless @separator[/,/]
      read_internal str
    end

    def line_skip=(line_skip_v)
      if line_skip_v < 0
        raise ArgumentError, "Line skip must be 0 or more."
      end

      @line_skip = line_skip_v
    end

    def line_limit=(line_limit_v)
      if line_limit_v < 0
        raise ArgumentError, "Line limit must be 0 (read all) or more."
      end

      @line_limit = line_limit_v
    end
  end

  class JPEGReader < Reader
    attr_reader :shrink_factor
    attr_accessor :fail_on_warn

    SHRINK_FACTOR = [1, 2, 4, 8]

    def initialize(path, options={})
      @shrink_factor = 1
      @fail_on_warn = false

      self.shrink_factor = options[:shrink_factor] if options.has_key?(:shrink_factor)
      self.fail_on_warn = options[:fail_on_warn] if options.has_key?(:fail_on_warn)

      super path, options={}
    end

    def read
      str = "#{@path}:#{shrink_factor}"
      str << ",fail" if @fail_on_warn

      read_internal str
    end

    def shrink_factor=(shrink_factor_v)
      unless SHRINK_FACTOR.include?(shrink_factor_v)
        raise ArgumentError, "Shrink factor must be one of: #{SHRINK_FACTOR.join ', '}."
      end

      @shrink_factor = shrink_factor_v
    end
  end

  class TIFFReader < Reader
    attr_reader :page_number

    def initialize(path, options={})
      self.page_number = options[:page_number] if options.has_key?(:page_number)
      super path, options={}
    end

    def read
      str = @path
      str << ":#{@page_number}" if @page_number

      read_internal str
    end

    def page_number=(page_number_v)
      unless page_number_v.nil? || page_number_v > 0
        raise ArgumentError, "Page number has to be nil or larger than zero."
      end

      @page_number = page_number_v
    end
  end

  class Image
    def self.ppm(*args)
      PPMReader.read *args
    end

    def self.exr(*args)
      EXRReader.read *args
    end

    def self.csv(*args)
      CSVReader.read *args
    end

    def self.jpeg(*args)
      JPEGReader.read *args
    end

    def self.magick(*args)
      MagickReader.read *args
    end

    def self.png(*args)
      PNGReader.read *args
    end

    def self.tiff(*args)
      TIFFReader.read *args
    end

    def self.vips(*args)
      VIPSReader.read *args
    end

    def self.new(*args)
      Reader.read *args
    end
  end
end
