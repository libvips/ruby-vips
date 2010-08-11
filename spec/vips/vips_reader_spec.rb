require "spec_helper"

describe VIPS::VIPSReader do
  before :all do
    @path = sample('wagon.v').to_s
  end

  it "should read a vips image" do
    im = VIPS::VIPSReader.new(@path).read
    im.x_size.should == 228
  end
end
