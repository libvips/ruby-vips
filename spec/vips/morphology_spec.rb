require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg 'wagon.v'
  end

  # TODO: "these methods operate on 1-channel binary images (b/w)" 
  it "should dilate an image given an integer mask" do
    pending "Image#dilate"
  end

  it "should erode an image given an integer mask" do
    pending "Image#erode"
  end

  it "should rank pixels in a given window of the image and create a new image with the order-th pixels" do
    im = @image.rank(15, 15, 2)
    im.should match_sha1('e440d0b84c2c701f8efc49c49daf3dd94ae7bf45')
  end

  it "should rank pixels from multiple images and create a new image with the index-th pixel" do
    im = simg 'huge.jpg'
    im = im.extract_area(0, 0, @image.x_size, @image.y_size)
    im2 = im.rank_image(1, @image.scale)
  end

  it "should generate an image with the max value from all given images" do
    im = simg 'huge.jpg'
    im = im.extract_area(0, 0, @image.x_size, @image.y_size)
    im.maxvalue(@image.scale).should match_image(im.rank_image(0, @image.scale))
  end

  it "should calculate the number of black (dark) white (light) transitions for lines in an image" do
    @image.bandmean.cntlines_h.should approximate(3.964)
    @image.bandmean.cntlines_v.should approximate(4.503)
  end

  it "should detect the +ve edges of zero crossings of an image" do
    im = @image.zerox_pos
    pending "need validation of Image#zerox_pos"
  end

  it "should detect the -ve edges of zero crossings of an image" do
    im = @image.zerox_neg
    pending "need validation of Image#zerox_neg"
  end

  it "should find the position of the first non-zero pixel from the top and from the left" do
    im = @image.bandmean.profile_v
    im.x_size.should == @image.x_size
    im.y_size.should == 1
    im[3, 0].should == 0

    im = @image.bandmean.profile_h
    im.x_size.should == 1
    im.y_size.should == @image.y_size
    im[0, 3].should == 0
  end

  it "should mark regions of 4-connected pixels with the same color" do
    im, segments = @image.label_regions
    segments.should == 15363
    im.should match_sha1('bdd5a7e8ebdf3358757fa988940eb3bf811cce1d')
  end
end

