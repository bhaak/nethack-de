#!/usr/bin/ruby

require 'date'

def ausgabe(en, de, prozent)

	if (prozent.to_i >= 100) then
	  puts '<td class="vollstaendig" colspan="10"></td>'
	elsif (prozent.to_i == 0) then
	  puts '<td class="leer" colspan="10"></td>'
	else
	  if (prozent.to_i / 10 > 0) then
		  puts '<td class="teilweise" colspan="'+(prozent.to_i / 10).to_s+'"></td>'
		end
		puts '<td class="leer" colspan="'+(10 -(prozent.to_i / 10)).to_s+'"></td>'
	end
end

puts '<style type="text/css">'
puts "  .nethack table { border-collapse: collapse }"
puts "  .nethack th, .nethack td {  padding: 0.2em 0.5em 0.2em 0.5em;"
puts "                              border: 1px solid black }"
puts "  .nethack th.balken { border-left: 1px solid white; border-right: 1px solid white; }"
puts "  .nethack th.balken:last-child { border-right: 1px solid black; }"
puts "  .nethack td.leer { background-color: #CCC; }"
puts "  .nethack td.teilweise { background-color: orange; }"
puts "  .nethack td.vollstaendig { background-color: green; }"
puts "  .nethack th.balken { border: 1px solid black; }"
puts "</style>"

en = 0
de = 0
i = 0
files = Dir.glob("*.c")
puts '<div class="nethack">'
puts '<table border="0">'
puts '  <tr>'
puts '    <th>filename</th>'
puts '    <th title="lines of text">en</th>'
puts '    <th title="lines of text not yet translated">todo</th>'
puts '    <th title="already translated (in percent)">%</th>'
10.times { puts '    <th class="balken"></th>' }
puts '  </tr>'
files.sort.each {|c|
	en_cnt = IO.popen('grep \ EN\  '+c+' | wc -l').readlines.join.strip
	de_cnt = IO.popen('grep TODO\ DE '+c+' | wc -l').readlines.join.strip
	if en_cnt.to_i > 0 then
					#print c +" "+en_cnt+" "+de_cnt+" "#+(de_cnt.to_i/en_cnt.to_i)
					puts '<tr>'
					puts '<td align="left">'+c+'</td>'
					puts '<td align="right">'+en_cnt+'</td>'
					puts '<td align="right">'+de_cnt+'</td>'
					if de_cnt.to_i > en_cnt.to_i then
						de_cnt = en_cnt
					end
					prozent = sprintf("%.2f", ((en_cnt.to_i-de_cnt.to_i)*100.00/en_cnt.to_i))
					#puts prozent
					puts '<td align="right">'+prozent+'</td>'
					ausgabe(en_cnt, de_cnt, prozent)
					puts '</tr>'
					en += en_cnt.to_i
					de += de_cnt.to_i
					i += 1
	end
}

#puts "total files: "+files.size.to_s
#puts "en: "+en.to_s
#puts "de: "+de.to_s
#puts "de: "+(en-de).to_s
#puts "percent: "+sprintf("%.2f", ((en-de)*100.00/en))
percent = sprintf("%.2f", ((en-de)*100.00/en))
puts '<tfoot>'
puts '<tr>'
puts '<td align="right">'+'total: '+files.size.to_s+'</td>'
puts '<td align="right">'+en.to_s+'</td>'
puts '<td align="right">'+de.to_s+'</td>'
puts '<td align="right">'+percent+'</td>'
ausgabe(en, de, percent)
puts '</tr>'
puts '</tfoot>'
puts '</table>'
puts '</div>'
puts '<p style="text-align: right">Last updated: '+Date.today.to_s+'</p>'
