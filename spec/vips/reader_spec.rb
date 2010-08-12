require "spec_helper"

describe VIPS::Reader do
  before :all do
    @path = sample('wagon.v').to_s
  end

  it "should open an image through the reader class" do
    im = VIPS::Reader.new(@path).read
    im.should match_image(VIPS::Image.vips(@path))
  end

  it "should open an image through the image class" do
    im = VIPS::Image.new(@path)
    im.should match_image(VIPS::Image.vips(@path))
  end
end
