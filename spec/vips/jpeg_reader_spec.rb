require "spec_helper"

describe VIPS::JPEGReader do
  before :all do
    @image = simg 'wagon.v'
    @path = formats('wagon.jpg').to_s
    @image.jpeg @path
  end

  it "should read a jpeg file" do
    im = VIPS::JPEGReader.new(@path).read
    im.x_size.should == @image.x_size
  end

  it "should recognize options in the init hash" do
    reader = VIPS::JPEGReader.new @path, :fail_on_warn => true
    reader.fail_on_warn.should be_true
    reader.path.should == @path
  end

  it "should shrink an image on read" do
    reader = VIPS::JPEGReader.new @path
    reader.shrink_factor = 4
    im = reader.read
    im.x_size.should == @image.x_size / 4
  end

  it "should recognize a jpeg image" do
    vips_res = VIPS::JPEGReader.recognized? sample('wagon.v').to_s
    vips_res.should be_false

    res = VIPS::JPEGReader.recognized? @path
    res.should be_true
  end

  it "should read a jpeg image from a memory buffer" do
    jpeg_data = IO.read(@path)
    reader = JPEGReader.new(jpeg_data, 
        :shrink_factor => 2, :fail_on_warn => true)
    im = reader.read_buffer
    im.x_size.should == @image.x_size / 2
  end
end
