require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg 'wagon.v'
  end

  it "should perform a partial affine transformation with interpolator " do
    im = @image.affinei(:bilinear, 2.1, 0, 0, 2.1, 70, 20, 200, 100, 300, 400)
    im.x_size.should == 300
    im.y_size.should == 400
  end

  it "should perform a full affine transformation with interpolator " do
    im_resized = @image.affinei :bilinear, 0.2, 0, 0, 0.2, 0, 0
    im_resized.x_size.should == (@image.x_size / 5.to_f).round

    im_resized2 = @image.affinei :nearest, 4, 0, 0, 4, 0, 0
    im_resized2.x_size.should == @image.x_size * 4
  end

  it "should perform a resizing-only affine transformation" do
    im_resized = @image.affinei_resize(:bilinear, 1/3.to_f)
    im_resized.x_size.should == @image.x_size / 3
    im_resized.y_size.should == @image.y_size / 3
  end

  it "should stretch an image by 3%" do
    w = @image.x_size
    new_x = 34*(w / 33) + w%33 - 3
    new_y = @image.y_size - 3

    im_uchar = @image.clip2fmt :USHORT
    im = im_uchar.stretch3(0, 0)
    im.x_size.should == new_x
    im.y_size.should == new_y
  end

  it "should shrink images without interpolation" do
    im = @image.shrink 2, 3
    im.x_size.should == @image.x_size / 2
    im.y_size.should == @image.y_size / 3

    @image.shrink(3).should match_image(@image.shrink(3, 3))
  end

  it "should shrink images via right shift operations and convert to band format" do
    im = @image.rightshift_size 2, 3, :UCHAR
    im.band_fmt.should == :UCHAR
    im.x_size.should == @image.x_size >> 2
    im.y_size.should == @image.y_size >> 3
  end

  it "should resample an image based on common points with another image" do
    # TODO: the docs don't explain what the xs/ys params do
    # im = @image.match_linear(@image, 10, 20, 1, 1, 30, 15, 1, 1)
  end

  it "should resample an image based on common points with another image, with search" do
    # TODO: the docs don't explain what the xs/ys params do
    # im = @image.match_linear_search(@image, 10, 20, 1, 1, 30, 15, 1, 1, 30, 30)
  end
end

