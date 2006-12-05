require 'set'

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
  else
    raise "Maskulin und feminin für adjektivisches Substantiv nicht implementiert"
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

class Verb
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
  return Verb.new(bezeichner, stamm)
end

def ausgabe_verbs
  puts "\nstruct verb_struct verben[] = {"
  [
    konjugiere_verb("VERB_SICH_FUEHLEN","fühl"),
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
    "",
    "/* unregelmässige Verben */",
    Verb.new("VERB_SEIN", "", "", ["bist", "ist", "seid", "sind"]),
    Verb.new("VERB_HAVE", "", "", ["hast", "hat", "habt", "haben"]),
    Verb.new("VERB_READ", "", "", ["liest", "liest", "lest","lesen"]),
    Verb.new("VERB_CAN", "", "", ["kannst", "kann", "könnt","können"]),
    Verb.new("VERB_KICK", "", "", ["trittst", "tritt", "tretet","treten"]),
    Verb.new("VERB_BITE", "", "", ["beißt", "beißt", "beißt","beißen"]),
    Verb.new("VERB_HIT", "", "", ["triffst", "trifft", "trefft", "treffen"]),
    Verb.new("VERB_SEE", "", "", ["siehst", "sieht", "seht", "sehen"]),
    Verb.new("VERB_CONTAIN", "", "", ["enthälst", "enthält", "enthaltet","enthalten"]),
    Verb.new("VERB_OPEN", "", "auf", ["gehst", "geht", "geht","gehen"]),
    Verb.new("VERB_EAT", "", "auf", ["isst", "isst", "esst","essen"]),
    Verb.new("VERB_DROP", "", "fallen", ["lässt", "lässt", "lasst","lassen"]),    # "You drop a potion."
    Verb.new("VERB_OBJECT_DROPS", "", "", ["fällst", "fällt", "fallt", "fallen"]), # e.g "A potion drops to the floor."
    Verb.new("VERB_SEEM","","zu sein", ["scheinst", "scheint","scheint", "scheinen"]),
    Verb.new("VERB_SCHLAGEN",  "","", ["schlägst", "schlägt","schlagen", "schlagt"]),
    Verb.new("VERB_ZUSCHLAGEN","","zu", ["schlägst", "schlägt","schlagen", "schlagt"]),
    Verb.new("VERB_STECHEN","","", ["stichst", "sticht","stecht", "stechen"]),
    Verb.new("VERB_ZUSTECHEN","","zu", ["stichst", "sticht","stecht", "stechen"]),
    Verb.new("VERB_SCHIESSEN", "", "", ["schießt", "schießt", "schießt","schießen"]),
    Verb.new("VERB_FANGEN", "", "", ["fängst", "fängt", "fangt","fangen"]),
    Verb.new("VERB_ABBRECHEN", "", "ab", ["brichst", "bricht", "brecht","brechen"]),
    Verb.new("VERB_EINATMEN", "", "ein", ["atmest", "atmet", "atmet","atmen"]),
    Verb.new("VERB_TRAGEN", "", "", ["trägst", "trägt", "tragt","tragen"]),
    Verb.new("VERB_KOENNEN", "", "", ["kannst", "kann", "könnt","können"]),
    Verb.new("VERB_ZITTERN", "", "", ["zitterst", "zittert", "zittert","zittern"]),
    Verb.new("VERB_GLAENZEN", "", "", ["glänzt", "glänzt", "glänzt","glänzen"]),
    Verb.new("VERB_NEHMEN", "", "", ["nimmst", "nimmt", "nehmt","nehmen"]),
    Verb.new("VERB_ZUSCHNUEREN", "", "zu", ["schnürst", "schnürt", "schnürt","schnüren"]),
    Verb.new("VERB_PASSEN", "", "", ["passt", "passt", "passt","passen"]),
    Verb.new("VERB_HALTEN", "", "", ["hälst", "hält", "haltet","halten"]),
    Verb.new("VERB_HINDERNT", "", "", ["hinderst", "hindert", "hindert","hindern"]),
    "",
    Verb.new("VERB_DACHTEST", "", "", ["dachtest", "dachte", "dachtet","dachten"]),
    Verb.new("VERB_HAETTEST", "", "", ["hättest", "hätte", "hättet","hätten"]),
    Verb.new("VERB_MOECHTEST", "", "", ["möchtest", "möchte", "möchtet","möchten"]),
    "",
    Verb.new("VERB_WORN", "getragen", "", ["", "", "", ""]),
    Verb.new("VERB_ZERBRECHEN", "", "", ["zerbrichst", "zerbricht", "zerbrecht", "zerbrechen"]),
  ].each { |v|   
    if v.is_a? String then
      puts v
    else
      v.to_struct.each { |vf| puts "  "+vf }
    end
  }
  puts "\n"
	puts "  {NULL, NULL, 0, 0}\n};"
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
    unregelmaessiges_wort("NOUN_CORPSE",  "die sterblichen Überreste", [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("NOUN_CORPSE",  "der sterblichen Überreste",  $gen,       $fem, $sg),
    unregelmaessiges_wort("NOUN_CORPSE",  "den sterblichen Überresten", $dat,       $fem, $sg),
    unregelmaessiges_wort("NOUN_CORPSEs",  "die sterblichen Überreste von", [$nom,$akk], $fem, $pl),
    unregelmaessiges_wort("NOUN_CORPSEs",  "der sterblichen Überreste von",  $gen,       $fem, $pl),
    unregelmaessiges_wort("NOUN_CORPSEs",  "den sterblichen Überresten von", $dat,       $fem, $pl),
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
    "",
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "du",    $nom,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "deiner",$gen,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "dir",   $dat,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "dich",  $akk,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "Ihr",   $nom,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "Euer",  $gen,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "Euch",  [$akk,$dat],[$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_3P_M_PERSONAL",  "er",    $nom,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_M_PERSONAL",  "seiner",$gen,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_M_PERSONAL",  "ihm",   $dat,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_M_PERSONAL",  "ihn",   $akk,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_PERSONAL",  "sie",   $nom,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_PERSONAL",  "ihrer", $gen,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_PERSONAL",  "ihr",   $dat,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_PERSONAL",  "sie",   $akk,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_N_PERSONAL",  "es",    $nom,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_N_PERSONAL",  "seiner",$gen,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_N_PERSONAL",  "ihm",   $dat,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_N_PERSONAL",  "es",    $akk,       [$mal,$fem,$neu], $sg),
    "",
    unregelmaessiges_wort("PRONOMEN_KEIN",  "kein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keine",  $nom,  $fem,       $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keiner", $gen, [$fem,$dat], $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keinem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keinen", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "keiner", $akk,  $fem,       $sg),
    unregelmaessiges_wort("PRONOMEN_KEIN",  "kein",   $akk,  $neu,       $sg),
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
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deinen", $dat, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_POSSESSIV", "deiner", $gen, [$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "sein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seinem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seinen", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "sein",   $akk,  $neu,       $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seine",  [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_3P_MN_POSSESSIV", "seiner", [$gen,$dat], $fem, $sg),
    "",
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihr",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihres", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihrem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihren", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihr",   $akk,  $neu,       $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihre",  [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_3P_F_POSSESSIV", "ihrer", [$gen,$dat], $fem, $sg),
    "",
    unregelmaessiges_wort("NOUN_OOPS",  "Hoppla",        [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]),
    "",
    unregelmaessiges_wort("PARTIKEL_NAMED",   "namens", [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]), # geannt, namens
    unregelmaessiges_wort("PARTIKEL_CALLED",  "genannt", [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]), # bezeichnet mit, genannt
    unregelmaessiges_wort("PARTIKEL_LABELED", "beschriftet", [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]), # bezeichnet mit, genannt
    unregelmaessiges_wort("PARTIKEL_VON", "von", [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl])
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
    "/* Nouns that are neither in mons-de.h nor in obj-de.h */",
    dekliniere_substantiv("NOUN_SPINACH","Spinat","es","Spinat","e","maskulin"),
    dekliniere_substantiv("NOUN_OBJ_DOOR", "Türe", "", "Türe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WALLET", "Geldbörse", "", "Geldbörse", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_MEAT", "Fleisch", "es", "", "e", "neutrum", ""),
    dekliniere_substantiv("NOUN_LUCK", "Glück", "es", "Glück", "e", "neutrum", "s"),
    dekliniere_substantiv("NOUN_GENDARMERIE", "Gendarmerie", "", "Gendarmerie", "en", "feminin"), 
    dekliniere_substantiv("NOUN_PAAR", "Paar", "es", "Paar", "e", "neutrum"), 
    dekliniere_substantiv("NOUN_PAAR", "Paar", "es", "Paar", "e", "neutrum"), 
    dekliniere_substantiv("NOUN_BOOTS", "Stiefel", "s", "Stiefel", "", "maskulin"), 
    dekliniere_substantiv("NOUN_GLOVES", "Handschuh", "es", "Handschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CLOAK", "Mantel", "s", "Mäntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_HELMET", "Helm", "es", "Helm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SHIELD", "Schild", "es", "Schild", "e", "maskulin"), 
    dekliniere_substantiv("NOUN_STONE", "Stein", "es", "Stein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SWORD", "Schwert", "es", "Schwert", "er", "neutrum"),
    "",
    dekliniere_substantiv("NOUN_HALS", "Hals", "es", "Häls", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BODY", "Körper", "s", "Körper", "", "maskulin"),
    "",
    dekliniere_substantiv("NOUN_VERSUCH", "Versuch", "es", "Versuch", "er", "maskulin"),
    dekliniere_substantiv("NOUN_FINGER", "Finger", "s", "Finger", "", "maskulin"),
    dekliniere_substantiv("NOUN_HAND", "Hand", "", "Händ", "e", "feminin"),
    dekliniere_substantiv("NOUN_HEAD", "Kopf", "es", "Köpf", "e", "maskulin"),
    dekliniere_substantiv("NOUN_LEG", "Bein", "es", "Bein", "e", "neutrum"),
    dekliniere_substantiv("NOUN_FACE", "Gesicht", "es", "Gesicht", "er", "neutrum"),
    "",
    "",

    dekliniere_substantiv("NOUN_SPE_SLEEP", "Schlaf", "es", "", "e", "maskulin"),

    dekliniere_substantiv("NOUN_WEAPON",    "Waffe",          "",   "Waffe",          "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ARMOR",     "Rüstzeug",        "es",   "Rüstzeug",      "e", "neutrum", "s"),
    #dekliniere_substantiv("NOUN_ARMOR",     "Rüstung",        "",   "Rüstung",      "en", "feminin"),
    dekliniere_substantiv("NOUN_ARMOR",     "Schutzbekleidung", "",   "Schutzbekleidung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_RING",      "Ring",           "es", "Ring",           "e",  "maskulin"),
    dekliniere_substantiv("NOUN_AMULET",    "Amulett",        "es", "Amulett",        "e",  "neutrum"),
    dekliniere_substantiv("NOUN_TOOL",      "Werkzeug",       "es", "Werkzeug",       "e",  "neutrum", "s"),
    "/* This is wrong. 'Essbares' should be inflected like an adjective */",
    dekliniere_adjektivisches_substantiv("NOUN_COMESTIBLE","Essbar", "neutrum"),
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
    #dekliniere_substantiv("NOUN_JAVELIN" # check Wurfspieß oder Pilum
    dekliniere_substantiv("NOUN_TRIDENT", "Dreizack", "s", "Dreizack", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DAGGER", "Dolch", "es", "Dolch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ELVEN_DAGGER", "Elbendolch", "es", "Elbendolch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_DAGGER", "Orkdolch", "es", "Orkdolch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SILVER_DAGGER", "Silberdolch", "es", "Silberdolch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ATHAME", "Athame", "", "Athame", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_SCALPEL", "Skalpell", "s", "Skalpell", "e", "neutrum"),
    dekliniere_substantiv("NOUN_KNIFE", "Messer", "s", "Messer", "", "neutrum", "s"),
    dekliniere_substantiv("NOUN_STILETTO", "Stilett", "s", "Stilett", "e", "neutrum"),
    #dekliniere_substantiv("NOUN_WORM_TOOTH"
    #dekliniere_substantiv("NOUN_CRYSKNIFE"
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
    #dekliniere_substantiv("NOUN_MUMMY_WRAPPING"
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
    #dekliniere_substantiv("NOUN_CLOAK_OF_DISPLACEMENT"
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
    dekliniere_substantiv("NOUN_LEVITATION_BOOTS", "Flugschuh", "es", "Fluschuh", "e", "maskulin"),
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
    #dekliniere_substantiv("NOUN_WAND_MAGIC_MISSILE", "Graben", "s", "", "", "neutrum"),  # magische Geschosse
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
    #dekliniere_substantiv("NOUN_RING_SHOCK_RESISTANCE"
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
    dekliniere_substantiv("NOUN_SACK", "Sack", "es", "Säck", "e", "maskulin"),
    dekliniere_substantiv("NOUN_OILSKIN_SACK","Ölzeugbeutel", "s", "Ölzeugbeutel", "", "maskulin"),  # Wachstuch, Oilskin, Oelzeug, Regenhaut
    dekliniere_nominalphrase("NOUN_BAG_OF_HOLDING", "Nimmervoll", "Beutel","s","Beutel","","maskulin"),
    dekliniere_substantiv("NOUN_BAG_OF_TRICKS", "Trickbeutel", "s", "Trickbeutel","","maskulin"), 
    # dekliniere_substantiv("NOUN_SKELETON_KEY", "Generalschlüssel", "s", "Generalschlüssel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SKELETON_KEY", "Passepartout", "s", "Passepartout", "", "maskulin"),
    dekliniere_substantiv("NOUN_LOCK_PICK", "Dietrich", "s", "Dietrich", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CREDIT_CARD", "Kreditkarte", "", "Kreditkarte", "en", "feminin"),
    dekliniere_substantiv("NOUN_TALLOW_CANDLE", "Talglicht", "es", "Talglicht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_WAX_CANDLE", "Wachskerze", "", "Wachskerze", "en", "feminin"),
    dekliniere_substantiv("NOUN_BRASS_LANTERN", "Laterne", "", "Laterne", "en", "feminin"),
    dekliniere_substantiv("NOUN_OIL_LAMP", "Öllampe", "", "Öllampe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_MAGIC_LAMP", "Wunderlampe", "", "Wunderlampe", "en", "feminin", "n"),
    dekliniere_nominalphrase("NOUN_EXPENSIVE_CAMERA", "teur", "Photoapparat", "es", "Photoapparat", "e", "maskulin", "s"),
    "/* FIX ME: NOUN_LOOKING_GLASS should be different from NOUN_MIRROR */",
    dekliniere_substantiv("NOUN_MIRROR","Spiegel","","Spiegel","","maskulin"), # FIX ME
    dekliniere_substantiv("NOUN_CRYSTAL_BALL", "Kristallkugel", "", "Kristallkugel", "en", "feminin"),
    dekliniere_substantiv("NOUN_LENSES", "Kontaktlinse", "", "Kontaktlinse", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_BLINDFOLD", "Augenbinde",   "",   "Augenbinde",   "en", "feminin"),
    dekliniere_substantiv("NOUN_TOWEL", "Handtuch", "es", "Handtüch", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SADDLE", "Sattel", "s", "Sättel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEASH", "Leine", "", "Leine", "en", "feminin"),
    dekliniere_substantiv("NOUN_STETHOSCOPE", "Stethoskop", "s", "Stethoskop", "e", "neutrum"),
    #dekliniere_substantiv("NOUN_TINNING_KIT", 
    dekliniere_substantiv("NOUN_TIN_OPENER", "Dosenöffner", "s", "Dosenöffner", "", "maskulin"),
    #dekliniere_substantiv("NOUN_CAN_OF_GREASE",  # Fettdose?
    dekliniere_substantiv("NOUN_FIGURINE", "Statuette", "", "Statuette", "en", "feminin"),
    #dekliniere_substantiv("NOUN_MAGIC_MARKER", # verzauberter Fueller?
    "/* 'Landmine' is a more generic term. I think 'Tretmine' is the more exact terminus for the mines in Nethack. */",
    dekliniere_substantiv("NOUN_LAND_MINE", "Tretmine", "", "Tretmine", "en", "feminin"),
    dekliniere_substantiv("NOUN_BEARTRAP", "Bärenfalle", "", "Bärenfalle", "en", "feminin", "n"),
    #dekliniere_substantiv("NOUN_TIN_WHISTLE", 
    #dekliniere_substantiv("NOUN_MAGIC_WHISTLE", 
    #dekliniere_substantiv("NOUN_WOODEN_FLUTE", 
    #dekliniere_substantiv("NOUN_MAGIC_FLUTE", 
    #dekliniere_substantiv("NOUN_TOOLED_HORN", 
    dekliniere_substantiv("NOUN_FROST_HORN", "Eishorn", "es", "Eishörn", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_FIRE_HORN", 
    dekliniere_substantiv("NOUN_HORN_OF_PLENTY", "Füllhorn", "es", "Füllhorn", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_WOODEN_HARP", 
    #dekliniere_substantiv("NOUN_MAGIC_HARP", 
    dekliniere_substantiv("NOUN_BELL", "Glöckchen", "s", "Glöckchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_BUGLE", "Waldhorn", "es", "Waldhörn", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_LEATHER_DRUM", 
    #dekliniere_substantiv("NOUN_DRUM_OF_EARTHQUAKE", 
    dekliniere_substantiv("NOUN_PICK_AXE", "Spitzhacke", "", "Spitzhacke", "en", "feminin"),
    #dekliniere_substantiv("NOUN_GRAPPLING_HOOK", 
    #dekliniere_substantiv("NOUN_UNICORN_HORN", 
    #dekliniere_substantiv("NOUN_CANDELABRUM_OF_INVOCATION", 
    #dekliniere_substantiv("NOUN_BELL_OF_OPENING", 
    "", 
    "/* Tools, unidentified */",
		dekliniere_substantiv("NOUN_BAG","Beutel","s","Beutel","","maskulin"),
    dekliniere_substantiv("NOUN_KEY","Schlüssel","s","Schlüssel","","maskulin"),
    dekliniere_substantiv("NOUN_CANDLE","Kerze","","Kerze","en","feminin"),
    dekliniere_substantiv("NOUN_LAMP","Lampe","","Lampe","en","feminin"),
    "/* FIX ME: NOUN_LOOKING_GLASS should be different from NOUN_MIRROR */",
    dekliniere_substantiv("NOUN_LOOKING_GLASS","Spiegel","","Spiegel","","maskulin"), # FIX ME
    #dekliniere_substantiv("NOUN_GLASS_ORB","","","","","maskulin"), # Glaskugel?
    dekliniere_substantiv("NOUN_WHISTLE","Pfeife","","Pfeife","en","feminin"),
    dekliniere_substantiv("NOUN_FLUTE","Flöte","","Flöte","en","feminin"),
    dekliniere_substantiv("NOUN_HORN","Horn","es","Hörn","er","neutrum"),
    dekliniere_substantiv("NOUN_HARP","Harfe","","Harfe","en","feminin"),
    dekliniere_substantiv("NOUN_DRUM","Trommel","","Trommel","en","feminin"),
    #dekliniere_substantiv("NOUN_IRON_HOOK","","","","","maskulin"),
    dekliniere_substantiv("NOUN_CANDELABRUM","Kronleuchter","s","Kronleuchter","","maskulin"),
    dekliniere_substantiv("NOUN_SILVER_BELL","Silberglocke","","Silberglocke","en","feminin"),
    "",
    "/* Food */",
    "/* Kutteln, Kaldaunen, Pansen, Wanst. I choose \"Pansen\". This is the name under which it is usually sold for pets. */",
    dekliniere_substantiv("NOUN_TRIPE_RATION", "Pansen", "s",  "Pansen", "", "maskulin"),
    "/* NOUN_CORPSE is irregular and treated specially */",
    dekliniere_substantiv("NOUN_CORPSE","Leiche","","Leiche","en","feminin"),
    dekliniere_substantiv("NOUN_CORPSE","Leichnam","s","Leichnam","e","feminin"),
    dekliniere_substantiv("NOUN_EGG","Ei","es","Ei","er","neutrum"),
    #dekliniere_substantiv("NOUN_MEATBALL" # Kloesschen, Fleischklops, Fleischball?
    #dekliniere_substantiv("NOUN_MEAT_STICK",Salami?
    #dekliniere_substantiv("NOUN_HUGE_CHUNK_OF_MEAT"
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
    #dekliniere_substantiv("NOUN_POT_GAIN_ABILITY" 
    #dekliniere_substantiv("NOUN_POT_RESTORE_ABILITY"
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
    #dekliniere_substantiv("NOUN_SCR_DESTROY_ARMOR"   # Schriftrolle der zerstörten Rüstung?
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
    dekliniere_substantiv("NOUN_SCR_FOOD_DETECTION", "Nahrungerspürung", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_SCR_IDENTIFY", "Identifizierung", "", "", "en", "feminin", "s"),
    dekliniere_nominalphrase("NOUN_SCR_MAGIC_MAPPING", "magisch", "Kartieren", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SCR_AMNESIA", "Amnesie", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_SCR_FIRE", "Feuer", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SCR_EARTH", "Erde", "", "", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SCR_PUNISHMENT", "Bestrafung", "", "", "en", "feminin", "s"),
    #dekliniere_substantiv("NOUN_SCR_CHARGING"       # Laden, Ladungs??
    #dekliniere_substantiv("NOUN_SCR_STINKING_CLOUD"
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
    #dekliniere_substantiv("NOUN_SCR_DUAM_XNAHT", "", "", "", "fremder", "maskulin"),
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
    "/* 'gebogen' ist IMO besser als 'gekrümmt'*/",
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
    dekliniere_substantiv("NOUN_GEM_LUCKSTONE", "Glücksstein", "es", "Glücksstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_LOADSTONE", "Teufelsstein", "es", "Teufelsstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_TOUCHSTONE", "Prüfstein", "es", "Prüfstein", "e", "maskulin"),
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
    #dekliniere_substantiv("NOUN_BLINDING_VENOM"
    "",
    "/* Other objects, unidentified */",
    #dekliniere_substantiv("NOUN_SPLASH_OF_VENOM"
    #dekliniere_substantiv("NOUN_ACID_VENOM"
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
    dekliniere_substantiv("NOUN_GELATINOUS_CUBE", "Gelatinewürfel", "s", "Gelatinewürfel", "", "maskulin"),
    dekliniere_substantiv("NOUN_CHICKATRICE", "Kikatrix", "", "Kikatrices", "fremder", "neutrum"), # Kikatrix? nach Adelung niedersächsisch kiken/küken (angelsächsisch cicen, englisch chicken) für Küchlein (neuhochdeutsch Küken)
    dekliniere_substantiv("NOUN_COCKATRICE", "Cocatrix", "", "Cocatrices", "fremder", "maskulin"), # Cocatrix? mittelalterliches Latein -> http://www.bartleby.com/61/43/C0444300.html
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
    dekliniere_substantiv("NOUN_FLOATING_EYE", "Schwebaug", "es", "Schwebaug", "en", "neutrum", "n"), # Schwebäugler?, Schwebäuger?, was mit Ocellus?, nuräeugler, flugäugler, flugauge,
    #dekliniere_substantiv("NOUN_FREEZING_SPHERE" # 
    #dekliniere_substantiv("NOUN_FLAMING_SPHERE"  # brennende Kugel
    #dekliniere_substantiv("NOUN_SHOCKING_SPHERE"
    dekliniere_substantiv("NOUN_BEHOLDER", "Betrachter", "s", "Betrachter", "", "maskulin"), # Betrachter
    dekliniere_substantiv("NOUN_KITTEN", "Kätzchen", "s", "Kätzchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_HOUSECAT","Hauskatze","","Hauskatze","en","feminin","n"),
    dekliniere_substantiv("NOUN_JAGUAR","Jaguar","s","Jaguar","e","maskulin"),
    dekliniere_substantiv("NOUN_LYNX","Luchs","es","Luchs","e","maskulin"),
    dekliniere_substantiv("NOUN_PANTHER","Panther","s","Panther","","maskulin"),
    dekliniere_nominalphrase("NOUN_LARGE_CAT", "groß", "Katze","","Katze","en","feminin","n"),
    dekliniere_substantiv("NOUN_TIGER","Tiger","s","Tiger","","maskulin"),
    dekliniere_substantiv("NOUN_GREMLIN", "Gremlin", "s", "Gremlin", "s", "maskulin"),
    #dekliniere_substantiv("NOUN_GARGOYLE", "Gargoyle", "s", "Gargoyle", "s", "maskulin"), # Gargoyle?, Gargil?, Gargula?, Gargyle?
    dekliniere_nominalphrase("NOUN_WINGED_GARGOYLE", "geflügelt", "Gargoyle", "s", "Gargoyle", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HOBBIT", "Hobbit", "s", "Hobbit", "s", "maskulin"),
    dekliniere_substantiv("NOUN_DWARF","Zwerg","es","Zwerg","e","maskulin", "en"),
    #dekliniere_substantiv("NOUN_BUGBEAR" Grottenschrat
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
    #dekliniere_substantiv("NOUN_LEPRECHAUN"
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
    #dekliniere_substantiv("NOUN_RABID_RAT"
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
    #dekliniere_substantiv("NOUN_BABY_LONG_WORM"
    #dekliniere_substantiv("NOUN_BABY_PURPLE_WORM"
    #dekliniere_substantiv("NOUN_LONG_WORM"
    #dekliniere_substantiv("NOUN_PURPLE_WORM"
    #dekliniere_substantiv("NOUN_GRID_BUG"
    #dekliniere_substantiv("NOUN_XAN"
    dekliniere_nominalphrase("NOUN_YELLOW_LIGHT", "gelb", "Licht", "es", "Licht", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_BLACK_LIGHT", "schwarz", "Licht", "es", "Licht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_ZRUTY", "Zruty", "s", "Zruty", "s", "maskulin"),
    dekliniere_substantiv("NOUN_COUATL", "Coatl", "", "Coatl", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_ALEAX", "Aleax", "", "Aleax", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_ANGEL", "Engel", "s", "Engel", "", "maskulin"),
    dekliniere_substantiv("NOUN_KI_RIN", "Kirin", "s", "Kirin", "s", "neutrum"),
    dekliniere_substantiv("NOUN_ARCHON", "Archon", "s", "Archont", "en", "maskulin"),
    dekliniere_substantiv("NOUN_BAT", "Fledermaus", "", "Fledermäus", "e", "feminin"),
    dekliniere_substantiv("NOUN_GIANT_BAT", "Riesenfledermaus", "", "Riesenfledermäus", "e", "feminin"),
    dekliniere_substantiv("NOUN_RAVEN", "Rabe", "en", "Rabe", "en", "maskulin"),
    dekliniere_substantiv("NOUN_VAMPIRE_BAT", "Vampirfledermaus", "", "Vampirfledermäus", "e", "feminin"),
    dekliniere_substantiv("NOUN_PLAINS_CENTAUR", "Steppenkentaur", "en", "Steppenkentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_FOREST_CENTAUR", "Waldkentaur", "en", "Waldkentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_MOUNTAIN_CENTAUR", "Bergkentaur", "en", "Bergkentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_BABY_GRAY_DRAGON", "Babygraudrache", "en", "Babygraudrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BABY_SILVER_DRAGON", "Babysilberdrache", "en", "Babysilberdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BABY_SHIMMERING_DRAGON", "Babyglanzdrache", "en", "Babyglanzdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BABY_RED_DRAGON", "Babyrotdrache", "en", "Babyrotdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BABY_WHITE_DRAGON", "Babyweißdrache", "en", "Babyweißdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BABY_ORANGE_DRAGON", "Babyorangedrache", "en", "Babyorangedrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BABY_BLACK_DRAGON", "Babyschwarzdrache", "en", "Babyschwarzdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BABY_BLUE_DRAGON", "Babyblaudrache", "en", "Babyblaudrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BABY_GREEN_DRAGON", "Babygründrache", "en", "Babygründrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BABY_YELLOW_DRAGON", "Babygelbdrache", "en", "Babygelbdrache", "en", "maskulin", "n"),
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
    # dekliniere_substantiv("NOUN_STALKER", "Nachsteller", "s", "Nachsteller", "", "maskulin"),
    dekliniere_substantiv("NOUN_STALKER", "Pirscher", "s", "Pirscher", "", "maskulin"), 
    dekliniere_substantiv("NOUN_AIR_ELEMENTAL",   "Luftelementar",   "es", "Luftelementar",   "e", "neutrum"),
    dekliniere_substantiv("NOUN_FIRE_ELEMENTAL",  "Feuerelementar",  "es", "Feuerelementar",  "e", "neutrum"),
    dekliniere_substantiv("NOUN_EARTH_ELEMENTAL", "Erdelementar",    "es", "Erdelementar",    "e", "neutrum"),
    dekliniere_substantiv("NOUN_WATER_ELEMENTAL", "Wasserelementar", "es", "Wasserelementar", "e", "neutrum"),
    dekliniere_substantiv("NOUN_LICHEN", "Flechte", "", "Flechte", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BROWN_MOLD", "Braunschimmel", "s", "Braunschimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_YELLOW_MOLD", "Gelbschimmel", "s", "Gelbschimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_GREEN_MOLD", "Grünschimmel", "s", "Grünschimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_RED_MOLD", "Rotschimmel", "s", "Rotschimmel", "", "maskulin"),
    #dekliniere_substantiv("NOUN_SHRIEKER" kreischen, schreien: Kreischer, Schreier, Schreihals, Krakeeler
    dekliniere_nominalphrase("NOUN_VIOLET_FUNGUS", "violett", "Pilz", "es", "Pilz", "e", "maskulin", ""),
    dekliniere_substantiv("NOUN_GNOME", "Gnom", "en", "Gnom", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_GNOME_LORD","Gnomenfürst","en","Gnomenfürst","en","maskulin", "en"),
    #dekliniere_substantiv("NOUN_GNOMISH_WIZARD"
    dekliniere_substantiv("NOUN_GNOME_KING","Gnomenkönig","es","Gnomenkönig","e","maskulin"),
    dekliniere_substantiv("NOUN_GIANT",       "Riese", "en", "Riese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_STONE_GIANT", "Steinriese", "en", "Steinriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_HILL_GIANT",  "Hügelriese", "en", "Hügelriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_FIRE_GIANT",  "Feuerriese", "en", "Feuerriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_FROST_GIANT", "Frostriese", "en", "Frostriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_STORM_GIANT", "Strumriese", "en", "Strumriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_ETTIN", "Ettin", "", "Ettin", "", "maskulin"),
    dekliniere_substantiv("NOUN_TITAN", "Titane", "en", "Titan", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_MINOTAUR","Minotaurus","","Minotaur","en","maskulin"),
    dekliniere_substantiv("NOUN_JABBERWOCK", "Jammerwoch", "s", "Jammerwoch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_VORPAL_JABBERWOCK", "Vorpahljammerwoch", "s", "Vorpahljammerwoch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_KEYSTONE_KOP", "Gendarm", "en", "Gendarmen", "en", "maskulin"), 
    dekliniere_substantiv("NOUN_KOP_SERGEANT", "Inspektor", "s", "Inspektor", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_KOP_LIEUTENANT", "Gruppeninspektor", "es", "Gruppeninspektor", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_KOP_KAPTAIN", "Chefinspektor", "es", "Chefinspektor", "en", "maskulin", "en"),
    #dekliniere_substantiv("NOUN_LICH"        # Liechnam?, Leichnam?
    #dekliniere_substantiv("NOUN_DEMILICH"    # Demiliechnam?, Halbliechnam?
    #dekliniere_substantiv("NOUN_MASTER_LICH" # Meisterliechnam
    #dekliniere_substantiv("NOUN_ARCH_LICH"   # Erzliechnam. Erzlichnam?
    dekliniere_substantiv("NOUN_KOBOLD_MUMMY", "Koboldsmumie", "", "Koboldsmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GNOME_MUMMY", "Gnomenmumie", "", "Gnomenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ORC_MUMMY", "Orkmumie", "", "Orkmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_DWARF_MUMMY", "Zwergenmumie", "", "Zwergenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ELF_MUMMY", "Elbenmumie", "", "Elbenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_HUMAN_MUMMY", "Menschenmumie", "", "Menschenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ETTIN_MUMMY", "Ettinmumie", "", "Ettinmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GIANT_MUMMY", "Riesenmumie", "", "Riesenmumie", "en", "feminin", "n"),
    #dekliniere_substantiv("NOUN_RED_NAGA_HATCHLING"
    #dekliniere_substantiv("NOUN_BLACK_NAGA_HATCHLING"
    #dekliniere_substantiv("NOUN_GOLDEN_NAGA_HATCHLING"
    #dekliniere_substantiv("NOUN_GUARDIAN_NAGA_HATCHLING"
    dekliniere_substantiv("NOUN_RED_NAGA", "Naga", "s", "Naga", "s", "feminin"), # Rotnaga, rote Naga, roter Naga?
    #dekliniere_substantiv("NOUN_BLACK_NAGA" # Schwarznaga
    #dekliniere_substantiv("NOUN_GOLDEN_NAGA" # Goldnaga oder goldene Naga
    #dekliniere_substantiv("NOUN_GUARDIAN_NAGA" # Wächternaga, Hüternaga
    dekliniere_substantiv("NOUN_OGRE", "Oger", "s", "Oger", "", "maskulin"),
    dekliniere_substantiv("NOUN_OGRE_LORD","Ogerfürst","en","Ogerfürst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_OGRE_KING","Ogerkönig","es","Ogerkönig","e","maskulin"),
    dekliniere_nominalphrase("NOUN_GRAY_OOZE", "Grau", "Schlamm", "es", "Schlamm", "e", "maskulin"), # grauer Schleim, Schlick?
    dekliniere_nominalphrase("NOUN_BROWN_PUDDING", "Braun", "Grütze", "", "Grütze", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_BLACK_PUDDING", "Schwarz", "Grütze", "", "Grütze", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_GREEN_SLIME", "Grün", "Schleim", "es", "Schleim", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_QUANTUM_MECHANIC"
    #dekliniere_substantiv("NOUN_RUST_MONSTER"
    #dekliniere_substantiv("NOUN_DISENCHANTER"
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
    #dekliniere_substantiv("NOUN_UMBER_HULK"
    dekliniere_substantiv("NOUN_VAMPIRE","Vampir","es","Vampir","e","maskulin"),
    #dekliniere_substantiv("NOUN_VAMPIRE_LORD"
    #dekliniere_substantiv("NOUN_VAMPIRE_MAGE"
    #dekliniere_substantiv("NOUN_VLAD_THE_IMPALER"
    #dekliniere_substantiv("NOUN_BARROW_WIGHT" # Grabunhold?
    #dekliniere_substantiv("NOUN_WRAITH"
    #dekliniere_substantiv("NOUN_NAZGUL" Nazgûl
    #dekliniere_substantiv("NOUN_XORN"
    #dekliniere_substantiv("NOUN_MONKEY", "Affe", "en", "Affe", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_APE", "Menschenaffe", "en", "Menschenaffe", "en", "maskulin"), # einer Menschenaffen stattdessen? Gibbon, Orang-Utan, Gorilla, Schimpanse. Gorilla scheint passend (ist auch der Vetegarischste der Menschenaffen)
    dekliniere_substantiv("NOUN_OWLBEAR", "Eulenbär", "en", "Eulenbär", "en", "maskulin"),
    dekliniere_substantiv("NOUN_YETI","Yeti","s","Yeti","s","maskulin"),
    #dekliniere_substantiv("NOUN_CARNIVOROUS_APE"
    #dekliniere_substantiv("NOUN_SASQUATCH", "Sasquatch", "", "Sasquatch", "e", "maskulin"),
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
    dekliniere_substantiv("NOUN_GUARD", "Wache", "", "Wache", "en", "feminin"),
    #dekliniere_substantiv("NOUN_PRISONER"
    dekliniere_substantiv("NOUN_ORACLE", "Orakel", "s", "Orakel", "", "neutrum"),
    #dekliniere_substantiv("NOUN_ALIGNED_PRIEST"
    dekliniere_substantiv("NOUN_HIGH_PRIEST", "Hohepriester", "s", "Hohepriester", "", "maskulin"),
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
    dekliniere_substantiv("NOUN_MEDUSA", "Medusa", "", "", "", "feminin"),
    #dekliniere_substantiv("NOUN_WIZARD_OF_YENDOR"
    dekliniere_substantiv("NOUN_CROESUS", "Krösus", "es", "Krösus", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_CHARON"
    dekliniere_substantiv("NOUN_GHOST", "Gespenst", "es", "Gespenst", "er", "neutrum", "er"),
    dekliniere_substantiv("NOUN_SHADE", "Nachtschatten", "s", "Nachtschatten", "", "maskulin"),
    #dekliniere_substantiv("NOUN_WATER_DEMON" # Wasserteufel
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
    #dekliniere_substantiv("NOUN_PIT_FIEND"
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
    dekliniere_substantiv("NOUN_ARCHEOLOGIST", "Archäologe", "en", "Archäologe", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BARBARIAN", "Barbar", "en", "Barbar", "en", "maskulin"),
    #dekliniere_substantiv("NOUN_CAVEMAN"
    #dekliniere_substantiv("NOUN_CAVEWOMAN"
    #dekliniere_substantiv("NOUN_HEALER"
    #dekliniere_substantiv("NOUN_KNIGHT"
    dekliniere_substantiv("NOUN_MONK", "Mönch", "es", "Mönch", "e", "maskulin", "en"),
    dekliniere_substantiv("NOUN_PRIEST", "Priester", "s", "Priester", "", "maskulin"),
    dekliniere_substantiv("NOUN_PRIESTESS", "Priesterin", "", "Priesterinn", "en", "feminin","nen"),
    #dekliniere_substantiv("NOUN_RANGER"
    #dekliniere_substantiv("NOUN_ROGUE"
    dekliniere_substantiv("NOUN_SAMURAI", "Samurai", "", "Samurai", "", "maskulin"),
    dekliniere_substantiv("NOUN_TOURIST", "Tourist", "en", "Tourist", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_VALKYRIE", "Walküre", "", "Walküre", "en", "feminin", "n"),
    #dekliniere_substantiv("NOUN_WIZARD"
    #dekliniere_substantiv("NOUN_LORD_CARNARVON"
    #dekliniere_substantiv("NOUN_PELIAS"
    #dekliniere_substantiv("NOUN_SHAMAN_KARNOV"
    #dekliniere_substantiv("NOUN_EARENDIL"
    #dekliniere_substantiv("NOUN_ELWING"
    #dekliniere_substantiv("NOUN_HIPPOCRATES"
    #dekliniere_substantiv("NOUN_KING_ARTHUR"
    dekliniere_substantiv("NOUN_GRAND_MASTER", "Großmeister", "s", "Großmeister", "", "maskulin"), 
    #dekliniere_substantiv("NOUN_ARCH_PRIEST", "Erzpriester", "s", "Erzpriester", "", "maskulin),
    dekliniere_substantiv("NOUN_ARCH_PRIEST", "Archipresbyter", "s", "Archipresbyter", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORION", "Orion", "s", "", "", "maskulin"),
    #dekliniere_substantiv("NOUN_MASTER_OF_THIEVES"
    #dekliniere_substantiv("NOUN_LORD_SATO"
    #dekliniere_substantiv("NOUN_TWOFLOWER"
    #dekliniere_substantiv("NOUN_NORN"
    #dekliniere_substantiv("NOUN_NEFERET_THE_GREEN"
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
    "/* Adjektive */",
    dekliniere_adjektiv("ADJEKTIV_CURSED","verflucht"),
    dekliniere_adjektiv("ADJEKTIV_UNCURSED","nicht verflucht"),
    "  /* blessed mit geheiligt oder gesegnet uebersetzen? */",
    dekliniere_adjektiv("ADJEKTIV_BLESSED","geheiligt"),
    "",
    "/* sonstige Adjektive */",
    dekliniere_adjektiv("ADJEKTIV_EATEN","verspeist"),
    dekliniere_adjektiv("ADJEKTIV_SADDLED","gesattelt"),
    dekliniere_adjektiv("ADJEKTIV_INVISIBLE","unsichtbar"),
    dekliniere_adjektiv("ADJEKTIV_BARE","bloß"),
    dekliniere_adjektiv("ADJEKTIV_ZWEIHAENDIG","zweihändig"),
    "",
    "/* 'andere' ist eigentlich ein Pronomen, wird aber wie ein Adjektiv dekliniert */",
    dekliniere_adjektiv("ADJEKTIV_ANDERE","ander"),
    "",
    "/* Adjektive und Substantive für Nominalphrasen */",
    dekliniere_substantiv("NOUN_STANGENWAFFE","Stangenwaffe","","Stangenwaffe","en","feminin"),
    dekliniere_adjektiv("ADJEKTIV_GEWOEHNLICH","gewöhnlich")
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
