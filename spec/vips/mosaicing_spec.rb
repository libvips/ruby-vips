require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg 'wagon.v'
    im = simg 'huge.jpg'
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

  it "should merge two images from left to right, doing an affine transformation to align two points on each image" do
    im = @image.lrmerge1(@image2, 199, 29, 27, 20, 212, 146, 19, 144, 50)
    # 7.40 has revised mosaic stuff
    if Spec::Helpers.match_vips_version("> 7.40")
      im.should match_sha1('e030e36d6097c3bdb4376a14bcdbc03caf5991dc')
    end
  end

  it "should merge two images from top to bottom, doing an affine transformation to align two points on each image" do
    im = @image.tbmerge1(@image2, 40, 140, 20, 30, 200, 150, 190, 25, 50)
    # 7.40 has revised mosaic stuff
    if Spec::Helpers.match_vips_version("> 7.40")
      im.should match_sha1('ac53cf11369892c5f0a388833c1f6f2fba8fed20')
    end
  end

  # it "should mosaic two image top to bottom (Image#tbmosaic)"
  # it "should mosaic two images left to right (Image#lrmosaic)"
#  it "should mosaic two image top to bottom, doing an affeine transformation to align two points on each image (Image#tbmosaic1)"
#  it "should mosaic two images left to right, doing an affeine transformation to align two points on each image (Image#lrmosaic1)"
#  it "should correleate two images (Image#correlate)"
#  it "should adjust the balance of an image put together with mosaic or merge (Image#global_balance)"
#  it "should adjust the balance of an image put together with mosaic or merge (Image#global_balancef)"
#  it "should substitute images in a mosaic based on filenames (Image#remosaic)"
#  it "should align the bands of an image (Image#align_bands)"
#  it "should do something complicated-looking (Image#maxpos_subpel)"
end

