require 'spec_helper.rb'

RSpec.describe Vips::Streami do
  if Vips::at_least_libvips?(8, 9)
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

    it 'can create a stream to a filename' do
      stream = Vips::Streamo.new_to_file timg('x.jpg')

      expect(stream)
    end

    it 'can\'t create a stream to a bad filename' do
      # we'll need to touch the file, set it read-only, then try writing
      #expect { 
      #  Vips::Streamo.new_to_file '/banana'
      #}.to raise_exception(Vips::Error)
    end

    it 'can save an image to a filename stream' do
      streami = Vips::Streami.new_from_file simg('wagon.jpg')
      image = Vips::Image.new_from_stream streami, ''
      streamo = Vips::Streamo.new_to_file timg('x.jpg')
      image.write_to_stream streamo, '.jpg'
    end

  end
end
