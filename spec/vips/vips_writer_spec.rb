require "spec_helper"

describe VIPS::VIPSWriter do
  before :all do
    @image = simg 'wagon.v'
    @writer = VIPS::VIPSWriter.new @image
    @path = tmp('wagon.v').to_s
  end

  it "should write a vips file" do
    @writer.write(@path)

    im = VIPS::Image.vips @path
    im.should match_image(@image)
  end

  it "should create a vips writer" do
    @writer.class.should == VIPS::VIPSWriter
    @writer.image.should == @image
  end
end
