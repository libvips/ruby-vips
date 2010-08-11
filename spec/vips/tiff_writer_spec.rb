require "spec_helper"

describe VIPS::TIFFWriter do
  before :all do
    @image = simg 'wagon.v'
    @writer = VIPS::TIFFWriter.new @image
    @path = tmp('wagon.tiff').to_s
  end

  it "should write a tiff file" do
    @writer.write(@path)

    im = VIPS::Image.tiff @path
    im.x_size.should == @image.x_size
    im.y_size.should == @image.y_size
  end

  it "should write tiff files in all available compression formats" do
    VIPS::TIFFWriter::FORMAT.each do |f|
      @writer.format = f
      @writer.write(@path)

      im = VIPS::Image.tiff @path
      im.x_size.should == @image.x_size
      im.y_size.should == @image.y_size
    end
  end

  it "should write an identical file when using no compression" do
    @writer.compression = :none
    @writer.write @path

    im = VIPS::Image.tiff @path
    im.should match_image(@image)
  end

  it "should write tiff files with different jpeg compression levels" do
    @writer.compression = :jpeg

    @writer.quality = 10
    @writer.write(@path)
    size1 = File.size @path

    @writer.quality = 90
    @writer.write(@path)
    size2 = File.size @path

    size1.should < size2 / 2
  end
  
  it "should store the internal resolution" do
    im = @image.shrink 2, 2
    x_res = im.x_res
    y_res = im.y_res

    im.tiff @path
    im2 = VIPS::Image.tiff @path
    
    im2.x_res.should == x_res
    im2.y_res.should == y_res
  end

  it "should allow setting a custom resolution" do
    @writer.resolution = [0.123, 0.456]
    @writer.write @path

    im = VIPS::Image.tiff @path

    # tiff images are stored at a resolution at 1/10 of vips resolution
    im.x_res.should approximate(0.123 / 10)
    im.y_res.should approximate(0.456 / 10)
  end

  it "should create a tiff writer from an image" do
    @writer.class.should == VIPS::TIFFWriter
    @writer.image.should == @image
  end

  it "should accept options as a has when creating from an image" do
    writer = @image.tiff(nil, :compression => :lzw, :layout => :tile,
      :tile_size => [37, 55])
    writer.compression.should == :lzw
    writer.layout.should == :tile
    writer.tile_size.should == [37, 55]
  end
end
