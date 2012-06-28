require "spec_helper"

describe VIPS::EXRReader do
  before :all do
    @path = sample('sample.exr').to_s
  end

  # TODO: find out whether the sample image really is a bunch of blotches. The
  # sample may be in an unsupported pixel format.
  pending "should read an exr image" do
    im = VIPS::Image.exr @path
    im.x_size.should == 610
    im.should match_sha1('3bea5ddaa343315c817ffae770f9bad4aa5e6e21')
  end

  pending "should recognize an exr image" do
    vips_res = VIPS::EXRReader.recognized? sample('wagon.v').to_s
    vips_res.should be_false

    res = VIPS::EXRReader.recognized? @path
    res.should be_true
  end
end
