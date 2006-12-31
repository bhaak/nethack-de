
class Verb
  attr_reader :infinitiv
  $praesens_endung =    ["e", "st", "t", "en", "t", "en"]
  $praeteritum_endung = ["",  "st", "",  "n", "t", "n"]
  
  def initialize(stamm, praeteritum_stamm=stamm, perfekt_stamm=stamm)
    @infinitiv = stamm + "en"

    @praesens_stamm = stamm
    @praeteritum_stamm = praeteritum_stamm
    @perfekt_stamm = perfekt_stamm

    @tempus  = :praesens  # praesens, praeteritum, perfekt, plusquamperfekt, futur, futur ii
    @modus   = :indikativ # indikativ, konjunktiv, imperativ
    @person  = 1          # 0, 1, 2
    @numerus = 0          # 0, 1

    @e_erweiterung = e_erweiterung?(@praesens_stamm)
  end

  def e_erweiterung?(stamm)
    #puts "Stamm: "+stamm+"-"
    if stamm[-1..-1]=='t' or stamm[-1..-1]=='d' then
      return true
    # else if # verschlusslaut oder reibelaut + m oder n
    # e_erweiterung = true
    else
      return false
    end
    #puts "e_erweiterung: "+@e_erweiterung.to_s+"-"
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

  def form
    if @tempus==:praesens && @modus==:indikativ then
      return @praesens_stamm + endung($praesens_endung)
    elsif @tempus==:praesens && @modus==:konjunktiv then
      return @praesens_stamm + "e" + endung($praeteritum_endung)
    elsif @tempus==:praeteritum && @modus==:indikativ then
      return @praeteritum_stamm + "te" + endung($praeteritum_endung)
    elsif @tempus==:praeteritum && @modus==:konjunktiv then
      return @praeteritum_stamm + "te" + endung($praeteritum_endung)
    end
  end

  def imperativ
    if singular? then
      return @praesens_stamm + (@e_erweiterung ? "e" : "")
    else
      return @praesens_stamm + (@e_erweiterung ? "e" : "") + "t"
    end
  end
  
  def endung(endungen)
    return endungen[@person+@numerus*3]
  end

  def partizip_praesens
    return @praesens_stamm + "end"
  end
  def partizip_perfekt
    return "ge"+@perfekt_stamm + "t"
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
  def aktiv?
    return true
  end

  def Verb.umlaute(stamm)
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
  def form
    if praeteritum? && indikativ? && aktiv? then
      return @praeteritum_stamm + endung($praeteritum_endung)
    elsif praeteritum? && konjunktiv? && aktiv? then
      return Verb.umlaute(@praeteritum_stamm) + (e_erweiterung?(@praeteritum_stamm) ? "e" : "") + endung($praeteritum_endung)
    end
    return super
  end

  def partizip_perfekt
    return "ge"+@perfekt_stamm + "en"
  end
end

class VerbHaben < VerbUnregelmaessig
  def initialize
    super("hab", "hat", "hab")
  end

  def form
    p = personNummer
    if praesens? && indikativ? && singular? then
      if zweitePerson? then
        return "hast"
      elsif drittePerson? then
        return "hat"
      end
    elsif praeteritum? && indikativ? then
      return ["hatte", "hattest", "hatte", "hatten", "hattet", "hatten"][p]
    elsif praeteritum? && konjunktiv? then
      return ["hätte", "hättest", "hätte", "hätten", "hättet", "hätten"][p]
    end
    return super
  end

  def partizip_perfekt
    return "gehabt"
  end
end

class VerbSein < VerbUnregelmaessig
  def initialize()
    super("sei", "war", "wes")
  end

  def infinitiv
    return "sein"
  end

  def form
    p = personNummer
    if praesens? && indikativ? then
      return ["bin", "bist", "ist", "sind", "seid", "sind"][p]
    elsif praesens? && konjunktiv? then
      if singular? and (erstePerson? or drittePerson?) then
        return "sei"
      end
    #elsif praeteritum? && indikativ? then
      #return ["war", "warst", "war", "waren", "wart", "waren"][p]
    end
    return super
  end
end


class VerbSein < VerbUnregelmaessig
  def initialize()
    super("sei", "war", "wes")
  end

  def infinitiv
    return "sein"
  end

  def form
    p = personNummer
    if praesens? && indikativ? then
      return ["bin", "bist", "ist", "sind", "seid", "sind"][p]
    elsif praesens? && konjunktiv? then
      if singular? and (erstePerson? or drittePerson?) then
        return "sei"
      end
    elsif praeteritum? && indikativ? then
      return ["war", "warst", "war", "waren", "wart", "waren"][p]
    end
    return super
  end
end

class VerbWerden < VerbUnregelmaessig
  def initialize()
    super("werd", "wurde", "word")
  end

  def form
    p = personNummer
    if praesens? && indikativ? then
      return ["werde", "wirst", "wird", "werden", "werdet", "werden"][p]
    #elsif praesens? && konjunktiv? then
      #if singular? and (erstePerson? or drittePerson?) then
        #return "sei"
      #end
    #elsif praeteritum? && indikativ? then
      #return ["war", "warst", "war", "waren", "wart", "waren"][p]
    end
    return super
  end
end
