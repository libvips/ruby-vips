module Vips

    # add a conventience method to Operation
    # @private
    class Operation
        # Fetch arg list, remove boring ones, sort into priority order.
        def get_args
            gobject_class = gtype.to_class
            props = gobject_class.properties.select do |name|
                flags = get_argument_flags name
                io = ((flags & :input) | (flags & :output)) != 0
                dep = (flags & :deprecated) != 0
                io & (not dep)
            end
            args = props.map {|name| Argument.new self, name}
            args.sort! {|a, b| a.priority - b.priority}
        end
    end
end
