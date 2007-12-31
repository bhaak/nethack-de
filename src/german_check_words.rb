#!/usr/bin/ruby

$htmloutput = true

file1 = File.readlines("mon-de.h")
file2 = File.readlines("obj-de.h")

# lines = file1.concat file2

def printWords(file, lines, htmlOutput=false) 
translated = []
File.open("german_vocabulary.c") { |german| 
  translated = german.readlines.collect { |l|
    if l =~ /\{"[^"]*", ("[A-Z0-9_]+")/ then
      $1
    end
  }.uniq
}

count = 0
lines.each { |line|
  if line =~ /\#define.*NAM_.*(".*")/ then
    if not translated.include? $1 then
      puts $1+' nicht übersetzt!' unless htmlOutput
      count += 1
    end
  end
}

if htmlOutput then
  puts '<tr>'
  puts '  <td>'+file+'</td>'
  puts '  <td align="right">'+lines.size.to_s+'</td>'
  puts '  <td align="right">'+count.to_s+'</td>'
  puts '  <td align="right">'+sprintf("%4d", lines.size-count)+'</td>'
  puts '  <td align="right">'+sprintf("%2g", (lines.size-count)*100.0/lines.size)+'</td>'
  puts '</tr>'
else
  puts
  puts "Gesamtzahl:            "+sprintf("%4d", lines.size.to_s)
  puts "Nicht übersetzte Wort: "+sprintf("%4d", count.to_s)
  puts "Übersetzte Worte:      "+sprintf("%4d", lines.size-count)
  puts "Prozentual übersetzt:  "+sprintf("%2g", (lines.size-count)*100.0/lines.size)
end

end

def printHeader
  puts "<tr>"
  puts "  <th>file</th>"
  puts "  <th>to translate<br/>(words)</th>"
  puts "  <th>not translated<br/>(words)</th>"
  puts "  <th>translated<br/>(words)</th>"
  puts "  <th>translated<br/>(percent)</th>"
  puts "</tr>"
end

puts "<table>" if $htmloutput
printHeader if $htmloutput
printWords("mon.h",file1, $htmloutput)
printWords("obj.h",file2, $htmloutput)
printWords("total",file1.concat(file2), $htmloutput)
puts "</table>" if $htmloutput
