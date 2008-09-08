#!/usr/bin/ruby

# Copyright (c) 2008 Patric Müller
# bhaak@gmx.net
#
# Released under the GNU GPL version 2 - http://www.gnu.org/copyleft/gpl.html

require 'parsearg.rb'

require "rexml/document"
require "rexml/xpath"

require 'date'
require 'md5'

require 'rss/maker'
require 'rss/dublincore'

require "atom/entry"
require "atom/feed"
require "atom/element"

######################################################################

def usage()
	printf "%s [--rss|--atom] [--version] file\n", $0
end

######################################################################

def parseargs()
	$USAGE = 'usage'

	parseArgs(0, false, "version", "rss", "atom")

	if ($OPT_version)
		printf "update version 1.0 (08.09.2008)\n"
		exit(0)
	end
	if ($OPT_rss)
		$rss = true
		#$stderr.puts "RSS-Ausgabe"
	end
	if ($OPT_atom)
		$atom = true
	end

end

######################################################################

def update(document, rss)
	url_base_link = "http://nethack-de.sourceforge.net/"
	feed_title = "NetHack-De-Updates"

	feed = Atom::Feed.new()
	feed_name = "atom.xml"

	rss = RSS::Maker.make("2.0") do |maker|
	#rss = RSS::Maker.make("0.9")
		maker.channel.title = feed_title
		feed.title = feed_title
		maker.channel.link = url_base_link
		atom_link = Atom::Link.new()
		atom_link['href'] = url_base_link
		feed.links << atom_link
		
		atom_link = Atom::Link.new()
		atom_link['rel'] = "self"
		atom_link['href'] = url_base_link+feed_name
		feed.links << atom_link

		maker.channel.description = feed_title
		feed.subtitle = feed_title
		feed.updated = DateTime.now.to_s
		feed.id = url_base_link

		atom_author = Atom::Author.new()
		atom_author.name = "Patric Mueller"
		atom_author.email = "bhaak@gmx.net"
		#atom_author.uri = url_base_link
		feed.authors << atom_author

		hrefattributes = REXML::XPath.match(document, "//update");

		hrefattributes.sort{|a,b| Date.parse(a.attributes['date']) <=> Date.parse(b.attributes['date'])}.reverse.each { |element| 
			updateDatum = DateTime.parse(element.attributes['date']).to_s
			$stderr.puts updateDatum


			atom_entry = Atom::Entry.new()
			rss_item = maker.items.new_item
			rss_item.date = Time.parse(element.attributes['date'])
			atom_entry.updated = updateDatum
			#atom_entry.authors << atom_author
			if element.attributes['title'] != nil then
				rss_item.title = element.attributes['title']
				atom_entry.title = element.attributes['title']
			else
				rss_item.title = element.children[1].get_text.to_s
				atom_entry.title = element.children[1].get_text.to_s
			end

			description = "<ul>\n"
			element.each_element { | entry|
				description +=  "<li>" + entry.get_text.to_s + "</li>\n"
			}
			description += "</ul>\n"


			rss_item.description = description
			atom_entry.content = description 
			atom_entry.content['type'] = "html"

			atom_entry_link = Atom::Link.new()
			atom_entry_link['href'] = url_base_link
			atom_entry.links << atom_entry_link
			rss_item.link = url_base_link

			rss_item.guid.isPermaLink = "false"
			rss_item.guid.content = MD5.new(rss_item.title+updateDatum).hexdigest
			atom_entry.id = "urn:uuid:"+MD5.new(rss_item.title+updateDatum).hexdigest
			feed.entries << atom_entry
		}
    
	end

	if $rss then
		$stdout.puts rss.to_s
	elsif $atom then
		REXML::Document.new(feed.to_s).write($stdout,0)
	end
end

######################################################################

def createXmlFromNewsExample(news)
	xml = REXML::Document.new
	updates = REXML::Element.new("updates")
	update = REXML::Element.new("update")
	update.add_attribute("date","10 Aug 2008")
	entry = REXML::Element.new("entry")
	entry.text = "neuer Release, juhu!"
	update.add_element entry
	entry = REXML::Element.new("entry")
	entry.text = "und viele Bugfixes"
	update.add_element entry
	updates.add_element update
	xml.add_element updates

	return xml
end

def createXmlFromNews(news)
	xml = REXML::Document.new
	updates = REXML::Element.new("updates")
	update = nil
	entry = nil

	found_entry = false
	news.readlines.each {|line|
		
		#$stderr.puts line
		case line
		when /^h4/
			update = REXML::Element.new("update")
			update.add_attribute("date",Date.strptime(line[9,10], "%Y-%m-%d"))
			update.add_attribute("title", line[line.index(": ")+2..-1].strip)
		when /^\*/
			entry = REXML::Element.new("entry")
			entry.text = line[2..-1].strip
			update.add_element entry
			found_entry = true
		when /^$/
			if (found_entry) then
				updates.add_element update
				found_entry = false
			end
		end
	}
	xml.add_element updates

	return xml
end

######################################################################

parseargs()

if ARGV.size == 0
	usage()
elsif ARGV.size == 1 # input stdin, output stdout
	news = File.open(ARGV[0])
	doc = createXmlFromNews(news)
	#$stderr.puts doc
	#$stderr.puts
	update(doc, false)
end

