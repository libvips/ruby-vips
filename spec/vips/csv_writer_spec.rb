require "spec_helper"

describe VIPS::CSVWriter do
  before :all do
    @image = simg('wagon.v').bandmean
    @writer = @image.to_csv
    @path = tmp('wagon.csv').to_s
  end

  it "should write a csv file" do
    @writer.write(@path)

    im = VIPS::Image.read_csv @path
    pending "figure out why a csv image doesn't perfectly match its original"

    im.should match_image(@image)
  end

  it "should write a csv file with an alternate separator character" do
    @writer.separator = ','
    @writer.write(@path)

    im = VIPS::Image.read_csv "#{@path}:sep:,"
    pending "figure out why a csv image doesn't perfectly match its original"
    im.should match_image(@image)
  end

  it "should create a csv writer" do
    writer = @image.to_csv
    writer.class.should == VIPS::CSVWriter
    writer.image.should == @image
  end

  it "should accept options on creation from an image" do
    writer = @image.to_csv(:separator => "|")
    writer.separator.should == "|"
  end
end

