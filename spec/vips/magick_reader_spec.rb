require "spec_helper"

describe VIPS::MagickReader do
  before :all do
    @path = sample('lion.svg').to_s
  end

  it "should read just about anything through imagemagick" do
    im = VIPS::Image.magick @path
    im.x_size.should == 425
  end

  it "should recognize a magick image" do
    vips_res = VIPS::MagickReader.recognized? sample('wagon.v').to_s
    vips_res.should be_false

    res = VIPS::MagickReader.recognized? @path
    res.should be_true
  end
end
