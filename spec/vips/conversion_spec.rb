require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg 'wagon.v'
  end

  it "should convert a uchar image to ushort" do
    im_ushort = @image.clip2fmt :USHORT
    im_ushort.band_fmt.should == :USHORT
  end

  it "should convert a single band image to a mask" do
    mask = @image.bandmean.to_mask
    mask.xsize.should == @image.x_size
  end

  it "should convert a double mask to an image" do
    coeffs = [
      [2.3, 3.3, 4.3],
      [1.2, 3.4, 5.6],
      [6.7, 7.7, 7.7]
    ]
    mask = VIPS::Mask.new coeffs
    im = mask.to_image
    im.y_size.should == mask.ysize
  end

  it "should dup an image" do
    im = @image.dup
    im2 = im.dup
    im3 = im2.dup
    im = "foo"
    im2 = "bar"
    im3.avg.should == @image.avg
  end

  it "should create pass-through copies of files" do
    im = @image.invert.copy_file
    im.x_size.should == @image.x_size
  end

  it "should scale the color range in an image" do
    im = @image.scale
    im.max.should == 255
    im.min.should == 0
  end

  it "should turn an image into an 8-bit image by discarding bits" do
    im = @image.multiply(@image)
    im.band_fmt.should == :USHORT
    im2 = im.msb
    im2.band_fmt.should == :UCHAR
  end

  it "should turn an individual band in an image to 8-bit by discarding bits" do
    im = @image.multiply(@image)
    im2 = im.msb(0)
    im3 = (im >> 8).extract_band(0)
    im2.equal(im2).min.should == 255
  end

  it "should swap the byte ordering of image data" do
    # make a u16 image
    im = @image.multiply(@image)
    im2 = im.copy_swap

    # just test pixel zero
    px1 = im[0,0][0]
    px2 = im2[0,0][0]
    ((px1 >> 8) | ((px1 & 255) << 8)).should == px2
  end

  it "should combine a real image and an imaginary image into a complex image" do
    im = @image.ri2c(@image.invert)
    im.band_fmt.should == :COMPLEX
  end

  it "should extract the imaginary part from a complex image" do
    im = @image.ri2c(@image.invert)
    im2 = im.c2imag
    im2.band_fmt.should == :FLOAT
  end

  it "should extract the real part from a complex image" do
    im = @image.ri2c(@image.invert)
    im2 = im.c2real
    im2.band_fmt.should == :FLOAT
  end

  it "should convert a complex image between rectangular and polar coordinates" do
    im = @image.ri2c(@image.invert)
    im2 = im.c2amph.c2rect
    im3 = im2.c2real
    im4 = (im3 - @image).abs
    # we can get small rounding errors
    im4.max.should < 0.001
  end

  it "should scale the power spectrum of an image" do
    im = @image.scaleps
    im.avg.should approximate(224.63862)
  end

  it "should add false color to a 1-band image, maximizing minor changes in brightness" do
    im = @image.scale.bandmean
    im2 = im.falsecolour
    im2.bands.should == 3
  end

  it "should generate a one-band image of gaussian noise" do
    im = VIPS::Image.gaussnoise 400, 200, 128.0, 30.2
    im.x_size.should == 400
    im.y_size.should == 200
    im.avg.should approximate(128)
  end

  it "should generate a black image" do
    im = VIPS::Image.black 200, 100, 3
    im.x_size.should == 200
    im.y_size.should == 100
    im.min.should == 0.0
    im.max.should == 0.0
  end

  it "should generate text to a new image" do
    im = VIPS::Image.text "Hello VIPS!", "sans 12", 300, 0, 72
    im.x_size.should == 65
  end

  it "should extract a band from an image" do
    im = @image.extract_band 0
    im2 = @image.extract_band 1
    im3 = @image.extract_band 2

    ((im.avg + im2.avg + im3.avg) / 3).should approximate(@image.avg)
  end

  it "should extract a band from an image" do
    im = @image.extract_band 0
    im2 = @image.extract_band 1, 2

    ((im.avg + im2.avg * 2) / 3).should approximate(@image.avg)
  end

  it "should extract an area from an image" do
    im = @image.extract_area(20, 22, 100, 101)
    im.x_size.should == 100
    im.y_size.should == 101
    im.bands.should == 3
    im.avg.should approximate(123.538)
  end

  it "should extract bands from an area from an image" do
    im = @image.extract_area(22, 29, 100, 101, 1, 1)
    im.x_size.should == 100
    im.y_size.should == 101
    im.bands.should == 1
    im.avg.should approximate(123.3526)
  end

  it "should embed an image inside a larger generated image" do
    im = @image.embed :mirror, 100, 150, 500, 600
    im.avg.should approximate(111.19658)
  end

  it "should join images by band" do
    im = @image.extract_band 0
    im2 = @image.extract_band 1
    im3 = @image.extract_band 2

    im4 = im2.bandjoin(im3).bandjoin(im)
    im4.avg.should == @image.avg
  end

  it "should join n arrays by band" do
    im = @image.extract_band 0
    im2 = @image.extract_band 1
    im3 = @image.extract_band 2

    im4 = im.bandjoin im2, im3
    im4.avg.should == @image.avg
  end

  it "should join an image on itself repeatedly without segfaulting" do
    @image.bandjoin @image, @image, @image
  end

  it "should insert one image into the other, filling gaps in with black" do
    im = @image.insert(@image, 100, 100)
    im.x_size.should == @image.x_size + 100
    im.y_size.should == @image.y_size + 100
  end

  it "should insert one image into the other cutting out anything beyond the boundaries of the original" do
    im = @image.insert_noexpand(@image, 100, 100)
    im.x_size.should == @image.x_size
    im.y_size.should == @image.y_size
  end

  it "should insert one image into another n times" do
    text = VIPS::Image.text "Hello VIPS!", "sans 12", 300, 0, 72
    text_3band = text.bandjoin text, text
    im = @image.insert(text_3band, [100, 200], [34, 2], [600, 900])
    im.avg.should approximate(108.583)
  end

  it "should place two image adjacent to one another" do
    text = VIPS::Image.text "Hello VIPS!", "sans 12", 300, 0, 72
    im = @image.lrjoin(text)
    im.avg.should approximate(65.56)
  end

  it "should place two image on top of one another" do
    text = VIPS::Image.text "Hello VIPS!", "sans 12", 300, 0, 72
    text_3band = text.bandjoin text, text
    im = @image.tbjoin(text_3band)
    im.avg.should approximate(100.746)
  end

  it "should replicate (tile) an image" do
    im = @image.replicate 3, 4
    im.x_size.should == @image.x_size * 3
    im.y_size.should == @image.y_size * 4
  end

  it "should cut image into strips and rearrange the strips into a grid" do
    im = @image.extract_area(0, 0, 20, 120).grid 20, 2, 3
    im.should match_sha1('accd3ae9e7be353722adfc202f63134ecd78c5e3')
  end

  it "should wrap an image" do
    im = @image.wrap 120, 233
    new_x = (23 + 120) % @image.x_size
    new_y = (42 + 233) % @image.y_size
    im[new_x, new_y].should == @image[23, 42]
  end

  it "should horizontally flip the image" do
    im = @image.fliphor
    new_x = @image.x_size - 1 - 23
    im[new_x, 42].should == @image[23, 42]
  end
    
  it "should vertically flip the image" do
    im = @image.flipver
    new_y = @image.y_size - 1 - 42
    im[23, new_y].should == @image[23, 42]
  end

  it "should rotate the image by 90 degrees" do
    im = @image.rot90

    im.x_size.should == @image.y_size
    im.y_size.should == @image.x_size

    x = 23
    y = 42
    new_x = @image.y_size - 1 - y
    new_y = x
    im[new_x, new_y].should == @image[x, y]
  end
    
  it "should rotate the image by 180 degrees" do
    im = @image.rot180
    im.should match_image(@image.rot90.rot90)
  end
    
  it "should rotate the image by 270 degrees" do
    im = @image.rot270
    im.should match_image(@image.rot90.rot90.rot90)
  end

  it "should shrink an image via nearest neighbor shrink" do
    im = @image.subsample(3, 2)

    im.x_size.should == @image.x_size / 3
    im.y_size.should == @image.y_size / 2

    new_x = 23
    new_y = 42
    old_x = new_x * 3
    old_y = new_y * 2
    im[new_x, new_y].should == @image[old_x, old_y]
  end

  it "should shrink an image with a single shrink factor" do
    im = @image.subsample 3
    im.should match_image(@image.subsample(3, 3))
  end

  it "should zoom an image via nearest neighbor" do
    im = @image.zoom(4, 3)

    im.x_size.should == @image.x_size * 4
    im.y_size.should == @image.y_size * 3

    x = 23
    y = 42
    im[x * 4, y * 3].should == @image[x, y]
  end

  it "should zoom an image with a single zoom factor" do
    im = @image.zoom 3
    im.should match_image(@image.zoom(3, 3))
  end
end

