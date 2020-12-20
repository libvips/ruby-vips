require "spec_helper"

RSpec.describe Vips::MutableImage do
  it "can set! metadata in mutate", version: [8, 10] do
    image = Vips::Image.black(16, 16)
    image = image.mutate { |x|
      x.set_type! GObject::GINT_TYPE, "banana", 12
    }

    expect(image.get("banana")).to eq(12)
  end

  it "can remove! metadata in mutate" do
    image = Vips::Image.black(16, 16)
    image = image.mutate { |x|
      x.set_type! GObject::GINT_TYPE, "banana", 12
    }

    image = image.mutate { |x|
      x.remove! "banana"
    }

    expect(image.get_typeof("banana")).to eq(0)
  end

  it "can call destructive operations in mutate" do
    image = Vips::Image.black(16, 16)
    image = image.mutate { |x|
      x.draw_line! 255, 0, 0, x.width, x.height
    }

    expect(image.avg).to be > 0
  end

  it "cannot call non-destructive operations in mutate" do
    image = Vips::Image.black(16, 16)

    expect {
      image = image.mutate { |x|
        x.invert
      }
    }.to raise_exception(Vips::Error)
  end

  it "cannot use mutable images as arguments in mutate" do
    image = Vips::Image.black(16, 16)

    expect {
      image = image.mutate { |x|
        x.draw_image! x, 10, 10
      }
    }.to raise_exception(Vips::Error)
  end

  it "cannot call destructive operations outside mutate" do
    image = Vips::Image.black(16, 16)

    expect {
      image.draw_line! 255, 0, 0, image.width, image.height
    }.to raise_exception(Vips::Error)
  end
end
