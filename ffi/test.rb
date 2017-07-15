#!/usr/bin/env ruby

require_relative 'ffi-vips'

puts "creating gvalue with GLib::GValue"
x = GLib::GValue.alloc 
gtype = GLib::g_type_from_name "gboolean"
puts "GLib::g_type_from_name 'gboolean' = #{gtype}"
x.init gtype
x.set true
puts "x = #{x}"
puts "x.get = #{x.get}"
puts ""

puts "creating enum"
# need to init this by hand, it's not created normally until the first image is
# made
Vips::vips_interpretation_get_type
x = GLib::GValue.alloc 
gtype = GLib::g_type_from_name "VipsInterpretation"
puts "GLib::g_type_from_name 'VipsInterpretation' = #{gtype}"
x.init gtype
x.set :lab
puts "x = #{x}"
puts "x.get = #{x.get}"
puts ""

puts "creating image"
x = Vips::Image.new_partial
Vips::showall
puts "x = #{x}"
puts "x.get_typeof('width') = #{x.get_typeof('width')}"
puts "Vips::GINT_TYPE = #{GLib::GINT_TYPE}"
puts "x.get('width') = #{x.get('width')}"
puts "x.set('width', 99)"
x.set('width', 99)
puts "x.get('width') = #{x.get('width')}"
puts "x[:parent] = #{x[:parent]}"
puts "x[:parent][:description] = #{x[:parent][:description]}"
x = nil
GC.start
Vips::showall
puts ""

def show_flags(table, flags)
    set = []
    table.each do |key, value|
        set << key.to_s if (flags & value) != 0
    end

    set.join " " 
end

puts "creating operation"
x = Vips::Operation.new_from_name "perlin"
puts "x = #{x}"
x.argument_map do |pspec, argument_class, argument_instance|
    puts "in arg_map fn"
    puts "   pspec[:name] = #{pspec[:name]}"
    puts "   argument_class = #{argument_class}"
    puts "   argument_instance = #{argument_instance}"
    puts "   flags = #{show_flags(Vips::ARGUMENT_FLAGS, argument_class[:flags])}"
end
args = x.get_construct_args
puts "x.get_construct_args = #{args}"
args.each do |name, flags| 
    puts "#{name} = #{flags}"
end
x.set("width", 100)
x.set("height", 100)
x = nil
puts ""

puts "calling Operation::call"
x = Vips::Operation.call "perlin", [128, 128]
puts "x = #{x}"
x = nil
puts ""

puts "calling Image::new_from_file"
x = Vips::Image.new_from_file "/home/john/pics/k2.jpg"
puts "x = #{x}"
x = nil
puts ""

# x = x.invert
# x.write_to_file "x.jpg"
