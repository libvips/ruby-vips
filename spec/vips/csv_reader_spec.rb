require "spec_helper"

describe VIPS::CSVReader do
  before :all do
    @image = simg('wagon.v').bandmean
    @path = formats('wagon.csv').to_s
    @image.csv @path
    @reader = VIPS::CSVReader.new @path
  end

  it "should recognize options in the init hash" do
    reader = VIPS::CSVReader.new @path, :line_skip => 3, :whitespace => '_',
      :separator => '|', :line_limit => 5

    reader.line_skip.should == 3
    reader.path.should == @path
    reader.whitespace.should == '_'
    reader.separator.should == '|'
    reader.line_limit.should == 5
  end

  it "should read a csv image" do
    im = @reader.read
    im.clip2fmt(:UCHAR).should match_image(@image)
  end

  it "should read the dimensions from the header" do
    @reader.x_size.should == @image.x_size
    @reader.y_size.should == @image.y_size
  end
end
