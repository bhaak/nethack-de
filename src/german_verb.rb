
class Verb
  attr_reader :infinitiv
  $praesens_endung =    ["e", "st", "t", "en", "t", "en"]
  $praeteritum_endung = ["",  "st", "",  "en", "t", "en"]
  
  def initialize(stamm)
    @stamm = stamm
    @infinitiv = stamm + "en"

    @tempus  = :praesens  # praesens, praeteritum, perfekt, plusquamperfekt, futur, futur ii
    @modus   = :indikativ # indikativ, konjunktiv, imperativ
    @person  = 1          # 0, 1, 2
    @numerus = 0          # 0, 1

    if @stamm[-1..-1]=='t' or @stamm[-1..-1]=='d' then
      @e_erweiterung = true
    # else if # verschlusslaut oder reibelaut + m oder n
    # e_erweiterung = true
    else
      @e_erweiterung = false
    end
  end

  def set_singular
    @numerus = 0
  end

  def set_plural
    @numerus = 1
  end

  def set_indikativ
    @modus = :indikativ
  end

  def set_konjunktiv
    @modus = :konjunktiv
  end

  def set_praeteritum
    @tempus = :praeteritum
  end

  def form
    if @tempus==:praesens && @modus==:indikativ then
      return @stamm + endung($praesens_endung)
    elsif @tempus==:praesens && @modus==:konjunktiv then
      return @stamm + "e" + endung($praeteritum_endung)
    elsif @tempus==:praeteritum && @modus==:indikativ then
      return @stamm + "te" + endung($praeteritum_endung)
    elsif @tempus==:praeteritum && @modus==:konjunktiv then
    end
  end

  def imperativ
    if @numerus == 0 then
      return @stamm + (@e_erweiterung ? "e" : "")
    else
      return @stamm + "t"
    end
  end
  
  def endung(endungen)
    return endungen[@person+@numerus*3]
  end

  def partizip_praesens
    return @stamm + "end"
  end
  def partizip_perfekt
    return "ge"+@stamm + "t"
  end
end

#machen = Verb.new("mach")
#puts machen.form
#puts machen.infinitiv
