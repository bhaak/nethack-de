require 'set'

require 'german_verb'

# use the genitive-e ("des Zauberbuches" vs. "des Zauberbuchs"
$genitiv_e = true;
# use the dativ-e ("dem Zauberbuche" vs. "dem Zauberbuch"
$dativ_e = true;

$nom = "nominativ";
$gen = "genitiv";
$dat = "dativ";
$akk = "akkusativ";

$sg = "singular"
$pl = "plural"

$mal = "maskulin"
$fem = "feminin"
$neu = "neutrum"

$grundform = "grundform"
$ohne = "ohne"
$bestimmter = "bestimmter"
$unbestimmter = "unbestimmter"

def ausgabe_definitions
  defs = "

#include \"german.h\"
/*
enum Casus   { nominativ=1, genitiv=2, dativ=4, akkusativ=8 };
enum Genus   { maskulin=1, feminin=2, neutrum=4 };
enum Numerus { n_singular=1, n_plural=2 };
enum Person  { erstePerson=1, zweitePerson=2, drittePerson=4 };
enum Artikel { ohne=1, bestimmter=2, unbestimmter=4, grundform=8 };
*/

/* used for nouns and adjectives, as there are nouns, 
   that are inflected like adjectives */
/*
struct substantiv_oder_adjekiv_struct {
	const char  *wort;
	const char  *typ;
  const char  *fugenelement;
	enum Casus   casus;
	enum Genus   genus;
	enum Numerus numerus;
	enum Artikel artikel;
};

struct verb_struct {           
	const char *verb;     
	const char *typ;     
	const char *praeverb;
	enum Person person;  
	enum Numerus numerus;
	enum Casus casus;    
};
*/
"

  puts defs
end

class Adjektiv
  attr_accessor :wort, :bezeichner, :casus, :geschlecht, :numerus, :artikel
  def initialize(w,b,c,g,n,a,f="")
    @wort = w
    @bezeichner = b
    @casus = Set.new
    add_to_set(@casus, c)

    @geschlecht = Set.new
    add_to_set(@geschlecht, g)

    @numerus = Set.new
    if n.kind_of?(Array) then
      n.each { |el| @numerus << 'n_'+el }
    else
      @numerus << 'n_'+n
    end

    @artikel = Set.new
    add_to_set(@artikel, a)

    @fugenelemnt = f
  end

  def add_to_set(set, array_or_string)
    if array_or_string.kind_of?(Array) then
      array_or_string.each { |el| set << el }
    else
      set << array_or_string
    end
  end

  def merge?(other)
    #return (other.wort == @wort  and other.bezeichner == @bezeichner and
              #other.geschlecht == @geschlecht and
              #other.numerus == @numerus and
              #other.artikel == @artikel)
    return (other.wort == @wort  and other.bezeichner == @bezeichner and
              other.geschlecht == @geschlecht and
              other.numerus == @numerus)
  end

  def merge(other)
    other.casus.each { |c| @casus << c }
    other.artikel.each { |c| @artikel << c }
  end

  # hängt eine unveränderliche Zeichenkette an das deklinierte Wort an
  def append(zusatz)
    @wort = @wort + " " + zusatz
  end

  def to_struct
    return '{"'+@wort+'", "'+@bezeichner+'", '+'"'+@fugenelemnt+'", '+
      @casus.to_a.join("|")+ ', '+
      @geschlecht.to_a.join("|")+', '+
      @numerus.to_a.join("|")+', '+
      @artikel.to_a.join("|")+'},'
  end

  def to_s
    return @wort+" "+@bezeichner+" "+@casus.to_a.join("|")+
      " "+@geschlecht.to_a.join("|")+" "+@numerus.to_a.join("|")+" ",@artikel.to_a.join("|")
  end

end

def substantiv_endung(singularstamm, genitiv_singular_endung,
                      pluralstamm, nominativ_plural_endung)

  casus = Hash.new
  casus[$nom] = Hash.new
  casus[$gen] = Hash.new
  casus[$dat] = Hash.new
  casus[$akk] = Hash.new
  
  case genitiv_singular_endung
  when "es"
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = ""
    if singularstamm[-1..-1]=='e' then
      casus[$dat][$sg] = ""
      casus[$gen][$sg] = "s"
    else
      casus[$dat][$sg] = $dativ_e ? "e" : ""
      casus[$gen][$sg] = $genitiv_e ? "es" : "s"
    end
  when "s" 
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = ""
    casus[$dat][$sg] = ""
    casus[$gen][$sg] = "s"
  when "en" 
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = singularstamm[-1..-1]=='e' ? "n" : "en"
    casus[$dat][$sg] = singularstamm[-1..-1]=='e' ? "n" : "en"
    casus[$gen][$sg] = singularstamm[-1..-1]=='e' ? "n" : "en"
  when "ens" 
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = singularstamm[-1..-1]=='e' ? "n" : "en"
    casus[$dat][$sg] = singularstamm[-1..-1]=='e' ? "n" : "en"
    casus[$gen][$sg] = singularstamm[-1..-1]=='e' ? "ns" : "ens"
  when "" 
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = ""
    casus[$dat][$sg] = ""
    casus[$gen][$sg] = ""
  else
    raise "No inflection "+genitiv_singular_endung+" found for "+singularstamm
  end

  if pluralstamm!="" then
    case nominativ_plural_endung
    when "er" 
      casus[$nom][$pl] = "er"
      casus[$akk][$pl] = "er"
      casus[$dat][$pl] = "ern"
      casus[$gen][$pl] = "er"
    when "e"
      endung = pluralstamm[-1..-1]
      casus[$nom][$pl] = pluralstamm[-1..-1]=='e' ? "" : "e"
      casus[$akk][$pl] = pluralstamm[-1..-1]=='e' ? "" : "e"
      casus[$dat][$pl] = pluralstamm[-1..-1]=='e' ? "" : "en"
      casus[$gen][$pl] = pluralstamm[-1..-1]=='e' ? "" : "e"
    when "en"
      e_Tilgung = false
      if pluralstamm[-1..-1] == "e"
        e_Tilgung = true
      elsif pluralstamm[-2..-1] == "el" or pluralstamm[-2..-1] == "er"
        e_Tilgung = true
      end
      casus[$nom][$pl] = e_Tilgung ? "n" : "en"
      casus[$akk][$pl] = e_Tilgung ? "n" : "en"
      casus[$dat][$pl] = e_Tilgung ? "n" : "en"
      casus[$gen][$pl] = e_Tilgung ? "n" : "en"
    when "s"
      casus[$nom][$pl] = "s"
      casus[$akk][$pl] = "s"
      casus[$dat][$pl] = "s"
      casus[$gen][$pl] = "s"
    when "" 
      casus[$nom][$pl] = ""
      casus[$akk][$pl] = ""
      casus[$dat][$pl] = pluralstamm[-1..-1]=='n' ? "" : "n"
      casus[$gen][$pl] = ""
    when "fremder" # foreign word conjugation
      casus[$nom][$pl] = ""
      casus[$akk][$pl] = ""
      casus[$dat][$pl] = ""
      casus[$gen][$pl] = ""
    else
      raise "No inflection "+nominativ_plural_endung+" found for "+pluralstamm
    end
  end

  return casus
end

def dekliniere_substantiv(bezeichner, singularstamm, genitiv_singular_endung,
                          pluralstamm, nominativ_plural_endung, geschlecht, fugenelement="", zusatz="")

  throw "Geschlecht fuer #{bezeichner} nicht angeben" if geschlecht.nil? or geschlecht == ""

  casus = substantiv_endung(singularstamm, genitiv_singular_endung,
                            pluralstamm, nominativ_plural_endung)

  numerus = Hash.new
  numerus[$sg] = singularstamm
  numerus[$pl] = pluralstamm

  nomen = [];
  [$sg].each { |n|
    [$nom, $gen, $dat, $akk].each { |c|
      nomen << Nomen.new(numerus[n]+casus[c][n], bezeichner, c, geschlecht, n, fugenelement);
      #puts nomen[-1].to_struct
    }
  }

  # pluralstamm="" -> no plural 
  if pluralstamm!="" then
    [$pl].each { |n|
      [$nom, $gen, $dat, $akk].each { |c|
        nomen << Nomen.new(numerus[n]+
                             casus[c][n],
                           bezeichner+'s',
                           c,
                           geschlecht,
                           n,
                           fugenelement);
        #puts nomen[-1].to_struct
      }
    }
  end

  # probably unnecessarly O(n^3)
  nomen.each {
    nomen.each { |a| 
      nomen.each { |b|
        if a!=b then
          if b.merge?(a) then
            b.merge(a)
            nomen.delete(a)
          end
        end
      }
    }
  }

  if zusatz!="" then
    nomen.each { |n|
      #$stderr.puts n
      n.append(zusatz)
      #$stderr.puts n
    }
  end
  nomen
end

def dekliniere_eigenname(bezeichner, singularstamm, geschlecht)

  namen = []
  namen << unregelmaessiges_wort(bezeichner, singularstamm, [$nom,$akk,$dat], geschlecht, $sg)
  if [-1..-1] == 's' then
    namen << unregelmaessiges_wort(bezeichner, singularstamm+"'", [$gen], geschlecht, $sg)
  else
    namen << unregelmaessiges_wort(bezeichner, singularstamm+"s", [$gen], geschlecht, $sg)
  end

  namen
end

def unregelmaessiges_wort(bezeichner, wort, casus, geschlecht, numerus, fugenelement="")
  return Nomen.new(wort, bezeichner, casus, geschlecht, numerus, fugenelement)
end

class Nomen < Adjektiv
  def initialize(w,b,c,g,n,fugenelement="")
    super(w,b,c,g,n,"")
    @artikel = Set.new([$ohne,$bestimmter,$unbestimmter,$grundform])
    @fugenelement = fugenelement
  end

  def to_struct
    return '{"'+@wort+'", "'+@bezeichner+'", '+'"'+@fugenelement+'", '+
      @casus.to_a.join("|")+', '+
      @geschlecht.to_a.join("|")+', '+
      @numerus.to_a.join("|")+', '+
      @artikel.to_a.join("|")+'},'
  end

  def to_s
    return @wort+" "+@bezeichner+" "+@casus.to_a.join("|")+
      " "+@geschlecht.to_a.join("|")+" "+@numerus.to_a.join("|")
  end
end

def dekliniere_adjektivisches_substantiv(bezeichner, stamm, artikel)
  adjektive = [];
  if artikel == $neu then
    [
      # Grundform, nur Ausgabe des Stammes
      Adjektiv.new(stamm, bezeichner,  [$nom,$gen,$dat,$akk], [$neu], [$sg,$pl], "grundform"),
      # schwache Flexion (mit bestimmtem Artikel)
      Adjektiv.new(stamm+'e',  bezeichner,  [$nom,$akk], [$neu],           $sg, "bestimmter"),
      Adjektiv.new(stamm+'en', bezeichner,  [$gen,$dat], [$neu],           $sg, "bestimmter"),
      Adjektiv.new(stamm+'en', bezeichner+'s',  [$nom,$gen,$dat,$akk], [$neu], $pl, "bestimmter"),
      # gemischte Flexion (mit ein, kein, Possessivpronomen u.a.)
      Adjektiv.new(stamm+'es', bezeichner,  [$nom,$akk],  $neu,            $sg, "unbestimmter"),
      Adjektiv.new(stamm+'en', bezeichner,  [$gen,$dat], [$neu], $sg, "unbestimmter"),
      Adjektiv.new(stamm+'en', bezeichner+'s',  [$nom,$gen,$dat,$akk], [$neu], $pl, "unbestimmter"),
      # starke Flexion (ohne Artikel)
      Adjektiv.new(stamm+'em', bezeichner,  $dat,      [$neu], $sg, "ohne"),
      Adjektiv.new(stamm+'es', bezeichner, [$nom,$akk], $neu,       $sg, "ohne"),
      Adjektiv.new(stamm+'en', bezeichner,  $gen,       $neu,       $sg, "ohne"),
      Adjektiv.new(stamm+'e',  bezeichner+'s', [$nom,$akk], [$neu], $pl, "ohne"),
      Adjektiv.new(stamm+'er', bezeichner+'s',  $gen,       [$neu], $pl, "ohne"),
      Adjektiv.new(stamm+'en', bezeichner+'s',  $dat,       [$neu], $pl, "ohne"),
      ""
    ].each { |a| adjektive << a }
  elsif artikel == $mal then
    [
      # Grundform, nur Ausgabe des Stammes
      Adjektiv.new(stamm, bezeichner,  [$nom,$gen,$dat,$akk], [$mal], [$sg,$pl], "grundform"),
      # schwache Flexion (mit bestimmtem Artikel)
      Adjektiv.new(stamm+'e',  bezeichner,  [$nom], [$mal],           $sg, "bestimmter"),
      Adjektiv.new(stamm+'en', bezeichner,  [$gen,$dat,$akk], [$mal],           $sg, "bestimmter"),
      Adjektiv.new(stamm+'en', bezeichner+'s',  [$nom,$gen,$dat,$akk], [$mal], $pl, "bestimmter"),
      # gemischte Flexion (mit ein, kein, Possessivpronomen u.a.)
      Adjektiv.new(stamm+'er', bezeichner,  [$nom],  $mal,            $sg, "unbestimmter"),
      Adjektiv.new(stamm+'en', bezeichner,  [$gen,$dat,$akk], [$mal], $sg, "unbestimmter"),
      Adjektiv.new(stamm+'en', bezeichner+'s',  [$nom,$gen,$dat,$akk], [$mal], $pl, "unbestimmter"),
      # starke Flexion (ohne Artikel)
      Adjektiv.new(stamm+'er', bezeichner,  $nom, $mal, $sg, "ohne"),
      Adjektiv.new(stamm+'en', bezeichner,  $akk, $mal, $sg, "ohne"),
      Adjektiv.new(stamm+'em', bezeichner,  $dat, $mal, $sg, "ohne"),
      Adjektiv.new(stamm+'en', bezeichner,  $gen, $mal, $sg, "ohne"),

      Adjektiv.new(stamm+'e',  bezeichner+'s', [$nom,$akk], [$mal], $pl, "ohne"),
      Adjektiv.new(stamm+'en', bezeichner+'s',  $dat,       [$mal], $pl, "ohne"),
      Adjektiv.new(stamm+'er', bezeichner+'s',  $gen,       [$mal], $pl, "ohne"),
      ""
    ].each { |a| adjektive << a }
  elsif artikel == $fem then
    [
      # Grundform, nur Ausgabe des Stammes
      Adjektiv.new(stamm, bezeichner,  [$nom,$gen,$dat,$akk], [$fem], [$sg,$pl], "grundform"),
      # schwache Flexion (mit bestimmtem Artikel)
      Adjektiv.new(stamm+'e',  bezeichner,  [$nom,$akk], [$fem],           $sg, "bestimmter"),
      Adjektiv.new(stamm+'en', bezeichner,  [$gen,$dat], [$fem],           $sg, "bestimmter"),
      Adjektiv.new(stamm+'en', bezeichner+'s',  [$nom,$gen,$dat,$akk], [$fem], $pl, "bestimmter"),
      # gemischte Flexion (mit ein, kein, Possessivpronomen u.a.)
      Adjektiv.new(stamm+'e',  bezeichner,  [$nom,$akk],  $fem, $sg, "unbestimmter"),
      Adjektiv.new(stamm+'en', bezeichner,  [$gen,$dat],  $fem, $sg, "unbestimmter"),
      Adjektiv.new(stamm+'en', bezeichner+'s',  [$nom,$gen,$dat,$akk], [$fem], $pl, "unbestimmter"),
      # starke Flexion (ohne Artikel)
      Adjektiv.new(stamm+'e',  bezeichner,  [$nom,$akk], $fem, $sg, "ohne"),
      Adjektiv.new(stamm+'er', bezeichner,  [$dat,$gen], $fem, $sg, "ohne"),

      Adjektiv.new(stamm+'e',  bezeichner+'s', [$nom,$akk], [$fem], $pl, "ohne"),
      Adjektiv.new(stamm+'en', bezeichner+'s',  $dat,       [$fem], $pl, "ohne"),
      Adjektiv.new(stamm+'er', bezeichner+'s',  $gen,       [$fem], $pl, "ohne"),
      ""
    ].each { |a| adjektive << a }
  else
    raise "Unbekanntes Geschlecht #{artikel} für adjektivisches Substantiv"
  end
  adjektive
end

def dekliniere_adjektiv(bezeichner, stamm)
  adjektive = [];
  [
    # Grundform, nur Ausgabe des Stammes
    Adjektiv.new(stamm, bezeichner,  [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl], "grundform"),
    # schwache Flexion (mit bestimmtem Artikel)
    Adjektiv.new(stamm+'e',  bezeichner,   $nom,       [$mal,$fem,$neu],           $sg, "bestimmter"),
    Adjektiv.new(stamm+'en', bezeichner,   $akk,        $mal,                      $sg, "bestimmter"),
    Adjektiv.new(stamm+'e',  bezeichner,   $akk,       [$fem,$neu],                $sg, "bestimmter"),
    Adjektiv.new(stamm+'en', bezeichner,  [$gen,$dat], [$mal,$fem,$neu],           $sg, "bestimmter"),
    Adjektiv.new(stamm+'en', bezeichner,  [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], $pl, "bestimmter"),
    # gemischte Flexion (mit ein, kein, Possessivpronomen u.a.)
    Adjektiv.new(stamm+'er', bezeichner,   $nom,        $mal,            $sg, "unbestimmter"),
    Adjektiv.new(stamm+'en', bezeichner,   $akk,        $mal,            $sg, "unbestimmter"),
    Adjektiv.new(stamm+'e',  bezeichner,  [$nom,$akk],  $fem,            $sg, "unbestimmter"),
    Adjektiv.new(stamm+'es', bezeichner,  [$nom,$akk],  $neu,            $sg, "unbestimmter"),
    Adjektiv.new(stamm+'en', bezeichner,  [$gen,$dat], [$mal,$fem,$neu], $sg, "unbestimmter"),
    Adjektiv.new(stamm+'en', bezeichner,  [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], $pl, "unbestimmter"),
    # starke Flexion (ohne Artikel)
    Adjektiv.new(stamm+'er', bezeichner,  $nom,       $mal,       $sg, "ohne"),
    Adjektiv.new(stamm+'en', bezeichner, [$akk,$gen], $mal,       $sg, "ohne"),
    Adjektiv.new(stamm+'em', bezeichner,  $dat,      [$mal,$neu], $sg, "ohne"),
    Adjektiv.new(stamm+'es', bezeichner, [$nom,$akk], $neu,       $sg, "ohne"),
    Adjektiv.new(stamm+'en', bezeichner,  $gen,       $neu,       $sg, "ohne"),
    Adjektiv.new(stamm+'e',  bezeichner, [$nom,$akk], $fem,       $sg, "ohne"),
    Adjektiv.new(stamm+'er', bezeichner, [$gen,$dat], $fem,       $sg, "ohne"),
    Adjektiv.new(stamm+'e',  bezeichner, [$nom,$akk], [$mal,$fem,$neu], $pl, "ohne"),
    Adjektiv.new(stamm+'er', bezeichner,  $gen,       [$mal,$fem,$neu], $pl, "ohne"),
    Adjektiv.new(stamm+'en', bezeichner,  $dat,       [$mal,$fem,$neu], $pl, "ohne"),
    ""
  ].each { |a|
    adjektive << a
  }
    
  adjektive
end


def ausgabe_adjectives
  puts "\nstruct adjektiv_struct adjektive[] = {"
  [

  ].each { |s| 
    s.each { |a|
      if a.is_a? String then
        puts a
      else
        puts "  "+a.to_struct
      end
    }
  }
  puts "\n"
  puts "  {NULL, NULL, 0, 0, 0, 0}\n};"
end

class Verb_alt
  attr_accessor :bezeichner, :konjugation, :casus

  def initialize(bezeichner, stamm="", praeverb="", formen=['st','t','t','en'], casus=$akk)
    @bezeichner = bezeichner;

    erweiterung = ""
    # e-Erweiterung, wenn Stamm auf d oder t endet
    if stamm=~ /[dt]$/ then erweiterung = "e" end

    @konjugation = Hash.new
    @konjugation['zweite_singular'] = stamm+erweiterung+formen[0]
    @konjugation['dritte_singular'] = stamm+erweiterung+formen[1]
    @konjugation['zweite_plural']   = stamm+erweiterung+formen[2]
    @konjugation['dritte_plural']   = stamm+formen[3]
    @praeverb = praeverb
    @casus = casus
  end

  def to_struct
    a = Array.new
    a << '{"'+@konjugation['zweite_singular']+'", "'+@bezeichner+'", "'+@praeverb+'", zweitePerson, n_singular, '+ @casus+'},'
    a << '{"'+@konjugation['dritte_singular']+'", "'+@bezeichner+'", "'+@praeverb+'", drittePerson, n_singular, '+@casus+'},'
    a << '{"'+@konjugation['zweite_plural']  +'", "'+@bezeichner+'", "'+@praeverb+'", zweitePerson, n_plural,   '+@casus+'},'
    a << '{"'+@konjugation['dritte_plural']  +'", "'+@bezeichner+'", "'+@praeverb+'", drittePerson, n_plural,   '+@casus+'},'
    return a
  end
end

def konjugiere_verb(bezeichner, stamm)
  return Verb_alt.new(bezeichner, stamm)
end

def ausgabe_verbs
  puts "\nstruct verb_alt_struct verben_alt[] = {"
  [
    konjugiere_verb("VERB_SICH_FUEHLEN","fühl"),
    konjugiere_verb("VERB_FUEHLEN","fühl"),
    konjugiere_verb("VERB_MERKEN","merk"),
    konjugiere_verb("VERB_LANDEN","land"),
    "",
    konjugiere_verb("VERB_FEEL","spür"),
    konjugiere_verb("VERB_HEAR","hör"),
    konjugiere_verb("VERB_SMELL","riech"),
    konjugiere_verb("VERB_MISS","verfehl"),
    konjugiere_verb("VERB_KILL","töt"),
    konjugiere_verb("VERB_TOUCH","berühr"),
    konjugiere_verb("VERB_SCHLEUDERN","schleuder"),
    konjugiere_verb("VERB_ZERSPRINGEN","zerspring"),
    konjugiere_verb("VERB_GELINGEN","geling"),
    konjugiere_verb("VERB_MACHEN","mach"),
    konjugiere_verb("VERB_VIBRIEREN","vibrier"),
    konjugiere_verb("VERB_RUTSCHEN","rutsch"),
    konjugiere_verb("VERB_GLEITEN","gleit"),
    konjugiere_verb("VERB_GEHEN","geh"),
    konjugiere_verb("VERB_FUEHREN","führ"),
    konjugiere_verb("VERB_ABSORBIEREN","absorbier"),
    konjugiere_verb("VERB_ZERTEILEN","zerteil"),
    konjugiere_verb("VERB_OEFFNEN","öffne"),
    konjugiere_verb("VERB_BEENDEN","beend"),
    konjugiere_verb("VERB_ERSCHEINEN","erschein"),
    konjugiere_verb("VERB_WRINGEN","wring"),
    konjugiere_verb("VERB_EXPLODIEREN","explodier"),
    konjugiere_verb("VERB_HAUEN","hau"),
    konjugiere_verb("VERB_PLATZIEREN","platzier"),
    konjugiere_verb("VERB_IMITIEREN","imitier"),
    konjugiere_verb("VERB_JUCKEN","juck"),
    konjugiere_verb("VERB_WIRKEN","wirk"),
    konjugiere_verb("VERB_TROPFEN","tropf"),
    konjugiere_verb("VERB_BRENNEN","brenn"),
    konjugiere_verb("VERB_VERBRENNEN","verbrenn"),
    konjugiere_verb("VERB_REFLEKTIEREN","reflektier"),
    konjugiere_verb("VERB_FRIEREN","frier"),
    konjugiere_verb("VERB_SAGEN","sag"),
    konjugiere_verb("VERB_MEDITIEREN","meditier"),
    konjugiere_verb("VERB_VERKUENDEN","verkünd"),
    konjugiere_verb("VERB_BEMERKEN","bemerk"),
    konjugiere_verb("VERB_LEGEN","leg"),
    konjugiere_verb("VERB_SPUEREN","spür"),
    konjugiere_verb("VERB_VERWANDELN","verwandel"),
    konjugiere_verb("VERB_VERSUCHEN","versuch"),
    konjugiere_verb("VERB_FINDEN","find"),
    konjugiere_verb("VERB_VERSCHWINDEN","verschwind"),
    konjugiere_verb("VERB_WUENSCHEN","wünsch"),
    konjugiere_verb("VERB_VERURSACHEN","verursach"),
    konjugiere_verb("VERB_SPIELEN","spiel"),
    konjugiere_verb("VERB_ENTLOCKEN","entlock"),
    konjugiere_verb("VERB_ERZEUGEN","erzeug"),
    konjugiere_verb("VERB_ERTRINKEN","ertrink"),
    konjugiere_verb("VERB_DEUTEN","deut"),
    konjugiere_verb("VERB_FLUCHEN","fluch"),
    konjugiere_verb("VERB_VERFLUCHEN","verfluch"),
    konjugiere_verb("VERB_JUCKEN","juck"),
    konjugiere_verb("VERB_ROSTEN","rost"),
    konjugiere_verb("VERB_MISSVERSTEHEN","missversteh"),
    konjugiere_verb("VERB_SHRIEK","kreisch"),
    konjugiere_verb("VERB_WAIL","wehklag"),
    konjugiere_verb("VERB_YELL","brüll"),
    konjugiere_verb("VERB_WUERDEN","würd"), # TODO
    "",
    "/* unregelmässige Verben */",
    Verb_alt.new("VERB_SEIN", "", "", ["bist", "ist", "seid", "sind"]),
    Verb_alt.new("VERB_HAVE", "", "", ["hast", "hat", "habt", "haben"]),
    Verb_alt.new("VERB_READ", "", "", ["liest", "liest", "lest","lesen"]),
    Verb_alt.new("VERB_CAN", "", "", ["kannst", "kann", "könnt","können"]),
    Verb_alt.new("VERB_KICK", "", "", ["trittst", "tritt", "tretet","treten"]),
    Verb_alt.new("VERB_BITE", "", "", ["beißt", "beißt", "beißt","beißen"]),
    Verb_alt.new("VERB_HIT", "", "", ["triffst", "trifft", "trefft", "treffen"]),
    Verb_alt.new("VERB_SEE", "", "", ["siehst", "sieht", "seht", "sehen"]),
    Verb_alt.new("VERB_CONTAIN", "", "", ["enthälst", "enthält", "enthaltet","enthalten"]),
    Verb_alt.new("VERB_OPEN", "", "auf", ["gehst", "geht", "geht","gehen"]),
    Verb_alt.new("VERB_EAT", "", "auf", ["isst", "isst", "esst","essen"]),
    Verb_alt.new("VERB_DROP", "", "fallen", ["lässt", "lässt", "lasst","lassen"]),    # "You drop a potion."
    Verb_alt.new("VERB_OBJECT_DROPS", "", "", ["fällst", "fällt", "fallt", "fallen"]), # e.g "A potion drops to the floor."
    Verb_alt.new("VERB_SEEM","","zu sein", ["scheinst", "scheint","scheint", "scheinen"]),
    Verb_alt.new("VERB_SCHLAGEN",  "","", ["schlägst", "schlägt","schlagen", "schlagt"]),
    Verb_alt.new("VERB_ZUSCHLAGEN","","zu", ["schlägst", "schlägt","schlagen", "schlagt"]),
    Verb_alt.new("VERB_STECHEN","","", ["stichst", "sticht","stecht", "stechen"]),
    Verb_alt.new("VERB_ZUSTECHEN","","zu", ["stichst", "sticht","stecht", "stechen"]),
    Verb_alt.new("VERB_SCHIESSEN", "", "", ["schießt", "schießt", "schießt","schießen"]),
    Verb_alt.new("VERB_FANGEN", "", "", ["fängst", "fängt", "fangt","fangen"]),
    Verb_alt.new("VERB_ABBRECHEN", "", "ab", ["brichst", "bricht", "brecht","brechen"]),
    Verb_alt.new("VERB_EINATMEN", "", "ein", ["atmest", "atmet", "atmet","atmen"]),
    Verb_alt.new("VERB_TRAGEN", "", "", ["trägst", "trägt", "tragt","tragen"]),
    Verb_alt.new("VERB_KOENNEN", "", "", ["kannst", "kann", "könnt","können"]),
    Verb_alt.new("VERB_ZITTERN", "", "", ["zitterst", "zittert", "zittert","zittern"]),
    Verb_alt.new("VERB_GLAENZEN", "", "", ["glänzt", "glänzt", "glänzt","glänzen"]),
    Verb_alt.new("VERB_NEHMEN", "", "", ["nimmst", "nimmt", "nehmt","nehmen"]),
    Verb_alt.new("VERB_ZUSCHNUEREN", "", "zu", ["schnürst", "schnürt", "schnürt","schnüren"]),
    Verb_alt.new("VERB_PASSEN", "", "", ["passt", "passt", "passt","passen"]),
    Verb_alt.new("VERB_HALTEN", "", "", ["hälst", "hält", "haltet","halten"]),
    Verb_alt.new("VERB_HINDERN", "", "", ["hinderst", "hindert", "hindert","hindern"]),
    Verb_alt.new("VERB_WERDEN", "", "", ["wirst", "wird", "werdet", "werden"]),
    Verb_alt.new("VERB_ZERFALLEN", "", "", ["zerfällst", "zerfällt", "zerfallt", "zerfallen"]),
    Verb_alt.new("VERB_ERSCHAUDERN", "", "", ["erschauderst", "erschaudert", "erschaudert", "erschaudern"]),
    Verb_alt.new("VERB_VORBEIZISCHEN", "", "vorbei", ["zischst", "zischt", "zischt", "zischen"]),
    Verb_alt.new("VERB_SITZEN", "", "", ["sitzt", "sitzt", "sitzt", "sitzen"]),
    Verb_alt.new("VERB_SETZEN", "", "", ["setzt", "setzt", "setzt", "setzen"]),
    Verb_alt.new("VERB_AUFLOESEN", "", "auf", ["löst", "löst", "löst", "lösen"]),
    Verb_alt.new("VERB_WOLLEN", "", "", ["willst", "will", "wollt", "wollen"]),
    "",
    Verb_alt.new("VERB_DACHTEST", "", "", ["dachtest", "dachte", "dachtet","dachten"]),
    Verb_alt.new("VERB_HAETTEST", "", "", ["hättest", "hätte", "hättet","hätten"]),
    Verb_alt.new("VERB_MOECHTEST", "", "", ["möchtest", "möchte", "möchtet","möchten"]),
    Verb_alt.new("VERB_LASSEN", "", "", ["lässt", "lässt", "lassen","lasst"]),
    "",
    Verb_alt.new("VERB_WORN", "getragen", "", ["", "", "", ""]),
    Verb_alt.new("VERB_ZERBRECHEN", "", "", ["zerbrichst", "zerbricht", "zerbrecht", "zerbrechen"]),
    Verb_alt.new("VERB_VERSTEINERN", "", "", ["versteinerst", "versteinert", "versteinert", "versteinern"]),
    Verb_alt.new("VERB_STERBEN", "", "", ["stirbst", "stirbt", "sterbt", "sterben"]),
    Verb_alt.new("VERB_VERHUNGERN", "", "", ["verhungerst", "verhungert", "verhungert", "verhungern"]),
    Verb_alt.new("VERB_FALLEN", "", "", ["fällst", "fällt", "fallt", "fallen"]),
    Verb_alt.new("VERB_TRAELLERN", "", "", ["trällerst", "trällert", "trällert", "trällern"]),
    Verb_alt.new("VERB_KLIMPERN", "", "", ["klimperst", "klimpert", "klimpert", "klimpern"]),
    Verb_alt.new("VERB_ZUHOEREN", "", "zu", ["hörst", "hört", "hört","hören"]),
    Verb_alt.new("VERB_HERANMACHEN", "", "heran", ["machst", "macht", "macht","machen"]),
    Verb_alt.new("VERB_LAUFEN", "", "", ["läufst", "läuft", "lauft","laufen"]),
    Verb_alt.new("VERB_KNISTERN", "", "", ["knisterst", "knistert", "knistert","knistern"]),
    Verb_alt.new("VERB_AUFHOEREN", "", "auf", ["hörst", "hört", "hört","hören"]),
    Verb_alt.new("VERB_VERHASPELN", "", "", ["verhaspelst", "verhaspelt","verhaspelt","verhaspeln"]),
    Verb_alt.new("VERB_AUFHOEREN", "", "auf", ["hebst", "hebt", "hebt","heben"]),
    Verb_alt.new("VERB_GRABEN", "", "", ["gräbst", "gräbt", "grabt","graben"]),
  ].each { |v|   
    if v.is_a? String then
      # puts v
    else
      # v.to_struct.each { |vf| puts "  "+vf }
    end
  }
  puts "\n"
	puts "  {NULL, NULL, 0, 0}\n};"

  verben = [
    Verb.verb("VERB_SEIN","sein"),
    Verb.verb("VERB_HAVE","haben"),
    Verb.verb("VERB_HABEN","haben"),
    Verb.verb("VERB_MACHEN","machen"),
    Verb.verb("VERB_HERANMACHEN","machen", "heran"),
    Verb.verb("VERB_WERDEN","werden"),
    Verb.verb("VERB_NEHMEN","nehmen"),
    Verb.verb("VERB_CAN","können"),
    Verb.verb("VERB_SMELL","riechen"),
    # Modalverben
    Verb.verb("VERB_KOENNEN","können"),
    Verb.verb("VERB_WOLLEN","wollen"),
    # Sonstige Verben
    Verb.verb("VERB_HEAR","hören"),
    Verb.verb("VERB_READ","lesen"),
		Verb.verb("VERB_HOEREN", "hören"),
    Verb.verb("VERB_ZUHOEREN","hören", "zu"),
    Verb.verb("VERB_AUFHOEREN","hören", "auf"),
    Verb.verb("VERB_EAT","essen"),
    Verb.verb("VERB_OPEN","gehen", "auf"),
    Verb.verb("VERB_RUTSCHEN","rutschen"),
    Verb.verb("VERB_GLEITEN","gleiten"),
    Verb.verb("VERB_BITE","beißen"),
    Verb.verb("VERB_KICK","treten"),
    Verb.verb("VERB_KILL","töten"),
    Verb.verb("VERB_MISS","verfehlen"),
    Verb.verb("VERB_HIT","treffen"),
    Verb.verb("VERB_SEE","sehen"),
    Verb.verb("VERB_TOUCH","berühren"),
    Verb.verb("VERB_BERUEHREN","berühren"),
    Verb.verb("VERB_ABBRECHEN","brechen","ab"),
    Verb.verb("VERB_AUFBRECHEN","brechen","auf"),
    Verb.verb("VERB_BRECHEN","brechen"),
    Verb.verb("VERB_ZUSCHNUEREN","schnüren","zu"),
    Verb.verb("VERB_GELINGEN","gelingen"),
    Verb.verb("VERB_FUEHREN","führen"),
    Verb.verb("VERB_BEENDEN","beenden"),
    Verb.verb("VERB_BETRETEN","betreten"),
    Verb.verb("VERB_ABSORBIEREN","absorbieren"),
    Verb.verb("VERB_HAUEN","hauen"),
    Verb.verb("VERB_PLATZIEREN","platzieren"),
    Verb.verb("VERB_JUCKEN","jucken"),
    Verb.verb("VERB_DROP","lassen", "fallen"),
    Verb.verb("VERB_LASSEN","lassen"),
    Verb.verb("VERB_OBJECT_DROPS","fallen"),
    Verb.verb("VERB_FALLEN","fallen"),
    Verb.verb("VERB_DENKEN","denken"),
    Verb.verb("VERB_LANDEN","landen"),
    Verb.verb("VERB_SEEM","scheinen", "zu sein"),
    Verb.verb("VERB_SCHEINEN","scheinen"),
    Verb.verb("VERB_ERSCHEINEN","erscheinen"),
    Verb.verb("VERB_SICH_FUEHLEN","fühlen"),
    Verb.verb("VERB_TRAGEN","tragen"),
    Verb.verb("VERB_TROPFEN","tropfen"),
    Verb.verb("VERB_BRENNEN","brennen"),
    Verb.verb("VERB_VERBRENNEN","verbrennen"),
    Verb.verb("VERB_VORBEIZISCHEN","zischen", "vorbei"),
    Verb.verb("VERB_FRIEREN","frieren"),
    Verb.verb("VERB_SAGEN","sagen"),
    Verb.verb("VERB_MEDITIEREN","meditieren"),
    Verb.verb("VERB_VERKUENDEN","verkünden"),
    Verb.verb("VERB_BEMERKEN","bemerken"),
    Verb.verb("VERB_AUFLOESEN","lösen", "auf"),
    Verb.verb("VERB_VERSUCHEN","versuchen"),
    Verb.verb("VERB_FINDEN","finden"),
    Verb.verb("VERB_VERSCHWINDEN","verschwinden"),
    Verb.verb("VERB_GRABEN","graben"),
    Verb.verb("VERB_FLIEGEN","fliegen"),
    Verb.verb("VERB_SCHWEBEN","schweben"),
    Verb.verb("VERB_YELL","brüllen"),
    Verb.verb("VERB_SLIP","entgleiten"),
    Verb.verb("VERB_WERFEN","werfen"),

		#
		Verb.verb("VERB_SCHIMMERN","schimmern"),
    Verb.verb("VERB_STAGGER","torkeln"),

		Verb.verb("VERB_SPIELEN","spielen"),
		Verb.verb("VERB_SPRECHEN","sprechen"),
		Verb.verb("VERB_SPUEREN","spüren"),
		Verb.verb("VERB_STECHEN","stechen"),
		Verb.verb("VERB_STECKEN","stecken"),
		Verb.verb("VERB_STEHEN","stehen"),
		Verb.verb("VERB_STEHLEN","stehlen"),
		Verb.verb("VERB_STERBEN","sterben"),
		Verb.verb("VERB_STUERZEN","stürzen"),
		Verb.verb("VERB_SUCHEN","suchen"),
		Verb.verb("VERB_TAUCHEN","tauchen"),
		Verb.verb("VERB_TRAELLERN","trällern"),
		Verb.verb("VERB_TRINKEN","trinken"),
		Verb.verb("VERB_VERFLUCHEN","verfluchen"),
		Verb.verb("VERB_VERHASPELN","verhaspeln"),
		Verb.verb("VERB_VERHUNGERN","verhungern"),
		Verb.verb("VERB_VERLETZEN","verletzen"),
		Verb.verb("VERB_VERSAUEN","versauen"),
		Verb.verb("VERB_VERSTECKEN","verstecken"),
		Verb.verb("VERB_VERSTEIFEN","versteifen"),
		Verb.verb("VERB_VERSTEINERN","versteinern"),
		Verb.verb("VERB_VERURSACHEN","verursachen"),
		Verb.verb("VERB_VERWANDELN","verwandeln"),
		Verb.verb("VERB_VIBRIEREN","vibrieren"),
		Verb.verb("VERB_VISUALISIEREN","visualisieren"),
		Verb.verb("VERB_WEIGERN","weigern"),
		Verb.verb("VERB_WIEGEN","wiegen"),
		Verb.verb("VERB_WIRKEN","wirken"),
		Verb.verb("VERB_WRINGEN","wringen"),
		Verb.verb("VERB_WUENSCHEN","wünschen"),
		Verb.verb("VERB_ZERBRECHEN","zerbrechen"),
		Verb.verb("VERB_ZERFALLEN","zerfallen"),
		Verb.verb("VERB_ZERREISSEN","zerreissen"),
		Verb.verb("VERB_ZERSPRINGEN","zerspringen"),
		Verb.verb("VERB_ZERTEILEN","zerteilen"),
		Verb.verb("VERB_ZITTERN","zittern"),
		Verb.verb("VERB_ZUCKEN","zucken"),
		Verb.verb("VERB_ZUSCHLAGEN","schlagen","zu"),
		Verb.verb("VERB_ZUSTECHEN","stechen","zu"),

		Verb.verb("VERB_AKTIVIEREN","aktivieren"),
		Verb.verb("VERB_ANFUEHLEN","fühlen", "an"),
		Verb.verb("VERB_AUSSEHEN","sehen", "aus"),
		Verb.verb("VERB_BENUTZEN", "benutzen"),
		Verb.verb("VERB_BERUHIGEN", "beruhigen"),
		Verb.verb("VERB_BEWEGEN", "bewegen"),
		Verb.verb("VERB_BRODELN", "brodeln"),
		Verb.verb("VERB_DEUTEN", "deuten"),
		Verb.verb("VERB_DREHEN", "drehen"),
		Verb.verb("VERB_DURCHLEBEN", "durchleben"),
		Verb.verb("VERB_EINATMEN", "atmen", "ein"),
		Verb.verb("VERB_EINSCHLAGEN", "schlagen", "ein"),
		Verb.verb("VERB_ENTLOCKEN", "entlocken"),
		Verb.verb("VERB_ERSCHAUDERN", "erschaudern"),
		Verb.verb("VERB_ERSTARREN", "erstarren"),
		Verb.verb("VERB_ERSTICKEN", "ersticken"),
		Verb.verb("VERB_ERTRAENKEN", "ertränken"),
		Verb.verb("VERB_ERTRINKEN", "ertrinken"),
		Verb.verb("VERB_ERZEUGEN", "erzeugen"),
		Verb.verb("VERB_EXPLODIEREN", "explodieren"),
		Verb.verb("VERB_FANGEN", "fangen"),
		Verb.verb("VERB_FLUCHEN", "fluchen"),
		Verb.verb("VERB_FRAGEN", "fragen"),
		Verb.verb("VERB_FUEHLEN", "fühlen"),
		Verb.verb("VERB_FUELLEN", "füllen"),
		Verb.verb("VERB_GAEHNEN", "gähnen"),
		Verb.verb("VERB_GALOPPIEREN", "galoppieren"),
		Verb.verb("VERB_GLAENZEN", "glänzen"),
		Verb.verb("VERB_IMITIEREN", "imitieren"),
		Verb.verb("VERB_KLIMPERN", "klimpern"),
		Verb.verb("VERB_KNISTERN", "knistern"),
		Verb.verb("VERB_LAECHELN", "lächeln"),
		Verb.verb("VERB_LAUFEN", "laufen"),
		Verb.verb("VERB_LEGEN", "legen"),
		Verb.verb("VERB_LOESCHEN", "löschen"),
		Verb.verb("VERB_LOESEN", "lösen"),
		Verb.verb("VERB_MERKEN", "merken"),
		Verb.verb("VERB_MISSVERSTEHEN", "missverstehen"),
		Verb.verb("VERB_MUTIEREN", "mutieren"),
		Verb.verb("VERB_NEHMEN","nehmen"),
		Verb.verb("VERB_OEFFNEN", "öffnen"),
		Verb.verb("VERB_PACKEN", "packen"),
		Verb.verb("VERB_PASSEN", "passen"),
		Verb.verb("VERB_PULSATE", "pulsieren"),
		Verb.verb("VERB_REAGIEREN", "reagieren"),
		Verb.verb("VERB_REFLEKTIEREN", "reflektieren"),
		Verb.verb("VERB_ROSTEN", "rosten"),
		Verb.verb("VERB_RUELPSEN", "rülpsen"),
		Verb.verb("VERB_SCHNAPPEN", "schnappen"),
		Verb.verb("VERB_SCHUETZEN", "schützen"),
		Verb.verb("VERB_OFFER", "opfern"),
		Verb.verb("VERB_LEUCHTEN", "leuchten"),
		Verb.verb("VERB_AUFLEUCHTEN", "leuchten","auf"),
		Verb.verb("VERB_SONNEN", "sonnen"),
		Verb.verb("VERB_GEBEN", "geben"),
		Verb.verb("VERB_UMGEBEN", "umgeben"),
		Verb.verb("VERB_BETEN", "beten"),
		Verb.verb("VERB_SCHAEDIGEN", "schädigen"),
		Verb.verb("VERB_ENTDECKEN", "entdecken"),
		Verb.verb("VERB_BLICKEN", "blicken"),
		Verb.verb("VERB_PULSIEREN", "pulsieren"),
		Verb.verb("VERB_ENTSORGEN", "entsorgen"),
		Verb.verb("VERB_GRAUEN", "grauen"),
		Verb.verb("VERB_ABPRALLEN", "prallen", "ab"),
		Verb.verb("VERB_LIEGEN", "liegen"),
		Verb.verb("VERB_SCHIESSEN", "schießen"),
		Verb.verb("VERB_SCHLIESSEN", "schließen"),
		Verb.verb("VERB_BEOBACHTEN", "beobachten"),
		Verb.verb("VERB_ENTWEICHEN", "entweichen"),
		Verb.verb("VERB_VERSCHLIESSEN", "verschließen"),
		Verb.verb("VERB_SCHLUCKEN", "schlucken"),
		Verb.verb("VERB_VERSCHLEIMEN", "verschleimen"),
		Verb.verb("VERB_AUFWACHEN", "wachen", "auf"),
		Verb.verb("VERB_KOMMEN", "kommen"),


		Verb.verb("VERB_ABLEHNEN", "lehnen", "ab"),
		Verb.verb("VERB_ANGREIFEN", "greifen", "an"),
		Verb.verb("VERB_AUFHEBEN", "heben", "auf"),
		Verb.verb("VERB_AUFERSTEHEN", "auferstehen"),
		Verb.verb("VERB_AUSGEHEN", "gehen", "aus"),
		Verb.verb("VERB_AUSGRABEN","graben","aus"),
		Verb.verb("VERB_AUSHAENDIGEN","händigen","aus"),
		Verb.verb("VERB_AUSLOESCHEN", "löschen", "aus"),
		Verb.verb("VERB_AUSLOESEN", "lösen", "aus"),
		Verb.verb("VERB_AUSRUTSCHEN","rutschen", "aus"),
		Verb.verb("VERB_AUSSPRECHEN","sprechen","aus"),
		Verb.verb("VERB_AUSSPUCKEN","spucken","aus"),
		Verb.verb("VERB_AUSWEICHEN","weichen","aus"),
		Verb.verb("VERB_AUSZIEHEN","ziehen","aus"),
		Verb.verb("VERB_BEARBEITEN","bearbeiten"),
		Verb.verb("VERB_BEFINDEN","befinden"),
		Verb.verb("VERB_BEGINNEN","beginnen"),
		Verb.verb("VERB_BEGREIFEN","begreifen"),
		Verb.verb("VERB_BEHINDERN","behindern"),
		Verb.verb("VERB_BESTRAHLEN","bestrahlen"),
		Verb.verb("VERB_BEISEITESCHIEBEN","schieben","beiseite"),
		Verb.verb("VERB_BEKOMMEN","bekommen"),
		Verb.verb("VERB_BELOHNEN","belohnen"),
		Verb.verb("VERB_BESCHWEREN","beschweren"),
		Verb.verb("VERB_BETRACHTEN","betrachten"),
		Verb.verb("VERB_BEZIRZEN","bezirzen"),
		Verb.verb("VERB_BITTEN","bitten"),
		Verb.verb("VERB_BLAETTERN","blättern"),
		Verb.verb("VERB_BLEIBEN","bleiben"),
		Verb.verb("VERB_BLENDEN","blenden"),
		Verb.verb("VERB_BLOCKEN","blocken"),
		Verb.verb("VERB_BRAUCHEN","brauchen"),
		Verb.verb("VERB_BRUELLEN","brüllen"),
		Verb.verb("VERB_CONFUSE","verwirren"),
		Verb.verb("VERB_CONTAIN","enthalten"),
		Verb.verb("VERB_DISARM","entschärfen"),
		Verb.verb("VERB_DONNERN","donnern"),
		Verb.verb("VERB_DUERFEN","dürfen"),
		Verb.verb("VERB_EINSCHLAFEN","schlafen","ein"),
		Verb.verb("VERB_ENTFESSELN","entfesseln"),
		Verb.verb("VERB_ENTZUENDEN","entzünden"),
		Verb.verb("VERB_ERFASSEN","erfassen"),
		Verb.verb("VERB_ERGREIFEN","ergreifen"),
		Verb.verb("VERB_ERHITZEN","erhitzen"),
		Verb.verb("VERB_ERLANGEN","erlangen"),
		Verb.verb("VERB_ERSTRAHLEN","erstrahlen"),
		Verb.verb("VERB_ERSETZEN","ersetzen"),
		Verb.verb("VERB_ERTRAGEN","ertragen"),
		Verb.verb("VERB_ERWAERMEN","erwärmen"),
		Verb.verb("VERB_ERWECKEN","erwecken"),
		Verb.verb("VERB_ERWIDERN","erwidern"),
		Verb.verb("VERB_ESSEN","essen"),
		Verb.verb("VERB_ENTGEHEN","entgehen"),
		Verb.verb("VERB_ENTHAUPTEN","enthaupten"),
		Verb.verb("VERB_ENTSCHAEDIGEN","entschädigen"),
		Verb.verb("VERB_ERINNERN","erinnert"),
		Verb.verb("VERB_EXHUMIEREN","exhumieren"),
		Verb.verb("VERB_FAELLEN","fällen"),
		Verb.verb("VERB_FAHREN","fahren"),
		Verb.verb("VERB_FEHLEN","fehlen"),
		Verb.verb("VERB_FLACKERN","flackern"),
		Verb.verb("VERB_FLIEHEN","fliehen"),
		Verb.verb("VERB_FLUESTERN","flüstern"),
		Verb.verb("VERB_FOTOGRAFIEREN","fotografieren"),
		Verb.verb("VERB_FRESSEN","fressen"),
		Verb.verb("VERB_FROESTELN","frösteln"),
		Verb.verb("VERB_FUNKELN","funkeln"),
		Verb.verb("VERB_GAZE","starren"),
		Verb.verb("VERB_GEHEN","gehen"),
		Verb.verb("VERB_GEWAEHREN","gewähren"),
		Verb.verb("VERB_GEWINNEN","gewinnen"),
		Verb.verb("VERB_GREIFEN","greifen"),
		Verb.verb("VERB_HALTEN","halten"),
		Verb.verb("VERB_HECHTEN","hechten"),
		Verb.verb("VERB_HELFEN","helfen"),
		Verb.verb("VERB_HINDERN","hindern"),
		Verb.verb("VERB_HINAUFSTEIGEN","steigen", "hinauf"),
		Verb.verb("VERB_HINDERN","hindern"),
		Verb.verb("VERB_HINZUFUEGEN","fügen","hinzu"),
		Verb.verb("VERB_HUEPFEN","hüpfen"),
		Verb.verb("VERB_HUSTEN","husten"),
		Verb.verb("VERB_IGNORIEREN","ignorieren"),
		Verb.verb("VERB_IDENTIFIZIEREN","identifizieren"),
		Verb.verb("VERB_IDENTIFY","identifizieren"),
		Verb.verb("VERB_INTERESSIEREN","interessieren"),
		Verb.verb("VERB_KAEMPFEN","kämpfen"),
		Verb.verb("VERB_KENNEN","kennen"),
		Verb.verb("VERB_KITZELN","kitzeln"),
		Verb.verb("VERB_KLAEREN","klären"),
		Verb.verb("VERB_KLETTERN","klettern"),
		Verb.verb("VERB_KNALLEN","knallen"),
		Verb.verb("VERB_KOLLIDIEREN","kollidieren"),
		Verb.verb("VERB_KONZENTRIEREN","konzentrieren"),
		Verb.verb("VERB_KOEPFEN","köpfen"),
		Verb.verb("VERB_KOTZEN","kotzen"),
		Verb.verb("VERB_KRACHEN","krachen"),
		Verb.verb("VERB_KRIECHEN","kriechen"),
		Verb.verb("VERB_KRIEGEN","kriegen"),
		Verb.verb("VERB_KUEHLEN","kühlen"),
		Verb.verb("VERB_LACHEN","lachen"),
		Verb.verb("VERB_LAEUTEN","läuten"),
		Verb.verb("VERB_LERNEN","lernen"),
		Verb.verb("VERB_LICHTEN","lichten"),
		Verb.verb("VERB_MEIDEN","meiden"),
		Verb.verb("VERB_MOEGEN","mögen"),
		Verb.verb("VERB_MUESSEN","müssen"),
		Verb.verb("VERB_NEHMEN","nehmen"),
		Verb.verb("VERB_OWE","schulden"),
		Verb.verb("VERB_PREDIGEN","predigen"),
		Verb.verb("VERB_QUELLEN","quellen"),
		Verb.verb("VERB_RAMMEN","rammen"),
		Verb.verb("VERB_REISSEN","reißen"),
		Verb.verb("VERB_REITEN","reiten"),
		Verb.verb("VERB_RIECHEN","riechen"),
		Verb.verb("VERB_SAEUBERN","säubern"),
		Verb.verb("VERB_SCHIEBEN","schieben"),
		Verb.verb("VERB_SCHLAGEN","schlagen"),
		Verb.verb("VERB_SCHLEUDERN","schleudern"),
		Verb.verb("VERB_SCHMATZEN","schmatzen"),
		Verb.verb("VERB_SCHMECKEN","schmecken"),
		Verb.verb("VERB_SCHREIBEN","schreiben"),
		Verb.verb("VERB_SCHREIEN","schreien"),
		Verb.verb("VERB_SCHWINDEN","schwinden"),
		Verb.verb("VERB_SEDUCE","verführen"),
		Verb.verb("VERB_SEHEN","sehen"),
		Verb.verb("VERB_SETZEN","setzen"),
		Verb.verb("VERB_SEUFZEN","seufzen"),
		Verb.verb("VERB_SINKEN","sinken"),
		Verb.verb("VERB_SITZEN","sitzen"),
		Verb.verb("VERB_SCHAUEN","schauen"),
		Verb.verb("VERB_SCHMELZEN","schmelzen"),
		Verb.verb("VERB_SCHWITZEN","schwitzen"),
		Verb.verb("VERB_SOLLEN","sollen"),
		Verb.verb("VERB_SONDIEREN","sondieren"),
		Verb.verb("VERB_SPEIEN","speien"),
		Verb.verb("VERB_SPINNEN","spinnen"),
		Verb.verb("VERB_SPRINGEN","springen"),
		Verb.verb("VERB_SPRITZEN","spritzen"),
		Verb.verb("VERB_SPUCKEN","spucken"),
		Verb.verb("VERB_STABILISIEREN","stabilisieren"),
		Verb.verb("VERB_STEIGEN","steigen"),
		Verb.verb("VERB_STELLEN","stellen"),
		Verb.verb("VERB_STOEREN","stören"),
		Verb.verb("VERB_STOLPERN","stolpern"),
		Verb.verb("VERB_STOPFEN","stopfen"),
		Verb.verb("VERB_STRAMPELN","strampeln"),
		Verb.verb("VERB_STRAUCHELN","straucheln"),
		Verb.verb("VERB_STREIFEN","streifen"),
		Verb.verb("VERB_SUPPEN","suppen"),
		Verb.verb("VERB_TAEUSCHEN","täuschen"),
		Verb.verb("VERB_TEILEN","teilen"),
		Verb.verb("VERB_TRAENEN","tränen"),
		Verb.verb("VERB_TRANSFORMIEREN","transformieren"),
		Verb.verb("VERB_TRETEN","treten"),
		Verb.verb("VERB_UEBERGEBEN","übergeben"),
		Verb.verb("VERB_UEBERFRESSEN","überfressen"),
		Verb.verb("VERB_UEBERMANNEN","übermannen"),
		Verb.verb("VERB_UEBERSTEHEN","überstehen"),
		Verb.verb("VERB_VERBLASSEN","verblassen"),
		Verb.verb("VERB_VERDAUEN","verdauen"),
		Verb.verb("VERB_VERFANGEN","verfangen"),
		Verb.verb("VERB_VERLASSEN","verlassen"),
		Verb.verb("VERB_VERMISCHEN","vermischen"),
		Verb.verb("VERB_VERMODERN","vermodern"),
		Verb.verb("VERB_VERMEHREN","vermehren"),
		Verb.verb("VERB_VERROSTEN","verrosten"),
		Verb.verb("VERB_VERSCHLINGEN","verschlingen"),
		Verb.verb("VERB_VERSCHRECKEN","verschrecken"),
		Verb.verb("VERB_VERSENGEN","versengen"),
		Verb.verb("VERB_VERWIRREN","verwirren"),
		Verb.verb("VERB_VERAETZEN","verätzen"),
		Verb.verb("VERB_VERLANGEN","verlangen"),
		Verb.verb("VERB_VERSINKEN","versinken"),
		Verb.verb("VERB_VERSPERREN","versperren"),
		Verb.verb("VERB_VERSTAERKEN","verstärken"),
		Verb.verb("VERB_VERSTOPFEN","verstopfen"),
		Verb.verb("VERB_WECHSELN","wechseln"),
		Verb.verb("VERB_WACHEN","wachen"),
		Verb.verb("VERB_WACKELN","wackeln"),
		Verb.verb("VERB_WECKEN","wecken"),
		Verb.verb("VERB_WEGWISCHEN","wischen", "weg"),
		Verb.verb("VERB_WINDEN","winden"),
		Verb.verb("VERB_WISSEN","wissen"),
		Verb.verb("VERB_WUERGEN","würgen"),
		Verb.verb("VERB_ZERKRATZEN","zerkratzen"),
		Verb.verb("VERB_ZUSCHNAPPEN","schnappen", "zu"),
		Verb.verb("VERB_ZAHLEN","zahlen"),
		Verb.verb("VERB_ZERSPLITTERN","zersplittern"),
		Verb.verb("VERB_ZIEHEN","ziehen"),
		Verb.verb("VERB_ZURUECKKEHREN","kehren", "zurück"),

    Verb.verb("VERB_HISS","fauchen"),
    Verb.verb("VERB_GROWL","knurren"),
    Verb.verb("VERB_YELP","jaulen"),
    Verb.verb("VERB_YOWL","jaulen"),
    Verb.verb("VERB_ROAR","brüllen"),
    Verb.verb("VERB_BUZZ","brummen"),
    Verb.verb("VERB_SQUEAL","quieken"),
    Verb.verb("VERB_SCREECH","krächzen"),
    Verb.verb("VERB_SCREAK","kreischen"),
    Verb.verb("VERB_SHRIEK","kreischen"),
    Verb.verb("VERB_NEIGH","wiehern"),
    Verb.verb("VERB_WAIL","heulen"),
    Verb.verb("VERB_SNARL","fletschen"),
    Verb.verb("VERB_WHIMPER","wimmern"),
    Verb.verb("VERB_WHINE","winseln"),
    #Verb.verb("VERB_COMMOTION"; /* EN ret = "commotion"; */
    Verb.verb("VERB_SCREAM","schreien"),
  ]

  puts "\nstruct verb_infinitiv_struct verben_infinitiv[] = {"
  verben.each {|v| puts '  {"'+v.infinitiv+'", "'+v.kennung+'"},' }
	puts "  {NULL, NULL}\n};"

  puts "\nstruct verb_partizip_struct verben_partizip[] = {"
  verben.each {|v| puts '  {"'+v.partizip_praesens+'", "'+v.partizip_perfekt+'", "'+v.kennung+'"},' }
	puts "  {NULL, NULL, NULL}\n};"


  puts "\nstruct verb_struct verben[] = {"
  verben.each { |v|
    v.singular.indikativ.praesens
    #puts '  {"'+v.erstePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", erstePerson, n_singular, praesens, akkusativ},'
    puts '  {"'+v.zweitePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", zweitePerson, n_singular, praesens, akkusativ},'
    puts '  {"'+v.drittePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", drittePerson, n_singular, praesens, akkusativ},'
    v.plural.indikativ.praesens
    #puts '  {"'+v.erstePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", erstePerson, n_plural, praesens, akkusativ},'
    puts '  {"'+v.zweitePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", zweitePerson, n_plural, praesens, akkusativ},'
    puts '  {"'+v.drittePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", drittePerson, n_plural, praesens, akkusativ},'

    v.singular.indikativ.praeteritum
    #puts '  {"'+v.erstePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", erstePerson, n_singular, praeteritum, akkusativ},'
    puts '  {"'+v.zweitePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", zweitePerson, n_singular, praeteritum, akkusativ},'
    puts '  {"'+v.drittePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", drittePerson, n_singular, praeteritum, akkusativ},'
    v.plural.indikativ.praeteritum
    #puts '  {"'+v.erstePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", erstePerson, n_plural, praeteritum, akkusativ},'
    puts '  {"'+v.zweitePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", zweitePerson, n_plural, praeteritum, akkusativ},'
    puts '  {"'+v.drittePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", drittePerson, n_plural, praeteritum, akkusativ},'

    v.singular.konjunktiv_ii
    #puts '  {"'+v.erstePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", erstePerson, n_singular, konjunktiv_ii, akkusativ},'
    puts '  {"'+v.zweitePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", zweitePerson, n_singular, konjunktiv_ii, akkusativ},'
    puts '  {"'+v.drittePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", drittePerson, n_singular, konjunktiv_ii, akkusativ},'
    v.plural.konjunktiv_ii
    #puts '  {"'+v.erstePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", erstePerson, n_plural, konjunktiv_ii, akkusativ},'
    puts '  {"'+v.zweitePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", zweitePerson, n_plural, konjunktiv_ii, akkusativ},'
    puts '  {"'+v.drittePerson.form+'", "'+v.kennung+'", "'+v.praeverb+'", drittePerson, n_plural, konjunktiv_ii, akkusativ},'
  }
	puts "  {NULL, NULL, NULL, 0, 0, 0, 0}\n};"

end


def adjektiv_endung(adjektiv, kasus, geschlecht, numerus, artikel)
  # behandle unveränderliche Adjektive hier
  case adjektiv
  when "Luzerner"
    return ""
  end

  # $stderr.puts kasus + " " + geschlecht + " " + numerus + " " + artikel
  # regelmässige Adjektive
  case artikel
  when $grundform, $ohne
    case numerus
    when $sg
      case geschlecht
      when $mal
        case kasus
        when $nom      then return "er"
        when $akk,$gen then return "en"
        when $dat      then return "em"
        end
      when $fem
        case kasus
        when $nom,$akk then return "e"
        when $dat,$gen then return "er"
        end
      when $neu
        case kasus
        when $nom,$akk then return "es"
        when $dat      then return "em"
        when $gen      then return "en"
        end
      end
    when $pl
      case kasus
      when $nom,$akk   then return "e"
      when $dat        then return "en"
      when $gen        then return "er"
      end
    end
  when $bestimmter
    case numerus
    when $sg
      case kasus
      when $nom      then return "e"
      when $dat,$gen then return "en"
      when $akk
        case geschlecht
        when $mal      then return "en"
        when $fem,$neu then return "e"
        end
      end
    when $pl
      return "en"
    end
  when $unbestimmter 
    case numerus
    when $sg
      case kasus
      when $dat,$gen then return "en"
      when $nom,$akk
        case geschlecht
        when $mal 
          case kasus
          when $nom then return "er"
          when $akk then return "en"
          end
        when $fem then return "e"
        when $neu then return "es"
        end
      end
    when $pl
      return "en"
    end
  end

  raise "Adjektivendung für " + adjektiv + " nicht gefunden: "+kasus+" "+geschlecht+" "+numerus+" "+artikel
end

def dekliniere_nominalphrase(bezeichner,
                             adjektive, # Array oder String
                             singularstamm, genitiv_singular_endung,
                             pluralstamm, nominativ_plural_endung,
                             geschlecht,
                             fugenelement="",
                             zusatz="")
  # Adjektiv.new(stamm, bezeichner,  [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl], "grundform"),

  numerus = [$sg, $pl]
  artikel = [$ohne,$bestimmter,$unbestimmter,$grundform]
  kasus   = [$nom, $akk, $dat, $gen]

  if adjektive.kind_of?(String) then
    adjektiv = [adjektive]
  else
    adjektiv = adjektive
  end

  substantiv_casus = substantiv_endung(singularstamm, genitiv_singular_endung,
                                       pluralstamm, nominativ_plural_endung)

  singularformen = Array.new
  pluralformen = Array.new
  
  artikel.each { |art|
    kasus.each { |kas|
      #$stderr.puts art+" "+kas+" "+adjektiv_endung(kas, geschlecht, $sg, art)+" "+
        #singularstamm+substantiv_casus[kas][$sg]
      #$stderr.puts art+" "+kas+" "+stamm+adjektiv_endung(kas, geschlecht, $pl, art)+" "+
        #singularstamm+substantiv_casus[kas][$pl]

      sg = adjektiv.collect {|adj|
        adj+adjektiv_endung(adj, kas, geschlecht, $sg, art)
      }.join(" ")
      sg = sg + " " + singularstamm+substantiv_casus[kas][$sg]
      #singularformen << unregelmaessiges_wort(bezeichner, sg, kas, geschlecht, $sg, fugenelement)
      singularformen << Adjektiv.new(sg, bezeichner, kas, geschlecht, $sg, art)
      
      if pluralstamm!="" then
        pl = adjektiv.collect {|adj|
          adj+adjektiv_endung(adj, kas, geschlecht, $pl, art)
        }.join(" ")
        pl = pl + " " + pluralstamm+substantiv_casus[kas][$pl]
        #pluralformen << unregelmaessiges_wort(bezeichner+"s", pl, kas, geschlecht, $pl, fugenelement)
        pluralformen << Adjektiv.new(pl, bezeichner+"s", kas, geschlecht, $pl, art)
      end
    }
  }
  formen = singularformen.concat(pluralformen)

  if zusatz!="" then
    formen.each { |n|
      #$stderr.puts n
      n.append(zusatz)
      #$stderr.puts n
    }
  end

  # probably unnecessarly O(n^3)
  formen.each {
    formen.each { |a| 
      formen.each { |b|
        if a!=b then
          if b.merge?(a) then
            b.merge(a)
            formen.delete(a)
          end
        end
      }
    }
  }

  #singularformen.each { |s| $stderr.puts s.to_struct } 
  #$stderr.puts singularformen.class
  return formen
end

def ausgabe_nouns
  puts "\nstruct substantiv_oder_adjekiv_struct worte[] = {"

  [
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "der", $nom,  $mal, $sg),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "das", [$nom,$akk],  $neu, $sg),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "des", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "dem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "den", $akk,  $mal, $sg),
    "",
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "die", [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "der", [$gen,$dat], $fem, $sg),
    "",
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "die", [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "der",  $gen,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "den",  $dat,       [$mal,$fem,$neu], $pl),
    "",
    #unregelmaessiges_wort("NOUN_CORPSE",  "sterblichen Überreste", [$nom,$akk], $fem, $sg),
    #unregelmaessiges_wort("NOUN_CORPSE",  "sterblichen Überreste",  $gen,       $fem, $sg),
    #unregelmaessiges_wort("NOUN_CORPSE",  "sterblichen Überresten", $dat,       $fem, $sg),
    #unregelmaessiges_wort("NOUN_CORPSEs",  "sterblichen Überreste von", [$nom,$akk], $fem, $pl),
    #unregelmaessiges_wort("NOUN_CORPSEs",  "sterblichen Überreste von",  $gen,       $fem, $pl),
    #unregelmaessiges_wort("NOUN_CORPSEs",  "sterblichen Überresten von", $dat,       $fem, $pl),
    "",
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "ein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "eines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "einem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "einen", $akk,  $mal, $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "ein",   $akk,  $neu, $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "eine",  [$nom,$akk],  $fem, $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "einer", [$gen,$dat],  $fem, $sg),
    "",
    unregelmaessiges_wort("NOUN_IT",            "es",  [$nom,$akk], $neu, $sg),
    unregelmaessiges_wort("NOUN_IT",            "ihm",  $dat,       $neu, $sg),
    "",
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "du",    $nom,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "deiner",$gen,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "dir",   $dat,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "dich",  $akk,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "Ihr",   $nom,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "Euer",  $gen,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "Euch",  [$akk,$dat],[$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_M_PERSONAL",  "er",    $nom,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_M_PERSONAL",  "seiner",$gen,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_M_PERSONAL",  "ihm",   $dat,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_M_PERSONAL",  "ihn",   $akk,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_F_PERSONAL",  "sie",   $nom,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_F_PERSONAL",  "ihrer", $gen,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_F_PERSONAL",  "ihr",   $dat,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_F_PERSONAL",  "sie",   $akk,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_N_PERSONAL",  "es",    $nom,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_N_PERSONAL",  "seiner",$gen,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_N_PERSONAL",  "ihm",   $dat,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_N_PERSONAL",  "es",    $akk,       [$mal,$fem,$neu], $sg),
    "",
    unregelmaessiges_wort("PRONOMEN_KEIN",  "kein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keinem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keinen", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "kein",   $akk,  $neu,       $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keine",  [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keiner", [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keine",  [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keiner",  $gen,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keinen",  $dat,       [$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "dein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deinem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deinen", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "dein",   $akk,  $neu,       $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deine",  [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deiner", [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deine",  [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deiner", $gen, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deinen", $dat, [$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Euer",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Eures", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Eurem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Euren", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Euer",   $akk,  $neu,       $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Eure",  [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Eurer", [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Eure",  [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Eurer", $gen, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV_PL", "Euren", $dat, [$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_DIESER",  "dieser",   $nom, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "diesen",   $akk, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "diesem",   $dat, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "dieses",   $gen, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "diese",    [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "dieser",   [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "dieses",   [$nom,$gen,$akk], $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "diesem",   $dat, $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "diese",    [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "diesen",   $dat, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_DIESER",  "dieser",   $gen, [$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_ALLE",  "aller",   $nom, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "allen",   $akk, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "allem",   $dat, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "alles",   $gen, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "alle",    [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "aller",   [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "alles",   [$nom,$gen,$akk], $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "allem",   $dat, $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "alle",    [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "allen",   $dat, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_ALLE",  "jalle",   $gen, [$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_JENER",  "jener",   $nom, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jenen",   $akk, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jenem",   $dat, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jenes",   $gen, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jene",    [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jener",   [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jenes",   [$nom,$gen,$akk], $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jenem",   $dat, $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jene",    [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jenen",   $dat, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_JENER",  "jener",   $gen, [$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "sein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seinem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seinen", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "sein",   $akk,  $neu,       $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seine",  [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seiner", [$gen,$dat], $fem, $sg),

    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seine", [$nom,$akk], [$mal,$fem, $neu], $pl),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seinen", $dat, [$mal,$fem, $neu], $pl),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seiner", $gen, [$mal,$fem, $neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihr",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihres", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihrem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihren", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihr",   $akk,  $neu,       $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihre",  [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihrer", [$gen,$dat], $fem, $sg),

    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihre",  [$nom,$akk], [$mal,$fem, $neu], $pl),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihrer", $gen, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihren", $dat, [$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("NOUN_OOPS",  "Hoppla",        [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]),
    unregelmaessiges_wort("NOUN_ETWAS", "etwas",        [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]),
    "",
    unregelmaessiges_wort("PARTIKEL_NAMED",   "namens", [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]), # geannt, namens
    unregelmaessiges_wort("PARTIKEL_CALLED",  "genannt", [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]), # bezeichnet mit, genannt
    unregelmaessiges_wort("PARTIKEL_LABELED", "beschriftet", [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]), # bezeichnet mit, genannt
    unregelmaessiges_wort("PARTIKEL_VON", "von", [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]),
		"",
    unregelmaessiges_wort("NOUN_JEMAND", "jemand", $nom, $mal, $sg),
    unregelmaessiges_wort("NOUN_JEMAND", "jemandes", $gen, $mal, $sg),
    unregelmaessiges_wort("NOUN_JEMAND", "jemandem", $dat, $mal, $sg),
    unregelmaessiges_wort("NOUN_JEMAND", "jemanden", $akk, $mal, $sg),
  ].each { |n|
    if n.is_a? String then
      puts n
    else
      puts "  "+n.to_struct
    end
  }
  puts "  /* ===================================================================== */"

  nomen = []

  [
    "/* generic terms */",
    dekliniere_substantiv("NOUN_WEAPON",    "Waffe",          "",   "Waffe",          "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ARMOR",     "Rüstung",        "",   "Rüstung",      "en", "feminin"),
    # dekliniere_substantiv("NOUN_ARMOR",     "Rüstzeug",        "es",   "Rüstzeug",      "e", "neutrum", "s"),
    # dekliniere_substantiv("NOUN_ARMOR",     "Schutzbekleidung", "",   "Schutzbekleidung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_RING",      "Ring",           "es", "Ring",           "e",  "maskulin"),
    dekliniere_substantiv("NOUN_AMULET",    "Amulett",        "es", "Amulett",        "e",  "neutrum"),
    dekliniere_substantiv("NOUN_TOOL",      "Werkzeug",       "es", "Werkzeug",       "e",  "neutrum", "s"),
    dekliniere_adjektivisches_substantiv("NOUN_COMESTIBLE","Essbar", "neutrum"),
    dekliniere_adjektivisches_substantiv("NOUN_ESSBARES","Essbar", "neutrum"),
    dekliniere_substantiv("NOUN_POTION",    "Trank",          "es", "Tränk",          "e",  "maskulin"),
    dekliniere_substantiv("NOUN_FLASCHE",   "Flasche",        "",  "Flasche",         "en", "feminin"),
    dekliniere_substantiv("NOUN_SCROLL",    "Schriftrolle",   "",   "Schriftrolle",   "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SPELLBOOK", "Zauberbuch",     "es", "Zauberbüch",     "er", "neutrum"),
    dekliniere_substantiv("NOUN_WAND",      "Zauberstab",     "es", "Zauberstäb",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_COIN",      "Münze",          "",   "Münze",          "en", "feminin"),
    "/* besser als Edelstein, Kleinod? */",
    dekliniere_substantiv("NOUN_GEM",       "Schmuckstein",   "es", "Schmuckstein",   "e",  "maskulin"),
    "",
    "/* Weapons, identified */",
    dekliniere_substantiv("NOUN_ARROW", "Pfeil", "es", "Pfeil", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ELVEN_ARROW", "Elbenpfeil", "es", "Elbenpfeil", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_ARROW", "Orkpfeil", "es", "Orkpfeil", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SILVER_ARROW", "Silberpfeil", "es", "Silberpfeil", "e", "maskulin"),
    dekliniere_substantiv("NOUN_YA", "Ya", "s", "Ya", "s", "maskulin"),
    dekliniere_substantiv("NOUN_CROSSBOW_BOLT", "Armbrustbolzen", "s", "Armbrustbolzen", "", "maskulin"),
    dekliniere_substantiv("NOUN_DART", "Wurfpfeil", "es", "Wurfpfeil", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SHURIKEN", "Shuriken", "", "Shuriken", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_BOOMERANG", "Bumerang", "es", "Bumerang", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SPEAR", "Speer", "es", "Speer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ELVEN_SPEAR", "Elbenspeer", "es", "Elbenspeer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_SPEAR", "Orkspeer", "es", "Orkspeer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DWARVISH_SPEAR", "Zwergenspeer", "es", "Zwergenspeer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SILVER_SPEAR", "Silberspeer", "es", "Silberspeer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_JAVELIN", "Wurfspeer", "es", "Wurfspeer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_TRIDENT", "Dreizack", "s", "Dreizack", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DAGGER", "Dolch", "es", "Dolch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ELVEN_DAGGER", "Elbendolch", "es", "Elbendolch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_DAGGER", "Orkdolch", "es", "Orkdolch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SILVER_DAGGER", "Silberdolch", "es", "Silberdolch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ATHAME", "Athame", "", "Athame", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_SCALPEL", "Skalpell", "s", "Skalpell", "e", "neutrum"),
    dekliniere_substantiv("NOUN_KNIFE", "Messer", "s", "Messer", "", "neutrum", "s"),
    dekliniere_substantiv("NOUN_STILETTO", "Stilett", "s", "Stilett", "e", "neutrum"),
    dekliniere_substantiv("NOUN_WORM_TOOTH", "Wurmzahn", "es", "Wurmzähn", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CRYSKNIFE", "Crysmesser", "s", "Crysmesser", "", "neutrum"),
    dekliniere_substantiv("NOUN_AXE", "Beil", "es", "Beil", "e", "neutrum"),
    dekliniere_substantiv("NOUN_BATTLE_AXE", "Streitaxt", "", "Streitäxt", "e", "feminin"),
    dekliniere_substantiv("NOUN_SHORT_SWORD", "Kurzschwert", "es", "Kurzschwert", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_ELVEN_SHORT_SWORD", "elbisch", "Kurzschwert", "es", "Kurzschwert", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_ORCISH_SHORT_SWORD", "orksch", "Kurzschwert", "es", "Kurzschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_DWARVISH_SHORT_SWORD", "Zwergen-Kurzschwert", "es", "Zwergen-Kurzschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SCIMITAR", "Szimitar", "", "Szimitar", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SILVER_SABER", "Silbersäbel", "s", "Silbersäbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_BROADSWORD", "Breitschwert", "es", "Breitschwert", "er", "neutrum", "s"),
    dekliniere_nominalphrase("NOUN_ELVEN_BROADSWORD", "elbisch", "Breitschwert", "es", "Breitschwert", "er", "neutrum", "s"),
    dekliniere_substantiv("NOUN_LONG_SWORD", "Langschwert", "es", "Langschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_TWO_HANDED_SWORD", "Zweihänder", "s", "Zweihänder", "", "maskulin"),
    dekliniere_substantiv("NOUN_TWO_HANDED_SWORD", "Bihänder", "s", "Bihänder", "", "maskulin"),
    dekliniere_substantiv("NOUN_KATANA", "Katana", "", "Katana", "s", "neutrum"),
    dekliniere_substantiv("NOUN_TSURUGI", "Tsurugi", "", "Tsurugi", "s", "neutrum"),
    dekliniere_substantiv("NOUN_RUNESWORD", "Runenschwert", "es", "Runenschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_PARTISAN", "Partisane", "", "Partisane", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RANSEUR", "Knebelspieß", "es", "Knebelspieß", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SPETUM", "Runka", "", "Runka", "s", "feminin"),
    dekliniere_substantiv("NOUN_GLAIVE", "Glefe", "", "Glefe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_LANCE", "Lanze", "", "Lanze", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_HALBERD", "Hellebarde", "", "Hellebarde", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BARDICHE", "Bardiche", "", "Bardiche", "", "feminin", "n"),
    dekliniere_substantiv("NOUN_VOULGE", "Helmbarte", "", "Helmbarte", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_DWARVISH_MATTOCK", "Zwergenhacke", "", "Zwergenhacke", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_FAUCHARD", "Kriegssense", "", "Kriegssense", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GUISARME", "Kuse", "", "Kuse", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BILL_GUISARME", "Roßschinder", "s", "Roßschinder", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_LUCERN_HAMMER", "Luzerner", "Hammer", "s", "Hämmer", "", "maskulin"),
    dekliniere_substantiv("NOUN_BEC_DE_CORBIN", "Rabenschnabel", "s", "Rabenschnäbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_MACE", "Streitkolben", "s", "Streitkolben", "", "maskulin"),
    dekliniere_substantiv("NOUN_MORNING_STAR", "Morgenstern", "es", "Morgenstern", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAR_HAMMER", "Kriegshammer", "s", "Kriegshämmer", "", "maskulin"),
    dekliniere_substantiv("NOUN_CLUB", "Keule", "", "Keule", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RUBBER_HOSE", "Gartenschlauch", "es", "Gartenschläuch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_QUARTERSTAFF", "Kampfstab", "es", "Kampfstäb", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_AKLYS", "Hakenspeer", "es", "Hakenspeer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FLAIL", "Dreschflegel", "s", "Dreschflegel", "", "maskulin"),
    dekliniere_substantiv("NOUN_BULLWHIP", "Peitsche", "", "Peitsche", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BOW", "Bogen", "s", "Bogen", "", "maskulin"),
    dekliniere_substantiv("NOUN_ELVEN_BOW", "Elbenbogen", "s", "Elbenbogen", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_BOW", "Orkbogen", "s", "Orkbogen", "", "maskulin"),
    dekliniere_substantiv("NOUN_YUMI", "Yumi", "", "Yumi", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SLING", "Schleuder", "", "Schleuder", "en", "feminin"),
    dekliniere_substantiv("NOUN_CROSSBOW", "Armbrust", "", "Armbrüst", "e", "feminin"),
    "",
    "/* Weapons, unidentified */",
    dekliniere_substantiv("NOUN_RUNED_ARROW", "Runenpfeil", "es", "Runenpfeil", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_CRUDE_ARROW" # klobig, wuchtig, unfoermig, primitiv,  grobschlaechtig?, massig, plumb, klotzig
    dekliniere_substantiv("NOUN_BAMBOO_ARROW", "Bambuspfeil", "es", "Bambuspfeil", "e", "maskulin"),
    dekliniere_substantiv("NOUN_THROWING_STAR", "Wurfstern", "es", "Wurfstern", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RUNED_SPEAR", "Runenspeer", "es", "Runenspeer", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_CRUDE_SPEAR"
    dekliniere_nominalphrase("NOUN_STOUT_SPEAR", "schwer", "Speer", "es", "Speer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_THROWING_SPEAR", "Wurfspeer", "es", "Wurfspeer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RUNED_DAGGER", "Runendolch", "es", "Runendolch", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_CRUDE_DAGGER"
    #dekliniere_substantiv("NOUN_DOUBLE_HEADED_AXE"
    dekliniere_substantiv("NOUN_RUNED_SHORT_SWORD", "Runenkurzschwert", "es", "Runenkurzschwert", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_CRUDE_SHORT_SWORD"
    #dekliniere_substantiv("NOUN_BROAD_SHORT_SWORD"
    dekliniere_substantiv("NOUN_CURVED_SWORD", "Krummschwert", "es", "Krummschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_RUNED_BROADSWORD", "Runenbreitschwert", "es", "Runenbreitschwert", "er", "neutrum", "s"),
    dekliniere_substantiv("NOUN_SAMURAI_SWORD", "Samuraischwert", "es", "Samuraischwert", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_LONG_SAMURAI_SWORD", "lang", "Samuraischwert", "es", "Samuraischwert", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_VULGAR_POLEARM", "gewöhnlich", "Stangenwaffe", "", "Stangenwaffe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_HILTED_POLEARM", "Stoßspieß", "es", "Stoßspieß", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_FORKED_POLEARM", "gegabelt", "Stangenwaffe", "", "Stangenwaffe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SINGLE_EDGED_POLEARM", "Schwertspieß", "es", "Schwertspieß", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_ANGLED_POLEAXE", "abgewinkelt", "Stangenaxt", "", "Stangenäxt", "e", "feminin"),
    dekliniere_nominalphrase("NOUN_LONG_POLEAXE", "lang", "Stangenbeil", "es", "Stangenbeil", "e", "neutrum"),
    dekliniere_substantiv("NOUN_POLE_CLEAVER", "Stangenbeil", "es", "Stangenbeil", "e", "neutrum"),
    # dekliniere_substantiv("NOUN_BROAD_PICK", "Rodehacke", "", "Rodehacke", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BROAD_PICK", "Breithacke", "", "Breithacke", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_POLE_SICKLE", "Stangensichel", "", "Stangensichel", "en", "feminin"),
    dekliniere_substantiv("NOUN_PRUNING_HOOK", "Stangenmesser", "s", "Stangenmesser", "", "neutrum"),
    dekliniere_substantiv("NOUN_HOOKED_POLEARM", "Hippe", "", "Hippe", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_PRONGED_POLEARM", "lang", "Streithammer", "s", "Streithämmer", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_BEAKED_POLEARM", "spitz", "Streithammer", "s", "Streithämmer", "", "maskulin"),
    dekliniere_substantiv("NOUN_STAFF", "Stock", "es", "Stöck", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_THONGED_CLUB"
    dekliniere_substantiv("NOUN_RUNED_BOW", "Runenbogen", "s", "Runenbogen", "", "maskulin"),
    #dekliniere_substantiv("NOUN_CRUDE_BOW"
    dekliniere_substantiv("NOUN_LONG_BOW", "Langbogen", "s", "Langbogen", "", "maskulin"),
    "",
    "/* Armor, identified */",
    dekliniere_nominalphrase("NOUN_ELVEN_LEATHER_HELM", "ledern", "Elbenhelm", "es", "Elbenhelm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_HELM", "Orkhelm", "es", "Orkhelm", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_DWARVISH_IRON_HELM", "eisern", "Zwergenhelm", "es", "Zwergenhelm", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_FEDORA" maskulin Akkusativ Fedora
    #dekliniere_substantiv("NOUN_CORNUTHAUM"
    #dekliniere_substantiv("NOUN_DUNCE_CAP" # Narrenkappe, Eselmütze
    dekliniere_nominalphrase("NOUN_DENTED_POT", "verbeult", "Topf", "es", "Töpf", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HELMET", "Helm", "es", "Helm", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_HELM_OF_BRILLIANCE"
    #dekliniere_substantiv("NOUN_HELM_OF_OPPOSITE_ALIGNMENT" # Helm der entgegengesetzten Gesinnung
    #dekliniere_substantiv("NOUN_HELM_OF_TELEPATHY" # Helm der Telepathie
    dekliniere_substantiv("NOUN_GRAY_DRAGON_SCALE_MAIL", "Graudrachen-Schuppenpanzer", "s", "Graudrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_SILVER_DRAGON_SCALE_MAIL", "Silberdrachen-Schuppenpanzer", "s", "Silberdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_SHIMMERING_DRAGON_SCALE_MAIL", "Glanzdrachen-Schuppenpanzer", "s", "Glanzdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_RED_DRAGON_SCALE_MAIL", "Rotdrachen-Schuppenpanzer", "s", "Rotdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_WHITE_DRAGON_SCALE_MAIL", "Weißdrachen-Schuppenpanzer", "s", "Weißdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORANGE_DRAGON_SCALE_MAIL", "Orangedrachen-Schuppenpanzer", "s", "Orangedrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_BLACK_DRAGON_SCALE_MAIL", "Schwarzdrachen-Schuppenpanzer", "s", "Schwarzdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_BLUE_DRAGON_SCALE_MAIL", "Blaudrachen-Schuppenpanzer", "s", "Blaudrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_GREEN_DRAGON_SCALE_MAIL", "Gründrachen-Schuppenpanzer", "s", "Gründrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_YELLOW_DRAGON_SCALE_MAIL", "Gelbdrachen-Schuppenpanzer", "s", "Gelbdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_GRAY_DRAGON_SCALES", "Graudrachen-Schuppe", "", "Graudrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SILVER_DRAGON_SCALES", "Silberdrachen-Schuppe", "", "Silberdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SHIMMERING_DRAGON_SCALES", "Glanzdrachen-Schuppe", "", "Glanzdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RED_DRAGON_SCALES", "Rotdrachen-Schuppe", "", "Rotdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WHITE_DRAGON_SCALES", "Weißdrachen-Schuppe", "", "Weißdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ORANGE_DRAGON_SCALES", "Orangedrachen-Schuppe", "", "Orangedrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BLACK_DRAGON_SCALES", "Schwarzdrachen-Schuppe", "", "Schwarzdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BLUE_DRAGON_SCALES", "Gründrachen-Schuppe", "", "Gründrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GREEN_DRAGON_SCALES", "Gründrachen-Schuppe", "", "Gründrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_YELLOW_DRAGON_SCALES", "Gelbdrachen-Schuppe", "", "Gelbdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_PLATE_MAIL", "Plattenpanzer",  "s", "Plattenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_CRYSTAL_PLATE_MAIL", "Kristallpanzer",  "s", "Kristallpanzer", "", "maskulin"), # Kristallplattenpanzer, kristallener Plattenpanzer, Kristallpanzer
    dekliniere_substantiv("NOUN_BRONZE_PLATE_MAIL", "Bronzepanzer",  "s", "Bronzepanzer", "", "maskulin"),  # Bronzeplattenpanzer, bronzener Plattenpanzer, Bronzepanzer
    dekliniere_substantiv("NOUN_SPLINT_MAIL", "Schienenpanzer",  "s", "Schienenpanzer", "", "maskulin"),
    # dekliniere_substantiv("NOUN_BANDED_MAIL", "Spangenharnisch",  "s", "Spangenharnisch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BANDED_MAIL", "Bänderpanzer",  "s", "Bänderpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_DWARVISH_MITHRIL_COAT", "Zwergen-Mithrilhemd", "es", "Zwergen-Mithrilhemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_ELVEN_MITHRIL_COAT", "Elben-Mithrilhemd", "es", "Elben-Mithrilhemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_CHAIN_MAIL", "Kettenhemd", "es", "Kettenhemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_ORCISH_CHAIN_MAIL", "Orkkettenhemd", "es", "Orkkettenhemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_SCALE_MAIL", "Schuppenpanzer", "s", "Schuppenpanzer", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_STUDDED_LEATHER_ARMOR", "beschlagen", "Lederwams", "es", "Lederwäms", "er", "neutrum"),
    dekliniere_substantiv("NOUN_RING_MAIL", "Ringpanzer", "s", "Ringpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_RING_MAIL", "Orkpanzer", "s", "Orkpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEATHER_ARMOR", "Lederpanzer", "s", "Lederpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEATHER_JACKET", "Lederjacke", "", "Lederjacke", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_HAWAIIAN_SHIRT", "Hawaiihemd", "es", "Hawaiihemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_T_SHIRT", "T-Shirt", "s", "T-Shirt", "s", "neutrum"),
    dekliniere_substantiv("NOUN_MUMMY_WRAPPING", "Mumienbinde", "", "Mumienbinde", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_ELVEN_CLOAK", "Elbenmantel", "s", "Elbenmäntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_CLOAK", "Orkmantel", "s", "Orkmäntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_DWARVISH_CLOAK", "Zwergenmantel", "s", "Zwergenmäntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_OILSKIN_CLOAK", "Ölzeug", "es", "Ölzeug", "e", "neutrum"),
    dekliniere_substantiv("NOUN_ROBE", "Robe", "", "Robe", "en", "feminin", "n"), # Robe, Talar
    dekliniere_substantiv("NOUN_ALCHEMY_SMOCK", "Alchemistenkittel", "s", "Alchemistenkittel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEATHER_CLOAK", "Ledermantel", "s", "Ledermäntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_CLOAK_OF_PROTECTION", "Umhang", "es", "Umhäng", "e", "maskulin", "s", "des Schutzes"),
    dekliniere_substantiv("NOUN_CLOAK_OF_INVISIBILITY", "Umhang", "es", "Umhäng", "e", "maskulin", "s", "der Unsichtbarkeit"),
    dekliniere_substantiv("NOUN_CLOAK_OF_MAGIC_RESISTANCE", "Umhang", "es", "Umhäng", "e", "maskulin", "s", "der Magieresistenz"),
    #dekliniere_substantiv("NOUN_CLOAK_OF_DISPLACEMENT" # deplaziert?, verstellt?, disloziert?, verdraengt?, verschoben?, verrueckt?, versetzt, 
    dekliniere_nominalphrase("NOUN_SMALL_SHIELD", "klein", "Schild", "es", "Schild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ELVEN_SHIELD", "Elbenschild", "es", "Elbenschild", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_URUK_HAI_SHIELD"
    dekliniere_substantiv("NOUN_ORCISH_SHIELD", "Orkschild", "es", "Orkschild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_LARGE_SHIELD", "Langschild", "es", "Langschild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DWARVISH_ROUNDSHIELD", "Zwergen-Rundschild", "es", "Zwergen-Rundschild", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_SHIELD_OF_REFLECTION"
    dekliniere_substantiv("NOUN_LEATHER_GLOVES", "Lederhandschuh", "es", "Lederhandschuh", "e", "feminin"),
    dekliniere_substantiv("NOUN_GAUNTLETS_OF_FUMBLING", "Handschuh", "es", "Handschuh", "e", "maskulin", "", "der Fummligkeit"), # Ungeschicklichkeit, Fummligkeit
    dekliniere_substantiv("NOUN_GAUNTLETS_OF_POWER", "Handschuh", "es", "Handschuh", "e", "maskulin", "", "der Macht"),
    dekliniere_substantiv("NOUN_GAUNTLETS_OF_DEXTERITY", "Handschuh", "es", "Handschuh", "e", "maskulin", "", "der Gewandtheit"), # Geschicklichkeit, Gewandheit, Fertigkeit, Fingerfertigkeit
    dekliniere_nominalphrase("NOUN_LOW_BOOTS", "flach", "Schuh", "es", "Schuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_IRON_SHOES", "Eisenschuh", "es", "Eisenschuh", "e", "feminin"),
    dekliniere_nominalphrase("NOUN_HIGH_BOOTS", "hoh", "Stiefel", "s", "Stiefel", "", "maskulin"), 
    dekliniere_substantiv("NOUN_SPEED_BOOTS", "Haststiefel", "s", "Haststiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_WATER_WALKING_BOOTS", "Wasserläufer", "s", "Wasserläufer", "", "maskulin"),
    dekliniere_substantiv("NOUN_JUMPING_BOOTS", "Springerstiefel", "s", "Springerstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_ELVEN_BOOTS", "Elbenstiefel", "s", "Elbenstiefel", "", "maskulin"),
    # dekliniere_substantiv("NOUN_KICKING_BOOTS", "Treterstiefel", "s", "Treterstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_KICKING_BOOTS", "Allestreter", "s", "Allestreter", "", "maskulin"),
    dekliniere_substantiv("NOUN_FUMBLE_BOOTS", "Trampelstiefel", "s", "Trampelstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEVITATION_BOOTS", "Flugschuh", "es", "Flugschuh", "e", "maskulin"),
    "",
    "/* Armor, unidentified */",
    #dekliniere_substantiv("NOUN_LEATHER_HAT"  # Lederhut
    #dekliniere_substantiv("NOUN_IRON_SKULL_CAP"
    #dekliniere_substantiv("NOUN_HARD_HAT"
    #dekliniere_substantiv("NOUN_CONICAL_HAT"
    #dekliniere_substantiv("NOUN_PLUMED_HELMET" # Normannenhelm?, Spangenhelm?, Topfhelm?
    #dekliniere_substantiv("NOUN_ETCHED_HELMET" # Kreuzhelm?, Bronzehelm?, Barbuta?, Eisenhut?
    dekliniere_substantiv("NOUN_CRESTED_HELMET", "Wikingerhelm", "es", "Wikingerhelm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_VISORED_HELMET", "Visierhelm", "es", "Visierhelm", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_CRUDE_CHAIN_MAIL"
    #dekliniere_substantiv("NOUN_CRUDE_RING_MAIL"
    #dekliniere_substantiv("NOUN_FADED_PALL"
    #dekliniere_substantiv("NOUN_COARSE_MANTELET"
    #dekliniere_substantiv("NOUN_HOODED_CLOAK"
    #dekliniere_substantiv("NOUN_SLIPPERY_CLOAK"
    dekliniere_substantiv("NOUN_APRON", "Schürze", "", "Schürze", "en", "feminin", "n"),
    #dekliniere_substantiv("NOUN_TATTERED_CAPE"
    dekliniere_substantiv("NOUN_OPERA_CLOAK", "Opernumhang", "es", "Opernumhäng", "e", "maskulin", "s"),
    #dekliniere_substantiv("NOUN_ORNAMENTAL_COPE"
    #dekliniere_substantiv("NOUN_PIECE_OF_CLOTH" # ein Stueck Stoff, ein Stueck Tuch
    #dekliniere_nominalphrase("NOUN_BLUE_AND_GREEN_SHIELD", ["blau", "und grün"], "Schild", "es", "Schild", "e", "maskulin"),
    #dekliniere_nominalphrase("NOUN_BLUE_AND_GREEN_SHIELD", ["blau-grün"], "Schild", "es", "Schild", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_WHITE_HANDED_SHIELD"
    #dekliniere_substantiv("NOUN_RED_EYED_SHIELD"
    #dekliniere_substantiv("NOUN_LARGE_ROUND_SHIELD"
    #dekliniere_substantiv("NOUN_POLISHED_SILVER_SHIELD"
    dekliniere_nominalphrase("NOUN_OLD_GLOVES", "alt", "Handschuh", "es", "Handschuh", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_PADDED_GLOVES", "gefüttert", "Handschuh", "es", "Handschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RIDING_GLOVES", "Reithandschuh", "es", "Reithandschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FENCING_GLOVES", "Fechthandschuh", "es", "Fechthandschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WALKING_SHOES", "Straßenschuh", "es", "Straßenschuh", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_HARD_SHOES", "hart", "Schuh", "es", "Schuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_JACKBOOTS", "Stulpenstiefel", "s", "Stulpenstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_COMBAT_BOOTS", "Kampfstiefel", "s", "Kampfstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_JUNGLE_BOOTS", "Dschungelstiefel", "s", "Dschungelstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_HIKING_BOOTS", "Wanderstiefel", "s", "Wanderstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_MUD_BOOTS", "Schlammstiefel", "s", "Schlammstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_BUCKLED_BOOTS", "Schnallenstiefel", "s", "Schnallenstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_RIDING_BOOTS", "Reitstiefel", "s", "Reitstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SNOW_BOOTS", "Schneestiefel", "s", "Schneestiefel", "", "maskulin"),
    "",
    "/* Wands, identified */",
    dekliniere_substantiv("NOUN_WAND_LIGHT", "Licht", "es", "Licht", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_SECRET_DOOR_DETECTION", # Geheimtürerspürung?
    #dekliniere_substantiv("NOUN_WAND_CREATE_MONSTER", "Monstererschaffung", "", "Monstererschaffung", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_CREATE_MONSTER", "Monsterherbeirufung",
    dekliniere_substantiv("NOUN_WAND_ENLIGHTENMENT", "Erleuchtung", "", "Erleuchtung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_WAND_CREATE_MONSTER", "Monsterbeschwörung", "", "Monsterbeschwörung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_WISHING", "Wünschen", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_NOTHING", "Nichts", "", "", "", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_STRIKING",
    dekliniere_substantiv("NOUN_WAND_MAKE_INVISIBLE", "Unsichtbarkeit", "", "Unsichtbarkeit", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SLOW_MONSTER", "Verlangsamung", "", "Verlangsamung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SPEED_MONSTER", "Hast", "", "Hast", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_UNDEAD_TURNING", "Untotenwandlung", "", "Untotenwandlung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_WAND_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_WAND_CANCELLATION", "Löschung", "", "Löschung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_WAND_TELEPORTATION", "Teleportation", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_WAND_OPENING", "Öffnung", "", "Öffnung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_LOCKING", "Schliessen", "s", "", "", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_PROBING", # Prüfung
    dekliniere_substantiv("NOUN_WAND_DIGGING", "Graben", "s", "", "", "neutrum"),
    dekliniere_nominalphrase("NOUN_WAND_MAGIC_MISSILE", "magisch", "Geschoss", "es", "Geschoss", "e", "neutrum"),  # magische Geschosse
    dekliniere_substantiv("NOUN_WAND_FIRE", "Feuer", "s", "Feuer", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_COLD", "Kälte", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SLEEP", "Schlaf", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAND_DEATH", "Tod", "es", "Tod", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAND_LIGHTNING", "Blitz", "es", "Blitz", "e", "maskulin"),
    "",
    "/* Rings, identified */",
    dekliniere_substantiv("NOUN_RING_ADORNMENT", "Zierde", "", "Zierde", "en", "feminin"), # der Verschönerung, Zierde, Verzierung
    dekliniere_substantiv("NOUN_RING_GAIN_STRENGTH", "Stärke", "", "Stärke", "en", "feminin"), #  der Stärke, der Kraft, der Kampfkraft?
    dekliniere_substantiv("NOUN_RING_GAIN_CONSTITUTION", "Konstitution", "", "Konstitution", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_INCREASE_ACCURACY", "Treffgenauigkeit", "", "Treffgenauigkeit", "en", "feminin"), # der Treffgenauigkeit, der Präzision, der groesseren Praezision
    dekliniere_substantiv("NOUN_RING_INCREASE_DAMAGE", "Schaden", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_RING_PROTECTION", "Schutz", "es", "Schutz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RING_REGENERATION", "Regeneration", "", "Regeneration", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_SEARCHING", "Suchen", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_RING_STEALTH", "Schleichen", "s", "", "", "neutrum"),
    #dekliniere_substantiv("NOUN_RING_SUSTAIN_ABILITY" 
    dekliniere_substantiv("NOUN_RING_LEVITATION", "Levitation", "", "Levitation", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_HUNGER", "Hungern", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_RING_AGGRAVATE_MONSTER", "Monsterreizung", "", "Monsterreizung", "en", "feminin"), # ?Monsterreizung, Monsternanziehung?
    dekliniere_substantiv("NOUN_RING_CONFLICT", "Konflikt", "es", "Konflikt", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RING_WARNING", "Vorwarnung", "", "Vorwarnung", "en", "feminin"), # Warnung, Vorwarnung?
    dekliniere_substantiv("NOUN_RING_POISON_RESISTANCE", "Giftresistenz", "", "Giftresistenz", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_FIRE_RESISTANCE", "Feuerresistenz", "", "Feuerresistenz", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_COLD_RESISTANCE", "Kälteresistenz", "", "Kälteresistenz", "en", "feminin"),
    #dekliniere_substantiv("NOUN_RING_SHOCK_RESISTANCE" # Stromresistenz
    dekliniere_substantiv("NOUN_RING_FREE_ACTION", "Bewegungsfreiheit", "", "Bewegungsfreiheit", "en", "feminin"), # Freiheit, Bewegungsfreiheit, freien Bewegung
    dekliniere_nominalphrase("NOUN_RING_SLOW_DIGESTION", "langsam", "Verdauung", "", "Verdauung", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_TELEPORTATION", "Teleportation", "", "Teleportation", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_TELEPORT_CONTROL", "Teleportationskontrolle", "", "Teleportationskontrolle", "en", "feminin"), 
    dekliniere_substantiv("NOUN_RING_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_POLYMORPH_CONTROL", "Transformationskontrolle", "", "Transformationskontrolle", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_INVISIBILITY", "Unsichtbarkeit", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_RING_SEE_INVISIBLE", "Enthüllung", "", "Enthüllung", "en", "feminin", " des Unsichtbaren"),
    dekliniere_substantiv("NOUN_RING_PROTECTION_FROM_SHAPE_CHANGERS", "Schutz", "es", "Schutz", "en", "feminin", "", "vor Formwandlern"), # des Schutzes vor Gestaltwandlern
    "",
    "/* Rings, unidentified */",
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_WOODEN",     "Holzring",       "es", "Holzring",       "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_GRANITE",    "Granitring",     "es", "Granitring",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_OPAL",       "Opalring",       "es", "Opalring",       "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_CLAY",       "Tonring",        "es", "Tonring",        "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_CORAL",      "Korallenring",   "es", "Korallenring",   "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_BLACK_ONYX", "Onyxring",       "es", "Onyxring",       "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_MOONSTONE",  "Mondsteinring",  "es", "Mondsteinring",  "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_TIGER_EYE",  "Tigeraugenring", "es", "Tigeraugenring", "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_JADE",       "Jadering",       "es", "Jadering",       "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_BRONZE",     "Bronzering",     "es", "Bronzering",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_AGATE",      "Achatring",      "es", "Achatring",      "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_TOPAZ",      "Topasring",      "es", "Topasring",      "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_SAPPHIRE",   "Saphirring",     "es", "Saphirring",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_RUBY",       "Rubinring",      "es", "Rubinring",      "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_DIAMOND",    "Diamantring",    "es", "Diamantring",    "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_PEARL",      "Perlenring",     "es", "Perlenring",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_IRON",       "Eisenring",      "es", "Eisenring",      "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_BRASS",      "Messingring",    "es", "Messingring",    "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_COPPER",     "Kupferring",     "es", "Kupferring",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_STEEL",      "Stahlring",      "es", "Stahlring",      "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_SILVER",     "Silberring",     "es", "Silberring",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_GOLD",       "Goldring",       "es", "Goldring",       "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_IVORY",      "Elfenbeinring",  "es", "Elfenbeinring",  "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_EMERALD",    "Smaragdring",    "es", "Smaragdring",    "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_WIRE",       "Drahtring",      "es", "Drahtring",      "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_ENGAGEMENT", "Verlobungsring", "",   "Verlobungsring", "e",  "maskulin"),
    "/* Don't want to do a special treatment for twisted and shiny, so these are replaced with other gems */ ",
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_TWISTED",    "Lapislazuliring","es", "Lapislazuliring","e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_SHINY",      "Zirkonring",     "es", "Zirkonring",     "e",  "maskulin"),

    dekliniere_substantiv("RING_UNIDENTIFIED_WOODEN",     "Holz",        "es", "Hölz",        "er", "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_GRANITE",    "Granit",      "s",  "Granit",      "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_OPAL",       "Opal",        "es", "Opal",        "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_CLAY",       "Ton",         "es", "Ton",         "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_CORAL",      "Koralle",     "",   "Koralle",     "en", "feminin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_BLACK_ONYX", "Onyx",        "es", "Onyx",        "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_MOONSTONE",  "Mondstein",   "es", "Mondstein",   "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_TIGER_EYE",  "Tigerauge",   "es", "Tigerauge",   "en", "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_JADE",       "Jade",        "s",  "",            "",   "feminin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_BRONZE",     "Bronze",      "",   "Bronze",      "en", "feminin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_AGATE",      "Achat",       "es", "Achat",       "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_TOPAZ",      "Topas",       "es", "Topas",       "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_SAPPHIRE",   "Saphir",      "s",  "Saphir",      "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_RUBY",       "Rubin",       "s",  "Rubin",       "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_DIAMOND",    "Diamant",     "en", "Diamant",     "en", "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_PEARL",      "Perle",       "",   "Perle",       "en", "feminin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_IRON",       "Eisen",       "s",  "Eisen",       "",   "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_BRASS",      "Messing",     "s",  "Messing",     "e",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_COPPER",     "Kupfer",      "s",  "Kupfer",      "",   "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_STEEL",      "Stahl",       "es", "Stahl",       "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_SILVER",     "Silber",      "s",  "",            "",   "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_GOLD",       "Gold",        "es", "",            "e",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_IVORY",      "Elfenbein",   "es", "Elfenbein",   "e",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_EMERALD",    "Smaragd",     "es", "Smaragd",     "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_WIRE",       "Draht",       "es", "Dräht",       "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_ENGAGEMENT", "Verlobung",   "",   "Verlobung",   "en", "feminin"),
    "/* Don't want to do a special treatment for twisted and shiny, so these are replaced with other gems */ ",
    dekliniere_substantiv("RING_UNIDENTIFIED_TWISTED",    "Lapislazuli", "",   "Lapislazuli", "",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_SHINY",      "Zirkon",      "s",  "Zirkon",      "e",  "maskulin"),
    "",
    "/* Amulets, identified */",
    dekliniere_substantiv("NOUN_AMULET_OF_ESP", "Amulett", "es", "Amulett", "e", "neutrum", "", 
                          "der Außersinnlichen Wahrnehmung"),
    dekliniere_substantiv("NOUN_AMULET_OF_LIFE_SAVING", "Amulett", "es", "Amulett", "e", "neutrum", "", 
                          "der Lebensrettung"),
    dekliniere_substantiv("NOUN_AMULET_OF_STRANGULATION", "Amulett", "es", "Amulett", "e", "neutrum", "", 
                          "der Strangulation"),
    dekliniere_substantiv("NOUN_AMULET_OF_RESTFUL_SLEEP", "Amulett", "es", "Amulett", "e", "neutrum", "", 
                          "des erholsamen Schlafes"),
    dekliniere_substantiv("NOUN_AMULET_VERSUS_POISON", "Amulett", "es", "Amulett", "e", "neutrum", "",
                          "gegen Gift"),
    dekliniere_substantiv("NOUN_AMULET_OF_CHANGE", "Amulett", "es", "Amulett", "e", "neutrum", "",
                          "der Wandlung"),
    dekliniere_substantiv("NOUN_AMULET_OF_UNCHANGING", "Amulett", "es", "Amulett", "e", "neutrum", "", "der Unwandelbarkeit"), # Unwandlung?
    dekliniere_substantiv("NOUN_AMULET_OF_REFLECTION", "Amulett", "es", "Amulett", "e", "neutrum", "", "der Spiegelung"),
    dekliniere_substantiv("NOUN_AMULET_OF_MAGICAL_BREATHING", "Amulett", "es", "Amulett", "e", "neutrum", "", "des magischen Lebensodems"), # magischer Lebensodem, magischer Odem?
    dekliniere_substantiv("NOUN_AMULET_OF_YENDOR", "Amulett", "es", "Amulett", "e", "neutrum", "", 
                          "von Yendor"),
    dekliniere_nominalphrase("NOUN_FAKE_AMULET_OF_YENDOR", "billig", "Plastikimitat", "es", "Plastikimitat", "e",
                             "neutrum", "", "des Amulettes von Yendor"),
    "",
    "/* Amulets, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_AMULET_CIRCULAR","rund"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_SPHERICAL","kugelförmig"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_OVAL","oval"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_TRIANGULAR","dreieckig"),
    "  /* oder besser pyramidenartig oder pyramidal?*/",
    dekliniere_adjektiv("ADJEKTIV_AMULET_PYRAMIDAL","pyramidenförmig"),
    "  /* oder besser rechteckig oder quadratisch?*/",
    dekliniere_adjektiv("ADJEKTIV_AMULET_SQUARE","viereckig"),
    "  /* oder besser konkav?*/",
    dekliniere_adjektiv("ADJEKTIV_AMULET_CONCAVE","gewölbt"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_HEXAGONAL","sechseckig"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_OCTAGONAL","achteckig"),
    "",
    "/* Tools, identified */",
    dekliniere_substantiv("NOUN_LARGE_BOX", "Kiste", "", "Kiste", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_CHEST", "Truhe", "", "Truhe", "en", "feminin"),
    dekliniere_substantiv("NOUN_ICE_BOX", "Kühltruhe", "", "Kühltruhe", "en", "feminin"),
    dekliniere_substantiv("NOUN_ICE_BOX", "Eisbox", "", "Eisbox", "en", "feminin"),
    dekliniere_substantiv("NOUN_SACK", "Sack", "es", "Säck", "e", "maskulin"),
    dekliniere_substantiv("NOUN_OILSKIN_SACK","Ölzeugbeutel", "s", "Ölzeugbeutel", "", "maskulin"),  # Wachstuch, Oilskin, Oelzeug, Regenhaut
    dekliniere_nominalphrase("NOUN_BAG_OF_HOLDING", "Nimmervoll", "Beutel","s","Beutel","","maskulin"),
    dekliniere_substantiv("NOUN_BAG_OF_TRICKS", "Trickbeutel", "s", "Trickbeutel","","maskulin"),  # netzhack: Wundertüte
    dekliniere_substantiv("NOUN_SKELETON_KEY", "Passepartout", "s", "Passepartout", "", "maskulin"),
    dekliniere_substantiv("NOUN_SKELETON_KEY", "Generalschlüssel", "s", "Generalschlüssel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SKELETON_KEY", "Hauptschlüssel", "s", "Hauptschlüssel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LOCK_PICK", "Dietrich", "s", "Dietrich", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CREDIT_CARD", "Kreditkarte", "", "Kreditkarte", "en", "feminin"),
    dekliniere_substantiv("NOUN_TALLOW_CANDLE", "Talglicht", "es", "Talglicht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_WAX_CANDLE", "Wachskerze", "", "Wachskerze", "en", "feminin"),
    dekliniere_substantiv("NOUN_BRASS_LANTERN", "Laterne", "", "Laterne", "en", "feminin"),
    dekliniere_substantiv("NOUN_OIL_LAMP", "Öllampe", "", "Öllampe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_MAGIC_LAMP", "Wunderlampe", "", "Wunderlampe", "en", "feminin", "n"),
    dekliniere_nominalphrase("NOUN_EXPENSIVE_CAMERA", "teur", "Photoapparat", "es", "Photoapparat", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_MIRROR","Spiegel","","Spiegel","","maskulin"),
    dekliniere_substantiv("NOUN_CRYSTAL_BALL", "Kristallkugel", "", "Kristallkugel", "en", "feminin"),
    dekliniere_substantiv("NOUN_LENSES", "Kontaktlinse", "", "Kontaktlinse", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_BLINDFOLD", "Augenbinde",   "",   "Augenbinde",   "en", "feminin"),
    dekliniere_substantiv("NOUN_TOWEL", "Handtuch", "es", "Handtüch", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SADDLE", "Sattel", "s", "Sättel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEASH", "Leine", "", "Leine", "en", "feminin"),
    dekliniere_substantiv("NOUN_STETHOSCOPE", "Stethoskop", "s", "Stethoskop", "e", "neutrum"),
    dekliniere_substantiv("NOUN_TINNING_KIT", "Eindosungs-Set", "s", "Eindosungs-Set", "s", "neutrum"), # Eindosungs-Set, Dos-O-matic?, Eindos-O-matic?
    dekliniere_substantiv("NOUN_TIN_OPENER", "Dosenöffner", "s", "Dosenöffner", "", "maskulin"),
    dekliniere_substantiv("NOUN_CAN_OF_GREASE", "Fettdose", "", "Fettdose", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_FIGURINE", "Statuette", "", "Statuette", "en", "feminin"),
    dekliniere_substantiv("NOUN_MAGIC_MARKER", "Zauberfüller", "s", "Zauberfüller", "", "maskulin"), # verzauberter Fueller? Zauberfueller?
    "/* 'Landmine' is a more generic term. I think 'Tretmine' is the more exact terminus for the mines in Nethack. */",
    dekliniere_substantiv("NOUN_LAND_MINE", "Tretmine", "", "Tretmine", "en", "feminin"),
    dekliniere_substantiv("NOUN_BEARTRAP", "Bärenfalle", "", "Bärenfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_TIN_WHISTLE","Metallpfeife","","Metallpfeife","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIC_WHISTLE","Zauberpfeife","","Zauberpfeife","en","feminin"),
    dekliniere_substantiv("NOUN_WOODEN_FLUTE","Blockflöte","","Blockflöte","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIC_FLUTE","Zauberflöte","","Zauberflöte","en","feminin"),
    dekliniere_substantiv("NOUN_TOOLED_HORN", "Hifthorn", "es", "Hifthörn", "er", "neutrum"), 
    dekliniere_substantiv("NOUN_FROST_HORN", "Eishorn", "es", "Eishörn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_FIRE_HORN", "Feuerhorn", "es", "Feuerhörn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_HORN_OF_PLENTY", "Füllhorn", "es", "Füllhörn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_WOODEN_HARP","Lyra","","Lyren","fremder","feminin"),
    dekliniere_substantiv("NOUN_MAGIC_HARP","Zauberharfe","","Zauberharfe","en","feminin"),
    dekliniere_substantiv("NOUN_BELL", "Glöckchen", "s", "Glöckchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_BUGLE", "Waldhorn", "es", "Waldhörn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_LEATHER_DRUM", "Ledertrommel", "", "Ledertrommel", "en", "feminin"),
    #dekliniere_substantiv("NOUN_DRUM_OF_EARTHQUAKE", 
    dekliniere_substantiv("NOUN_PICK_AXE", "Spitzhacke", "", "Spitzhacke", "en", "feminin"),
    #dekliniere_substantiv("NOUN_GRAPPLING_HOOK", 
    dekliniere_substantiv("NOUN_UNICORN_HORN", "Ainkhürn", "es", "Ainkhürn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_UNICORN_HORN", "Einhorn-Horn", "es", "Einhorn-Hörn", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_CANDELABRUM_OF_INVOCATION", 
    #dekliniere_substantiv("NOUN_BELL_OF_OPENING", 
    "", 
    "/* Tools, unidentified */",
    dekliniere_substantiv("NOUN_BAG","Beutel","s","Beutel","","maskulin"),
    dekliniere_substantiv("NOUN_KEY","Schlüssel","s","Schlüssel","","maskulin"),
    dekliniere_substantiv("NOUN_CANDLE","Kerze","","Kerze","en","feminin"),
    dekliniere_substantiv("NOUN_LAMP","Lampe","","Lampe","en","feminin"),
    dekliniere_substantiv("NOUN_LOOKING_GLASS","Bronzeplatte","","Bronzeplatte","en","feminin"),
    dekliniere_substantiv("NOUN_GLASS_ORB","Glaskugel","","Glaskugel","en","feminin"),
    dekliniere_substantiv("NOUN_WHISTLE","Trillerpfeife","","Trillerpfeife","en","feminin"),
    dekliniere_substantiv("NOUN_FLUTE","Flöte","","Flöte","en","feminin"),
    dekliniere_substantiv("NOUN_HORN","Horn","es","Hörn","er","neutrum"),
    dekliniere_substantiv("NOUN_HARP","Harfe","","Harfe","en","feminin"),
    dekliniere_substantiv("NOUN_DRUM","Trommel","","Trommel","en","feminin"),
    dekliniere_substantiv("NOUN_IRON_HOOK","Eisenhaken","","","","maskulin"),
    dekliniere_substantiv("NOUN_CANDELABRUM","Armleuchter","s","Armleuchter","","maskulin"), 
    dekliniere_substantiv("NOUN_CANDELABRUM","Kerzenleuchter","s","Kerzenleuchter","","maskulin"), # Kandelaber, Armleuchter, Kerzenleuchter
    dekliniere_substantiv("NOUN_SILVER_BELL","Silberglocke","","Silberglocke","en","feminin"),
    "",
    "/* Food */",
    "/* Kutteln, Kaldaunen, Pansen, Wanst. I choose \"Pansen\". This is the name under which it is usually sold for pets. */",
    dekliniere_substantiv("NOUN_TRIPE_RATION", "Pansen", "s",  "Pansen", "", "maskulin"),
    "/* NOUN_CORPSE is irregular and treated specially */",
    dekliniere_substantiv("NOUN_CORPSE","Kadaver","s","Kadaver","","maskulin"),
    # dekliniere_substantiv("NOUN_CORPSE","Leiche","","Leiche","en","feminin"),
    # dekliniere_substantiv("NOUN_CORPSE","Leichnam","s","Leichnam","e","feminin"),
    dekliniere_substantiv("NOUN_EGG","Ei","es","Ei","er","neutrum"),
    dekliniere_substantiv("NOUN_MEATBALL", "Hackbällchen", "s", "Hackbällchen", "", "neutrum"), # Kloesschen, Fleischklops, Frikadelle
    dekliniere_substantiv("NOUN_MEAT_STICK", "Salami", "", "Salami", "", "feminin"),
    dekliniere_substantiv("NOUN_HUGE_CHUNK_OF_MEAT", "Fleischberg", "es", "Fleischberg", "e", "maskulin"),
    dekliniere_substantiv("NOUN_MEAT_RING","Fleischring","es","Fleischring","e","maskulin"),
    "/* Kelpwedel, Kelpblatt? */",
    dekliniere_substantiv("NOUN_KELP_FROND","Seetangblatt","es","Seetangblätt","er","neutrum"),
    dekliniere_substantiv("NOUN_EUCALYPTUS_LEAF","Eukalyptusblatt","es","Eukalyptusblätt","er","neutrum"),
    dekliniere_substantiv("NOUN_APPLE","Apfel","s","Äpfel","","maskulin"),
    # dekliniere_substantiv("NOUN_ORANGE","Apfelsine","","Apfelsine","en","feminin"),
    dekliniere_substantiv("NOUN_ORANGE","Orange","","Orange","en","feminin"),
    dekliniere_substantiv("NOUN_PEAR","Birne","","Birne","en","feminin"),
    dekliniere_substantiv("NOUN_MELON","Melone","","Melone","en","feminin"),
    dekliniere_substantiv("NOUN_BANANA","Banane","","Banane","en","feminin"),
    dekliniere_substantiv("NOUN_CARROT","Karotte","","Karotte","en","feminin"),
    "/* IMPROVE ME: don't really like \"Zweig\" */",
    dekliniere_substantiv("NOUN_SPRIG_OF_WOLFSBANE","Wolfseisenhutzweig","es","Wolfseisenhutzweig","e","maskulin"),
    dekliniere_substantiv("NOUN_CLOVE_OF_GARLIC","Knoblauchzehe","","Knoblauchzehe","en","feminin"),
    dekliniere_substantiv("NOUN_SLIME_MOLD","Schleimpilz","es","Schleimpilz","e","maskulin"),
    dekliniere_substantiv("NOUN_LUMP_OF_ROYAL_JELLY", "Stück", "es", "Stück", "e", "neutrum", "", "Gelée Royale"), # Gelée Royale, Weiselfuttersaft Königinfuttersaft, Bienenmutterbrei
    dekliniere_substantiv("NOUN_CREAM_PIE","Sahnetorte","","Sahnetorte","en","feminin"),
    dekliniere_substantiv("NOUN_CANDY_BAR","Schokoriegel","s","Schokoriegel","","maskulin"),
    dekliniere_substantiv("NOUN_FORTUNE_COOKIE","Glückskeks","es","Glückskeks","e","maskulin"),
    dekliniere_substantiv("NOUN_PANCAKE","Pfannkuchen","s","Pfannkuchen","","maskulin"),
    dekliniere_substantiv("NOUN_LEMBAS_WAFER","Lembasbrot","es","Lembasbrot","e","maskulin"),
    dekliniere_substantiv("NOUN_CRAM_RATION", "Ration", "", "Ration", "en", "feminin", "", "Cram"), # Zwergen-Lebensmittel aus HdR
    dekliniere_substantiv("NOUN_FOOD_RATION","Essensration","","Essensration","en","feminin"),
    dekliniere_substantiv("NOUN_K_RATION","Kampfration","","Kampfration","en","feminin"),
    dekliniere_substantiv("NOUN_C_RATION","Notration","","Notration","en","feminin"),
    dekliniere_substantiv("NOUN_TIN","Dose","","Dose","en","feminin"),
    "",
    "/* Potions, identified */",
    #dekliniere_substantiv("NOUN_POT_GAIN_ABILITY" # Fertigkeitsgewinn
    #dekliniere_substantiv("NOUN_POT_RESTORE_ABILITY" #
    dekliniere_substantiv("NOUN_POT_CONFUSION", "Verwirrung", "", "Verwirrung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_POT_BLINDNESS", "Blindheit", "", "", "", "feminin", "s"),
    dekliniere_substantiv("NOUN_POT_PARALYSIS", "Lähmung", "", "Lähmung", "en", "feminin", "s"), # der Lähmung, der Paralyse
    dekliniere_substantiv("NOUN_POT_SPEED", "Hast", "", "Hast", "en", "feminin"),
    dekliniere_substantiv("NOUN_POT_LEVITATION", "Levitation", "", "Levitation", "en", "feminin"), 
    dekliniere_substantiv("NOUN_POT_HALLUCINATION", "Halluzination", "", "Halluzination", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_POT_INVISIBILITY", "Unsichtbarkeit", "", "Unsichtbarkeit", "en", "feminin"),
    #dekliniere_substantiv("NOUN_POT_SEE_INVISIBLE" # Enthüllung des Unsichtbaren
    dekliniere_substantiv("NOUN_POT_HEALING", "Linderung", "", "", "", "feminin", "s"),
    dekliniere_substantiv("NOUN_POT_EXTRA_HEALING", "Heilung", "", "Heilung", "en", "feminin", "s"),
    dekliniere_nominalphrase("NOUN_POT_FULL_HEALING", "vollständig", "Heilung", "", "Heilung", "en", "feminin", "s"),
    #dekliniere_substantiv("NOUN_POT_GAIN_LEVEL"
    dekliniere_substantiv("NOUN_POT_ENLIGHTENMENT", "Erleuchtung", "", "Erleuchtung", "en", "feminin", "s"), # Erleuchtung, Klarheit?
    #dekliniere_substantiv("NOUN_POT_MONSTER_DETECTION" # Monstererspüren
    #dekliniere_substantiv("NOUN_POT_OBJECT_DETECTION"  # Objekterspürung, Objekterspürens
    #dekliniere_substantiv("NOUN_POT_GAIN_ENERGY"       # Energiegewinnung?
    dekliniere_substantiv("NOUN_POT_SLEEPING", "Schlaf", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_POT_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin", "s"), 
    dekliniere_substantiv("NOUN_POT_BOOZE", "Schnaps", "es", "", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_POT_SICKNESS", "Übelkeit", "", "Übelkeit", "en", "feminin"), # Übelkeit ?
    dekliniere_substantiv("NOUN_POT_FRUIT_JUICE", "Fruchtsaft", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_POT_ACID", "Säure", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_POT_OIL", "Öl", "es", "", "e", "neutrum"), # Öl, Brandöl ?
    dekliniere_substantiv("NOUN_POT_WATER", "Wasser", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_POT_HOLY_WATER", "Weihwasser", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_POT_UNHOLY_WATER", "Satanswasser", "s", "", "", "neutrum"),
    "",
    "/* Potions, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_POT_RUBY","rubinrot"),
    dekliniere_adjektiv("ADJEKTIV_POT_PINK","rosarot"),
    "  /* eigentlich unveränderlich */",
    dekliniere_adjektiv("ADJEKTIV_POT_ORANGE","orangen"),
    dekliniere_adjektiv("ADJEKTIV_POT_YELLOW","gelb"),
    dekliniere_adjektiv("ADJEKTIV_POT_EMERALD","smaragdgrün"),
    dekliniere_adjektiv("ADJEKTIV_POT_DARK_GREEN","dunkelgrün"),
    dekliniere_adjektiv("ADJEKTIV_POT_CYAN","tiefblau"),
    dekliniere_adjektiv("ADJEKTIV_POT_SKY_BLUE","himmelblau"),
    dekliniere_adjektiv("ADJEKTIV_POT_BRILLIANT_BLUE","blauglänzend"),
    dekliniere_adjektiv("ADJEKTIV_POT_MAGENTA","tiefrot"),
    dekliniere_adjektiv("ADJEKTIV_POT_PURPLE_RED","purpurrot"),
    dekliniere_adjektiv("ADJEKTIV_POT_PUCE","dunkelbraun"),
    dekliniere_adjektiv("ADJEKTIV_POT_MILKY","milchig"),
    dekliniere_adjektiv("ADJEKTIV_POT_SWIRLY","verwirbelt"),
    dekliniere_adjektiv("ADJEKTIV_POT_BUBBLY","sprudelnd"),
    dekliniere_adjektiv("ADJEKTIV_POT_SMOKY","rauchig"),
    dekliniere_adjektiv("ADJEKTIV_POT_CLOUDY","unklar"),
    dekliniere_adjektiv("ADJEKTIV_POT_EFFERVESCENT","übersprudelnd"),
    dekliniere_adjektiv("ADJEKTIV_POT_BLACK","schwarz"),
    dekliniere_adjektiv("ADJEKTIV_POT_GOLDEN","golden"),
    dekliniere_adjektiv("ADJEKTIV_POT_BROWN","braun"),
    dekliniere_adjektiv("ADJEKTIV_POT_FIZZY","zischend"),
    dekliniere_adjektiv("ADJEKTIV_POT_DARK","dunkl"),
    dekliniere_adjektiv("ADJEKTIV_POT_WHITE","weiß"),
    dekliniere_adjektiv("ADJEKTIV_POT_MURKY","trüb"),
    dekliniere_adjektiv("ADJEKTIV_POT_CLEAR","durchsichtig"),
    "",
    "/* Scrolls, identified */",
    #dekliniere_substantiv("NOUN_SCR_ENCHANT_ARMOR"   # Schriftrolle der verzauberten Rüstung?
    #dekliniere_substantiv("NOUN_SCR_DESTROY_ARMOR"   # Schriftrolle der zerstörten Rüstung?, beschädigten Rüstung?
    dekliniere_substantiv("NOUN_SCR_CONFUSE_MONSTER", "Monsterverwirrung", "", "Monsterverwirrung", "en", "feminin", "s"),
    #dekliniere_substantiv("NOUN_SCR_SCARE_MONSTER"   
    #dekliniere_substantiv("NOUN_SCR_REMOVE_CURSE"   # Schriftrolle des gebannten Fluches, des Fluchbannens, der Fluchbannung
    #dekliniere_substantiv("NOUN_SCR_ENCHANT_WEAPON"  # Schriftrolle der verzauberten Waffe?
    dekliniere_substantiv("NOUN_SCR_CREATE_MONSTER", "Monsterbeschwörung", "", "", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SCR_TAMING", "Zähmung", "", "Zähmung", "en", "feminin", "s"), # Zähmens, Zähmung
    dekliniere_substantiv("NOUN_SCR_GENOCIDE", "Genozid", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_LIGHT", "Licht", "es", "", "er", "neutrum"), 
    dekliniere_substantiv("NOUN_SCR_TELEPORTATION", "Teleportation", "", "", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SCR_GOLD_DETECTION", "Golderspürung", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_SCR_FOOD_DETECTION", "Nahrungserspürung", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_SCR_IDENTIFY", "Identifizierung", "", "", "en", "feminin", "s"),
    dekliniere_nominalphrase("NOUN_SCR_MAGIC_MAPPING", "magisch", "Kartieren", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SCR_AMNESIA", "Amnesie", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_SCR_FIRE", "Feuer", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SCR_EARTH", "Erde", "", "", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SCR_PUNISHMENT", "Bestrafung", "", "", "en", "feminin", "s"),
    #dekliniere_substantiv("NOUN_SCR_CHARGING"       # Laden, Ladungs??
    #dekliniere_substantiv("NOUN_SCR_STINKING_CLOUD" # Gaswolke?
    #dekliniere_substantiv("NOUN_SCR_MAIL"
    #dekliniere_substantiv("NOUN_SCR_BLANK_PAPER"
    "",
    "/* Scrolls, unidentified */",
    dekliniere_substantiv("NOUN_SCR_ZELGO_MER", "ZELGO MER", "", "ZELGO MER", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_JUYED_AWK_YACC", "JUYED AWK YACC", "", "JUYED AWK YACC", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_NR_9", "NR 9", "", "NR 9", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_XIXAXA_XOXAXA_XUXAXA", "XIXAXA XOXAXA XUXAXA", "", "XIXAXA XOXAXA XUXAXA", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_PRATYAVAYAH", "PRATYAVAYAH", "", "PRATYAVAYAH", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_DAIYEN_FOOELS", "DAIYEN FOOELS", "", "DAIYEN FOOELS", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_LEP_GEX_VEN_ZEA", "LEP GEX VEN ZEA", "", "LEP GEX VEN ZEA", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_PRIRUTSENIE", "PRIRUTSENIE", "", "PRIRUTSENIE", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_ELBIB_YLOH",  "TFIRHCS EGILIEH", "", "TFIRHCS EGILIEH", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_VERR_YED_HORRE", "VERR YED HORRE", "", "VERR YED HORRE", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_VENZAR_BORGAVVE", "VENZAR BORGAVVE", "", "VENZAR BORGAVVE", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_THARR", "THARR", "", "THARR", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_YUM_YUM", "MJAM MJAM", "", "MJAM MJAM", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_KERNOD_WEL", "KERNOD WEL", "", "KERNOD WEL", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_ELAM_EBOW", "ELAM EBOW", "", "ELAM EBOW", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_DUAM_XNAHT", "EKNA EKNAD", "", "EKNA EKNAD", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_ANDOVA_BEGARIN", "ANDOVA BEGARIN", "", "ANDOVA BEGARIN", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_KIRJE", "KIRJE", "", "KIRJE", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_VE_FORBRYDERNE", "VE FORBRYDERNE", "", "VE FORBRYDERNE", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_HACKEM_MUCHE", "HACKEM MUCHE", "", "HACKEM MUCHE", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_VELOX_NEB", "VELOX NEB", "", "VELOX NEB", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_FOOBIE_BLETCH", "FOOBIE BLETCH", "", "FOOBIE BLETCH", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_TEMOV", "TEMOV", "", "TEMOV", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_GARVEN_DEH", "GARVEN DEH", "", "GARVEN DEH", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SCR_READ_ME", "LIES MICH", "", "LIES MICH", "fremder", "maskulin"),
    #dekliniere_substantiv("NOUN_SCR_STAMPED", "", "", "", "", "maskulin"),
    #dekliniere_substantiv("NOUN_SCR_UNLABELED", "", "", "", "", "maskulin"),
    "",
    "/* Spellbooks, identified */",
    dekliniere_substantiv("NOUN_SPE_DIG", "Graben", "s", "", "", "neutrum"),
    dekliniere_nominalphrase("NOUN_SPE_MAGIC_MISSILE", "magisch", "Geschoss", "es", "Geschoss", "e", "neutrum"), # magische Geschosse
    dekliniere_substantiv("NOUN_SPE_FIREBALL", "Feuerball", "es", "Feuerbäll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SPE_CONE_OF_COLD", "Kältekegel", "s", "Kältekegel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SPE_SLEEP", "Schlaf", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SPE_FINGER_OF_DEATH", "Finger", "s", "Finger", "", "maskulin", "", "des Todes"),
    dekliniere_substantiv("NOUN_SPE_LIGHT", "Licht", "es", "Licht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SPE_DETECT_MONSTERS", "Erspüren", "s", "", "", "neutrum", "", "von Monstern"),  # Monster finden, Monster erspüren
    dekliniere_substantiv("NOUN_SPE_HEALING", "Linderung", "", "", "", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_KNOCK", "Klopfen", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SPE_FORCE_BOLT", "Kraftstoß", "es", "Kraftstöß", "e", "maskulin"), # Kraftstoss, Kraftschlag?, Energiestoss?
    dekliniere_substantiv("NOUN_SPE_CONFUSE_MONSTER", "Monsterverwirrung", "", "Monsterverwirrung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_CURE_BLINDNESS", "Kurieren", "s", "", "", "neutrum", "", "von Blindheit"), # Blindheitheilen
    dekliniere_substantiv("NOUN_SPE_DRAIN_LIFE", "Lebensentzug", "es", "", "e", "maskulin", "s"), # Lebensentzug, Blutsauger?
    dekliniere_substantiv("NOUN_SPE_SLOW_MONSTER", "Verlangsamung", "", "Verlangsamung", "en", "feminin", "s"), # Verlangsamung
    dekliniere_substantiv("NOUN_SPE_WIZARD_LOCK", "Zauberverschluss", "es", "Zauberverschlüss", "e", "maskulin"), # Zauberverschluss
    dekliniere_substantiv("NOUN_SPE_CREATE_MONSTER", "Monsterbeschwörung", "", "Monsterbeschwörung", "en", "feminin"), # Monsterbeschwörung
    dekliniere_substantiv("NOUN_SPE_DETECT_FOOD", "Erspüren", "s", "", "", "neutrum", "", "von Nahrung"), # Esswaren, Speise, Lebensmittel, Essen erspüren
    dekliniere_substantiv("NOUN_SPE_CAUSE_FEAR", "Furcht", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_SPE_CLAIRVOYANCE", "Hellsehen", "s", "", "", "neutrum"), # Hellsehen, Hellseherei?, Prophetie?
    dekliniere_substantiv("NOUN_SPE_CURE_SICKNESS", "Kurieren", "s", "", "", "neutrum", "", "von Erkrankungen"), # Krankheit/Erkrankung heilen/kurieren
    dekliniere_substantiv("NOUN_SPE_CHARM_MONSTER", "Monsterbezauberung", "", "Monsterbezauberung", "en", "feminin", "s"),    # Monster bezaubern
    dekliniere_substantiv("NOUN_SPE_HASTE_SELF", "Selbsthast", "", "Selbsthast", "en", "feminin"),
    dekliniere_substantiv("NOUN_SPE_DETECT_UNSEEN", "Erspüren", "s", "", "", "neutrum", "", "des Ungesehenen"),    # Ungesehenes erspüren
    dekliniere_substantiv("NOUN_SPE_LEVITATION", "Levitation", "", "Levitation", "en", "feminin"),
    dekliniere_substantiv("NOUN_SPE_EXTRA_HEALING", "Heilung", "", "Heilung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_RESTORE_ABILITY", "Wiederherstellen", "s", "", "", "neutrum", "", "von Fertigkeiten"),  # Fertigkeit wiederherstellen # IMPROVE ME
    dekliniere_substantiv("NOUN_SPE_INVISIBILITY", "Unsichtbarkeit", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_SPE_DETECT_TREASURE", "Erspüren", "s", "", "", "neutrum", "", "von Schätzen"),  # Schätze erspüren
    dekliniere_substantiv("NOUN_SPE_REMOVE_CURSE", "Fluchbannen", "s", "", "", "neutrum"),     # Fluchbannens
    dekliniere_nominalphrase("NOUN_SPE_MAGIC_MAPPING", "magisch", "Kartieren", "s", "", "", "neutrum"), # magisches Kartieren/Kartographieren?
    dekliniere_substantiv("NOUN_SPE_IDENTIFY", "Identifizierung", "", "Identifizierung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_TURN_UNDEAD", "Untotenwandlung", "", "Untotenwandlung", "en", "feminin", "s"),      # Untotenwandlung, Untote umwandeln?
    dekliniere_substantiv("NOUN_SPE_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_TELEPORT_AWAY", "Wegteleportieren", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SPE_CREATE_FAMILIAR", "Haustierbeschwörung", "", "Haustierbeschwörung", "en", "feminin"), # ????beschwörung # IMPROVE ME, gefällt mir nicht so gut
    dekliniere_substantiv("NOUN_SPE_CANCELLATION", "Löschung", "", "Löschung", "en", "feminin", "s"),     # Annullierung, Entwertung, Löschung, Widerruf
    dekliniere_substantiv("NOUN_SPE_PROTECTION", "Schutz", "es", "Schutz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SPE_JUMPING", "Springen", "s", "Springen", "", "neutrum"),
    dekliniere_substantiv("NOUN_SPE_STONE_TO_FLESH", "Stein-zu-Fleisch", "", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_SPE_FLAME_SPHERE", "Flammenkugel", "", "Flammenkugel", "en", "feminin"), # Flammenkugel, Feuerkugel?, Flammensphäre?, Feuersphäre?
    dekliniere_substantiv("NOUN_SPE_FREEZE_SPHERE", "Frostkugel", "", "Frostkugel", "en", "feminin"),    # Frostkugel, Frostsphäre?
    dekliniere_nominalphrase("NOUN_SPE_BLANK_PAPER", "unbeschriftet", "Papier", "es", "Papier", "e", "neutrum"), # leeres Papier, unbeschriftetes Papier # FIX ME
    dekliniere_substantiv("NOUN_SPE_BOOK_OF_THE_DEAD", "Buch", "es", "Büch", "er", "neutrum", "", "der Toten"), # "Das Buch der Toten"
    "",
    "/* Spellbooks, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_SPE_PARCHMENT", "pergamenten"), # pergamentartig?, Kandidat für 'aus Pergament'
    # dekliniere_substantiv("MADE_OF_SPE_VELLUM", "Velum", "s", "", "s", "neutrum"),
    # dekliniere_adjektiv("ADJEKTIV_SPE_VELLUM", "velin"), # Kandidat für 'aus Velum' # IMPROVE ME
    dekliniere_adjektiv("ADJEKTIV_SPE_VELLUM", "velinpapieren"), # Kandidat für 'aus Velum' # IMPROVE ME
    dekliniere_adjektiv("ADJEKTIV_SPE_RAGGED", "ausgefranst"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DOG_EARED", "eselsohrig"),
    dekliniere_adjektiv("ADJEKTIV_SPE_MOTTLED", "fleckig"),
    dekliniere_adjektiv("ADJEKTIV_SPE_STAINED", "beschmutzt"),
    # dekliniere_substantiv("MADE_OF_SPE_CLOTH", "Stoff", "es", "", "", "maskulin"),
    # dekliniere_adjektiv("ADJEKTIV_SPE_CLOTH", "stoffig"), # Kandidat für 'aus Stoff'
    dekliniere_adjektiv("ADJEKTIV_SPE_CLOTH", "stoffartig"), # Kandidat für 'aus Stoff'
    dekliniere_adjektiv("ADJEKTIV_SPE_LEATHER", "ledern"), # Kandidat für 'aus Leder'
    dekliniere_adjektiv("ADJEKTIV_SPE_WHITE", "weiß"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PINK", "rosarot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_RED", "rot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_ORANGE", "orangen"), # Kandidat für unveränderliche Konjugation
    dekliniere_adjektiv("ADJEKTIV_SPE_YELLOW", "gelb"),
    dekliniere_adjektiv("ADJEKTIV_SPE_VELVET", "samten"), # Kandidat für 'aus Samt'
    dekliniere_adjektiv("ADJEKTIV_SPE_LIGHT_GREEN", "hellgrün"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DARK_GREEN", "dunkelgrün"),
    dekliniere_adjektiv("ADJEKTIV_SPE_TURQUOISE", "türkisfarben"), # Kandidat für unveränderliche Konjugation
    dekliniere_adjektiv("ADJEKTIV_SPE_CYAN", "tiefblau"),
    dekliniere_adjektiv("ADJEKTIV_SPE_LIGHT_BLUE", "hellblau"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DARK_BLUE", "dunkelblau"),
    dekliniere_adjektiv("ADJEKTIV_SPE_INDIGO", "indigofarben"),
    dekliniere_adjektiv("ADJEKTIV_SPE_MAGENTA", "tiefrot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PURPLE", "purpurn"),
    dekliniere_adjektiv("ADJEKTIV_SPE_VIOLET", "violett"),
    dekliniere_adjektiv("ADJEKTIV_SPE_TAN", "braun"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PLAID", "gemustert"), # kariert? Aber ein "kariertes Buch"?
    dekliniere_adjektiv("ADJEKTIV_SPE_LIGHT_BROWN", "hellbraun"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DARK_BROWN", "dunkelbraun"),
    dekliniere_adjektiv("ADJEKTIV_SPE_GRAY", "grau"),
    dekliniere_adjektiv("ADJEKTIV_SPE_WRINKLED", "zerknittert"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DUSTY", "verstaubt"),
    dekliniere_adjektiv("ADJEKTIV_SPE_BRONZE", "bronzen"), # oder 'bronzefarben'?
    dekliniere_adjektiv("ADJEKTIV_SPE_COPPER", "kupfern"), # oder 'kupferrot'?
    dekliniere_adjektiv("ADJEKTIV_SPE_SILVER", "silbern"),
    dekliniere_adjektiv("ADJEKTIV_SPE_GOLD", "golden"),
    dekliniere_adjektiv("ADJEKTIV_SPE_GLITTERING", "gleißend"), # LEO gibt glänzend,glanzvoll, gleißend, glitzernd, verlockend
    dekliniere_adjektiv("ADJEKTIV_SPE_SHINING", "glänzend"), # oder 'blank'?
    dekliniere_adjektiv("ADJEKTIV_SPE_DULL", "matt"), # oder 'stumpf' oder 'trübe'?
    dekliniere_adjektiv("ADJEKTIV_SPE_THIN", "dünn"),
    dekliniere_adjektiv("ADJEKTIV_SPE_THICK", "dick"),
    dekliniere_adjektiv("ADJEKTIV_SPE_CANVAS", "seiden"),
    dekliniere_adjektiv("ADJEKTIV_SPE_HARDCOVER", "gebunden"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PLAIN", "unbedruckt"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PAPYRUS", "papyren"), # Kandidat für 'aus Papyrus'
    "",
    "/* Wands, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_WAND_GLASS", "gläsern"),
    dekliniere_substantiv("MADE_OF_WAND_BALSA", "Balsaholz",  "es", "", "er", "neutrum"),
    dekliniere_adjektiv("ADJEKTIV_WAND_CRYSTAL", "kristallen"),
    dekliniere_substantiv("MADE_OF_WAND_MAPLE", "Ahornholz",  "es", "", "er", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_PINE", "Kiefernholz", "es", "", "er", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_OAK", "Eichenholz", "es", "", "er", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_EBONY", "Ebenholz", "es", "", "er", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_MARBLE", "Marmor", "s", "", "e", "maskulin"),
    dekliniere_substantiv("MADE_OF_WAND_TIN", "Zinn", "es", "", "e", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_BRASS", "Messing", "s", "", "e", "neutrum"),
    dekliniere_adjektiv("ADJEKTIV_WAND_COPPER", "kupfern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_SILVER", "silbern"),
    dekliniere_substantiv("MADE_OF_WAND_PLATINUM", "Platin", "s", "", "s", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_IRIDIUM", "Iridium", "s", "", "en", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_ZINC", "Zink", "es", "", "e", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_ALUMINUM", "Aluminium", "s", "", "", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_URANIUM", "Uran", "s", "", "e", "neutrum"),
    dekliniere_adjektiv("ADJEKTIV_WAND_IRON", "eisern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_STEEL", "stählern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_HEXAGONAL","sechseckig"),
    dekliniere_adjektiv("ADJEKTIV_WAND_SHORT","kurz"),
    dekliniere_adjektiv("ADJEKTIV_WAND_RUNED","runenbeschriftet"),
    dekliniere_adjektiv("ADJEKTIV_WAND_LONG","lang"),
    dekliniere_adjektiv("ADJEKTIV_WAND_CURVED","gebogen"),
    dekliniere_adjektiv("ADJEKTIV_WAND_FORKED","gegabelt"),
    dekliniere_adjektiv("ADJEKTIV_WAND_SPIKED","spitzig"),
    dekliniere_adjektiv("ADJEKTIV_WAND_JEWELED","juwelenbesetzt"),
    "",
    "/* Coins */",
    dekliniere_substantiv("NOUN_GOLD_PIECE","Goldstück","es","Goldstück","e","neutrum"),
    "",
    "/* Gems, identified */",
    dekliniere_substantiv("NOUN_GEM_DILITHIUM_CRYSTAL", "Dilithiumkristall", "es", "Dilithiumkristall", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_DIAMOND", "Diamant", "en", "Diamant", "en", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_RUBY", "Rubin", "s", "Rubin", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_JACINTH", "Hyazinth", "es", "Hyazinth", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_SAPPHIRE", "Saphir", "s", "Saphir", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_BLACK_OPAL", "Schwarzopal", "es", "Schwarzopal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_EMERALD", "Smaragd", "es", "Smaragd", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_TURQUOISE", "Türkis", "es", "Türkis", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_CITRINE", "Zitrin", "s", "Zitrin", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_AQUAMARINE", "Aquamarin", "s", "Aquamarin", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_AMBER", "Bernsteinperle", "", "Bernsteinperle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GEM_TOPAZ", "Topas", "es", "Topas", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_JET", "Gagat", "es", "Gagat", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_OPAL", "Opal", "es", "Opal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_CHRYSOBERYL", "Chrysoberyll", "s", "Chrysoberyll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_GARNET", "Granat", "es", "Granat", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_AMETHYST", "Amethyst", "es", "Amethyst", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_JASPER", "Jaspis", "", "Jaspiss", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_FLUORITE", "Flußspat", "es", "Flußspat", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_OBSIDIAN", "Obsidian", "s", "Obsidian", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_AGATE", "Achat", "es", "Achat", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_JADE", "Jadestein", "es", "Jadestein", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_GEM_WHITE_GLASS", ["wertlos","weiß"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_BLUE_GLASS", ["wertlos","blau"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_RED_GLASS", ["wertlos","rot"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_YELLOWISH_BROWN_GLASS", ["wertlos","gelblichbraun"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_ORANGE_GLASS", ["wertlos","orangen"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_YELLOW_GLASS", ["wertlos","gelb"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_BLACK_GLASS", ["wertlos","schwarz"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_GREEN_GLASS", ["wertlos","grün"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_VIOLET_GLASS", ["wertlos","violett"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_COLORED_GLASS", ["wertlos","gefärbt"], "Glasstück", "es", "Glasstück", "e", "neutrum"),
    dekliniere_substantiv("NOUN_GEM_LUCKSTONE", "Glücksstein", "es", "Glücksstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_LOADSTONE", "Teufelsstein", "es", "Teufelsstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_TOUCHSTONE", "Prüfstein", "es", "Prüfstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_TOUCHSTONE", "Probierstein", "es", "Probierstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_FLINT", "Feuerstein", "es", "Feuerstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_ROCK", "Stein", "es", "Stein", "e", "maskulin"),
    "",
    "/* Gems, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_GEM_WHITE", "weiß"),
    dekliniere_adjektiv("ADJEKTIV_GEM_RED", "rot"),
    dekliniere_adjektiv("ADJEKTIV_GEM_ORANGE", "orangen"),
    dekliniere_adjektiv("ADJEKTIV_GEM_BLUE", "blau"),
    dekliniere_adjektiv("ADJEKTIV_GEM_BLACK", "schwarz"),
    dekliniere_adjektiv("ADJEKTIV_GEM_GREEN", "grün"),
    dekliniere_adjektiv("ADJEKTIV_GEM_YELLOW", "gelb"),
    dekliniere_adjektiv("ADJEKTIV_GEM_YELLOWISH_BROWN", "gelblichbraun"),
    dekliniere_adjektiv("ADJEKTIV_GEM_VIOLET", "violett"),
    dekliniere_adjektiv("ADJEKTIV_GEM_GRAY", "grau"),
    "",
    "/* Other objects, identified */",
    dekliniere_substantiv("NOUN_BOULDER",   "Felsbrocken",    "s",  "Felsbrocken",    "",   "maskulin"),
    dekliniere_substantiv("NOUN_STATUE",    "Statue",         "",   "Statue",         "en", "feminin"),
    dekliniere_nominalphrase("NOUN_HEAVY_IRON_BALL", "schwer", "Eisenkugel", "", "Eisenkugel", "en", "feminin"),
    dekliniere_substantiv("NOUN_IRON_CHAIN", "Eisenkette", "", "Eisenkette", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BLINDING_VENOM", "Blendgift","es","Blendgift","e","neutrum"),
    "",
    "/* Other objects, unidentified */",
    dekliniere_substantiv("NOUN_SPLASH_OF_VENOM", "Spritzer","s","Spritzer","","maskulin", "", "Schlangengift"),
    dekliniere_substantiv("NOUN_ACID_VENOM", "Säuregift","es","Säuregift","e","neutrum"),
    "",
    "/* ======================================================= */",
    "",
    "/* all monster names */",
    dekliniere_substantiv("NOUN_GIANT_ANT","Riesenameise","","Riesenameise","en","feminin","n"),
    dekliniere_substantiv("NOUN_KILLER_BEE","Killerbiene","","Killerbiene","en","feminin","n"),
    dekliniere_substantiv("NOUN_SOLDIER_ANT","Ameisensoldat","en","Ameisensoldat","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_FIRE_ANT","Feuerameise","","Feuerameise","en","feminin", "n"),
    dekliniere_substantiv("NOUN_GIANT_BEETLE", "Riesenkäfer", "s", "Riesenkäfer", "", "maskulin"),
    dekliniere_substantiv("NOUN_QUEEN_BEE","Bienenkönigin","","Bienenköniginn","en","feminin", "nen"),
    dekliniere_substantiv("NOUN_ACID_BLOB", "Säureklumpen", "s", "Säureklumpen", "", "maskulin"), # Säureklumpen?, Säureklotz??
    dekliniere_substantiv("NOUN_QUIVERING_BLOB", "Zitterklumpen", "s", "Zitterklumpen", "", "maskulin"), # zitternder/bebender Klumpen, Zitterklumpen
    dekliniere_substantiv("NOUN_BLOB", "Klumpen", "s", "Klumpen", "", "maskulin"),
    dekliniere_substantiv("NOUN_GELATINOUS_CUBE", "Gelatinewürfel", "s", "Gelatinewürfel", "", "maskulin"),
    dekliniere_substantiv("NOUN_CHICKATRICE", "Kikatrix", "", "Kikatrices", "fremder", "neutrum"), # Kikatrix? nach Adelung niedersächsisch kiken/küken (angelsächsisch cicen, englisch chicken) für Küchlein (neuhochdeutsch Küken)
    dekliniere_substantiv("NOUN_COCKATRICE", "Kokatrix", "", "Kokatrices", "fremder", "maskulin"), # Cocatrix? mittelalterliches Latein -> http://www.bartleby.com/61/43/C0444300.html
    dekliniere_substantiv("NOUN_PYROLISK", "Pyrolisk", "en", "Pyrolisk", "en", "maskulin"),
    dekliniere_substantiv("NOUN_JACKAL", "Schakal", "s", "Schakal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FOX","Fuchs","es","Füchs","e","maskulin"),
    dekliniere_substantiv("NOUN_COYOTE","Kojote","en","Kojote","en","maskulin","n"),
    dekliniere_substantiv("NOUN_WEREJACKAL", "Werschakal", "s", "Werschakal", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_LITTLE_DOG", "klein", "Hund", "es", "Hund", "e", "maskulin","e"),
    dekliniere_substantiv("NOUN_DOG", "Hund", "es", "Hund", "e", "maskulin","e"),
    dekliniere_nominalphrase("NOUN_LARGE_DOG", "groß", "Hund", "es", "Hund", "e", "maskulin","e"),
    dekliniere_substantiv("NOUN_DINGO","Dingo","s","Dingo","s","maskulin"),
    dekliniere_substantiv("NOUN_WOLF", "Wolf", "es", "Wölf", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_WEREWOLF","Werwolf","es","Werwölf","e","maskulin", "s"),
    dekliniere_substantiv("NOUN_WARG", "Warg","", "Warg", "e", "maskulin"),
    dekliniere_adjektivisches_substantiv("NOUN_WINTER_WOLF_CUB", "Schneewolfjung", "neutrum"),
    dekliniere_substantiv("NOUN_WINTER_WOLF", "Schneewolf", "es", "Schneewölf", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_HELL_HOUND_PUP", "Höllenhundwelpe", "en", "Höllenhundwelpe", "en", "maskulin","e"),
    dekliniere_substantiv("NOUN_HELL_HOUND", "Höllenhund", "es", "Höllenhund", "e", "maskulin","e"),
    dekliniere_substantiv("NOUN_CERBERUS", "Zerberus", "", "Zerberuss", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GAS_SPORE", "Gasspore", "", "Gasspore", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_FLOATING_EYE", "Schwebäugler", "s", "Schwebäugler", "", "maskulin"),
    dekliniere_substantiv("NOUN_FLOATING_EYE", "Schwebaug", "es", "Schwebaug", "en", "neutrum", "n"), # Schwebäugler?, Schwebäuger?, was mit Ocellus?, nuräeugler, flugäugler, flugauge,
    dekliniere_substantiv("NOUN_FREEZING_SPHERE", "Frostkugel", "", "Frostkugel", "en", "feminin"), # Frostkugel, Frostsphäre, Frostball
    dekliniere_substantiv("NOUN_FLAMING_SPHERE", "Flammenkugel", "", "Flammenkugel", "en", "feminin"), # Flammenkugel, Flammensphäre, Flammenball
    dekliniere_substantiv("NOUN_SHOCKING_SPHERE", "Elektrokugel", "", "Elektrokugel", "en", "feminin"), # Stosskugel?, Schlagkugel?, Stromkugel? Stromsphäre? Stromball? Elektrokugel?
    dekliniere_substantiv("NOUN_BEHOLDER", "Betrachter", "s", "Betrachter", "", "maskulin"), # Betrachter
    dekliniere_substantiv("NOUN_KITTEN", "Kätzchen", "s", "Kätzchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_HOUSECAT","Hauskatze","","Hauskatze","en","feminin","n"),
    dekliniere_substantiv("NOUN_JAGUAR","Jaguar","s","Jaguar","e","maskulin"),
    dekliniere_substantiv("NOUN_LYNX","Luchs","es","Luchs","e","maskulin"),
    dekliniere_substantiv("NOUN_PANTHER","Panther","s","Panther","","maskulin"),
    dekliniere_nominalphrase("NOUN_LARGE_CAT", "groß", "Katze","","Katze","en","feminin","n"),
    dekliniere_substantiv("NOUN_TIGER","Tiger","s","Tiger","","maskulin"),
    dekliniere_substantiv("NOUN_GREMLIN", "Gremlin", "s", "Gremlin", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GARGOYLE", "Gargyle", "", "Gargyle", "en", "feminin"),
    dekliniere_substantiv("NOUN_GARGOYLE", "Gargoyle", "s", "Gargoyle", "s", "maskulin"), # Gargoyle?, Gargil?, Gargula?, Gargyle?
    dekliniere_substantiv("NOUN_WINGED_GARGOYLE", "Fluggargyle", "", "Fluggargyle", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_WINGED_GARGOYLE", "geflügelt", "Gargoyle", "s", "Gargoyle", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HOBBIT", "Hobbit", "s", "Hobbit", "s", "maskulin"),
    dekliniere_substantiv("NOUN_DWARF","Zwerg","es","Zwerg","e","maskulin", "en"),
    dekliniere_substantiv("NOUN_BUGBEAR", "Grottenschrat","es","Grottenschrat","e","maskulin", "en"),
    dekliniere_substantiv("NOUN_DWARF_LORD","Zwergenfürst","en","Zwergenfürst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_DWARF_KING","Zwergenkönig","es","Zwergenkönig","e","maskulin"),
    dekliniere_substantiv("NOUN_MIND_FLAYER", "Gedankendieb", "es", "Gedankendieb", "e", "maskulin", "s"), # Gedankenschinder, Gedankendieb
    dekliniere_substantiv("NOUN_MASTER_MIND_FLAYER", "Gedankenschinder", "s", "Gedankenschinder", "", "maskulin", "s"), # Superhirnschinder, Gedankenschinder
    dekliniere_substantiv("NOUN_MANES", "Mane", "en", "Mane", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HOMUNCULUS", "Homunculus", "", "Homunculi", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_IMP", "Imp", "", "Imp", "s", "maskulin"),
    dekliniere_substantiv("NOUN_LEMURE", "Lemur", "en", "Lemur", "en", "maskulin"),
    dekliniere_substantiv("NOUN_QUASIT", "Quasit", "", "Quasit", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_TENGU", "Tengu", "", "Tengu", "fremder", "maskulin"),
    dekliniere_nominalphrase("NOUN_BLUE_JELLY", "blau", "Glibber", "s", "", "", "maskulin"), # Glibber, Gallert, Gelle
    dekliniere_nominalphrase("NOUN_SPOTTED_JELLY", "gesprenkelt", "Glibber", "s", "", "", "maskulin"), # gefleckt, gepunktet
    dekliniere_nominalphrase("NOUN_OCHRE_JELLY", "ockergelb", "Glibber", "s", "", "", "maskulin"), # ockergelb, beige, gelb, braun
    dekliniere_substantiv("NOUN_KOBOLD","Kobold","es","Kobold","e","maskulin", "s"),
    dekliniere_nominalphrase("NOUN_LARGE_KOBOLD","groß", "Kobold","es","Kobold","e","maskulin", "s"),
    dekliniere_substantiv("NOUN_KOBOLD_LORD", "Koboldfürst","en","Koboldfürst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_KOBOLD_SHAMAN", "Koboldschamane", "en", "Koboldschamane", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_LEPRECHAUN", "Leprechaun", "", "Leprechaun", "", "maskulin", "en"), # Ampferwichtel?
    dekliniere_nominalphrase("NOUN_SMALL_MIMIC","klein", "Imitator","","","","maskulin", "en"),
    dekliniere_nominalphrase("NOUN_LARGE_MIMIC","groß", "Imitator","","","","maskulin", "en"),
    dekliniere_nominalphrase("NOUN_GIANT_MIMIC","riesig", "Imitator","","","","maskulin", "en"),
		# Riesenimitator wäre etwas irreführend
    # dekliniere_substantiv("NOUN_GIANT_MIMIC", "Riesenimitator","","Riesenimitator","","maskulin", "en"),
    dekliniere_substantiv("NOUN_WOOD_NYMPH",     "Baumnymphe",   "", "Baumnymphe",   "en", "feminin","n"),
    dekliniere_substantiv("NOUN_WATER_NYMPH",    "Wassernymphe", "", "Wassernymphe", "en", "feminin","n"),
    dekliniere_substantiv("NOUN_MOUNTAIN_NYMPH", "Bergnymphe",   "", "Bergnymphe",   "en", "feminin","n"),
    "/* In reality goblin would be an appropriate translation for 'Kobold'. In tolkienspeek goblin is a synonym for orc.*/",
    dekliniere_substantiv("NOUN_GOBLIN", "Goblin", "s", "Goblin", "", "maskulin"),
    dekliniere_substantiv("NOUN_HOBGOBLIN", "Hobgoblin", "s", "Hobgoblin", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORC","Ork","s","Ork","s","maskulin"),
    dekliniere_substantiv("NOUN_HILL_ORC","Hügelork","s","Hügelork","s","maskulin"),
    dekliniere_substantiv("NOUN_MORDOR_ORC","Mordorork","s","Mordorork","s","maskulin"),
    dekliniere_substantiv("NOUN_URUK_HAI","Uruk-Hai","","Uruk-Hai","fremder","maskulin"),
    dekliniere_substantiv("NOUN_ORC_SHAMAN", "Orkschamane", "en", "Orkschamane", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_ORC_CAPTAIN", "Orkkommandant", "en", "Orkkommandant", "en", "maskulin", "en"), # Orkkommandant/hauptmann
    dekliniere_substantiv("NOUN_ROCK_PIERCER", "Steinbohrer", "s", "Steinbohrer", "", "maskulin"),  # Steinbrecher?, Steinbohrer?, Steinschneider?
    dekliniere_substantiv("NOUN_IRON_PIERCER", "Eisenbohrer", "s", "Eisenbohrer", "", "maskulin"),  # Eisenbrecher?, Eisenbohrer?, Eisenschneider?
    dekliniere_substantiv("NOUN_GLASS_PIERCER", "Glasbohrer", "s", "Glasbohrer", "", "maskulin"), # Glasbrecher?, Glasbohrer?, Glasschneider?
    dekliniere_substantiv("NOUN_ROTHE", "Rothé", "", "Rothé", "fremder", "feminin"),
    dekliniere_substantiv("NOUN_MUMAK", "Mûmak", "s", "Mûmakil", "fremder", "neutrum"), # Olifant, Mûmak Mûmakil(pl)
    dekliniere_substantiv("NOUN_LEOCROTTA", "Leocrotta", "", "Leocrottae", "fremder", "neutrum"),  # Leucrocotta? Leucrotta? Plinius 8.Buch der Natur
    dekliniere_substantiv("NOUN_WUMPUS", "Wumpus", "", "Wumpi", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_TITANOTHERE", "Titanotherium", "", "Titanotheria", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_BALUCHITHERIUM", "Baluchitherium", "", "Baluchitheria", "fremder", "neutrum"), # Paraceratherium, Baluchitherium, Indricotherium
    dekliniere_substantiv("NOUN_MASTODON", "Mastodon", "es", "Mastodon", "en", "neutrum"),
    dekliniere_substantiv("NOUN_SEWER_RAT", "Kanalratte", "", "Kanalratte", "en", "feminin", "n"),
    dekliniere_nominalphrase("NOUN_GIANT_RAT", "riesig", "Ratte", "", "Ratte", "en", "feminin", "n"),
    dekliniere_nominalphrase("NOUN_RABID_RAT", "tollwütig", "Ratte", "", "tollwütig", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WERERAT", "Werratte", "", "Werratte", "en", "feminin", "n"),
    #dekliniere_substantiv("NOUN_ROCK_MOLE"
    dekliniere_substantiv("NOUN_WOODCHUCK", "Murmeltier", "es", "Murmeltier", "e", "neutrum"),
    dekliniere_substantiv("NOUN_CAVE_SPIDER", "Höhlenspinne", "", "Höhlenspinne", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_CENTIPEDE", "Tausendfüßler", "s", "Tausendfüßler", "", "maskulin"),
    dekliniere_substantiv("NOUN_GIANT_SPIDER", "Riesenspinne", "", "Riesenspinne", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SCORPION", "Skorpion", "s", "Skorpion", "e", "maskulin"),
    dekliniere_substantiv("NOUN_LURKER_ABOVE", "Laurer", "s", "Laurer", "", "maskulin"),
    dekliniere_substantiv("NOUN_TRAPPER", "Fallensteller", "s", "Fallensteller", "", "maskulin"), 
    dekliniere_nominalphrase("NOUN_WHITE_UNICORN", "weiß", "Einhorn", "es", "Einhörn", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_GRAY_UNICORN", "grau", "Einhorn", "es", "Einhörn", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_BLACK_UNICORN", "schwarz", "Einhorn", "es", "Einhörn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_PONY","Pony","s","Pony","s","neutrum"),
    dekliniere_substantiv("NOUN_HORSE","Pferd","es","Pferd","e","neutrum", "e"),
    dekliniere_substantiv("NOUN_WARHORSE","Schlachtroß","es","Schlachtroß","e","neutrum"),
    dekliniere_substantiv("NOUN_FOG_CLOUD", "Nebelwolke", "", "Nebelwolke", "en", "feminin"),
    dekliniere_substantiv("NOUN_DUST_VORTEX", "Staubwirbel", "s", "Staubwirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_ICE_VORTEX", "Eiswirbel", "s", "Eiswirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_ENERGY_VORTEX", "Energiewirbel", "s", "Energiewirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_STEAM_VORTEX", "Dampfwirbel", "s", "Dampfwirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_FIRE_VORTEX", "Feuerwirbel", "s", "Feuerwirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_BABY_LONG_WORM", "Langwürmchen", "s", "Langwürmchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_PURPLE_WORM", "Purpurwürmchen", "s", "Purpurwürmchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_LONG_WORM", "Langwurm", "es", "Langwürm", "er", "maskulin"),
    dekliniere_substantiv("NOUN_PURPLE_WORM", "Purpurwurm", "es", "Purpurwürm", "er", "maskulin"),
    dekliniere_substantiv("NOUN_GRID_BUG", "Rastertaster", "s", "Rastertaster", "", "maskulin"), # Rastertaster, Matrixviech
    dekliniere_substantiv("NOUN_XAN", "Xan", "", "Xan", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_YELLOW_LIGHT", "gelb", "Licht", "es", "Licht", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_BLACK_LIGHT", "schwarz", "Licht", "es", "Licht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_ZRUTY", "Zruty", "s", "Zruty", "s", "maskulin"),
    dekliniere_substantiv("NOUN_COUATL", "Coatl", "", "Coatl", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_ALEAX", "Aleax", "", "Aleax", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_ANGEL", "Engel", "s", "Engel", "", "maskulin"),
    dekliniere_substantiv("NOUN_KI_RIN", "Kirin", "s", "Kirin", "s", "neutrum"),
    dekliniere_substantiv("NOUN_ARCHON", "Archon", "s", "Archont", "en", "maskulin"),
    dekliniere_substantiv("NOUN_BAT", "Fledermaus", "", "Fledermäus", "e", "feminin"),
    dekliniere_substantiv("NOUN_GIANT_BAT", "Riesenfledermaus", "", "Riesenfledermäus", "e", "feminin"), # von netzhack Flughund? Andererseits sind Giant Bats figuren aus d&d http://en.wikipedia.org/wiki/Giant_bat
    dekliniere_substantiv("NOUN_RAVEN", "Rabe", "en", "Rabe", "en", "maskulin"),
    dekliniere_substantiv("NOUN_VAMPIRE_BAT", "Vampirfledermaus", "", "Vampirfledermäus", "e", "feminin"),
    dekliniere_substantiv("NOUN_PLAINS_CENTAUR", "Steppenkentaur", "en", "Steppenkentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_FOREST_CENTAUR", "Waldkentaur", "en", "Waldkentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_MOUNTAIN_CENTAUR", "Bergkentaur", "en", "Bergkentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_BABY_GRAY_DRAGON", "Graudrachenbaby", "s", "Graudrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_SILVER_DRAGON", "Silberdrachenbaby", "s", "Silberdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_SHIMMERING_DRAGON", "Glanzdrachenbaby", "s", "Glanzdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_RED_DRAGON", "Rotdrachenbaby", "s", "Rotdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_WHITE_DRAGON", "Weißdrachenbaby", "s", "Weißdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_ORANGE_DRAGON", "Orangedrachenbaby", "s", "Orangedrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_BLACK_DRAGON", "Schwarzdrachenbaby", "s", "Schwarzdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_BLUE_DRAGON", "Blaudrachenbaby", "s", "Blaudrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_GREEN_DRAGON", "Gründrachenbaby", "s", "Gründrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_YELLOW_DRAGON", "Gelbdrachenbaby", "s", "Gelbdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_GRAY_DRAGON", "Graudrache", "en", "Graudrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_SILVER_DRAGON", "Silberdrache", "en", "Silberdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_SHIMMERING_DRAGON", "Glanzdrache", "en", "Glanzdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_RED_DRAGON", "Rotdrache", "en", "Rotdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_WHITE_DRAGON", "Weißdrache", "en", "Weißdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_ORANGE_DRAGON", "Orangedrache", "en", "Orangedrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BLACK_DRAGON", "Schwarzdrache", "en", "Schwarzdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BLUE_DRAGON", "Blaudrache", "en", "Blaudrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_GREEN_DRAGON", "Gründrache", "en", "Gründrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_YELLOW_DRAGON", "Gelbdrache", "en", "Gelbdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_STALKER", "Pirscher", "s", "Pirscher", "", "maskulin"), # Schleicher?
    # dekliniere_substantiv("NOUN_STALKER", "Nachsteller", "s", "Nachsteller", "", "maskulin"),
    dekliniere_substantiv("NOUN_ELEMENTAL",   "Elementar",   "es", "Elementar",   "e", "neutrum"),
    dekliniere_substantiv("NOUN_AIR_ELEMENTAL",   "Luftelementar",   "es", "Luftelementar",   "e", "neutrum"),
    dekliniere_substantiv("NOUN_FIRE_ELEMENTAL",  "Feuerelementar",  "es", "Feuerelementar",  "e", "neutrum"),
    dekliniere_substantiv("NOUN_EARTH_ELEMENTAL", "Erdelementar",    "es", "Erdelementar",    "e", "neutrum"),
    dekliniere_substantiv("NOUN_WATER_ELEMENTAL", "Wasserelementar", "es", "Wasserelementar", "e", "neutrum"),
    dekliniere_substantiv("NOUN_LICHEN", "Flechte", "", "Flechte", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BROWN_MOLD", "Braunschimmel", "s", "Braunschimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_YELLOW_MOLD", "Gelbschimmel", "s", "Gelbschimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_GREEN_MOLD", "Grünschimmel", "s", "Grünschimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_RED_MOLD", "Rotschimmel", "s", "Rotschimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SHRIEKER","Krakeeler", "s", "Krakeeler","", "maskulin"), # kreischen, schreien: Kreischer, Schreier, Schreihals, Krakeeler
    dekliniere_nominalphrase("NOUN_VIOLET_FUNGUS", "violett", "Pilz", "es", "Pilz", "e", "maskulin", ""),
    dekliniere_substantiv("NOUN_GNOME", "Gnom", "en", "Gnom", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_GNOME_LORD","Gnomenfürst","en","Gnomenfürst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_GNOMISH_WIZARD", "Gnomenzauberer", "s", "Gnomenzauberer", "", "maskulin"),
    dekliniere_substantiv("NOUN_GNOME_KING","Gnomenkönig","es","Gnomenkönig","e","maskulin"),
    dekliniere_substantiv("NOUN_GIANT",       "Riese", "en", "Riese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_STONE_GIANT", "Steinriese", "en", "Steinriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_HILL_GIANT",  "Hügelriese", "en", "Hügelriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_FIRE_GIANT",  "Feuerriese", "en", "Feuerriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_FROST_GIANT", "Frostriese", "en", "Frostriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_STORM_GIANT", "Sturmriese", "en", "Sturmriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_ETTIN", "Ettin", "", "Ettin", "", "maskulin"),
    dekliniere_substantiv("NOUN_TITAN", "Titane", "en", "Titan", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_MINOTAUR","Minotaurus","","Minotaur","en","maskulin"),
    dekliniere_substantiv("NOUN_JABBERWOCK", "Jammerwoch", "s", "Jammerwoch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_VORPAL_JABBERWOCK", "Vorpahljammerwoch", "s", "Vorpahljammerwoch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_KEYSTONE_KOP", "Karabiniere", "s", "Karabinieri", "fremder", "maskulin"), 
    dekliniere_substantiv("NOUN_KOP_SERGEANT", "Karabiniere", "s", "Karabinieri", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_KOP_LIEUTENANT", "Brigadiere", "", "Brigadieri", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_KOP_KAPTAIN", "Kapitano", "", "Kapitani", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_LICH", "Liechnam", "s", "Liechnam", "e", "maskulin", "en"), # Liechnam?, Leichnam? Lichnam? Liichnam? Lijchnam? Liehchnam?
    dekliniere_substantiv("NOUN_DEMILICH", "Halbliechnam", "s", "Halbliechnam", "e", "maskulin", "en"), # Demiliechnam?, Halbliechnam?
    dekliniere_substantiv("NOUN_MASTER_LICH", "Meisterliechnam", "s", "Meisterliechnam", "e", "maskulin", "en"), # Meisterliechnam
    dekliniere_substantiv("NOUN_ARCH_LICH", "Erzliechnam", "s", "Erzliechnam", "e", "maskulin", "en"),
    dekliniere_substantiv("NOUN_KOBOLD_MUMMY", "Koboldsmumie", "", "Koboldsmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GNOME_MUMMY", "Gnomenmumie", "", "Gnomenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ORC_MUMMY", "Orkmumie", "", "Orkmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_DWARF_MUMMY", "Zwergenmumie", "", "Zwergenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ELF_MUMMY", "Elbenmumie", "", "Elbenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_HUMAN_MUMMY", "Menschenmumie", "", "Menschenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ETTIN_MUMMY", "Ettinmumie", "", "Ettinmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GIANT_MUMMY", "Riesenmumie", "", "Riesenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RED_NAGA_HATCHLING", "Rotnagaschlüpfling", "s", "Rotnagaschlüpfling", "s", "feminin"), # Rotnagaküken, Rotnagaschlüpfling
    dekliniere_substantiv("NOUN_BLACK_NAGA_HATCHLING", "Schwarznagaschlüpfling", "s", "Schwarznagaschlüpfling", "s", "feminin"),
    dekliniere_substantiv("NOUN_GOLDEN_NAGA_HATCHLING", "Goldnagaschlüpfling", "s", "Goldnagaschlüpfling", "s", "feminin"),
    dekliniere_substantiv("NOUN_GUARDIAN_NAGA_HATCHLING", "Schutznagaschlüpfling", "s", "Schutznagaschlüpfling", "s", "feminin"),
    dekliniere_substantiv("NOUN_RED_NAGA", "Rotnaga", "s", "Rotnaga", "s", "feminin"), # Rotnaga, rote Naga, roter Naga?
    dekliniere_substantiv("NOUN_BLACK_NAGA", "Schwarznaga", "s", "Schwarznaga", "s", "feminin"), 
    dekliniere_substantiv("NOUN_GOLDEN_NAGA", "Goldnaga", "s", "Goldnaga", "s", "feminin"),
    dekliniere_substantiv("NOUN_GUARDIAN_NAGA", "Schutznaga", "s", "Schutznaga", "s", "feminin"), # Wächternaga, Hüternaga, Schutznaga
    dekliniere_substantiv("NOUN_NAGA", "Naga", "s", "Naga", "s", "feminin"), 
    dekliniere_substantiv("NOUN_OGRE", "Oger", "s", "Oger", "", "maskulin"),
    dekliniere_substantiv("NOUN_OGRE_LORD","Ogerfürst","en","Ogerfürst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_OGRE_KING","Ogerkönig","es","Ogerkönig","e","maskulin"),
    dekliniere_nominalphrase("NOUN_GRAY_OOZE", "Grau", "Schlamm", "es", "Schlamm", "e", "maskulin"), # grauer Schleim, Schlick?
    dekliniere_nominalphrase("NOUN_BROWN_PUDDING", "Braun", "Grütze", "", "Grütze", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_BLACK_PUDDING", "Schwarz", "Grütze", "", "Grütze", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_GREEN_SLIME", "Grün", "Schleim", "es", "Schleim", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_QUANTUM_MECHANIC"
    dekliniere_substantiv("NOUN_RUST_MONSTER", "Rostmonster", "s", "Rostmonster", "", "neutrum"),
    dekliniere_substantiv("NOUN_DISENCHANTER", "Entzauberer", "s", "Entzauberer", "", "maskulin"),
    #dekliniere_substantiv("NOUN_GARTER_SNAKE"
    dekliniere_substantiv("NOUN_SNAKE","Schlange","","Schlange","en","feminin", "n"),
    #dekliniere_substantiv("NOUN_WATER_MOCCASIN"
    #dekliniere_substantiv("NOUN_PIT_VIPER"
    dekliniere_substantiv("NOUN_PYTHON", "Python", "s", "Python", "s", "feminin"),
    dekliniere_substantiv("NOUN_COBRA", "Kobra", "", "Kobra", "s", "feminin"),
    dekliniere_substantiv("NOUN_TROLL", "Troll",  "es", "Troll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ICE_TROLL", "Eistroll",  "es", "Eistroll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ROCK_TROLL", "Steintroll",  "es", "Steintroll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WATER_TROLL", "Wassertroll",  "es", "Wassertroll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_OLOG_HAI","Olog-Hai","","Olog-Hai","fremder","maskulin"),
    dekliniere_substantiv("NOUN_UMBER_HULK", "Umberholk", "es", "Umberholk", "e","maskulin", "en"), # Erdkoloss, Umbraunkoloss?, Umberholk?, Umbraholk?, Umbraunholk? Umbrauner Holk? Umbrauner Koloss?
    # dekliniere_nominalphrase("NOUN_UMBER_HULK", "Umbraun", "Holk", "es", "Holk", "e","maskulin", "en"),
    dekliniere_substantiv("NOUN_VAMPIRE","Vampir","es","Vampir","e","maskulin"),
    #dekliniere_substantiv("NOUN_VAMPIRE_LORD"
    #dekliniere_substantiv("NOUN_VAMPIRE_MAGE"
		[unregelmaessiges_wort("NOUN_VLAD_THE_IMPALER", "Vlad der Pfähler", $nom, $mal, $sg)],
		[unregelmaessiges_wort("NOUN_VLAD_THE_IMPALER", "Vlad des Pfählers", $gen, $mal, $sg)],
		[unregelmaessiges_wort("NOUN_VLAD_THE_IMPALER", "Vlad dem Pfähler", $dat, $mal, $sg)],
		[unregelmaessiges_wort("NOUN_VLAD_THE_IMPALER", "Vlad den Pfähler", $akk, $mal, $sg)],
    #dekliniere_substantiv("NOUN_BARROW_WIGHT" # Grabunhold? , Wight Gruftschrecken
    #dekliniere_substantiv("NOUN_WRAITH" # Todesalb
    dekliniere_substantiv("NOUN_NAZGUL","Nazgûl","","Nazgûl","","maskulin"),
    dekliniere_substantiv("NOUN_XORN","Xorn","","Xorn","","maskulin","en"),
    #dekliniere_substantiv("NOUN_MONKEY", "Affe", "en", "Affe", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_APE", "Menschenaffe", "en", "Menschenaffe", "en", "maskulin"), # einer Menschenaffen stattdessen? Gibbon, Orang-Utan, Gorilla, Schimpanse. Gorilla scheint passend (ist auch der Vetegarischste der Menschenaffen)
    dekliniere_substantiv("NOUN_OWLBEAR", "Eulenbär", "en", "Eulenbär", "en", "maskulin"), # Netzhack: Uhlenbär
    dekliniere_substantiv("NOUN_YETI","Yeti","s","Yeti","s","maskulin"),
    #dekliniere_substantiv("NOUN_CARNIVOROUS_APE"
    # dekliniere_substantiv("NOUN_SASQUATCH", "Sasquatch", "", "Sasquatch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SASQUATCH", "Sasquatch", "", "Sasquatches", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_KOBOLD_ZOMBIE", "Koboldszombie", "s", "Koboldszombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GNOME_ZOMBIE", "Gnomenzombie", "s", "Gnomenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_ORC_ZOMBIE", "Orkzombie", "s", "Orkzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_DWARF_ZOMBIE", "Zwergenzombie", "s", "Zwergenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_ELF_ZOMBIE", "Elbenzombie", "s", "Elbenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HUMAN_ZOMBIE", "Menschenzombie", "s", "Menschenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_ETTIN_ZOMBIE", "Ettinzombie", "s", "Ettinzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GIANT_ZOMBIE", "Riesenzombie", "s", "Riesenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GHOUL", "Ghul", "s", "Ghul", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SKELETON", "Skelett", "es", "Skelett", "e", "neutrum"),
    dekliniere_substantiv("NOUN_STRAW_GOLEM", "Strohgolem", "s", "Strohgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_PAPER_GOLEM", "Papiergolem", "s", "Papiergolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_ROPE_GOLEM", "Seilgolem", "s", "Seilgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GOLD_GOLEM", "Goldgolem", "s", "Goldgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_LEATHER_GOLEM", "Ledergolem", "s", "Ledergolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_WOOD_GOLEM", "Holzgolem", "s", "Holzgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_FLESH_GOLEM", "Fleischgolem", "s", "Fleischgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_CLAY_GOLEM", "Lehmgolem", "s", "Lehmgolem", "s", "maskulin"),
    # dekliniere_substantiv("NOUN_CLAY_GOLEM", "Tongolem", "s", "Tongolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_STONE_GOLEM", "Steingolem", "s", "Steingolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GLASS_GOLEM", "Glasgolem", "s", "Glasgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_IRON_GOLEM", "Eisengolem", "s", "Eisengolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HUMAN", "Mensch", "en", "Mensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_ELF", "Elb", "es", "Elb", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_WOODLAND_ELF", "Waldelb", "es", "Waldelb", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_GREEN_ELF", "Grünelb", "es", "Grünelb", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_GREY_ELF", "Grauelb", "es", "Grauelb", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_ELF_LORD", "Elbenfürst", "en", "Elbenfürst", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_ELVENKING", "Elbenkönig", "es", "Elbenkönig", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_DOPPELGANGER", "Doppelgänger", "s", "Doppelgänger", "", "maskulin"),
    dekliniere_substantiv("NOUN_NURSE", "Krankenschwester", "", "Krankenschwester", "en", "feminin"),
    dekliniere_substantiv("NOUN_SHOPKEEPER", "Ladenbesitzer", "s", "Ladenbesitzer", "", "maskulin"), # Ladenbesitzer, Ladeninhaber, Händler?
    dekliniere_substantiv("NOUN_SHOPKEEPERIN", "Ladenbesitzerin", "", "Ladenbesitzerinn", "en", "feminin", "nen"), 
    dekliniere_substantiv("NOUN_GUARD", "Wache", "", "Wache", "en", "feminin"),
    #dekliniere_substantiv("NOUN_PRISONER"
    dekliniere_substantiv("NOUN_ORACLE", "Orakel", "s", "Orakel", "", "neutrum"),
    #dekliniere_substantiv("NOUN_ALIGNED_PRIEST"
    dekliniere_substantiv("NOUN_HIGH_PRIEST", "Hohepriester", "s", "Hohepriester", "", "maskulin"),
    dekliniere_substantiv("NOUN_HIGH_PRIESTESS", "Hohepriesterin", "", "Hohepriesterinn", "en", "feminin","nen"),
    dekliniere_substantiv("NOUN_SOLDIER", "Soldat", "en", "Soldat", "en", "maskulin", "en"),
    "/* 'Korporal' exists as a military term in the Austrian 'Bundesheer' and\nthe Swiss Army. "+
    "'Leutnant' and 'Hauptmann' exist in these too and also in\nthe German 'Bundeswehr'.\n"+
    "Although not referring to the exactly same rank. */",
    dekliniere_substantiv("NOUN_SERGEANT", "Korporal", "s", "Korporal", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_LIEUTENANT", "Leutnant", "s", "Leutnant", "s", "maskulin", "s"),
    # dekliniere_substantiv("NOUN_CAPTAIN", "Hauptmann", "es", "Hauptmänn", "er", "maskulin", "s"),
    dekliniere_substantiv("NOUN_CAPTAIN", "Hauptmann", "es", "Hauptleut", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_WATCHMAN", "Wachmann", "es", "Wachmänn", "er", "maskulin", "s"),
    dekliniere_substantiv("NOUN_WATCH_CAPTAIN", "Wachkommandant", "en", "Wachkommandant", "en", "maskulin", "en"),
    dekliniere_eigenname("NOUN_MEDUSA", "Medusa", "feminin"),
    #dekliniere_substantiv("NOUN_MEDUSA", "Medusa", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_WIZARD_OF_YENDOR","Zauberer", "s", "Zauberer", "", "maskulin","","von Yendor"),
    dekliniere_substantiv("NOUN_CROESUS", "Krösus", "es", "Krösus", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_CHARON"
    dekliniere_substantiv("NOUN_GHOST", "Geist", "es", "Geist", "er", "maskulin", "er"),
    dekliniere_substantiv("NOUN_SHADE", "Nachtschatten", "s", "Nachtschatten", "", "maskulin"),
    #dekliniere_substantiv("NOUN_WATER_DEMON" # Wasserteufelm Wasserdämon
    #dekliniere_substantiv("NOUN_HORNED_DEVIL" # Hornteufel Malebranche
    dekliniere_substantiv("NOUN_SUCCUBUS", "Sukkubus", "", "Sukkuben", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_INCUBUS", "Incubus", "", "Incuben", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_ERINYS", "Erinnye", "", "Erinnye", "en", "feminin", "n"),
    #dekliniere_substantiv("NOUN_BARBED_DEVIL"
    #dekliniere_substantiv("NOUN_MARILITH" # die Marilith
    #dekliniere_substantiv("NOUN_VROCK"
    #dekliniere_substantiv("NOUN_HEZROU" # der Heztou?
    #dekliniere_substantiv("NOUN_BONE_DEVIL" # Knochenteufel
    #dekliniere_substantiv("NOUN_ICE_DEVIL"  # Eisteufel Gelugon
    #dekliniere_substantiv("NOUN_NALFESHNEE"
    #dekliniere_substantiv("NOUN_PIT_FIEND" # Ice Fiend - Eisunhold? / Eisdämon?
    #dekliniere_substantiv("NOUN_BALROG"
    #dekliniere_substantiv("NOUN_JUIBLEX"
    #dekliniere_substantiv("NOUN_YEENOGHU"
    #dekliniere_substantiv("NOUN_ORCUS"
    #dekliniere_substantiv("NOUN_GERYON"
    #dekliniere_substantiv("NOUN_DISPATER" # Dispater, Eigenname
    #dekliniere_substantiv("NOUN_BAALZEBUB"
    #dekliniere_substantiv("NOUN_ASMODEUS"
    #dekliniere_substantiv("NOUN_DEMOGORGON"
    dekliniere_substantiv("NOUN_DEATH","Tod","es","Tod","e","maskulin"),
    dekliniere_substantiv("NOUN_PESTILENCE","Pest","","","","feminin"),
    dekliniere_substantiv("NOUN_FAMINE","Hunger","s","","","maskulin"),
    #dekliniere_substantiv("NOUN_MAIL_DAEMON"
    dekliniere_substantiv("NOUN_DJINNI", "Dschinn", "es", "Dschinn", "", "maskulin"),
    #dekliniere_substantiv("NOUN_SANDESTIN"
    dekliniere_substantiv("NOUN_JELLYFISH", "Qualle", "", "Qualle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_PIRANHA","Piranha","s","Piranha","s","maskulin"),
    dekliniere_substantiv("NOUN_SHARK","Hai","es","Hai","e","maskulin"),
    dekliniere_substantiv("NOUN_GIANT_EEL", "Riesenaal", "es", "Riesenaal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ELECTRIC_EEL", "Zitteraal", "es", "Zitteraal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_KRAKEN", "Kraken", "en", "Krake", "en", "maskulin","n"),
    dekliniere_substantiv("NOUN_NEWT", "Molch","es","Molch","e","maskulin", "s"),
    dekliniere_substantiv("NOUN_GECKO", "Gecko", "s", "Gecko", "s", "maskulin"),
    dekliniere_substantiv("NOUN_IGUANA", "Leguan", "s", "Leguan", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BABY_CROCODILE","Babykrokodil","s","Babykrokodil","e","neutrum"),
    dekliniere_substantiv("NOUN_LIZARD", "Eidechse", "", "Eidechse", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_CHAMELEON", "Chamäleon", "s", "Chamäleon", "s", "neutrum"),
    dekliniere_substantiv("NOUN_CROCODILE","Krokodil","s","Krokodil","e","neutrum"),
    dekliniere_substantiv("NOUN_SALAMANDER", "Salamander", "s", "Salamander", "", "maskulin"),
    #dekliniere_substantiv("NOUN_LONG_WORM_TAIL"
    dekliniere_substantiv("NOUN_ARCHAEOLOGE", "Archäologe", "en", "Archäologe", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_ARCHAEOLOGIN", "Archäologin", "", "Archäologinn", "en", "feminin"),
    dekliniere_substantiv("NOUN_BARBAR", "Barbar", "en", "Barbar", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_BARBARIN", "Barbarin", "", "Barbarinn", "en", "feminin"),
    dekliniere_substantiv("NOUN_HOEHLENMENSCH", "Höhlenmensch", "en", "Höhlenmensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_CAVEMAN", "Höhlenmensch", "en", "Höhlenmensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_CAVEWOMAN", "Höhlenmensch", "en", "Höhlenmensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_HEILER","Heiler","s","","","maskulin"),
    dekliniere_substantiv("NOUN_HEILERIN","Heilerin","","","","feminin"),
    dekliniere_substantiv("NOUN_KNIGHT", "Paladin","s","Paladin","e","maskulin"),
    dekliniere_substantiv("NOUN_PALADIN","Paladin","s","Paladin","e","maskulin"),
    dekliniere_substantiv("NOUN_MONK",  "Mönch", "es", "Mönch", "e", "maskulin", "en"),
    dekliniere_substantiv("NOUN_MOENCH","Mönch", "es", "Mönch", "e", "maskulin", "en"),
    dekliniere_substantiv("NOUN_KANDIDATIN", "Kandidatin", "", "Kandidatin", "en", "feminin"),
    dekliniere_substantiv("NOUN_PRIEST", "Priester", "s", "Priester", "", "maskulin"),
    dekliniere_substantiv("NOUN_PRIESTESS", "Priesterin", "", "Priesterinn", "en", "feminin","nen"),
    dekliniere_substantiv("NOUN_PRIESTER", "Priester", "s", "Priester", "", "maskulin"),
    dekliniere_substantiv("NOUN_PRIESTERIN", "Priesterin", "", "Priesterinn", "en", "feminin","nen"),
    dekliniere_substantiv("NOUN_RANGER","Waldläufer","s","Waldläufer","","maskulin"),
    dekliniere_substantiv("NOUN_WALDLAEUFER","Waldläufer","s","Waldläufer","","maskulin"),
    dekliniere_substantiv("NOUN_ROGUE","Dieb","es","Dieb","e","maskulin"),
    dekliniere_substantiv("NOUN_DIEB", "Dieb","es","Dieb","e","maskulin"),
    dekliniere_substantiv("NOUN_DIEBIN","Diebin","","Diebinn","en","feminin","nen"),
    dekliniere_substantiv("NOUN_SAMURAI", "Samurai", "", "Samurai", "", "maskulin"),
    dekliniere_substantiv("NOUN_SAMURAIIN", "Samurai", "", "Samurai", "", "feminin"),
    dekliniere_substantiv("NOUN_TOURIST", "Tourist", "en", "Tourist", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_TOURISTIN", "Touristin", "", "Touristinn", "en", "feminin", "nen"),
    dekliniere_substantiv("NOUN_VALKYRIE", "Walküre", "", "Walküre", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WIZARD","Magier","s","Magier","","maskulin"),
    #dekliniere_substantiv("NOUN_LORD_CARNARVON"
    #dekliniere_substantiv("NOUN_PELIAS"
    #dekliniere_substantiv("NOUN_SHAMAN_KARNOV"
    #dekliniere_substantiv("NOUN_EARENDIL"
    #dekliniere_substantiv("NOUN_ELWING"
    #dekliniere_substantiv("NOUN_HIPPOCRATES"
    #dekliniere_substantiv("NOUN_KING_ARTHUR"
    dekliniere_substantiv("NOUN_GRAND_MASTER", "Großmeister", "s", "Großmeister", "", "maskulin"), 
    # dekliniere_substantiv("NOUN_ARCH_PRIEST", "Erzpriester", "s", "Erzpriester", "", "maskulin),
    dekliniere_substantiv("NOUN_ARCH_PRIEST", "Archipresbyter", "s", "Archipresbyter", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORION", "Orion", "s", "", "", "maskulin"),
    #dekliniere_substantiv("NOUN_MASTER_OF_THIEVES"
    #dekliniere_substantiv("NOUN_LORD_SATO"
    #dekliniere_substantiv("NOUN_TWOFLOWER"
    #dekliniere_substantiv("NOUN_NORN"
		[unregelmaessiges_wort("NOUN_NEFERET_THE_GREEN", "Neferet die Grüne", $nom, $fem, $sg)],
		[unregelmaessiges_wort("NOUN_NEFERET_THE_GREEN", "Neferet der Grünen", $gen, $fem, $sg)],
		[unregelmaessiges_wort("NOUN_NEFERET_THE_GREEN", "Neferet der Grünen", $dat, $fem, $sg)],
		[unregelmaessiges_wort("NOUN_NEFERET_THE_GREEN", "Neferet die Grüne", $akk, $fem, $sg)],
    #dekliniere_substantiv("NOUN_MINION_OF_HUHETOTL"
    #dekliniere_substantiv("NOUN_THOTH_AMON"
    #dekliniere_substantiv("NOUN_CHROMATIC_DRAGON"
    dekliniere_substantiv("NOUN_GOBLIN_KING","Goblinkönig","es","Goblinkönig","e","maskulin"),
    #dekliniere_substantiv("NOUN_CYCLOPS"
    #dekliniere_substantiv("NOUN_IXOTH"
    #dekliniere_substantiv("NOUN_MASTER_KAEN"
    #dekliniere_substantiv("NOUN_NALZOK"
    #dekliniere_substantiv("NOUN_SCORPIUS"
    #dekliniere_substantiv("NOUN_MASTER_ASSASSIN"
    #dekliniere_substantiv("NOUN_ASHIKAGA_TAKAUJI"
    #dekliniere_substantiv("NOUN_LORD_SURTUR"
    #dekliniere_substantiv("NOUN_DARK_ONE"
    #dekliniere_substantiv("NOUN_STUDENT"
    #dekliniere_substantiv("NOUN_CHIEFTAIN"
    #dekliniere_substantiv("NOUN_NEANDERTHAL"
    #dekliniere_substantiv("NOUN_HIGH_ELF"
    #dekliniere_substantiv("NOUN_ATTENDANT"
    #dekliniere_substantiv("NOUN_PAGE"
    #dekliniere_substantiv("NOUN_ABBOT"
    #dekliniere_substantiv("NOUN_ACOLYTE"
    #dekliniere_substantiv("NOUN_HUNTER"
    #dekliniere_substantiv("NOUN_THUG"
    #dekliniere_substantiv("NOUN_NINJA"
    #dekliniere_substantiv("NOUN_ROSHI"
    #dekliniere_substantiv("NOUN_GUIDE"
    #dekliniere_substantiv("NOUN_WARRIOR"
    #dekliniere_substantiv("NOUN_APPRENTICE"
    "",
    "/* ======================================================= */",
    "/* drawing.c */",
    dekliniere_substantiv("NOUN_AIR","Luft","","Lüft","e","feminin","n"),
    dekliniere_substantiv("NOUN_ANT","Ameise","","Ameise","en","feminin","n"),
    dekliniere_substantiv("NOUN_ARACHNID","Spinnentier","es","Spinnentier","e","neutrum"),
    dekliniere_substantiv("NOUN_BEAR_TRAP", "Bärenfalle", "", "Bärenfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BIRD","Vogel","s","Vögel","","maskulin",""),
    dekliniere_substantiv("NOUN_CAT","Katze","","Katze","en","feminin","n"),
    dekliniere_substantiv("NOUN_CLOUD","Wolke","","Wolke","en","neutrum","n"),
    dekliniere_substantiv("NOUN_CENTAUR", "Kentaur", "en", "Kentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_CORRIDOR", "Korridor", "s", "Korridor", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DOORWAY", "Eingang", "es", "Eingang", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_DRAGON", "Drache", "en", "Drache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_FIRE_TRAP", "Feuerfalle", "", "Feuerfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GOLEM", "Golem", "s", "Golem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GRAVE", "Grab", "es", "Gräb", "er", "neutrum"),
    dekliniere_substantiv("NOUN_INSECT", "Insekt", "es", "Insekt", "en", "neutrum"),
    dekliniere_substantiv("NOUN_JELLY", "Glibber", "s", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_HOLE", "Loch", "es", "Löch", "er", "neutrum"),
    dekliniere_substantiv("NOUN_ICE", "Eis", "es", "Eis", "e", "neutrum"),
    dekliniere_substantiv("NOUN_IRON_BAR", "Eisenstange", "", "Eisenstange", "en", "feminin"),
    dekliniere_substantiv("NOUN_LADDER", "Leiter", "", "Leiter", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_LIT_CORRIDOR", "erleuchtet", "Korridor", "s", "Korridor", "e", "maskulin"),
    dekliniere_substantiv("NOUN_MIMIC", "Imitator","","","","maskulin", "en"),
    dekliniere_substantiv("NOUN_MOLD", "Schimmel", "s", "Schimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_MUMMY", "Mumie", "", "Mumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_NYMPH", "Nymphe", "", "Nymphe", "en", "feminin","n"),
    dekliniere_substantiv("NOUN_PIERCER", "Bohrer", "s", "Bohrer", "", "maskulin"),
    dekliniere_substantiv("NOUN_PIT", "Grube", "", "Grube", "en", "feminin"),
    dekliniere_substantiv("NOUN_PUDDING", "Grütze", "", "Grütze", "en", "feminin"),
    dekliniere_substantiv("NOUN_POLYMORPH_TRAP", "Transformationsfalle", "", "Transformationsfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_QUADRUPED", "Vierbeiner", "s", "Vierbeiner", "", "maskulin"),
    #dekliniere_substantiv("NOUN_ROCK", "Stein", "es", "Stein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RODENT", "Nagetier", "es", "Nagetier", "e", "neutrum"),
    dekliniere_substantiv("NOUN_ROLLING_BOULDER_TRAP", "Felskugelfalle", "", "Felskugelfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RUST_TRAP", "Rostfalle", "", "Rostfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SEA_MONSTER", "Seemonster", "s", "Seemonster", "", "neutrum"),
    dekliniere_substantiv("NOUN_SINK", "Spüle", "", "Spüle", "en", "feminin"),
    dekliniere_substantiv("NOUN_SLEEPING_GAS_TRAP", "Schlafgasfalle", "", "Schlafgasfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SPHERE", "Kugel", "", "Kugel", "en", "feminin"),
    dekliniere_substantiv("NOUN_SPIKED_PIT", "Stachelgrube", "", "Stachelgrube", "en", "feminin"),
    dekliniere_substantiv("NOUN_TELEPORTATION_TRAP", "Teleportationsfalle", "", "Teleportationsfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_TRAP_DOOR", "Falltüre", "", "Falltüre", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_TREE", "Baum", "es", "Bäum", "e", "maskulin", "n"),
    dekliniere_substantiv("NOUN_UNICORN", "Einhorn", "es", "Einhörn", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_USEFUL_ITEM", "nützlich", "Gerät", "es", "Gerät", "e", "neutrum"),
    dekliniere_substantiv("NOUN_VORTEX", "Wirbel", "s", "Wirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_WATER", "Wasser", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_ZOMBIE", "Zombie", "s", "Zombie", "s", "maskulin"),
    "",
    "/* ========== */",
    "/* Artefakte  */",
    "/* ========== */",
    dekliniere_substantiv("NOUN_EXCALIBUR","Excalibur","","","","neutrum"),
    dekliniere_substantiv("NOUN_STORMBRINGER","Sturmbringer","s","","","maskulin"),
    dekliniere_substantiv("NOUN_MJOLLNIR","Mjöllnir","","","","maskulin"),
    dekliniere_substantiv("NOUN_MJOLLNIR","Mjölnir","","","","maskulin"),
    dekliniere_substantiv("NOUN_CLEAVER","Fleischspalter","s","","","maskulin"), # Fleicherbeil, Hackbeil, Fleischspalter
    dekliniere_substantiv("NOUN_GRIMTOOTH","Grimmzahn","es","","","maskulin"),
    dekliniere_substantiv("NOUN_ORCRIST","Orkrist","","","","neutrum"),
    dekliniere_substantiv("NOUN_ORCRIST","Orcrist","","","","neutrum"),
    dekliniere_substantiv("NOUN_STING","Stich","","","","maskulin"),
    #dekliniere_substantiv("NOUN_MAGICBANE","Magietöter/Magiebann/Magiebanner?","","","",""),
    #dekliniere_substantiv("NOUN_FROST_BRAND","","","","",""),
    #dekliniere_substantiv("NOUN_FIRE_BRAND","","","","",""),
    dekliniere_substantiv("NOUN_DRAGONBANE","Drachentöter","s","","","maskulin"),
    dekliniere_substantiv("NOUN_DEMONBANE","Dämonentöter","s","","","maskulin"),
    dekliniere_substantiv("NOUN_WEREBANE","Wertöter","s","","","maskulin"),
    dekliniere_substantiv("NOUN_GRAYSWANDIR","Grayswandir","","","","neutrum"),
    #dekliniere_substantiv("NOUN_GIANTSLAYER","","","","",""), # Riesenschlächter
    dekliniere_substantiv("NOUN_OGRESMASHER","Ogermantscher","","","","maskulin"),
    dekliniere_substantiv("NOUN_TROLLSBANE","Trolltöter","s","","","maskulin"),
    dekliniere_substantiv("NOUN_VORPAL_BLADE","Vorpahlklinge","","","","feminin"),
    dekliniere_substantiv("NOUN_SNICKERSNEE","Schnippelschnipp","es","","","neutrum"), # EAV
    dekliniere_substantiv("NOUN_SNICKERSNEE","Snickersnee","","","","neutrum"),
    dekliniere_substantiv("NOUN_SUNSWORD","Sonnenschwert","es","","","neutrum"),
    dekliniere_substantiv("NOUN_THE_ORB_OF_DETECTION","Kristallkugel","","","","feminin","","der Enthüllung"),
    dekliniere_substantiv("NOUN_THE_HEART_OF_AHRIMAN","Herz","ens","","","neutrum","","von Ahriman"),
    dekliniere_substantiv("NOUN_THE_SCEPTRE_OF_MIGHT","Szepter","s","","","neutrum","","der Macht"),
    dekliniere_substantiv("NOUN_THE_SCEPTRE_OF_MIGHT","Zepter","s","","","neutrum","","der Macht"),
    dekliniere_substantiv("NOUN_THE_STAFF_OF_AESCULAPIUS","Stab","es","","","maskulin","","des Asklepios"),
    dekliniere_substantiv("NOUN_THE_STAFF_OF_AESCULAPIUS","Stab","es","","","maskulin","","des Äskulap"),
    dekliniere_substantiv("NOUN_THE_MAGIC_MIRROR_OF_MERLIN","Zauberspiegel","","","","maskulin","","Merlins"),
    dekliniere_substantiv("NOUN_THE_EYES_OF_THE_OVERWORLD","Auge","es","Auge","en","neutrum","","der Überwelt"),
    dekliniere_substantiv("NOUN_THE_MITRE_OF_HOLINESS","Mitra","","","","feminin","","der Heiligkeit"),
    dekliniere_substantiv("NOUN_THE_LONGBOW_OF_DIANA","Langbogen","s","","","maskulin","","der Diana"),
    dekliniere_substantiv("NOUN_THE_MASTER_KEY_OF_THIEVERY","Generalschlüssel", "s", "", "", "maskulin","","der Dieberei"), # netzhack Meisterschlüssel der Dieberey
    dekliniere_substantiv("NOUN_THE_TSURUGI_OF_MURAMASA","Tsurugi","","","","neutrum","","des Muramasa"),
    dekliniere_substantiv("NOUN_THE_PLATINUM_YENDORIAN_EXPRESS_CARD","Yendorian-Express-Platinkarte","","","","feminin"),
    dekliniere_substantiv("NOUN_THE_ORB_OF_FATE","Kristallkugel", "", "", "", "feminin","","des Schicksals"),
    dekliniere_substantiv("NOUN_THE_EYE_OF_THE_AETHIOPICA","Auge","","","","neutrum","","der Aethiopica"),
    "",
    "/* ======================================================= */",
    "/* Nouns that are neither in mons-de.h nor in obj-de.h */",
    "/* ======================================================= */",
    dekliniere_substantiv("NOUN_OBJEKT","Objekt","es","Objekt","e","neutrum"),
    dekliniere_substantiv("NOUN_SPINACH","Spinat","es","Spinat","e","maskulin"),
    dekliniere_substantiv("NOUN_OBJ_DOOR", "Türe", "", "Türe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_DOOR", "Türe", "", "Türe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WALLET", "Geldbörse", "", "Geldbörse", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_MEAT", "Fleisch", "es", "", "e", "neutrum", ""),
    dekliniere_substantiv("NOUN_LUCK", "Glück", "es", "Glück", "e", "neutrum", "s"),
    dekliniere_substantiv("NOUN_PAAR", "Paar", "es", "Paar", "e", "neutrum"), 
    dekliniere_substantiv("NOUN_PAAR", "Paar", "es", "Paar", "e", "neutrum"), 
    dekliniere_substantiv("NOUN_BOOTS", "Stiefel", "s", "Stiefel", "", "maskulin"), 
    dekliniere_substantiv("NOUN_GLOVES", "Handschuh", "es", "Handschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CLOAK", "Mantel", "s", "Mäntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_HELMET", "Helm", "es", "Helm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SHIELD", "Schild", "es", "Schild", "e", "maskulin"), 
    #dekliniere_substantiv("NOUN_STONE", "Stein", "es", "Stein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SWORD", "Schwert", "es", "Schwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SPELL", "Spruch", "es", "Sprüch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_MEDALLION", "Medaillon", "s", "Medaillon", "s", "neutrum"),
    "",
    dekliniere_substantiv("NOUN_DRESSING_MANEUVER", "Bekleidungsakt", "es", "Bekleidungsakt", "e", "maskulin"),
    dekliniere_substantiv("NOUN_QUICKNESS", "Schnelligkeit", "", "Schnelligkeit", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_BLUT", "Blut", "es", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SAEURE", "Säure", "", "Säure", "en", "feminin"),
    dekliniere_substantiv("NOUN_RICHTUNG", "Richtung", "", "Richtung", "en", "feminin"),
    dekliniere_substantiv("NOUN_BILD", "Bild", "es", "Bild", "er", "neutrum"),
    "",
    dekliniere_substantiv("NOUN_BOLT", "Blitz", "es", "Blitz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BLITZ", "Blitz", "es", "Blitz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BOLT_OF_FIRE", "Feuerblitz", "es", "Feuerblitz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BOLT_OF_COLD", "Kälteblitz", "es", "Kälteblitz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BOLT_OF_LIGHTNING", "Blitzschlag", "es", "Blitzschläg", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RAY", "Strahl", "es", "Strahl", "en", "maskulin"),
    dekliniere_nominalphrase("NOUN_MAGIC_MISSILE", "magisch", "Geschoss", "es", "Geschoss", "e", "neutrum"),
    dekliniere_substantiv("NOUN_SLEEP_RAY", "Schlafstrahl", "es", "Schlafstrahl", "en", "maskulin"),
    dekliniere_substantiv("NOUN_DEATH_RAY", "Todesstrahl", "es", "Todesstrahl", "en", "maskulin"),
    dekliniere_substantiv("NOUN_FIREBALL", "Feuerball", "es", "Feuerbäll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CONE_OF_COLD", "Kältekegel", "s", "Kältekegel", "", "maskulin"),
    dekliniere_substantiv("NOUN_FINGER_OF_DEATH", "Finger", "s", "Finger", "", "maskulin", "", "des Todes"),
    "",
    dekliniere_substantiv("NOUN_HALS", "Hals", "es", "Häls", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BODY", "Körper", "s", "Körper", "", "maskulin"),
    dekliniere_substantiv("NOUN_HAUT", "Haut", "", "Häut", "e", "feminin"),
    "",
    dekliniere_substantiv("NOUN_VERSUCH", "Versuch", "es", "Versuch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FINGER", "Finger", "s", "Finger", "", "maskulin"),
    dekliniere_substantiv("NOUN_HAND", "Hand", "", "Händ", "e", "feminin"),
    dekliniere_substantiv("NOUN_HEAD", "Kopf", "es", "Köpf", "e", "maskulin"),
    dekliniere_substantiv("NOUN_LEG", "Bein", "es", "Bein", "e", "neutrum"),
    dekliniere_substantiv("NOUN_FACE", "Gesicht", "es", "Gesicht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_FOOT", "Fuß", "es", "Füß", "e", "maskulin"),
    "",
    dekliniere_substantiv("NOUN_DUST", "Staub", "es", "Staub", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FLOOR", "Boden", "s", "Böden", "", "maskulin"),
    dekliniere_substantiv("NOUN_GROUND", "Erde", "", "Erde", "en", "feminin"),
    "",
    dekliniere_substantiv("NOUN_TREFFERMALUS", "Treffermalus", "", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_TREFFERBONUS", "Trefferbonus", "", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_SCHADENSMALUS", "Schadensmalus", "", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_SCHADENSBONUS", "Schadensbonus", "", "", "", "maskulin"),
    "",
    dekliniere_substantiv("NOUN_HINTERTEIL", "Hinterteil", "es", "Hinterteil", "e", "maskulin"),
    dekliniere_substantiv("NOUN_UNTERSEITE", "Unterseite", "", "Unterseite", "en", "feminin"),
    dekliniere_substantiv("NOUN_GEDANKE", "Gedanken", "s", "Gedanke", "en", "maskulin"),
    dekliniere_substantiv("NOUN_THRON", "Thron", "es", "Thron", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DRAWBRIDGE", "Zugbrücke", "", "Zugbrücke", "en", "feminin"),
    dekliniere_substantiv("NOUN_KLINGE", "Klinge", "", "Klinge", "en", "feminin"),
    dekliniere_substantiv("NOUN_LAVA", "Lava", "", "Lav", "en", "feminin", ""),
    dekliniere_substantiv("NOUN_BLICK", "Blick", "es", "Blick", "e", "maskulin"),
    dekliniere_substantiv("NOUN_STUFE", "Stufe", "", "Stufe", "en", "feminin"),
    dekliniere_substantiv("NOUN_MOVEMENT", "Bewegung", "", "Bewegung", "en", "feminin"),
    dekliniere_substantiv("NOUN_SCHLAG", "Schlag", "es", "Schläg", "e", "maskulin"),
    dekliniere_substantiv("NOUN_STUHL", "Stuhl", "es", "Stühl", "e", "maskulin"),
    dekliniere_substantiv("NOUN_MENSCH", "Mensch", "en", "Mensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_RAT", "Ratte", "", "Ratte", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_PURSE", "Portemonnaie", "s", "Portemonnaie", "s", "neutrum", ""),
    "",
    dekliniere_substantiv("NOUN_ERKRANKUNG", "Erkrankung", "", "Erkrankung", "en", "feminin", ""),
    dekliniere_substantiv("NOUN_ERSCHOEPFUNG", "Erschöpfung", "", "Erschöpfung", "en", "feminin", ""),
    dekliniere_substantiv("NOUN_HUNGERTOD", "Hungertod", "es", "Hungertod", "e", "maskulin", ""),
    
    dekliniere_substantiv("NOUN_ALTAR","Altar","es","Altär","e","maskulin"),
    dekliniere_substantiv("NOUN_FOUNTAIN","Springbrunnen","s","Springbrunnen","","maskulin"),
    dekliniere_substantiv("NOUN_HEADSTONE","Grabstein","es","Grabstein","e","maskulin"),
    dekliniere_substantiv("NOUN_THRONE","Thron","es","Thron","e","maskulin"),

    dekliniere_substantiv("NOUN_THRONE","Thron","es","Thron","e","maskulin"),

    dekliniere_substantiv("NOUN_HEADSCHMERZEN","Kopfschmerz","es","Kopfschmerz","en","maskulin"),
    dekliniere_substantiv("NOUN_VERLUST","Verlust","es","Verlust","e","feminin"),
    dekliniere_substantiv("NOUN_SPENDE","Spende","","Spende","en","feminin"),
    dekliniere_substantiv("NOUN_KNACKEN","Knacken","s","Knacken","","neutrum"),
    dekliniere_substantiv("NOUN_GESTALT","Gestalt","","Gestalt","en","feminin"),

		dekliniere_substantiv("NOUN_ARM","Arm","es","Arm","e","maskulin"),
		dekliniere_substantiv("NOUN_BRIDGE","Brücke","","Brücke","en","feminin"),
		dekliniere_substantiv("NOUN_CEILING","Decke","","Decke","en","feminin"),
		dekliniere_substantiv("NOUN_CHEMIE","Chemie","","Chemie","en","feminin"),
		dekliniere_substantiv("NOUN_EYE","Auge","es","Auge","en","neutrum"),
		dekliniere_substantiv("NOUN_FASSUNG","Fassung","","Fassung","en","feminin"),
		dekliniere_substantiv("NOUN_FEUER","Feuer","s","Feuer","","neutrum"),
		dekliniere_substantiv("NOUN_FINGERTIP","Fingerspitze","","Fingerspitze","en","feminin"),
		dekliniere_substantiv("NOUN_SCHNEE","Schnee","s","","","maskulin"),
		dekliniere_substantiv("NOUN_GEBET","Gebet","s","Gebet","e","neutrum"),
		dekliniere_substantiv("NOUN_GEGENWART","Gegenwart","","","","feminin"),
		dekliniere_substantiv("NOUN_GESCHMACK","Geschmack","es","Geschmäck","e","maskulin"),
		dekliniere_substantiv("NOUN_GOLD","Gold","es","","","neutrum"),
		dekliniere_substantiv("NOUN_GRABWERKZEUG","Grabwerkzeug","es","Grabwerkzeug","e","neutrum"),
		dekliniere_substantiv("NOUN_HERZSCHLAG","Herzschlag","es","Herzschläg","e","maskulin"),
		dekliniere_substantiv("NOUN_HERZ","Herz","ens","Herz","en","neutrum"),
		dekliniere_substantiv("NOUN_KNIE","Knie","s","Knie","","neutrum"),
		dekliniere_substantiv("NOUN_LEITUNGSWASSER", "Leitungswasser", "s", "", "", "neutrum"),
		dekliniere_substantiv("NOUN_LIGHT","Licht","es","Licht","er","neutrum"),
		dekliniere_substantiv("NOUN_LICHT","Licht","es","Licht","er","neutrum"),
		dekliniere_substantiv("NOUN_LOCH","Loch","es","Löch","er","neutrum"),
		dekliniere_substantiv("NOUN_MUND","Mund","es","Münd","er","maskulin"),
		dekliniere_substantiv("NOUN_PASSAGE","Korridor","s","Korridor","e","maskulin"),
		dekliniere_substantiv("NOUN_SACRIFICE","Opfer","s","Opfer","","neutrum"),
		dekliniere_substantiv("NOUN_SCHEPPERN","Scheppern","s","","","neutrum"),
		dekliniere_substantiv("NOUN_SEHKRAFT","Sehkraft","","","","feminin"),
		dekliniere_substantiv("NOUN_SKY","Himmel","s","","","maskulin"),
		dekliniere_substantiv("NOUN_SPINE","Rücken","s","Rücken","","maskulin"),
		dekliniere_substantiv("NOUN_STIMME","Stimme","","Stimme","en","feminin"),
		dekliniere_substantiv("NOUN_UMGEBUNG","Umgebung","","Umgebung","en","feminin"),
		dekliniere_substantiv("NOUN_ZEITLUPENTRITT","Zeitlupentritt","es","Zeitlupentritt","e","maskulin"),
		dekliniere_substantiv("NOUN_WOLKE","Wolke","","Wolke","en","feminin"),
		dekliniere_substantiv("NOUN_RAUCHWOLKE","Rauchwolke","","Rauchwolke","en","feminin"),
		dekliniere_substantiv("NOUN_RAUCHWOLKE","Rauchwolke","","Rauchwolke","en","feminin"),
		dekliniere_substantiv("NOUN_DUNST","Dunst","es","Dünst","e","feminin"),
		dekliniere_substantiv("NOUN_KRISTALL","Kristall","es","Kristall","e","maskulin"),

    dekliniere_substantiv("NOUN_HUMANITY","Menschheit","","","","feminin"),
    dekliniere_substantiv("NOUN_ELVENKIND","Elbenheit","","","","feminin"),
    dekliniere_substantiv("NOUN_DWARVENKIND","Zwergenheit","","","","feminin"),
    dekliniere_substantiv("NOUN_GNOMEHOOD","Gnomenheit","","","","feminin"),
    dekliniere_substantiv("NOUN_ORCDOM","Orkenheit","","","","feminin"),

		dekliniere_substantiv("NOUN_LEUCHTEN","Leuchten","s","","","neutrum"),
		dekliniere_substantiv("NOUN_AURA","Aura","","","","feminin"),
		dekliniere_substantiv("NOUN_OHR","Ohr","es","Ohr","en","neutrum"),
		dekliniere_substantiv("NOUN_GEFOLGSCHAFT","Gefolgschaft","","Gefolgschaft","en","feminin"),
		dekliniere_substantiv("NOUN_ANWESENHEIT","Anwesenheit","","","","feminin"),
		dekliniere_substantiv("NOUN_ABWESENHEIT","Abwesenheit","","","","feminin"),
		dekliniere_substantiv("NOUN_DUNKELHEIT","Dunkelheit","","","","feminin"),
		dekliniere_substantiv("NOUN_SEHKRAFT","Sehkraft","","","","feminin"),
		dekliniere_substantiv("NOUN_OBJECT","Objekt","es","Objekt","e","neutrum"),
		dekliniere_substantiv("NOUN_NAEHE","Nähe","","","","feminin"),
		dekliniere_substantiv("NOUN_GEISTESSCHAERFE","Geistesschärfe","","Geistesschärfe","en","feminin"),
		dekliniere_substantiv("NOUN_FRESSLUST","Fresslust","","Fresslüst","e","feminin"),
		dekliniere_substantiv("NOUN_VERBUNDENHEIT","Verbundenheit","","Verbundenheit","en","feminin"),
		dekliniere_substantiv("NOUN_SCHULD","Schuld","","Schuld","en","feminin"),
		dekliniere_substantiv("NOUN_KAMERA","Kamera","","","","feminin"),
		dekliniere_substantiv("NOUN_HABE","Habe","","Habe","en","feminin"),

    dekliniere_substantiv("NOUN_ALIGNMENT","Gesinnung","","Gesinnung","en","feminin"),
    #dekliniere_substantiv("NOUN_LAW","","","","",""),
    #dekliniere_substantiv("NOUN_BALANCE","","","","",""),
    #dekliniere_substantiv("NOUN_CHAOS","","","","",""),
    dekliniere_substantiv("NOUN_RACE","Volk","es","Völk","er","neutrum"),
    dekliniere_substantiv("NOUN_ROLE","Klasse","","Klasse","en","feminin"),
    dekliniere_substantiv("NOUN_GENDER","Geschlecht","es","","","neutrum"),
    dekliniere_substantiv("NOUN_NAME","Name","ens","Name","en","maskulin"),
    dekliniere_substantiv("NOUN_CHARACTER","Charakter","s","Charakter","e","maskulin"),

    "/* Shopbezeichnungen */",
    dekliniere_substantiv("NOUN_ANTIKWAFFENLAGER","Antikwaffenlager","s","","","neutrum"),
    dekliniere_substantiv("NOUN_ANTIQUARIAT","Antiquariat","es","","e","neutrum"),
    dekliniere_substantiv("NOUN_BOUTIQUE","Boutique","","","en","feminin"),
    dekliniere_substantiv("NOUN_BUECHERFLOHMARKT","Bücherflohmarkt","es","","e","maskulin"),
    dekliniere_substantiv("NOUN_DELIKATESSENLADEN","Delikatessenladen","s","","","maskulin"),
    dekliniere_substantiv("NOUN_EISENWARENHANDLUNG","Eisenwarenhandlung","","","en","feminin"),
    dekliniere_substantiv("NOUN_JUWELIERLADEN","Juwelierladen","s","","","maskulin"),
    dekliniere_substantiv("NOUN_KRAMLADEN","Kramladen","s","","","maskulin"),
    dekliniere_substantiv("NOUN_LICHTFACHMARKT","Lichtfachmarkt","es","","e","maskulin"),
    dekliniere_substantiv("NOUN_SCHUTZBEKLEIDUNGSFACHGESCHAEFT","Schutzbekleidungsfachgeschäft","es","","e","neutrum"),
    dekliniere_substantiv("NOUN_SPIRITUOSENHANDLUNG","Spirituosenhandlung","","","en","feminin"),

    "/* Fallen */",
    dekliniere_substantiv("NOUN_ANTI_MAGIC_FIELD","Antimagiefeld","es","Antimagiefeld","er","neutrum"),

    "/* Klassen (role.c) */",
    "/* Archäologe */",
    # "NOUN_ARCHAEOLOGE", "NOUN_ARCHAEOLOGIN"
    dekliniere_substantiv("NOUN_HILFSKRAFT","Hilfskraft","","Hilfskräft","e","maskulin"),
    dekliniere_substantiv("NOUN_FELDFORSCHER","Feldforscher","s","Feldforscher","","maskulin"),
    dekliniere_substantiv("NOUN_FELDFORSCHERIN","Feldforscherin","","Feldforscherinn","en","feminin"),
    dekliniere_substantiv("NOUN_EXHUMIERER","Exhumierer","s","Exhumierer","","maskulin"),
    dekliniere_substantiv("NOUN_EXHUMIERERIN","Exhumiererin","","Exhumiererinn","en","feminin"),
    dekliniere_substantiv("NOUN_AUSGRAEBER","Ausgräber","s","Ausgräber","","maskulin"),
    dekliniere_substantiv("NOUN_AUSGRAEBERIN","Ausgräberin","","Ausgräberinn","en","feminin"),
    dekliniere_substantiv("NOUN_PROJEKTLEITER","Projektleiter","s","Projektleiter","","maskulin"),
    dekliniere_substantiv("NOUN_PROJEKTLEITERIN","Projektleiterin","","Projektleiterinn","en","feminin"),
    dekliniere_substantiv("NOUN_HOEHLENGEHER","Höhlengeher","s","Höhlengeher","","maskulin"),
    dekliniere_substantiv("NOUN_HOEHLENGEHERIN","Höhlengeherin","","Höhlengeherinn","en","feminin"),
    dekliniere_substantiv("NOUN_SPELAEOLOGE","Speläologe","en","Speläologe","en","maskulin"),
    dekliniere_substantiv("NOUN_SPELAEOLOGIN","Speläologin","","Speläologinn","en","feminin"),
    dekliniere_substantiv("NOUN_SAMMLER","Sammler","s","Sammler","","maskulin"),
    dekliniere_substantiv("NOUN_SAMMLERIN","Sammlerin","","Sammlerinn","en","feminin"),
    dekliniere_substantiv("NOUN_KURATOR","Kurator","es","Kurator","en","maskulin"),
    dekliniere_substantiv("NOUN_KURATORIN","Kuratorin","","Kuratorinn","en","feminin"),

    "/* Barbar */",
    # "NOUN_BARBAR", "NOUN_BARBARIN"
    dekliniere_substantiv("NOUN_STRAUCHDIEB","Strauchdieb","es","Strauchdieb","e","maskulin"),
    dekliniere_substantiv("NOUN_STRAUCHDIEBIN","Strauchdiebin","","Strauchdiebinn","en","feminin"),
    dekliniere_substantiv("NOUN_BRANDSCHATZER","Brandschatzer","s","Brandschatzer","","maskulin"),
    dekliniere_substantiv("NOUN_BRANDSCHATZERIN","Brandschätzerin","","Brandschätzerinn","en","feminin"),
    dekliniere_substantiv("NOUN_BANDIT","Bandit","en","Bandit","en","maskulin","en"),
    dekliniere_substantiv("NOUN_BANDITIN","Banditin","","Banditinn","en","feminin"),
    dekliniere_substantiv("NOUN_BRIGANT","Brigant","en","Brigant","en","maskulin","en"),
    dekliniere_substantiv("NOUN_BRIGANTIN","Brigantin","","Brigantinn","en","feminin"),
    dekliniere_substantiv("NOUN_RAEUBER","Räuber","s","Räuber","","maskulin"),
    dekliniere_substantiv("NOUN_RAEUBERIN","Räuberin","","Räuberinn","en","feminin"),
    dekliniere_substantiv("NOUN_PLUENDERER","Plünderer","s","Plünderer","","maskulin"),
    dekliniere_substantiv("NOUN_PLUENDERIN","Plünderin","","Plünderinn","en","feminin"),
    dekliniere_substantiv("NOUN_TOTSCHLAEGER","Totschläger","s","Totschläger","","maskulin"),
    dekliniere_substantiv("NOUN_TOTSCHLAEGERIN","Totschlägerin","","Totschlägerinn","en","feminin"),
    dekliniere_substantiv("NOUN_STAMMESFUEHRER","Stammesführer","s","Stammesführer","","maskulin"),
    dekliniere_substantiv("NOUN_STAMMESFUEHRERIN","Stammesfüherin","","Stammesfüherinn","en","feminin"),
    dekliniere_substantiv("NOUN_EROBERER","Eroberer","s","Eroberer","","maskulin"),
    dekliniere_substantiv("NOUN_EROBERIN","Eroberin","","Eroberinn","en","feminin"),

    "/* Barbar */",
    # "NOUN_CAVEMAN"
    dekliniere_substantiv("NOUN_TROGLODYT","Troglodyt","s","Troglodyt","en","maskulin"),
    dekliniere_substantiv("NOUN_TROGLODYTIN","Troglodytin","","inn","en","feminin"),
    dekliniere_adjektivisches_substantiv("NOUN_EINGEBORENER","Eingeboren","maskulin"),
    dekliniere_adjektivisches_substantiv("NOUN_EINGEBORENE","Eingeboren","feminin"),
    dekliniere_substantiv("NOUN_HERUMTREIBER","Herumtreiber","","Herumtreiber","","maskulin"),
    dekliniere_substantiv("NOUN_HERUMTREIBERIN","Herumtreiberin","","Herumtreiberinn","en","feminin"),
    dekliniere_substantiv("NOUN_LANDSTREICHER","Landstreicher","s","Landstreicher","","maskulin"),
    dekliniere_substantiv("NOUN_LANDSTREICHERIN","Landstreicherin","","Landstreicherinn","en","feminin"),
    dekliniere_substantiv("NOUN_VAGABUND","Vagabund","en","Vagabund","en","maskulin"),
    dekliniere_substantiv("NOUN_VAGABUNDIN","Vagabundin","","Vagabundinn","en","feminin"),
    dekliniere_substantiv("NOUN_VAGANT","Vagant","en","Vagant","en","maskulin"),
    dekliniere_substantiv("NOUN_VAGANTIN","Vagantin","","Vagantinn","en","feminin"),
    dekliniere_substantiv("NOUN_NOMADE","Nomade","en","Nomade","en","maskulin"),
    dekliniere_substantiv("NOUN_NOMADIN","Nomadin","","Nomadinn","en","feminin"),
    dekliniere_substantiv("NOUN_WANDERER","Wanderer","s","Wanderer","","maskulin"),
    dekliniere_substantiv("NOUN_WANDERIN","Wanderin","","Wanderinn","en","feminin"),
    dekliniere_substantiv("NOUN_PIONIER","Pionier","s","Pionier","e","maskulin"),
    dekliniere_substantiv("NOUN_PIONIERIN","Pionierin","","Pionierinn","en","feminin"),

    "/* Heiler */",
		# "NOUN_HEILER", "NOUN_HEILERIN"
    dekliniere_substantiv("NOUN_RHIZOTOMIST","Rhizotom","en","Rhizotom","en","maskulin"),
    dekliniere_substantiv("NOUN_RHIZOTOMISTIN","Rhizotomin","","Rhizotomistinn","en","feminin"),
    dekliniere_substantiv("NOUN_EMPIRIC","Empiriker","s","Empiriker","","maskulin"),
    dekliniere_substantiv("NOUN_EMPIRICIN","Empirikerin","","Empirikerinn","en","feminin"),
    dekliniere_substantiv("NOUN_EMBALMER","Balsamierer","s","Balsamierer","","maskulin"),
    dekliniere_substantiv("NOUN_EMBALMERIN","Balsamiererin","","Balsamiererinn","en","feminin"),
    dekliniere_substantiv("NOUN_DRESSER","Bader","s","Bader","","maskulin"),
    dekliniere_substantiv("NOUN_DRESSERIN","Baderin","","Baderinn","en","feminin"),
    dekliniere_substantiv("NOUN_MEDICUS_OSSIUM","Wundarzt","es","Wundärzt","e","maskulin"), # Chirurgicus
    dekliniere_substantiv("NOUN_MEDICA_OSSIUM","Wundärztin","","Wundärztinn","en","feminin"),
    dekliniere_adjektivisches_substantiv("NOUN_HERBALIST","Heilkundig","maskulin"),
    dekliniere_adjektivisches_substantiv("NOUN_HERBALISTIN","Heilkundig","feminin"),
    dekliniere_substantiv("NOUN_MAGISTER","Magister","s","Magister","","maskulin"),
    dekliniere_substantiv("NOUN_MAGISTRA","Magistra","","Magistra","fremder","feminin"),
    dekliniere_substantiv("NOUN_PHYSICIAN","Physikus","","Physikuss","e","maskulin"),
    dekliniere_substantiv("NOUN_PHYSICIANIN","Physika","","Physika","fremder","feminin"),
    dekliniere_substantiv("NOUN_CHIRURGEON","Medikus","","Medizi","fremder","maskulin"),
    dekliniere_substantiv("NOUN_CHIRURGEONIN","Medika","","Medika","fremder","feminin"),

	 "/* Paladin */",
	 # "NOUN_KNIGHT"

    "/* Mönch */",
    # "NOUN_MOENCH"
    dekliniere_substantiv("NOUN_KANDIDAT","Kandidat","en","Kandidat","en","maskulin"),
    dekliniere_substantiv("NOUN_KANDIDATIN","Kandidatin","","Kandidatinn","en","feminin"),
    dekliniere_substantiv("NOUN_NOVIZE","Novize","en","Novize","en","maskulin"),
    dekliniere_substantiv("NOUN_NOVIZIN","Novizin","","Novizinn","en","feminin"),
    dekliniere_adjektivisches_substantiv("NOUN_GEWEIHTE","Geweiht","maskulin"),
    dekliniere_adjektivisches_substantiv("NOUN_GEWEIHTIN","Geweiht","feminin"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_STEINE","Schüler","s","Schüler","","maskulin","","der Steine"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_STEINE","Schülerin","","Schülerinn","en","feminin","","der Steine"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_WASSER","Schüler","s","Schüler","","maskulin","","der Wasser"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_WASSER","Schülerin","","Schülerinn","en","feminin","","der Wasser"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_METALLE","Schüler","s","Schüler","","maskulin","","der Metalle"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_METALLE","Schülerin","","Schülerinn","en","feminin","","der Metalle"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_WINDE","Schüler","s","Schüler","","maskulin","","der Winde"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_WINDE","Schülerin","","Schülerinn","en","feminin","","der Winde"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_FEUER","Schüler","s","Schüler","","maskulin","","der Feuer"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_FEUER","Schülerin","","Schülerinn","en","feminin","","der Feuer"),
    dekliniere_substantiv("NOUN_MEISTER","Meister","s","Meister","","maskulin"),
    dekliniere_substantiv("NOUN_MEISTERIN","Meisterin","","Meisterinn","en","feminin"),

    "/* Priester */",
    # "NOUN_PRIESTER", "NOUN_PRIESTERIN"
    dekliniere_substantiv("NOUN_ASPIRANT","Aspirant","en","Aspirant","en","maskulin"),
    dekliniere_substantiv("NOUN_ASPIRANTIN","Aspirantin","","Aspirantinn","en","feminin"),
    dekliniere_substantiv("NOUN_ACOLYTE","Akolyth","en","Akolyth","en","maskulin"),
    dekliniere_substantiv("NOUN_ACOLYTEIN","Akolythin","","Akolythinn","en","feminin"),
    dekliniere_substantiv("NOUN_ADEPT","Adept","en","Adept","en","maskulin"),
    dekliniere_substantiv("NOUN_ADEPTIN","Adeptin","","Adeptinn","en","feminin"),
    # "NOUN_PRIESTER", "NOUN_PRIESTERIN"
    dekliniere_substantiv("NOUN_CURATE","Kurat","en","Kurat","en","maskulin"),
    dekliniere_substantiv("NOUN_CURATEIN","Kuratin","","Kuratinn","en","feminin"),
    dekliniere_substantiv("NOUN_CANON","Kanonikus","","Kanoniker","fremder","maskulin"),
    dekliniere_substantiv("NOUN_CANONESS","Kanonisse","","Kanonisse","en","feminin"),
    dekliniere_substantiv("NOUN_LAMA","Lama","s","Lama","s","maskulin"),
    dekliniere_substantiv("NOUN_LAMAIN","Lama","","Lama","fremder","feminin"),
    dekliniere_substantiv("NOUN_PATRIARCH","Patriarch","en","Patriarch","en","maskulin"),
    dekliniere_substantiv("NOUN_MATRIARCH","Matriarchin","","Matriarchinn","en","feminin"),
    # "NOUN_HIGH_PRIEST" "NOUN_HIGH_PRIESTESS"

    "/* Schurke */",
    # "NOUN_ROGUE"
    dekliniere_substantiv("NOUN_STRASSENRAEUBER","Straßenräuber","s","Straßenräuber","","maskulin"),
    dekliniere_substantiv("NOUN_STRASSENRAEUBERIN","Straßenräuberin","","Strßsenräuberinn","en","feminin"),
    dekliniere_substantiv("NOUN_LANGFINGER","Langfinger","s","Langfinger","","maskulin"),
    dekliniere_substantiv("NOUN_HALLODRI","Hallodri","s","Hallodris","s","maskulin"),
    dekliniere_substantiv("NOUN_GANOVE","Ganove","en","Ganove","en","maskulin"),
    dekliniere_substantiv("NOUN_GANOVIN","Ganovin","","Ganovinn","en","feminin"),
    # dekliniere_substantiv("NOUN_RAEUBER"
    # dekliniere_substantiv("NOUN_RAEUBERIN"
    dekliniere_substantiv("NOUN_EINBRECHER","Einbrecher","s","Einbrecher","","maskulin"),
    dekliniere_substantiv("NOUN_EINBRECHERIN","Einbrecherin","","Einbrecherinn","en","feminin"),
    dekliniere_substantiv("NOUN_BETRUEGER","Betrüger","s","Betrüger","","maskulin"),
    dekliniere_substantiv("NOUN_BETRUEGERIN","Betrügerin","","Betrügerinn","en","feminin"),
    dekliniere_substantiv("NOUN_GANGSTER","Gangster","s","Gangster","","maskulin"),
    dekliniere_substantiv("NOUN_GANGSTERBRAUT","Gangsterbraut","","Gangsterbräut","e","feminin"),
    # dekliniere_substantiv("NOUN_DIEB"
    # dekliniere_substantiv("NOUN_DIEBIN"

	 "/* Waldläufer */",
	 # "NOUN_RANGER"
    dekliniere_substantiv("NOUN_JUNGSPUND","Jungspund","es","Jungspund","e","maskulin"),
    dekliniere_substantiv("NOUN_AUSGUCK","Ausguck","es","Ausguck","e","maskulin"),
    dekliniere_substantiv("NOUN_VORREITER","Vorreiter","s","","Vorreiter","maskulin"),
    dekliniere_substantiv("NOUN_VORREITERIN","Vorreiterin","","Vorreiterinn","en","feminin"),
    dekliniere_substantiv("NOUN_SPAEHER","Späher","s","Späher","","maskulin"),
    dekliniere_substantiv("NOUN_SPAEHERIN","Späherin","","Späherinn","en","feminin"),
    dekliniere_substantiv("NOUN_KUNDSCHAFTER","Kundschafter","s","Kundschafter","","maskulin"),
    dekliniere_substantiv("NOUN_KUNDSCHAFTERIN","Kundschafterin","","Kundschafterinn","en","feminin"),
    dekliniere_substantiv("NOUN_ARMBRUSTER","Armbruster","s","Armbruster","","maskulin"),
    dekliniere_substantiv("NOUN_ARMBRUSTERIN","Armbrusterin","","Armbrusterinn","en","feminin"),
    dekliniere_substantiv("NOUN_BOGENSCHUETZE","Bogenschütze","en","Bogenschütze","en","maskulin"),
    dekliniere_substantiv("NOUN_BOGENSCHUETZIN","Bogenschützin","","Bogenschützinn","en","feminin"),
    dekliniere_substantiv("NOUN_SCHARFSCHUETZE","Scharfschütze","en","Scharfschütze","en","maskulin"),
    dekliniere_substantiv("NOUN_SCHARFSCHUETZIN","Scharfschützin","","Scharfschützinn","en","feminin"),
    dekliniere_substantiv("NOUN_MEISTERSCHUETZE","Meisterschütze","en","Meisterschütze","en","maskulin"),
    dekliniere_substantiv("NOUN_MEISTERSCHUETZIN","Meisterschützin","","Meisterschützinn","en","feminin"),

	 "/* Samurai */",
    # "NOUN_SAMURAI", "NOUN_SAMURAIIN"
    dekliniere_substantiv("NOUN_HATAMOTO","Hatamoto","","Hatamoto","s","maskulin"),
    dekliniere_substantiv("NOUN_HATAMOTOIN","Hatamoto","","Hatamoto","s","feminin"),
    dekliniere_substantiv("NOUN_RONIN","Ronin","","Ronin","s","maskulin"),
    dekliniere_substantiv("NOUN_RONININ","Ronin","","Ronin","s","feminin"),
    dekliniere_substantiv("NOUN_NINJA","Ninja","","Ninja","s","maskulin"),
    dekliniere_substantiv("NOUN_KUNOICHI","Kunoichi","","Kunoichi","s","feminin"),
    dekliniere_substantiv("NOUN_JOSHU","Joshu","","Joshus","s","maskulin"),
    dekliniere_substantiv("NOUN_JOSHUIN","Joshu","","Joshu","s","feminin"),
    dekliniere_substantiv("NOUN_RYOSHU","Ryoshu","","Ryoshu","s","maskulin"),
    dekliniere_substantiv("NOUN_RYOSHUIN","Ryoshu","","Ryoshu","s","feminin"),
    dekliniere_substantiv("NOUN_KOKUSHU","Kokushu","","Kokushu","s","maskulin"),
    dekliniere_substantiv("NOUN_KOKUSHUIN","Kokushu","","Kokushu","s","feminin"),
    dekliniere_substantiv("NOUN_DAIMYO","Daimyo","","Daimyo","s","maskulin"),
    dekliniere_substantiv("NOUN_DAIMYOIN","Daimyo","","Daimyo","s","feminin"),
    dekliniere_substantiv("NOUN_KUGE","Kuge","","Kuge","s","maskulin"),
    dekliniere_substantiv("NOUN_KUGEIN","Kuge","","Kuge","s","feminin"),
    dekliniere_substantiv("NOUN_SHOGUN","Shogun","s","Shogun","e","maskulin"),
    dekliniere_substantiv("NOUN_SHOGUNIN","Shogun","s","Shogun","e","feminin"),


	 "/* Tourist */",
    # "NOUN_TOURIST", "NOUN_TOURISTIN"
    # "NOUN_WANDERER"
    # "NOUN_WANDERIN"
    dekliniere_adjektivisches_substantiv("NOUN_SCHAULUSTIGE","Schaulustig","maskulin"),
    dekliniere_adjektivisches_substantiv("NOUN_SCHAULUSTIGIN","Schaulustig","feminin"),
    dekliniere_substantiv("NOUN_AUSFLUEGLER","Ausflügler","s","Ausflügler","","maskulin"),
    dekliniere_substantiv("NOUN_AUSFLUEGLERIN","Ausflüglerin","","Ausflüglerinn","en","feminin"),
    dekliniere_adjektivisches_substantiv("NOUN_REISENDE","Reisend","maskulin"),
    dekliniere_adjektivisches_substantiv("NOUN_REISENDEIN","Reisend","feminin"),
    dekliniere_substantiv("NOUN_GLOBETROTTER","Globetrotter","s","Globetrotter","","maskulin"),
    dekliniere_substantiv("NOUN_GLOBETROTTERIN","Globetrotterin","","Globetrotterinn","en","feminin"),
    dekliniere_substantiv("NOUN_WELTENBUMMLER","Weltenbummler","s","Weltenbummler","","maskulin"),
    dekliniere_substantiv("NOUN_WELTENBUMMLERIN","Weltenbummlerin","","Weltenbummlerinn","en","feminin"),
    dekliniere_substantiv("NOUN_SEEFAHRER","Seefahrer","s","Seefahrer","","maskulin"),
    dekliniere_substantiv("NOUN_SEEFAHRERIN","Seefahrerin","","Seefahrerinn","en","feminin"),
    dekliniere_substantiv("NOUN_ERFORSCHER","Erforscher","s","Erforscher","","maskulin"),
    dekliniere_substantiv("NOUN_ERFORSCHERIN","Erforscherin","","Erforscherinn","en","feminin"),
    dekliniere_substantiv("NOUN_ABENTEURER","Abenteurer","s","Abenteurer","","maskulin"),
    dekliniere_substantiv("NOUN_ABENTEURERIN","Abenteurerin","","Abenteurerinn","en","feminin"),

    "/* Walküre */",
    # "NOUN_VALKYRIE"
    dekliniere_substantiv("NOUN_GRUENSCHNABEL","Grünschnabel","s","Grünschnäbel","","maskulin"),
    dekliniere_substantiv("NOUN_HAUDEGEN","Haudegen","s","Haudegen","","maskulin"),
    dekliniere_substantiv("NOUN_STREITER","Streiter","s","Streiter","","maskulin"),
    dekliniere_substantiv("NOUN_STREITERIN","Streiterin","","Streiterinn","en","feminin"),
    dekliniere_substantiv("NOUN_KAEMPFER","Kämpfer","s","Kämpfer","","maskulin"),
    dekliniere_substantiv("NOUN_KAEMPFERIN","Kämpferin","","Kämpferinn","en","feminin"),
    dekliniere_substantiv("NOUN_KRIEGER","Krieger","s","Krieger","","maskulin"),
    dekliniere_substantiv("NOUN_KRIEGERIN","Kriegerin","","Kriegerinn","en","feminin"),
    dekliniere_substantiv("NOUN_SAEBELRASSLER","Säbelrassler","","Säbelrassler","","maskulin"),
    dekliniere_substantiv("NOUN_SAEBELRASSLERIN","Säbelrasslerin","","Säbelrasslerinn","en","feminin"),
    dekliniere_substantiv("NOUN_HELD","Held","en","Held","en","maskulin","en"),
    dekliniere_substantiv("NOUN_HELDIN","Heldin","","Heldinn","en","feminin"),
    dekliniere_substantiv("NOUN_KRIEGERPRINZ","Kriegerprinz","en","Kriegerprinz","en","maskulin"),
    dekliniere_substantiv("NOUN_KRIEGERPRINZESSIN", "Kriegerprinzessin", "", "Kriegerprinzessine", "en", "feminin"),
    dekliniere_substantiv("NOUN_LORD","Lord","s","Lord","s","maskulin"),
    dekliniere_substantiv("NOUN_LADY","Lady","","Lady","s","feminin"),

	 # ,"","","","","maskulin"),
    # ,"in","","inn","en","feminin"),
    "/* Zauberer */",
    # "NOUN_WIZARD"
    dekliniere_substantiv("NOUN_WIZARDIN","Magierin","s","Magierinn","en","feminin"),
    dekliniere_substantiv("NOUN_BESCHWOERER","Beschwörer","","Beschwörer","","maskulin"),
    dekliniere_substantiv("NOUN_BESCHWOERERIN","Beschwörerin","","Beschwörerinn","en","feminin"),
    dekliniere_substantiv("NOUN_GEISTERBANNER","Banner","s","Banner","","maskulin"),
    dekliniere_substantiv("NOUN_GEISTERBANNERIN","Bannerin","","Bannerinn","en","feminin"),
    dekliniere_substantiv("NOUN_THAUMATURG","Thaumaturg","en","Thaumaturg","en","maskulin"),
    dekliniere_substantiv("NOUN_THAUMATURGIN","Thaumaturgin","","Thaumaturginn","en","feminin"),
    dekliniere_substantiv("NOUN_ILLUSIONIST","Illusionist","en","Illusionist","en","maskulin"),
    dekliniere_substantiv("NOUN_ILLUSIONISTIN","Illusionistin","","Illusionistinn","en","feminin"),
    dekliniere_substantiv("NOUN_HEXER","Hexer","s","Hexer","","maskulin"),
    dekliniere_substantiv("NOUN_HEXE","Hexe","","Hexe","en","feminin"),
    dekliniere_substantiv("NOUN_HEXENMEISTER","Hexenmeister","","Hexenmeister","","maskulin"),
    dekliniere_substantiv("NOUN_HEXENMEISTERIN","Hexenmeisterin","","Hexenmeisterinn","en","feminin"),
    dekliniere_substantiv("NOUN_NEKROMANT","Nekromant","en","Nekromant","en","maskulin"),
    dekliniere_substantiv("NOUN_NEKROMANTIN","Nekromantin","","Nekromantinn","en","feminin"),
    dekliniere_substantiv("NOUN_ZAUBERER","Zauberer","s","Zauberer","","maskulin"),
    dekliniere_substantiv("NOUN_ZAUBERERIN","Zaubererin","","Zaubererinn","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIER","Magier","s","Magier","","maskulin"),
    dekliniere_substantiv("NOUN_MAGIERIN","Magierin","","Magierinn","en","feminin"),

    "/* sonstige Substantive */",
    dekliniere_substantiv("NOUN_ACID","Säure","","Säure","en","feminin"),
    dekliniere_nominalphrase("NOUN_ANGELIC_BEING","engelhaft","Geschöpf","es","Geschöpf","e","neutrum"),
    dekliniere_substantiv("NOUN_CONDUCT","Gebaren","s","","","neutrum"),
    dekliniere_nominalphrase("NOUN_APELIKE_CREATURE","affenartig","Kreatur","","Kreatur","en","feminin"),
    dekliniere_substantiv("NOUN_ARROW_TRAP","Pfeilfalle","","Pfeilfalle","en","feminin"),
    dekliniere_substantiv("NOUN_ARTIFACT","Artefakt","es","Artefakt","e","neutrum"),
    dekliniere_substantiv("NOUN_ATTACK","Angriff","es","Angriffe","e","maskulin"),
    dekliniere_substantiv("NOUN_BEFREIUNG","Befreiung","","Befreiung","en","feminin"),
    dekliniere_substantiv("NOUN_BELOHNUNG", "Belohnung", "", "Belohnung", "en", "feminin", ""),
    dekliniere_substantiv("NOUN_BEWAFFNUNG","Bewaffnung","","Bewaffnung","en","feminin"),
    dekliniere_substantiv("NOUN_BITE","Biss","es","Biss","e","maskulin"),
    dekliniere_substantiv("NOUN_BLADE","Klinge","","Klinge","en","feminin"),
    dekliniere_substantiv("NOUN_BOOK","Buch","es","Büch","er","neutrum"),
    dekliniere_adjektivisches_substantiv("NOUN_CANINE","Hundeartig","neutrum"),
    dekliniere_substantiv("NOUN_CHAIN","Kette","","Kette","en","feminin"),
    dekliniere_substantiv("NOUN_CHAOS","Chaos","","","","neutrum"),
    dekliniere_substantiv("NOUN_CONDUCT","Gebaren","s","","","neutrum"),
    dekliniere_substantiv("NOUN_CREATURE","Kreatur","","Kreatur","en","feminin"),
    dekliniere_substantiv("NOUN_DART_TRAP","Dartfalle","","Dartfalle","en","feminin"),
    dekliniere_substantiv("NOUN_DEMON", "Dämon", "s", "Dämon", "en", "maskulin"),
    dekliniere_substantiv("NOUN_DURST","Durst","es","","","maskulin"),
    dekliniere_substantiv("NOUN_EIGENSCHAFT","Eigenschaft","","Eigenschaft","en","feminin"),
    dekliniere_substantiv("NOUN_EINKAUF","Einkauf","es","Einkäuf","e","maskulin"),
    dekliniere_substantiv("NOUN_ELLBOGEN","Ellbogen","s","Ellbogen","","maskulin"),
    dekliniere_substantiv("NOUN_ERSTWAFFE","Erstwaffe","","Erstwaffe","en","feminin"),
    dekliniere_substantiv("NOUN_ESSEN","Essen","s","Essen","","neutrum"),
    dekliniere_substantiv("NOUN_EXPLOSION","Explosion","","Explosion","en","feminin"),
    dekliniere_substantiv("NOUN_EXTREMITAET","Extremität","","Extremität","en","feminin"),
    dekliniere_adjektivisches_substantiv("NOUN_FELINE","Katzenartig","neutrum"),
    dekliniere_substantiv("NOUN_FLEISCH","Fleisch","es","","","neutrum"),
    dekliniere_substantiv("NOUN_GAS_SPORE_EXPLOSION","Gassporenexplosion","","Gassporenexplosion","en","feminin"),
    dekliniere_substantiv("NOUN_GAZE","Blick","es","Blick","e","maskulin"),
    dekliniere_substantiv("NOUN_GEIST","Geist","es","Geist","e","maskulin"),
    dekliniere_substantiv("NOUN_GELD","Geld","es","Geld","er","neutrum"),
    dekliniere_substantiv("NOUN_GEHIRN","Gehirn","es","Gehirn","e","neutrum"),
    dekliniere_substantiv("NOUN_GEM_COLLECTION", "Edelsteinsammlung", "", "Edelsteinsammlung", "en", "feminin"),
    dekliniere_substantiv("NOUN_GESCHENK","Geschenk","es","Geschenk","e","neutrum"),
    dekliniere_substantiv("NOUN_GESCHWINDIGKEIT","Geschwindigkeit","","Geschwindigkeit","en","feminin"),
    dekliniere_substantiv("NOUN_GRIFF","Griff","es","Griff","e","maskulin"),
    dekliniere_substantiv("NOUN_GLUECKSSTRAEHNE","Glückssträhne","","Glückssträhne","en","feminin"),
    dekliniere_substantiv("NOUN_PECHSTRAEHNE","Pechsträhne","","Pechsträhne","en","feminin"),
    dekliniere_substantiv("NOUN_GAME","Spiel","es","Spiel","e","neutrum"),
    dekliniere_substantiv("NOUN_GASWOLKE","Gaswolke","","Gaswolke","en","feminin"),
    dekliniere_substantiv("NOUN_GIFT","Gift","es","Gift","e","neutrum"),
    dekliniere_substantiv("NOUN_GIFTGASWOLKE","Giftgaswolke","","Giftgaswolke","en","feminin"),
    dekliniere_substantiv("NOUN_GOTTHEIT","Gottheit","","Gottheit","en","feminin"),
    dekliniere_substantiv("NOUN_GOTT","Gott","es","Gött","er","maskulin"),
    dekliniere_substantiv("NOUN_GOETTIN","Göttin","","Göttinn","en","feminin"),
    dekliniere_substantiv("NOUN_GRIP","Griff","es","Griff","e","maskulin"),
    dekliniere_substantiv("NOUN_GRUBE","Grube","","Grube","en","feminin"),
    dekliniere_substantiv("NOUN_HALBGOETTIN","Halbgöttin","","Halbgöttinn","en","feminin"),
    dekliniere_substantiv("NOUN_HALBGOTT","Halbgott","es","Halbgött","er","maskulin"),
    dekliniere_substantiv("NOUN_HEIMAT","Heimat","","Heimat","en","feminin"),
    dekliniere_substantiv("NOUN_IRON_BALL", "Eisenkugel", "", "Eisenkugel", "en", "feminin"),
    dekliniere_substantiv("NOUN_KAELTE","Kälte","","","","feminin"),
    dekliniere_substantiv("NOUN_KICK","Tritt","es","Tritt","e","maskulin"),
    dekliniere_substantiv("NOUN_KLEID","Kleid","es","Kleid","er","neutrum"),
    dekliniere_substantiv("NOUN_KNOWLEDGE","Kenntnis","","Kentniss","e","feminin"),
    dekliniere_substantiv("NOUN_KOERPER","Körper","s","Körper","","maskulin"),
    dekliniere_substantiv("NOUN_KOPF","Kopf","es","Köpf","e","maskulin"),
    dekliniere_substantiv("NOUN_KORRIDOR","Korridor","s","Korridor","e","maskulin"),
    dekliniere_substantiv("NOUN_LADDER_DOWN", "Leiter", "", "Leiter", "en", "feminin","","hinauf"),
    dekliniere_substantiv("NOUN_LADDER_UP", "Leiter", "", "Leiter", "en", "feminin","","hinab"),
    dekliniere_substantiv("NOUN_LANTERN","Laterne","","Laterne","en","feminin"),
    dekliniere_substantiv("NOUN_LEBEN","Leben","s","Leben","","neutrum"),
    dekliniere_substantiv("NOUN_LEBENSKRAFT","Lebenskraft","","Lebenskräft","e","feminin"),
    dekliniere_substantiv("NOUN_LEIB","Leib","es","Leib","er","maskulin"),
    dekliniere_substantiv("NOUN_LEINE","Leine","","Leine","en","feminin"),
    dekliniere_substantiv("NOUN_LICHTBLITZ","Lichtblitz","es","Lichtblitz","e","maskulin"),
    dekliniere_nominalphrase("NOUN_LOWERED_DRAWBRIDGE", "heruntergelassene", "Zugbrücke", "", "Zugbrücke", "en", "feminin"),
    dekliniere_adjektivisches_substantiv("NOUN_MAGENINNERE","Mageninner", "neutrum"),
    dekliniere_nominalphrase("NOUN_MAGIC_PORTAL","magisch","Portal","s","Portal","e","neutrum"),
    dekliniere_substantiv("NOUN_MAGIC_TRAP","Magiefalle","","Magiefalle","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIEEXPLOSION","Magieexplosion","","Magieexplosion","en","feminin"),
    dekliniere_substantiv("NOUN_MAHL","Mahl","es","Mahl","e","neutrum"),
    dekliniere_substantiv("NOUN_MAHLZEIT","Mahlzeit","","Mahlzeit","en","feminin"),
    dekliniere_nominalphrase("NOUN_MAJOR_DEMON", "größer", "Dämon", "s", "Dämon", "en", "maskulin"),
    dekliniere_substantiv("NOUN_MAN", "Mann", "es", "Männ", "er", "maskulin"),
    dekliniere_substantiv("NOUN_MARKER", "Füller", "s", "Füller", "", "maskulin"),
    dekliniere_substantiv("NOUN_MAUL","Maul","es","Mäul","er","neutrum"),
    dekliniere_nominalphrase("NOUN_MINOR_DEMON", "kleiner", "Dämon", "s", "Dämon", "en", "maskulin"),
    dekliniere_substantiv("NOUN_MUELL","Müll","s","","","maskulin"),
    dekliniere_substantiv("NOUN_MOAT","Burggraben","s","Burggräben","","maskulin"), # Burggraben, Wassergraben
    dekliniere_nominalphrase("NOUN_MOLTEN_LAVA", "geschmolzen", "Lava", "", "Lav", "en", "feminin", ""),
    dekliniere_substantiv("NOUN_MONSTER","Monster","s","Monster","","neutrum"),
    dekliniere_substantiv("NOUN_NETZ","Netz","es","Netz","e","neutrum"),
    dekliniere_substantiv("NOUN_PAYMENT","Bezahlung","","Bezahlung","en","feminin"),
    dekliniere_substantiv("NOUN_POISON","Gift","es","Gift","e","neutrum"),
    dekliniere_substantiv("NOUN_PROTOPLASMA","Protoplasma","s","Protoplasm","en","neutrum"),
    dekliniere_substantiv("NOUN_QUIVER","Köcher","s","Köcher","","maskulin"),
    dekliniere_substantiv("NOUN_RACHEN","Rachen","s","Rachen","","maskulin"),
    dekliniere_nominalphrase("NOUN_RAISED_DRAWBRIDGE", "hochgezogen", "Zugbrücke", "", "Zugbrücke", "en", "feminin"),
    dekliniere_substantiv("NOUN_RAUCH","Rauch","s","","","maskulin"),
    dekliniere_substantiv("NOUN_REPERTOIRE","Repertoire","s","Repertoire","s","neutrum"),
    dekliniere_substantiv("NOUN_REITUNFALL","Reitunfall","es","Reitunfäll","e","maskulin"),
    dekliniere_substantiv("NOUN_RUCKSACK","Rucksack","s","Rucksäck","e","maskulin"),
    dekliniere_substantiv("NOUN_SCHLANGENGIFT","Schlangengift","es","Schlangengift","e","neutrum"),
    dekliniere_substantiv("NOUN_SCHOENSCHRIFT","Schönschrift","","Schönschrift","en","feminin"),
    dekliniere_substantiv("NOUN_SCHRIFT","Schrift","","Schrift","en","feminin"),
    dekliniere_substantiv("NOUN_SHIRT","Hemd","es","Hemd","en","neutrum"),
    dekliniere_substantiv("NOUN_SHOP","Laden","s","Läden","","maskulin"),
    dekliniere_substantiv("NOUN_SHOPS_CEILING","Ladendecke","","Ladendecke","en","feminin"),
    dekliniere_substantiv("NOUN_SILBER","Silber","s","","","neutrum"),
    dekliniere_substantiv("NOUN_SNACK","Snack","s","Snack","s","maskulin"),
    dekliniere_substantiv("NOUN_SCHWALL","Schwall","es","Schwall","e","maskulin"),
    dekliniere_substantiv("NOUN_SINN","Sinn","es","Sinn","e","maskulin"),
    dekliniere_substantiv("NOUN_SONDIERUNG","Sondierung","","Sondierung","en","feminin"),
    dekliniere_substantiv("NOUN_STOMACH","Magen","s","Mägen","","maskulin"),
    dekliniere_substantiv("NOUN_SUMMEN","Summen","s","","","neutrum"),
    dekliniere_substantiv("NOUN_TEMPLES_CEILING","Tempeldecke","","Tempeldecke","en","feminin"),
    dekliniere_substantiv("NOUN_TENTAKEL","Tentakel","s","Tentakel", "","maskulin"),
    dekliniere_substantiv("NOUN_TODESFELD","Todesfeld","es","Todesfeld","er","feminin"),
    dekliniere_substantiv("NOUN_TRAP","Falle","","Falle","en","feminin"),
    dekliniere_substantiv("NOUN_TREFFER","Treffer","s","Treffer","","maskulin"),
    dekliniere_substantiv("NOUN_UNFAEHIGKEIT","Unfähigkeit","","","","feminin"),
    dekliniere_substantiv("NOUN_UNSICHTBARKEIT","Unsichtbarkeit","","","","feminin"),
    dekliniere_substantiv("NOUN_VERSTAND","Verstand","es","","","maskulin"),
    dekliniere_substantiv("NOUN_VORFAHRE","Vorfahr","en","Vorfahr","en","maskulin"),
    dekliniere_substantiv("NOUN_REFLECTION", "Spiegelbild", "es", "Spiegelbild", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SPIDER_WEB", "Spinnennetz", "es", "Spinnennetz", "e", "neutrum"),
    dekliniere_substantiv("NOUN_STAIRS", "Treppe", "", "Treppe", "en", "feminin"),
    dekliniere_substantiv("NOUN_WATERS_SURFACE", "Wasseroberfläche", "", "Wasseroberfläche", "en", "feminin"),
    dekliniere_substantiv("NOUN_WEB","Netz","es","Netz","e","neutrum"),
    dekliniere_substantiv("NOUN_WIND","Wind","es","Wind","e","maskulin"),
    dekliniere_substantiv("NOUN_WOMAN", "Frau", "", "Frau", "en", "feminin"),
    dekliniere_substantiv("NOUN_WORM", "Wurm", "es", "Würm", "er", "maskulin"),

    "/* ======================================================= */",
    "/* Adjektive */",
    "/* ======================================================= */",
    dekliniere_adjektiv("ADJEKTIV_CURSED","verflucht"),
    dekliniere_adjektiv("ADJEKTIV_UNCURSED","nicht verflucht"),
    "  /* blessed mit geheiligt oder gesegnet uebersetzen? */",
    dekliniere_adjektiv("ADJEKTIV_BLESSED","geheiligt"),
    dekliniere_adjektiv("ADJEKTIV_BLESSED","gesegnet"),
    "",
    "  /* Adjektive aus role.c */",
		dekliniere_adjektiv("ADJEKTIV_MENSCHLICH", "menschlich"),
    dekliniere_adjektiv("ADJEKTIV_ELBISCH", "elbisch"),
    dekliniere_adjektiv("ADJEKTIV_ZWERGISCH", "zwergisch"),
    dekliniere_adjektiv("ADJEKTIV_GNOMISCH", "gnomisch"),
    dekliniere_adjektiv("ADJEKTIV_ORKSCH", "orksch"),
    "",
		dekliniere_adjektiv("ADJEKTIV_MAENNLICH", "männlich"),
    dekliniere_adjektiv("ADJEKTIV_WEIBLICH", "weiblich"),
    dekliniere_adjektiv("ADJEKTIV_SAECHLICH", "sächlich"),
    "",
    "/* Farbennamen */",
    dekliniere_adjektiv("ADJEKTIV_FARBE_BLACK","schwarz"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_AMBER","bernsteinfarben"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GOLDEN","golden"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_LIGHT_BLUE","hellblau"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_RED","rot"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GREEN","grün"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_SILVER","silbern"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BLUE","blau"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_PURPLE","purpurn"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_WHITE","weiß"),

    dekliniere_adjektiv("ADJEKTIV_FARBE_BROWN","braun"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_MAGENTA","tiefrot"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_CYAN","türkis"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GRAY","grau"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_TRANSPARENT","durchsichtig"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_ORANGE","orangen"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BRIGHT_GREEN","knallgrün"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_YELLOW","gelb"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BRIGHT_BLUE","knallblau"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BRIGHT_MAGENTA","knallrot"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BRIGHT_CYAN","blaugrün"),

    "",
    "/* 'andere' ist eigentlich ein Pronomen, wird aber wie ein Adjektiv dekliniert */",
    dekliniere_adjektiv("ADJEKTIV_ANDERE","ander"),
    "",
    "/* Adjektive und Substantive für Nominalphrasen */",
    dekliniere_substantiv("NOUN_STANGENWAFFE","Stangenwaffe","","Stangenwaffe","en","feminin"),
    dekliniere_adjektiv("ADJEKTIV_GEWOEHNLICH","gewöhnlich"),

    "/* Gesinnungsadjektive */",
    dekliniere_adjektiv("ADJEKTIV_CHAOTIC", "chaotisch"),
    dekliniere_adjektiv("ADJEKTIV_NEUTRAL", "neutral"),
    dekliniere_adjektiv("ADJEKTIV_LAWFUL", "rechtschaffen"),
    dekliniere_adjektiv("ADJEKTIV_UNALIGNED", "frei"),
    dekliniere_adjektiv("ADJEKTIV_UNKNOWN", "unbekannt"),
		"",
    dekliniere_adjektiv("ADJEKTIV_TAME","zahm"),
    dekliniere_adjektiv("ADJEKTIV_PEACEFUL","friedlich"),
    dekliniere_adjektiv("ADJEKTIV_HOSTILE","feindlich"),
    dekliniere_adjektiv("ADJEKTIV_HOSTILE","feindselig"),
    "",
    "/* sonstige Adjektive */",
    dekliniere_adjektiv("ADJEKTIV_ANGESENGT","angesengt"),
    dekliniere_adjektiv("ADJEKTIV_AETZEND","ätzend"),
    dekliniere_adjektiv("ADJEKTIV_BARE","bloß"),
    dekliniere_adjektiv("ADJEKTIV_BEHANDSCHUHT","behandschuht"),
    dekliniere_adjektiv("ADJEKTIV_BEISSEND","beissend"),
    dekliniere_adjektiv("ADJEKTIV_BLUTRUENSTIG","blutrünstig"),
    dekliniere_adjektiv("ADJEKTIV_BRENNEND","brennend"),
    dekliniere_adjektiv("ADJEKTIV_CARNIVOROUS","fleischfressend"),
    dekliniere_adjektiv("ADJEKTIV_EATEN","verspeist"),
    dekliniere_adjektiv("ADJEKTIV_EIGENE","eigen"),
    dekliniere_adjektiv("ADJEKTIV_EINSTUERZEND","einstürzend"),
    dekliniere_adjektiv("ADJEKTIV_ELEKTRISCH","elektrisch"),
    dekliniere_adjektiv("ADJEKTIV_ELEMENTAR","elementar"),
    dekliniere_adjektiv("ADJEKTIV_EXPLODIEREND","explodierend"),
    dekliniere_adjektiv("ADJEKTIV_EXPLODIERT","explodiert"),
    dekliniere_adjektiv("ADJEKTIV_FALLEND","fallend"),
    dekliniere_adjektiv("ADJEKTIV_FREI","frei"),
    dekliniere_adjektiv("ADJEKTIV_FLUESSIG","flüssig"),
    dekliniere_adjektiv("ADJEKTIV_FEST","fest"),
    dekliniere_adjektiv("ADJEKTIV_FETTIG","fettig"),
    dekliniere_adjektiv("ADJEKTIV_FEUCHT","feucht"),
    dekliniere_adjektiv("ADJEKTIV_GEFAEHRLICH","gefährlich"),
    dekliniere_adjektiv("ADJEKTIV_GERUCHLOS","geruchlos"),
    dekliniere_adjektiv("ADJEKTIV_GETRAGEN","getragen"),
    dekliniere_adjektiv("ADJEKTIV_GESAMT","gesamt"),
    dekliniere_adjektiv("ADJEKTIV_GIFTGETRAENKT","giftgetränkt"),
    dekliniere_adjektiv("ADJEKTIV_GIFTIG","giftig"),
    dekliniere_adjektiv("ADJEKTIV_GREASED","gefettet"),
    dekliniere_adjektiv("ADJEKTIV_GROSS","groß"),
    dekliniere_adjektiv("ADJEKTIV_INVISIBLE","unsichtbar"),
    dekliniere_adjektiv("ADJEKTIV_KUEHLEND","kühlend"),
    dekliniere_adjektiv("ADJEKTIV_KAPUTT","kaputt"),
    dekliniere_adjektiv("ADJEKTIV_KLEIN","klein"),
    dekliniere_adjektiv("ADJEKTIV_KONTAMINIERT","kontaminiert"),
    dekliniere_adjektiv("ADJEKTIV_KRAFTLOS","kraftlos"),
    dekliniere_adjektiv("ADJEKTIV_LEICHT","leicht"),
    dekliniere_adjektiv("ADJEKTIV_LETZT","letzt"),
    dekliniere_adjektiv("ADJEKTIV_LINK","link"),
    dekliniere_adjektiv("ADJEKTIV_MAGIE_ABSORBIEREND","magie-absorbierend"),
    dekliniere_adjektiv("ADJEKTIV_MAGISCH","magisch"),
    dekliniere_adjektiv("ADJEKTIV_MODERAT","moderat"),
    dekliniere_adjektiv("ADJEKTIV_MYTHICAL","sagenhaft"),
    dekliniere_adjektiv("ADJEKTIV_NACHGEWACHSEN","nachgewachsen"),
    dekliniere_adjektiv("ADJEKTIV_NEU","neu"),
    dekliniere_adjektiv("ADJEKTIV_PLUMP","plump"),
    dekliniere_adjektiv("ADJEKTIV_POISONED",""),
    dekliniere_adjektiv("ADJEKTIV_POOR","arm"),
    dekliniere_adjektiv("ADJEKTIV_RECHT","recht"),
    dekliniere_adjektiv("ADJEKTIV_REICH","reich"),
    dekliniere_adjektiv("ADJEKTIV_RIESIG","riesig"),
    dekliniere_adjektiv("ADJEKTIV_ROSTIG","rostig"),
    dekliniere_adjektiv("ADJEKTIV_SAEUREHALTIG","säurehaltig"),
    dekliniere_adjektiv("ADJEKTIV_SADDLED","gesattelt"),
    dekliniere_adjektiv("ADJEKTIV_SCHLIESSEND","schließend"),
    dekliniere_adjektiv("ADJEKTIV_SCHOEN","schön"),
    dekliniere_adjektiv("ADJEKTIV_SCHWACH","schwach"),
    dekliniere_adjektiv("ADJEKTIV_SELTSAM","seltsam"),
    dekliniere_adjektiv("ADJEKTIV_SOLCH","solch"),
    dekliniere_adjektiv("ADJEKTIV_STECHEND","stechend"),
    dekliniere_adjektiv("ADJEKTIV_STOFFIG","stoffig"),
    dekliniere_adjektiv("ADJEKTIV_TOEDLICH","tödlich"),
    dekliniere_adjektiv("ADJEKTIV_UNSICHTBAR","unsichtbar"),
    dekliniere_adjektiv("ADJEKTIV_VERLETZT","verletzt"),
    dekliniere_adjektiv("ADJEKTIV_WEITENTFERNT","weitentfernt"),
    dekliniere_adjektiv("ADJEKTIV_WOHLVERDIENT","wohlverdient"),
    dekliniere_adjektiv("ADJEKTIV_ZWEIHAENDISCH","zweihändisch"),

    dekliniere_adjektiv("ADJEKTIV_FARBE_BRAUN","braun"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_DARK_RED","dunkelrot"),
    dekliniere_adjektiv("ADJEKTIV_GLAESERN","gläsern"),
    dekliniere_adjektiv("ADJEKTIV_GOLDEN","golden"),
    dekliniere_adjektiv("ADJEKTIV_HOELZERN","hölzern"),
    dekliniere_adjektiv("ADJEKTIV_KNOECHERN","knöchern"),
    dekliniere_adjektiv("ADJEKTIV_LECKER","lecker"),
    dekliniere_adjektiv("ADJEKTIV_LEDERN","ledern"),
    dekliniere_adjektiv("ADJEKTIV_METALLEN","metallen"),
    dekliniere_adjektiv("ADJEKTIV_MINERALISCH","mineralisch"),
    dekliniere_adjektiv("ADJEKTIV_PAPIEREN","papieren"),
    dekliniere_adjektiv("ADJEKTIV_VERGAMMELT","vergammelt"),
    dekliniere_adjektiv("ADJEKTIV_NAHE","nah"),

    "",
  ].each { |n|
    nomen << n
  }

  nomen.each {|n| 
    n.each { |w|
      if w.is_a? String
        puts "  "+w
      else
        puts "  "+w.to_struct
      end
    }
    puts "\n"
  }
  puts "  {NULL, NULL, 0, 0, 0, 0}\n};"
end

# print everything
#puts "#ifndef _GERMAN_H\n#define _GERMAN_H 1\n#include <stddef.h>\n"
ausgabe_definitions
ausgabe_nouns
ausgabe_verbs
#puts "#endif /* !_GERMAN_H */"
