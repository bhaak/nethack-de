file1 = File.readlines("mon-de.h")
file2 = File.readlines("obj-de.h")

lines = file1.concat file2

translated = []
File.open("german.h") { |german| 
  translated = german.readlines.collect { |l|
    if l =~ /\{"[^ ]*", ("[A-Z_]+")/ then
      $1
    end
  }.uniq
}

count = 0
lines.each { |line|
  if line =~ /\#define.*NAM_.*(".*")/ then
    if not translated.include? $1 then
      puts $1+' nicht übersetzt!'
      count += 1
    end
  end
}

puts
puts "Gesamtzahl:            "+sprintf("%4d", lines.size.to_s)
puts "Nicht übersetzte Wort: "+sprintf("%4d", count.to_s)
puts "Übersetzte Worte:      "+sprintf("%4d", lines.size-count)
puts "Prozentual übersetzt:  "+sprintf("%2g", (lines.size-count)*100.0/lines.size)
