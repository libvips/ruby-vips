require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg('wagon.v')
  end

  it "should perform a bitwise and with an image" do
    im = @image & @image.fliphor

    s1 = @image[34, 42]
    s2 = @image.fliphor[34, 42]
    im[34, 42].each_with_index{ |px, i| px.should == s1[i] & s2[i] }
  end

  it "should perform a bitwise or with an image" do
    im = @image | @image.fliphor

    s1 = @image[34, 42]
    s2 = @image.fliphor[34, 42]
    im[34, 42].each_with_index{ |px, i| px.should == s1[i] | s2[i] }
  end

  it "should perform a bitwise either or with an image" do
    im = @image ^ @image.fliphor

    s1 = @image[34, 42]
    s2 = @image.fliphor[34, 42]
    im[34, 42].each_with_index{ |px, i| px.should == s1[i] ^ s2[i] }
  end

  it "should perform a bitwise and with an image given one or more constants" do
    s = @image[34, 42]

    im = @image & 20
    im[34, 42].each_with_index{ |px, i| px.should == s[i] & 20 }

    im = @image & [33, 66, 99]
    r = im[34, 42]
    r[0].should == s[0] & 33
    r[1].should == s[1] & 66
    r[2].should == s[2] & 99
  end

  it "should perform a bitwise or with an image given one or more constants" do
    s = @image[34, 42]

    im = @image | 20
    im[34, 42].each_with_index{ |px, i| px.should == s[i] | 20 }

    im = @image | [33, 66, 99]
    r = im[34, 42]
    r[0].should == s[0] | 33
    r[1].should == s[1] | 66
    r[2].should == s[2] | 99
  end

  it "should perform a bitwise either or with an image given one or more constants" do
    s = @image[34, 42]

    im = @image ^ 20
    im[34, 42].each_with_index{ |px, i| px.should == s[i] ^ 20 }

    im = @image ^ [33, 66, 99]
    r = im[34, 42]
    r[0].should == s[0] ^ 33
    r[1].should == s[1] ^ 66
    r[2].should == s[2] ^ 99
  end

  it "should perform a right and left shift on an image" do
	im = @image.clip2fmt(:UINT)
	s = im[43, 64]

    im2 = im << 3
    im2[43, 64].each_with_index{ |px, i| px.should == s[i] << 3 }

    im3 = im >> 3
    im3[43, 64].each_with_index{ |px, i| px.should == s[i] >> 3 }
  end
end

