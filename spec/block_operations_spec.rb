require "spec_helper"

RSpec.describe Vips, version: [8, 13] do
  let(:svg_image) { simg("lion.svg") }
  let(:jpg_image) { simg("wagon.jpg") }

  if has_svg?
    it "can block untrusted operations" do
      untrusted_image = svg_image # svgload operation is known as untrusted

      # Block
      Vips.block_untrusted(true)
      expect { Vips::Image.new_from_file(untrusted_image) }.to raise_error Vips::Error, /not a known/

      # Unblock
      Vips.block_untrusted(false)
      expect { Vips::Image.new_from_file(untrusted_image) }.not_to raise_error
    end
  end

  if has_jpeg? && has_svg?
    it "can block specific operations" do
      # Block all loaders except jpeg
      Vips.block("VipsForeignLoad", true)
      Vips.block("VipsForeignLoadJpeg", false)
      expect { Vips::Image.new_from_file(svg_image) }.to raise_error Vips::Error, /not a known/
      expect { Vips::Image.new_from_file(jpg_image) }.not_to raise_error

      # Unblock all loaders
      Vips.block("VipsForeignLoad", false)
      expect { Vips::Image.new_from_file(svg_image) }.not_to raise_error
    end
  end
end
