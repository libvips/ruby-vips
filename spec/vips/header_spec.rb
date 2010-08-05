require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = VIPS::Image.new sample_join('huge.jpg')
  end

  it "should get the header as a hash" do
    h = VIPS::Image.new(sample("huge.jpg").to_s).header_to_hash
    h['Bbits'].should == 8
    h['exif-Date and Time'].should == "2005:04:28 19:23:38 (ASCII, 20 bytes)"
  end

  it "should get exif data as a hash" do
    h = VIPS::Image.new(sample("huge.jpg").to_s).exif_to_hash
    h['exif-Maker Note'].should == "770 bytes undefined data (Undefined, 770 bytes)"
  end

  it "should indicate that an image has attached exif data" do
    @image.exif?.should be_true
  end

  it "should indicate that an image has an attached color profile" do
    @image.icc?.should be_false
    im = VIPS::Image.new sample("icc.jpg").to_s
    im.icc?.should be_true
  end

  it "should get raw exif data" do
    exif = @image.exif
    pending "use an exif reader to validate the exif binary"
  end

  it "should get raw icc data" do
    im = VIPS::Image.new(sample("icc.jpg").to_s)
    pending "use an icc reader to validate the icc data"
  end

  it "should remove icc from the header", :vips_lib_version => "> 7.20" do
    with_icc_path = tmp('with_icc.jpg').to_s
    without_icc_path = tmp('without_icc.jpg').to_s

    im = VIPS::Image.new(sample("icc.jpg").to_s)

    im.to_jpeg(with_icc_path);
    im.remove_icc.to_jpeg(without_icc_path);
    
    File.size(with_icc_path).should > File.size(without_icc_path)
  end

  it "should remove exif from the header", :vips_lib_version => "> 7.20" do
    with_path = tmp('with_exif.jpg').to_s
    without_path = tmp('without_exif.jpg').to_s

    im = VIPS::Image.new(sample("huge.jpg").to_s)

    im.to_jpeg(with_path);
    im.remove_exif.to_jpeg(without_path);
    
    File.size(with_path).should > File.size(without_path)
  end


end

