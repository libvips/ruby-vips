require "spec_helper"

describe VIPS::PPMWriter do
  before :all do
    @image = simg 'wagon.v'
    @writer = VIPS::PPMWriter.new @image
    @path = tmp('wagon.ppm').to_s
  end

  it "should write a ppm file" do
    @writer.write(@path)

    im = VIPS::Image.ppm @path
    im.should match_image(@image)
  end

  it "should allow setting the format of the ppm file to ascii" do
    @writer.format = :ascii
    @writer.write @path

    im = VIPS::Image.ppm @path
    im.should match_image(@image)
  end

  it "should create a ppm writer" do
    @writer.class.should == VIPS::PPMWriter
    @writer.image.should == @image
  end

  it "should accept options on creation from an image" do
    writer = @image.ppm(nil, :format => :ascii)
    writer.format.should == :ascii
  end
end
