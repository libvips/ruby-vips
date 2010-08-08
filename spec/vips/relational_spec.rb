require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg 'wagon.v'
    im = simg 'huge.jpg'
    @image2 = im.extract_area(100, 100, @image.x_size, @image.y_size)
  end

  it "should test for equality between two images" do
    im = @image.equal(@image.fliphor)
    im.should match_sha1('d03aedad4480b4200956b1922462799c4de706b5')
  end

  it "should test for equality between an image and constants" do
    im = @image.equal(200)
    im.should match_sha1('4de2fc653e48bf819fb619fef4e360fc1dc3d214')
  end

  it "should test for inequality between two images" do
    im = @image.notequal(@image.fliphor)
    im.should match_sha1('d2b2d4afdcaaf4567e8853abe6c9c31a5f6aaace')
  end

  it "should test for inequality between an image and constants" do
    im = @image.notequal(200)
    im.should match_sha1('d6cbaefac9aa13ad9bfd8c836977141cd861031a')
  end

  it "should test for pixels greater than others between two images" do
    im = @image.more(@image.fliphor)
    im.should match_sha1('0ee896e058a2a957e807304ecef5a6030950b02b')
  end

  it "should test for pixels greater than constants", :vips_lib_version => "!= 7.20" do
    im = @image.more(200)
    im.should match_sha1('64e5988ce4c6b1d1048a09fe5e624b3dbeeb71d8')
  end

  it "should test for pixels less than others between two images" do
    im = @image.less(@image.fliphor)
    im.should match_sha1('b1ed6ab5802831f2695a61975afe3afccf4ce2ea')
  end

  it "should test for pixels less than constants" do
    im = @image.less(210)
    im.should match_sha1('f19119b1159d4a8c494be9cccfce6039fd64086b')
  end

  it "should test for pixels greater than or equal others between two images" do
    im = @image.moreeq(@image.fliphor)
    im.should match_sha1('5bdb2f8177556a95d94a3ad3b606ee6246611e93')
  end

  it "should test for pixels greater than or equal constants" do
    im = @image.moreeq(200)
    im.should match_sha1('f03796ad26bd2de71f664d74072083e40907669f')
  end

  it "should test for pixels less than or equal others between two images" do
    im = @image.lesseq(@image.fliphor)
    im.should match_sha1('d8bc858ec69258ea68053ac24d68fc16f82a176b')
  end

  it "should test for pixels less than or equal constants" do
    im = @image.lesseq(210)
    im.should match_sha1('69f0beed7c4f648f004bef5ea032658a0c10065f')
  end

  it "should merge two images based on a third image" do
    mask = @image.moreeq(@image.fliphor)
    im = mask.ifthenelse(@image, @image.fliphor)
    im.should match_sha1('961d492112e3fd98ec4ff1ca57071907ab67fdb2')
  end

  it "should blend two images based on a third image" do
    mask = @image.moreeq(@image.fliphor)
    im = mask.blend(@image, @image.fliphor)
    im.should match_sha1('961d492112e3fd98ec4ff1ca57071907ab67fdb2')
  end
end

