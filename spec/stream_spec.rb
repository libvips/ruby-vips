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
  class Mystreami < Vips::Streamiu
    def initialize(filename, pipe_mode=false)
      @filename = filename
      @contents = File.open(@filename, "rb").read
      @length = @contents.length
      @pipe_mode = pipe_mode
      @read_point = 0

      super()

      signal_connect "read" do |buf, len|
        bytes_available = @length - @read_point
        bytes_to_copy = [bytes_available, len].min
        buf.put_bytes(0, @contents, @read_point, bytes_to_copy)
        @read_point += bytes_to_copy

        bytes_to_copy
      end

      signal_connect "seek" do |offset, whence|
        if @pipe_mode
          -1
        else
          case whence 
          when 0
            # SEEK_SET
            new_read_point = offset
          when 1
            # SEEK_CUR
            new_read_point = self.read_point + offset
          when 2
            # SEEK_END
            new_read_point = self.length + offset
          else
            raise "bad whence #{whence}"
          end

          @read_point = [0, [@length, new_read_point].min].max
        end
      end
    end
  end

  class Mystreamo < Vips::Streamou
    def initialize(filename)
      @filename = filename
      @f = File.open(@filename, "wb")

      super()

      signal_connect "write" do |buf, len|
        @f.write(buf.get_bytes(0, len))
        len
      end

      signal_connect "finish" do 
        @f.close
        @f = nil
      end

    end
  end
end

if Vips::at_least_libvips?(8, 9)
  RSpec.describe Vips::Streamiu do
    it 'can create a user input stream' do
      streamiu = Mystreami.new simg('wagon.jpg')

      expect(streamiu)
    end

    it 'can load a user stream' do
      streamiu = Mystreami.new simg('wagon.jpg')
      image = Vips::Image.new_from_stream streamiu, ''

      expect(image)
      expect(image.width).to eq(685)
      expect(image.height).to eq(478)
      expect(image.bands).to eq(3)
      expect(image.avg).to be_within(0.001).of(109.789)
    end

    it 'can load a user stream in pipe mode' do
      streamiu = Mystreami.new simg('wagon.jpg'), true
      image = Vips::Image.new_from_stream streamiu, ''

      expect(image)
      expect(image.width).to eq(685)
      expect(image.height).to eq(478)
      expect(image.bands).to eq(3)
      expect(image.avg).to be_within(0.001).of(109.789)
    end

    it 'can create a user output stream' do
      streamou = Mystreamo.new timg('x.jpg')

      expect(streamou)
    end

    it 'can write an image to a user output stream' do
      image = Vips::Image.new_from_file simg('wagon.jpg')
      filename = timg('x5.png')
      streamou = Mystreamo.new filename
      image.write_to_stream streamou, '.png'

      image = Vips::Image.new_from_file filename
      expect(image)
      expect(image.width).to eq(685)
      expect(image.height).to eq(478)
      expect(image.bands).to eq(3)
      expect(image.avg).to be_within(0.001).of(109.789)
    end

  end

end
