
require 'set' #

# where possible, use the genitive-e ("des Zauberbuches" vs. "des Zauberbuchs"
$genitiv_e = true;
# where possible, use the dativ-e ("dem Zauberbuche" vs. "dem Zauberbuch"
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

def ausgabe_definitions
  defs = "
enum Casus   { nominativ=1, genitiv=2, dativ=4, akkusativ=8 };
enum Genus   { maskulin=1, feminin=2, neutrum=4 };
enum Numerus { n_singular=1, n_plural=2 };
enum Person  { erstePerson=1, zweitePerson=2, drittePerson=4 };
enum Artikel { ohne=1, bestimmter=2, unbestimmter=4 };

struct substantiv {
	const char  *wort;
	const char  *typ;
	enum Casus   casus;
	enum Genus   genus;
	enum Numerus numerus;
};


struct verb {
	const char *verb;
	const char *typ;
	enum Person person;
	enum Numerus numerus;
	enum Casus casus;
};

struct adjektiv {
	const char  *wort;
	const char  *typ;
	enum Casus   casus;
	enum Genus   genus;
	enum Numerus numerus;
	enum Artikel artikel;
};
"

puts defs
end



class Nomen
  attr_accessor :wort, :bezeichner, :casus, :geschlecht, :numerus
  def initialize(w,b,c,g,n)
    @wort = w
    @bezeichner = b
    @casus = Set.new
    if c.kind_of?(Array) then
      c.each { |ca| @casus << ca }
    else
      @casus << c
    end

    @geschlecht = Set.new
    if g.kind_of?(Array) then
      g.each { |ge| @geschlecht << ge }
    else 
      @geschlecht << g
    end

    @numerus = Set.new
    if n.kind_of?(Array) then
      n.each { |nu| @numerus << nu }
    else
      @numerus << n
    end
  end

  def add_to_set(set, array_or_string)
    if array_or_string.kind_of?(Array) then
      array_or_string.each { |el| set << el }
    else
      set << n
    end
  end

  def merge?(other)
    return (other.wort == @wort  and other.bezeichner == @bezeichner and
      other.geschlecht == @geschlecht and
      other.numerus == @numerus)
  end

  def merge(other)
    other.casus.each { |c|
      @casus << c
    }
  end

  def to_struct
    return '{"'+@wort+'", "'+@bezeichner+'", '+@casus.to_a.join("|")+', '+@geschlecht.to_a.join("|")+', '+@numerus.to_a.join("|")+'},'
  end

  def to_s
    return @wort+" "+@bezeichner+" "+@casus.to_a.join("|")+" "+@geschlecht.to_a.join("|")+" "+@numerus.to_a.join("|")
  end

end

def dekliniere_substantiv(bezeichner, singularstamm, genitiv_singular_endung, pluralstamm, nominativ_plural_endung, geschlecht)
  casus = Hash.new
  casus[$nom] = Hash.new
  casus[$gen] = Hash.new
  casus[$dat] = Hash.new
  casus[$akk] = Hash.new

  numerus = Hash.new
  numerus[$sg] = singularstamm
  numerus[$pl] = pluralstamm

  case genitiv_singular_endung
  when "es"
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = ""
    casus[$dat][$sg] = $dativ_e ? "e" : ""
    casus[$gen][$sg] = $genitiv_e ? "es" : "s"
  when "s" 
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = ""
    casus[$dat][$sg] = ""
    casus[$gen][$sg] = "s"
  when "en" 
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = "n"
    casus[$dat][$sg] = "n"
    casus[$gen][$sg] = "n"
  when "" 
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = ""
    casus[$dat][$sg] = ""
    casus[$gen][$sg] = ""
  end

  case nominativ_plural_endung
  when "er" 
    casus[$nom][$pl] = "er"
    casus[$akk][$pl] = "er"
    casus[$dat][$pl] = "ern"
    casus[$gen][$pl] = "er"
  when "e"
    casus[$nom][$pl] = "e"
    casus[$akk][$pl] = "e"
    casus[$dat][$pl] = "en"
    casus[$gen][$pl] = "e"
  when "en"
    casus[$nom][$pl] = "n"
    casus[$akk][$pl] = "n"
    casus[$dat][$pl] = "n"
    casus[$gen][$pl] = "n"
  when "" 
    casus[$nom][$pl] = ""
    casus[$akk][$pl] = ""
    casus[$dat][$pl] = "n"
    casus[$gen][$pl] = ""
  end


  nomen = [];
  [$sg].each { |n|
    #[$nom, $gen, $dat, $akk].each { |c|
    [$nom, $gen, $akk].each { |c|
      nomen << Nomen.new(numerus[n]+casus[c][n], bezeichner, c, geschlecht, 'n_'+n);
      puts nomen[-1].to_struct
    }
  }

  # pluralstamm="" -> no plural 
  if pluralstamm!="" then
    [$pl].each { |n|
      #[$nom, $gen, $dat, $akk].each { |c|
      [$nom, $gen, $akk].each { |c|
        nomen << Nomen.new(numerus[n]+casus[c][n], bezeichner+'s', c, geschlecht, 'n_'+n);
        puts nomen[-1].to_struct
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
  
  nomen
end


def unregelmaessiges_wort(bezeichner, wort, casus, geschlecht, numerus)
  return Nomen.new(wort, bezeichner, casus, geschlecht, numerus)
end

#puts
#substantiv("NOUN_SACK", "Sack", "es", "Säck", "e", "maskulin")
#puts
# output_definitions
# output_nouns
# output_verbs
# output_adjective

def ausgabe_nouns
  puts "\nstruct substantiv worte[] = {"

  [
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "der", $nom, $mal, $sg),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "des", $gen, $mal, $sg),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "dem", $dat, $mal, $sg),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "den", $akk, $mal, $sg),
    "",
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "die", [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("ARTIKEL_BESTIMMTER", "der", [$gen,$dat], $fem, $sg),
    "",
    unregelmaessiges_wort("NOUN_CORPSE",  "die Leiche", [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("NOUN_CORPSE",  "der Leiche", [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("NOUN_CORPSE",  "die Leichen von", [$nom,$akk], $fem, $pl),
    unregelmaessiges_wort("NOUN_CORPSE",  "den Leichen von", $dat, $fem, $pl),
    unregelmaessiges_wort("NOUN_CORPSE",  "der Leichen von", $gen, $fem, $pl),
    unregelmaessiges_wort("NOUN_CORPSE",  "der Leichen von", $gen, $fem, $pl),
    "",
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "ein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "eines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "einem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "einen", $akk,  $mal, $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "einen", $akk,  $neu, $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "eine",  [$nom,$akk],  $fem, $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "einer", [$gen,$dat],  $fem, $sg),
    "",
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "du",   $nom,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "Ihr",  $nom,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "dich", $akk,       [$mal,$fem,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_PERSONAL",  "Euch", $akk,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("NOUN_IT",            "es",  [$nom,$akk], $neu, $sg),
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
    "",
    unregelmaessiges_wort("PARTIKEL_NAMED",  "genannt",        [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]),
    unregelmaessiges_wort("PARTIKEL_CALLED", "bezeichnet mit", [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl])
  ].each { |n|
    if n.is_a? String then
      puts n
    else
      puts "  "+n.to_struct
    end
  }
  puts "/* ========= */"

  nomen = []

  #nomen << dekliniere_substantiv("NOUN_TOCHTER", "Tochter", "", "Töchter", "", "feminin")
  [
    dekliniere_substantiv("NOUN_POT_BLINDNESS", "Blindheit", "", "", "", "feminin"),

    dekliniere_substantiv("NOUN_WAND",      "Zauberstab",   "es", "Zauberstäb",   "e",  "maskulin"),
    dekliniere_substantiv("NOUN_AMULET",    "Amulett",      "es", "Amulett",      "e",  "neutrum"),
    dekliniere_substantiv("NOUN_SCROLL",    "Schriftrolle", "",   "Schriftrolle", "",   "feminin"),
    dekliniere_substantiv("NOUN_POTION",     "Trank",       "es", "Tränk",        "e",  "maskulin"),
    dekliniere_substantiv("NOUN_SPELLBOOK", "Zauberbuch",   "es", "Zauberbüch",   "er", "maskulin"),
    dekliniere_substantiv("NOUN_BLINDFOLD", "Augenbinde",   "",   "Augenbinde",   "en", "feminin"),

    dekliniere_substantiv("NOUN_TROLL", "Troll",  "es", "Troll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GIANT_BEETLE", "Riesenkäfer", "s", "Riesenkäfer", "", "maskulin"),
    dekliniere_substantiv("NOUN_DOG", "Hund", "es", "Hund", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SACK", "Sack", "es", "Säck", "e", "maskulin"),
    dekliniere_substantiv("NOUN_LICHEN", "Flechte", "", "Flechte", "en", "feminin"),
    dekliniere_substantiv("NOUN_SEWER_RAT", "Kanalratte", "", "Kanalratte", "en", "feminin"),
    dekliniere_substantiv("NOUN_JACKAL", "Schakal", "s", "Schakal", "e", "maskulin"),

    # FIX ME: der blaue drache vs ein blauer drache oder Blaudrache?*/
    dekliniere_substantiv("NOUN_BLACK_DRAGON", "Schwarzdrache", "en", "Schwarzdrache", "en", "maskulin"),

    dekliniere_substantiv("NOUN_OBJ_DOOR", "Türe", "", "Türe", "en", "feminin")
  ].each { |n|
    nomen << n
  }

  nomen.each {|n| 
    n.each { |w|
      puts "  "+w.to_struct
    }
    puts "\n"
  }
  puts "  {NULL, NULL, 0, 0, 0}\n};"
end

# print everything
#ausgabe_definitions
ausgabe_nouns
