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
    im.should match_sha1('4c017e1d28bbc4833faad83142231a4a88663289')
  end

  it "should perform a seperable convolution with a float mask" do
    m = VIPS::Mask.new [[1.2, -2.1, 3.0]], 3.4
    im = @image.convsep m
    im.should match_sha1('a3263e83248b79df792d22981ad2c863aa28e887')
  end

  it "should convolve 8 times, rotating 45 deg each time" do
    mask = [
      [ 2, -3, 1 ],
      [ 2,-3, 2 ],
      [ -1, -2, 5]
    ]
    m = VIPS::Mask.new mask, 3
    im = @image.compass m
    im.should match_sha1('e37d7c4a0b2f8863c1ee0760b2de1d47e8359b60')
  end

  it "should convolve twice, rotating 90 deg the second time" do
    mask = [
      [ 2, -3, 4 ],
      [ 2,-3, 2 ],
      [ -1, 1, -2]
    ]
    m = VIPS::Mask.new mask, 3
    im = @image.gradient m
    im.should match_sha1('a6e3e83f58c7599d616722e98f7c586e4b1a8c21')
  end

  it "should convolve 4 times, rotating 45 deg each time" do
    mask = [
      [ 2, -3, 1 ],
      [ 2,-3, 2 ],
      [ -1, -2, 5]
    ]
    m = VIPS::Mask.new mask, 3
    im = @image.lindetect m
    im.should match_sha1('78580ff728cca4722c73f610406ac8eba0604ba9')
  end

  it "should sharpen an image coded in LABS or LABQ format" do
    im = @image.srgb_to_xyz.xyz_to_lab.lab_to_labs.sharpen(7, 1.5, 20, 50, 1, 2)
    # revised in 7.38
    if Spec::Helpers.match_vips_version("> 7.38")
      im.should match_sha1('f6fdf3136cc01b3f267b80450f4c4419ae22b77a')
    end
  end

  it "should find horizontal differences between adjacent pixels" do
    im = @image.extract_band(0).grad_x
    im.should match_sha1('36e8ebe274179b760d0903adfdd683ee4317dd2f')
  end

  it "should find vertical differences between adjacent pixels" do
    im = @image.extract_band(0).grad_y
    im.should match_sha1('d9d95867bed664bfe1680d02b9163a7d99406a60')
  end

  it "should calculate a fast correlation surface between two images" do
    segment = @image.extract_area(120, 120, 30, 30, 1, 1).scale
    im = @image.extract_band(0).fastcor(segment)
    # revised in 7.38
    if Spec::Helpers.match_vips_version("> 7.38")
      im.should match_sha1('e14c148c8b9f86707ac911f61b166e9f2e1cf54d')
    end
  end

  it "should calculate a correlation surface between two images" do
    segment = @image.extract_area(120, 120, 30, 30, 1, 1).scale
    im = @image.extract_band(0).spcor(segment)
    # revised in 7.38
    if Spec::Helpers.match_vips_version("> 7.38")
      im.should match_sha1('d2c2bcf3b6d28f9ae705dadae82b11c2df1bf344')
    end
  end

  it "should calculate a correlation surface between two images via gradients" do
    segment = @image.extract_area(120, 120, 30, 30, 1, 1).scale
    im = @image.extract_band(0).gradcor(segment)
    # revised in 7.38
    if Spec::Helpers.match_vips_version("> 7.38")
      im.should match_sha1('3f0193e24f89b5c5b98f56e48c1ee858ec2a2c01')
    end
  end

  it "should generate an image where every pixel represents the contrast within a window" do
    im = @image.bandmean.contrast_surface(10, 2)
    # revised in 7.38
    if Spec::Helpers.match_vips_version("> 7.38")
      im.should match_sha1('3bee875c3f12c263bf96ea8881e5b76b10f1ae7e')
    end
  end

  it "should add gaussian noise to an image" do
    im = @image.addgnoise(128)
    im.should_not match_image(@image)
  end
end

