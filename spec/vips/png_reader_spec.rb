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
end
