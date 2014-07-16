require "spec_helper"

# use this to get hex digets for tests
# require 'digest/sha1'
# puts "im3.sha1 == #{Digest::SHA1.hexdigest(im3.data)}"

describe VIPS::Image do
  before :all do
    @image = simg 'wagon.v'
    im = simg 'huge.jpg'
    @image2 = im.extract_area(100, 100, @image.x_size, @image.y_size)
  end

  # TODO: These specs validate various decorations but not the actual image
  # contents.

  it "should convert various formats to XYZ" do
    im = @image.srgb_to_xyz

    lab = im.xyz_to_lab.lab_to_xyz
    labq = im.xyz_to_lab.lab_to_labq.labq_to_xyz
    lab_temp = im.xyz_to_lab.lab_to_xyz_temp 0.5, 0.5, 0.5
    ucs = im.xyz_to_ucs.ucs_to_xyz
    yxy = im.xyz_to_yxy.yxy_to_xyz

    [im, lab, labq, lab_temp, ucs].each do |i|
      i.vtype.should == :XYZ
      i.coding.should == :NONE
      i.band_fmt.should == :FLOAT
      i.bands.should == 3
    end
  end

  it "should convert various formats to Lab" do
    im = @image.srgb_to_xyz
    im2 = im.xyz_to_lab

    lch = im.xyz_to_ucs.ucs_to_lch.lch_to_lab
    labq = im2.lab_to_labq.labq_to_lab
    labs = im2.lab_to_labs.labs_to_lab
    ucs = im.xyz_to_ucs.ucs_to_lab
    xyz = im.xyz_to_lab
    xyz_temp = im.xyz_to_lab_temp 0.5, 0.5, 0.5
    
    [lch, labq, labs, ucs, xyz, xyz_temp].each do |i|
      i.vtype.should == :LAB
      i.coding.should == :NONE
      i.band_fmt.should == :FLOAT
      i.bands.should == 3
    end
  end

  it "should convert various formats to LCh" do
    im = @image.srgb_to_xyz

    lab = im.xyz_to_lab.lab_to_lch
    ucs = im.xyz_to_ucs.ucs_to_lch

    [lab, ucs].each do |i|
      if Spec::Helpers.match_vips_version("> 7.22")
        i.vtype.should == :LCH
      end
      i.coding.should == :NONE
      i.band_fmt.should == :FLOAT
      i.bands.should == 3
    end
  end

  it "should convert various formats to UCS" do
    im = @image.srgb_to_xyz

    xyz = im.xyz_to_ucs
    lab = im.xyz_to_lab.lab_to_ucs
    lch = im.xyz_to_lab.lab_to_lch.lch_to_ucs

    [xyz, lab, lch].each do |i|
      i.vtype.should == :UCS
      i.coding.should == :NONE
      i.band_fmt.should == :FLOAT
      i.bands.should == 3
    end
  end

  it "should convert various formats to LabQ" do
    im = @image.srgb_to_xyz.xyz_to_lab

    lab = im.lab_to_labq
    labs = im.lab_to_labs.labs_to_labq

    [lab, labs].each do |i|
      if Spec::Helpers.match_vips_version("> 7.23")
        i.vtype.should == :LABQ
      end
      i.coding.should == :LABQ
      i.band_fmt.should == :UCHAR
      i.bands.should == 4
    end
  end

  it "should convert various formats to LabS" do
    im = @image.srgb_to_xyz.xyz_to_lab

	lab = im.lab_to_labs
	labq = im.lab_to_labq.labq_to_labs

    [lab, labq].each do |i|
      i.vtype.should == :LABS
      i.coding.should == :NONE
      i.band_fmt.should == :SHORT
      i.bands.should == 3
    end
  end

  it "should convert a float image to rad" do
    im = @image.srgb_to_xyz.float_to_rad
    im.coding.should == :RAD
    im.band_fmt.should == :UCHAR
  end

  it "should convert a rad image to float" do
    im = @image.srgb_to_xyz.float_to_rad.rad_to_float
    im.coding.should == :NONE
    im.band_fmt.should == :FLOAT
  end

  it "should convert an XYZ image to sRGB" do
    im = @image.srgb_to_xyz.xyz_to_srgb
    im.vtype.should == :sRGB
    im.coding.should == :NONE
    im.band_fmt.should == :UCHAR
  end

  it "should convert an XYZ image to yxy" do
    im = @image.srgb_to_xyz.xyz_to_yxy
    im.vtype.should == :YXY
    im.coding.should == :NONE
    im.band_fmt.should == :FLOAT
  end

  it "should calculate a CMC color difference image between two lab images" do
    im = @image.srgb_to_xyz.xyz_to_lab
    im2 = @image2.srgb_to_xyz.xyz_to_lab
    diff = im.decmc_from_lab(im2)
    # 7.32 has revised colour difference stuff
    if Spec::Helpers.match_vips_version("> 7.32")
      diff.should match_sha1('91396adda33cc10e71bc79b0cd3cbf449f756d0b')
    end
  end

  it "should calculate a CIE76 color difference image between two lab images" do
    im = @image.srgb_to_xyz.xyz_to_lab
    im2 = @image2.srgb_to_xyz.xyz_to_lab
    diff = im.de_from_lab(im2)
    # 7.32 has revised colour difference stuff
    if Spec::Helpers.match_vips_version("> 7.32")
      diff.should match_sha1('e5810662fa9866a39e778bdf73e910c2196e56e9')
    end
  end

  it "should calculate a CIEDE2000 color difference image between two lab images" do
    im = @image.srgb_to_xyz.xyz_to_lab
    im2 = @image2.srgb_to_xyz.xyz_to_lab
    diff = im.de00_from_lab(im2)
    # 7.32 has revised colour difference stuff
    if Spec::Helpers.match_vips_version("> 7.32")
      diff.should match_sha1('629b5fb844c1f2e61c5e652f23435297b0ec76f4')
    end
  end

  it "should import an embedded icc profile" do
    im = VIPS::Image.new(sample('icc.jpg').to_s)
    im2 = im.icc_import_embedded(:RELATIVE_COLORIMETRIC)

    # we don't want to test a hash, it'll change with lcms versions
    # we need a better way to test this result
    # im2.should match_sha1('c1d2a147be14e9ff615396bf040202cdd4c3b9d1')
  end

  it "should import an embedded icc profile and then export using an external icc profile" do
    im = VIPS::Image.new(sample('icc.jpg').to_s)
    im2 = im.icc_import_embedded(:RELATIVE_COLORIMETRIC)
    im3 = im2.icc_export_depth(8, sample('lcd.icc').to_s, :RELATIVE_COLORIMETRIC)
 
    # we don't want to test a hash, it'll change with lcms versions
    # we need a better way to test this result
    # im3.should match_sha1('cf1d8ff608231a13b70d938f00e88dd924e83bc8')
  end

  it "should transform an image using an import and an export icc profile (Image#icc_transform)" do
    im = @image.icc_import(sample('lcd.icc').to_s, :RELATIVE_COLORIMETRIC)
    im2 = im.icc_export_depth(8, sample('lcd.icc').to_s, :RELATIVE_COLORIMETRIC)

    a = @image.srgb_to_xyz.xyz_to_lab
    b = im2.srgb_to_xyz.xyz_to_lab
    diff = a.de_from_lab(b).max

    diff.should < 3
  end

  #it "should transform an image using an import and an export icc profile (Image#icc_transform)"
  #it "should import an external icc profile (Image#icc_import)"
  #it "should convert in image to media relative colorimetry using an icc profile (Image#icc_ac2rc)"
end

