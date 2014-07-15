require "spec_helper"

describe VIPS::PNGReader do
  before :all do
    @image = simg 'wagon.v'
    @path = formats('wagon.png').to_s
    @image.png @path
  end

  it "should read a png file" do
    im = VIPS::PNGReader.new(@path).read
    im.should match_image @image
  end

  it "should recognize a png image" do
    vips_res = VIPS::PNGReader.recognized? sample('wagon.v').to_s
    vips_res.should == false

    res = VIPS::PNGReader.recognized? @path
    res.should == true
  end

  it "should read a png file from memory" do
    if Spec::Helpers.match_vips_version(">= 7.34")
      png_data = IO.read(@path)
      reader = VIPS::PNGReader.new(png_data)
      im = reader.read_buffer
      im.x_size.should == @image.x_size
    end
  end
end
