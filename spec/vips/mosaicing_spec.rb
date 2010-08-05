require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = VIPS::Image.new sample_join('wagon.v')
    im = VIPS::Image.new(sample_join('huge.jpg'))
    @image2 = im.extract_area(100, 100, @image.x_size, @image.y_size)
  end

  it "should merge two images from left to right" do
    im = @image.lrmerge(@image2, 0, 0, 400)
    x, y = im.x_size - 1, im.y_size - 1

    im[0, 0].should == @image[0, 0]
    im[0, y].should == @image[0, y]
    im[x, y].should == @image2[x, y]
    im[x, 0].should == @image2[x, 0]
  end

  it "should merge two images from top to bottom" do
    im = @image.tbmerge(@image2, 0, 0, 400)
    x, y = im.x_size - 1, im.y_size - 1

    im[0, 0].should == @image[0, 0]
    im[x, 0].should == @image[x, 0]
    im[x, y].should == @image2[x, y]
    im[0, y].should == @image2[0, y]
  end

  # TODO: May find clues to this issue in im_lrmerge.c:398
  it "should merge two images from left to right, doing an affeine transformation to align two points on each image" do
    im = @image.lrmerge1(@image2, 10, 20, 20, 30, 40, 30, 100, 20, 400)
    pending "there appears to be a bug in the *merge methods where the value black is not ignored"
  end

  it "should merge two images from top to bottom, doing an affeine transformation to align two points on each image" do
    im = @image.tbmerge1(@image2, 40, 20, 20, 30, 40, 30, 10, 20, 400)
    pending "there appears to be a bug in the *merge methods where the value black is not ignored"
  end

  it "should mosaic two image top to bottom (Image#tbmosaic)"
  it "should mosaic two images left to right (Image#lrmosaic)"
  it "should mosaic two image top to bottom, doing an affeine transformation to align two points on each image (Image#tbmosaic1)"
  it "should mosaic two images left to right, doing an affeine transformation to align two points on each image (Image#lrmosaic1)"
  it "should correleate two images (Image#correlate)"
  it "should adjust the balance of an image put together with mosaic or merge (Image#global_balance)"
  it "should adjust the balance of an image put together with mosaic or merge (Image#global_balancef)"
  it "should substitute images in a mosaic based on filenames (Image#remosaic)"
  it "should align the bands of an image (Image#align_bands)"
  it "should do something complicated-looking (Image#maxpos_subpel)"
end

