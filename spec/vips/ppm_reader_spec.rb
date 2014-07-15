require "spec_helper"

describe VIPS::PPMReader do
  before :all do
    @image = simg 'wagon.v'
    @path = formats('wagon.ppm').to_s
    @image.ppm @path
  end

  it "should read a ppm image" do
    im = VIPS::PPMReader.new(@path).read
    im.should match_image @image
  end

  it "should recognize a ppm image" do
    vips_res = VIPS::PPMReader.recognized? sample('wagon.v').to_s
    vips_res.should == false

    res = VIPS::PPMReader.recognized? @path
    res.should == true
  end
end
