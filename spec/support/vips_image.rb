module Spec
  module Path
    def simg(filename)
      path = sample(filename).to_s

      case File.extname(filename)
      when '.jpg' then VIPS::Image.read_jpeg(path)
      when '.v' then VIPS::Image.read(path)
      end
    end

    extend self
  end
end

