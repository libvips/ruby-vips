require "spec_helper"

describe VIPS::PNGWriter do
  before :all do
    @image = simg 'wagon.v'
    @writer = @image.to_png
    @path = tmp('wagon.png').to_s
  end

  it "should write to a png file" do
    @writer.write @path

    im = VIPS::Image.read_png @path
    im.x_size.should == @image.x_size
    im.y_size.should == @image.y_size
  end

  it "should write a png to memory", :vips_lib_version => "> 7.22" do
    str = @writer.to_memory
    str.size.should == 1234
  end

  it "should write a tiny png file to memory", :vips_lib_version => "> 7.22" do
    im = VIPS::Image.black(10, 10, 1)
    s = im.to_png.to_memory
    s.size.should == 411
  end

  it "should allow setting of the png compression" do
    @writer.compression = 9
    @writer.compression.should == 9
  end

  it "should raise an exception when trying to set an invalid compression" do
    lambda{ @writer.compression = -2 }.should raise_error(ArgumentError)
    lambda{ @writer.compression = "abc" }.should raise_error(ArgumentError)
    lambda{ @writer.compression = 3333 }.should raise_error(ArgumentError)
  end

  it "should generate smaller memory images with higher compression settings", :vips_lib_version => "> 7.22" do
    @writer.compression = 0
    mempng = @writer.to_memory

    @writer.quality = 9
    mempng = @writer.to_memory

    mempng.size.should < mempng2.size / 2
  end

  it "should write smaller images with lower quality settings" do
    @writer.compression = 9
    @writer.write(@path)
    size1 = File.size @path

    @writer.compression = 0
    @writer.write(@path)
    size2 = File.size @path

    size1.should < size2 / 2
  end

  it "should write an interlaced png" do
    @writer.interlace = true
    @writer.write @path

    im = VIPS::Image.read_png(@path)
    im.should match_image(@image)
  end

  it "should write an interlaced png to memory", :vips_lib_version => "> 7.22" do
    @writer.interlace = true
    str = @writer.to_memory
  end

  it "should create a png writer" do
    writer = @image.to_png
    writer.class.should == VIPS::PNGWriter
    writer.image.should == @image
  end

  it "should accept options on creation from an image" do
    writer = @image.to_png(:compression => 3, :interlace => true)
    writer.compression.should == 3
    writer.interlace.should be_true
  end
end
