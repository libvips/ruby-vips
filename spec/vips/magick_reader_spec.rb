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
end
