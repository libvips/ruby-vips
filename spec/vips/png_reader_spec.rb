require "spec_helper"

describe VIPS::PNGReader do
  before :all do
    @image = simg 'wagon.v'
    @path = formats('wagon.png').to_s
    @image.png @path
  end

  pending "should read a png file" do
    im = VIPS::PNGReader.new(@path).read
    im.should match_image @image
  end

  it "should recognize a png image" do
    vips_res = VIPS::PNGReader.recognized? sample('wagon.v').to_s
    vips_res.should be_false

    res = VIPS::PNGReader.recognized? @path
    res.should be_true
  end
end
