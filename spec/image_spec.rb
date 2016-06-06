require 'spec_helper.rb'

def has_jpeg?
    Vips::type_find("VipsOperation", "jpegload") != nil
end

RSpec.describe Vips::Image do
    describe '#new' do
        it 'can make an empty image' do
            image = Vips::Image.new

            expect(image.width).to eq(1)
            expect(image.height).to eq(1)
            expect(image.bands).to eq(1)
        end

        it 'can save an image to a file' do
            filename = timg "x.v"

            image = Vips::Image.black(16, 16) + 128
            image.write_to_file filename

            expect(File.exist?(filename)).to be true
        end

        it 'can load an image from a file' do
            filename = timg "x.v"

            image = Vips::Image.black(16, 16) + 128
            image.write_to_file(filename)

            x = Vips::Image.new_from_file filename
            expect(x.width).to eq(16)
            expect(x.height).to eq(16)
            expect(x.bands).to eq(1)
            expect(x.avg).to eq(128)
        end

        if has_jpeg?
            it 'can save an image to a buffer' do
                image = Vips::Image.black(16, 16) + 128
                buffer = image.write_to_buffer ".jpg"
                expect(buffer.length).to be > 100
            end
        end

        if has_jpeg?
            it 'can load an image from a buffer' do
                image = Vips::Image.black(16, 16) + 128
                buffer = image.write_to_buffer ".jpg"
                x = Vips::Image.new_from_buffer buffer, ""
                expect(x.width).to eq(16)
                expect(x.height).to eq(16)
            end
        end

        it 'can make an image from a 2d array' do
            image = Vips::Image.new_from_array [[1, 2], [3, 4]]
            expect(image.width).to eq(2)
            expect(image.height).to eq(2)
            expect(image.bands).to eq(1)
            expect(image.avg).to eq(2.5)
        end

        it 'can make an image from a 1d array' do
            image = Vips::Image.new_from_array [1, 2]
            expect(image.width).to eq(2)
            expect(image.height).to eq(1)
            expect(image.bands).to eq(1)
            expect(image.avg).to eq(1.5)
        end

        it 'can use array consts for image args' do
            r = Vips::Image.black(16, 16) 
            r = r.draw_rect 255, 10, 12, 1, 1
            g = Vips::Image.black(16, 16) 
            g = g.draw_rect 255, 10, 11, 1, 1
            b = Vips::Image.black(16, 16) 
            b = b.draw_rect 255, 10, 10, 1, 1
            im = r.bandjoin([g, b])

            expect(im.width).to eq(16)
            expect(im.height).to eq(16)
            expect(im.bands).to eq(3)

            im = im.conv [
                [0.11, 0.11, 0.11],
                [0.11, 0.11, 0.11],
                [0.11, 0.11, 0.11]
            ], :precision => :float

            expect(im.width).to eq(16)
            expect(im.height).to eq(16)
            expect(im.bands).to eq(3)
        end

        it 'can set scale and offset on a convolution mask' do
            image = Vips::Image.new_from_array [1, 2], 8, 2
            expect(image.width).to eq(2)
            expect(image.height).to eq(1)
            expect(image.bands).to eq(1)
            expect(image.scale).to eq(8)
            expect(image.offset).to eq(2)
            expect(image.avg).to eq(1.5)
        end

        if has_jpeg?
            it 'can load a sample jpg image' do
                x = Vips::Image.new_from_file simg("wagon.jpg")
                expect(x.width).to eq(685)
                expect(x.height).to eq(478)
                expect(x.bands).to eq(3)
                expect(x.avg).to be_within(0.001).of(109.789)
            end
        end

        if has_jpeg?
            it 'can extract an ICC profile from a jpg image' do
                x = Vips::Image.new_from_file simg("icc.jpg")
                expect(x.width).to eq(2800)
                expect(x.height).to eq(2100)
                expect(x.bands).to eq(3)
                expect(x.avg).to be_within(0.001).of(109.189)

                profile = x.get_value "icc-profile-data"
                expect(profile.class).to eq(String)
                expect(profile.length).to eq(2360)
            end
        end

        if has_jpeg?
            it 'can set an ICC profile on a jpg image' do
                x = Vips::Image.new_from_file simg("icc.jpg")
                profile = File.open(simg("lcd.icc"), "rb").read
                x.set_value "icc-profile-data", profile
                x.write_to_file(timg("x.jpg"))

                x = Vips::Image.new_from_file timg("x.jpg")
                expect(x.width).to eq(2800)
                expect(x.height).to eq(2100)
                expect(x.bands).to eq(3)
                expect(x.avg).to be_within(0.1).of(109.189)

                profile = x.get_value "icc-profile-data"
                expect(profile.class).to eq(String)
                expect(profile.length).to eq(3048)
            end
        end

        if has_jpeg?
            it 'can load a sample jpg image' do
                x = Vips::Image.new_from_file simg("wagon.jpg")
                expect(x.width).to eq(685)
                expect(x.height).to eq(478)
                expect(x.bands).to eq(3)
                expect(x.avg).to be_within(0.001).of(109.789)
            end
        end

        it 'has binary arithmetic operator overloads with constants' do
            image = Vips::Image.black(16, 16) + 128

            image += 128
            image -= 128
            image *= 2
            image /= 2
            image %= 100
            image += 100
            image **= 2
            image **= 0.5
            image <<= 1
            image >>= 1
            image |= 64
            image &= 32
            image ^= 128

            expect(image.avg).to eq(128) 
        end

        it 'has binary arithmetic operator overloads with array constants' do
            image = Vips::Image.black(16, 16, :bands => 3) + 128

            image += [128, 0, 0]
            image -= [128, 0, 0]
            image *= [2, 1, 1]
            image /= [2, 1, 1]
            image %= [100, 99, 98]
            image += [100, 99, 98]
            image **= [2, 3, 4]
            image **= [1.0 / 2.0, 1.0 / 3.0, 1.0 / 4.0]
            image <<= [1, 2, 3]
            image >>= [1, 2, 3]
            image |= [64, 128, 256]
            image &= [64, 128, 256]
            image ^= [64 + 128, 0, 256 + 128]

            expect(image.avg).to eq(128) 
        end

        it 'has binary arithmetic operator overloads with image args' do
            image = Vips::Image.black(16, 16) + 128
            x = image

            x += image
            x -= image
            x *= image
            x /= image
            x %= image
            x += image
            x |= image
            x &= image
            x ^= image

            expect(image.avg).to eq(128) 
        end

        it 'has relational operator overloads with constants' do
            image = Vips::Image.black(16, 16) + 128

            expect((image > 128).avg).to eq(0) 
            expect((image >= 128).avg).to eq(255) 
            expect((image < 128).avg).to eq(0) 
            expect((image <= 128).avg).to eq(255) 
            expect((image == 128).avg).to eq(255) 
            expect((image != 128).avg).to eq(0) 
        end

        it 'has relational operator overloads with array constants' do
            image = Vips::Image.black(16, 16, :bands => 3) + [100, 128, 130]

            expect((image > [100, 128, 130]).avg).to eq(0) 
            expect((image >= [100, 128, 130]).avg).to eq(255) 
            expect((image < [100, 128, 130]).avg).to eq(0) 
            expect((image <= [100, 128, 130]).avg).to eq(255) 
            expect((image == [100, 128, 130]).avg).to eq(255) 
            expect((image != [100, 128, 130]).avg).to eq(0) 
        end

        it 'has relational operator overloads with image args' do
            image = Vips::Image.black(16, 16) + 128

            expect((image > image).avg).to eq(0) 
            expect((image >= image).avg).to eq(255) 
            expect((image < image).avg).to eq(0) 
            expect((image <= image).avg).to eq(255) 
            expect((image == image).avg).to eq(255) 
            expect((image != image).avg).to eq(0) 
        end

        it 'has band extract with numeric arg' do
            image = Vips::Image.black(16, 16, :bands => 3) + [100, 128, 130]
            x = image[1]

            expect(x.width).to eq(16)
            expect(x.height).to eq(16)
            expect(x.bands).to eq(1)
            expect(x.avg).to eq(128)
        end

        it 'has band extract with range arg' do
            image = Vips::Image.black(16, 16, :bands => 3) + [100, 128, 130]
            x = image[1..2]

            expect(x.width).to eq(16)
            expect(x.height).to eq(16)
            expect(x.bands).to eq(2)
            expect(x.avg).to eq(129)
        end

        it 'has rounding members' do
            # need to avoid rounding down to 0.499999
            image = Vips::Image.black(16, 16) + 0.500001

            expect(image.floor.avg).to eq(0)
            expect(image.ceil.avg).to eq(1)
            expect(image.rint.avg).to eq(1)
        end

        it 'has bandsplit and bandjoin' do
            image = Vips::Image.black(16, 16, :bands => 3) + [100, 128, 130]

            split = image.bandsplit
            x = split[0].bandjoin split[1..2]

            expect(x[0].avg).to eq(100)
            expect(x[1].avg).to eq(128)
            expect(x[2].avg).to eq(130)
        end

        it 'can bandjoin constants' do
            image = Vips::Image.black(16, 16, :bands => 3) + [100, 128, 130]

            x = image.bandjoin 255

            expect(x[0].avg).to eq(100)
            expect(x[1].avg).to eq(128)
            expect(x[2].avg).to eq(130)
            expect(x[3].avg).to eq(255)
            expect(x.bands).to eq(4)

            x = image.bandjoin [1, 2]

            expect(x[0].avg).to eq(100)
            expect(x[1].avg).to eq(128)
            expect(x[2].avg).to eq(130)
            expect(x[3].avg).to eq(1)
            expect(x[4].avg).to eq(2)
            expect(x.bands).to eq(5)

        end

        it 'has minpos/maxpos' do
            image = Vips::Image.black(16, 16) + 128
            image = image.draw_rect 255, 10, 12, 1, 1
            v, x, y = image.maxpos

            expect(v).to eq(255)
            expect(x).to eq(10)
            expect(y).to eq(12)

            image = Vips::Image.black(16, 16) + 128
            image = image.draw_rect 12, 10, 12, 1, 1
            v, x, y = image.minpos

            expect(v).to eq(12)
            expect(x).to eq(10)
            expect(y).to eq(12)

        end

        it 'can form complex images' do
            r = Vips::Image.black(16, 16) + 128
            i = Vips::Image.black(16, 16) + 12
            cmplx = r.complexform i 
            re = cmplx.real
            im = cmplx.imag

            expect(re.avg).to eq(128)
            expect(im.avg).to eq(12)
        end

        it 'can convert complex polar <-> rectangular' do
            r = Vips::Image.black(16, 16) + 128
            i = Vips::Image.black(16, 16) + 12
            cmplx = r.complexform i 

            cmplx = cmplx.rect.polar

            expect(cmplx.real.avg).to be_within(0.001).of(128)
            expect(cmplx.imag.avg).to be_within(0.001).of(12)
        end

        it 'can take complex conjugate' do
            r = Vips::Image.black(16, 16) + 128
            i = Vips::Image.black(16, 16) + 12
            cmplx = r.complexform i 

            cmplx = cmplx.conj

            expect(cmplx.real.avg).to be_within(0.001).of(128)
            expect(cmplx.imag.avg).to be_within(0.001).of(-12)
        end

        it 'has working trig functions' do
            image = Vips::Image.black(16, 16) + 67

            image = image.sin.cos.tan
            image = image.atan.acos.asin

            expect(image.avg).to be_within(0.01).of(67)
        end

        it 'has working log functions' do
            image = Vips::Image.black(16, 16) + 67

            image = image.log.exp.log10.exp10

            expect(image.avg).to be_within(0.01).of(67)
        end

        it 'can flip' do
            a = Vips::Image.black(16, 16) 
            a = a.draw_rect 255, 10, 12, 1, 1
            b = Vips::Image.black(16, 16) 
            b = b.draw_rect 255, 15 - 10, 12, 1, 1

            expect((a - b.fliphor).abs.max).to eq(0.0)

            a = Vips::Image.black(16, 16) 
            a = a.draw_rect 255, 10, 15 - 12, 1, 1
            b = Vips::Image.black(16, 16) 
            b = b.draw_rect 255, 10, 12, 1, 1

            expect((a - b.flipver).abs.max).to eq(0.0)
        end

        it 'can getpoint' do
            a = Vips::Image.black(16, 16) 
            a = a.draw_rect 255, 10, 12, 1, 1
            b = Vips::Image.black(16, 16) 
            b = b.draw_rect 255, 10, 10, 1, 1
            im = a.bandjoin(b)

            expect(im.getpoint(10, 12)).to eq([255, 0])
            expect(im.getpoint(10, 10)).to eq([0, 255])
        end

        it 'can median' do
            a = Vips::Image.black(16, 16) 
            a = a.draw_rect 255, 10, 12, 1, 1
            im = a.median

            expect(im.max).to eq(0)
        end

        it 'can erode' do
            a = Vips::Image.black(16, 16) 
            a = a.draw_rect 255, 10, 12, 1, 1
            mask = Vips::Image.new_from_array [
                [128, 255, 128],
                [255, 255, 255],
                [128, 255, 128]
            ]
            im = a.erode mask

            expect(im.max).to eq(0)
        end

        it 'can dilate' do
            a = Vips::Image.black(16, 16) 
            a = a.draw_rect 255, 10, 12, 1, 1
            mask = Vips::Image.new_from_array [
                [128, 255, 128],
                [255, 255, 255],
                [128, 255, 128]
            ]
            im = a.dilate mask

            expect(im.getpoint(10, 12)).to eq([255])
            expect(im.getpoint(11, 12)).to eq([255])
            expect(im.getpoint(12, 12)).to eq([0])
        end

        it 'can rot' do
            a = Vips::Image.black(16, 16) 
            a = a.draw_rect 255, 10, 12, 1, 1

            im = a.rot90.rot90.rot90.rot90
            expect((a - im).abs.max).to eq(0.0)

            im = a.rot180.rot180
            expect((a - im).abs.max).to eq(0.0)

            im = a.rot270.rot270.rot270.rot270
            expect((a - im).abs.max).to eq(0.0)
        end

        it 'can bandbool' do
            a = Vips::Image.black(16, 16) 
            a = a.draw_rect 255, 10, 12, 1, 1
            b = Vips::Image.black(16, 16) 
            b = b.draw_rect 255, 10, 10, 1, 1
            im = a.bandjoin(b)

            expect(im.bandand.getpoint(10, 12)).to eq([0])
            expect(im.bandor.getpoint(10, 12)).to eq([255])
            expect(im.bandeor.getpoint(10, 12)).to eq([255])
        end

        it 'ifthenelse with image arguments' do
            image = Vips::Image.black(16, 16) 
            image = image.draw_rect 255, 10, 12, 1, 1
            black = Vips::Image.black(16, 16) 
            white = Vips::Image.black(16, 16) + 255

            result = image.ifthenelse black, white

            v, x, y = result.minpos

            expect(v).to eq(0)
            expect(x).to eq(10)
            expect(y).to eq(12)
        end

        it 'ifthenelse with constant arguments' do
            image = Vips::Image.black(16, 16) 
            image = image.draw_rect 255, 10, 12, 1, 1

            result = image.ifthenelse 0, 255

            v, x, y = result.minpos

            expect(v).to eq(0)
            expect(x).to eq(10)
            expect(y).to eq(12)
        end

        it 'ifthenelse with vector arguments' do
            image = Vips::Image.black(16, 16) 
            image = image.draw_rect 255, 10, 12, 1, 1
            white = Vips::Image.black(16, 16) + 255

            result = image.ifthenelse [255, 0, 0], white

            v, x, y = result.minpos

            expect(v).to eq(0)
            expect(x).to eq(10)
            expect(y).to eq(12)
        end

    end

end

