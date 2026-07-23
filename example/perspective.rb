#!/usr/bin/ruby
  
require 'vips'

image = Vips::Image.new_from_file ARGV[0]

# perspective distortion: each pixel (x', y') in the output image is
# interpolated from pixel (x, y) in the input using:
#
#   x' = (A x + B y + C) / (G x + H y + 1)
#   y' = (D x + E y + F) / (G x + H y + 1)
#
# where the constants A .. H are from the transform matrix T
#
# T = [A, B, .. H]

T = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0003, 0.0001]

# make an index image where pixels have the value of their (x, y) coordinates
i = Vips::Image.xyz image.width, image.height

x = (i[0] * T[0] + i[1] * T[1] + T[2]) / (i[0] * T[6] + i[1] * T[7] + 1)
y = (i[0] * T[3] + i[1] * T[4] + T[5]) / (i[0] * T[6] + i[1] * T[7] + 1)

# join up x and y as a map image
m = x.bandjoin y

# and use it to transform our original image
image = image.mapim m 

image.write_to_file ARGV[1]
