require 'spec_helper.rb'

RSpec.describe Vips do
    describe '#call' do
        it 'can make a black image' do
            image = Vips::Operation.call "black", [200, 200]

            expect(image.width).to eq(200)
            expect(image.height).to eq(200)
            expect(image.bands).to eq(1)
        end

        it 'can take an optional argument' do
            image = Vips::Operation.call "black", [200, 200, :bands => 12]

            expect(image.width).to eq(200)
            expect(image.height).to eq(200)
            expect(image.bands).to eq(12)
        end

        it 'can take an optional argument' do
            image = Vips::Operation.call "black", [200, 200, :bands => 12]

            expect(image.width).to eq(200)
            expect(image.height).to eq(200)
            expect(image.bands).to eq(12)
        end

        it 'can handle enum arguments' do
            black = Vips::Operation.call "black", [200, 200]
            embed = Vips::Operation.call "embed", [black, 10, 10, 500, 500, 
                :extend => :mirror]

            expect(embed.width).to eq(500)
            expect(embed.height).to eq(500)
            expect(embed.bands).to eq(1)
        end

        it 'enum arguments can be strings' do
            black = Vips::Operation.call "black", [200, 200]
            embed = Vips::Operation.call "embed", [black, 10, 10, 500, 500, 
                :extend => "mirror"]

            expect(embed.width).to eq(500)
            expect(embed.height).to eq(500)
            expect(embed.bands).to eq(1)
        end

        it 'can return optional output args' do
            point = Vips::Operation.call "black", [1, 1]
            test = Vips::Operation.call "embed", [point, 20, 10, 100, 100, 
                :extend => :white]
            value, opts = Vips::Operation.call "min", [test, 
                :x => true, :y => true]

            expect(value).to eq(0)
            expect(opts['x']).to eq(20)
            expect(opts['y']).to eq(10)
        end

        it 'can call draw operations' do
            black = Vips::Operation.call "black", [100, 100]
            test = Vips::Operation.call "draw_rect", [black, 255, 10, 10, 1, 1]

            max_black = Vips::Operation.call "max", [black]
            max_test = Vips::Operation.call "max", [test]

            expect(max_black).to eq(0)
            expect(max_test).to eq(255)
        end

        it 'can throw errors for failed operations' do
            black = Vips::Operation.call "black", [100, 1]

            expect{black.resize(0.4)}.to raise_exception(Vips::Error)
        end

    end

end

