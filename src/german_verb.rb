#!/usr/bin/env ruby
# coding: iso-8859-1

# Besonderheiten http://www.canoo.net/services/OnlineGrammar/InflectionRules/FRegeln-V/Texte/Besonderheiten.html

class Verb
  attr_reader :infinitiv, :e_erweiterung, :partizip_perfekt
  attr_accessor :kennung, :praeverb
  attr_accessor :umlaut # http://www.canoo.net/services/OnlineGrammar/InflectionRules/FRegeln-V/Listen/Umlaut-Irreg-Pres.html

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
    elsif praeteritum_stamm[-1..-1] == 'e' && praeteritum_stamm[-2..-1] != 'ie' then
      @praeteritum_stamm = praeteritum_stamm[0..-2]
    elsif praeteritum_stamm == "" then
      @praeteritum_stamm = stamm
    else
      @praeteritum_stamm = praeteritum_stamm
    end

    @e_erweiterung = e_erweiterung?(@praesens_stamm)

    if partizip_perfekt != "" then
      @partizip_perfekt = partizip_perfekt
    elsif @infinitiv =~ /...ieren$/ then
      @partizip_perfekt = stamm + 't'
    elsif stamm =~  /^(be|er|ent|ge|ver|zer|wider)(....*)/ then
      @partizip_perfekt = $1 + $2 + (@e_erweiterung ? "e" : "") + 't'
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

    @umlaut = false

    return self
  end

  def e_erweiterung?(stamm)
    #puts "Stamm: "+stamm+"-"
    if stamm[-1..-1]=='t' or stamm[-1..-1]=='d' then
      return true
      # verschlusslaut oder reibelaut + m oder n
    elsif stamm[-1..-1]=='m' or stamm[-1..-1]=='n' then
      if ['tm','fn','pn'].include?(stamm[-2..-1]) then # TODO
        @e_erweiterung = true
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
    return zweitePerson? && singular? && ['s', '�', 'x', 'z'].include?(stamm[-1..-1])
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
    elsif @tempus==:praeteritum then
      return @praeteritum_stamm + e_erweiterung + "te" + endung(@praeteritum_endung)
    end
  end

  def infinitiv
    return @praeverb+@infinitiv
  end

  def infinitiv_zu
    if @praeverb != ''
      "#{@praeverb}zu#{@infinitiv}"
    else
      "zu #{@infinitiv}"
    end
  end

  def imperativ
    case infinitiv
    when "w�hlen","benutzen"
      return @praesens_stamm+"e"
    end
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
    return @praeverb+@praesens_stamm + (@e_tilgung ? "nd" : "end")
  end

  def partizip_perfekt
    return @praeverb+@partizip_perfekt
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
    when "lauf" then return "l�uf"
    when "a�" then return "��"
    end

    vokal = stamm.match(/[aou][^aeiou]/)
    umlaut = ""
    case vokal.to_s[0..0]
    when "a" then umlaut = "�"
    when "o" then umlaut = "�"
    when "u" then umlaut = "�"
    else
      return stamm
    end
    return stamm.sub(/([^aeiou])[aou]([^aeiou])/, '\1'+umlaut+'\2')
  end
end

class VerbUnregelmaessig < Verb

  def initialize(stamm, praeteritum_stamm=stamm, perfekt_stamm=stamm)
    super(stamm, praeteritum_stamm, perfekt_stamm)
    @praeteritum_endung = ["",  "st", "",  "en", "t", "en"]

    case @praesens_stamm
    when /back$/, /blas$/, /brat$/, /fahr$/, /fall$/, /fang$/, /grab$/,
      /halt$/, /lad$/, /lass$/, /lauf$/, /rat$/, /sauf$/, /schlaf$/,
      /schlag$/, /sto�$/, /trag$/, /wachs$/, /wasch$/
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
        # http://www.canoo.net/services/OnlineGrammar/InflectionRules/FRegeln-V/Listen/e-Erweiterung-except.html
        if (zweitePerson?) then 
          return Verb.umlaute(@praesens_stamm) + endung(@praesens_endung)
        else
          return Verb.umlaute(@praesens_stamm) + (@praesens_stamm[-1..-1] == 't' ? "" : endung(@praesens_endung))
        end
      else
        return @praesens_stamm + e_erweiterung + endung(@praesens_endung)
      end
    elsif praeteritum? && indikativ? then
      if plural? && zweitePerson? then
        return @praeteritum_stamm + e_erweiterung + endung(@praeteritum_endung)
      elsif (plural? && (erstePerson? || drittePerson?)) &&  # Paragraph 19 der Rechtschreibreform
        (@praeteritum_stamm[-2..-1] == "ie" || @praeteritum_stamm[-2..-1] == "ee") then
        return @praeteritum_stamm[0..-2] + endung(@praeteritum_endung)
      elsif s_verschmelzung?(@praeteritum_stamm) then
        return @praeteritum_stamm + 'est'
        # return @praeteritum_stamm + 't' # auch m�glich
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
      return ["h�tte", "h�ttest", "h�tte", "h�tten", "h�ttet", "h�tten"][personNummer]
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

class VerbTun < VerbUnregelmaessig
  def initialize()
    super("tun", "tat", "getan")
    @infinitiv = "tun"
  end

  def imperativ
    if singular? then
      return "tu"
    else
      return "tut"
    end
  end

  def partizip_praesens
    return "tuend"
  end

  def form
    if praesens? && indikativ? then
      return ["tue", "tust", "tut", "tun", "tut", "tun"][personNummer]
    elsif praesens? && konjunktiv? then
      return ["tue", "tuest", "tue", "tuen", "tuet", "tuen"][personNummer]
    elsif praeteritum? && indikativ? then
      return ["tat", "tatest", "tat", "taten", "tatet", "taten"][personNummer]
    end
    return super
  end
end

class VerbModal < Verb
  def form
    if praesens? && indikativ? && singular? then
      case infinitiv
      when "k�nnen" then return ["kann", "kannst", "kann"][personNummer]
      when "wollen" then return ["will", "willst", "will"][personNummer]
      when "wissen" then return ["wei�", "wei�t", "wei�"][personNummer]
      when "m�gen" then return ["mag", "magst", "mag"][personNummer]
      when "m�ssen" then return ["muss", "musst", "muss"][personNummer]
      when "sollen" then return ["soll", "sollst", "soll"][personNummer]
      when "d�rfen" then return ["darf", "darfst", "darf"][personNummer]
      else
        return super
      end
    elsif praeteritum? && konjunktiv? then
      case infinitiv
      when "sollen", "wollen"
        return @praeteritum_stamm + "te" + endung(@konjunktiv_endung)
      else
        return Verb.umlaute(@praeteritum_stamm) + "te" + endung(@konjunktiv_endung)
      end
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
      when /brennen$/, "kennen", /nennen$/,
        /rennen$/, /senden$/, /wenden$/ # gemischte Flexion
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
    # regelm�ssige Verben
  when /(.*)blassen$/ then v = Verb.new($1+"blassen", $1+"blasste", ge($1)+"blasst")
  when /(.*)breiten$/ then v = Verb.new($1+"breiten", $1+"breite", ge($1)+"breitet")
    # unregelm�ssige Verben
  when "werden" then   v = VerbWerden.new
  when "sein" then     v = VerbSein.new
  when "haben" then    v = VerbHaben.new
  when "tun" then      v = VerbTun.new
  when "denken" then   v = VerbSchwachUnregelmaessig.new("denken", "dachte", "gedacht")
  when "gehen" then    v = VerbUnregelmaessig.new("gehen", "ging", "gegangen")
  when "hei�en" then   v = VerbUnregelmaessig.new("hei�en", "hie�", "gehei�en")
  when "scheinen" then v = VerbUnregelmaessig.new("scheinen", "schien", "geschienen")
    # mit Rueckumlaut
  when /(.*)brennen$/ then  v = VerbSchwachUnregelmaessig.new($1+"brennen", $1+"brannte", ge($1)+"brannt")
    # regelm�ssig
  when /(.*)nennen$/ then  v = VerbSchwachUnregelmaessig.new($1+"nennen", $1+"nannte", ge($1)+"nannt")
    # e/i-Wechsel
  when "nehmen" then  v = Verb_EI_Wechsel.new("nehmen", "nahm", "genommen", "nimm")
  when /(.*)treten/ then  v = Verb_EI_Wechsel.new($1+"treten", $1+"trat", ge($1)+"treten", $1+"tritt")
  when "treffen" then v = Verb_EI_Wechsel.new("treffen", "traf", "getroffen", "triff")
  when "sehen" then   v = Verb_EI_Wechsel.new("sehen", "sah", "gesehen", "sieh")
  when /(.*)brechen$/ then v = Verb_EI_Wechsel.new($1+"brechen", $1+"brach", ge($1)+"brochen", $1+"brich")
  when "essen" then   v = Verb_EI_Wechsel.new("essen", "a�", "gegessen", "iss")
    # Modalverben
  when "k�nnen" then v = VerbModal.new("k�nnen", "konnte", "gekonnt")
  when "wollen" then v = VerbModal.new("wollen", "wollte", "gewollt")
  when "wissen" then v = VerbModal.new("wissen", "wusste", "gewusst")
  when "m�gen" then v = VerbModal.new("m�gen", "mochte", "gemocht")
  when "m�ssen" then v = VerbModal.new("m�ssen", "musste", "gemusst")
  when "sollen" then v = VerbModal.new("sollen", "sollte", "gesollt")
  when "d�rfen" then v = VerbModal.new("d�rfen", "durfte", "gedurft")
    #  e i a
  when /(.*)gehen$/ then v = VerbUnregelmaessig.new($1+"gehen", $1+"ging", ge($1)+"gangen")
    #  e a e
  when /(.*)fressen$/ then v = Verb_EI_Wechsel.new($1+"fressen", $1+"fra�", ge($1)+"fressen", $1+"friss")
  when /(.*)geben$/ then v = Verb_Konjunktiv_II.new($1+"geben", $1+"gab", ge($1)+"geben", $1+"gib", $1+"g�b")
  when /(.*)lesen$/ then v = Verb_EI_Wechsel.new($1+"lesen", $1+"las", ge($1)+"lesen", $1+"lies")
  when /(.*)treten$/ then v = Verb_Konjunktiv_II.new($1+"treten", $1+"trat", ge($1)+"treten", $1+"tritt", $1+"tr�t")
    #  i a a
  when /(.*)bringen$/ then v = VerbSchwachUnregelmaessig.new($1+"bringen", $1+"brachte", ge($1)+"bracht")
    #  i a u
  when /(.*)dringen$/ then v = VerbUnregelmaessig.new($1+"dringen", $1+"drang", ge($1)+"drungen")
  when /(.*)springen$/ then v = VerbUnregelmaessig.new($1+"springen", $1+"sprang", ge($1)+"sprungen")
  when /(.*)finden$/ then v = VerbUnregelmaessig.new($1+"finden", $1+"fand", ge($1)+"funden")
  when "wringen" then v = VerbUnregelmaessig.new("wringen", "wrang", "gewrungen")
  when /(.*)schlingen$/ then v = VerbUnregelmaessig.new($1+"schlingen", $1+"schlang", ge($1)+"schlungen")
  when /(.*)schwinden$/ then v = VerbUnregelmaessig.new($1+"schwinden", $1+"schwand", ge($1)+"schwunden")
  when /(.*)schwingen$/ then v = VerbUnregelmaessig.new($1+"schwingen", $1+"schwang", ge($1)+"schwungen")
  when /(.*)singen$/ then v = VerbUnregelmaessig.new($1+"singen", $1+"sang", ge($1)+"sungen")
  when /(.*)sinken/ then v = VerbUnregelmaessig.new($1+"sinken", $1+"sank", ge($1)+"sunken")
  when /(.*)trinken/ then v = VerbUnregelmaessig.new($1+"trinken", $1+"trank", ge($1)+"trunken")
  when /(.*)winden/ then v = VerbUnregelmaessig.new($1+"winden", $1+"wand", ge($1)+"wunden")
    #  e a o
  when "helfen" then v = Verb_Konjunktiv_II.new("helfen", "half", "geholfen", "hilf", "h�lf")
  when "werfen" then v = Verb_Konjunktiv_II.new("werfen", "warf", "geworfen", "wirf", "w�rf")
  when /(.*)sterben$/ then v = Verb_Konjunktiv_II.new($1+"sterben", $1+"starb", ge($1)+"storben", $1+"stirb", $1+"st�rb")
  when "nehmen" then v = Verb_EI_Wechsel.new("nehmen", "nahm", "genommen", "nimm")
  when "sprechen" then v = Verb_EI_Wechsel.new("sprechen", "sprach", "gesprochen", "sprich")
  when /(.*)stechen$/ then v = Verb_EI_Wechsel.new($1+"stechen", $1+"stach", ge($1)+"stochen", $1+"stich")
  when "stehlen" then v = Verb_Konjunktiv_II.new("stehlen", "stahl", "gestohlen", "stiehl", "st�hl")
    #  e a a
  when /(.*)stehen/ then v = Verb_Konjunktiv_II.new($1+"stehen", $1+"stand", ge($1)+"standen", $1+"steh", $1+"st�nd")
    #  e o o
  when "bewegen" then v = VerbUnregelmaessig.new("bewegen", "bewog", "bewogen")
  when /(.*)heben$/ then  v = VerbUnregelmaessig.new($1+"heben", $1+"hob", ge($1)+"hoben")
  when /(.*)quellen$/ then v = Verb_Konjunktiv_II.new($1+"quellen", $1+"quoll", ge($1)+"quollen", $1+"quill", $1+"qu�ll")
  when /(.*)schmelzen$/ then v = Verb_Konjunktiv_II.new($1+"schmelzen", $1+"schmolz", ge($1)+"schmolzen", $1+"schmilz", $1+"schm�lz")
    #  ei i i
  when /(.*)bei�en$/ then  v = VerbUnregelmaessig.new($1+"bei�en", $1+"biss", ge($1)+"bissen")
  when /(.*)gleiten$/ then  v = VerbUnregelmaessig.new($1+"gleiten", $1+"glitt", ge($1)+"glitten")
  when /(.*)rei�en$/ then  v = VerbUnregelmaessig.new($1+"rei�en", $1+"riss", ge($1)+"rissen")
  when /(.*)weichen$/ then  v = VerbUnregelmaessig.new($1+"weichen", $1+"wich", ge($1)+"wichen")
  when /(.*)greifen$/ then  v = VerbUnregelmaessig.new($1+"greifen", $1+"griff", ge($1)+"griffen")
  when /(.*)reiten$/ then  v = VerbUnregelmaessig.new($1+"reiten", $1+"ritt", ge($1)+"ritten")
  when /(.*)schmei�en$/ then  v = VerbUnregelmaessig.new($1+"schmei�en", $1+"schmiss", ge($1)+"schmissen")
  when /(.*)schneiden$/ then  v = VerbUnregelmaessig.new($1+"schneiden", $1+"schnitt", ge($1)+"schnitten")
    #  ei ie ie
  when /(.*)bleiben$/ then  v = VerbUnregelmaessig.new($1+"bleiben", $1+"blieb", ge($1)+"blieben")
  when /(.*)meiden$/ then  v = VerbUnregelmaessig.new($1+"meiden", $1+"mied", ge($1)+"mieden")
  when /(.*)schreiben$/ then  v = VerbUnregelmaessig.new($1+"schreiben", $1+"schrieb", ge($1)+"schrieben")
  when /(.*)schreien$/ then  v = VerbUnregelmaessig.new($1+"schreien", $1+"schrie", ge($1)+"schrien")
  when /(.*)speien$/ then  v = VerbUnregelmaessig.new($1+"speien", $1+"spie", ge($1)+"spien")
  when /(.*)steigen$/ then  v = VerbUnregelmaessig.new($1+"steigen", $1+"stieg", ge($1)+"stiegen")
    #  a u a
  when /(.*)fahren$/ then v = VerbUnregelmaessig.new($1+"fahren", $1+"fuhr", ge($1)+"fahren")
  when /(.*)tragen$/ then v = VerbUnregelmaessig.new($1+"tragen", $1+"trug", ge($1)+"tragen")
  when /(.*)graben$/ then v = VerbUnregelmaessig.new($1+"graben", $1+"grub", ge($1)+"graben")
  when /(.*)schlagen$/ then v = VerbUnregelmaessig.new($1+"schlagen", $1+"schlug", ge($1)+"schlagen")
    #  au ie au
  when "laufen" then v = VerbUnregelmaessig.new("laufen", "lief", "gelaufen")
  when "hauen" then  v = VerbUnregelmaessig.new("hauen", "hieb", "gehauen")
    # a i a
  when /(.*)fangen$/ then v = VerbUnregelmaessig.new($1+"fangen", $1+"fing", ge($1)+"fangen")
    # a ie a
  when /(.*)lassen$/ then v = VerbUnregelmaessig.new($1+"lassen", $1+"lie�", ge($1)+"lassen")
  when /(.*)fallen$/ then v = VerbUnregelmaessig.new($1+"fallen", $1+"fiel", ge($1)+"fallen")
  when /(.*)halten$/ then v = VerbUnregelmaessig.new($1+"halten", $1+"hielt", ge($1)+"halten")
  when /(.*)schlafen$/ then v = VerbUnregelmaessig.new($1+"schlafen", $1+"schlief", ge($1)+"schlafen")
    # ie o o
  when /(.*)bieten$/ then v = VerbUnregelmaessig.new($1+"bieten", $1+"bot", ge($1)+"boten")
  when /(.*)fliehen$/ then v = VerbUnregelmaessig.new($1+"fliehen", $1+"floh", ge($1)+"flohen")
  when "fliegen" then v = VerbUnregelmaessig.new("fliegen", "flog", "geflogen")
  when /(.*)frieren$/ then v = VerbUnregelmaessig.new($1+"frieren", $1+"fror", ge($1)+"froren")
  when /(.*)lieren$/ then v = VerbUnregelmaessig.new($1+"lieren", $1+"lor", ge($1)+"loren")
  when /(.*)kriechen$/ then v = VerbUnregelmaessig.new($1+"kriechen", $1+"kroch", ge($1)+"krochen")
  when /(.*)riechen$/ then v = VerbUnregelmaessig.new($1+"riechen", $1+"roch", ge($1)+"rochen")
  when /(.*)schieben$/ then v = VerbUnregelmaessig.new($1+"schieben", $1+"schob", ge($1)+"schoben")
  when /(.*)schie�en$/ then v = VerbUnregelmaessig.new($1+"schie�en", $1+"schoss", ge($1)+"schossen")
  when /(.*)schlie�en$/ then v = VerbUnregelmaessig.new($1+"schlie�en", $1+"schloss", ge($1)+"schlossen")
  when "wiegen" then v = VerbUnregelmaessig.new("wiegen", "wog", "gewogen")
  when /(.*)ziehen$/ then v = VerbUnregelmaessig.new($1+"ziehen", $1+"zog", ge($1)+"zogen")
    # i a o
  when /(.*)ginnen$/ then v = VerbUnregelmaessig.new($1+"ginnen", $1+"gann", ge($1)+"gonnen")
  when /(.*)spinnen$/ then v = VerbUnregelmaessig.new($1+"spinnen", $1+"spann", ge($1)+"sponnen")
  when /(.*)winnen$/ then v = VerbUnregelmaessig.new($1+"winnen", $1+"wann", ge($1)+"wonnen")
    # i a e
  when /(.*)bitten$/ then v = VerbUnregelmaessig.new($1+"bitten", $1+"bat", ge($1)+"beten")
  when /(.*)sitzen$/ then v = VerbUnregelmaessig.new($1+"sitzen", $1+"sa�", ge($1)+"sessen")
    # o a o
  when /(.*)kommen$/ then v = VerbUnregelmaessig.new($1+"kommen", $1+"kam", ge($1)+"kommen")
    # u ie u
  when /(.*)rufen$/ then v = VerbUnregelmaessig.new($1+"rufen", $1+"rief", ge($1)+"rufen")
    # o ie o
  when /(.*)sto�en$/ then v = VerbUnregelmaessig.new($1+"sto�en", $1+"stie�", ge($1)+"st��en")
    # regelm�ssig ohne ge
  when "durchleben" then v = Verb.new("durchleben", "durchlebte", "durchlebt")
  when "erfassen" then v = Verb.new("erfassen", "erfasste", "erfasst")
  when /(.*)(fessel)n$/ then v = Verb.new($1+$2+"n", $1+$2+"te", ge($1)+$2+"t")
  when /(.*)(hitz)en$/ then v = Verb.new($1+$2+"en", $1+$2+"te", ge($1)+$2+"t")
  when /(.*)(mann)en$/ then v = Verb.new($1+$2+"en", $1+$2+"te", ge($1)+$2+"t")
  when /(.*)(schreck)en$/ then v = Verb.new($1+$2+"en", $1+$2+"te", ge($1)+$2+"t")
  when /(.*)(splitter)n$/ then v = Verb.new($1+$2+"n", $1+$2+"te", ge($1)+$2+"t")
  when /(.*)(w�rm)en$/ then v = Verb.new($1+$2+"en", $1+$2+"te", ge($1)+$2+"t")
  when /(.*)(wider)n$/ then v = Verb.new($1+$2+"n", $1+$2+"te", ge($1)+$2+"t")
  when /(.*)(z�nd)en$/ then v = Verb.new($1+$2+"en", $1+$2+"te", ge($1)+$2+"t")
    #when /(.*)fluchen$/ then v = Verb.new($1+"fluchen", $1+"fluchte", ge($1)+"flucht")
    #when /(.*)haupten$/ then v = Verb.new($1+"haupten", $1+"hauptete", ge($1)+"hauptet")
    #when /(.*)kratzen$/ then v = Verb.new($1+"kratzen", $1+"kratzte", ge($1)+"kratzt")
    #when /(.*)langen$/ then v = Verb.new($1+"langen", $1+"langte", ge($1)+"langt")
    #when /(.*)letzen$/ then v = Verb.new($1+"letzen", $1+"letzte", ge($1)+"letzt")
    #when /(.*)setzen$/ then v = Verb.new($1+"setzen", $1+"setzte", ge($1)+"setzt")
    #when /(.*)st�rken$/ then v = Verb.new($1+"st�rken", $1+"st�rkte", ge($1)+"st�rkt")
    #when /(.*)stopfen$/ then v = Verb.new($1+"stopfen", $1+"stopfte", ge($1)+"stopft")
    #when /(.*)sperren$/ then v = Verb.new($1+"sperren", $1+"sperrte", ge($1)+"sperrt")
    #when /(.*)wecken$/ then v = Verb.new($1+"wecken", $1+"weckte", ge($1)+"weckt")

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

