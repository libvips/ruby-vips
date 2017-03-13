module Vips

    # Call a vips operation.
    #
    # This will crash if there's a GC during the call, only use 
    # this via {Vips.call}. 
    private 
    class Call
        attr_writer :instance, :option_string

        def initialize(name, supplied_values)
            @name = name
            @supplied_values = supplied_values
            @instance = nil
            @option_string = nil

            if @supplied_values.last.is_a? Hash
                @optional_values = @supplied_values.last
                @supplied_values.delete_at -1
            else
                @optional_values = {}
            end

            begin
                @op = Vips::Operation.new @name
            rescue
                raise Vips::Error, "no operator '#{@name}'"
            end

            log "Vips::Call.init"
            log "name = #{@name}"
            log "supplied_values are:"
            @supplied_values.each {|x| log "   #{x}"}
            log "optional_values are:"
            @optional_values.each {|x| log "   #{x}"}
        end

        # set any string options on the operation
        def set_string_args
            if @option_string
                log "setting string options #{@option_string} ..."

                if @op.set_from_string(@option_string) != 0
                    raise Error
                end
            end
        end

        # set @match_image to be the image we build constants to match
        def find_match_image
            # the instance, if supplied, must be a vips image ... we use it for
            # match_image, below
            if @instance and not @instance.is_a? Vips::Image
                raise Vips::Error, "@instance is not a Vips::Image."
            end

            # if the op needs images but the user supplies constants, we expand
            # them to match the first input image argument ... find the first
            # image
            log "searching for first image argument ..."
            match_image = @instance
            if match_image == nil
                match_image = @supplied_values.find {|x| x.is_a? Vips::Image}
            end
            if match_image == nil
                match = @optional_values.find do |name, value|
                    value.is_a? Vips::Image
                end
                # if we found a match, it'll be [name, value]
                if match
                    match_image = match[1]
                end
            end

            return match_image
        end

        # look through the operation args and find required and optional 
        # input args
        def find_input
            all_args = @op.get_args

            # find unassigned required input args
            log "finding unassigned required input arguments ..."
            required_input = all_args.select do |arg|
                not arg.isset and
                (arg.flags & :input) != 0 and
                (arg.flags & :required) != 0 
            end

            # find optional unassigned input args
            log "finding optional unassigned input arguments ..."
            optional_input = all_args.select do |arg|
                not arg.isset and
                (arg.flags & :input) != 0 and
                (arg.flags & :required) == 0 
            end

            # make a hash from name to arg for the options
            optional_input = Hash[
                optional_input.map(&:name).zip(optional_input)]

            return required_input, optional_input
        end

        def find_optional_output
            all_args = @op.get_args

            log "finding optional output arguments ..."
            optional_output = all_args.select do |arg|
                (arg.flags & :output) != 0 and
                (arg.flags & :required) == 0 
            end
            optional_output = Hash[
                optional_output.map(&:name).zip(optional_output)]

            return optional_output

        end

        def set_required_input(match_image, required_input)
            log "set_required_input: @instance = #{@instance}"

            # do we have a non-nil instance? set the first image arg with this
            if @instance != nil
                log "BANANA"
                log "setting first image arg with instance ..."
                x = required_input.find do |arg|
                    gtype = GLib::Type["VipsImage"]
                    vtype = arg.prop.value_type

                    vtype.type_is_a? gtype
                end
                if x == nil
                    raise Vips::Error, 
                        "No #{@instance.class} argument to #{@name}."
                end
                x.set_value match_image, @instance
                required_input.delete x
            end

            if required_input.length != @supplied_values.length
                raise Vips::Error, 
                    "Wrong number of arguments. '#{@name}' requires " +
                    "#{required_input.length} arguments, you supplied " +
                    "#{@supplied_values.length}."
            end

            log "setting required input arguments ..."
            required_input.zip(@supplied_values).each do |arg, value|
                arg.set_value match_image, value
            end

        end       

        def set_optional_input(match_image, optional_input, optional_output)
            log "setting optional input args ..."
            @optional_values.each do |name, value|
                # we are passed symbols as keys
                name = name.to_s
                if optional_input.has_key? name
                    log "setting #{name} to #{value}"
                    optional_input[name].set_value match_image, value
                elsif optional_output.has_key? name and value != true
                    raise Vips::Error, 
                        "Optional output argument #{name} must be true."
                elsif not optional_output.has_key? name 
                    raise Vips::Error, "No such option '#{name}',"
                end
            end
        end

        def build
            log "building ..."

            op2 = Vips::cache_operation_lookup @op
            if op2
                log "cache hit"

                hit = true
                @op = op2
            else
                log "cache miss ... building"

                hit = false
                if @op.build() != 0
                    raise Vips::Error
                end
                # showall

                log "adding to cache ... "
                Vips::cache_operation_add @op
            end

            return hit
        end

        def fetch_output(optional_output)
            log "fetching outputs ..."

            # gather output args 
            out = []

            all_args = @op.get_args
            all_args.each do |arg|
                # required output
                if (arg.flags & :output) != 0 and
                    (arg.flags & :required) != 0 
                    log "fetching required output #{arg.name}"
                    out << arg.get_value
                end

                # modified input arg ... this will get the result of the 
                # copy() we did in Argument.set_value 
                if (arg.flags & :input) != 0 and
                    (arg.flags & :modify) != 0 
                    log "fetching modified input arg ..."
                    out << arg.get_value
                end
            end

            opts = {}
            @optional_values.each do |name, value|
                # we are passed symbols as keys
                name = name.to_s
                if optional_output.has_key? name
                    log "fetching optional output arg ..."
                    opts[name] = optional_output[name].get_value
                end
            end
            out << opts if opts != {}

            if out.length == 1
                out = out[0]
            elsif out.length == 0
                out = nil
            end

            return out

        end

        def invoke
            log "invoke starting ..."
            log "invoke instance is #{@instance}"
            set_string_args()
            match_image = find_match_image()
            required_input, optional_input = find_input()
            optional_output = find_optional_output()

            set_required_input(match_image, required_input)
            set_optional_input(match_image, optional_input, optional_output)

            hit = build()

            # if there was a cache hit, we need to refind this since all the arg
            # pointers will have changed
            if hit
                optional_output = find_optional_output()
            end
            out = fetch_output(optional_output)

            log "unreffing outputs ..."
            @op.unref_outputs()
            @op = nil
            # showall

            log "success! #{@name}.out = #{out}"

            return out
        end

    end

    # full-fat call, with the GC disabled
    private
    def self.call_base(name, instance, option_string, supplied_values)
        gc_was_enabled = GC.disable
        begin
            call = Call.new(name, supplied_values)
            call.instance = instance
            call.option_string = option_string
            result = call.invoke
        ensure
            GC.enable if gc_was_enabled
        end

        return result
    end

    public

    # This is the public entry point for the vips binding. {call} will run
    # any vips operation, for example:
    #
    # ```ruby
    # out = Vips::call "black", 100, 100, :bands => 12
    # ```
    #
    # will call the C function 
    #
    # ```C
    # vips_black( &out, 100, 100, "bands", 12, NULL );
    # ```
    # 
    # There are {Image#method_missing} hooks which will run {call} for you 
    # on {Image} for undefined instance or class methods. So you can also 
    # write:
    #
    # ```ruby
    # out = Vips::Image.black 100, 100, :bands => 12
    # ```
    #
    # Or perhaps:
    #
    # ```ruby
    # x = Vips::Image.black 100, 100
    # y = x.invert
    # ```
    #
    # to run the `vips_invert()` operator.
    #
    # There are also a set of operator overloads and some convenience functions,
    # see {Image}. 
    #
    # If the operator needs a vector constant, {call} will turn a scalar into a
    # vector for you. So for `x.linear(a, b)`, which calculates 
    # `x * a + b` where `a` and `b` are vector constants, you can write:
    #
    # ```ruby
    # x = Vips::Image.black 100, 100, :bands => 3
    # y = x.linear(1, 2)
    # y = x.linear([1], 4)
    # y = x.linear([1, 2, 3], 4)
    # ```
    #
    # or any other combination. The operator overloads use this facility to
    # support all the variations on:
    #
    # ```ruby
    # x = Vips::Image.black 100, 100, :bands => 3
    # y = x * 2
    # y = x + [1,2,3]
    # y = x % [1]
    # ```
    #
    # Similarly, wherever an image is required, you can use a constant. The
    # constant will be expanded to an image matching the first input image
    # argument. For example, you can write:
    #
    # ```
    # x = Vips::Image.black 100, 100, :bands => 3
    # y = x.bandjoin(255)
    # ```
    #
    # to add an extra band to the image where each pixel in the new band has 
    # the constant value 255. 

    def self.call(name, *args)
        gc_was_enabled = GC.disable
        begin
            call = Call.new(name, args)
            result = call.invoke
        ensure
            GC.enable if gc_was_enabled
        end

        return result
    end

end
