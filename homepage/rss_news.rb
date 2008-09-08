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
		puts "RSS-Ausgabe"
	end
	if ($OPT_atom)
		$atom = true
	end

end

######################################################################

def update(document, rss)
	url_base_link = "http://nethack-de.sourceforge.net/"

	feed = Atom::Feed.new()
	feed_name = "atom.xml"

	rss = RSS::Maker.make("2.0") do |maker|
	#rss = RSS::Maker.make("0.9")
	maker.channel.title = "Updates der NetHack-De-Homepage"
		feed.title = "Updates der NetHack-De-Homepage"
		maker.channel.link = url_base_link
		atom_link = Atom::Link.new()
		atom_link['href'] = url_base_link
		feed.links << atom_link
		
		atom_link = Atom::Link.new()
		atom_link['rel'] = "self"
		atom_link['href'] = url_base_link+feed_name
		feed.links << atom_link

		maker.channel.description = "Updates der NetHack-De-Homepage"
		feed.subtitle = "Updates der NetHack-De-Homepage"
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
			atom_entry_link['href'] = url_base_link + "changes.html"
			atom_entry.links << atom_entry_link
			rss_item.link = url_base_link + "changes.html"

			rss_item.guid.isPermaLink = "false"
			rss_item.guid.content = MD5.new(rss_item.title+updateDatum).hexdigest
			atom_entry.id = "urn:uuid:"+MD5.new(rss_item.title+updateDatum).hexdigest
			feed.entries << atom_entry
		}
    
	end

	if $rss then
		puts rss.to_s
	elsif $atom then
		REXML::Document.new(feed.to_s).write($stdout,0)
	end
end

######################################################################

# <!DOCTYPE updates SYSTEM "/home/bubi/dtd/homepage_updates.dtd">
# <updates>

# <update date="02 Aug 2008" title="New: Vilistextum on the Web, Update: Funny RFCs">
#        <entry url="misc/aprilrfcs.html">added funny rfcs 5241 and 5242</entry>
#        <entry url="vilistextum/ontheweb.html">Vilistextum on the Web</entry>
# </update>

# <update date="09 Feb 2008" title="Added: Link to &#187;Carl Grunert - Die Maschine des Theodolus Energios&#171;">
#        <entry url="buchscans/weitere.html">Added: new link to &#187;Carl Grunert - Die Maschine des Theodolus Energios&#171;</entry>
# </update>

def createXmlFromNews(news)
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

######################################################################

parseargs()

if ARGV.size == 0
	usage()
elsif ARGV.size == 1 # input stdin, output stdout
	$input = File.new(ARGV[0])
	news = REXML::Document.new($input)
	#puts ARGV[0]
	#doc.write $stdout
	doc = createXmlFromNews(news)
	puts doc
	puts
	update(doc, false)
end

