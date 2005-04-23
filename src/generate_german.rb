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
	const char *praeverb;
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

def dekliniere_substantiv(bezeichner, singularstamm,
                          genitiv_singular_endung, pluralstamm, nominativ_plural_endung, geschlecht)
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
    "  /* eigentlich unver�nderlich */",
    dekliniere_adjektiv("ADJEKTIV_POT_ORANGE","orangen"),
    dekliniere_adjektiv("ADJEKTIV_POT_YELLOW","gelb"),
    dekliniere_adjektiv("ADJEKTIV_POT_EMERALD","smaragdgr�n"),
    dekliniere_adjektiv("ADJEKTIV_POT_DARK_GREEN","dunkelgr�n"),
    dekliniere_adjektiv("ADJEKTIV_POT_CYAN","tiefblau"),
    dekliniere_adjektiv("ADJEKTIV_POT_SKY_BLUE","himmelblau"),
    dekliniere_adjektiv("ADJEKTIV_POT_BRILLIANT_BLUE","blaugl�nzend"),
    dekliniere_adjektiv("ADJEKTIV_POT_MAGENTA","tiefrot"),
    dekliniere_adjektiv("ADJEKTIV_POT_PURPLE_RED","purpurrot"),
    dekliniere_adjektiv("ADJEKTIV_POT_PUCE","dunkelbraun"),
    dekliniere_adjektiv("ADJEKTIV_POT_MILKY","milchig"),
    dekliniere_adjektiv("ADJEKTIV_POT_SWIRLY","verwirbelt"),
    dekliniere_adjektiv("ADJEKTIV_POT_BUBBLY","sprudelnd"),
    dekliniere_adjektiv("ADJEKTIV_POT_SMOKY","rauchig"),
    dekliniere_adjektiv("ADJEKTIV_POT_CLOUDY","unklar"),
    dekliniere_adjektiv("ADJEKTIV_POT_EFFERVESCENT","�bersprudelnd"),
    dekliniere_adjektiv("ADJEKTIV_POT_BLACK","schwarz"),
    dekliniere_adjektiv("ADJEKTIV_POT_GOLDEN","golden"),
    dekliniere_adjektiv("ADJEKTIV_POT_BROWN","braun"),
    dekliniere_adjektiv("ADJEKTIV_POT_FIZZY","zischend"),
    dekliniere_adjektiv("ADJEKTIV_POT_DARK","dunkl"),
    dekliniere_adjektiv("ADJEKTIV_POT_WHITE","wei�"),
    dekliniere_adjektiv("ADJEKTIV_POT_MURKY","tr�b"),
    dekliniere_adjektiv("ADJEKTIV_POT_CLEAR","durchsichtig"),
    "",
    "/* Spellbooks, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_SPE_PARCHMENT", "pergamentartig"), # Kandidat f�r 'aus Pergament'
    dekliniere_adjektiv("ADJEKTIV_SPE_VELLUM", "velin"), # Kandidat f�r 'aus Velum'
    dekliniere_adjektiv("ADJEKTIV_SPE_RAGGED", "ausgefranst"),
    #dekliniere_adjektiv("ADJEKTIV_SPE_DOG_EARED", ""),
    dekliniere_adjektiv("ADJEKTIV_SPE_MOTTLED", "fleckig"),
    dekliniere_adjektiv("ADJEKTIV_SPE_STAINED", "beschmutzt"),
    #dekliniere_adjektiv("ADJEKTIV_SPE_CLOTH", ""), # Kandidat f�r 'aus Stoff'
    dekliniere_adjektiv("ADJEKTIV_SPE_LEATHER", "ledern"), # Kandidat f�r 'aus Leder'
    dekliniere_adjektiv("ADJEKTIV_SPE_WHITE", "wei�"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PINK", "rosarot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_RED", "rot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_ORANGE", "orangen"), # Kandidat f�r unver�nderlich
    dekliniere_adjektiv("ADJEKTIV_SPE_YELLOW", "gelb"),
    dekliniere_adjektiv("ADJEKTIV_SPE_VELVET", "samten"), # Kandidat f�r 'aus Samt'
    dekliniere_adjektiv("ADJEKTIV_SPE_LIGHT_GREEN", "hellgr�n"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DARK_GREEN", "dunkelgr�n"),
    dekliniere_adjektiv("ADJEKTIV_SPE_TURQUOISE", "t�rkisfarben"), # Kandidat f�r unver�nderlich
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
    dekliniere_adjektiv("ADJEKTIV_SPE_GLITTERING", "glei�end"), # LEO gibt gl�nzend,glanzvoll, glei�end, glitzernd, verlockend
    dekliniere_adjektiv("ADJEKTIV_SPE_SHINING", "gl�nzend"), # oder 'blank'?
    dekliniere_adjektiv("ADJEKTIV_SPE_DULL", "matt"), # oder 'stumpf' oder 'tr�be'?
    dekliniere_adjektiv("ADJEKTIV_SPE_THIN", "d�nn"),
    dekliniere_adjektiv("ADJEKTIV_SPE_THICK", "dick"),
    #dekliniere_adjektiv("ADJEKTIV_SPE_CANVAS", ""),
    dekliniere_adjektiv("ADJEKTIV_SPE_HARDCOVER", "gebunden"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PLAIN", "unbedruckt"),
    #dekliniere_adjektiv("ADJEKTIV_SPE_PAPYRUS", ""), # Kandidat f�r 'aus Papyrus'
    "",
    "/* Wands, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_WAND_GLASS", "gl�sern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_CRYSTAL", "kristallen"),
    dekliniere_adjektiv("ADJEKTIV_WAND_COPPER", "kupfern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_SILVER", "silbern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_IRON", "eisern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_STEEL", "st�hlern"),
    dekliniere_adjektiv("ADJEKTIV_WAND_HEXAGONAL","sechseckig"),
    dekliniere_adjektiv("ADJEKTIV_WAND_SHORT","kurz"),
    dekliniere_adjektiv("ADJEKTIV_WAND_RUNED","runenbeschriftet"),
    dekliniere_adjektiv("ADJEKTIV_WAND_LONG","lang"),
    "/* 'gebogen' ist IMO besser als 'gekr�mmt'*/",
    dekliniere_adjektiv("ADJEKTIV_WAND_CURVED","gebogen"),
    dekliniere_adjektiv("ADJEKTIV_WAND_FORKED","gegabelt"),
    dekliniere_adjektiv("ADJEKTIV_WAND_SPIKED","spitzig"),
    dekliniere_adjektiv("ADJEKTIV_WAND_JEWELED","juwelenbesetzt"),
    "",
    "/* Amulets, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_AMULET_CIRCULAR","rund"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_SPHERICAL","kugelf�rmig"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_OVAL","oval"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_TRIANGULAR","dreieckig"),
    "  /* oder besser pyramidenartig oder pyramidal?*/",
    dekliniere_adjektiv("ADJEKTIV_AMULET_PYRAMIDAL","pyramidenf�rmig"),
    "  /* oder besser rechteckig oder quadratisch?*/",
    dekliniere_adjektiv("ADJEKTIV_AMULET_SQUARE","viereckig"),
    "  /* oder besser konkav?*/",
    dekliniere_adjektiv("ADJEKTIV_AMULET_CONCAVE","gew�lbt"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_HEXAGONAL","sechseckig"),
    dekliniere_adjektiv("ADJEKTIV_AMULET_OCTAGONAL","achteckig"),
    "",
    dekliniere_adjektiv("ADJEKTIV_CURSED","verflucht"),
    dekliniere_adjektiv("ADJEKTIV_UNCURSED","nicht verflucht"),
    "  /* blessed mit geheiligt oder gesegnet uebersetzen? */",
    dekliniere_adjektiv("ADJEKTIV_BLESSED","geheiligt"),
    "",
    "/* sonstige Adjektive */",
    dekliniere_adjektiv("ADJEKTIV_EATEN","verspeist"),
    dekliniere_adjektiv("ADJEKTIV_SADDLED","gesattelt"),
    ""
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
  puts "\nstruct verb verben[] = {"
  [
    konjugiere_verb("VERB_SICH_FUEHLEN","f�hl"),
    konjugiere_verb("VERB_MERKEN","merk"),
    konjugiere_verb("VERB_LANDEN","land"),
    "",
    konjugiere_verb("VERB_FEEL","sp�r"),
    konjugiere_verb("VERB_HEAR","h�r"),
    konjugiere_verb("VERB_SMELL","riech"),
    konjugiere_verb("VERB_MISS","verfehl"),
    konjugiere_verb("VERB_KILL","t�te"),
    konjugiere_verb("VERB_TOUCH","ber�hr"),
    "",
    "/* unregelm�ssige Verben */",
    Verb.new("VERB_SEIN", "", "", ["bist", "ist", "seid", "sind"]),
    Verb.new("VERB_HAVE", "", "", ["hast", "hat", "habt", "haben"]),
    Verb.new("VERB_READ", "", "", ["liest", "liest", "lest","lesen"]),
    Verb.new("VERB_CAN", "", "", ["kannst", "kann", "k�nnt","k�nnen"]),
    Verb.new("VERB_KICK", "", "", ["trittst", "tritt", "tretet","treten"]),
    Verb.new("VERB_BITE", "", "", ["bei�t", "bei�t", "bei�t","bei�en"]),
    Verb.new("VERB_HIT", "", "", ["triffst", "trifft", "trefft", "treffen"]),
    Verb.new("VERB_SEE", "", "", ["siehst", "sieht", "seht", "sehen"]),
    Verb.new("VERB_CONTAIN", "", "", ["enth�lst", "enth�lt", "enthaltet","enthalten"]),
    Verb.new("VERB_OPEN", "", "auf", ["gehst", "geht", "geht","gehen"]),
    Verb.new("VERB_EAT", "", "auf", ["isst", "isst", "esst","essen"]),
    Verb.new("VERB_DROP", "", "fallen", ["l�sst", "l�sst", "lasst","lassen"]),    # "You drop a potion."
    Verb.new("VERB_OBJECT_DROPS", "", "", ["f�llst", "f�llt", "fallt", "fallen"]), # e.g "A potion drops to the floor."
    Verb.new("VERB_SEEM","","zu sein", ["scheinst", "scheint","scheint", "scheinen"]),
    "",
    Verb.new("VERB_DACHTEST", "", "", ["dachtest", "dachte", "dachtet","dachten"]),
    Verb.new("VERB_HAETTEST", "", "", ["h�ttest", "h�tte", "h�ttet","h�tten"]),
    "",
    Verb.new("VERB_WORN", "getragen", "", ["", "", "", ""]),
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
    unregelmaessiges_wort("NOUN_CORPSE",  "die sterblichen �berreste", [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("NOUN_CORPSE",  "der sterblichen �berreste",  $gen,       $fem, $sg),
    unregelmaessiges_wort("NOUN_CORPSE",  "den sterblichen �berresten", $dat,       $fem, $sg),
    unregelmaessiges_wort("NOUN_CORPSEs",  "die sterblichen �berreste von", [$nom,$akk], $fem, $pl),
    unregelmaessiges_wort("NOUN_CORPSEs",  "der sterblichen �berreste von",  $gen,       $fem, $pl),
    unregelmaessiges_wort("NOUN_CORPSEs",  "den sterblichen �berresten von", $dat,       $fem, $pl),
    "",
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "ein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "eines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "einem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "einen", $akk,  $mal, $sg),
    unregelmaessiges_wort("ARTIKEL_UNBESTIMMTER", "ein",   $akk,  $neu, $sg),
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
    unregelmaessiges_wort("NOUN_OOPS",  "Hoppla",        [$nom,$gen,$dat,$akk], [$mal,$fem,$neu], [$sg,$pl]),
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

  #nomen << dekliniere_substantiv("NOUN_TOCHTER", "Tochter", "", "T�chter", "", "feminin")
  [
    "/* Nouns that are neither in mons-de.h nor in obj-de.h */",
    dekliniere_substantiv("NOUN_SPINACH","Spinat","es","Spinat","e","maskulin"),
    dekliniere_substantiv("NOUN_OBJ_DOOR", "T�re", "", "T�re", "en", "feminin"),
    dekliniere_substantiv("NOUN_WALLET", "Geldb�rse", "", "Geldb�rse", "en", "feminin"),
    dekliniere_substantiv("NOUN_MEAT", "Fleisch", "es", "", "e", "neutrum"),
    dekliniere_substantiv("NOUN_LUCK", "Gl�ck", "es", "Gl�ck", "e", "neutrum"),
    "",
    dekliniere_substantiv("NOUN_BAG", "Beutel", "s", "Beutel", "", "maskulin"),
    "",
    dekliniere_substantiv("NOUN_POT_BLINDNESS", "Blindheit", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_POT_HEALING", "Heilung", "", "Heilung", "en", "feminin"),

    dekliniere_substantiv("NOUN_SPE_SLEEP", "Schlaf", "es", "", "e", "maskulin"),

    dekliniere_substantiv("NOUN_WEAPON",    "Waffe",          "",   "Waffe",          "en", "feminin"),
    #dekliniere_substantiv("NOUN_ARMOR",     "R�stung",        "",   "R�stung",      "en", "feminin"),
    dekliniere_substantiv("NOUN_ARMOR",     "Schutzbekleidung", "",   "Schutzbekleidung", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING",      "Ring",           "es", "Ring",           "e",  "maskulin"),
    dekliniere_substantiv("NOUN_AMULET",    "Amulett",        "es", "Amulett",        "e",  "neutrum"),
    dekliniere_substantiv("NOUN_TOOL",      "Werkzeug",       "es", "Werkzeug",       "e",  "neutrum"),
    "/* This is wrong. 'Essbares' should be inflected like an adjective */",
    dekliniere_substantiv("NOUN_COMESTIBLE","Essbare",        "",   "Essbare",        "s",  "neutrum"),
    dekliniere_substantiv("NOUN_POTION",    "Trank",          "es", "Tr�nk",          "e",  "maskulin"),
    dekliniere_substantiv("NOUN_SCROLL",    "Schriftrolle",   "",   "Schriftrolle",   "en", "feminin"),
    dekliniere_substantiv("NOUN_SPELLBOOK", "Zauberbuch",     "es", "Zauberb�ch",     "er", "neutrum"),
    dekliniere_substantiv("NOUN_WAND",      "Zauberstab",     "es", "Zauberst�b",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_COIN",      "M�nze",          "",   "M�nze",          "en", "feminin"),
    "/* besser als Edelstein, Kleinod? */",
    dekliniere_substantiv("NOUN_GEM",       "Schmuckstein",   "es", "Schmuckstein",   "e",  "maskulin"),
    dekliniere_substantiv("NOUN_BOULDER",   "Felsbrocken",    "s",  "Felsbrocken",    "",   "maskulin"),
    dekliniere_substantiv("NOUN_STATUE",    "Statue",         "",   "Statue",         "en", "feminin"),
    "",
    "/* Weapons, identified */",
    #dekliniere_substantiv("NOUN_ARROW"
    #dekliniere_substantiv("NOUN_ELVEN_ARROW"
    #dekliniere_substantiv("NOUN_ORCISH_ARROW"
    #dekliniere_substantiv("NOUN_SILVER_ARROW"
    #dekliniere_substantiv("NOUN_YA"
    #dekliniere_substantiv("NOUN_CROSSBOW_BOLT"
    #dekliniere_substantiv("NOUN_DART"
    #dekliniere_substantiv("NOUN_SHURIKEN"
    #dekliniere_substantiv("NOUN_BOOMERANG"
    #dekliniere_substantiv("NOUN_SPEAR"
    #dekliniere_substantiv("NOUN_ELVEN_SPEAR"
    #dekliniere_substantiv("NOUN_ORCISH_SPEAR"
    #dekliniere_substantiv("NOUN_DWARVISH_SPEAR"
    #dekliniere_substantiv("NOUN_SILVER_SPEAR"
    #dekliniere_substantiv("NOUN_JAVELIN"
    #dekliniere_substantiv("NOUN_TRIDENT"
    #dekliniere_substantiv("NOUN_DAGGER"
    #dekliniere_substantiv("NOUN_ELVEN_DAGGER"
    #dekliniere_substantiv("NOUN_ORCISH_DAGGER"
    #dekliniere_substantiv("NOUN_SILVER_DAGGER"
    #dekliniere_substantiv("NOUN_ATHAME"
    #dekliniere_substantiv("NOUN_SCALPEL"
    #dekliniere_substantiv("NOUN_KNIFE"
    #dekliniere_substantiv("NOUN_STILETTO"
    #dekliniere_substantiv("NOUN_WORM_TOOTH"
    #dekliniere_substantiv("NOUN_CRYSKNIFE"
    #dekliniere_substantiv("NOUN_AXE"
    #dekliniere_substantiv("NOUN_BATTLE_AXE"
    #dekliniere_substantiv("NOUN_SHORT_SWORD"
    #dekliniere_substantiv("NOUN_ELVEN_SHORT_SWORD"
    #dekliniere_substantiv("NOUN_ORCISH_SHORT_SWORD"
    #dekliniere_substantiv("NOUN_DWARVISH_SHORT_SWORD"
    #dekliniere_substantiv("NOUN_SCIMITAR"
    #dekliniere_substantiv("NOUN_SILVER_SABER"
    #dekliniere_substantiv("NOUN_BROADSWORD"
    #dekliniere_substantiv("NOUN_ELVEN_BROADSWORD"
    #dekliniere_substantiv("NOUN_LONG_SWORD"
    #dekliniere_substantiv("NOUN_TWO_HANDED_SWORD"
    #dekliniere_substantiv("NOUN_KATANA"
    #dekliniere_substantiv("NOUN_TSURUGI"
    #dekliniere_substantiv("NOUN_RUNESWORD"
    #dekliniere_substantiv("NOUN_PARTISAN"
    #dekliniere_substantiv("NOUN_RANSEUR"
    #dekliniere_substantiv("NOUN_SPETUM"
    #dekliniere_substantiv("NOUN_GLAIVE"
    #dekliniere_substantiv("NOUN_LANCE"
    #dekliniere_substantiv("NOUN_HALBERD"
    #dekliniere_substantiv("NOUN_BARDICHE"
    #dekliniere_substantiv("NOUN_VOULGE"
    #dekliniere_substantiv("NOUN_DWARVISH_MATTOCK"
    #dekliniere_substantiv("NOUN_FAUCHARD"
    #dekliniere_substantiv("NOUN_GUISARME"
    #dekliniere_substantiv("NOUN_BILL_GUISARME"
    #dekliniere_substantiv("NOUN_LUCERN_HAMMER"
    #dekliniere_substantiv("NOUN_BEC_DE_CORBIN"
    #dekliniere_substantiv("NOUN_MACE"
    #dekliniere_substantiv("NOUN_MORNING_STAR"
    #dekliniere_substantiv("NOUN_WAR_HAMMER"
    #dekliniere_substantiv("NOUN_CLUB"
    #dekliniere_substantiv("NOUN_RUBBER_HOSE"
    #dekliniere_substantiv("NOUN_QUARTERSTAFF"
    #dekliniere_substantiv("NOUN_AKLYS"
    #dekliniere_substantiv("NOUN_FLAIL"
    #dekliniere_substantiv("NOUN_BULLWHIP"
    #dekliniere_substantiv("NOUN_BOW"
    #dekliniere_substantiv("NOUN_ELVEN_BOW"
    #dekliniere_substantiv("NOUN_ORCISH_BOW"
    #dekliniere_substantiv("NOUN_YUMI"
    #dekliniere_substantiv("NOUN_SLING"
    #dekliniere_substantiv("NOUN_CROSSBOW"
    "",
    "/* Weapons, unidentified */",
    #dekliniere_substantiv("NOUN_RUNED_ARROW"
    #dekliniere_substantiv("NOUN_CRUDE_ARROW"
    #dekliniere_substantiv("NOUN_BAMBOO_ARROW"
    #dekliniere_substantiv("NOUN_THROWING_STAR"
    #dekliniere_substantiv("NOUN_RUNED_SPEAR"
    #dekliniere_substantiv("NOUN_CRUDE_SPEAR"
    #dekliniere_substantiv("NOUN_STOUT_SPEAR"
    #dekliniere_substantiv("NOUN_THROWING_SPEAR"
    #dekliniere_substantiv("NOUN_RUNED_DAGGER"
    #dekliniere_substantiv("NOUN_CRUDE_DAGGER"
    #dekliniere_substantiv("NOUN_DOUBLE_HEADED_AXE"
    #dekliniere_substantiv("NOUN_RUNED_SHORT_SWORD"
    #dekliniere_substantiv("NOUN_CRUDE_SHORT_SWORD"
    #dekliniere_substantiv("NOUN_BROAD_SHORT_SWORD"
    #dekliniere_substantiv("NOUN_CURVED_SWORD"
    #dekliniere_substantiv("NOUN_RUNED_BROADSWORD"
    #dekliniere_substantiv("NOUN_SAMURAI_SWORD"
    #dekliniere_substantiv("NOUN_LONG_SAMURAI_SWORD", "ADJEKTIV_LANG NOUN_SAMURAI_SWORD",
    #dekliniere_substantiv("NOUN_VULGAR_POLEARM"
    #dekliniere_substantiv("NOUN_HILTED_POLEARM"
    #dekliniere_substantiv("NOUN_FORKED_POLEARM"
    #dekliniere_substantiv("NOUN_SINGLE_EDGED_POLEARM"
    #dekliniere_substantiv("NOUN_ANGLED_POLEAXE"
    #dekliniere_substantiv("NOUN_LONG_POLEAXE"
    #dekliniere_substantiv("NOUN_POLE_CLEAVER"
    #dekliniere_substantiv("NOUN_BROAD_PICK"
    #dekliniere_substantiv("NOUN_POLE_SICKLE"
    #dekliniere_substantiv("NOUN_PRUNING_HOOK"
    #dekliniere_substantiv("NOUN_HOOKED_POLEARM"
    #dekliniere_substantiv("NOUN_PRONGED_POLEARM"
    #dekliniere_substantiv("NOUN_BEAKED_POLEARM"
    #dekliniere_substantiv("NOUN_STAFF"
    #dekliniere_substantiv("NOUN_THONGED_CLUB"
    #dekliniere_substantiv("NOUN_RUNED_BOW"
    #dekliniere_substantiv("NOUN_CRUDE_BOW"
    #dekliniere_substantiv("NOUN_LONG_BOW"
    "",
    "/* Armor, identified */",
    #dekliniere_substantiv("NOUN_ELVEN_LEATHER_HELM"
    #dekliniere_substantiv("NOUN_ORCISH_HELM"
    #dekliniere_substantiv("NOUN_DWARVISH_IRON_HELM"
    #dekliniere_substantiv("NOUN_FEDORA"
    #dekliniere_substantiv("NOUN_CORNUTHAUM"
    #dekliniere_substantiv("NOUN_DUNCE_CAP"
    #dekliniere_substantiv("NOUN_DENTED_POT"
    #dekliniere_substantiv("NOUN_HELMET"
    #dekliniere_substantiv("NOUN_HELM_OF_BRILLIANCE"
    #dekliniere_substantiv("NOUN_HELM_OF_OPPOSITE_ALIGNMENT"
    #dekliniere_substantiv("NOUN_HELM_OF_TELEPATHY"
    #dekliniere_substantiv("NOUN_GRAY_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_SILVER_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_SHIMMERING_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_RED_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_WHITE_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_ORANGE_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_BLACK_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_BLUE_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_GREEN_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_YELLOW_DRAGON_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_GRAY_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_SILVER_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_SHIMMERING_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_RED_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_WHITE_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_ORANGE_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_BLACK_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_BLUE_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_GREEN_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_YELLOW_DRAGON_SCALES"
    #dekliniere_substantiv("NOUN_PLATE_MAIL"
    #dekliniere_substantiv("NOUN_CRYSTAL_PLATE_MAIL"
    #dekliniere_substantiv("NOUN_BRONZE_PLATE_MAIL"
    #dekliniere_substantiv("NOUN_SPLINT_MAIL"
    #dekliniere_substantiv("NOUN_BANDED_MAIL"
    #dekliniere_substantiv("NOUN_DWARVISH_MITHRIL_COAT"
    #dekliniere_substantiv("NOUN_ELVEN_MITHRIL_COAT"
    #dekliniere_substantiv("NOUN_CHAIN_MAIL"
    #dekliniere_substantiv("NOUN_ORCISH_CHAIN_MAIL"
    #dekliniere_substantiv("NOUN_SCALE_MAIL"
    #dekliniere_substantiv("NOUN_STUDDED_LEATHER_ARMOR"
    #dekliniere_substantiv("NOUN_RING_MAIL"
    #dekliniere_substantiv("NOUN_ORCISH_RING_MAIL"
    #dekliniere_substantiv("NOUN_LEATHER_ARMOR"
    #dekliniere_substantiv("NOUN_LEATHER_JACKET"
    #dekliniere_substantiv("NOUN_HAWAIIAN_SHIRT"
    #dekliniere_substantiv("NOUN_T_SHIRT"
    #dekliniere_substantiv("NOUN_MUMMY_WRAPPING"
    #dekliniere_substantiv("NOUN_ELVEN_CLOAK"
    #dekliniere_substantiv("NOUN_ORCISH_CLOAK"
    #dekliniere_substantiv("NOUN_DWARVISH_CLOAK"
    #dekliniere_substantiv("NOUN_OILSKIN_CLOAK"
    #dekliniere_substantiv("NOUN_ROBE"
    #dekliniere_substantiv("NOUN_ALCHEMY_SMOCK"
    #dekliniere_substantiv("NOUN_LEATHER_CLOAK"
    #dekliniere_substantiv("NOUN_CLOAK_OF_PROTECTION"
    #dekliniere_substantiv("NOUN_CLOAK_OF_INVISIBILITY"
    #dekliniere_substantiv("NOUN_CLOAK_OF_MAGIC_RESISTANCE"
    #dekliniere_substantiv("NOUN_CLOAK_OF_DISPLACEMENT"
    #dekliniere_substantiv("NOUN_SMALL_SHIELD"
    #dekliniere_substantiv("NOUN_ELVEN_SHIELD"
    #dekliniere_substantiv("NOUN_URUK_HAI_SHIELD"
    #dekliniere_substantiv("NOUN_ORCISH_SHIELD"
    #dekliniere_substantiv("NOUN_LARGE_SHIELD"
    #dekliniere_substantiv("NOUN_DWARVISH_ROUNDSHIELD"
    #dekliniere_substantiv("NOUN_SHIELD_OF_REFLECTION"
    #dekliniere_substantiv("NOUN_LEATHER_GLOVES"
    #dekliniere_substantiv("NOUN_GAUNTLETS_OF_FUMBLING"
    #dekliniere_substantiv("NOUN_GAUNTLETS_OF_POWER"
    #dekliniere_substantiv("NOUN_GAUNTLETS_OF_DEXTERITY"
    #dekliniere_substantiv("NOUN_LOW_BOOTS"
    #dekliniere_substantiv("NOUN_IRON_SHOES"
    #dekliniere_substantiv("NOUN_HIGH_BOOTS"
    #dekliniere_substantiv("NOUN_SPEED_BOOTS"
    #dekliniere_substantiv("NOUN_WATER_WALKING_BOOTS"
    #dekliniere_substantiv("NOUN_JUMPING_BOOTS"
    #dekliniere_substantiv("NOUN_ELVEN_BOOTS"
    #dekliniere_substantiv("NOUN_KICKING_BOOTS"
    #dekliniere_substantiv("NOUN_FUMBLE_BOOTS"
    #dekliniere_substantiv("NOUN_LEVITATION_BOOTS"
    "",
    "/* Armor, unidentified */",
    #dekliniere_substantiv("NOUN_LEATHER_HAT"
    #dekliniere_substantiv("NOUN_IRON_SKULL_CAP"
    #dekliniere_substantiv("NOUN_HARD_HAT"
    #dekliniere_substantiv("NOUN_CONICAL_HAT"
    #dekliniere_substantiv("NOUN_PLUMED_HELMET"
    #dekliniere_substantiv("NOUN_ETCHED_HELMET"
    #dekliniere_substantiv("NOUN_CRESTED_HELMET"
    #dekliniere_substantiv("NOUN_VISORED_HELMET"
    #dekliniere_substantiv("NOUN_CRUDE_CHAIN_MAIL"
    #dekliniere_substantiv("NOUN_CRUDE_RING_MAIL"
    #dekliniere_substantiv("NOUN_FADED_PALL"
    #dekliniere_substantiv("NOUN_COARSE_MANTELET"
    #dekliniere_substantiv("NOUN_HOODED_CLOAK"
    #dekliniere_substantiv("NOUN_SLIPPERY_CLOAK"
    #dekliniere_substantiv("NOUN_APRON"
    #dekliniere_substantiv("NOUN_TATTERED_CAPE"
    #dekliniere_substantiv("NOUN_OPERA_CLOAK"
    #dekliniere_substantiv("NOUN_ORNAMENTAL_COPE"
    #dekliniere_substantiv("NOUN_PIECE_OF_CLOTH"
    #dekliniere_substantiv("NOUN_BLUE_AND_GREEN_SHIELD"
    #dekliniere_substantiv("NOUN_WHITE_HANDED_SHIELD"
    #dekliniere_substantiv("NOUN_RED_EYED_SHIELD"
    #dekliniere_substantiv("NOUN_LARGE_ROUND_SHIELD"
    #dekliniere_substantiv("NOUN_POLISHED_SILVER_SHIELD"
    #dekliniere_substantiv("NOUN_OLD_GLOVES"
    #dekliniere_substantiv("NOUN_PADDED_GLOVES"
    #dekliniere_substantiv("NOUN_RIDING_GLOVES"
    #dekliniere_substantiv("NOUN_FENCING_GLOVES"
    #dekliniere_substantiv("NOUN_WALKING_SHOES"
    #dekliniere_substantiv("NOUN_HARD_SHOES"
    #dekliniere_substantiv("NOUN_JACKBOOTS"
    #dekliniere_substantiv("NOUN_COMBAT_BOOTS"
    #dekliniere_substantiv("NOUN_JUNGLE_BOOTS"
    #dekliniere_substantiv("NOUN_HIKING_BOOTS"
    #dekliniere_substantiv("NOUN_MUD_BOOTS"
    #dekliniere_substantiv("NOUN_BUCKLED_BOOTS"
    #dekliniere_substantiv("NOUN_RIDING_BOOTS"
    #dekliniere_substantiv("NOUN_SNOW_BOOTS"
    "",
    "/* Wands, identified */",
    dekliniere_substantiv("NOUN_WAND_LIGHT", "Licht", "es", "Licht", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_SECRET_DOOR_DETECTION", "Licht", "es", "Licht", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_CREATE_MONSTER", "Monstererschaffung", "", "Monstererschaffung", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_ENLIGHTENMENT", "Monsterbeschw�rung", "", "Monsterbeschw�rung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_CREATE_MONSTER", "Monsterbeschw�rung", "", "Monsterbeschw�rung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_WISHING", "W�nschen", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_NOTHING", "Nichts", "", "", "", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_STRIKING", "", "", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_MAKE_INVISIBLE", "Unsichtbarkeit", "", "Unsichtbarkeit", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SLOW_MONSTER", "Verlangsamung", "", "Verlangsamung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SPEED_MONSTER", "Hast", "", "Hast", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_UNDEAD_TURNING", "", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_WAND_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_CANCELLATION", "Vielgestaltigkeit", "", "Vielgestaltigkeit", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_TELEPORTATION", "Teleportation", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_WAND_OPENING", "�ffnung", "", "�ffnung", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_LOCKING", "�ffnung", "", "�ffnung", "en", "feminin"),
    #dekliniere_substantiv("NOUN_WAND_PROBING", "�ffnung", "", "�ffnung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_DIGGING", "Graben", "s", "", "", "neutrum"),
    #dekliniere_substantiv("NOUN_WAND_MAGIC_MISSILE", "Graben", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_FIRE", "Feuer", "s", "Feuer", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_COLD", "K�lte", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SLEEP", "Schlaf", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAND_DEATH", "Tod", "es", "Tod", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAND_LIGHTNING", "Blitz", "es", "Blitz", "e", "maskulin"),
    "",
    "/* Rings, unidentified */",
    dekliniere_substantiv("RING_UNIDENTIFIED_WOODEN",     "Holz",      "es", "H�lz",      "er",  "neutrum"),
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
    dekliniere_substantiv("RING_UNIDENTIFIED_WIRE",       "Draht", "es",   "Dr�ht", "e",  "maskulin"),
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
    "/* Tools, identified */",
    #dekliniere_substantiv("NOUN_LARGE_BOX", 
    dekliniere_substantiv("NOUN_CHEST", "Truhe", "", "Truhe", "en", "feminin"),
    dekliniere_substantiv("NOUN_ICE_BOX", "K�hltruhe", "", "K�hltruhe", "en", "feminin"),
    dekliniere_substantiv("NOUN_SACK", "Sack", "es", "S�ck", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_OILSKIN_SACK", 
    #dekliniere_substantiv("NOUN_BAG_OF_HOLDING", 
    #dekliniere_substantiv("NOUN_BAG_OF_TRICKS", 
    #dekliniere_substantiv("NOUN_SKELETON_KEY", "Generalschl�ssel", "s", "Generalschl�ssel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SKELETON_KEY", "Passepartout", "s", "Generalschl�ssel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LOCK_PICK", "Dietrich", "s", "Dietrich", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CREDIT_CARD", "Kreditkarte", "", "Kreditkarte", "en", "feminin"),
    dekliniere_substantiv("NOUN_TALLOW_CANDLE", "Talglicht", "es", "Talglicht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_WAX_CANDLE", "Wachskerze", "", "Wachskerze", "en", "feminin"),
    dekliniere_substantiv("NOUN_BRASS_LANTERN", "Laterne", "", "Laterne", "en", "feminin"),
    #dekliniere_substantiv("NOUN_OIL_LAMP", 
    #dekliniere_substantiv("NOUN_MAGIC_LAMP", 
    #dekliniere_substantiv("NOUN_EXPENSIVE_CAMERA", 
    "/* FIX ME: NOUN_LOOKING_GLASS should be different from NOUN_MIRROR */",
    dekliniere_substantiv("NOUN_MIRROR","Spiegel","","Spiegel","","maskulin"), # FIX ME
    dekliniere_substantiv("NOUN_CRYSTAL_BALL", "Kristallkugel", "", "Kristallkugel", "en", "feminin"),
    #dekliniere_substantiv("NOUN_LENSES", 
    dekliniere_substantiv("NOUN_BLINDFOLD", "Augenbinde",   "",   "Augenbinde",   "en", "feminin"),
    dekliniere_substantiv("NOUN_TOWEL", "Handtuch", "es", "Handt�ch", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SADDLE", "Sattel", "s", "S�ttel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEASH", "Leine", "", "Leine", "en", "feminin"),
    dekliniere_substantiv("NOUN_STETHOSCOPE", "Stethoskop", "s", "Stethoskop", "e", "neutrum"),
    #dekliniere_substantiv("NOUN_TINNING_KIT", 
    dekliniere_substantiv("NOUN_TIN_OPENER", "Dosen�ffner", "s", "Dosen�ffner", "", "maskulin"),
    #dekliniere_substantiv("NOUN_CAN_OF_GREASE", 
    dekliniere_substantiv("NOUN_FIGURINE", "Statuette", "", "Statuette", "en", "feminin"),
    #dekliniere_substantiv("NOUN_MAGIC_MARKER", 
    "/* 'Landmine' is a more generic term. I think 'Tretmine' is the more exact terminus for the mines in Nethack. */",
    dekliniere_substantiv("NOUN_LAND_MINE", "Tretmine", "", "Tretmine", "en", "feminin"),
    #dekliniere_substantiv("NOUN_BEARTRAP", 
    #dekliniere_substantiv("NOUN_TIN_WHISTLE", 
    #dekliniere_substantiv("NOUN_MAGIC_WHISTLE", 
    #dekliniere_substantiv("NOUN_WOODEN_FLUTE", 
    #dekliniere_substantiv("NOUN_MAGIC_FLUTE", 
    #dekliniere_substantiv("NOUN_TOOLED_HORN", 
    dekliniere_substantiv("NOUN_FROST_HORN", "Eishorn", "es", "Eish�rn", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_FIRE_HORN", 
    dekliniere_substantiv("NOUN_HORN_OF_PLENTY", "F�llhorn", "es", "F�llhorn", "er", "neutrum"),
    #dekliniere_substantiv("NOUN_WOODEN_HARP", 
    #dekliniere_substantiv("NOUN_MAGIC_HARP", 
    dekliniere_substantiv("NOUN_BELL", "Gl�ckchen", "s", "Gl�ckchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_BUGLE", "Waldhorn", "es", "Waldh�rn", "er", "neutrum"),
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
    dekliniere_substantiv("NOUN_KEY","Schl�ssel","s","Schl�ssel","","maskulin"),
    dekliniere_substantiv("NOUN_CANDLE","Kerze","","Kerze","en","feminin"),
    dekliniere_substantiv("NOUN_LAMP","Lampe","","Lampe","en","feminin"),
    "/* FIX ME: NOUN_LOOKING_GLASS should be different from NOUN_MIRROR */",
    dekliniere_substantiv("NOUN_LOOKING_GLASS","Spiegel","","Spiegel","","maskulin"), # FIX ME
    #dekliniere_substantiv("NOUN_GLASS_ORB","","","","","maskulin"),
    dekliniere_substantiv("NOUN_WHISTLE","Pfeife","","Pfeife","en","feminin"),
    dekliniere_substantiv("NOUN_FLUTE","Fl�te","","Fl�te","en","feminin"),
    dekliniere_substantiv("NOUN_HORN","Horn","es","H�rn","er","neutrum"),
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
    "/* Kelpwedel, Kelpblatt? */",
    dekliniere_substantiv("NOUN_KELP_FROND","Seetangblatt","es","Seetangbl�tt","er","neutrum"),
    dekliniere_substantiv("NOUN_EUCALYPTUS_LEAF","Eukalyptusblatt","es","Eukalyptusbl�tt","er","neutrum"),
    dekliniere_substantiv("NOUN_APPLE","Apfel","s","�pfel","","maskulin"),
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
    dekliniere_substantiv("NOUN_FORTUNE_COOKIE","Gl�ckskeks","es","Gl�ckskeks","e","maskulin"),
    dekliniere_substantiv("NOUN_PANCAKE","Pfannkuchen","s","Pfannkuchen","","maskulin"),
    dekliniere_substantiv("NOUN_LEMBAS_WAFER","Lembasbrot","es","Lembasbrot","e","maskulin"),
    # cram, Lebensmittel aus HdR
    #dekliniere_substantiv("NOUN_CRAM_RATION"
    dekliniere_substantiv("NOUN_FOOD_RATION","Essensration","","Essensration","en","feminin"),
    dekliniere_substantiv("NOUN_K_RATION","Kampfration","","Kampfration","en","feminin"),
    dekliniere_substantiv("NOUN_C_RATION","Notration","","Notration","en","feminin"),
    dekliniere_substantiv("NOUN_TIN","Dose","","Dose","en","feminin"),
    "",
    "/* Coins */",
    dekliniere_substantiv("NOUN_GOLD_PIECE","Goldst�ck","es","Goldst�ck","e","neutrum"),
    "",
    "/* Spellbooks, unidentified */",
    dekliniere_substantiv("MADE_OF_SPE_VELLUM", "Velum", "s", "", "s", "neutrum"),
    "",
    "/* all monster names */",
    dekliniere_substantiv("NOUN_GIANT_ANT","Riesenameise","","Riesenameise","en","feminin"),
    dekliniere_substantiv("NOUN_KILLER_BEE","Killerbiene","","Killerbiene","en","feminin"),
    dekliniere_substantiv("NOUN_SOLDIER_ANT","Ameisensoldat","en","Ameisensoldat","en","maskulin"),
    dekliniere_substantiv("NOUN_FIRE_ANT","Feuerameise","","Feuerameise","en","feminin"),
    dekliniere_substantiv("NOUN_GIANT_BEETLE", "Riesenk�fer", "s", "Riesenk�fer", "", "maskulin"),
    dekliniere_substantiv("NOUN_QUEEN_BEE","Bienenk�nigin","","Bienenk�niginn","en","feminin"),
    #dekliniere_substantiv("NOUN_ACID_BLOB"
    #dekliniere_substantiv("NOUN_QUIVERING_BLOB"
    #dekliniere_substantiv("NOUN_GELATINOUS_CUBE"
    #dekliniere_substantiv("NOUN_CHICKATRICE"
    #dekliniere_substantiv("NOUN_COCKATRICE"
    #dekliniere_substantiv("NOUN_PYROLISK"
    dekliniere_substantiv("NOUN_JACKAL", "Schakal", "s", "Schakal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FOX","Fuchs","es","F�chs","e","maskulin"),
    dekliniere_substantiv("NOUN_COYOTE","Kojote","en","Kojote","en","maskulin"),
    dekliniere_substantiv("NOUN_WEREJACKAL", "Werschakal", "s", "Werschakal", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_LITTLE_DOG"
    dekliniere_substantiv("NOUN_DOG", "Hund", "es", "Hund", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_LARGE_DOG"
    dekliniere_substantiv("NOUN_DINGO","Dingo","s","Dingo","s","maskulin"),
    dekliniere_substantiv("NOUN_WOLF","Wolf","es","W�lf","e","maskulin"),
    dekliniere_substantiv("NOUN_WEREWOLF","Werwolf","es","Werw�lf","e","maskulin"),
    #dekliniere_substantiv("NOUN_WARG"
    #dekliniere_substantiv("NOUN_WINTER_WOLF_CUB"
    #dekliniere_substantiv("NOUN_WINTER_WOLF"
    #dekliniere_substantiv("NOUN_HELL_HOUND_PUP"
    #dekliniere_substantiv("NOUN_HELL_HOUND"
    #dekliniere_substantiv("NOUN_CERBERUS"
    #dekliniere_substantiv("NOUN_GAS_SPORE"
    #dekliniere_substantiv("NOUN_FLOATING_EYE"
    #dekliniere_substantiv("NOUN_FREEZING_SPHERE"
    #dekliniere_substantiv("NOUN_FLAMING_SPHERE"
    #dekliniere_substantiv("NOUN_SHOCKING_SPHERE"
    #dekliniere_substantiv("NOUN_BEHOLDER"
    dekliniere_substantiv("NOUN_KITTEN", "K�tzchen", "s", "K�tzchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_HOUSECAT","Hauskatze","","Hauskatze","en","feminin"),
    dekliniere_substantiv("NOUN_JAGUAR","Jaguar","s","Jaguar","e","maskulin"),
    dekliniere_substantiv("NOUN_LYNX","Luchs","es","Luchs","e","maskulin"),
    dekliniere_substantiv("NOUN_PANTHER","Panther","s","Panther","","maskulin"),
    #dekliniere_substantiv("NOUN_LARGE_CAT"
    dekliniere_substantiv("NOUN_TIGER","Tiger","s","Tiger","","maskulin"),
    #dekliniere_substantiv("NOUN_GREMLIN"
    #dekliniere_substantiv("NOUN_GARGOYLE"
    #dekliniere_substantiv("NOUN_WINGED_GARGOYLE"
    #dekliniere_substantiv("NOUN_HOBBIT"
    dekliniere_substantiv("NOUN_DWARF","Zwerg","es","Zwerg","e","maskulin"),
    #dekliniere_substantiv("NOUN_BUGBEAR"
    #dekliniere_substantiv("NOUN_DWARF_LORD"
    #dekliniere_substantiv("NOUN_DWARF_KING"
    #dekliniere_substantiv("NOUN_MIND_FLAYER"
    #dekliniere_substantiv("NOUN_MASTER_MIND_FLAYER"
    #dekliniere_substantiv("NOUN_MANES"
    #dekliniere_substantiv("NOUN_HOMUNCULUS"
    #dekliniere_substantiv("NOUN_IMP"
    #dekliniere_substantiv("NOUN_LEMURE"
    #dekliniere_substantiv("NOUN_QUASIT"
    #dekliniere_substantiv("NOUN_TENGU"
    #dekliniere_substantiv("NOUN_BLUE_JELLY"
    #dekliniere_substantiv("NOUN_SPOTTED_JELLY"
    #dekliniere_substantiv("NOUN_OCHRE_JELLY"
    dekliniere_substantiv("NOUN_KOBOLD","Kobold","es","Kobold","e","maskulin"),
    #dekliniere_substantiv("NOUN_LARGE_KOBOLD"
    #dekliniere_substantiv("NOUN_KOBOLD_LORD"
    #dekliniere_substantiv("NOUN_KOBOLD_SHAMAN"
    #dekliniere_substantiv("NOUN_LEPRECHAUN"
    #dekliniere_substantiv("NOUN_SMALL_MIMIC"
    #dekliniere_substantiv("NOUN_LARGE_MIMIC"
    #dekliniere_substantiv("NOUN_GIANT_MIMIC"
    dekliniere_substantiv("NOUN_WOOD_NYMPH",     "Baumnymphe",   "", "Baumnymphe",   "en", "feminin"),
    dekliniere_substantiv("NOUN_WATER_NYMPH",    "Wassernymphe", "", "Wassernymphe", "en", "feminin"),
    dekliniere_substantiv("NOUN_MOUNTAIN_NYMPH", "Bergnymphe",   "", "Bergnymphe",   "en", "feminin"),
    "/* In reality goblin would be an appropriate translation for 'Kobold'. In tolkienspeek goblin is a synonym for orc.*/",
    dekliniere_substantiv("NOUN_GOBLIN","Goblin","s","Goblin","","maskulin"),
    #dekliniere_substantiv("NOUN_HOBGOBLIN"
    dekliniere_substantiv("NOUN_ORC","Ork","s","Ork","s","maskulin"),
    dekliniere_substantiv("NOUN_HILL_ORC","H�gelork","s","H�gelork","s","maskulin"),
    #dekliniere_substantiv("NOUN_MORDOR_ORC"
    #dekliniere_substantiv("NOUN_URUK_HAI"
    #dekliniere_substantiv("NOUN_ORC_SHAMAN"
    #dekliniere_substantiv("NOUN_ORC_CAPTAIN"
    #dekliniere_substantiv("NOUN_ROCK_PIERCER"
    #dekliniere_substantiv("NOUN_IRON_PIERCER"
    #dekliniere_substantiv("NOUN_GLASS_PIERCER"
    #dekliniere_substantiv("NOUN_ROTHE"
    #dekliniere_substantiv("NOUN_MUMAK"
    #dekliniere_substantiv("NOUN_LEOCROTTA"
    #dekliniere_substantiv("NOUN_WUMPUS"
    #dekliniere_substantiv("NOUN_TITANOTHERE"
    #dekliniere_substantiv("NOUN_BALUCHITHERIUM"
    dekliniere_substantiv("NOUN_MASTODON", "Mastodon", "es", "Mastodon", "en", "neutrum"),
    dekliniere_substantiv("NOUN_SEWER_RAT", "Kanalratte", "", "Kanalratte", "en", "feminin"),
    #dekliniere_substantiv("NOUN_GIANT_RAT"
    #dekliniere_substantiv("NOUN_RABID_RAT"
    dekliniere_substantiv("NOUN_WERERAT", "Werratte", "", "Werratte", "en", "feminin"),
    #dekliniere_substantiv("NOUN_ROCK_MOLE"
    dekliniere_substantiv("NOUN_WOODCHUCK", "Murmeltier", "es", "Murmeltier", "e", "neutrum"),
    #dekliniere_substantiv("NOUN_CAVE_SPIDER"
    #dekliniere_substantiv("NOUN_CENTIPEDE"
    #dekliniere_substantiv("NOUN_GIANT_SPIDER"
    dekliniere_substantiv("NOUN_SCORPION", "Skorpion", "s", "Skorpion", "e", "maskulin"),
    dekliniere_substantiv("NOUN_LURKER_ABOVE", "Laurer", "s", "Laurer", "", "maskulin"),
    dekliniere_substantiv("NOUN_TRAPPER", "Fallensteller", "s", "Fallensteller", "", "maskulin"), 
    #dekliniere_substantiv("NOUN_WHITE_UNICORN"
    #dekliniere_substantiv("NOUN_GRAY_UNICORN"
    #dekliniere_substantiv("NOUN_BLACK_UNICORN"
    dekliniere_substantiv("NOUN_PONY","Pony","s","Pony","s","neutrum"),
    dekliniere_substantiv("NOUN_HORSE","Pferd","es","Pferd","e","neutrum"),
    dekliniere_substantiv("NOUN_WARHORSE","Schlachtro�","es","Schlachtro�","e","neutrum"),
    #dekliniere_substantiv("NOUN_FOG_CLOUD"
    #dekliniere_substantiv("NOUN_DUST_VORTEX"
    #dekliniere_substantiv("NOUN_ICE_VORTEX"
    #dekliniere_substantiv("NOUN_ENERGY_VORTEX"
    #dekliniere_substantiv("NOUN_STEAM_VORTEX"
    #dekliniere_substantiv("NOUN_FIRE_VORTEX"
    #dekliniere_substantiv("NOUN_BABY_LONG_WORM"
    #dekliniere_substantiv("NOUN_BABY_PURPLE_WORM"
    #dekliniere_substantiv("NOUN_LONG_WORM"
    #dekliniere_substantiv("NOUN_PURPLE_WORM"
    #dekliniere_substantiv("NOUN_GRID_BUG"
    #dekliniere_substantiv("NOUN_XAN"
    #dekliniere_substantiv("NOUN_YELLOW_LIGHT",
    #dekliniere_substantiv("NOUN_BLACK_LIGHT"
    #dekliniere_substantiv("NOUN_ZRUTY"
    #dekliniere_substantiv("NOUN_COUATL"
    #dekliniere_substantiv("NOUN_ALEAX"
    dekliniere_substantiv("NOUN_ANGEL", "Engel", "s", "Engel", "", "maskulin"),
    #dekliniere_substantiv("NOUN_KI_RIN"
    #dekliniere_substantiv("NOUN_ARCHON"
    dekliniere_substantiv("NOUN_BAT", "Fledermaus", "", "Flederm�use", "e", "feminin"),
    dekliniere_substantiv("NOUN_GIANT_BAT", "Riesenfledermaus", "", "Riesenflederm�use", "e", "feminin"),
    dekliniere_substantiv("NOUN_RAVEN", "Rabe", "en", "Rabe", "en", "maskulin"),
    dekliniere_substantiv("NOUN_VAMPIRE_BAT", "Vampirfledermaus", "", "Vampirflederm�use", "e", "feminin"),
    #dekliniere_substantiv("NOUN_PLAINS_CENTAUR"
    #dekliniere_substantiv("NOUN_FOREST_CENTAUR"
    #dekliniere_substantiv("NOUN_MOUNTAIN_CENTAUR"
    #dekliniere_substantiv("NOUN_BABY_GRAY_DRAGON"
    #dekliniere_substantiv("NOUN_BABY_SILVER_DRAGON"
    #dekliniere_substantiv("NOUN_BABY_SHIMMERING_DRAGON"
    #dekliniere_substantiv("NOUN_BABY_RED_DRAGON"
    #dekliniere_substantiv("NOUN_BABY_WHITE_DRAGON"
    #dekliniere_substantiv("NOUN_BABY_ORANGE_DRAGON"
    #dekliniere_substantiv("NOUN_BABY_BLACK_DRAGON"
    #dekliniere_substantiv("NOUN_BABY_BLUE_DRAGON"
    #dekliniere_substantiv("NOUN_BABY_GREEN_DRAGON"
    #dekliniere_substantiv("NOUN_BABY_YELLOW_DRAGON"
    dekliniere_substantiv("NOUN_GRAY_DRAGON", "Graudrache", "en", "Graudrache", "en", "maskulin"),
    dekliniere_substantiv("NOUN_SILVER_DRAGON", "Silberdrache", "en", "Silberdrache", "en", "maskulin"),
    dekliniere_substantiv("NOUN_SHIMMERING_DRAGON", "Glanzdrache", "en", "Glanzdrache", "en", "maskulin"),
    dekliniere_substantiv("NOUN_RED_DRAGON", "Rotdrache", "en", "Rotdrache", "en", "maskulin"),
    dekliniere_substantiv("NOUN_WHITE_DRAGON", "Wei�drache", "en", "Wei�drache", "en", "maskulin"),
    dekliniere_substantiv("NOUN_ORANGE_DRAGON", "Orangedrache", "en", "Orangedrache", "en", "maskulin"),
    dekliniere_substantiv("NOUN_BLACK_DRAGON", "Schwarzdrache", "en", "Schwarzdrache", "en", "maskulin"),
    dekliniere_substantiv("NOUN_BLUE_DRAGON", "Blaudrache", "en", "Blaudrache", "en", "maskulin"),
    dekliniere_substantiv("NOUN_GREEN_DRAGON", "Gr�ndrache", "en", "Gr�ndrache", "en", "maskulin"),
    dekliniere_substantiv("NOUN_YELLOW_DRAGON", "Gelbdrache", "en", "Gelbdrache", "en", "maskulin"),
    #dekliniere_substantiv("NOUN_STALKER"
    dekliniere_substantiv("NOUN_STALKER", "Nachsteller", "s", "Nachsteller", "", "maskulin"),
    dekliniere_substantiv("NOUN_AIR_ELEMENTAL",   "Luftelementar",   "es", "Luftelementar",   "e", "neutrum"),
    dekliniere_substantiv("NOUN_FIRE_ELEMENTAL",  "Feuerelementar",  "es", "Feuerelementar",  "e", "neutrum"),
    dekliniere_substantiv("NOUN_EARTH_ELEMENTAL", "Erdelementar",    "es", "Erdelementar",    "e", "neutrum"),
    dekliniere_substantiv("NOUN_WATER_ELEMENTAL", "Wasserelementar", "es", "Wasserelementar", "e", "neutrum"),
    dekliniere_substantiv("NOUN_LICHEN", "Flechte", "", "Flechte", "en", "feminin"),
    #dekliniere_substantiv("NOUN_BROWN_MOLD"
    #dekliniere_substantiv("NOUN_YELLOW_MOLD"
    #dekliniere_substantiv("NOUN_GREEN_MOLD"
    #dekliniere_substantiv("NOUN_RED_MOLD"
    #dekliniere_substantiv("NOUN_SHRIEKER"
    #dekliniere_substantiv("NOUN_VIOLET_FUNGUS"
    dekliniere_substantiv("NOUN_GNOME", "Gnom", "en", "Gnom", "en", "maskulin"),
    #dekliniere_substantiv("NOUN_GNOME_LORD"
    #dekliniere_substantiv("NOUN_GNOMISH_WIZARD"
    #dekliniere_substantiv("NOUN_GNOME_KING"
    dekliniere_substantiv("NOUN_GIANT",       "Riese", "en", "Riese", "en", "maskulin"),
    dekliniere_substantiv("NOUN_STONE_GIANT", "Steinriese", "en", "Steinriese", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HILL_GIANT",  "H�gelriese", "en", "H�gelriese", "en", "maskulin"),
    dekliniere_substantiv("NOUN_FIRE_GIANT",  "Feuerriese", "en", "Feuerriese", "en", "maskulin"),
    dekliniere_substantiv("NOUN_FROST_GIANT", "Frostriese", "en", "Frostriese", "en", "maskulin"),
    dekliniere_substantiv("NOUN_STORM_GIANT", "Strumriese", "en", "Strumriese", "en", "maskulin"),
    dekliniere_substantiv("NOUN_ETTIN", "Ettin", "", "Ettin", "", "maskulin"),
    dekliniere_substantiv("NOUN_TITAN", "Titane", "en", "Titan", "en", "maskulin"),
    dekliniere_substantiv("NOUN_MINOTAUR","Minotaurus","","Minotaur","en","maskulin"),
    #dekliniere_substantiv("NOUN_JABBERWOCK"
    #dekliniere_substantiv("NOUN_VORPAL_JABBERWOCK"
    #dekliniere_substantiv("NOUN_KEYSTONE_KOP"
    #dekliniere_substantiv("NOUN_KOP_SERGEANT"
    #dekliniere_substantiv("NOUN_KOP_LIEUTENANT"
    #dekliniere_substantiv("NOUN_KOP_KAPTAIN"
    #dekliniere_substantiv("NOUN_LICH"
    #dekliniere_substantiv("NOUN_DEMILICH"
    #dekliniere_substantiv("NOUN_MASTER_LICH"
    #dekliniere_substantiv("NOUN_ARCH_LICH"
    #dekliniere_substantiv("NOUN_KOBOLD_MUMMY"
    #dekliniere_substantiv("NOUN_GNOME_MUMMY"
    #dekliniere_substantiv("NOUN_ORC_MUMMY"
    #dekliniere_substantiv("NOUN_DWARF_MUMMY"
    #dekliniere_substantiv("NOUN_ELF_MUMMY"
    #dekliniere_substantiv("NOUN_HUMAN_MUMMY"
    #dekliniere_substantiv("NOUN_ETTIN_MUMMY"
    #dekliniere_substantiv("NOUN_GIANT_MUMMY"
    #dekliniere_substantiv("NOUN_RED_NAGA_HATCHLING"
    #dekliniere_substantiv("NOUN_BLACK_NAGA_HATCHLING"
    #dekliniere_substantiv("NOUN_GOLDEN_NAGA_HATCHLING"
    #dekliniere_substantiv("NOUN_GUARDIAN_NAGA_HATCHLING"
    #dekliniere_substantiv("NOUN_RED_NAGA"
    #dekliniere_substantiv("NOUN_BLACK_NAGA"
    #dekliniere_substantiv("NOUN_GOLDEN_NAGA"
    #dekliniere_substantiv("NOUN_GUARDIAN_NAGA"
    dekliniere_substantiv("NOUN_OGRE", "Oger", "s", "Oger", "", "maskulin"),
    #dekliniere_substantiv("NOUN_OGRE_LORD"
    #dekliniere_substantiv("NOUN_OGRE_KING"
    #dekliniere_substantiv("NOUN_GRAY_OOZE"
    #dekliniere_substantiv("NOUN_BROWN_PUDDING"
    #dekliniere_substantiv("NOUN_BLACK_PUDDING"
    #dekliniere_substantiv("NOUN_GREEN_SLIME"
    #dekliniere_substantiv("NOUN_QUANTUM_MECHANIC"
    #dekliniere_substantiv("NOUN_RUST_MONSTER"
    #dekliniere_substantiv("NOUN_DISENCHANTER"
    #dekliniere_substantiv("NOUN_GARTER_SNAKE"
    dekliniere_substantiv("NOUN_SNAKE","Schlange","","Schlange","en","feminin"),
    #dekliniere_substantiv("NOUN_WATER_MOCCASIN"
    #dekliniere_substantiv("NOUN_PIT_VIPER"
    dekliniere_substantiv("NOUN_PYTHON", "Python", "s", "Python", "s", "feminin"),
    dekliniere_substantiv("NOUN_COBRA", "Kobra", "", "Kobra", "s", "feminin"),
    dekliniere_substantiv("NOUN_TROLL", "Troll",  "es", "Troll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ICE_TROLL", "Eistroll",  "es", "Eistroll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ROCK_TROLL", "Steintroll",  "es", "Steintroll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WATER_TROLL", "Wassertroll",  "es", "Wassertroll", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_OLOG_HAI"
    #dekliniere_substantiv("NOUN_UMBER_HULK"
    dekliniere_substantiv("NOUN_VAMPIRE","Vampir","es","Vampir","e","maskulin"),
    #dekliniere_substantiv("NOUN_VAMPIRE_LORD"
    #dekliniere_substantiv("NOUN_VAMPIRE_MAGE"
    #dekliniere_substantiv("NOUN_VLAD_THE_IMPALER"
    #dekliniere_substantiv("NOUN_BARROW_WIGHT"
    #dekliniere_substantiv("NOUN_WRAITH"
    #dekliniere_substantiv("NOUN_NAZGUL"
    #dekliniere_substantiv("NOUN_XORN"
    dekliniere_substantiv("NOUN_MONKEY", "Affe", "en", "Affe", "en", "maskulin"),
    #dekliniere_substantiv("NOUN_APE", "Menschenaffe", "en", "Menschenaffe", "en", "maskulin"),
    #dekliniere_substantiv("NOUN_OWLBEAR"
    dekliniere_substantiv("NOUN_YETI","Yeti","s","Yeti","s","maskulin"),
    #dekliniere_substantiv("NOUN_CARNIVOROUS_APE"
    #dekliniere_substantiv("NOUN_SASQUATCH"
    #dekliniere_substantiv("NOUN_KOBOLD_ZOMBIE"
    #dekliniere_substantiv("NOUN_GNOME_ZOMBIE"
    #dekliniere_substantiv("NOUN_ORC_ZOMBIE"
    #dekliniere_substantiv("NOUN_DWARF_ZOMBIE"
    #dekliniere_substantiv("NOUN_ELF_ZOMBIE"
    #dekliniere_substantiv("NOUN_HUMAN_ZOMBIE"
    #dekliniere_substantiv("NOUN_ETTIN_ZOMBIE"
    #dekliniere_substantiv("NOUN_GIANT_ZOMBIE"
    dekliniere_substantiv("NOUN_GHOUL","Ghul","s","Ghul","e","maskulin"),
    dekliniere_substantiv("NOUN_SKELETON", "Skelett", "es", "Skelett", "e", "neutrum"),
    #dekliniere_substantiv("NOUN_STRAW_GOLEM"
    #dekliniere_substantiv("NOUN_PAPER_GOLEM"
    #dekliniere_substantiv("NOUN_ROPE_GOLEM"
    #dekliniere_substantiv("NOUN_GOLD_GOLEM"
    #dekliniere_substantiv("NOUN_LEATHER_GOLEM"
    #dekliniere_substantiv("NOUN_WOOD_GOLEM"
    #dekliniere_substantiv("NOUN_FLESH_GOLEM"
    #dekliniere_substantiv("NOUN_CLAY_GOLEM"
    #dekliniere_substantiv("NOUN_STONE_GOLEM"
    #dekliniere_substantiv("NOUN_GLASS_GOLEM"
    #dekliniere_substantiv("NOUN_IRON_GOLEM"
    dekliniere_substantiv("NOUN_HUMAN", "Mensch", "en", "Mensch", "en", "maskulin"),
    #dekliniere_substantiv("NOUN_ELF"
    #dekliniere_substantiv("NOUN_WOODLAND_ELF"
    #dekliniere_substantiv("NOUN_GREEN_ELF"
    #dekliniere_substantiv("NOUN_GREY_ELF"
    #dekliniere_substantiv("NOUN_ELF_LORD"
    #dekliniere_substantiv("NOUN_ELVENKING"
    #dekliniere_substantiv("NOUN_DOPPELGANGER"
    #dekliniere_substantiv("NOUN_NURSE",
    #dekliniere_substantiv("NOUN_SHOPKEEPER"
    #dekliniere_substantiv("NOUN_GUARD"
    #dekliniere_substantiv("NOUN_PRISONER"
    #dekliniere_substantiv("NOUN_ORACLE"
    #dekliniere_substantiv("NOUN_ALIGNED_PRIEST"
    #dekliniere_substantiv("NOUN_HIGH_PRIEST"
    #dekliniere_substantiv("NOUN_SOLDIER"
    #dekliniere_substantiv("NOUN_SERGEANT"
    #dekliniere_substantiv("NOUN_LIEUTENANT"
    #dekliniere_substantiv("NOUN_CAPTAIN"
    #dekliniere_substantiv("NOUN_WATCHMAN"
    #dekliniere_substantiv("NOUN_WATCH_CAPTAIN"
    #dekliniere_substantiv("NOUN_MEDUSA"
    #dekliniere_substantiv("NOUN_WIZARD_OF_YENDOR"
    #dekliniere_substantiv("NOUN_CROESUS"
    #dekliniere_substantiv("NOUN_CHARON"
    #dekliniere_substantiv("NOUN_GHOST"
    #dekliniere_substantiv("NOUN_SHADE"
    #dekliniere_substantiv("NOUN_WATER_DEMON"
    #dekliniere_substantiv("NOUN_HORNED_DEVIL"
    #dekliniere_substantiv("NOUN_SUCCUBUS"
    #dekliniere_substantiv("NOUN_INCUBUS"
    #dekliniere_substantiv("NOUN_ERINYS"
    #dekliniere_substantiv("NOUN_BARBED_DEVIL"
    #dekliniere_substantiv("NOUN_MARILITH"
    #dekliniere_substantiv("NOUN_VROCK"
    #dekliniere_substantiv("NOUN_HEZROU"
    #dekliniere_substantiv("NOUN_BONE_DEVIL"
    #dekliniere_substantiv("NOUN_ICE_DEVIL"
    #dekliniere_substantiv("NOUN_NALFESHNEE"
    #dekliniere_substantiv("NOUN_PIT_FIEND"
    #dekliniere_substantiv("NOUN_BALROG"
    #dekliniere_substantiv("NOUN_JUIBLEX"
    #dekliniere_substantiv("NOUN_YEENOGHU"
    #dekliniere_substantiv("NOUN_ORCUS"
    #dekliniere_substantiv("NOUN_GERYON"
    #dekliniere_substantiv("NOUN_DISPATER"
    #dekliniere_substantiv("NOUN_BAALZEBUB"
    #dekliniere_substantiv("NOUN_ASMODEUS"
    #dekliniere_substantiv("NOUN_DEMOGORGON"
    dekliniere_substantiv("NOUN_DEATH","Tod","es","Tod","e","maskulin"),
    dekliniere_substantiv("NOUN_PESTILENCE","Pest","","","","feminin"),
    dekliniere_substantiv("NOUN_FAMINE","Hunger","s","","","maskulin"),
    #dekliniere_substantiv("NOUN_MAIL_DAEMON"
    dekliniere_substantiv("NOUN_DJINNI", "Dschinn", "es", "Dschinn", "", "maskulin"),
    #dekliniere_substantiv("NOUN_SANDESTIN"
    dekliniere_substantiv("NOUN_JELLYFISH", "Qualle", "", "Qualle", "en", "feminin"),
    dekliniere_substantiv("NOUN_PIRANHA","Piranha","s","Piranha","s","maskulin"),
    dekliniere_substantiv("NOUN_SHARK","Hai","es","Hai","e","maskulin"),
    dekliniere_substantiv("NOUN_GIANT_EEL", "Riesenaal", "es", "Riesenaal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ELECTRIC_EEL", "Zitteraal", "es", "Zitteraal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_KRAKEN", "Krake", "en", "Krake", "en", "maskulin"),
    dekliniere_substantiv("NOUN_NEWT", "Molch","es","Molch","e","maskulin"),
    dekliniere_substantiv("NOUN_GECKO", "Gecko", "s", "Gecko", "s", "maskulin"),
    dekliniere_substantiv("NOUN_IGUANA", "Leguan", "s", "Leguan", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_BABY_CROCODILE"
    dekliniere_substantiv("NOUN_LIZARD", "Eidechse", "", "Eidechse", "en", "feminin"),
    dekliniere_substantiv("NOUN_CHAMELEON", "Cham�leon", "s", "Cham�leon", "s", "neutrum"),
    dekliniere_substantiv("NOUN_CROCODILE","Krokodil","s","Krokodil","e","maskulin"),
    dekliniere_substantiv("NOUN_SALAMANDER", "Salamander", "s", "Salamander", "", "maskulin"),
    #dekliniere_substantiv("NOUN_LONG_WORM_TAIL"
    #dekliniere_substantiv("NOUN_ARCHEOLOGIST"
    #dekliniere_substantiv("NOUN_BARBARIAN"
    #dekliniere_substantiv("NOUN_CAVEMAN"
    #dekliniere_substantiv("NOUN_CAVEWOMAN"
    #dekliniere_substantiv("NOUN_HEALER"
    #dekliniere_substantiv("NOUN_KNIGHT"
    #dekliniere_substantiv("NOUN_MONK"
    #dekliniere_substantiv("NOUN_PRIEST"
    #dekliniere_substantiv("NOUN_PRIESTESS"
    #dekliniere_substantiv("NOUN_RANGER"
    #dekliniere_substantiv("NOUN_ROGUE"
    #dekliniere_substantiv("NOUN_SAMURAI"
    #dekliniere_substantiv("NOUN_TOURIST"
    #dekliniere_substantiv("NOUN_VALKYRIE"
    #dekliniere_substantiv("NOUN_WIZARD"
    #dekliniere_substantiv("NOUN_LORD_CARNARVON"
    #dekliniere_substantiv("NOUN_PELIAS"
    #dekliniere_substantiv("NOUN_SHAMAN_KARNOV"
    #dekliniere_substantiv("NOUN_EARENDIL"
    #dekliniere_substantiv("NOUN_ELWING"
    #dekliniere_substantiv("NOUN_HIPPOCRATES"
    #dekliniere_substantiv("NOUN_KING_ARTHUR"
    #dekliniere_substantiv("NOUN_GRAND_MASTER"
    #dekliniere_substantiv("NOUN_ARCH_PRIEST"
    #dekliniere_substantiv("NOUN_ORION"
    #dekliniere_substantiv("NOUN_MASTER_OF_THIEVES"
    #dekliniere_substantiv("NOUN_LORD_SATO"
    #dekliniere_substantiv("NOUN_TWOFLOWER"
    #dekliniere_substantiv("NOUN_NORN"
    #dekliniere_substantiv("NOUN_NEFERET_THE_GREEN"
    #dekliniere_substantiv("NOUN_MINION_OF_HUHETOTL"
    #dekliniere_substantiv("NOUN_THOTH_AMON"
    #dekliniere_substantiv("NOUN_CHROMATIC_DRAGON"
    #dekliniere_substantiv("NOUN_GOBLIN_KING"
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
    ""
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
puts "#ifndef _GERMAN_H\n#define _GERMAN_H 1\n#include <stddef.h>\n"
ausgabe_definitions
ausgabe_nouns
ausgabe_adjectives
ausgabe_verbs
puts "#endif /* !_GERMAN_H */"
