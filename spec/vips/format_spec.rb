require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg 'wagon.v'
    @gauss = VIPS::Image.gaussnoise 400, 200, 128.0, 4.2
  end

  it "should read a jpeg file" do
    im = VIPS::Image.read_jpeg sample('wagon.jpg').to_s
    im.x_size.should == 685
    im.should match_sha1('f473c98d984b11adeb06fa8f959695d2a7e69605')
  end

  it "should read just about anything through imagemagick" do
    path = sample('lion.svg').to_s
    im = VIPS::Image.read_magick path
    im.x_size.should == 425
  end

  # TODO: find out whether the sample image really is a bunch of blotches. The
  # sample may be in an unsupported pixel format.
  it "should read an exr image" do
    path = sample('sample.exr').to_s
    im = VIPS::Image.read_exr path
    im.x_size.should == 610
    im.should match_sha1('3bea5ddaa343315c817ffae770f9bad4aa5e6e21')
  end

  it "should read a ppm image" do
    path = sample('ghost.ppm').to_s
    im = VIPS::Image.read_ppm path
    im.x_size.should == 49
    im.should match_sha1('b619b314a1ed71918df15694c0102f18e10ffe3a')
  end

  it "should read an analyze image" do
    pending "don't have a sample analyze file"
    path = sample('analyze.img').to_s
    im = VIPS::Image.read_analyze path
    im.x_size.should == 49
    im.should match_sha1('17cee1a22aca57966abb03f81dd066914f123978')
  end

  it "should read a csv image", :vips_lib_version => "> 7.20" do
    path = sample('sample.csv').to_s
    im = VIPS::Image.read_csv path
    im.x_size.should == 7
    im.should match_sha1('a6a7552f83ff6a1e76850a0bb064e4efcf24a524')
  end
end
