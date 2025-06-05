require "spec_helper"

RSpec.describe Vips::Source, version: [8, 9] do
  it "can create a source from a descriptor" do
    source = Vips::Source.new_from_descriptor(0)

    expect(source)
  end

  it "can create a source from a filename" do
    source = Vips::Source.new_from_file simg("wagon.jpg")

    expect(source)
  end

  it "can't create a source from a bad filename" do
    expect {
      Vips::Source.new_from_file simg("banana.jpg")
    }.to raise_exception(Vips::Error)
  end

  it "can create a source from an area of memory" do
    str = File.binread(simg("wagon.jpg"))
    source = Vips::Source.new_from_memory str

    expect(source)
  end

  it "sources have filenames and nicks" do
    source = Vips::Source.new_from_file simg("wagon.jpg")

    expect(source.filename).to eq(simg("wagon.jpg"))
    expect(source.nick)
  end

  it "can load an image from filename source" do
    source = Vips::Source.new_from_file simg("wagon.jpg")
    image = Vips::Image.new_from_source source, ""

    expect(image)
    expect(image.width).to eq(685)
    expect(image.height).to eq(478)
    expect(image.bands).to eq(3)
    expect(image.avg).to be_within(0.001).of(109.789)
  end
end

RSpec.describe Vips::Target, version: [8, 9] do
  it "can create a target to a filename" do
    target = Vips::Target.new_to_file timg("x.jpg")

    expect(target)
  end

  it "can create a target to a descriptor" do
    target = Vips::Target.new_to_descriptor 1

    expect(target)
  end

  it "can create a target to a memory area" do
    target = Vips::Target.new_to_memory

    expect(target)
  end

  it "can't create a target to a bad filename" do
    expect {
      target = Vips::Target.new_to_file "/banana/monkey"
      image = Vips::Image.black 1, 1
      image.write_to_target(target, "")
    }.to raise_exception(Vips::Error)
  end

  it "can save an image to a filename target" do
    source = Vips::Source.new_from_file simg("wagon.jpg")
    image = Vips::Image.new_from_source source, ""
    filename = timg("x4.png")
    target = Vips::Target.new_to_file filename
    image.write_to_target target, ".png"

    image = Vips::Image.new_from_file filename
    expect(image)
    expect(image.width).to eq(685)
    expect(image.height).to eq(478)
    expect(image.bands).to eq(3)
    expect(image.avg).to be_within(0.001).of(109.789)
  end

  it "can save an image to a memory target" do
    source = Vips::Source.new_from_file simg("wagon.jpg")
    image = Vips::Image.new_from_source source, ""
    target = Vips::Target.new_to_memory
    image.write_to_target target, ".png"
    memory = target.get("blob")

    image = Vips::Image.new_from_buffer memory, ""
    expect(image)
    expect(image.width).to eq(685)
    expect(image.height).to eq(478)
    expect(image.bands).to eq(3)
    expect(image.avg).to be_within(0.001).of(109.789)
  end
end

RSpec.describe Vips::SourceCustom, version: [8, 9] do
  it "can create a custom source" do
    source = Vips::SourceCustom.new

    expect(source)
  end

  it "can load a custom source" do
    file = File.open simg("wagon.jpg"), "rb"
    source = Vips::SourceCustom.new
    source.on_read { |length| file.read length }
    source.on_seek { |offset, whence| file.seek(offset, whence) }
    image = Vips::Image.new_from_source source, ""

    expect(image)
    expect(image.width).to eq(685)
    expect(image.height).to eq(478)
    expect(image.bands).to eq(3)
    expect(image.avg).to be_within(0.001).of(109.789)
  end

  it "on_seek is optional" do
    file = File.open simg("wagon.jpg"), "rb"
    source = Vips::SourceCustom.new
    source.on_read { |length| file.read length }
    image = Vips::Image.new_from_source source, ""

    expect(image)
    expect(image.width).to eq(685)
    expect(image.height).to eq(478)
    expect(image.bands).to eq(3)
    expect(image.avg).to be_within(0.001).of(109.789)
  end

  it "can create a user output stream" do
    target = Vips::TargetCustom.new

    expect(target)
  end
end

# 8.13 had a broken on_finish, so close ourselves before 8.14
RSpec.describe Vips::SourceCustom, version: [8, 9] do
  it "can write an image to a user output stream with explicit close" do
    image = Vips::Image.new_from_file simg("wagon.jpg")

    filename = timg("x5.png")
    file = File.open filename, "wb"
    target = Vips::TargetCustom.new
    target.on_write { |chunk| file.write(chunk) }
    image.write_to_target target, ".png"
    file.close

    image2 = Vips::Image.new_from_file filename
    expect(image2)
    expect(image2.width).to eq(685)
    expect(image2.height).to eq(478)
    expect(image2.bands).to eq(3)
    expect(image2.avg).to eq(image.avg)
  end
end

# on_finish started working again in 8.14 (and 8.13.4+, and 8.12 and earlier)
RSpec.describe Vips::SourceCustom, version: [8, 14] do
  it "can write an image to a user output stream" do
    image = Vips::Image.new_from_file simg("wagon.jpg")

    filename = timg("x5.png")
    file = File.open filename, "wb"
    target = Vips::TargetCustom.new
    target.on_write { |chunk| file.write(chunk) }
    target.on_finish { file.close }
    image.write_to_target target, filename

    image2 = Vips::Image.new_from_file filename
    expect(image2)
    expect(image2.width).to eq(685)
    expect(image2.height).to eq(478)
    expect(image2.bands).to eq(3)
    expect(image2.avg).to eq(image.avg)
  end
end
