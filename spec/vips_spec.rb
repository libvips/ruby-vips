require 'spec_helper.rb'

RSpec.describe Vips do
    describe '#call' do
        it 'can make a black image' do
            image = Vips::call "black", 200, 200

            expect(image.width).to eq(200)
            expect(image.height).to eq(200)
            expect(image.bands).to eq(1)
        end

        it 'can take an optional argument' do
            image = Vips::call "black", 200, 200, :bands => 12

            expect(image.width).to eq(200)
            expect(image.height).to eq(200)
            expect(image.bands).to eq(12)
        end

        it 'can take an optional argument' do
            image = Vips::call "black", 200, 200, :bands => 12

            expect(image.width).to eq(200)
            expect(image.height).to eq(200)
            expect(image.bands).to eq(12)
        end

        it 'can handle enum arguments' do
            black = Vips::call "black", 200, 200
            embed = Vips::call "embed", black, 10, 10, 500, 500, 
                :extend => :mirror

            expect(embed.width).to eq(500)
            expect(embed.height).to eq(500)
            expect(embed.bands).to eq(1)
        end

        it 'enum arguments can be strings' do
            black = Vips::call "black", 200, 200
            embed = Vips::call "embed", black, 10, 10, 500, 500, 
                :extend => "mirror"

            expect(embed.width).to eq(500)
            expect(embed.height).to eq(500)
            expect(embed.bands).to eq(1)
        end

        it 'can return optional output args' do
            point = Vips::call "black", 1, 1
            test = Vips::call "embed", point, 20, 10, 100, 100, 
                :extend => :white
            value, opts = Vips::call "min", test, :x => true, :y => true

            expect(value).to eq(0)
            expect(opts['x']).to eq(20)
            expect(opts['y']).to eq(10)
        end

        it 'can call draw operations' do
            black = Vips::call "black", 100, 100
            test = Vips::call "draw_rect", black, 255, 10, 10, 1, 1

            max_black = Vips::call "max", black
            max_test = Vips::call "max", test

            expect(max_black).to eq(0)
            expect(max_test).to eq(255)
        end

    end

end

