require 'digest/sha1'

RSpec::Matchers.define :match_sha1 do |sha1|
  match do |image|
    actual_sha1 = Digest::SHA1.hexdigest(image.data) 
    if actual_sha1 != sha1
        puts "expected #{sha1}, saw #{actual_sha1}"
    end
    Digest::SHA1.hexdigest(image.data) == sha1
  end
end

RSpec::Matchers.define :match_image do |target|
  match do |image|
    sha_ones = [image, target].map do |i|
      Digest::SHA1.hexdigest i.data
    end
    sha_ones[0] == sha_ones[1]
  end
end

RSpec::Matchers.define :approximate do |target|
  match do |approximation|
    difference = (target - approximation).abs
    difference == 0 || (difference / approximation) < 0.01
  end
end

RSpec::Matchers.define :be_in_array do |array|
  match do |value|
    array.include?(value)
  end
end

