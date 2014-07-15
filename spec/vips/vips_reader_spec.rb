require "spec_helper"

describe VIPS::VIPSReader do
  before :all do
    @path = sample('wagon.v').to_s
  end

  it "should read a vips image through a reader" do
    im = VIPS::VIPSReader.new(@path).read
    im.x_size.should == 228
  end

  it "should read a vips image through the Image class" do
    im = VIPS::Image.vips(@path)
    im.x_size.should == 228
  end

  it "should recognize an vips image" do
    vips_res = VIPS::VIPSReader.recognized? sample('icc.jpg').to_s
    vips_res.should == false

    res = VIPS::VIPSReader.recognized? @path
    res.should == true
  end
end
