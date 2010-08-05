require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = VIPS::Image.new(sample_join('wagon.jpg'))
  end

  it "should perform integer mask convolution" do
    mask = [
      [1,  2,  3],
      [4,  5, -6],
      [7, -8, -9]
    ]
    m = VIPS::Mask.new mask, 2, 0
    im = @image.conv(m)
    # TODO: some kind of validation of the generated image
  end

  it "should perform float mask convolution" do
    mask = [
      [1,    2,    3],
      [4, -0.2,   -6],
      [7,   -8, -0.9]
    ]
    m = VIPS::Mask.new mask, 2, 0
    im = @image.conv(m)
    # TODO: some kind of validation of the generated image
  end

  it "should perform a seperable convolution with an int mask" do
    m = VIPS::Mask.new [[1, -2, 3]], 3, 0
    im = @image.convsep m
    im.should match_sha1('437ca32e319986a4fd58d937045fe6f931ba46ed')
  end

  it "should perform a seperable convolution with a float mask" do
    m = VIPS::Mask.new [[1.2, -2.1, 3.0]], 3.4, 0
    im = @image.convsep m
    im.should match_sha1('46c89a987baf1d4023dd482b7bad3559e10a6231')
  end

  it "should convolute by reapplying the mask 8 times and rotating 45 deg each time" do
    mask = [
      [ 2, -3, 1 ],
      [ 2,-3, 2 ],
      [ -1, -2, 5]
    ]
    m = VIPS::Mask.new mask, 3, 0
    im = @image.compass m
    im.should match_sha1('bfd610e8ae58b47b0b8312783320de2c0627d1bb')
  end

  it "should convolute by applying mask twice, rotated 90 deg the second time" do
    mask = [
      [ 2, -3, 4 ],
      [ 2,-3, 2 ],
      [ -1, 1, -2]
    ]
    m = VIPS::Mask.new mask, 3, 0
    im = @image.gradient m
    im.should match_sha1('d7d998925d67fed59c0084030b65909b925dad26')
  end

  it "should convolute by reapplying the mask 4 times and rotating 45 deg each time" do
    mask = [
      [ 2, -3, 1 ],
      [ 2,-3, 2 ],
      [ -1, -2, 5]
    ]
    m = VIPS::Mask.new mask, 3, 0
    im = @image.lindetect m
    im.should match_sha1('1f4815b6e7597893c22058dfc71c0bfd0185c3c4')
  end

  it "should sharpen an image coded in LABS or LABQ format", :vips_lib_version => "> 7.23" do
    im = @image.srgb_to_xyz.xyz_to_lab.lab_to_labs.sharpen(7, 1.5, 20, 50, 1, 2)
    im.should match_sha1('f46d4ba8b0c16e19af95fcd3518caae0a7c2170d')
  end

  it "should find horizontal differences between adjacent pixels" do
    im = @image.extract_band(0).grad_x
    im.should match_sha1('e96c97fefa3058a05af702bc43a303849a7de998')
  end

  it "should find vertical differences between adjacent pixels" do
    im = @image.extract_band(0).grad_y
    im.should match_sha1('180c460e449317193106d9e5045c07cc49bf4c8a')
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
    im.should match_sha1('526df1cde521d31c3f4e4c0de5fd6faef84849b8')
  end

  it "should generate an image where every pixel represents the contrast within a window" do
    im = @image.bandmean.contrast_surface(10, 2)
    im.should match_sha1('6eb425ceda2958552a8b3984fdaebc203d239ce1')
  end

  it "should add gaussian noise to an image" do
    im = @image.addgnoise(128)
    im.should_not match_image(@image)
  end
end

