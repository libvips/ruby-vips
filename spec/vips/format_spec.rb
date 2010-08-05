require "spec_helper"

describe VIPS::Format do
  before :all do
    @path = sample_join 'wagon.jpg'
    @jpeg_fmt = VIPS::Format::FORMATS[:jpeg]
    @gauss = VIPS::Image.gaussnoise 400, 200, 128.0, 4.2
  end

  it "should have a list of available formats" do
    formats = VIPS::Format::FORMATS
    formats.size.should > 1
    formats.any?{|k, v| v.nickname == 'jpeg'}.should be_true
  end

  it "should return correct format attributes" do
    @jpeg_fmt.priority.should == 0
    @jpeg_fmt.nickname.should == 'jpeg'
    @jpeg_fmt.description.should == 'JPEG'
    @jpeg_fmt.suffs.should == %w{.jpg .jpeg .jpe}
  end

  it "should get flags for a format" do
    @jpeg_fmt.flags(@path).should == []
  end

  it "should get the header from a disk image" do
    im = @jpeg_fmt.header @path
    im.y_size.should == 478
  end

  it "should discover the format of a file" do
    f = VIPS::Format.for_file @path
    f.nickname.should == 'jpeg'
  end

  it "should discover the format of a filename" do
    f = VIPS::Format.for_name 'test.png'
    f.nickname.should == 'png'
  end

  it "should read a file" do
    im = VIPS::Image.read @path
    im.x_size.should == 685
  end

  it "should write a jpeg file using Image#write" do
    t = tmp 'format_write.jpg'
    @gauss.write t.to_s
    im2 = VIPS::Image.new t.to_s
    im2.x_size.should == 400
  end

  it "should read a jpeg file" do
    im = VIPS::Image.from_jpeg @path
    im.x_size.should == 685
    im.should match_sha1('f473c98d984b11adeb06fa8f959695d2a7e69605')
  end

  it "should write a jpeg file using Image#to_jpeg" do
    t = tmp 'format_write.jpg'
    @gauss.to_jpeg t.to_s
    im2 = VIPS::Image.new t.to_s
    im2.x_size.should == 400
  end

  it "should write a tiny jpeg file to memory", :vips_lib_version => "> 7.20" do
    im3 = VIPS::Image.black(10, 10, 1)
    s = im3.to_bufjpeg 75
    s.size.should == 411
  end    

  it "should write a tiff file" do
    t = tmp 'format_write.tiff'
    @gauss.to_tiff t.to_s
    im2 = VIPS::Image.new t.to_s
    im2.x_size.should == 400
  end

  it "should read just about anything through imagemagick" do
    path = sample('lion.svg').to_s
    im = VIPS::Image.from_magick path
    im.x_size.should == 425
  end

  # TODO: find out whether the sample image really is a bunch of blotches. The
  # sample may be in an unsupported pixel format.
  it "should read an exr image" do
    path = sample('sample.exr').to_s
    im = VIPS::Image.from_exr path
    im.x_size.should == 610
    im.should match_sha1('3bea5ddaa343315c817ffae770f9bad4aa5e6e21')
  end

  it "should read a ppm image" do
    path = sample('ghost.ppm').to_s
    im = VIPS::Image.from_ppm path
    im.x_size.should == 49
    im.should match_sha1('b619b314a1ed71918df15694c0102f18e10ffe3a')
  end

  it "should write a ppm file" do
    t = tmp 'format_write.ppm'
    @gauss.clip2fmt(:UCHAR).to_ppm t.to_s
    im2 = VIPS::Image.new t.to_s
    im2.x_size.should == 400
  end

  it "should read an analyze image" do
    pending "don't have a sample analyze file"
    path = sample('analyze.img').to_s
    im = VIPS::Image.from_analyze path
    im.x_size.should == 49
    im.should match_sha1('17cee1a22aca57966abb03f81dd066914f123978')
  end

  it "should read a csv image", :vips_lib_version => "> 7.20" do
    path = sample('sample.csv').to_s
    im = VIPS::Image.from_csv path
    im.x_size.should == 7
    im.should match_sha1('a6a7552f83ff6a1e76850a0bb064e4efcf24a524')
  end

  it "should write a csv file" do
    t = tmp 'format_write.csv'
    @gauss.clip2fmt(:UCHAR).to_csv t.to_s
    im2 = VIPS::Image.from_csv t.to_s
    im2.x_size.should == 400
  end

  it "should read and write a png file" do
    t = tmp 'format_write.png'
    @gauss.to_png t.to_s
    im2 = VIPS::Image.from_png t.to_s
    im2.x_size.should == 400
  end

  it "should write a png file to a string", :vips_lib_version => "> 7.22" do
    im3 = VIPS::Image.black(100, 100, 1)
    s = im3.to_bufpng 3, 0
    s.size.should == 125
  end

  it "should read and write a raw file" do
    pending "raw writing fails"
    im = VIPS::Image.black(400, 200, 1)
    t = tmp 'format_write.raw'
    im.to_raw t.to_s
    im2 = VIPS::Image.from_raw t.to_s, 400, 200, 16, 0
    im2.x_size.should == 400
  end

  it "should read a mat image" do
    pending "need a matlab sample file to test"
    path = sample('sample.mat').to_s
    im = VIPS::Image.from_mat path
    im.x_size.should == 7
    im.should match_sha1('8b96e49963f021b8b395e48ab89bbe0ce314b858')
  end

  it "should read and write a raw file" do
    pending "libvips 7.20.4 doesn't do rad"
    im = VIPS::Image.black(400, 200, 1)
    t = tmp('format_write.img').to_s
    im.to_rad t
    im2 = VIPS::Image.from_rad t
    im2.x_size.should == 400
  end

end

