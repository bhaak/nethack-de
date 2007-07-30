
# TODO
# Besonderheiten http://www.canoo.net/services/OnlineGrammar/InflectionRules/FRegeln-V/Texte/Besonderheiten.html
#
# [ ] Trennbare Verben
# [X] e-Erweiterung
# [ ] e-Tilgung
# [ ] e-Tilgung im Konjunktiv II
# [X] e/i-Wechsel
# [X] Partizip mit/ohne ge-
# [X] s-Verschmelzung
# [X] Umlaut im Präsens
# [X] Ablaut in Stammformen

class Verb
  attr_reader :infinitiv, :e_erweiterung, :partizip_perfekt
  attr_accessor :kennung, :praeverb
  
  def initialize(stamm, praeteritum_stamm="", partizip_perfekt="")
    if stamm[-2..-1] == "en" then
      @infinitiv = stamm
      stamm = stamm[0..-3]
    elsif stamm[-3..-1] == "ern" or stamm[-3..-1] == "eln"
			@infinitiv = stamm
			stamm = stamm[0..-2]
			@e_tilgung = true
    else
      @infinitiv = stamm + "en"
    end

    @praesens_stamm = stamm

    if praeteritum_stamm[-2..-1] == 'te' then
      @praeteritum_stamm = praeteritum_stamm[0..-3]
    elsif praeteritum_stamm[-1..-1] == 'e' then
      @praeteritum_stamm = praeteritum_stamm[0..-2]
    elsif praeteritum_stamm == "" then
			@praeteritum_stamm = stamm
    else
      @praeteritum_stamm = praeteritum_stamm
    end

    @e_erweiterung = e_erweiterung?(@praesens_stamm)

    if partizip_perfekt != "" then
      @partizip_perfekt = partizip_perfekt
    elsif @infinitiv =~ /....ieren$/ then
      @partizip_perfekt = stamm + 't'
    else
      @partizip_perfekt = "ge" + stamm + (@e_erweiterung ? "e" : "") + "t"
    end

		if @e_tilgung then
			@praesens_endung =    ["e", "st", "t", "n", "t", "n"]
		else
		  @praesens_endung =    ["e", "st", "t", "en", "t", "en"]
		end
  	@praeteritum_endung = ["",  "st", "",  "n", "t", "n"]
		@konjunktiv_endung =  ["",  "st", "",  "n", "t", "n"]

    @tempus  = :praesens  # praesens, praeteritum, perfekt, plusquamperfekt, futur, futur ii
    @modus   = :indikativ # indikativ, konjunktiv, imperativ
    @person  = 1          # 0, 1, 2
    @numerus = 0          # 0, 1


    return self
  end

  def e_erweiterung?(stamm)
    #puts "Stamm: "+stamm+"-"
    if stamm[-1..-1]=='t' or stamm[-1..-1]=='d' then
		  return true
    # verschlusslaut oder reibelaut + m oder n
		elsif stamm[-1..-1]=='m' or stamm[-1..-1]=='n' then
		  if stamm[-2..-2]=='f' then # TODO
			  e_erweiterung = true
			end
    else
      return false
    end
  end

  def singular
    @numerus = 0
    return self
  end

  def plural
    @numerus = 1
    return self
  end

  def indikativ
    @modus = :indikativ
    return self
  end

  def konjunktiv
    @modus = :konjunktiv
    return self
  end

  def konjunktiv_ii
    praeteritum
    @modus = :konjunktiv
    return self
  end

  def praesens
    @tempus = :praesens
    return self
  end

  def praeteritum
    @tempus = :praeteritum
    return self
  end

  def erstePerson
    @person = 0
    return self
  end

  def zweitePerson
    @person = 1
    return self
  end

  def drittePerson
    @person = 2
    return self
  end

  def personNummer
    return @person+@numerus*3
  end

  def s_verschmelzung?(stamm)
      return zweitePerson? && singular? && ['s', 'ß', 'x', 'z'].include?(stamm[-1..-1])
  end

	def form
	  e_erweiterung = @e_erweiterung ? "e" : ""
		if @tempus==:praesens && @modus==:indikativ then
      if s_verschmelzung?(@praesens_stamm) then
				return @praesens_stamm + 't'
			elsif zweitePerson? || (singular? && drittePerson?) then
				return @praesens_stamm + (@e_erweiterung ? "e" : "") + endung(@praesens_endung)
			else
				return @praesens_stamm + endung(@praesens_endung)
			end	
    elsif @tempus==:praesens && @modus==:konjunktiv then
			if @e_tilgung && ((singular? && zweitePerson?) or plural?) then
      	return @praesens_stamm + endung(@konjunktiv_endung)
			else
      	return @praesens_stamm + "e" + endung(@konjunktiv_endung)
			end
    elsif @tempus==:praeteritum 
      return @praeteritum_stamm + e_erweiterung + "te" + endung(@praeteritum_endung)
    end
  end

  def imperativ
    if singular? then
      return @praesens_stamm + (@e_erweiterung ? "e" : "") + (@e_tilgung ? "e" : "")
    else
      return @praesens_stamm + (@e_erweiterung ? "e" : "") + "t"
    end
  end
  
  def endung(endungen)
    return endungen[@person+@numerus*3]
  end

  def partizip_praesens
    return @praesens_stamm + (@e_tilgung ? "nd" : "end")
  end

  def singular?
    return @numerus == 0
  end
  def plural?
    return @numerus == 1
  end
  def erstePerson?
    return @person == 0
  end
  def zweitePerson?
    return @person == 1
  end
  def drittePerson?
    return @person == 2
  end
  def praesens?
    return @tempus==:praesens
  end
  def praeteritum?
    return @tempus==:praeteritum
  end
  def indikativ?
      return @modus==:indikativ
  end
  def konjunktiv?
    return @modus==:konjunktiv
  end

  def Verb.umlaute(stamm)
    case stamm
      when "lauf": return "läuf"
      when "aß": return "äß"
    end
      
    vokal = stamm.match(/[aou][^aeiou]/)
    umlaut = ""
    case vokal.to_s[0..0]
    when "a": umlaut = "ä"
    when "o": umlaut = "ö"
    when "u": umlaut = "ü"
    else
      return stamm
    end
    return stamm.sub(/([^aeiou])[aou]([^aeiou])/, '\1'+umlaut+'\2')
  end
end

class VerbUnregelmaessig < Verb
  attr_accessor :umlaut # http://www.canoo.net/services/OnlineGrammar/InflectionRules/FRegeln-V/Listen/Umlaut-Irreg-Pres.html
  def initialize(stamm, praeteritum_stamm=stamm, perfekt_stamm=stamm)
    super(stamm, praeteritum_stamm, perfekt_stamm)
  	@praeteritum_endung = ["",  "st", "",  "en", "t", "en"]

    @umlaut = false
    case @praesens_stamm
    when /back$/, /blas$/, /brat$/, /fahr$/, /fall$/, /fang$/, /grab$/,
      /halt$/, /lad$/, /lass$/, /lauf$/, /rat$/, /sauf$/, /schlaf$/,
      /schlag$/, /stoß$/, /trag$/, /wachs$/, /wasch$/
      @umlaut = true
    end
  end

  def form
    e_erweiterung = @e_erweiterung ? "e" : ""
    if praesens? && indikativ? && singular? && (zweitePerson? || drittePerson?) then
      if s_verschmelzung?(@praesens_stamm) then
				if umlaut then
					return Verb.umlaute(@praesens_stamm) + 't'
				else
					return @praesens_stamm + 't'
				end
      end

      if umlaut then
        return Verb.umlaute(@praesens_stamm) + e_erweiterung + endung(@praesens_endung)
      else
        return @praesens_stamm + e_erweiterung + endung(@praesens_endung)
      end
    elsif praeteritum? && indikativ? then
      if plural? && zweitePerson? then
        return @praeteritum_stamm + e_erweiterung + endung(@praeteritum_endung)
      elsif s_verschmelzung?(@praeteritum_stamm) then
        return @praeteritum_stamm + 'est'
        # return @praeteritum_stamm + 't' # auch möglich
      else
        return @praeteritum_stamm + endung(@praeteritum_endung)
      end
    elsif praeteritum? && konjunktiv? then
			return Verb.umlaute(@praeteritum_stamm) + "e" + endung(@konjunktiv_endung)
		end
    return super
  end
end

class VerbHaben < VerbUnregelmaessig
  def initialize
    super("haben", "hat", "gehabt")
  end

  def form
    if praesens? && indikativ? && singular? then
      if zweitePerson? then
        return "hast"
      elsif drittePerson? then
        return "hat"
      end
    elsif praeteritum? && indikativ? then
      return ["hatte", "hattest", "hatte", "hatten", "hattet", "hatten"][personNummer]
    elsif praeteritum? && konjunktiv? then
      return ["hätte", "hättest", "hätte", "hätten", "hättet", "hätten"][personNummer]
    end
    return super
  end

end

class VerbSein < VerbUnregelmaessig
  def initialize()
    super("sein", "war", "gewesen")
    @infinitiv = "sein"
    @praesens_stamm = "sei"
  end

  def form
    if praesens? && indikativ? then
      return ["bin", "bist", "ist", "sind", "seid", "sind"][personNummer]
    elsif praesens? && konjunktiv? then
      if singular? and (erstePerson? or drittePerson?) then
        return "sei"
      end
    elsif praeteritum? && indikativ? then
      return ["war", "warst", "war", "waren", "wart", "waren"][personNummer]
    end
    return super
  end
end

class VerbWerden < VerbUnregelmaessig
  def initialize()
    super("werden", "wurde", "geworden")
  end

  def form
    if praesens? && indikativ? then
      return ["werde", "wirst", "wird", "werden", "werdet", "werden"][personNummer]
    elsif praeteritum? && indikativ? then
      return ["wurde", "wurdest", "wurde", "wurden", "wurdet", "wurden"][personNummer]
    end
    return super
  end
end

class VerbModal < Verb
  def form
    if praesens? && indikativ? && singular? then
      case infinitiv
      when "können": return ["kann", "kannst", "kann"][personNummer]
      when "wollen": return ["will", "willst", "will"][personNummer]
      else
        return super
      end
    elsif praeteritum? && konjunktiv? then
      return Verb.umlaute(@praeteritum_stamm) + "te" + endung(@konjunktiv_endung)
    end
    return super
  end

  def imperativ
    return ""
  end
end

class VerbSchwachUnregelmaessig < Verb
  def form
    if praeteritum? && konjunktiv? then
			case infinitiv
			when /brennen$/, "kennen"
      	return (@praesens_stamm) + "te" + endung(@konjunktiv_endung)
			end
    	return Verb.umlaute(@praeteritum_stamm) + "te" + endung(@konjunktiv_endung)
    end
    return super
  end
end

class Verb_EI_Wechsel < VerbUnregelmaessig

  def initialize(stamm, praeteritum_stamm, perfekt_stamm, ei_wechsel_stamm)
    super(stamm, praeteritum_stamm, perfekt_stamm)
    @ei_wechsel_stamm = ei_wechsel_stamm
  end

  def form
    if praesens? && indikativ? && singular? && zweitePerson? then
			return @ei_wechsel_stamm + (s_verschmelzung?(@ei_wechsel_stamm) ? "t" : "st")
    elsif praesens? && indikativ? && singular? && drittePerson? then
      return @ei_wechsel_stamm + (@ei_wechsel_stamm[-1..-1]=='t' ? "" : "t")
    end
    return super
  end

  def imperativ
    if singular? then
      return @ei_wechsel_stamm
    end
    super
  end
end

class Verb_Konjunktiv_II < Verb_EI_Wechsel

  def initialize(stamm, praeteritum_stamm, perfekt_stamm, ei_wechsel_stamm, konjunktiv_ii_stamm)
    super(stamm, praeteritum_stamm, perfekt_stamm, ei_wechsel_stamm)
    @konjunktiv_ii_stamm = konjunktiv_ii_stamm
  end

  def form
    if praeteritum? && konjunktiv? then
				return @konjunktiv_ii_stamm + "e" + endung(@konjunktiv_endung)
    end
    return super
  end

end

def Verb.verb(kennung, infinitiv, praeverb="")
  if kennung=="" then
    kennung = "VERB_"+infinitiv.upcase
  end
  v = nil

  case infinitiv
  when "werden":   v = VerbWerden.new
  when "sein":     v = VerbSein.new
  when "haben":    v = VerbHaben.new
  when "denken":   v = VerbSchwachUnregelmaessig.new("denken", "dachte", "gedacht")
  when "gehen":    v = VerbUnregelmaessig.new("gehen", "ging", "gegangen")
  when "heißen":   v = VerbUnregelmaessig.new("heißen", "hieß", "geheißen")
  when "scheinen": v = VerbUnregelmaessig.new("scheinen", "schien", "geschienen")
	# mit Rueckumlaut
  when /(.*)brennen$/:  v = VerbSchwachUnregelmaessig.new($1+"brennen", $1+"brannte", ge($1)+"brannt")
    # e/i-Wechsel
  when "nehmen":  v = Verb_EI_Wechsel.new("nehmen", "nahm", "genommen", "nimm")
  when "treten":  v = Verb_EI_Wechsel.new("treten", "trat", "getreten", "tritt")
  when "treffen": v = Verb_EI_Wechsel.new("treffen", "traf", "getroffen", "triff")
  when "sehen":   v = Verb_EI_Wechsel.new("sehen", "sah", "gesehen", "sieh")
  when /(.*)brechen$/: v = Verb_EI_Wechsel.new($1+"brechen", $1+"brach", ge($1)+"brochen", $1+"brich")
  when "essen":   v = Verb_EI_Wechsel.new("essen", "aß", "gegessen", "iss")
    # Modalverben
  when "können": v = VerbModal.new("können", "konnte", "gekonnt")
  when "wollen": v = VerbModal.new("wollen", "wollte", "gewollt")
    #  i a u
  when /(.*)springen$/: v = VerbUnregelmaessig.new($1+"springen", $1+"sprang", ge($1)+"sprungen")
  when "finden": v = VerbUnregelmaessig.new("finden", "fand", "gefunden")
  when "wringen": v = VerbUnregelmaessig.new("wringen", "wrang", "gewrungen")
  when "trinken": v = VerbUnregelmaessig.new("trinken", "trank", "getrunken")
  when /(.*)schwinden$/: v = VerbUnregelmaessig.new($1+"schwinden", $1+"schwand", ge($1)+"schwunden")
		#  e a o
  when "werfen": v = Verb_Konjunktiv_II.new("werfen", "warf", "geworfen", "wirf", "würf")
  when "sterben": v = Verb_Konjunktiv_II.new("sterben", "starb", "gestobren", "stirb", "stürb")
  when "nehmen": v = Verb_EI_Wechsel.new("nehmen", "nahm", "genommen", "nimm")
  when "sprechen": v = Verb_EI_Wechsel.new("sprechen", "sprach", "gesprochen", "sprich")
  when "stechen": v = Verb_EI_Wechsel.new("stechen", "stach", "gestochen", "sticht")
  when "stehlen": v = Verb_Konjunktiv_II.new("stehlen", "stahl", "gestohlen", "stiehl", "stöhl")
		#  e a o
  when "stehen": v = Verb_Konjunktiv_II.new("stehen", "stand", "gestanden", "steh", "stünd")
    #  ei i i
  when /(.*)gleiten$/:  v = VerbUnregelmaessig.new($1+"gleiten", $1+"glitt", ge($1)+"glitten")
  when /(.*)reißen$/:  v = VerbUnregelmaessig.new($1+"reißen", $1+"riss", ge($1)+"rissen")
    #  a u a
  when "tragen": v = VerbUnregelmaessig.new("tragen", "trug", "getragen")
  when "graben": v = VerbUnregelmaessig.new("graben", "grub", "gegraben")
  when "schlagen": v = VerbUnregelmaessig.new("schlagen", "schlug", "geschlagen")
    #  au ie au
  when "laufen": v = VerbUnregelmaessig.new("laufen", "lief", "gelaufen")
  when "hauen":  v = VerbUnregelmaessig.new("hauen", "hieb", "gehauen")
    # a i a
  when "fangen": v = VerbUnregelmaessig.new("fangen", "fing", "gefangen")
  when "empfangen": v = VerbUnregelmaessig.new("empfangen", "empfing", "empfangen")
    # a ie a
  when "lassen": v = VerbUnregelmaessig.new("lassen", "ließ", "gelassen")
  when /(.*)fallen$/: v = VerbUnregelmaessig.new($1+"fallen", $1+"fiel", ge($1)+"fallen")
		# ie o o
  when "fliegen": v = VerbUnregelmaessig.new("fliegen", "flog", "geflogen")
  when "wiegen": v = VerbUnregelmaessig.new("wiegen", "wog", "gewogen")
  else
    # regelmaessige Verben
    v = Verb.new(infinitiv)
  end
    
  v.praeverb = praeverb
  v.kennung = kennung
  v
end

def ge(vorsilbe)
	return  (vorsilbe == "" ? "ge" : vorsilbe)
end
  
