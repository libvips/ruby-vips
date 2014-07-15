require "spec_helper"

describe VIPS::Mask do
  before :all do
    @coeff_i = [
      [ 1,  2,  3],
      [ 4,  5,  6],
      [ 7,  8, -1],
      [10, 11, 12]
    ]

    @coeff_d = [
      [ 1.2, 2.9, 3.6, 4.0 ],
      [ 4.0, 5.0,   6, 7.1 ],
      [ 7.1, 8.7, 9.9, 10.1]
    ]
  end

  it "should create and recognize int and double masks" do
    mask = VIPS::Mask.new @coeff_i, 1, 2
    mask.int?.should == true

    mask = VIPS::Mask.new @coeff_i, 1, 2.1
    mask.int?.should == false

    mask = VIPS::Mask.new @coeff_d, 3
    mask.int?.should == false
  end

  it "should recognize the size" do
    mask = VIPS::Mask.new @coeff_i
    mask.xsize.should == 3
    mask.ysize.should == 4

    mask = VIPS::Mask.new @coeff_d
    mask.xsize.should == 4
    mask.ysize.should == 3
  end

  it "should allow setting and getting of the scale and offset" do
    mask = VIPS::Mask.new @coeff_i, 3, 7
    mask.scale.should == 3
    mask.offset.should == 7
  end

  it "should allow setting and getting of the coefficient" do
    mask = VIPS::Mask.new @coeff_i
    mask.coeff[0][0].should == 1
    mask.coeff[3][1].should == 11
    mask.coeff[1][2].should == 6
    mask.coeff[2][2].should == -1

    mask = VIPS::Mask.new @coeff_d, 4, 2.2
    mask.coeff[2][1].should == 8.7
    mask.coeff[2][3].should == 10.1
  end

  it "should default the offset to zero" do
    mask = VIPS::Mask.new @coeff_i
    mask.offset.should == 0

    mask = VIPS::Mask.new @coeff_d
    mask.offset.should == 0.0
  end

  it "should default the scale to one" do
    mask = VIPS::Mask.new @coeff_i
    mask.scale.should == 1

    mask = VIPS::Mask.new @coeff_d
    mask.scale.should == 1.0
  end
end
