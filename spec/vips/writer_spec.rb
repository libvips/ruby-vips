require "spec_helper"

describe VIPS::Writer do
  before :all do
    @image = simg 'wagon.v'
    @writer = VIPS::Writer.new(@image)
    @path = tmp('wagon.ppm').to_s
  end

  before :each do
    File.unlink @path if File.exists?(@path)
  end

  it "should write an arbitrary file" do
    @writer.write @path
    File.exists?(@path).should == true

    im = VIPS::Image.new @path
    im.should match_image(@image)
  end

  it "should write through the image class" do
    @image.write @path
    File.exists?(@path).should == true

    im = VIPS::Image.new @path
    im.should match_image(@image)
  end

  it "should create a generic writer" do
    @writer.class.should == VIPS::Writer
    @writer.image.should == @image
  end

  it "should write a tiled TIFF" do
    @tiff_out = tmp('temp.tiff').to_s
    File.unlink(@tiff_out) if File.exists?(@tiff_out)
    @image.tiff(@tiff_out, :layout => :tile)
  end
end
