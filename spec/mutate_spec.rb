require 'spec_helper.rb'

RSpec.describe Vips::MutableImage do
  it 'can set! metadata' do
    image = Vips::Image.black(16, 16)
    image = image.mutate do |x|
      x.set_type! GObject::GINT_TYPE, "banana", 12
    end

    expect(image.get "banana").to eq(12)
  end

  it 'can remove! metadata' do
    image = Vips::Image.black(16, 16)
    image = image.mutate do |x|
      x.set_type! GObject::GINT_TYPE, "banana", 12
    end

    image = image.mutate do |x|
      x.remove! "banana"
    end

    expect(image.get_typeof "banana").to eq(0)
  end

  it 'can call destructive operations' do
    image = Vips::Image.black(16, 16)
    image = image.mutate do |x|
      x.draw_line! 255, 0, 0, x.width, x.height
    end

    expect(image.avg).to be > 0
  end

  it 'can cannot call non-destructive operations' do
    image = Vips::Image.black(16, 16)

    expect {
      image = image.mutate do |x|
        x.invert
      end
    }.to raise_exception(Vips::Error)
  end

  it 'can cannot use mutable images as arguments' do
    image = Vips::Image.black(16, 16)

    expect {
      image = image.mutate do |x|
        x.draw_image! x, 10, 10
      end
    }.to raise_exception(Vips::Error)
  end

  it 'cannot call destructive operations outside mutate blocks' do
    image = Vips::Image.black(16, 16)

    expect {
      image.draw_line! 255, 0, 0, image.width, image.height
    }.to raise_exception(Vips::Error)
  end

end
