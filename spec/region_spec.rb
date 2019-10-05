require 'spec_helper.rb'

RSpec.describe Vips::Region do
  it 'can create a region on an image' do
    image = Vips::Image.black(100, 100)
    region = Vips::Region.new image

    expect(region)
  end

  if Vips::at_least_libvips?(8, 8)
    it 'can fetch pixels from a region' do
      image = Vips::Image.black(100, 100)
      region = Vips::Region.new image
      pixel_data = region.fetch 10, 20, 30, 40

      expect(pixel_data)
      expect(pixel_data.length == 30 * 40)
    end
  end

  if Vips::at_least_libvips?(8, 8)
    it 'can make regions with width and height' do
      image = Vips::Image.black(100, 100)
      region = Vips::Region.new image
      pixel_data = region.fetch 10, 20, 30, 40

      expect(pixel_data)
      expect(region.width == 30)
      expect(region.height == 40)
    end
  end
end
