require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg 'wagon.v'
    @image2 = simg 'balloon.v'
  end

  it "should return correct image attributes for a three band image" do
    im = @image
    im.bands.should == 3
    im.band_fmt.should == :UCHAR
    im.coding.should == :NONE
    im.vtype.should == :sRGB
    im.x_res.should approximate(0.333)
    im.y_res.should approximate(0.333)
    im.x_offset.should == 0
    im.y_offset.should == 0
    im.filename.should =~ /wagon.v$/
    im.kill.should == 0
  end

  it "should return correct image attributes for a one band image" do
    im = @image2
    im.bands.should == 1
    im.band_fmt.should == :UCHAR
    im.vtype.should == :B_W
    im.x_res.should approximate(1.574803)
    im.y_res.should approximate(1.574803)
    im.filename.should =~ /balloon.v$/
  end

  it "should get correct image dimensions" do
    @image.x_size.should == 228
    @image.y_size.should == 159
  end

  it "should get the size of a one band element" do
    @image.sizeof_element.should == 1
  end

  it "should get the size of one pixel" do
    @image.sizeof_pel.should == 3
  end

  it "should get the size of one line of pixels" do
    @image.sizeof_line.should == 684
  end

  it "should get the number of band elements in a line" do
    @image.n_elements.should == 684
  end

  it "should allow access to individual pixel values on a uchar image" do
    @image[40, 131].should == [161, 136, 113] # pixels verified in the gimp
    @image[0, 0].should == [29, 51, 20]
    @image[1, 0].should == [29, 42, 12]
    @image[0, 1].should == [28, 51, 20]
    @image[227, 158].should == [29, 42, 12]
  end

  it "should return one-band pixel values directly" do
    @image2[40, 20].should == 255
  end

  it "should allow access to individual pixel values in a signed char image" do
    im = @image.clip2fmt :CHAR
    im[40, 131].should == [127, 127, 113]
    im[0, 0].should == [29, 51, 20]
  end

  it "should return negative signed char pixels" do
    im = @image.clip2fmt(:CHAR).lin(-1, 0)
    im[40, 131].should == [-127, -127, -113]
    im[0, 0].should == [-29, -51, -20]
  end

  it "should return pixels when they have huge numbers" do
    im = @image.pow(9)

    expected = @image[23, 45].map{|v| v**9}
    got = im[23, 45]

    got[0].should approximate(expected[0])
    got[1].should approximate(expected[1])
  end
end

