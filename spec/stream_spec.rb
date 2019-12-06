require 'spec_helper.rb'

if Vips::at_least_libvips?(8, 9)
  RSpec.describe Vips::Streami do
    it 'can create a stream from a descriptor' do
      stream = Vips::Streami.new_from_descriptor(0)

      expect(stream)
    end

    it 'can create a stream from a filename' do
      stream = Vips::Streami.new_from_file simg('wagon.jpg')

      expect(stream)
    end

    it 'can\'t create a stream from a bad filename' do
      expect { 
        Vips::Streami.new_from_file simg('banana.jpg') 
      }.to raise_exception(Vips::Error)
    end

    it 'can create a stream from an area of memory' do
      str = File.open(simg('wagon.jpg'), 'rb').read
      stream = Vips::Streami.new_from_memory str

      expect(stream)
    end

    it 'streams have filenames and nicks' do
      stream = Vips::Streami.new_from_file simg('wagon.jpg')

      expect(stream.filename).to eq(simg('wagon.jpg'))
      expect(stream.nick)
    end

    it 'can load an image from filename stream' do
      stream = Vips::Streami.new_from_file simg('wagon.jpg')
      image = Vips::Image.new_from_stream stream, ''

      expect(image)
      expect(image.width).to eq(685)
      expect(image.height).to eq(478)
      expect(image.bands).to eq(3)
      expect(image.avg).to be_within(0.001).of(109.789)

    end
  end
end

if Vips::at_least_libvips?(8, 9)
  RSpec.describe Vips::Streamo do
    it 'can create a stream to a filename' do
      stream = Vips::Streamo.new_to_file timg('x.jpg')

      expect(stream)
    end

    it 'can create a stream to a descriptor' do
      stream = Vips::Streamo.new_to_descriptor 1

      expect(stream)
    end

    it 'can create a stream to a memory area' do
      stream = Vips::Streamo.new_to_memory

      expect(stream)
    end

    it 'can\'t create a stream to a bad filename' do
      expect { 
        Vips::Streamo.new_to_file '/banana/monkey'
      }.to raise_exception(Vips::Error)
    end

    it 'can save an image to a filename stream' do
      streami = Vips::Streami.new_from_file simg('wagon.jpg')
      image = Vips::Image.new_from_stream streami, ''
      filename = timg('x4.png')
      streamo = Vips::Streamo.new_to_file filename
      image.write_to_stream streamo, '.png'

      image = Vips::Image.new_from_file filename
      expect(image)
      expect(image.width).to eq(685)
      expect(image.height).to eq(478)
      expect(image.bands).to eq(3)
      expect(image.avg).to be_within(0.001).of(109.789)
    end

    it 'can save an image to a memory stream' do
      streami = Vips::Streami.new_from_file simg('wagon.jpg')
      image = Vips::Image.new_from_stream streami, ''
      streamo = Vips::Streamo.new_to_memory
      image.write_to_stream streamo, '.png'
      memory = streamo.get('blob')

      image = Vips::Image.new_from_buffer memory, ''
      expect(image)
      expect(image.width).to eq(685)
      expect(image.height).to eq(478)
      expect(image.bands).to eq(3)
      expect(image.avg).to be_within(0.001).of(109.789)
    end

  end
end

if Vips::at_least_libvips?(8, 9)
  RSpec.describe Vips::Streamiu do
    it 'can create a user input stream' do
      input_stream = Vips::Streamiu.new

      expect(input_stream)
    end

    it 'can load a user stream' do
      source = File.open simg('wagon.jpg'), "rb"
      input_stream = Vips::Streamiu.new
      input_stream.on_read { |length| source.read length }
      input_stream.on_seek { |offset, whence| source.seek(offset, whence) }
      image = Vips::Image.new_from_stream input_stream, ""

      expect(image)
      expect(image.width).to eq(685)
      expect(image.height).to eq(478)
      expect(image.bands).to eq(3)
      expect(image.avg).to be_within(0.001).of(109.789)
    end

    it 'on_seek is optional' do
      source = File.open simg('wagon.jpg'), "rb"
      input_stream = Vips::Streamiu.new
      input_stream.on_read { |length| source.read length }
      input_stream.on_seek { |offset, whence| -1 }
      image = Vips::Image.new_from_stream input_stream, ""

      expect(image)
      expect(image.width).to eq(685)
      expect(image.height).to eq(478)
      expect(image.bands).to eq(3)
      expect(image.avg).to be_within(0.001).of(109.789)
    end

    it 'can create a user output stream' do
      output_stream = Vips::Streamou.new

      expect(output_stream)
    end

    it 'can write an image to a user output stream' do
      filename = timg('x5.png')
      dest = File.open filename, "wb"
      output_stream = Vips::Streamou.new
      output_stream.on_write { |chunk| dest.write(chunk) }
      output_stream.on_finish { dest.close }
      image = Vips::Image.new_from_file simg('wagon.jpg')
      image.write_to_stream output_stream, ".png"

      image = Vips::Image.new_from_file filename
      expect(image)
      expect(image.width).to eq(685)
      expect(image.height).to eq(478)
      expect(image.bands).to eq(3)
      expect(image.avg).to be_within(0.001).of(109.789)
    end

  end

end
