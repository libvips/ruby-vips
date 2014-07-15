require "spec_helper"

describe VIPS::EXRReader do
  before :all do
    @path = sample('sample.exr').to_s
  end

  it "should read an exr image" do
    im = VIPS::Image.exr @path
    im.x_size.should == 610
    im = im.lin(256,0).clip2fmt(:UCHAR)
    im.should match_sha1('4d1c515a038aa624318da9f6b186bf442fc1635c')
  end

  it "should recognize an exr image" do
    res = VIPS::EXRReader.recognized? sample('wagon.v').to_s
    res.should == false

    res = VIPS::EXRReader.recognized? @path
    res.should == true
  end
end
