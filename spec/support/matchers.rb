require 'digest/sha1'

Spec::Matchers.define :match_sha1 do |sha1|
  match do |image|
    Digest::SHA1.hexdigest(image.data) == sha1
  end
end

Spec::Matchers.define :match_image do |target|
  match do |image|
    sha_ones = [image, target].map do |i|
      Digest::SHA1.hexdigest i.data
    end
    sha_ones[0] == sha_ones[1]
  end
end

Spec::Matchers.define :approximate do |target|
  match do |approximation|
    difference = (target - approximation).abs
    difference == 0 || (difference / approximation) < 0.01
  end
end

Spec::Matchers.define :be_in_array do |array|
  match do |value|
    array.include?(value)
  end
end

