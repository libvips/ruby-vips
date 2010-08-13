require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg('wagon.v')
  end

  it "should perform integer mask convolution" do
    mask = [
      [1,  2,  3],
      [4,  5, -6],
      [7, -8, -9]
    ]
    m = VIPS::Mask.new mask, 2
    im = @image.conv(m)
    # TODO: some kind of validation of the generated image
  end

  it "should perform float mask convolution" do
    mask = [
      [1,    2,    3],
      [4, -0.2,   -6],
      [7,   -8, -0.9]
    ]
    m = VIPS::Mask.new mask, 2
    im = @image.conv(m)
    # TODO: some kind of validation of the generated image
  end

  it "should perform a seperable convolution with an int mask" do
    m = VIPS::Mask.new [[1, -2, 3]], 3
    im = @image.convsep m
    im.should match_sha1('1389e7d7fb3f1646e774ac69d6a8802c26652aa5')
  end

  it "should perform a seperable convolution with a float mask" do
    m = VIPS::Mask.new [[1.2, -2.1, 3.0]], 3.4
    im = @image.convsep m
    im.should match_sha1('e6c79ddd45c93b31a77c830c925561c6d2139529')
  end

  it "should convolute by reapplying the mask 8 times and rotating 45 deg each time" do
    mask = [
      [ 2, -3, 1 ],
      [ 2,-3, 2 ],
      [ -1, -2, 5]
    ]
    m = VIPS::Mask.new mask, 3
    im = @image.compass m
    im.should match_sha1('e37d7c4a0b2f8863c1ee0760b2de1d47e8359b60')
  end

  it "should convolute by applying mask twice, rotated 90 deg the second time" do
    mask = [
      [ 2, -3, 4 ],
      [ 2,-3, 2 ],
      [ -1, 1, -2]
    ]
    m = VIPS::Mask.new mask, 3
    im = @image.gradient m
    im.should match_sha1('a6e3e83f58c7599d616722e98f7c586e4b1a8c21')
  end

  it "should convolute by reapplying the mask 4 times and rotating 45 deg each time" do
    mask = [
      [ 2, -3, 1 ],
      [ 2,-3, 2 ],
      [ -1, -2, 5]
    ]
    m = VIPS::Mask.new mask, 3
    im = @image.lindetect m
    im.should match_sha1('78580ff728cca4722c73f610406ac8eba0604ba9')
  end

  it "should sharpen an image coded in LABS or LABQ format", :vips_lib_version => "> 7.23" do
    im = @image.srgb_to_xyz.xyz_to_lab.lab_to_labs.sharpen(7, 1.5, 20, 50, 1, 2)
    im.should match_sha1('f46d4ba8b0c16e19af95fcd3518caae0a7c2170d')
  end

  it "should find horizontal differences between adjacent pixels" do
    im = @image.extract_band(0).grad_x
    im.should match_sha1('36e8ebe274179b760d0903adfdd683ee4317dd2f')
  end

  it "should find vertical differences between adjacent pixels" do
    im = @image.extract_band(0).grad_y
    im.should match_sha1('d9d95867bed664bfe1680d02b9163a7d99406a60')
  end

  it "should calculate a fast corrolation surface between two images" do
    segment = @image.extract_area(120, 120, 30, 30, 1, 1).scale
    im = @image.extract_band(0).fastcor(segment)
    pending "not sure how to get a useful image from this method"
  end

  it "should calculate a corrolation surface between two images" do
    segment = @image.extract_area(120, 120, 30, 30, 1, 1).scale
    im = @image.extract_band(0).spcor(segment)
  end

  it "should calculate a corrolation surface between two images via gradients" do
    segment = @image.extract_area(120, 120, 30, 30, 1, 1).scale
    im = @image.extract_band(0).gradcor(segment)
    im.should match_sha1('83c74a9d78eb4023e68fabf88aff052aa8b3d757')
  end

  it "should generate an image where every pixel represents the contrast within a window" do
    im = @image.bandmean.contrast_surface(10, 2)
    im.should match_sha1('bf06f18589d8a264d26ecf8fbc3af283678acda8')
  end

  it "should add gaussian noise to an image" do
    im = @image.addgnoise(128)
    im.should_not match_image(@image)
  end
end

