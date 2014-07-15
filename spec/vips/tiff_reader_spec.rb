require "spec_helper"

describe VIPS::TIFFReader do
  before :all do
    @image = simg 'wagon.v'
    @path = formats('wagon.tiff').to_s
    @image.tiff @path
  end

  it "should read a tiff image" do
    im = VIPS::TIFFReader.new(@path).read
    im.should match_image @image
  end

  it "should recognize a tiff image" do
    vips_res = VIPS::TIFFReader.recognized? sample('wagon.v').to_s
    vips_res.should == false

    tiff_res = VIPS::TIFFReader.recognized? @path
    tiff_res.should == true
  end

  it "should recognize options" do
    tiff = VIPS::TIFFReader.new(@path, :page_number => 1)
    tiff.page_number.should == 1
  end

  it "should read the width from the header" do
    tiff = VIPS::TIFFReader.new @path
    tiff.read.x_size.should == @image.x_size
  end
end
