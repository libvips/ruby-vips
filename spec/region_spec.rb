require "spec_helper"

RSpec.describe Vips::Region do
  it "can create a region on an image" do
    image = Vips::Image.black(100, 100)
    region = Vips::Region.new image

    expect(region)
  end

  it "can fetch pixels from a region", version: [8, 8] do
    image = Vips::Image.black(100, 100)
    region = Vips::Region.new image
    pixel_data = region.fetch 10, 20, 30, 40

    expect(pixel_data)
    expect(pixel_data.length == 30 * 40)
  end

  it "can make regions with width and height", version: [8, 8] do
    image = Vips::Image.black(100, 100)
    region = Vips::Region.new image
    pixel_data = region.fetch 10, 20, 30, 40

    expect(pixel_data)
    expect(region.width == 30)
    expect(region.height == 40)
  end
end
