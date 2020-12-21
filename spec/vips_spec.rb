require "spec_helper"

RSpec.describe Vips do
  describe "Vips" do
    it "can set concurrency" do
      Vips.concurrency_set 12
    end

    it "can set SIMD" do
      Vips.vector_set true
    end

    it "can enable leak testing" do
      Vips.leak_set true
      Vips.leak_set false
    end

    it "can set the operation cache size" do
      Vips.cache_set_max 0
      Vips.cache_set_max 100
    end

    it "can set the operation cache memory limit" do
      Vips.cache_set_max_mem 0
      Vips.cache_set_max_mem 10000000
    end

    it "can set the operation cache file limit" do
      Vips.cache_set_max_files 0
      Vips.cache_set_max_files 100
    end

    it "can get a set of filename suffixes" do
      suffs = Vips.get_suffixes
      expect(suffs.length > 10).to be true unless suffs.empty?
    end
  end

  describe "#call" do
    it "can make a black image" do
      image = Vips::Operation.call "black", [200, 200]

      expect(image.width).to eq(200)
      expect(image.height).to eq(200)
      expect(image.bands).to eq(1)
    end

    it "can take an optional argument" do
      image = Vips::Operation.call "black", [200, 200], bands: 12

      expect(image.width).to eq(200)
      expect(image.height).to eq(200)
      expect(image.bands).to eq(12)
    end

    it "ignores optional arguments with nil values" do
      image = Vips::Operation.call "black", [200, 200], bands: nil

      expect(image.width).to eq(200)
      expect(image.height).to eq(200)
      expect(image.bands).to eq(1)
    end

    it "can handle enum arguments" do
      black = Vips::Operation.call "black", [200, 200]
      embed = Vips::Operation.call "embed", [black, 10, 10, 500, 500],
        extend: :mirror

      expect(embed.width).to eq(500)
      expect(embed.height).to eq(500)
      expect(embed.bands).to eq(1)
    end

    it "can handle enum arguments with underscores" do
      black = Vips::Image.black 200, 100, bands: 3
      mono = black.colourspace :b_w

      expect(mono.width).to eq(200)
      expect(mono.height).to eq(100)
      expect(mono.bands).to eq(1)
    end

    it "enum arguments can be strings" do
      black = Vips::Operation.call "black", [200, 200]
      embed = Vips::Operation.call "embed", [black, 10, 10, 500, 500],
        extend: "mirror"

      expect(embed.width).to eq(500)
      expect(embed.height).to eq(500)
      expect(embed.bands).to eq(1)
    end

    it "enum arguments can be ints" do
      black = Vips::Image.black 200, 100
      # not angle 0, enum 0 (VIPS_ANGLE_D0)
      rot = black.rot 0

      expect(rot.width).to eq(200)
      expect(rot.height).to eq(100)
      expect(rot.bands).to eq(1)
    end

    it "can return optional output args" do
      point = Vips::Operation.call "black", [1, 1]
      test = Vips::Operation.call "embed", [point, 20, 10, 100, 100],
        extend: :white
      value, opts = Vips::Operation.call "min", [test], x: true, y: true

      expect(value).to eq(0)
      expect(opts["x"]).to eq(20)
      expect(opts["y"]).to eq(10)
    end

    it "can call draw operations" do
      black = Vips::Operation.call "black", [100, 100]
      test = Vips::Operation.call "draw_rect", [black, 255, 10, 10, 1, 1]

      max_black = Vips::Operation.call "max", [black]
      max_test = Vips::Operation.call "max", [test]

      expect(max_black).to eq(0)
      expect(max_test).to eq(255)
    end

    it "can throw errors for failed operations" do
      black = Vips::Operation.call "black", [100, 1]

      expect { black.crop(10, 10, 1, 1) }.to raise_exception(Vips::Error)
    end
  end
end
