require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg('wagon.v')
    @image2 = simg('balloon.v')
  end

  it "should measure image areas" do
    res = @image.measure_area(0, 0, 20, 20, 2, 2, [2, 4])

    res.should == [
      [37.24, 60.36, 66.16],
      [47.52, 78.24, 107.68]
    ]
  end

  it "should gather image stats" do
    res = @image.stats

    # columns 7 and later may give variable results between runs ... they give
    # the image maximum and minimum position, and are the first found rather
    # than the average
 
    # only test the first 6 columns

    res.map { |x| x[0..5] }.should == [
      [3.0, 248.0, 11922736.0, 1581021538.0, 109.62830556475045, 50.18948900355393], 
      [10.0, 248.0, 3820127.0, 491492227.0, 105.37699988966126, 49.5319475110716], 
      [11.0, 205.0, 4096959.0, 541591827.0, 113.01332340284674, 46.558441399535226], 
      [3.0, 192.0, 4005650.0, 547937484.0, 110.49459340174336, 53.90461824817105]
    ]
  end

  it "should calculate the max of an image" do
    @image.max.should == @image.maxpos[2]
  end

  it "should calculate the min of an image" do
    @image.min.should == @image.minpos[2]
  end

  it "should calculate the average of an image" do
    i = sum = 0;
    @image.each_pixel{ |px, x, y| px.each{ |v| sum += v; i += 1 } } 

    @image.avg.should approximate(sum / i.to_f)
  end

  it "should calculate the standard deviation of an image" do
    i = sum = 0
    mean = @image.avg

    @image.each_pixel{ |px, x, y| px.each{ |v| sum += (v - mean)**2; i += 1 } }
    expected_deviation = Math.sqrt(sum / i.to_f)

    @image.deviate.should approximate(expected_deviation)
  end

  def image_maxes(image)
    max = nil
    max_locs = []
    image.each_pixel do |px, x, y|
      if max.nil? || px.max >= max
        max = px.max
        max_locs << [x, y]
      end
    end
    [max, max_locs]
  end

  def image_mins(image)
    min = nil
    min_locs = []
    image.each_pixel do |px, x, y|
      if min.nil? || px.min <= min
        min = px.min
        min_locs << [x, y]
      end
    end
    [min, min_locs]
  end

  it "should return the location of the image max" do
    max, max_locs = image_maxes @image

    mp = @image.maxpos

    mp[2].should == max
    mp[0..1].should be_in_array(max_locs)
  end

  it "should return the location of the image min" do
    min, min_locs = image_mins @image

    mp = @image.minpos

    mp[2].should == min
    mp[0..1].should be_in_array(min_locs)
  end

  it "should return the average location of the image max" do
    pending "variable results in git master right now"
    @image.maxpos_avg.should == [12.0, 116.5, 248.0]
  end

  it "should return an array of n image max locations" do
    vec = @image.bandmean.maxpos(3)
    [143, 138, 204.0].should be_in_array(vec)
    [141, 138, 204.0].should be_in_array(vec)
  end

  it "should return an array of n image min locations" do
    vec = @image.bandmean.minpos(3)
    [110, 157, 9.0].should be_in_array(vec)
    [105, 157, 9.0].should be_in_array(vec)
  end

  it "should convert a multi-band image to a single-band image consisting of the mean of each band" do
    im = @image.bandmean
    im.bands.should == 1

    expected_pel = @image[45, 20].inject{ |a, b| a + b }.to_f / @image.bands
    im[45, 20].should == expected_pel.round
  end

  it "should add two images" do
    im = @image.add(@image)
    im.avg.should == @image.avg * 2

    im[31, 45].each_with_index do |pel, i|
      @image[31, 45][i].should == pel / 2
    end
  end

  it "should subtract two images" do
    im = @image.subtract(@image)
    im.avg.should == 0
  end

  it "should invert an image" do
    im = @image.invert
    (255 - im.avg).should approximate(@image.avg)
  end

  it "should perform a linear transformation on an image" do
    im = @image.lin(0.2, 100)

    expect = @image[81, 21].map{ |v| v * 0.2 + 100 }
    im[81, 21].each_with_index{ |v, i| v.should approximate(expect[i]) }

    im.avg.should approximate(@image.avg * 0.2 + 100)
  end

  it "should perform a linear transformations on each band of an image" do
    transformations = [ [0.2, 0.1, 0.3], [10, 10, 50] ]
    im = @image.lin *transformations

    im.bands.times do |i|
      m = transformations[0][i - 1]
      c = transformations[1][i - 1]
      im[81, 21][i - 1].should approximate(m * @image[81, 21][i - 1] + c)
    end
  end

  it "should multiply two images" do
    im = @image.multiply(@image)
    im[32, 33].should == @image[32, 33].map{ |v| v * v }
  end

  it "should divide two images" do
    im = @image.fliphor
    im2 = @image.divide(im).floor

    im2[41, 1].each_with_index do |v, i|
      v.should == @image[41, 1][i] / im[41, 1][i]
    end

    im.avg.should == @image.avg
  end

  it "should divide by a black (zero) image and return a zero image" do
    if Spec::Helpers.match_vips_version("> 7.22")
      im = @image.divide(VIPS::Image.black(@image.x_size, @image.y_size, 3))
      im.avg.should == 0
    else
      pending "your version of VIPS will segfault when dividing by a zero image"
    end
  end

  it "should calculate the remainders for two images" do
    im = @image.fliphor
    im2 = @image.remainder im

    im2[11, 87].each_with_index do |v, i|
      v.should == @image[11, 87][i] % im[11, 87][i]
    end
  end

  it "should calculate the remainder for one constant" do
    im = @image.remainder(2)
    im[33, 44].should == @image[33, 44].map{ |v| v % 2 }
  end

  it "should calculate the remainder for multiple constants" do
    im = @image.remainder(2, 3, 4)

    pix = @image[33, 44]
    im[33, 44].should == [pix[0] % 2, pix[1] % 3, pix[2] % 4]
  end

  it "should recombine image bands using a matrix" do
    coeff = [
      [0.2, 3.2, 0.1],
      [1.1, 0.0, 2.1]
    ]
    im = @image.recomb(coeff)
    im.avg.should approximate(370.8604)
  end

  it "should find unit vectors from the direction of pixel values" do
    im = @image.sign
    im.avg.should == 1.0
  end

  it "should find the absolute value of an image" do
    im = @image.lin(-1, 0)
    im2 = im.abs

    im2[45, 23][1].should == im[45, 23][1] * -1
  end

  it "should find the floor value of an image" do
    im = @image.srgb_to_xyz
    im2 = im.floor

    im2[3, 4][2].should == im[3, 4][2].floor
  end

  it "should find the nearest integral value of an image" do
    im = @image.srgb_to_xyz
    im2 = im.rint

    im2[3, 4][2].should == im[3, 4][2].round
  end

  it "should find the ceiling value of an image" do
    im = @image.srgb_to_xyz
    im2 = im.ceil

    im2[3, 4][2].should == im[3, 4][2].ceil
  end

  it "should find the linear regression from an array of images" do
    i = @image.bandmean
    i2 = @image.invert.bandmean
    i3 = @image.abs.bandmean

    im = VIPS::Image.linreg(
      [i,  0.4],
      [i2, 0.1],
      [i3, 0.2]
    )
    im.avg.should approximate(64.50617)
  end

  it "should calculate the value of a point in a band in an image" do
    pt = @image.point(:bilinear, 103, 100, 1)
    # 7.40 fixed im_point()
    if Spec::Helpers.match_vips_version("> 7.40")
      pt.should == @image[103, 100][1]
    end

    pt2 = @image.point(:bilinear, 102.5, 100, 0)
    # 7.40 fixed im_point()
    if Spec::Helpers.match_vips_version("> 7.40")
      pt2.should == 92.0
    end
  end

  it "should raise pixel band values in an image to the given power" do
    im = @image.pow(2.3)
    im.avg.ceil.should == @image.multiply(@image).multiply(@image.pow(0.3)).avg.ceil

    im = @image.pow(1.2, 3.1, 0.4);
    exp = @image[32, 11]
    got = im[32, 11]
    got[0].should approximate(exp[0] ** 1.2)
    got[1].should approximate(exp[1] ** 3.1)
    got[2].should approximate(exp[2] ** 0.4)
  end

  it "should raise the given base to the power of image pixel band values" do
    im = @image.expn(1.0002)
    exp = @image[45, 2]

    im[45, 2].each_with_index do |v, i|
      v.should approximate(1.0002 ** exp[i])
    end

    im = @image.expn(1.0002, 1.00000031, 1.00000002)

    got = im[45, 2]
    got[0].should approximate(1.0002     ** exp[0])
    got[1].should approximate(1.00000031 ** exp[1])
    got[2].should approximate(1.00000002 ** exp[2])
  end

  it "should calculate the natural logarithm of pixel band values" do
    im = @image.log

    exp = @image[3, 3]
    im[3, 3].each_with_index do |v, i|
      v.should approximate(Math.log(exp[i]))
    end
  end

  it "should calculate the logarithm to the base 10 of pixel band values" do
    im = @image.log10

    exp = @image[66, 3]
    im[66, 3].each_with_index do |v, i|
      v.should approximate(Math.log10(exp[i]))
    end
  end

  def deg2rad(deg)
    deg * Math::PI / 180
  end

  def rad2deg(rad)
    rad * 180 / Math::PI
  end

  it "should compute sine on pixel band values" do
    im = @image.sin

    exp = @image[66, 3]
    im[66, 3].each_with_index do |v, i|
      v.should approximate(Math.sin(deg2rad(exp[i])))
    end
  end

  it "should compute cosine on pixel band values" do
    im = @image.cos

    exp = @image[66, 3]
    im[66, 3].each_with_index do |v, i|
      v.should approximate(Math.cos(deg2rad(exp[i])))
    end
  end

  it "should compute tangent on pixel band values" do
    im = @image.tan

    exp = @image[66, 3]
    im[66, 3].each_with_index do |v, i|
      v.should approximate(Math.tan(deg2rad(exp[i])))
    end
  end

  it "should compute arc sine on pixel band values" do
    # need pixel values between -1 and 1
    im = @image.clip2fmt(:FLOAT).lin(0.001, 0)
    im2 = im.asin

    exp = im[66, 3]
    im2[66, 3].each_with_index do |v, i|
      v.should approximate(rad2deg(Math.asin(exp[i])))
    end
  end

  it "should compute arc cosine on pixel band values" do
    # need pixel values between -1 and 1
    im = @image.clip2fmt(:FLOAT).lin(0.001, 0)
    im2 = im.acos

    exp = im[66, 3]
    im2[66, 3].each_with_index do |v, i|
      v.should approximate(rad2deg(Math.acos(exp[i])))
    end
  end

  it "should compute arc tangent on pixel band values" do
    im = @image.atan

    exp = @image[66, 3]
    im[66, 3].each_with_index do |v, i|
      v.should approximate(rad2deg(Math.atan(exp[i])))
    end
  end

  # TODO: Need a complex image to test this
  # it "should calculate cross phases between pixel values of two images"
end

