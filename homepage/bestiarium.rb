#!/usr/bin/env ruby
#
# Copyright (c) 2009-2014 Patric Mueller <bhaak@gmx.net>
#
# Released under the MIT license - http://www.opensource.org/licenses/mit-license.php

require "rexml/document"
require "rexml/xpath"
require "fileutils"

FileUtils.mkdir_p('content/nhtohtml/')

files = Dir.glob("../nhtohtml/html/*.html")
files.each {|file|
	puts file
	new_file = File.basename(file, ".html")
	xml = REXML::Document.new(File.read(file))
	#puts xml.get_text("/html/head/title")
	File.open("content/nhtohtml/"+new_file+".erb", 'w') {|f|
		f.puts "---"
		f.puts "title: "+xml.get_text("/html/head/title").to_s.gsub(": ", "&#58; ")
		f.puts "---"
		xml.each_element("/html/body") {|b|
			f.puts b
		}
	}
}
