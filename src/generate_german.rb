
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
enum Artikel { ohne=1, bestimmter=2, unbestimmter=4, grundform=8 };

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

class Adjektiv
  attr_accessor :wort, :bezeichner, :casus, :geschlecht, :numerus, :artikel
  def initialize(w,b,c,g,n,a)
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

    @artikel = a
  end

  def add_to_set(set, array_or_string)
    if array_or_string.kind_of?(Array) then
      array_or_string.each { |el| set << el }
    else
      set << array_or_string
    end
  end

  def merge?(other)
    return (other.wort == @wort  and other.bezeichner == @bezeichner and
              other.geschlecht == @geschlecht and
              other.numerus == @numerus and
              other.artikel == @artikel)
  end

  def merge(other)
    other.casus.each { |c|
      @casus << c
    }
  end

  def to_struct
    return '{"'+@wort+'", "'+@bezeichner+'", '+@casus.to_a.join("|")+
      ', '+@geschlecht.to_a.join("|")+', '+@numerus.to_a.join("|")+', '+@artikel+'},'
  end

  def to_s
    return @wort+" "+@bezeichner+" "+@casus.to_a.join("|")+
      " "+@geschlecht.to_a.join("|")+" "+@numerus.to_a.join("|")+" ",@artikel
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
    casus[$akk][$sg] = singularstamm[-1..-1]=='e' ? "n" : "en"
    casus[$dat][$sg] = singularstamm[-1..-1]=='e' ? "n" : "en"
    casus[$gen][$sg] = singularstamm[-1..-1]=='e' ? "n" : "en"
  when "" 
    casus[$nom][$sg] = ""
    casus[$akk][$sg] = ""
    casus[$dat][$sg] = ""
    casus[$gen][$sg] = ""
  end

  if pluralstamm!="" then
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
      casus[$nom][$pl] = pluralstamm[-1..-1]=='e' ? "n" : "en"
      casus[$akk][$pl] = pluralstamm[-1..-1]=='e' ? "n" : "en"
      casus[$dat][$pl] = pluralstamm[-1..-1]=='e' ? "n" : "en"
      casus[$gen][$pl] = pluralstamm[-1..-1]=='e' ? "n" : "en"
    when "" 
      casus[$nom][$pl] = ""
      casus[$akk][$pl] = ""
      casus[$dat][$pl] = pluralstamm[-1..-1]=='n' ? "" : "n"
      casus[$gen][$pl] = ""
    end
  end

  nomen = [];
  [$sg].each { |n|
    [$nom, $gen, $dat, $akk].each { |c|
      nomen << Nomen.new(numerus[n]+casus[c][n], bezeichner, c, geschlecht, n);
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
                           n);
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
  
  nomen
end

def unregelmaessiges_wort(bezeichner, wort, casus, geschlecht, numerus)
  return Nomen.new(wort, bezeichner, casus, geschlecht, numerus)
end

class Nomen < Adjektiv
  def initialize(w,b,c,g,n)
    super(w,b,c,g,n,"")
  end

  def to_struct
    return '{"'+@wort+'", "'+@bezeichner+'", '+@casus.to_a.join("|")+
      ', '+@geschlecht.to_a.join("|")+', '+@numerus.to_a.join("|")+'},'
  end

  def to_s
    return @wort+" "+@bezeichner+" "+@casus.to_a.join("|")+
      " "+@geschlecht.to_a.join("|")+" "+@numerus.to_a.join("|")
  end
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
  puts "\nstruct adjektiv adjektive[] = {"
  [
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
    "/* Spellbooks, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_SPE_PARCHMENT", "pergamentartig"), # Kandidat für 'aus Pergament'
    dekliniere_adjektiv("ADJEKTIV_SPE_VELLUM", "velin"), # Kandidat für 'aus Velum'
    dekliniere_adjektiv("ADJEKTIV_SPE_RAGGED", "ausgefranst"),
    #dekliniere_adjektiv("ADJEKTIV_SPE_DOG_EARED", ""),
    dekliniere_adjektiv("ADJEKTIV_SPE_MOTTLED", "fleckig"),
    dekliniere_adjektiv("ADJEKTIV_SPE_STAINED", "beschmutzt"),
    #dekliniere_adjektiv("ADJEKTIV_SPE_CLOTH", ""), # Kandidat für 'aus Stoff'
    dekliniere_adjektiv("ADJEKTIV_SPE_LEATHER", "ledern"), # Kandidat für 'aus Leder'
    dekliniere_adjektiv("ADJEKTIV_SPE_WHITE", "weiß"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PINK", "rosarot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_RED", "rot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_ORANGE", "orangen"), # Kandidat für unveränderlich
    dekliniere_adjektiv("ADJEKTIV_SPE_YELLOW", "gelb"),
    dekliniere_adjektiv("ADJEKTIV_SPE_VELVET", "samten"), # Kandidat für 'aus Samt'
    dekliniere_adjektiv("ADJEKTIV_SPE_LIGHT_GREEN", "hellgrün"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DARK_GREEN", "dunkelgrün"),
    dekliniere_adjektiv("ADJEKTIV_SPE_TURQUOISE", "türkisfarben"), # Kandidat für unveränderlich
    dekliniere_adjektiv("ADJEKTIV_SPE_CYAN", "tiefblau"),
    dekliniere_adjektiv("ADJEKTIV_SPE_LIGHT_BLUE", "hellblau"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DARK_BLUE", "dunkelblau"),
    dekliniere_adjektiv("ADJEKTIV_SPE_INDIGO", "indigofarben"),
    dekliniere_adjektiv("ADJEKTIV_SPE_MAGENTA", "tiefrot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PURPLE", "purpurn"),
    dekliniere_adjektiv("ADJEKTIV_SPE_VIOLET", "violett"),
    dekliniere_adjektiv("ADJEKTIV_SPE_TAN", "hellbraun"),
    #dekliniere_adjektiv("ADJEKTIV_SPE_PLAID", "hellbraun"), # kariert?
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
    #dekliniere_adjektiv("ADJEKTIV_SPE_CANVAS", ""),
    dekliniere_adjektiv("ADJEKTIV_SPE_HARDCOVER", "gebunden"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PLAIN", "unbedruckt"),
    #dekliniere_adjektiv("ADJEKTIV_SPE_PAPYRUS", ""), # Kandidat für 'aus Papyrus'
    "",
    "/* Wands, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_WAND_GLASS", "gläsern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_CRYSTAL", "kristallen"),
    dekliniere_adjektiv("ADJEKTIV_WAND_COPPER", "kupfern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_SILVER", "silbern"),
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
    dekliniere_adjektiv("ADJEKTIV_CURSED","verflucht"),
    dekliniere_adjektiv("ADJEKTIV_UNCURSED","nicht verflucht"),
    "  /* blessed mit geheiligt oder gesegnet uebersetzen? */",
    dekliniere_adjektiv("ADJEKTIV_BLESSED","geheiligt"),
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
  def initialize(bezeichner, stamm)
    initialize(bezeichner, stamm+'st', stamm+'t', stamm+'t', $akk)
  end

  def initialize(bezeichner, stamm="", formen=['st','t','t'], casus=$akk)
    @bezeichner = bezeichner;

    @konjugation = Hash.new
    @konjugation['zweite_singular'] = stamm+formen[0]
    @konjugation['dritte_singular'] = stamm+formen[1]
    @konjugation['zweite_plural']   = stamm+formen[2]
    @casus = casus
  end

  def to_struct
    a = Array.new
    a << '{"'+@konjugation['zweite_singular']+ '", "'+@bezeichner+ '", zweitePerson, n_singular, '+ @casus+'},'
    a << '{"'+@konjugation['dritte_singular']+'", "'+@bezeichner+'", drittePerson, n_singular, '+@casus+'},'
    a << '{"'+@konjugation['zweite_plural']  +'", "'+@bezeichner+'", zweitePerson, n_plural,   '+@casus+'},'
    return a
  end
end

def konjugiere_verb(bezeichner, stamm)
  return Verb.new(bezeichner, stamm)
end

def ausgabe_verbs
  puts "\nstruct verb verben[] = {"
  [
    konjugiere_verb("VERB_FEEL","spür"),
    konjugiere_verb("VERB_HEAR","hör"),
    konjugiere_verb("VERB_MERKEN","merk"),
    konjugiere_verb("VERB_SMELL","riech"),
    konjugiere_verb("VERB_MISS","verfehl"),
    konjugiere_verb("VERB_KILL","töte"),
    "",
    Verb.new("VERB_TOUCH","berühr"),
    "",
    Verb.new("VERB_SEIN", "", ["bist", "ist", "seid"]),
    Verb.new("VERB_CAN", "", ["kannst", "kann", "könnt"]),
    Verb.new("VERB_KICK", "", ["trittst", "tritt", "tretet"]),
    Verb.new("VERB_BITE", "", ["beißt", "beißt", "beißt"]),
    Verb.new("VERB_HIT", "", ["triffst", "trifft", "trefft"]),
    Verb.new("VERB_SEE", "", ["siehst", "sieht", "seht"]),
    "",
    Verb.new("VERB_WORN", "getragen", ["", "", ""]),
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
def ausgabe_nouns
  puts "\nstruct substantiv worte[] = {"

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
    unregelmaessiges_wort("NOUN_CORPSE",  "die sterblichen Überreste von", [$nom,$akk], $fem, $pl),
    unregelmaessiges_wort("NOUN_CORPSE",  "der sterblichen Überreste von",  $gen,       $fem, $pl),
    unregelmaessiges_wort("NOUN_CORPSE",  "den sterblichen Überresten von", $dat,       $fem, $pl),
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
  puts "  /* ===================================================================== */"

  nomen = []

  #nomen << dekliniere_substantiv("NOUN_TOCHTER", "Tochter", "", "Töchter", "", "feminin")
  [
    dekliniere_substantiv("NOUN_POT_BLINDNESS", "Blindheit", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_POT_HEALING", "Heilung", "", "Heilung", "en", "feminin"),

    dekliniere_substantiv("NOUN_SPE_SLEEP", "Schlaf", "es", "", "e", "maskulin"),

    dekliniere_substantiv("NOUN_WAND",      "Zauberstab",   "es", "Zauberstäb",   "e",  "maskulin"),
    dekliniere_substantiv("NOUN_AMULET",    "Amulett",      "es", "Amulett",      "e",  "neutrum"),
    dekliniere_substantiv("NOUN_SCROLL",    "Schriftrolle", "",   "Schriftrolle", "en", "feminin"),
    dekliniere_substantiv("NOUN_POTION",    "Trank",        "es", "Tränk",        "e",  "maskulin"),
    dekliniere_substantiv("NOUN_SPELLBOOK", "Zauberbuch",   "es", "Zauberbüch",   "er", "neutrum"),
    dekliniere_substantiv("NOUN_RING",      "Ring",         "es", "Ring",         "e",  "maskulin"),
    "/* besser als Edelstein, Kleinod */",
    dekliniere_substantiv("NOUN_GEM",       "Schmuckstein", "es", "Schmuckstein", "e",  "maskulin"),
    dekliniere_substantiv("NOUN_TOOLS",       "Schmuckstein", "es", "Schmuckstein", "e",  "maskulin"),
    "",
    dekliniere_substantiv("NOUN_BLINDFOLD", "Augenbinde",   "",   "Augenbinde",   "en", "feminin"),
    "",
    "/* Wands, identified */",
    dekliniere_substantiv("NOUN_WAND_LIGHT", "Licht", "es", "Licht", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_SECRET_DOOR_DETECTION", "Licht", "es", "Licht", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_CREATE_MONSTER", "Monstererschaffung", "", "Monstererschaffung", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_ENLIGHTENMENT", "Monsterbeschwörung", "", "Monsterbeschwörung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_CREATE_MONSTER", "Monsterbeschwörung", "", "Monsterbeschwörung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_WISHING", "Wünschen", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_NOTHING", "Nichts", "", "", "", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_STRIKING", "", "", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_MAKE_INVISIBLE", "Unsichtbarkeit", "", "Unsichtbarkeit", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SLOW_MONSTER", "Verlangsamung", "", "Verlangsamung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SPEED_MONSTER", "Hast", "", "Hast", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_UNDEAD_TURNING", "Hast", "", "Hast", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_CANCELLATION", "Vielgestaltigkeit", "", "Vielgestaltigkeit", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_TELEPORTATION", "Teleportation", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_WAND_OPENING", "Öffnung", "", "Öffnung", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_LOCKING", "Öffnung", "", "Öffnung", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_PROBING", "Öffnung", "", "Öffnung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_DIGGING", "Graben", "s", "", "", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_MAGIC_MISSILE", "Graben", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_FIRE", "Feuer", "s", "Feuer", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_COLD", "Kälte", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SLEEP", "Schlaf", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAND_DEATH", "Tod", "es", "Tod", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAND_LIGHTNING", "Blitz", "es", "Blitz", "e", "maskulin"),
    "",
    dekliniere_substantiv("NOUN_TROLL", "Troll",  "es", "Troll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GIANT_BEETLE", "Riesenkäfer", "s", "Riesenkäfer", "", "maskulin"),
    dekliniere_substantiv("NOUN_DOG", "Hund", "es", "Hund", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SACK", "Sack", "es", "Säck", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BAG", "Beutel", "s", "Beutel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LICHEN", "Flechte", "", "Flechte", "en", "feminin"),
    dekliniere_substantiv("NOUN_SEWER_RAT", "Kanalratte", "", "Kanalratte", "en", "feminin"),
    dekliniere_substantiv("NOUN_JACKAL", "Schakal", "s", "Schakal", "e", "maskulin"),
    "",
    "/* Rings, unidentified */",
    dekliniere_substantiv("RING_UNIDENTIFIED_WOODEN",     "Holz",      "es", "Hölz",      "er",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_GRANITE",    "Granit", "s",   "Granit", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_OPAL",       "Opal", "es",   "Opal", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_CLAY",       "Ton", "es",   "Ton", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_CORAL",      "Koralle", "",   "Koralle", "en",  "feminin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_BLACK_ONYX", "Onyx", "es",   "Onyx", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_MOONSTONE",  "Mondstein", "es",   "Mondstein", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_TIGER_EYE",  "Tigerauge", "es",   "Tigerauge", "en",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_JADE",       "Jade", "s",   "", "",  "feminin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_BRONZE",     "Bronze", "",   "Bronze", "en",  "feminin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_AGATE",      "Achat", "es",   "Achat", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_TOPAZ",      "Topas", "es",   "Topas", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_SAPPHIRE",   "Saphir", "s",   "Saphir", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_RUBY",       "Rubin", "s",   "Rubin", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_DIAMOND",    "Diamant", "en",   "Diamant", "en",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_PEARL",      "Perle", "",   "Perle", "en",  "feminin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_IRON",       "Eisen", "s",   "Eisen", "",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_BRASS",      "Messing", "s",   "Messing", "e",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_COPPER",     "Kupfer", "s",   "Kupfer", "",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_STEEL",      "Stahl", "es",   "Stahl", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_SILVER",     "Silber", "s",   "", "",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_GOLD",       "Gold", "es",   "", "e",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_IVORY",      "Elfenbein", "es",   "Elfenbein", "e",  "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_EMERALD",    "Smaragd", "es",   "Smaragd", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_WIRE",       "Draht", "es",   "Dräht", "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_ENGAGEMENT", "Verlobung", "",   "Verlobung", "en",  "feminin"),
    "/* Don't want to do a special treatment for twisted and shiny, so these are replaced with other gems */ ",
    dekliniere_substantiv("RING_UNIDENTIFIED_TWISTED",    "Lapislazuli", "",   "Lapislazuli", "",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_SHINY",      "Zirkon", "s",   "Zirkon", "e",  "maskulin"),
    "",
    "/* Wands, unidentified */",
    dekliniere_substantiv("MADE_OF_WAND_BALSA", "Balsaholz",  "es", "", "er", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_MAPLE", "Ahornholz",  "es", "", "er", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_PINE", "Kiefernholz", "es", "", "er", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_OAK", "Eichenholz", "es", "", "er", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_EBONY", "Ebenholz", "es", "", "er", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_MARBLE", "Marmor", "s", "", "e", "maskulin"),
    dekliniere_substantiv("MADE_OF_WAND_TIN", "Zinn", "es", "", "e", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_BRASS", "Messing", "s", "", "e", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_PLATINUM", "Platin", "s", "", "s", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_IRIDIUM", "Iridium", "s", "", "en", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_ZINC", "Zink", "es", "", "e", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_ALUMINIUM", "Aluminium", "s", "", "", "neutrum"),
    dekliniere_substantiv("MADE_OF_WAND_URANIUM", "Uran", "s", "", "e", "neutrum"),
    "",
    "/* Tools, unidentified */",
		dekliniere_substantiv("NOUN_BAG","Beutel","s","Beutel","","maskulin"),
    dekliniere_substantiv("NOUN_KEY","Schlüssel","s","Schlüssel","","maskulin"),
    dekliniere_substantiv("NOUN_CANDLE","Kerze","","Kerze","en","feminin"),
    dekliniere_substantiv("NOUN_LAMP","Lampe","","Lampe","en","maskulin"),
    "/* FIX ME: NOUN_LOOKING_GLASS should be different from NOUN_MIRROR */",
    dekliniere_substantiv("NOUN_LOOKING_GLASS","Spiegel","","Spiegel","","maskulin"), # FIX ME
    #dekliniere_substantiv("NOUN_GLASS_ORB","","","","","maskulin"),
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
    dekliniere_substantiv("NOUN_EGG","Ei","es","Ei","er","neutrum"),
    #dekliniere_substantiv("NOUN_MEATBALL"
    #dekliniere_substantiv("NOUN_MEAT_STICK","?Salami?"
    #dekliniere_substantiv("NOUN_HUGE_CHUNK_OF_MEAT"
    dekliniere_substantiv("NOUN_MEAT_RING","Fleischring","es","Fleischring","e","maskulin"),
    "/* Kelpwedel, Kelpblatt?",
    dekliniere_substantiv("NOUN_KELP_FROND","Seetangblatt","es","Seetangblätt","er","neutrum"),
    dekliniere_substantiv("NOUN_EUCALYPTUS_LEAF","Eukalyptusblatt","es","Eukalyptusblätt","er","neutrum"),
    dekliniere_substantiv("NOUN_APPLE","Apfel","s","Äpfel","","maskulin"),
    #dekliniere_substantiv("NOUN_ORANGE","Apfelsine","","Apfelsine","en","feminin"),
    dekliniere_substantiv("NOUN_ORANGE","Orange","","Orange","en","feminin"),
    dekliniere_substantiv("NOUN_PEAR","Birne","","Birne","en","feminin"),
    dekliniere_substantiv("NOUN_MELON","Melone","","Melone","en","feminin"),
    dekliniere_substantiv("NOUN_BANANA","Banane","","Banane","en","feminin"),
    dekliniere_substantiv("NOUN_CARROT","Karotte","","Karotte","en","feminin"),
    "/* FIX ME: don't really like \"Zweig\" */",
    dekliniere_substantiv("NOUN_SPRIG_OF_WOLFSBANE","Wolfseisenhutzweig","es","Wolfseisenhutzweig","e","maskulin"),
    dekliniere_substantiv("NOUN_CLOVE_OF_GARLIC","Knoblauchzehe","","Knoblauchzehe","en","feminin"),
    dekliniere_substantiv("NOUN_SLIME_MOLD","Schleimpilz","es","Schleimpilz","e","maskulin"),
    #dekliniere_substantiv("NOUN_LUMP_OF_ROYAL_JELLY"
    dekliniere_substantiv("NOUN_CREAM_PIE","Sahnetorte","","Sahnetorte","en","feminin"),
    dekliniere_substantiv("NOUN_CANDY_BAR","Schokoriegel","s","Schokoriegel","","maskulin"),
    dekliniere_substantiv("NOUN_FORTUNE_COOKIE","Glückskeks","es","Glückskeks","e","maskulin"),
    dekliniere_substantiv("NOUN_PANCAKE","Pfannkuchen","s","Pfannkuchen","","maskulin"),
    dekliniere_substantiv("NOUN_LEMBAS_WAFER","Lembasbrot","es","Lembasbrot","e","maskulin"),
    # cram, Lebensmittel aus HdR
    #dekliniere_substantiv("NOUN_CRAM_RATION"
    dekliniere_substantiv("NOUN_FOOD_RATION","Essensration","","Essensration","en","feminin"),
    dekliniere_substantiv("NOUN_K_RATION","Kampfration","","Kampfration","en","feminin"),
    dekliniere_substantiv("NOUN_C_RATION","Notration","","Notration","en","feminin"),
    dekliniere_substantiv("NOUN_TIN","Dose","","Dose","en","feminin"),
    dekliniere_substantiv("NOUN_SPINACH","Spinat","es","Spinat","e","maskulin"),
    "",
    "/* Coins */",
    dekliniere_substantiv("NOUN_GOLD_PIECE","Goldstück","es","Goldstück","e","neutrum"),
    "",
    "/* Spellbooks, unidentified */",
    dekliniere_substantiv("MADE_OF_SPE_VELLUM", "Velum", "s", "", "s", "neutrum"),
    "",
    # FIX ME: der blaue drache vs ein blauer drache oder Blaudrache?*/
    dekliniere_substantiv("NOUN_BLACK_DRAGON", "Schwarzdrache", "en", "Schwarzdrache", "en", "maskulin"),

    dekliniere_substantiv("NOUN_OBJ_DOOR", "Türe", "", "Türe", "en", "feminin")
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
  puts "  {NULL, NULL, 0, 0, 0}\n};"
end

# print everything
ausgabe_definitions
ausgabe_nouns
ausgabe_adjectives
ausgabe_verbs
