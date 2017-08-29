#!/usr/bin/env ruby

require 'logger'
require 'vips'

GLib::logger.level = Logger::DEBUG

op = Vips::Operation.new "black"

op = nil
GC.start
Vips::Operation.print_all
