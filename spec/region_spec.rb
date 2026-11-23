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

  # fetch evaluates the pipeline, so it must release the GVL, or every other
  # Ruby thread stalls until it returns
  it "lets other threads run during fetch", version: [8, 8] do
    image = Vips::Image.gaussnoise(2000, 2000).gaussblur(4)
    region = Vips::Region.new image

    ticks = 0
    running = true
    ticker = Thread.new do
      while running
        ticks += 1
        sleep 0.001
      end
    end
    sleep 0.01 until ticks > 0

    before = ticks
    region.fetch(0, 0, 2000, 2000)
    during = ticks - before
    running = false
    ticker.join

    expect(during).to be > 0
  end
end
