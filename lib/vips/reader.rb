module VIPS
  class Reader
    attr_reader :path

    def initialize(path, options={})
      @path = path
      @_im = nil
    end

    def read
      # in case the sub-class has not read it
      if not @_im 
          @_im = read_retry @path, 0
      end
      @_im
    end

    # support these two for compat with older ruby-vips
    def x_size
        read.x_size
    end

    def y_size
        read.y_size
    end

    def exif
        read.exif
    end

    def exif?
        read.exif?
    end

    private

    # read can fail due to no file descriptors ... if we fail, do a GC and try
    # again a second time
    def read_retry(path, seq)
      im = nil
      begin
        im = read_internal path, seq
      rescue
        GC.start
        im = read_internal path, seq
      end
      im
    end
  end

  class JPEGReader < Reader
    attr_reader :shrink_factor
    attr_accessor :fail_on_warn
    attr_accessor :sequential

    SHRINK_FACTOR = [1, 2, 4, 8]

    # Creates a jpeg image file reader.
    def initialize(path, options={})
      @shrink_factor = options[:shrink_factor] || 1
      @fail_on_warn = options[:fail_on_warn] || false
      @sequential = options[:sequential] || false

      super path, options
    end

    def read
      str = "#{@path}:#{shrink_factor}"
      str << "," 
      str << "fail" if @fail_on_warn

      seq = 0
      if VIPS.sequential_mode_supported?
        str << "," 
        str << "sequential" if @sequential
        seq = 1
      end

      @_im = read_retry str, seq
    end

    def b_to_i(b)
      if b
        1
      else
        0
      end
    end

    def read_buffer
      @_im = buf_internal @path, @shrink_factor, b_to_i(@fail_on_warn)
    end

    # Shrink the jpeg while reading from disk. This means that the entire image
    # never has to be loaded in memory. Shrink factor can be 1 (no shrink), 2,
    # 4, or 8.
    def shrink_factor=(shrink_factor_v)
      unless SHRINK_FACTOR.include?(shrink_factor_v)
        raise ArgumentError, "Shrink factor must be one of: #{SHRINK_FACTOR.join ', '}."
      end

      @shrink_factor = shrink_factor_v
    end
  end

  class CSVReader < Reader
    attr_reader :line_skip, :line_limit
    attr_accessor :whitespace, :separator

    # Creates a CSV reader.
    def initialize(path, options={})
      @line_skip = options[:line_skip] || 0
      @whitespace = options[:whitespace] || ' "'
      @separator = options[:separator] || ";,\t"
      @line_limit = options[:line_limit] || 0

      super path, options
    end

    def read
      str = "#{@path}:ski:#{@line_skip},whi:#{@whitespace}"
      str << ",lin:#{@line_limit == 0 ? -1 : @line_limit}"

      # VIPS magic open path limitation/bug -- we cannot specify the comma char
      str << ",sep:#{@separator}" unless @separator[/,/]

      @_im = read_retry str, 0
    end

    # Set the number of lines to skip at the beginning of the file.
    def line_skip=(line_skip_v)
      if line_skip_v < 0
        raise ArgumentError, "Line skip must be 0 or more."
      end

      @line_skip = line_skip_v
    end

    # Set a limit of how many lines to read in.
    def line_limit=(line_limit_v)
      if line_limit_v < 0
        raise ArgumentError, "Line limit must be 0 (read all) or more."
      end

      @line_limit = line_limit_v
    end
  end

  class TIFFReader < Reader
    attr_reader :page_number
    attr_accessor :sequential

    # Create a tiff image file reader.
    def initialize(path, options={})
      @page_number = nil
      @sequential = options[:sequential] || false

      self.page_number = options[:page_number] if options.has_key?(:page_number)
      super path, options
    end

    def read
      str = "#{@path}:"
      str << "#{@page_number}" if @page_number

      seq = 0
      if VIPS.sequential_mode_supported?
        str << ","
        str << "sequential" if @sequential
        seq = 1
      end

      @_im = read_retry str, seq
    end

    # Select which page in a multiple-page tiff to read. When set to nil, all
    # pages are read as a single image.
    def page_number=(page_number_v)
      unless page_number_v.nil? || page_number_v > 0
        raise ArgumentError, "Page number has to be nil or larger than zero."
      end

      @page_number = page_number_v
    end
  end

  class PNGReader < Reader
    attr_accessor :sequential

    # Create a png image file reader.
    def initialize(path, options={})
      @sequential = options[:sequential] || false

      super path, options
    end

    def read
      str = "#{@path}:"

      seq = 0
      if VIPS.sequential_mode_supported?
        str << "sequential" if @sequential
        seq = 1
      end

      @_im = read_retry str, seq
    end

    def read_buffer
      @_im = buf_internal @path
    end
  end

  class Image

    # Load a ppm file straight to a VIPS Image.
    def self.ppm(*args)
      PPMReader.new(*args).read
    end

    # Load an exr file straight to a VIPS Image.
    def self.exr(*args)
      EXRReader.new(*args).read
    end

    # Load a csv file straight to a VIPS Image.
    def self.csv(*args)
      CSVReader.new(*args).read
    end

    # Load a jpeg file straight to a VIPS Image.
    def self.jpeg(*args)
      JPEGReader.new(*args).read
    end

    # Load a file straight to a VIPS Image using the magick library.
    def self.magick(*args)
      MagickReader.new(*args).read
    end

    # Load a png file straight to a VIPS Image.
    def self.png(*args)
      PNGReader.new(*args).read
    end

    # Load a tiff file straight to a VIPS Image.
    def self.tiff(*args)
      TIFFReader.new(*args).read
    end

    # Load a native vips file straight to a VIPS Image.
    def self.vips(*args)
      VIPSReader.new(*args).read
    end

    # Load any file straight to a VIPS Image. VIPS will determine the format
    # based on the file extension. If the extension is not recognized, VIPS
    # will look at the file signature.
    def self.new(*args)
      Reader.new(*args).read
    end
  end
end
