file1 = File.readlines("mon-de.h")
file2 = File.readlines("obj-de.h")

lines = file1.concat file2

translated = []
File.open("german_vocabulary.c") { |german| 
  translated = german.readlines.collect { |l|
    if l =~ /\{"[^"]*", "([A-Z0-9_]+)"/ then
      $1
    end
  }.uniq
}


t = []
files = []
if ARGV.length == 0 then
	files = %w{allmain.c alloc.c apply.c artifact.c attrib.c ball.c bones.c botl.c cmd.c dbridge.c decl.c detect.c dig.c display.c dlb.c do.c do_name.c do_wear.c dog.c dogmove.c dokick.c dothrow.c drawing.c dungeon.c eat.c end.c engrave.c exper.c explode.c extralev.c files.c fountain.c hack.c hacklib.c invent.c light.c lock.c mail.c makemon.c mapglyph.c mcastu.c mhitm.c mhitu.c minion.c mklev.c mkmap.c mkmaze.c mkobj.c mkroom.c mon.c mondata.c monmove.c monst.c monstr.c mplayer.c mthrowu.c muse.c music.c o_init.c objects.c objnam.c options.c pager.c pickup.c pline.c polyself.c potion.c pray.c priest.c quest.c questpgr.c read.c rect.c region.c restore.c rip.c rnd.c role.c rumors.c save.c shk.c shknam.c sit.c sounds.c sp_lev.c spell.c steal.c steed.c teleport.c timeout.c topten.c track.c trap.c u_init.c uhitm.c vault.c version.c vis_tab.c vision.c weapon.c were.c wield.c windows.c wizard.c worm.c worn.c write.c zap.c}
else
	files = ARGV
end

files.each { |f|
  File.open(f) { |file| 
    tmp = file.readlines.collect {|l|
      l.scan(/"[^"]*[A-Z][A-Z0-9_]+[^"]*"/)
    }
    t << tmp.flatten.collect {|l|
      l.scan(/[A-Z][A-Z0-9_]+/)
    }
  }
}

t = t.flatten.reject {|e|
  e.match(/^(ABCDEF|ABCDEFGHIJKLMNOPQRSTUVWXYZ|AC|AEIOU|AEXE|AIR|ALIGN_TOP|ALTAR|ARTICLE_A|ARTICLE_NONE|ARTICLE_THE|ARTICLE_YOUR|AT|AT386|A_CON|A_DEX|A_STR|A_WIS|BGPENS|BIOS|BLCORNER|BONESDIR|BOOM|BOULDER|BRCORNER|BSTRCMP|BSTRCMPI|CANNOT|CATNAME|CATS|CHARACTER|CLOUD|CONFIGDIR|CONFIGPREFIX|CORR|CRASH|CROSSWALL|DATADIR|DBWALL|DEC|DEFINE|DEPTH|DOGNAME|DOOR|DOORMAX|DOS|DRAWBRIDGE_DOWN|DRAWBRIDGE_UP|DRIPENS|DUNGEON|EFFECTS|EFGIOVW|EN|ENGINE|EOF|E_|FALSE|FGPENS|FILE|FL|FONT|FOUNTAIN|GEM_CLASS|GRAPHICS|HACKDIR|HACKOPTIONS|HD|HJKL|HOME|HP|HUP|HWALL|IBM|IBM_|ICE|II|IMMEDIATE|IN|IS_ROCK|KAABLAMM|KABOOM|KADOOM|LADDER|LAST_GEM|LAVAPOOL|LEVELDIR|LEVELS|LOCKDIR|LOCKS|LOOO|LRS|MAIL|MAILREADER|MAXRECT|MAZEWALK|MENUCOLOR|MENUPENS|MESG|MOAT|MONSTERS|MSGPENS|NAME|NEST|NETHACKOPTIONS|NH_HEAPLOG|NICHT|NO|OBJECT|OBJECTS|OFF|OK|ON|OPEN|OPTIONS|OTHERPENS|PARTIKEL_OF|PATH|PEACE|PENS|PF|PID|POOL|POT_ACID|PREFIX|PREMATURE|QT_COMPACT|QT_FONTSIZE|QT_TILEHEIGHT|QT_TILEWIDTH|QUEST|QWERTY|RAMDISK|REST|ROGUEOPTS|ROLE|ROOM|SATZKLAMMER|SAVE|SAVEDIR|SCOREDIR|SCORR|SCREENMODE|SDOOR|SHOPTYPE|SINK|SO|SOUND|SOUNDDIR|SPAWN|SPLASH|SPLEVTYPE|STAIRS|STATUSPENS|STONE|TDWALL|TERM|TEXT|TEXTPENS|THIS|THRONE|THUD|TLCORNER|TLWALL|TM|TO|TONE|TRAPS|TRCORNER|TRIED|TROUBLEDIR|TRWALL|TT_LAVA|TUWALL|VWALL|WARNINGS|WATER|WEB|WHAMMM|WIZKIT|X11|Y2K|ZOT|KASUS_(NOMINATIV|GENITIV|DATIV|AKKUSATIV)|MODIFIER_CORPSE|NOUN_|NOUN_RING_UNIDENTIFIED_|NUMERUS_(SINGULAR|PLURAL)|SUBJECT(|_IM_SATZ)|VERB_|NEUER_SATZ|PM_)$/)
}

#puts "Anzahl ", (t.flatten - translated).uniq.sort
puts (t.flatten - translated).sort.uniq
puts "Anzahl ", t.flatten.uniq.length
puts "Anzahl ", (t.flatten - translated).uniq.length
