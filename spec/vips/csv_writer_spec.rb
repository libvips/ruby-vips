require "spec_helper"

describe VIPS::CSVWriter do
  before :all do
    @image = simg('wagon.v').bandmean
    @writer = VIPS::CSVWriter.new @image
    @path = tmp('wagon.csv').to_s
  end

  it "should write a csv file" do
    @writer.write(@path)

    im = VIPS::Image.csv @path
    im.clip2fmt(:UCHAR).should match_image(@image)
  end

  it "should write a csv file with an alternate separator character" do
    @writer.separator = '|'
    @writer.write(@path)

    im = VIPS::Image.csv @path, :separator => '|'
    im.clip2fmt(:UCHAR).should match_image(@image)
  end

  it "should create a csv writer" do
    @writer.class.should == VIPS::CSVWriter
    @writer.image.should == @image
  end

  it "should accept options on creation from an image" do
    writer = VIPS::CSVWriter.new @image, :separator => "|"
    writer.separator.should == "|"
  end
end
