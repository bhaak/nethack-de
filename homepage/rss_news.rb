#!/usr/bin/ruby

# Copyright (C) 2004 Patric Müller
# bhaak@gmx.net
#
# Released under the GNU GPL - http://www.gnu.org/copyleft/gpl.html

require 'parsearg.rb'

require "rexml/document"
require "rexml/xpath"

require 'genericoutput/htmloutput.rb'
require 'genericoutput/latexoutput.rb'

require 'date'
require 'md5'

require 'rss/maker'
require 'rss/dublincore'

require "atom/entry"
require "atom/feed"
require "atom/element"

######################################################################

def usage()
  printf "%s [--rss|--atom] [-m] [-o output] [--version] absolute_url files\n", $0
end

######################################################################

def parseargs()
	$USAGE = 'usage'

	parseArgs(0, false, "m", "output:", "version", "rss", "atom")

	if ($OPT_version)
    printf "update version 0.7 (14.10.2006)\n"
    exit(0)
  end
	if ($OPT_rss)
		$rss = true
	end
	if ($OPT_atom)
		$atom = true
	end
	if ($OPT_m)
		$modify = true
	end

end

######################################################################

def update(document, rss)
	url_base_link = "http://bhaak.dyndns.org/"

	feed = Atom::Feed.new()
	feed_name = "atom.xml"

  html = []
  rss = RSS::Maker.make("2.0") do |maker|
    #rss = RSS::Maker.make("0.9")
    maker.channel.title = "bhaak.dyndns.org homepage updates"
		feed.title = "bhaak.dyndns.org homepage updates"
    maker.channel.link = url_base_link
		atom_link = Atom::Link.new()
		atom_link['href'] = url_base_link
    feed.links << atom_link
		
		atom_link = Atom::Link.new()
		atom_link['rel'] = "self"
		atom_link['href'] = url_base_link+feed_name
    feed.links << atom_link

    maker.channel.description = "Updates of bhaak.dyndns.org"
    feed.subtitle = "Updates of bhaak.dyndns.org"
		feed.updated = DateTime.now.to_s
		feed.id = url_base_link

		atom_author = Atom::Author.new()
		atom_author.name = "Patric Mueller"
		atom_author.email = "bhaak@gmx.net"
		#atom_author.uri = url_base_link
		feed.authors << atom_author

    output = GOUT::Html.new

    hrefattributes = REXML::XPath.match(document, "//update");
    output.set_title("Homepage update")
    html << output.begin_document << "\n"

    hrefattributes.sort{|a,b| Date.parse(a.attributes['date']) <=> Date.parse(b.attributes['date'])}.reverse.each { |element| 
			updateDatum = DateTime.parse(element.attributes['date']).to_s
      $stderr.puts updateDatum

      html << output.begin_paragraph("update") << element.attributes['date'] << output.end_paragraph 

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

      description = output.begin_list + "\n"
      element.each_element { | entry|
        description += output.begin_item +
											 output.link(entry.get_text.to_s, url_base_link + entry.attributes['url']) +
											 output.end_item + "\n"
      }
      description += output.end_list + "\n"

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
      html << description
    }
    
    html << output.end_document
  end

	if $rss then
		puts rss.to_s
	elsif $atom then
		REXML::Document.new(feed.to_s).write($stdout,0)
	else
		print html, "\n"
	end
end

######################################################################

$modify = false

parseargs()

if ARGV.size == 0
	usage()
elsif ARGV.size == 1 # input stdin, output stdout
	$input = File.new(ARGV[0])
	doc = REXML::Document.new($input)
	#puts ARGV[0]
	#doc.write $stdout
	update(doc, false)
end

