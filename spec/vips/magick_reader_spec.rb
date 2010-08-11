require "spec_helper"

describe VIPS::MagickReader do
  before :all do
    @path = sample('lion.svg').to_s
  end

  it "should read just about anything through imagemagick" do
    im = VIPS::Image.magick @path
    im.x_size.should == 425
  end
end
