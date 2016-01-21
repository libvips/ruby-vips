#!/usr/bin/env ruby

require 'rubygems'
require 'vips'

repeat = 2000

def rss
    `ps -o rss= -p #{$$}`.chomp.to_i/1024
end

GC.start
puts "RSS at startup with gems loaded: %d MB" % [rss]

repeat.times do |i|
    ARGV.each do |filename|
        img = VIPS::Image.jpeg filename, :sequential => true

        img = VIPS::JPEGWriter.new(img, quality: 50)
        img.write('test.jpg')

        print "\rIteration: %-8d RSS: %6d MB File: %-32s".freeze % [i+1, rss, filename]
    end
end
puts

GC.start
puts "RSS at exit: %d MB" % [rss]
