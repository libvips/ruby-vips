require "spec_helper"

describe VIPS::MagickReader do
  before :all do
    @path = sample('lion.svg').to_s
  end

  it "should read just about anything through imagemagick" do
    im = VIPS::Image.magick @path
    im.x_size.should == 531
  end

  it "should recognize a magick image" do
    res = VIPS::MagickReader.recognized? @path
    res.should == true
  end

  it "should read a magick file from memory" do
    if Spec::Helpers.match_vips_version(">= 8.2")
      png_data = IO.read(@path)
      reader = VIPS::MagickReader.new(png_data)
      im = reader.read_buffer
      im.x_size.should == @image.x_size
    end
  end
end
