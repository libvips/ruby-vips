require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg 'wagon.v'
    @binary = @image.more(128).extract_band(1)
  end

  it "should dilate an image given an integer mask" do
    mask = [
        [128, 255, 128],
        [255, 255, 255],
        [128, 255, 128]
    ]
    im = @binary.dilate(mask)
    im.should match_sha1('82b0e8e68504beeca1dc59fe879fe135ae0a90be')
  end

  it "should erode an image given an integer mask" do
    mask = [
        [128, 255, 128],
        [255, 255, 255],
        [128, 255, 128]
    ]
    im = @binary.erode(mask)
    im.should match_sha1('38cf97b262f9baa8409be567d35b609151b51d8f')
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
    im = @binary.zerox_pos
    im.should match_sha1('b6cf701f906e28948acd8b9c1a6abdb29be111ac')
  end

  it "should detect the -ve edges of zero crossings of an image" do
    im = @binary.zerox_neg
    im.should match_sha1('b6cf701f906e28948acd8b9c1a6abdb29be111ac')
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

# 7.40 has revised label stuff
if Spec::Helpers.match_vips_version("> 7.40")
  it "should mark regions of 4-connected pixels with the same color" do
    im, segments = @image.label_regions
    segments.should == 30728
    im.should match_sha1('426bf35dcca03d79354b85375334a677141e7296')
  end
end
end

