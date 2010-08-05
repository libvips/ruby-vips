require "spec_helper"

describe VIPS do
  it "has a library version" do
    a = VIPS::LIB_VERSION_ARRAY
    a.size.should == 3
    a[0].should >= 7
    a[1].should >= 20
    a[2].should > -1
    VIPS::LIB_VERSION.should =~ /^\d+\.\d+\.\d+/
  end
end

