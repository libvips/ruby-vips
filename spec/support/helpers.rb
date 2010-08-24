require 'digest/sha1'
require 'tempfile'

module Spec
  module Helpers
    def reset_working!
      FileUtils.rm Dir[tmp.join('*.*')]
      FileUtils.mkdir_p(tmp)
    end

    def reset_formats!
      FileUtils.rm Dir[formats.join('*.*')]
      FileUtils.mkdir_p(formats)
    end

    def show_image(im, test_name = nil)
      dir = "preview#{test_name}"
      FileUtils.mkdir_p tmp(dir)

      view_path = tmp dir, 'vips_rspec_view.jpg'
      html_path = tmp dir, 'vips_rspec_view.html'

      im.jpeg view_path.to_s
      sha1 = Digest::SHA1.hexdigest im.data

      title = "Vips Rspec #{test_name}"
      File.open(html_path, 'w') do |f|
        f << "<html><title>#{title}</title><body><img src=\"#{view_path}\"/><p>#{title}</p><p>#{sha1}</p></body></html>"
      end

      fork do
#        exec "/usr/bin/firefox #{html_path}"
        exec "/Applications/Firefox.app/Contents/MacOS/firefox #{html_path}"
      end
    end

    def self.match_vips_version(version)
      return true if version.nil?
      op, maj, min, mic = parse_version_requirement version
      ver = VIPS::LIB_VERSION_ARRAY

      tests = [ver[0] <=> maj]
      tests << ( ver[1] <=> min ) unless min.nil?
      tests << ( ver[2] <=> mic ) unless mic.nil?

      eq = tests.all?{ |t| t == 0 }
      gt = tests[0] == 1 || tests[0..1] == [0, 1] || tests == [0, 0, 1]
      lt = tests[0] == -1 || tests[0..1] == [0, -1] || tests == [0, 0, -1]

      case op
      when NilClass, '=' then eq
      when '!=', '=!'    then !eq
      when '>'           then gt
      when '<'           then lt
      when '>=', '=>'    then ( gt || eq )
      when '<=', '=<'    then ( lt || eq )
      end
    end

    def self.parse_version_requirement(version)
      parts = version.match /\A([!><=~]+ )?(\d+)(\.\d+)?(\.\d+)?\Z/
      op = parts[1] ? parts[1].rstrip : nil
      maj = parts[2].to_i
      min = parts[3] ? parts[3][1..-1].to_i : nil
      mic = parts[4] ? parts[4][1..-1].to_i : nil

      [op, maj, min, mic]
    end

  end
end

