require "spec_helper"

describe VIPS::WEBPWriter do
  before :all do
    @image = simg 'wagon.v'
    @writer = VIPS::WEBPWriter.new @image
    @path = tmp('wagon.webp').to_s
  end

  it "should write to a webp file" do
    if Spec::Helpers.match_vips_version(">= 7.42")
      @writer.write(@path)

      # no webp reader
      #im = VIPS::Image.webp @path
      #im.x_size.should == @image.x_size
      #im.y_size.should == @image.y_size
    end
  end

  it "should set a default quality of 75" do
    @writer.quality.should == 75
  end

  it "should allow setting of the jpeg quality" do
    @writer.quality = 33
    @writer.quality.should == 33
  end

  it "should raise an exception when trying to set an invalid quality" do
    lambda{ @writer.quality = -2 }.should raise_error(ArgumentError)
    lambda{ @writer.quality = "abc" }.should raise_error(ArgumentError)
    lambda{ @writer.quality = 3333 }.should raise_error(ArgumentError)
  end

  it "should write smaller images with lower quality settings" do
    if Spec::Helpers.match_vips_version(">= 7.42")
      @writer.quality = 10
      @writer.write(@path)
      size1 = File.size @path
  
      @writer.quality = 90
      @writer.write(@path)
      size2 = File.size @path
  
      size1.should < size2 / 2
    end
  end

  it "should create a webp writer from an image" do
    @writer.class.should == VIPS::WEBPWriter
    @writer.image.should == @image
  end

  it "should accept options on creation from an image" do
    if Spec::Helpers.match_vips_version(">= 7.42")
      writer = @image.webp(nil, :quality => 22)
      writer.quality.should == 22
    end
  end

  it "should allow access to header fields" do
    @writer.x_size.should == @image.x_size
    @writer.y_size.should == @image.y_size
  end
end
