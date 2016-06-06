#!/usr/bin/ruby

require 'vips'
$vips_debug = true

puts ""
puts "starting up:"

# this makes vips keep a list of all active objects which we can print out
Vips::leak_set true

# disable the operation cache
Vips::cache_set_max 0

puts ""
puts "creating object:"
x = Vips::Image.new
Vips::Object::print_all

puts ""
puts "freeing object:"
x = nil
GC.start
Vips::Object::print_all

puts ""
puts "creating operation:"
op = Vips::Operation.new "black"
Vips::Object::print_all
op.set_property "width", 200
op.set_property "height", 300

puts ""
puts "after operation init:"
GC.start
Vips::Object::print_all

puts ""
puts "operation lookup:"
op2 = Vips::cache_operation_lookup op
if op2
    puts "cache hit"
    op = op2
    op2 = nil
else
    puts "cache miss ... building"
    if not op.build
        puts "*** build error"
    end
    Vips::cache_operation_add op
end

puts ""
puts "after build:"
GC.start
Vips::Object::print_all

puts ""
puts "fetching output:"
im = op.get_property "out"
GC.start
Vips::Object::print_all

puts ""
puts "fetching output again:"
im2 = op.get_property "out"
GC.start
Vips::Object::print_all

puts ""
puts "freeing operation:"
op.unref_outputs
op = nil
op2 = nil
GC.start
Vips::Object::print_all

puts ""
puts "shutting down:"
im = nil
im2 = nil
GC.start
Vips::shutdown
GC.start
