module VIPS
  class CSVWriter < Writer
    attr_accessor :separator

    def initialize(*args)
      @separator = "\t"
      super *args
    end

    def write(path)
      csv_write "#{path}:sep:#{@separator}"
    end
  end

  class Image
    def to_csv(options = {})
      writer = CSVWriter.new self
      writer.separator = options[:separator] if options.has_key?(:separator)
      writer
    end
  end
end
