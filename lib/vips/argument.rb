
module Vips

    # This class is used internally to convert Ruby values to arguments to 
    # libvips operations. 
    # @private
    class Argument 
        attr_reader :op, :name, :flags, :priority, :isset, :prop
        attr_reader :blurb, :gtype, :type

        # map gobject-introspection's ruby class names to ours
        @@map_goi_to_vips = {
            "TrueClass" => "Boolean",
            "Vips::ArrayDouble" => "Array<Double>",
            "Vips::ArrayInt" => "Array<Integer>",
            "Vips::ArrayImage" => "Array<Image>",
            "Vips::ArrayString" => "Array<String>",
        }

        def initialize(op, name)
            @op = op
            @name = name.tr '-', '_'
            @prop = op.gtype.to_class.property name
            @blurb = @prop.blurb
            @gtype = prop.value_type
            @flags = op.get_argument_flags name
            @priority = op.get_argument_priority @name
            @isset = op.argument_isset @name

            type = GLib::Type[gtype.name].to_class.name
            type = @@map_goi_to_vips[type] if @@map_goi_to_vips.include? type
            @type = type
        end

        private

        def self.imageize match_image, value
            return value if match_image == nil
            return value if value.is_a? Vips::Image

            # 2D array values become tiny 2D images
            if value.is_a? Array and value[0].is_a? Array
                return Vips::Image.new_from_array value
            end

            # if there's nothing to match to, we also make a 2D image
            if match_image == nil
                return Vips::Image.new_from_array value
            end

            # we have a 1D array ... use that as a pixel constant and expand 
            # to match match_image
            pixel = (Vips::Image.black(1, 1) + value).cast(match_image.format)
            pixel = pixel.copy :interpretation => match_image.interpretation,
                :xres => match_image.xres, :yres =>  match_image.yres
            pixel.embed(0, 0, match_image.width, match_image.height, 
                        :extend => :copy)
        end

        # @private
        class ArrayImageConst < Vips::ArrayImage 
            def self.new(value)
                if not value.is_a? Array
                    value = [value]
                end

                match_image = value.find {|x| x.is_a? Vips::Image}
                if match_image == nil
                    raise Vips::Error, 
                        "Argument must contain at least one image."
                end

                value = value.map {|x| Argument::imageize match_image, x}

                # we'd like to just 
                #   super(value)
                # to construct, but the gobject-introspection gem does not
                # support new from object array ... instead, we build in stages
                array = Vips::ArrayImage.empty
                value.each {|x| array = array.append(x)}

                return array
            end
        end

        # if this gtype needs an array, try to transform the value into one
        def self.arrayize(gtype, value)
            arrayize_map = {
                GLib::Type["VipsArrayDouble"] => Vips::ArrayDouble,
                GLib::Type["VipsArrayInt"] => Vips::ArrayInt,
                GLib::Type["VipsArrayImage"] => ArrayImageConst
            }

            if arrayize_map.has_key? gtype
                if not value.is_a? Array
                    value = [value]
                end

                value = arrayize_map[gtype].new(value)
            end

            value
        end

        def self.unwrap value
            [Vips::Blob, Vips::ArrayDouble, Vips::ArrayImage, 
                Vips::ArrayInt, Vips::RefString].each do |cls|
                if value.is_a? cls
                    value, length = value.get

                    # blobs come from gobject-introspection as arrays ... 
                    # repack as strings for convenience
                    if value and cls == Vips::Blob
                        value = value.pack("C*")
                    end

                end

            end

            value
        end

        public

        def set_value(match_image, value)
            # array-ize
            value = Argument::arrayize gtype, value

            # blob-ize
            if gtype.type_is_a? GLib::Type["VipsBlob"]
                if not value.is_a? Vips::Blob
                    value = Vips::Blob.copy value
                end
            end

            # image-ize
            if gtype.type_is_a? GLib::Type["VipsImage"]
                if not value.is_a? Vips::Image
                    value = Argument::imageize match_image, value
                end
            end

            # MODIFY input images need to be copied before assigning them
            if (flags & :modify) != 0
                # don't use .copy(): we want to make a new pipeline with no
                # reference back to the old stuff ... this way we can free the
                # previous image earlier 
                new_image = Vips::Image.memory
                value.write new_image
                value = new_image
            end

            op.set_property @name, value
        end

        def get_value
            Argument::unwrap @op.get_property(@name)
        end

    end

end
