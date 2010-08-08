require "spec_helper"

describe VIPS::PPMWriter do
  before :all do
    @image = simg 'wagon.v'
    @writer = @image.to_ppm
    @path = tmp('wagon.ppm').to_s
  end

  it "should write a ppm file" do
    @writer.write(@path)

    im = VIPS::Image.read_ppm @path
    im.should match_image(@image)
  end

  it "should allow setting the format of the ppm file to ascii" do
    @writer.format = :ascii
    @writer.write @path

    im = VIPS::Image.read_ppm @path
    im.should match_image(@image)
  end

  it "should create a ppm writer" do
    writer = @image.to_ppm
    writer.class.should == VIPS::PPMWriter
    writer.image.should == @image
  end

  it "should accept options on creation from an image" do
    writer = @image.to_ppm(:format => :ascii)
    writer.format.should == :ascii
  end
end

