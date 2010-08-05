module Spec
  module Path
    def simg(filename)
      VIPS::Image.new(sample(filename).to_s)
    end

    extend self
  end
end

