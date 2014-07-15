require "spec_helper"

describe VIPS::JPEGWriter do
  before :all do
    @image = simg 'wagon.v'
    @writer = VIPS::JPEGWriter.new @image
    @path = tmp('wagon.jpg').to_s
    @icc_path = sample('lcd.icc').to_s
  end

  it "should write to a jpeg file" do
    @writer.write(@path)

    im = VIPS::Image.jpeg @path
    im.x_size.should == @image.x_size
    im.y_size.should == @image.y_size
  end

  it "should write a tiny jpeg file to memory", :vips_lib_version => "> 7.20" do
    im = VIPS::Image.black(10, 10, 1)
    s = im.jpeg.to_memory
    if Spec::Helpers.match_vips_version(">= 7.28")
      reader = VIPS::JPEGReader.new(s)
      im2 = reader.read_buffer
      im2.x_size.should == im.x_size 
    end
  end

  it "should write a large jpeg file to memory" do
    im = VIPS::Image.black(1000, 1000, 3)
    s = im.jpeg.to_memory
    if Spec::Helpers.match_vips_version(">= 7.28")
      reader = VIPS::JPEGReader.new(s)
      im2 = reader.read_buffer
      im2.x_size.should == im.x_size 
    end
  end

  it "should detect icc headers and preserve the input image icc header" do
    im = simg 'icc.jpg'
    im2 = im.shrink 5, 5
    jpeg = im2.jpeg

    jpeg.icc?.should == true

    jpeg.write @path
    im3 = VIPS::Image.jpeg @path
    jpeg2 = im3.jpeg
    jpeg2.icc?.should == true
    jpeg.icc.should == jpeg2.icc
  end

  it "should remove the icc header", :vips_lib_version => ">= 7.22" do
    im = simg 'icc.jpg'
    im2 = im.shrink 5, 5
    jpeg = im2.jpeg

    jpeg.remove_icc
    jpeg.icc?.should == false

    jpeg.write @path
    im3 = VIPS::Image.jpeg @path
    jpeg2 = im3.jpeg
    jpeg2.icc?.should == false
  end

  it "should allow attaching an external icc profile to the header", :vips_lib_version => ">= 7.22" do
    im = simg 'icc.jpg'
    im2 = im.shrink 5, 5
    jpeg = im2.jpeg

    icc = File.open(@icc_path, 'rb').read

    jpeg.remove_icc
    jpeg.icc = icc
    jpeg.icc?.should == true
    jpeg.icc.should == icc

    jpeg0 = im.jpeg

    jpeg.write @path
    im3 = VIPS::Image.jpeg @path
    jpeg2 = im3.jpeg

    jpeg2.icc.should_not == jpeg0.icc
    jpeg2.icc.should == jpeg.icc
  end

  it "should detect exif headers and have exif header when the input image had one" do
    im = simg 'huge.jpg'
    im2 = im.shrink 5, 5
    jpeg = im2.jpeg

    jpeg.exif?.should == true

    jpeg.write @path
    im3 = VIPS::Image.jpeg @path
    jpeg2 = im3.jpeg
    jpeg2.exif?.should == true
  end

  it "should remove the exif header", :vips_lib_version => ">= 7.22" do
    im = simg 'huge.jpg'
    im2 = im.shrink 5, 5
    jpeg = im2.jpeg

    jpeg.remove_exif
    jpeg.exif?.should == false

    jpeg.write @path
    im3 = VIPS::Image.jpeg @path
    jpeg2 = im3.jpeg
    jpeg2.exif.size.should < 200
  end

  it "should survive a method missing call" do
    im = simg 'icc.jpg'
    jpeg = im.jpeg
    lambda{ jpeg.xyz? }.should raise_error{NoMethodError}
  end

  it "should allow attaching an external exif to the header" do
    im = simg 'huge.jpg'
    exif = im.jpeg.exif

    jpeg = @image.jpeg
    jpeg.exif?.should == false

    jpeg.exif = exif
    jpeg.write @path

    jpeg2 = VIPS::Image.jpeg(@path).jpeg
    jpeg2.exif.size.should > 9000
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

  it "should generate smaller memory images with lower quality settings" do
    @writer.quality = 10
    memjpeg = @writer.to_memory

    @writer.quality = 90
    memjpeg2 = @writer.to_memory

    memjpeg.size.should < memjpeg2.size / 2
  end

  it "should write smaller images with lower quality settings" do
    @writer.quality = 10
    @writer.write(@path)
    size1 = File.size @path

    @writer.quality = 90
    @writer.write(@path)
    size2 = File.size @path

    size1.should < size2 / 2
  end

  it "should create a jpeg writer from an image" do
    @writer.class.should == VIPS::JPEGWriter
    @writer.image.should == @image
  end

  it "should accept options on creation from an image" do
    writer = @image.jpeg(nil, :quality => 22)
    writer.quality.should == 22
  end

  it "should allow access to header fields" do
    @writer.x_size.should == @image.x_size
    @writer.y_size.should == @image.y_size
  end
end
