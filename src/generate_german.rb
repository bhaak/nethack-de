#!/usr/bin/env ruby
# coding: iso-8859-1

require 'set'

require './german_verb'

# 
class String
	def each
		yield self
	end
end

# use the genitive-e ("des Zauberbuches" vs. "des Zauberbuchs"
$genitiv_e = true;
# use the dativ-e ("dem Zauberbuche" vs. "dem Zauberbuch"
$dativ_e = false;

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

/* Diese Datei wird automatisch durch generate_german.rb erzeugt. */

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

  # h�ngt eine unver�nderliche Zeichenkette an das deklinierte Wort an
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
                          pluralstamm, nominativ_plural_endung, geschlecht="", fugenelement="", zusatz="")

  throw "Geschlecht fuer #{bezeichner} nicht angeben" if geschlecht == ""

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

def dekliniere_eigenname(bezeichner, singularstamm="", geschlecht="")
  throw "Geschlecht fuer #{bezeichner} nicht angeben" if geschlecht == ""
  throw "Singularstamm fuer #{bezeichner} nicht angeben" if singularstamm == ""

  namen = []
  namen << unregelmaessiges_wort(bezeichner, singularstamm, [$nom,$akk,$dat], geschlecht, $sg)
  if ['s','x','z'].include?(singularstamm[-1..-1]) then
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

def dekliniere_adjektivisches_substantiv(bezeichner, stamm="", artikel="")
  throw "Stamm fuer #{bezeichner} nicht angeben" if stamm == ""
  throw "Artikel fuer #{bezeichner} nicht angeben" if artikel == ""

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
    raise "Unbekanntes Geschlecht #{artikel} f�r adjektivisches Substantiv"
  end
  adjektive
end

def dekliniere_adjektiv(bezeichner, stamm="")
  throw "Stamm fuer #{bezeichner} nicht angeben" if stamm == ""

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
    konjugiere_verb("VERB_SICH_FUEHLEN","f�hl"),
    konjugiere_verb("VERB_FUEHLEN","f�hl"),
    konjugiere_verb("VERB_MERKEN","merk"),
    konjugiere_verb("VERB_LANDEN","land"),
    "",
    konjugiere_verb("VERB_FEEL","sp�r"),
    konjugiere_verb("VERB_HEAR","h�r"),
    konjugiere_verb("VERB_SMELL","riech"),
    konjugiere_verb("VERB_MISS","verfehl"),
    konjugiere_verb("VERB_KILL","t�t"),
    konjugiere_verb("VERB_TOUCH","ber�hr"),
    konjugiere_verb("VERB_SCHLEUDERN","schleuder"),
    konjugiere_verb("VERB_ZERSPRINGEN","zerspring"),
    konjugiere_verb("VERB_GELINGEN","geling"),
    konjugiere_verb("VERB_MACHEN","mach"),
    konjugiere_verb("VERB_VIBRIEREN","vibrier"),
    konjugiere_verb("VERB_RUTSCHEN","rutsch"),
    konjugiere_verb("VERB_GLEITEN","gleit"),
    konjugiere_verb("VERB_GEHEN","geh"),
    konjugiere_verb("VERB_FUEHREN","f�hr"),
    konjugiere_verb("VERB_ABSORBIEREN","absorbier"),
    konjugiere_verb("VERB_ZERTEILEN","zerteil"),
    konjugiere_verb("VERB_OEFFNEN","�ffne"),
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
    konjugiere_verb("VERB_VERKUENDEN","verk�nd"),
    konjugiere_verb("VERB_BEMERKEN","bemerk"),
    konjugiere_verb("VERB_LEGEN","leg"),
    konjugiere_verb("VERB_SPUEREN","sp�r"),
    konjugiere_verb("VERB_VERWANDELN","verwandel"),
    konjugiere_verb("VERB_VERSUCHEN","versuch"),
    konjugiere_verb("VERB_FINDEN","find"),
    konjugiere_verb("VERB_VERSCHWINDEN","verschwind"),
    konjugiere_verb("VERB_WUENSCHEN","w�nsch"),
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
    konjugiere_verb("VERB_YELL","br�ll"),
    konjugiere_verb("VERB_WUERDEN","w�rd"), # TODO
    "",
    "/* unregelm�ssige Verben */",
    Verb_alt.new("VERB_SEIN", "", "", ["bist", "ist", "seid", "sind"]),
    Verb_alt.new("VERB_HAVE", "", "", ["hast", "hat", "habt", "haben"]),
    Verb_alt.new("VERB_READ", "", "", ["liest", "liest", "lest","lesen"]),
    Verb_alt.new("VERB_CAN", "", "", ["kannst", "kann", "k�nnt","k�nnen"]),
    Verb_alt.new("VERB_KICK", "", "", ["trittst", "tritt", "tretet","treten"]),
    Verb_alt.new("VERB_BITE", "", "", ["bei�t", "bei�t", "bei�t","bei�en"]),
    Verb_alt.new("VERB_HIT", "", "", ["triffst", "trifft", "trefft", "treffen"]),
    Verb_alt.new("VERB_SEE", "", "", ["siehst", "sieht", "seht", "sehen"]),
    Verb_alt.new("VERB_CONTAIN", "", "", ["enth�lst", "enth�lt", "enthaltet","enthalten"]),
    Verb_alt.new("VERB_OPEN", "", "auf", ["gehst", "geht", "geht","gehen"]),
    Verb_alt.new("VERB_EAT", "", "auf", ["isst", "isst", "esst","essen"]),
    Verb_alt.new("VERB_DROP", "", "fallen", ["l�sst", "l�sst", "lasst","lassen"]),    # "You drop a potion."
    Verb_alt.new("VERB_OBJECT_DROPS", "", "", ["f�llst", "f�llt", "fallt", "fallen"]), # e.g "A potion drops to the floor."
    Verb_alt.new("VERB_SEEM","","zu sein", ["scheinst", "scheint","scheint", "scheinen"]),
    Verb_alt.new("VERB_SCHLAGEN",  "","", ["schl�gst", "schl�gt","schlagen", "schlagt"]),
    Verb_alt.new("VERB_ZUSCHLAGEN","","zu", ["schl�gst", "schl�gt","schlagen", "schlagt"]),
    Verb_alt.new("VERB_STECHEN","","", ["stichst", "sticht","stecht", "stechen"]),
    Verb_alt.new("VERB_ZUSTECHEN","","zu", ["stichst", "sticht","stecht", "stechen"]),
    Verb_alt.new("VERB_SCHIESSEN", "", "", ["schie�t", "schie�t", "schie�t","schie�en"]),
    Verb_alt.new("VERB_FANGEN", "", "", ["f�ngst", "f�ngt", "fangt","fangen"]),
    Verb_alt.new("VERB_ABBRECHEN", "", "ab", ["brichst", "bricht", "brecht","brechen"]),
    Verb_alt.new("VERB_EINATMEN", "", "ein", ["atmest", "atmet", "atmet","atmen"]),
    Verb_alt.new("VERB_TRAGEN", "", "", ["tr�gst", "tr�gt", "tragt","tragen"]),
    Verb_alt.new("VERB_KOENNEN", "", "", ["kannst", "kann", "k�nnt","k�nnen"]),
    Verb_alt.new("VERB_ZITTERN", "", "", ["zitterst", "zittert", "zittert","zittern"]),
    Verb_alt.new("VERB_GLAENZEN", "", "", ["gl�nzt", "gl�nzt", "gl�nzt","gl�nzen"]),
    Verb_alt.new("VERB_NEHMEN", "", "", ["nimmst", "nimmt", "nehmt","nehmen"]),
    Verb_alt.new("VERB_ZUSCHNUEREN", "", "zu", ["schn�rst", "schn�rt", "schn�rt","schn�ren"]),
    Verb_alt.new("VERB_PASSEN", "", "", ["passt", "passt", "passt","passen"]),
    Verb_alt.new("VERB_HALTEN", "", "", ["h�lst", "h�lt", "haltet","halten"]),
    Verb_alt.new("VERB_HINDERN", "", "", ["hinderst", "hindert", "hindert","hindern"]),
    Verb_alt.new("VERB_WERDEN", "", "", ["wirst", "wird", "werdet", "werden"]),
    Verb_alt.new("VERB_ZERFALLEN", "", "", ["zerf�llst", "zerf�llt", "zerfallt", "zerfallen"]),
    Verb_alt.new("VERB_ERSCHAUDERN", "", "", ["erschauderst", "erschaudert", "erschaudert", "erschaudern"]),
    Verb_alt.new("VERB_VORBEIZISCHEN", "", "vorbei", ["zischst", "zischt", "zischt", "zischen"]),
    Verb_alt.new("VERB_SITZEN", "", "", ["sitzt", "sitzt", "sitzt", "sitzen"]),
    Verb_alt.new("VERB_SETZEN", "", "", ["setzt", "setzt", "setzt", "setzen"]),
    Verb_alt.new("VERB_AUFLOESEN", "", "auf", ["l�st", "l�st", "l�st", "l�sen"]),
    Verb_alt.new("VERB_WOLLEN", "", "", ["willst", "will", "wollt", "wollen"]),
    "",
    Verb_alt.new("VERB_DACHTEST", "", "", ["dachtest", "dachte", "dachtet","dachten"]),
    Verb_alt.new("VERB_HAETTEST", "", "", ["h�ttest", "h�tte", "h�ttet","h�tten"]),
    Verb_alt.new("VERB_MOECHTEST", "", "", ["m�chtest", "m�chte", "m�chtet","m�chten"]),
    Verb_alt.new("VERB_LASSEN", "", "", ["l�sst", "l�sst", "lassen","lasst"]),
    "",
    Verb_alt.new("VERB_WORN", "getragen", "", ["", "", "", ""]),
    Verb_alt.new("VERB_ZERBRECHEN", "", "", ["zerbrichst", "zerbricht", "zerbrecht", "zerbrechen"]),
    Verb_alt.new("VERB_VERSTEINERN", "", "", ["versteinerst", "versteinert", "versteinert", "versteinern"]),
    Verb_alt.new("VERB_STERBEN", "", "", ["stirbst", "stirbt", "sterbt", "sterben"]),
    Verb_alt.new("VERB_VERHUNGERN", "", "", ["verhungerst", "verhungert", "verhungert", "verhungern"]),
    Verb_alt.new("VERB_FALLEN", "", "", ["f�llst", "f�llt", "fallt", "fallen"]),
    Verb_alt.new("VERB_TRAELLERN", "", "", ["tr�llerst", "tr�llert", "tr�llert", "tr�llern"]),
    Verb_alt.new("VERB_KLIMPERN", "", "", ["klimperst", "klimpert", "klimpert", "klimpern"]),
    Verb_alt.new("VERB_ZUHOEREN", "", "zu", ["h�rst", "h�rt", "h�rt","h�ren"]),
    Verb_alt.new("VERB_HERANMACHEN", "", "heran", ["machst", "macht", "macht","machen"]),
    Verb_alt.new("VERB_LAUFEN", "", "", ["l�ufst", "l�uft", "lauft","laufen"]),
    Verb_alt.new("VERB_KNISTERN", "", "", ["knisterst", "knistert", "knistert","knistern"]),
    Verb_alt.new("VERB_AUFHOEREN", "", "auf", ["h�rst", "h�rt", "h�rt","h�ren"]),
    Verb_alt.new("VERB_VERHASPELN", "", "", ["verhaspelst", "verhaspelt","verhaspelt","verhaspeln"]),
    Verb_alt.new("VERB_AUFHOEREN", "", "auf", ["hebst", "hebt", "hebt","heben"]),
    Verb_alt.new("VERB_GRABEN", "", "", ["gr�bst", "gr�bt", "grabt","graben"]),
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
		Verb.verb("VERB_CAN","k�nnen"),
		Verb.verb("VERB_SMELL","riechen"),
# Modalverben
		Verb.verb("VERB_KOENNEN","k�nnen"),
		Verb.verb("VERB_WOLLEN","wollen"),
# sonstige Verben
		Verb.verb("VERB_HEAR","h�ren"),
		Verb.verb("VERB_READ","lesen"),
		Verb.verb("VERB_HOEREN", "h�ren"),
		Verb.verb("VERB_ZUHOEREN","h�ren", "zu"),
		Verb.verb("VERB_AUFHOEREN","h�ren", "auf"),
		Verb.verb("VERB_EAT","essen"),
		Verb.verb("VERB_OPEN","gehen", "auf"),
		Verb.verb("VERB_RUTSCHEN","rutschen"),
		Verb.verb("VERB_GLEITEN","gleiten"),
		Verb.verb("VERB_BITE","bei�en"),
		Verb.verb("VERB_KICK","treten"),
		Verb.verb("VERB_KILL","t�ten"),
		Verb.verb("VERB_MISS","verfehlen"),
		Verb.verb("VERB_HIT","treffen"),
		Verb.verb("VERB_SEE","sehen"),
		Verb.verb("VERB_TOUCH","ber�hren"),
		Verb.verb("VERB_DROP","lassen", "fallen"),
		Verb.verb("VERB_LASSEN","lassen"),
		Verb.verb("VERB_OBJECT_DROPS","fallen"),
		Verb.verb("VERB_FALLEN","fallen"),
		Verb.verb("VERB_DENKEN","denken"),
		Verb.verb("VERB_LANDEN","landen"),
		Verb.verb("VERB_SEEM","scheinen", "zu sein"),
		Verb.verb("VERB_SCHEINEN","scheinen"),
		Verb.verb("VERB_YELL","br�llen"),
		Verb.verb("VERB_SLIP","entgleiten"),
		Verb.verb("VERB_STAGGER","torkeln"),
		Verb.verb("VERB_DIP","tauchen","ein"),
		Verb.verb("VERB_CANCEL", "l�schen"), # canceln, annullieren, entwerten, streichen

		# deutsche Bezeichner
		Verb.verb("VERB_ABBRECHEN","brechen","ab"),
		Verb.verb("VERB_ABGLEITEN","gleiten","ab"),
		Verb.verb("VERB_ABLEHNEN", "lehnen", "ab"),
		Verb.verb("VERB_ABPRALLEN", "prallen", "ab"),
		Verb.verb("VERB_ABRUTSCHEN","rutschen", "ab"),
		Verb.verb("VERB_ABSONDERN","sondern", "ab"),
		Verb.verb("VERB_ABSORBIEREN","absorbieren"),
		Verb.verb("VERB_AKTIVIEREN","aktivieren"),
		Verb.verb("VERB_AKZEPTIEREN","akzeptieren"),
		Verb.verb("VERB_ANFUEHLEN","f�hlen", "an"),
		Verb.verb("VERB_ANGELN","angeln"),
		Verb.verb("VERB_ANGREIFEN", "greifen", "an"),
		Verb.verb("VERB_ANKAEMPFEN","k�mpfen","an"),
		Verb.verb("VERB_ANLAECHELN", "l�cheln", "an"),
		Verb.verb("VERB_ANLOCKEN","locken", "an"),
		Verb.verb("VERB_ANPEILEN", "peilen", "an"),
		Verb.verb("VERB_ANPEITSCHEN","peitschen","an"),
		Verb.verb("VERB_ANTWORTEN","antworten"),
		Verb.verb("VERB_ANZIEHEN","ziehen","an"),
		Verb.verb("VERB_AUFBRECHEN","brechen","auf"),
		Verb.verb("VERB_AUFERSTEHEN", "auferstehen"),
		Verb.verb("VERB_AUFGEHEN","gehen","auf"),
		Verb.verb("VERB_AUFHEBEN", "heben", "auf"),
		Verb.verb("VERB_AUFLEUCHTEN", "leuchten","auf"),
		Verb.verb("VERB_AUFLOESEN","l�sen", "auf"),
		Verb.verb("VERB_AUFSCHLIESSEN", "schlie�en", "auf"),
		Verb.verb("VERB_AUFSCHRECKEN","schrecken","auf"),
		Verb.verb("VERB_AUFTAUCHEN","tauchen","auf"),
		Verb.verb("VERB_AUFWACHEN", "wachen", "auf"),
		Verb.verb("VERB_AUSGEHEN", "gehen", "aus"),
		Verb.verb("VERB_AUSGLEITEN","gleiten","aus"),
		Verb.verb("VERB_AUSGRABEN","graben","aus"),
		Verb.verb("VERB_AUSHAENDIGEN","h�ndigen","aus"),
		Verb.verb("VERB_AUSLOESCHEN", "l�schen", "aus"),
		Verb.verb("VERB_AUSLOESEN", "l�sen", "aus"),
		Verb.verb("VERB_AUSRUTSCHEN","rutschen", "aus"),
		Verb.verb("VERB_AUSSEHEN","sehen", "aus"),
		Verb.verb("VERB_AUSSPRECHEN","sprechen","aus"),
		Verb.verb("VERB_AUSSPUCKEN","spucken","aus"),
		Verb.verb("VERB_AUSSTRECKEN","strecken","aus"),
		Verb.verb("VERB_AUSWEICHEN","weichen","aus"),
		Verb.verb("VERB_AUSZIEHEN","ziehen","aus"),
		Verb.verb("VERB_BEARBEITEN","bearbeiten"),
		Verb.verb("VERB_BEENDEN","beenden"),
		Verb.verb("VERB_BEFESTIGEN","befestigen"),
		Verb.verb("VERB_BEFINDEN","befinden"),
		Verb.verb("VERB_BEFREIEN","befreien"),
		Verb.verb("VERB_BEGEGNEN", "begegnen"),
		Verb.verb("VERB_BEGEHREN", "begehren"),
		Verb.verb("VERB_BEGINNEN","beginnen"),
		Verb.verb("VERB_BEGREIFEN","begreifen"),
		Verb.verb("VERB_BEHERRSCHEN","beherrschen"),
		Verb.verb("VERB_BEHINDERN","behindern"),
		Verb.verb("VERB_BEISEITESCHIEBEN","schieben","beiseite"),
		Verb.verb("VERB_BEISSEN","bei�en"),
		Verb.verb("VERB_BEKLAGEN","beklagen"),
		Verb.verb("VERB_BEKOMMEN","bekommen"),
		Verb.verb("VERB_BELEIDIGEN","beleidigen"),
		Verb.verb("VERB_BELOHNEN","belohnen"),
		Verb.verb("VERB_BEMERKEN","bemerken"),
		Verb.verb("VERB_BENENNEN","benennen"),
		Verb.verb("VERB_BENUTZEN", "benutzen"),
		Verb.verb("VERB_BEOBACHTEN", "beobachten"),
		Verb.verb("VERB_BERUEHREN","ber�hren"),
		Verb.verb("VERB_BERUHIGEN", "beruhigen"),
		Verb.verb("VERB_BESCHAEDIGEN","besch�digen"),
		Verb.verb("VERB_BESCHREIBEN","beschreiben"),
		Verb.verb("VERB_BESCHWATZEN","beschwatzen"),
		Verb.verb("VERB_BESCHWEREN","beschweren"),
		Verb.verb("VERB_BESCHWOEREN","beschw�ren"),
		Verb.verb("VERB_BESITZEN","besitzen"),
		Verb.verb("VERB_BESTRAHLEN","bestrahlen"),
		Verb.verb("VERB_BETAEUBEN","bet�uben"),
		Verb.verb("VERB_BETEN", "beten"),
		Verb.verb("VERB_BETRACHTEN","betrachten"),
		Verb.verb("VERB_BETRETEN","betreten"),
		Verb.verb("VERB_BEWAFFNEN", "bewaffnen"),
		Verb.verb("VERB_BEWEGEN", "bewegen"),
		Verb.verb("VERB_BEWUNDERN","bewundern"),
		Verb.verb("VERB_BEZAHLEN","bezahlen"),
		Verb.verb("VERB_BEZIRZEN","bezirzen"),
		Verb.verb("VERB_BIETEN","bieten"),
		Verb.verb("VERB_BILDEN","bilden"),
		Verb.verb("VERB_BIMMELN","bimmeln"),
		Verb.verb("VERB_BITTEN","bitten"),
		Verb.verb("VERB_BLAETTERN","bl�ttern"),
		Verb.verb("VERB_BLEIBEN","bleiben"),
		Verb.verb("VERB_BLENDEN","blenden"),
		Verb.verb("VERB_BLICKEN", "blicken"),
		Verb.verb("VERB_BLOCKEN","blocken"),
		Verb.verb("VERB_BLOCKIEREN","blockieren"),
		Verb.verb("VERB_BOHREN","bohren"),
		Verb.verb("VERB_BRAUCHEN","brauchen"),
		Verb.verb("VERB_BRECHEN","brechen"),
		Verb.verb("VERB_BRENNEN","brennen"),
		Verb.verb("VERB_BRINGEN","bringen"),
		Verb.verb("VERB_BRODELN", "brodeln"),
		Verb.verb("VERB_BRUELLEN","br�llen"),
		Verb.verb("VERB_BRUTZELN","brutzeln"),
		Verb.verb("VERB_CHILLEN","chillen"),
		Verb.verb("VERB_DAHINSIECHEN","siechen","dahin"),
		Verb.verb("VERB_DAMPFEN","dampfen"),
		Verb.verb("VERB_DEMATERIALISIEREN","dematerialisieren"),
		Verb.verb("VERB_DESINTEGRIEREN","desintegrieren"),
		Verb.verb("VERB_DEUTEN", "deuten"),
		Verb.verb("VERB_DONNERN","donnern"),
		Verb.verb("VERB_DREHEN", "drehen"),
		Verb.verb("VERB_DRUECKEN","dr�cken"),
		Verb.verb("VERB_DUERFEN","d�rfen"),
		Verb.verb("VERB_DURCHBOHREN","durchbohren"),
		Verb.verb("VERB_DURCHDRINGEN","durchdringen"),
		Verb.verb("VERB_DURCHLEBEN", "durchleben"),
		Verb.verb("VERB_DURCHSTECHEN","durchstechen"),
		Verb.verb("VERB_EIERN","eiern"),
		Verb.verb("VERB_EINATMEN", "atmen", "ein"),
		Verb.verb("VERB_EINBRENNEN","brennen","ein"),
		Verb.verb("VERB_EINDOSEN","dosen","ein"),
		Verb.verb("VERB_EINGEBEN","geben","ein"),
		Verb.verb("VERB_EINRITZEN","ritzen","ein"),
		Verb.verb("VERB_EINSCHLAFEN","schlafen","ein"),
		Verb.verb("VERB_EINSCHLAGEN", "schlagen", "ein"),
		Verb.verb("VERB_EINSCHMELZEN","schmelzen","ein"),
		Verb.verb("VERB_EINSETZEN","setzen","ein"),
		Verb.verb("VERB_EINTRETEN","treten","ein"),
		Verb.verb("VERB_EMPFANGEN", "empfangen"),
		Verb.verb("VERB_EMPORSTEIGEN","steigen","empor"),
		Verb.verb("VERB_ENTDECKEN", "entdecken"),
		Verb.verb("VERB_ENTFERNEN","entfernen"),
		Verb.verb("VERB_ENTFESSELN","entfesseln"),
		Verb.verb("VERB_ENTGEGENEXPLODIEREN","explodierem","entgegen"),
		Verb.verb("VERB_ENTGEHEN","entgehen"),
		Verb.verb("VERB_ENTGLEITEN","entgleiten"),
		Verb.verb("VERB_ENTHAUPTEN","enthaupten"),
		Verb.verb("VERB_ENTLOCKEN", "entlocken"),
		Verb.verb("VERB_ENTSCHAEDIGEN","entsch�digen"),
		Verb.verb("VERB_ENTSORGEN", "entsorgen"),
		Verb.verb("VERB_ENTWEICHEN", "entweichen"),
		Verb.verb("VERB_ENTWINDEN","entwinden"),
		Verb.verb("VERB_ENTZIEHEN","entziehen"),
		Verb.verb("VERB_ENTZUENDEN","entz�nden"),
		Verb.verb("VERB_ERAHNEN","erahnen"),
		Verb.verb("VERB_ERFASSEN","erfassen"),
		Verb.verb("VERB_ERGEBEN","ergeben"),
		Verb.verb("VERB_ERGREIFEN","ergreifen"),
		Verb.verb("VERB_ERHALTEN","erhalten"),
		Verb.verb("VERB_ERHEBEN","erheben"),
		Verb.verb("VERB_ERHITZEN","erhitzen"),
		Verb.verb("VERB_ERINNERN","erinnern"),
		Verb.verb("VERB_ERLANGEN","erlangen"),
		Verb.verb("VERB_ERLOESCHEN","erl�schen"),
		Verb.verb("VERB_ERREICHEN","erreichen"),
		Verb.verb("VERB_ERSCHAUDERN", "erschaudern"),
		Verb.verb("VERB_ERSCHEINEN","erscheinen"),
		Verb.verb("VERB_ERSCHRECKEN","erschrecken"),
		Verb.verb("VERB_ERSETZEN","ersetzen"),
		Verb.verb("VERB_ERSTARREN", "erstarren"),
		Verb.verb("VERB_ERSTAUNEN","erstaunen"),
		Verb.verb("VERB_ERSTICKEN", "ersticken"),
		Verb.verb("VERB_ERSTRAHLEN","erstrahlen"),
		Verb.verb("VERB_ERTASTEN","ertasten"),
		Verb.verb("VERB_ERTRAENKEN", "ertr�nken"),
		Verb.verb("VERB_ERTRAGEN","ertragen"),
		Verb.verb("VERB_ERTRINKEN", "ertrinken"),
		Verb.verb("VERB_ERWACHEN","erwachen"),
		Verb.verb("VERB_ERWAERMEN","erw�rmen"),
		Verb.verb("VERB_ERWECKEN","erwecken"),
		Verb.verb("VERB_ERWIDERN","erwidern"),
		Verb.verb("VERB_ERWUERGEN","erw�rgen"),
		Verb.verb("VERB_ERZEUGEN", "erzeugen"),
		Verb.verb("VERB_ESSEN","essen"),
		Verb.verb("VERB_EXHUMIEREN","exhumieren"),
		Verb.verb("VERB_EXPLODIEREN", "explodieren"),
		Verb.verb("VERB_FABRIZIEREN","fabrizieren"),
		Verb.verb("VERB_FAELLEN","f�llen"),
		Verb.verb("VERB_FAHREN","fahren"),
		Verb.verb("VERB_FANGEN", "fangen"),
		Verb.verb("VERB_FEHLEN","fehlen"),
		Verb.verb("VERB_FESTHALTEN","halten","fest"),
		Verb.verb("VERB_FESTSAUGEN","saugen","fest"),
		Verb.verb("VERB_FINDEN","finden"),
		Verb.verb("VERB_FLACKERN","flackern"),
		Verb.verb("VERB_FLATTERN","flattern"),
		Verb.verb("VERB_FLEUCHEN", "fleuchen"),
		Verb.verb("VERB_FLIEGEN","fliegen"),
		Verb.verb("VERB_FLIEHEN","fliehen"),
		Verb.verb("VERB_FLIESSEN", "flie�en"),
		Verb.verb("VERB_FLIPPEN","flippen"),
		Verb.verb("VERB_FLUCHEN", "fluchen"),
		Verb.verb("VERB_FLUESTERN","fl�stern"),
		Verb.verb("VERB_FOLGEN","folgen"),
		Verb.verb("VERB_FOTOGRAFIEREN","fotografieren"),
		Verb.verb("VERB_FRAGEN", "fragen"),
		Verb.verb("VERB_FRESSEN","fressen"),
		Verb.verb("VERB_FRIEREN","frieren"),
		Verb.verb("VERB_FROESTELN","fr�steln"),
		Verb.verb("VERB_FUEGEN","f�gen"),
		Verb.verb("VERB_FUEHLEN", "f�hlen"),
		Verb.verb("VERB_FUEHREN","f�hren"),
		Verb.verb("VERB_FUELLEN", "f�llen"),
		Verb.verb("VERB_FUNKELN","funkeln"),
		Verb.verb("VERB_GAEHNEN", "g�hnen"),
		Verb.verb("VERB_GALOPPIEREN", "galoppieren"),
		Verb.verb("VERB_GAZE","starren"),
		Verb.verb("VERB_GEBEN", "geben"),
		Verb.verb("VERB_GEFRIEREN","gefrieren"),
		Verb.verb("VERB_GEHEN","gehen"),
		Verb.verb("VERB_GELINGEN","gelingen"),
		Verb.verb("VERB_GEWAEHREN","gew�hren"),
		Verb.verb("VERB_GEWINNEN","gewinnen"),
		Verb.verb("VERB_GIGGELN","giggeln"),
		Verb.verb("VERB_GLAENZEN", "gl�nzen"),
		Verb.verb("VERB_GLAUBEN","glauben"),
		Verb.verb("VERB_GLUCKSEN","glucksen"),
		Verb.verb("VERB_GRABEN","graben"),
		Verb.verb("VERB_GRAUEN", "grauen"),
		Verb.verb("VERB_GREIFEN","greifen"),
		Verb.verb("VERB_HAENGEN","h�ngen"),
		Verb.verb("VERB_HALTEN","halten"),
		Verb.verb("VERB_HAMPELN","hampeln"),
		Verb.verb("VERB_HAUEN","hauen"),
		Verb.verb("VERB_HEBEN","heben"),
		Verb.verb("VERB_HECHTEN","hechten"),
		Verb.verb("VERB_HELFEN","helfen"),
		Verb.verb("VERB_HEULEN","heulen"),
		Verb.verb("VERB_HINAUFSTEIGEN","steigen", "hinauf"),
		Verb.verb("VERB_HINDERN","hindern"),
		Verb.verb("VERB_HINDERN","hindern"),
		Verb.verb("VERB_HINTERLASSEN","hinterlassen"),
		Verb.verb("VERB_HINZUFUEGEN","f�gen","hinzu"),
		Verb.verb("VERB_HOCHHEBEN", "heben", "hoch"),
		Verb.verb("VERB_HOLEN","holen"),
		Verb.verb("VERB_HUEPFEN","h�pfen"),
		Verb.verb("VERB_HUSTEN","husten"),
		Verb.verb("VERB_IDENTIFIZIEREN","identifizieren"),
		Verb.verb("VERB_IDENTIFY","identifizieren"),
		Verb.verb("VERB_IGNORIEREN","ignorieren"),
		Verb.verb("VERB_IMITIEREN", "imitieren"),
		Verb.verb("VERB_INTERESSIEREN","interessieren"),
		Verb.verb("VERB_JIEPEN","jiepen"),
		Verb.verb("VERB_JOHLEN","johlen"),
		Verb.verb("VERB_JONGLIEREN","jonglieren"),
		Verb.verb("VERB_JUCKEN","jucken"),
		Verb.verb("VERB_KAEMPFEN","k�mpfen"),
		Verb.verb("VERB_KEHREN","kehren"),
		Verb.verb("VERB_KENNEN","kennen"),
		Verb.verb("VERB_KICHERN","kichern"),
		Verb.verb("VERB_KITZELN","kitzeln"),
		Verb.verb("VERB_KLAEREN","kl�ren"),
		Verb.verb("VERB_KLAPPERN","klappern"),
		Verb.verb("VERB_KLATSCHEN","klatschen"),
		Verb.verb("VERB_KLEBEN","kleben"),
		Verb.verb("VERB_KLETTERN","klettern"),
		Verb.verb("VERB_KLIMPERN", "klimpern"),
		Verb.verb("VERB_KNACKEN","knacken"),
		Verb.verb("VERB_KNALLEN","knallen"),
		Verb.verb("VERB_KNARREN","knarren"),
		Verb.verb("VERB_KNISTERN", "knistern"),
		Verb.verb("VERB_KOEPFEN","k�pfen"),
		Verb.verb("VERB_KOKELN","kokeln"),
		Verb.verb("VERB_KOLLIDIEREN","kollidieren"),
		Verb.verb("VERB_KOMMEN", "kommen"),
		Verb.verb("VERB_KONZENTRIEREN","konzentrieren"),
		Verb.verb("VERB_KORRODIEREN","korrodieren"),
		Verb.verb("VERB_KOSTEN","kosten"),
		Verb.verb("VERB_KOTZEN","kotzen"),
		Verb.verb("VERB_KRACHEN","krachen"),
		Verb.verb("VERB_KRAKELN","krakeln"),
		Verb.verb("VERB_KRIECHEN","kriechen"),
		Verb.verb("VERB_KRIEGEN","kriegen"),
		Verb.verb("VERB_KRITZELN","kritzeln"),
		Verb.verb("VERB_KUEHLEN","k�hlen"),
		Verb.verb("VERB_LACHEN","lachen"),
		Verb.verb("VERB_LAECHELN", "l�cheln"),
		Verb.verb("VERB_LAEUTEN","l�uten"),
		Verb.verb("VERB_LAUFEN", "laufen"),
		Verb.verb("VERB_LEGEN", "legen"),
		Verb.verb("VERB_LEHNEN","lehnen"),
		Verb.verb("VERB_LERNEN","lernen"),
		Verb.verb("VERB_LEUCHTEN", "leuchten"),
		Verb.verb("VERB_LICHTEN","lichten"),
		Verb.verb("VERB_LIEGENBLEIBEN","bleiben","liegen"),
		Verb.verb("VERB_LIEGEN", "liegen"),
		Verb.verb("VERB_LOESCHEN", "l�schen"),
		Verb.verb("VERB_LOESEN", "l�sen"),
		Verb.verb("VERB_LOSREISSEN","rei�en","los"),
		Verb.verb("VERB_MAMPFEN","mampfen"),
		Verb.verb("VERB_MEDITIEREN","meditieren"),
		Verb.verb("VERB_MEIDEN","meiden"),
		Verb.verb("VERB_MEINEN","meinen"),
		Verb.verb("VERB_MERKEN", "merken"),
		Verb.verb("VERB_MISSVERSTEHEN", "missverstehen"),
		Verb.verb("VERB_MOEGEN","m�gen"),
		Verb.verb("VERB_MUESSEN","m�ssen"),
		Verb.verb("VERB_MUTIEREN", "mutieren"),
		Verb.verb("VERB_NEHMEN","nehmen"),
		Verb.verb("VERB_NEHMEN","nehmen"),
		Verb.verb("VERB_OEFFNEN", "�ffnen"),
		Verb.verb("VERB_ORIENTIEREN","orientieren"),
		Verb.verb("VERB_PACKEN", "packen"),
		Verb.verb("VERB_PASSEN", "passen"),
		Verb.verb("VERB_PFLUEGEN","pfl�gen"),
		Verb.verb("VERB_PIEKSEN","pieksen"),
		Verb.verb("VERB_PIEPSEN","piepsen"),
		Verb.verb("VERB_PINKELN","pinkeln"),
		Verb.verb("VERB_PLATZEN","platzen"),
		Verb.verb("VERB_PLATZIEREN","platzieren"),
		Verb.verb("VERB_PRAHLEN","prahlen"),
		Verb.verb("VERB_PREDIGEN","predigen"),
		Verb.verb("VERB_PROVOZIEREN","provozieren"),
		Verb.verb("VERB_PULSATE", "pulsieren"),
		Verb.verb("VERB_PULSIEREN", "pulsieren"),
		Verb.verb("VERB_PURZELN", "purzeln"),
		Verb.verb("VERB_PUTZEN", "putzen"),
		Verb.verb("VERB_QUELLEN","quellen"),
		Verb.verb("VERB_RAMMEN","rammen"),
		Verb.verb("VERB_RASSELN","rasseln"),
		Verb.verb("VERB_REAGIEREN", "reagieren"),
		Verb.verb("VERB_REFLEKTIEREN", "reflektieren"),
		Verb.verb("VERB_REGURGITATE","w�rgen","heraus"),
		Verb.verb("VERB_REISSEN","rei�en"),
		Verb.verb("VERB_REITEN","reiten"),
		Verb.verb("VERB_REPARIEREN","reparieren"),
		Verb.verb("VERB_RIECHEN","riechen"),
		Verb.verb("VERB_ROSTEN", "rosten"),
		Verb.verb("VERB_RUEHREN","r�hren"),
		Verb.verb("VERB_RUELPSEN", "r�lpsen"),
		Verb.verb("VERB_RUFEN","rufen"),
		Verb.verb("VERB_RUMOREN","rumoren"),
		Verb.verb("VERB_SAEUBERN","s�ubern"),
		Verb.verb("VERB_SAGEN","sagen"),
		Verb.verb("VERB_SATTELN", "satteln"),
		Verb.verb("VERB_SAUGEN","saugen"),
		Verb.verb("VERB_SCHAEDIGEN", "sch�digen"),
		Verb.verb("VERB_SCHAFFEN", "schaffen"),
		Verb.verb("VERB_SCHAUEN","schauen"),
		Verb.verb("VERB_SCHIEBEN","schieben"),
		Verb.verb("VERB_SCHIESSEN", "schie�en"),
		Verb.verb("VERB_SCHIMMERN","schimmern"),
		Verb.verb("VERB_SCHLAGEN","schlagen"),
		Verb.verb("VERB_SCHLEUDERN","schleudern"),
		Verb.verb("VERB_SCHLIESSEN", "schlie�en"),
		Verb.verb("VERB_SCHLITZEN", "schlitzen"),
		Verb.verb("VERB_SCHLUCKEN", "schlucken"),
		Verb.verb("VERB_SCHLUEPFEN","schl�pfen"),
		Verb.verb("VERB_SCHMATZEN","schmatzen"),
		Verb.verb("VERB_SCHMECKEN","schmecken"),
		Verb.verb("VERB_SCHMEISSEN","schmeissen"),
		Verb.verb("VERB_SCHMELZEN","schmelzen"),
		Verb.verb("VERB_SCHMERZEN","schmerzen"),
		Verb.verb("VERB_SCHMIEREN","schmieren"),
		Verb.verb("VERB_SCHNAPPEN", "schnappen"),
		Verb.verb("VERB_SCHNEIDEN","schneiden"),
		Verb.verb("VERB_SCHNIPPEN","schnippen"),
		Verb.verb("VERB_SCHNUEFFELN","schn�ffeln"),
		Verb.verb("VERB_SCHREIBEN","schreiben"),
		Verb.verb("VERB_SCHREIEN","schreien"),
		Verb.verb("VERB_SCHRUMPFEN","schrumpfen"),
		Verb.verb("VERB_SCHUETZEN", "sch�tzen"),
		Verb.verb("VERB_SCHWANKEN","schwanken"),
		Verb.verb("VERB_SCHWEBEN","schweben"),
		Verb.verb("VERB_SCHWINDEN","schwinden"),
		Verb.verb("VERB_SCHWINGEN","schwingen"),
		Verb.verb("VERB_SCHWITZEN","schwitzen"),
		Verb.verb("VERB_SEDUCE","verf�hren"),
		Verb.verb("VERB_SEHEN","sehen"),
		Verb.verb("VERB_SETZEN","setzen"),
		Verb.verb("VERB_SEUFZEN","seufzen"),
		Verb.verb("VERB_SICH_FUEHLEN","f�hlen"),
		Verb.verb("VERB_SIEDEN","sieden"),
		Verb.verb("VERB_SINGEN","singen"),
		Verb.verb("VERB_SINKEN","sinken"),
		Verb.verb("VERB_SITZEN","sitzen"),
		Verb.verb("VERB_SOLLEN","sollen"),
		Verb.verb("VERB_SONDIEREN","sondieren"),
		Verb.verb("VERB_SONNEN", "sonnen"),
		Verb.verb("VERB_SPAZIEREN","spazieren"),
		Verb.verb("VERB_SPEIEN","speien"),
		Verb.verb("VERB_SPIELEN","spielen"),
		Verb.verb("VERB_SPINNEN","spinnen"),
		Verb.verb("VERB_SPRECHEN","sprechen"),
		Verb.verb("VERB_SPRENGEN","sprengen"),
		Verb.verb("VERB_SPRINGEN","springen"),
		Verb.verb("VERB_SPRITZEN","spritzen"),
		Verb.verb("VERB_SPUCKEN","spucken"),
		Verb.verb("VERB_SPUEREN","sp�ren"),
		Verb.verb("VERB_STABILISIEREN","stabilisieren"),
		Verb.verb("VERB_STARREN","starren"),
		Verb.verb("VERB_STECHEN","stechen"),
		Verb.verb("VERB_STECKEN","stecken"),
		Verb.verb("VERB_STEHEN","stehen"),
		Verb.verb("VERB_STEHLEN","stehlen"),
		Verb.verb("VERB_STEIGEN","steigen"),
		Verb.verb("VERB_STELLEN","stellen"),
		Verb.verb("VERB_STERBEN","sterben"),
		Verb.verb("VERB_STOEREN","st�ren"),
		Verb.verb("VERB_STOLPERN","stolpern"),
		Verb.verb("VERB_STOPFEN","stopfen"),
		Verb.verb("VERB_STOPPEN","stoppen"),
		Verb.verb("VERB_STOSSEN","sto�en"),
		Verb.verb("VERB_STRAMPELN","strampeln"),
		Verb.verb("VERB_STRAUCHELN","straucheln"),
		Verb.verb("VERB_STRECKEN","strecken"),
		Verb.verb("VERB_STREIFEN","streifen"),
		Verb.verb("VERB_STROEMEN","str�men"),
		Verb.verb("VERB_STUERZEN","st�rzen"),
		Verb.verb("VERB_STUPSEN","stupsen"),
		Verb.verb("VERB_SUCHEN","suchen"),
		Verb.verb("VERB_SUPPEN","suppen"),
		Verb.verb("VERB_TAEUSCHEN","t�uschen"),
		Verb.verb("VERB_TAUCHEN","tauchen"),
		Verb.verb("VERB_TAUSCHEN","tauschen"),
		Verb.verb("VERB_TAUMELN","taumeln"),
		Verb.verb("VERB_TEILEN","teilen"),
		Verb.verb("VERB_TELEPORTIEREN","teleportieren"),
		Verb.verb("VERB_TIPPEN","tippen"),
		Verb.verb("VERB_TJOSTIEREN","tjostieren"),
		Verb.verb("VERB_TRAELLERN","tr�llern"),
		Verb.verb("VERB_TRAENEN","tr�nen"),
		Verb.verb("VERB_TRAEUMEN","tr�umen"),
		Verb.verb("VERB_TRAGEN","tragen"),
		Verb.verb("VERB_TRANSFORMIEREN","transformieren"),
		Verb.verb("VERB_TREFFEN","treffen"),
		Verb.verb("VERB_TRETEN","treten"),
		Verb.verb("VERB_TRILLERN","trillern"),
		Verb.verb("VERB_TRINKEN","trinken"),
		Verb.verb("VERB_TROPFEN","tropfen"),
		Verb.verb("VERB_TUN","tun"),
		Verb.verb("VERB_TUTEN","tuten"),
		Verb.verb("VERB_UEBERFRESSEN","�berfressen"),
		Verb.verb("VERB_UEBERGEBEN","�bergeben"),
		Verb.verb("VERB_UEBERLEGEN","�berlegen"),
		Verb.verb("VERB_UEBERMANNEN","�bermannen"),
		Verb.verb("VERB_UEBERRASCHEN","�berraschen"),
		Verb.verb("VERB_UEBERSCHLAGEN","�berschlagen"),
		Verb.verb("VERB_UEBERSCHREIBEN","�berschreiben"),
		Verb.verb("VERB_UEBERSTEHEN","�berstehen"),
		Verb.verb("VERB_UEBERZIEHEN","�berziehen"),
		Verb.verb("VERB_UMGEBEN", "umgeben"),
		Verb.verb("VERB_UMSCHLINGEN","umschlingen"),
		Verb.verb("VERB_VERAETZEN","ver�tzen"),
		Verb.verb("VERB_VERBLASSEN","verblassen"),
		Verb.verb("VERB_VERBREITEN","verbreiten"),
		Verb.verb("VERB_VERBRENNEN","verbrennen"),
		Verb.verb("VERB_VERBRUTZELN","verbrutzeln"),
		Verb.verb("VERB_VERDAMPFEN","verdampfen"),
		Verb.verb("VERB_VERDAUEN","verdauen"),
		Verb.verb("VERB_VERDIENEN","verdienen"),
		Verb.verb("VERB_VERDUENNEN","verd�nnen"),
		Verb.verb("VERB_VEREINIGEN","vereinigen"),
		Verb.verb("VERB_VEREISEN","vereisen"),
		Verb.verb("VERB_VERFANGEN","verfangen"),
		Verb.verb("VERB_VERFAULEN","verfaulen"),
		Verb.verb("VERB_VERFLUCHEN","verfluchen"),
		Verb.verb("VERB_VERFLUECHTIGEN","verfl�chtigen"),
		Verb.verb("VERB_VERGEHEN","vergehen"),
		Verb.verb("VERB_VERHAERTEN","verh�rten"),
		Verb.verb("VERB_VERHASPELN","verhaspeln"),
		Verb.verb("VERB_VERHEDDERN","verheddern"),
		Verb.verb("VERB_VERHUNGERN","verhungern"),
		Verb.verb("VERB_VERINNERLICHEN","verinnerlichen"),
		Verb.verb("VERB_VERKOHLEN","verkohlen"),
		Verb.verb("VERB_VERKUENDEN","verk�nden"),
		Verb.verb("VERB_VERLANGEN","verlangen"),
		Verb.verb("VERB_VERLASSEN","verlassen"),
		Verb.verb("VERB_VERLETZEN","verletzen"),
		Verb.verb("VERB_VERLIEREN","verlieren"),
		Verb.verb("VERB_VERMEHREN","vermehren"),
		Verb.verb("VERB_VERMISCHEN","vermischen"),
		Verb.verb("VERB_VERMODERN","vermodern"),
		Verb.verb("VERB_VERPASSEN","verpassen"),
		Verb.verb("VERB_VERROSTEN","verrosten"),
		Verb.verb("VERB_VERSAUEN","versauen"),
		Verb.verb("VERB_VERSCHIESSEN","verschiessen"),
		Verb.verb("VERB_VERSCHLEIMEN", "verschleimen"),
		Verb.verb("VERB_VERSCHLIESSEN", "verschlie�en"),
		Verb.verb("VERB_VERSCHLINGEN","verschlingen"),
		Verb.verb("VERB_VERSCHLUCKEN","verschlucken"),
		Verb.verb("VERB_VERSCHMELZEN","verschmelzen"),
		Verb.verb("VERB_VERSCHMIEREN","verschmieren"),
		Verb.verb("VERB_VERSCHRECKEN","verschrecken"),
		Verb.verb("VERB_VERSCHUETTEN","versch�tten"),
		Verb.verb("VERB_VERSCHWINDEN","verschwinden"),
		Verb.verb("VERB_VERSENGEN","versengen"),
		Verb.verb("VERB_VERSETZEN","versetzen"),
		Verb.verb("VERB_VERSINKEN","versinken"),
		Verb.verb("VERB_VERSPERREN","versperren"),
		Verb.verb("VERB_VERSPUEREN","versp�ren"),
		Verb.verb("VERB_VERSTAERKEN","verst�rken"),
		Verb.verb("VERB_VERSTECKEN","verstecken"),
		Verb.verb("VERB_VERSTEHEN","verstehen"),
		Verb.verb("VERB_VERSTEIFEN","versteifen"),
		Verb.verb("VERB_VERSTEINERN","versteinern"),
		Verb.verb("VERB_VERSTOPFEN","verstopfen"),
		Verb.verb("VERB_VERSTROEMEN","verstr�men"),
		Verb.verb("VERB_VERSUCHEN","versuchen"),
		Verb.verb("VERB_VERURSACHEN","verursachen"),
		Verb.verb("VERB_VERWANDELN","verwandeln"),
		Verb.verb("VERB_VERWIRREN","verwirren"),
		Verb.verb("VERB_VERZICHTEN","verzichten"),
		Verb.verb("VERB_VIBRIEREN","vibrieren"),
		Verb.verb("VERB_VISUALISIEREN","visualisieren"),
		Verb.verb("VERB_VORBEIZISCHEN","zischen", "vorbei"),
		Verb.verb("VERB_WACHEN","wachen"),
		Verb.verb("VERB_WACKELN","wackeln"),
		Verb.verb("VERB_WAEHLEN","w�hlen"),
		Verb.verb("VERB_WAGEN","wagen"),
		Verb.verb("VERB_WANKEN","wanken"),
		Verb.verb("VERB_WARTEN","warten"),
		Verb.verb("VERB_WECHSELN","wechseln"),
		Verb.verb("VERB_WECKEN","wecken"),
		Verb.verb("VERB_WEGSCHUBSEN","schubsen","weg"),
		Verb.verb("VERB_WEGWISCHEN","wischen", "weg"),
		Verb.verb("VERB_WEICHEN","weichen"),
		Verb.verb("VERB_WEIGERN","weigern"),
		Verb.verb("VERB_WERFEN","werfen"),
		Verb.verb("VERB_WICKELN","wickeln"),
		Verb.verb("VERB_WIDERSETZEN","widersetzen"),
		Verb.verb("VERB_WIDERSTEHEN","widerstehen"),
		Verb.verb("VERB_WIEGEN","wiegen"),
		Verb.verb("VERB_WINDEN","winden"),
		Verb.verb("VERB_WIRKEN","wirken"),
		Verb.verb("VERB_WISSEN","wissen"),
		Verb.verb("VERB_WRINGEN","wringen"),
		Verb.verb("VERB_WUENSCHEN","w�nschen"),
		Verb.verb("VERB_WUERGEN","w�rgen"),
		Verb.verb("VERB_ZAHLEN","zahlen"),
		Verb.verb("VERB_ZAPPELN","zappeln"),
		Verb.verb("VERB_ZERBRECHEN","zerbrechen"),
		Verb.verb("VERB_ZERFALLEN","zerfallen"),
		Verb.verb("VERB_ZERKRATZEN","zerkratzen"),
		Verb.verb("VERB_ZERQUETSCHEN","zerquetschen"),
		Verb.verb("VERB_ZERREISSEN","zerrei�en"),
		Verb.verb("VERB_ZERSPLITTERN","zersplittern"),
		Verb.verb("VERB_ZERSPRINGEN","zerspringen"),
		Verb.verb("VERB_ZERSTOEREN","zerst�ren"),
		Verb.verb("VERB_ZERTEILEN","zerteilen"),
		Verb.verb("VERB_ZIEHEN","ziehen"),
		Verb.verb("VERB_ZITTERN","zittern"),
		Verb.verb("VERB_ZUCKEN","zucken"),
		Verb.verb("VERB_ZURUECKKEHREN","kehren", "zur�ck"),
		Verb.verb("VERB_ZUSCHLAGEN","schlagen","zu"),
		Verb.verb("VERB_ZUSCHNAPPEN","schnappen", "zu"),
		Verb.verb("VERB_ZUSCHNUEREN","schn�ren","zu"),
		Verb.verb("VERB_ZUSTECHEN","stechen","zu"),
		Verb.verb("VERB_ZWAENGEN","zw�ngen"),

		# englische Bezeichner
		Verb.verb("VERB_BUTT","schlagen", "aus"),
		Verb.verb("VERB_CHUCKLE","kichern"),
		Verb.verb("VERB_CONFUSE","verwirren"),
		Verb.verb("VERB_CONSUME", "konsumieren"),
		Verb.verb("VERB_CONTAIN","enthalten"),
		Verb.verb("VERB_CORRODE", "korrodieren"),
		Verb.verb("VERB_DECIDE", "beschlie�en"),
		Verb.verb("VERB_DESTROY", "zerst�ren"),
		Verb.verb("VERB_DISARM","entsch�rfen"),
		Verb.verb("VERB_ENGULF","verschlingen"),
		Verb.verb("VERB_EXPEL","schleudern", "heraus"),
		Verb.verb("VERB_OFFER", "opfern"),
		Verb.verb("VERB_OWE","schulden"),
		Verb.verb("VERB_SACRIFICE", "opfern"),
		Verb.verb("VERB_STING", "stechen"),

		# aus sounds.c
		Verb.verb("VERB_HISS","fauchen"),
		Verb.verb("VERB_GROWL","knurren"),
		Verb.verb("VERB_YELP","jaulen"),
		Verb.verb("VERB_YOWL","jaulen"),
		Verb.verb("VERB_ROAR","br�llen"),
		Verb.verb("VERB_BUZZ","brummen"),
		Verb.verb("VERB_SQUEAL","quieken"),
		Verb.verb("VERB_SCREECH","kr�chzen"),
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
	verben.each {|v| puts '  {"'+v.infinitiv+'", "'+v.infinitiv_zu+'", "'+v.kennung+'"},' }
	puts "  {NULL, NULL, NULL}\n};"

	puts "\nstruct verb_imperativ_struct verben_imperativ[] = {"
	verben.each {|v|
		puts '  {"'+v.imperativ+'", "'+
			v.plural.praesens.zweitePerson.form+'", "'+
			v.praeverb+'", "'+v.kennung+'"},' }
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
  # behandle unver�nderliche Adjektive hier
  case adjektiv
  when "Luzerner"
    return ""
  end

  # $stderr.puts kasus + " " + geschlecht + " " + numerus + " " + artikel
  # regelm�ssige Adjektive
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

  raise "Adjektivendung f�r " + adjektiv + " nicht gefunden: "+kasus+" "+geschlecht+" "+numerus+" "+artikel
end

def dekliniere_nominalphrase(bezeichner,
                             adjektive, # Array oder String
                             singularstamm, genitiv_singular_endung,
                             pluralstamm, nominativ_plural_endung,
                             geschlecht="",
                             fugenelement="",
                             zusatz="")
  throw "Geschlecht fuer #{bezeichner} nicht angeben" if geschlecht == ""
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
    #unregelmaessiges_wort("NOUN_CORPSE",  "sterblichen �berreste", [$nom,$akk], $fem, $sg),
    #unregelmaessiges_wort("NOUN_CORPSE",  "sterblichen �berreste",  $gen,       $fem, $sg),
    #unregelmaessiges_wort("NOUN_CORPSE",  "sterblichen �berresten", $dat,       $fem, $sg),
    #unregelmaessiges_wort("NOUN_CORPSEs",  "sterblichen �berreste von", [$nom,$akk], $fem, $pl),
    #unregelmaessiges_wort("NOUN_CORPSEs",  "sterblichen �berreste von",  $gen,       $fem, $pl),
    #unregelmaessiges_wort("NOUN_CORPSEs",  "sterblichen �berresten von", $dat,       $fem, $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_EINER",  "einer",   $nom, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_EINER",  "einen",   $akk, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_EINER",  "einem",   $dat, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_EINER",  "eines",   $gen, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_EINER",  "eine",    [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_EINER",  "einer",   [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_EINER",  "eines",   [$nom,$gen,$akk], $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_EINER",  "einem",   $dat, $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_EINER",  "eine",    [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_EINER",  "einen",   $dat, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_EINER",  "einer",   $gen, [$mal,$fem,$neu], $pl),
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
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_MFN_PERSONALs", "sie",   $nom,     [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_MFN_PERSONALs", "ihrer", $gen,     [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_MFN_PERSONALs", "ihnen", $dat,     [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("NOUN_PRONOMEN_3P_MFN_PERSONALs", "sie",   $akk,     [$mal,$fem,$neu], $pl),
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
    unregelmaessiges_wort("PRONOMEN_MEIN",  "mein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_MEIN",  "meines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_MEIN",  "meinem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_MEIN",  "meinen", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_MEIN",  "mein",   $akk,  $neu,       $sg),
    unregelmaessiges_wort("PRONOMEN_MEIN",  "meine",  [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_MEIN",  "meiner", [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_MEIN",  "meine",  [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_MEIN",  "meiner",  $gen,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_MEIN",  "meinen",  $dat,       [$mal,$fem,$neu], $pl),
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
    unregelmaessiges_wort("PRONOMEN_SEIN",  "sein",   $nom, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_SEIN",  "seines", $gen, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_SEIN",  "seinem", $dat, [$mal,$neu], $sg),
    unregelmaessiges_wort("PRONOMEN_SEIN",  "seinen", $akk,  $mal,       $sg),
    unregelmaessiges_wort("PRONOMEN_SEIN",  "sein",   $akk,  $neu,       $sg),
    unregelmaessiges_wort("PRONOMEN_SEIN",  "seine",  [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_SEIN",  "seiner", [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_SEIN",  "seine",  [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_SEIN",  "seiner",  $gen,       [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_SEIN",  "seinen",  $dat,       [$mal,$fem,$neu], $pl),
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
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welcher",   $nom, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welchen",   $akk, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welchem",   $dat, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welches",   $gen, $mal, $sg),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welche",    [$nom,$akk], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welcher",   [$gen,$dat], $fem, $sg),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welches",   [$nom,$gen,$akk], $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welchem",   $dat, $neu, $sg),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welche",    [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welchen",   $dat, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_WELCHER",  "welcher",   $gen, [$mal,$fem,$neu], $pl),
    "",
    unregelmaessiges_wort("PRONOMEN_MEHRERE",  "mehrere",   [$nom,$akk], [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_MEHRERE",  "mehreren",   $dat, [$mal,$fem,$neu], $pl),
    unregelmaessiges_wort("PRONOMEN_MEHRERE",  "mehrerer",   $gen, [$mal,$fem,$neu], $pl),
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
    dekliniere_substantiv("NOUN_ARMOR",     "R�stung",        "",   "R�stung",      "en", "feminin"),
    # dekliniere_substantiv("NOUN_ARMOR",     "R�stzeug",        "es",   "R�stzeug",      "e", "neutrum", "s"),
    # dekliniere_substantiv("NOUN_ARMOR",     "Schutzbekleidung", "",   "Schutzbekleidung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_RING",      "Ring",           "es", "Ring",           "e",  "maskulin"),
    dekliniere_substantiv("NOUN_AMULET",    "Amulett",        "es", "Amulett",        "e",  "neutrum"),
    dekliniere_substantiv("NOUN_TOOL",      "Werkzeug",       "es", "Werkzeug",       "e",  "neutrum", "s"),
    # dekliniere_adjektivisches_substantiv("NOUN_COMESTIBLE","Essbar", "neutrum"),
    dekliniere_substantiv("NOUN_COMESTIBLE","Lebensmittel",   "s",  "Lebensmittel",   "",   "neutrum"),
    dekliniere_adjektivisches_substantiv("NOUN_ESSBARES","Essbar", "neutrum"),
    dekliniere_substantiv("NOUN_POTION",    "Trank",          "es", "Tr�nk",          "e",  "maskulin"),
    dekliniere_substantiv("NOUN_FLASCHE",   "Flasche",        "",  "Flasche",         "en", "feminin"),
    dekliniere_substantiv("NOUN_SCROLL",    "Schriftrolle",   "",   "Schriftrolle",   "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SPELLBOOK", "Zauberbuch",     "es", "Zauberb�ch",     "er", "neutrum"),
    dekliniere_substantiv("NOUN_WAND",      "Zauberstab",     "es", "Zauberst�b",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_COIN",      "M�nze",          "",   "M�nze",          "en", "feminin"),
    "/* besser als Edelstein, Kleinod? */",
    dekliniere_substantiv("NOUN_GEM",       "Schmuckstein",   "es", "Schmuckstein",   "e",  "maskulin"),
    dekliniere_substantiv("NOUN_GEM",       "Edelstein",   "es", "Edelstein",   "e",  "maskulin"),
    "",
    dekliniere_nominalphrase("NOUN_STRANGE_OBJECT", "merkw�rdig", "Objekt", "es", "Objekt", "e", "neutrum"),
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
    dekliniere_substantiv("NOUN_WORM_TOOTH", "Wurmzahn", "es", "Wurmz�hn", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CRYSKNIFE", "Crysmesser", "s", "Crysmesser", "", "neutrum"),
    dekliniere_substantiv("NOUN_AXE", "Beil", "es", "Beil", "e", "neutrum"),
    dekliniere_substantiv("NOUN_AXE", "Axt", "", "�xt", "e", "feminin"),
    dekliniere_substantiv("NOUN_BATTLE_AXE", "Streitaxt", "", "Streit�xt", "e", "feminin"),
    dekliniere_substantiv("NOUN_SHORT_SWORD", "Kurzschwert", "es", "Kurzschwert", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_ELVEN_SHORT_SWORD", "elbisch", "Kurzschwert", "es", "Kurzschwert", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_ORCISH_SHORT_SWORD", "orksch", "Kurzschwert", "es", "Kurzschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_DWARVISH_SHORT_SWORD", "Zwergen-Kurzschwert", "es", "Zwergen-Kurzschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SCIMITAR", "Szimitar", "", "Szimitar", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SILVER_SABER", "Silbers�bel", "s", "Silbers�bel", "", "maskulin"),
    dekliniere_substantiv("NOUN_BROADSWORD", "Breitschwert", "es", "Breitschwert", "er", "neutrum", "s"),
    dekliniere_nominalphrase("NOUN_ELVEN_BROADSWORD", "elbisch", "Breitschwert", "es", "Breitschwert", "er", "neutrum", "s"),
    dekliniere_substantiv("NOUN_LONG_SWORD", "Langschwert", "es", "Langschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_TWO_HANDED_SWORD", "Zweih�nder", "s", "Zweih�nder", "", "maskulin"),
    dekliniere_substantiv("NOUN_TWO_HANDED_SWORD", "Bih�nder", "s", "Bih�nder", "", "maskulin"),
    dekliniere_substantiv("NOUN_KATANA", "Katana", "", "Katana", "s", "neutrum"),
    dekliniere_substantiv("NOUN_TSURUGI", "Tsurugi", "", "Tsurugi", "s", "neutrum"),
    dekliniere_substantiv("NOUN_RUNESWORD", "Runenschwert", "es", "Runenschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_PARTISAN", "Partisane", "", "Partisane", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RANSEUR", "Knebelspie�", "es", "Knebelspie�", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SPETUM", "Runka", "", "Runka", "s", "feminin"),
    dekliniere_substantiv("NOUN_GLAIVE", "Glefe", "", "Glefe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_LANCE", "Lanze", "", "Lanze", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_HALBERD", "Hellebarde", "", "Hellebarde", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BARDICHE", "Bardiche", "", "Bardiche", "", "feminin", "n"),
    dekliniere_substantiv("NOUN_VOULGE", "Helmbarte", "", "Helmbarte", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_DWARVISH_MATTOCK", "Zwergenhacke", "", "Zwergenhacke", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_FAUCHARD", "Kriegssense", "", "Kriegssense", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GUISARME", "Kuse", "", "Kuse", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BILL_GUISARME", "Ro�schinder", "s", "Ro�schinder", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_LUCERN_HAMMER", "Luzerner", "Hammer", "s", "H�mmer", "", "maskulin"),
    dekliniere_substantiv("NOUN_BEC_DE_CORBIN", "Rabenschnabel", "s", "Rabenschn�bel", "", "maskulin"),
    dekliniere_substantiv("NOUN_MACE", "Streitkolben", "s", "Streitkolben", "", "maskulin"),
    dekliniere_substantiv("NOUN_MORNING_STAR", "Morgenstern", "es", "Morgenstern", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAR_HAMMER", "Kriegshammer", "s", "Kriegsh�mmer", "", "maskulin"),
    dekliniere_substantiv("NOUN_CLUB", "Keule", "", "Keule", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RUBBER_HOSE", "Gummikn�ppel", "s", "Gummikn�ppel", "", "maskulin"),
    dekliniere_substantiv("NOUN_QUARTERSTAFF", "Kampfstab", "es", "Kampfst�b", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_AKLYS", "Aklys", "", "Aklys", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_FLAIL", "Dreschflegel", "s", "Dreschflegel", "", "maskulin"),
    dekliniere_substantiv("NOUN_BULLWHIP", "Peitsche", "", "Peitsche", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BULLWHIP", "Bullenpeitsche", "", "Bullenpeitsche", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BOW", "Bogen", "s", "Bogen", "", "maskulin"),
    dekliniere_substantiv("NOUN_ELVEN_BOW", "Elbenbogen", "s", "Elbenbogen", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_BOW", "Orkbogen", "s", "Orkbogen", "", "maskulin"),
    dekliniere_substantiv("NOUN_YUMI", "Yumi", "", "Yumi", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_SLING", "Schleuder", "", "Schleuder", "en", "feminin"),
    dekliniere_substantiv("NOUN_CROSSBOW", "Armbrust", "", "Armbr�st", "e", "feminin"),
    "",
    "/* Weapons, unidentified */",
    dekliniere_substantiv("NOUN_RUNED_ARROW", "Runenpfeil", "es", "Runenpfeil", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_CRUDE_ARROW", "klobig", "Pfeil", "es", "Pfeil", "e", "maskulin"), # klobig, wuchtig, unfoermig, primitiv,  grobschlaechtig?, massig, plumb, klotzig
    dekliniere_substantiv("NOUN_BAMBOO_ARROW", "Bambuspfeil", "es", "Bambuspfeil", "e", "maskulin"),
    dekliniere_substantiv("NOUN_THROWING_STAR", "Wurfstern", "es", "Wurfstern", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RUNED_SPEAR", "Runenspeer", "es", "Runenspeer", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_CRUDE_SPEAR", "klobig", "Speer", "es", "Speer", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_STOUT_SPEAR", "schwer", "Speer", "es", "Speer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_THROWING_SPEAR", "Wurfspeer", "es", "Wurfspeer", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RUNED_DAGGER", "Runendolch", "es", "Runendolch", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_CRUDE_DAGGER", "klobig", "Dolch", "es", "Dolch", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_DOUBLE_HEADED_AXE", "zweischneidig", "Doppelaxt", "", "Doppel�xt", "e", "feminin"),
    dekliniere_substantiv("NOUN_DOUBLE_HEADED_AXE", "Doppelaxt", "", "Doppel�xt", "e", "feminin"),
    dekliniere_substantiv("NOUN_RUNED_SHORT_SWORD", "Runenkurzschwert", "es", "Runenkurzschwert", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_CRUDE_SHORT_SWORD", "klobig", "Kurzschwert", "es", "Kurzschwert", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_BROAD_SHORT_SWORD", "breit", "Kurzschwert", "es", "Kurzschwert", "e", "neutrum"),
    dekliniere_substantiv("NOUN_CURVED_SWORD", "Krummschwert", "es", "Krummschwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_RUNED_BROADSWORD", "Runenbreitschwert", "es", "Runenbreitschwert", "er", "neutrum", "s"),
    dekliniere_substantiv("NOUN_SAMURAI_SWORD", "Samuraischwert", "es", "Samuraischwert", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_LONG_SAMURAI_SWORD", "lang", "Samuraischwert", "es", "Samuraischwert", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_VULGAR_POLEARM", "gew�hnlich", "Stangenwaffe", "", "Stangenwaffe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_HILTED_POLEARM", "Sto�spie�", "es", "Sto�spie�", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_FORKED_POLEARM", "gegabelt", "Stangenwaffe", "", "Stangenwaffe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SINGLE_EDGED_POLEARM", "Schwertspie�", "es", "Schwertspie�", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_ANGLED_POLEAXE", "abgewinkelt", "Stangenaxt", "", "Stangen�xt", "e", "feminin"),
    dekliniere_nominalphrase("NOUN_LONG_POLEAXE", "lang", "Stangenbeil", "es", "Stangenbeil", "e", "neutrum"),
    dekliniere_substantiv("NOUN_POLE_CLEAVER", "Stangenbeil", "es", "Stangenbeil", "e", "neutrum"),
    dekliniere_substantiv("NOUN_BROAD_PICK", "Breithacke", "", "Breithacke", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BROAD_PICK", "Rodehacke", "", "Rodehacke", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_POLE_SICKLE", "Stangensichel", "", "Stangensichel", "en", "feminin"),
    dekliniere_substantiv("NOUN_PRUNING_HOOK", "Stangenmesser", "s", "Stangenmesser", "", "neutrum"),
    dekliniere_substantiv("NOUN_HOOKED_POLEARM", "Hippe", "", "Hippe", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_PRONGED_POLEARM", "lang", "Streithammer", "s", "Streith�mmer", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_BEAKED_POLEARM", "spitz", "Streithammer", "s", "Streith�mmer", "", "maskulin"),
    dekliniere_substantiv("NOUN_STAFF", "Stock", "es", "St�ck", "e", "maskulin"),
    dekliniere_substantiv("NOUN_THONGED_CLUB", "Riemenkn�ppel", "s", "Riemenkn�ppel", "", "maskulin"),
    dekliniere_substantiv("NOUN_RUNED_BOW", "Runenbogen", "s", "Runenbogen", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_CRUDE_BOW", "klobig", "Bogen", "s", "Bogen", "", "maskulin"),
    dekliniere_substantiv("NOUN_LONG_BOW", "Langbogen", "s", "Langbogen", "", "maskulin"),
    "",
    "/* Armor, identified */",
    dekliniere_nominalphrase("NOUN_ELVEN_LEATHER_HELM", "ledern", "Elbenhelm", "es", "Elbenhelm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_HELM", "Orkhelm", "es", "Orkhelm", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_DWARVISH_IRON_HELM", "eisern", "Zwergenhelm", "es", "Zwergenhelm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FEDORA", "Fedora", "", "Fedora", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_CORNUTHAUM", "Cornuthaum", "", "Cornuthaum", "fremder", "maskulin"), # Zaubererhut? Etwas lasch
    dekliniere_substantiv("NOUN_CORNUTHAUM", "Zauberhut", "es", "Zauberh�t", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DUNCE_CAP", "Narrenkappe", "", "Narrenkappe", "en", "feminin"), # Narrenkappe, Eselsm�tze
    dekliniere_nominalphrase("NOUN_DENTED_POT", "verbeult", "Topf", "es", "T�pf", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HELMET", "Helm", "es", "Helm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HELM_OF_BRILLIANCE", "Helm", "es", "Helm", "e", "maskulin","","der Genialit�t"),
    dekliniere_substantiv("NOUN_HELM_OF_OPPOSITE_ALIGNMENT", "Helm", "es", "Helm", "e", "maskulin","","des Gesinnungswandel"), # Helm der entgegengesetzten Gesinnung, Helm des Gesinnungswandel
    dekliniere_substantiv("NOUN_HELM_OF_OPPOSITE_ALIGNMENT", "Wendehalshelm", "es", "Wendehalshelm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HELM_OF_TELEPATHY", "Helm", "es", "Helm", "e", "maskulin","","der Telepathie"),
    dekliniere_substantiv("NOUN_GRAY_DRAGON_SCALE_MAIL", "Graudrachen-Schuppenpanzer", "s", "Graudrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_SILVER_DRAGON_SCALE_MAIL", "Silberdrachen-Schuppenpanzer", "s", "Silberdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_SHIMMERING_DRAGON_SCALE_MAIL", "Glanzdrachen-Schuppenpanzer", "s", "Glanzdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_RED_DRAGON_SCALE_MAIL", "Rotdrachen-Schuppenpanzer", "s", "Rotdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_WHITE_DRAGON_SCALE_MAIL", "Wei�drachen-Schuppenpanzer", "s", "Wei�drachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORANGE_DRAGON_SCALE_MAIL", "Orangedrachen-Schuppenpanzer", "s", "Orangedrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_BLACK_DRAGON_SCALE_MAIL", "Schwarzdrachen-Schuppenpanzer", "s", "Schwarzdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_BLUE_DRAGON_SCALE_MAIL", "Blaudrachen-Schuppenpanzer", "s", "Blaudrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_GREEN_DRAGON_SCALE_MAIL", "Gr�ndrachen-Schuppenpanzer", "s", "Gr�ndrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_YELLOW_DRAGON_SCALE_MAIL", "Gelbdrachen-Schuppenpanzer", "s", "Gelbdrachen-Schuppenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_GRAY_DRAGON_SCALES", "Graudrachen-Schuppe", "", "Graudrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SILVER_DRAGON_SCALES", "Silberdrachen-Schuppe", "", "Silberdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SHIMMERING_DRAGON_SCALES", "Glanzdrachen-Schuppe", "", "Glanzdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RED_DRAGON_SCALES", "Rotdrachen-Schuppe", "", "Rotdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WHITE_DRAGON_SCALES", "Wei�drachen-Schuppe", "", "Wei�drachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ORANGE_DRAGON_SCALES", "Orangedrachen-Schuppe", "", "Orangedrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BLACK_DRAGON_SCALES", "Schwarzdrachen-Schuppe", "", "Schwarzdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BLUE_DRAGON_SCALES", "Blaudrachen-Schuppe", "", "Blaudrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GREEN_DRAGON_SCALES", "Gr�ndrachen-Schuppe", "", "Gr�ndrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_YELLOW_DRAGON_SCALES", "Gelbdrachen-Schuppe", "", "Gelbdrachen-Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_PLATE_MAIL", "Plattenpanzer",  "s", "Plattenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_CRYSTAL_PLATE_MAIL", "Kristallpanzer",  "s", "Kristallpanzer", "", "maskulin"), # Kristallplattenpanzer, kristallener Plattenpanzer, Kristallpanzer
    dekliniere_substantiv("NOUN_BRONZE_PLATE_MAIL", "Bronzepanzer",  "s", "Bronzepanzer", "", "maskulin"),  # Bronzeplattenpanzer, bronzener Plattenpanzer, Bronzepanzer
    dekliniere_substantiv("NOUN_SPLINT_MAIL", "Schienenpanzer",  "s", "Schienenpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_BANDED_MAIL", "B�nderpanzer",  "s", "B�nderpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_BANDED_MAIL", "Spangenharnisch",  "s", "Spangenharnisch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DWARVISH_MITHRIL_COAT", "Zwergen-Mithrilhemd", "es", "Zwergen-Mithrilhemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_ELVEN_MITHRIL_COAT", "Elben-Mithrilhemd", "es", "Elben-Mithrilhemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_CHAIN_MAIL", "Kettenhemd", "es", "Kettenhemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_ORCISH_CHAIN_MAIL", "Orkkettenhemd", "es", "Orkkettenhemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_SCALE_MAIL", "Schuppenpanzer", "s", "Schuppenpanzer", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_STUDDED_LEATHER_ARMOR", "beschlagen", "Lederwams", "es", "Lederw�ms", "er", "neutrum"),
    dekliniere_substantiv("NOUN_RING_MAIL", "Ringpanzer", "s", "Ringpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_RING_MAIL", "Orkpanzer", "s", "Orkpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEATHER_ARMOR", "Lederpanzer", "s", "Lederpanzer", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEATHER_JACKET", "Lederjacke", "", "Lederjacke", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_HAWAIIAN_SHIRT", "Hawaiihemd", "es", "Hawaiihemd", "en", "neutrum", "en"),
    dekliniere_substantiv("NOUN_T_SHIRT", "T-Shirt", "s", "T-Shirt", "s", "neutrum"),
    dekliniere_substantiv("NOUN_MUMMY_WRAPPING", "Mumienbinde", "", "Mumienbinde", "en", "feminin", "en"), # Mumienbandage? js
    dekliniere_substantiv("NOUN_ELVEN_CLOAK", "Elbenmantel", "s", "Elbenm�ntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_CLOAK", "Orkmantel", "s", "Orkm�ntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_DWARVISH_CLOAK", "Zwergenmantel", "s", "Zwergenm�ntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_OILSKIN_CLOAK", "�lzeug", "es", "�lzeug", "e", "neutrum"), # �lcape oder Regencape? js
    dekliniere_substantiv("NOUN_ROBE", "Robe", "", "Robe", "en", "feminin", "n"), # Robe, Talar
    dekliniere_substantiv("NOUN_ALCHEMY_SMOCK", "Alchemistenkittel", "s", "Alchemistenkittel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEATHER_CLOAK", "Ledermantel", "s", "Lederm�ntel", "", "maskulin"),
    dekliniere_substantiv("NOUN_CLOAK_OF_PROTECTION", "Umhang", "es", "Umh�ng", "e", "maskulin", "s", "des Schutzes"),
    dekliniere_substantiv("NOUN_CLOAK_OF_INVISIBILITY", "Umhang", "es", "Umh�ng", "e", "maskulin", "s", "der Unsichtbarkeit"),
    dekliniere_substantiv("NOUN_CLOAK_OF_MAGIC_RESISTANCE", "Umhang", "es", "Umh�ng", "e", "maskulin", "s", "der Magieresistenz"),
    dekliniere_substantiv("NOUN_CLOAK_OF_DISPLACEMENT", "Umhang", "es", "Umh�ng", "e", "maskulin", "s", "der Dislokation"), # Deplatzierung, Dislokation, Verdr�ngung, Verschiebung, Verr�ckung, Versatz, Verlagerung, Verwerfung, Verzerrung,
    dekliniere_nominalphrase("NOUN_SMALL_SHIELD", "klein", "Schild", "es", "Schild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ELVEN_SHIELD", "Elbenschild", "es", "Elbenschild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_URUK_HAI_SHIELD", "Uruk-Hai-Schild", "es", "Uruk-Hai-Schild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ORCISH_SHIELD", "Orkschild", "es", "Orkschild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_LARGE_SHIELD", "Langschild", "es", "Langschild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DWARVISH_ROUNDSHIELD", "Zwergen-Rundschild", "es", "Zwergen-Rundschild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SHIELD_OF_REFLECTION", "Schild", "es", "Schild", "e", "maskulin", "", "der Spiegelung"),
    dekliniere_substantiv("NOUN_SHIELD_OF_REFLECTION", "Schild", "es", "Schild", "e", "maskulin", "", "der Reflektion"),
    dekliniere_substantiv("NOUN_LEATHER_GLOVES", "Lederhandschuh", "es", "Lederhandschuh", "e", "feminin"),
    dekliniere_substantiv("NOUN_GAUNTLETS_OF_FUMBLING", "Handschuh", "es", "Handschuh", "e", "maskulin", "", "der Tolpatschigkeit"), # Ungeschicklichkeit, Fummligkeit, # js des Ungeschicks?
    dekliniere_substantiv("NOUN_GAUNTLETS_OF_FUMBLING", "Handschuh", "es", "Handschuh", "e", "maskulin", "", "der Fummligkeit"),
    dekliniere_substantiv("NOUN_GAUNTLETS_OF_POWER", "Handschuh", "es", "Handschuh", "e", "maskulin", "", "der Macht"),
    dekliniere_substantiv("NOUN_GAUNTLETS_OF_DEXTERITY", "Handschuh", "es", "Handschuh", "e", "maskulin", "", "der Gewandtheit"), # Geschicklichkeit, Gewandheit, Fertigkeit, Fingerfertigkeit
    dekliniere_nominalphrase("NOUN_LOW_BOOTS", "flach", "Schuh", "es", "Schuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_IRON_SHOES", "Eisenschuh", "es", "Eisenschuh", "e", "feminin"),
    dekliniere_nominalphrase("NOUN_HIGH_BOOTS", "hoh", "Stiefel", "s", "Stiefel", "", "maskulin"), 
    dekliniere_substantiv("NOUN_SPEED_BOOTS", "Haststiefel", "s", "Haststiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_WATER_WALKING_BOOTS", "Wasserl�ufer", "s", "Wasserl�ufer", "", "maskulin"),
    dekliniere_substantiv("NOUN_JUMPING_BOOTS", "Springerstiefel", "s", "Springerstiefel", "", "maskulin"), # Sprungstiefel js
    dekliniere_substantiv("NOUN_ELVEN_BOOTS", "Elbenstiefel", "s", "Elbenstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_KICKING_BOOTS", "Allestreter", "s", "Allestreter", "", "maskulin"),
    dekliniere_substantiv("NOUN_KICKING_BOOTS", "Treterstiefel", "s", "Treterstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_FUMBLE_BOOTS", "Trampelstiefel", "s", "Trampelstiefel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEVITATION_BOOTS", "Flugschuh", "es", "Flugschuh", "e", "maskulin"), # Schwebestiefel? js
    "",
    "/* Armor, unidentified */",
    dekliniere_substantiv("NOUN_LEATHER_HAT", "Lederhut", "es", "Lederh�t", "e", "maskulin"),
    dekliniere_substantiv("NOUN_IRON_SKULL_CAP", "Eisenkappe", "", "Eisenkappe", "en", "feminin"),
    dekliniere_substantiv("NOUN_HARD_HAT", "Grubenhelm", "es", "Grubenhelm", "e", "maskulin"), # woertlich: Schutzhelm # Grubenhelm js
    dekliniere_substantiv("NOUN_SCHLAPPHUT", "Schlapphut", "es", "Schlapph�t", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CONICAL_HAT", "Zipfelm�tze", "", "Zipfelm�tze", "en", "feminin"),
    "/* Die folgenden vier Helme muessen nur verschieden benannt sein. Der Visierhelm muss der einzige sein, der eine Visier hat, da der gegen Blendangriffe schuetzt. */",
    dekliniere_substantiv("NOUN_PLUMED_HELMET", "Normannenhelm", "es", "Normannenhelm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ETCHED_HELMET", "Perserhelm", "es", "Perserhelm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CRESTED_HELMET", "Wikingerhelm", "es", "Wikingerhelm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_VISORED_HELMET", "Visierhelm", "es", "Visierhelm", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_CRUDE_CHAIN_MAIL", "klobig", "Kettenhemd", "es", "Kettenhemd", "en", "neutrum", "en"),
    dekliniere_nominalphrase("NOUN_CRUDE_RING_MAIL", "klobig", "Ringpanzer", "s", "Ringpanzer", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_FADED_PALL", "ausgeblichen", "Mantel", "s", "M�ntel", "", "maskulin"), # Ausgeblichener Mantel js
    dekliniere_nominalphrase("NOUN_COARSE_MANTELET", "grob", "�berwurf", "es", "�berw�rf", "e", "maskulin"), # Grober �berwurf js
    dekliniere_substantiv("NOUN_HOODED_CLOAK", "Kapuzenmantel", "s", "Kapuzenm�ntel", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_SLIPPERY_CLOAK", "rutschig", "Umhang", "s", "Umh�ng", "e", "maskulin"), # glibberig, schl�pfrig, rutschig, aalglatt, �lig
    dekliniere_substantiv("NOUN_APRON", "Sch�rze", "", "Sch�rze", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_TATTERED_CAPE", "Lumpenumhang", "es", "Lumpenumh�ng", "e", "maskulin"), # js
    dekliniere_substantiv("NOUN_OPERA_CLOAK", "Opernumhang", "es", "Opernumh�ng", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_ORNAMENTAL_COPE", "Zierumhang", "es", "Zierumh�ng", "e", "maskulin"), # js
    dekliniere_substantiv("NOUN_PIECE_OF_CLOTH", "St�ck", "es", "St�ck", "e", "neutrum", "", "Stoff"), # ein Stueck Stoff, ein Stueck Tuch
    dekliniere_nominalphrase("NOUN_BLUE_AND_GREEN_SHIELD", ["blau-gr�n"], "Schild", "es", "Schild", "e", "maskulin"),
    # dekliniere_nominalphrase("NOUN_BLUE_AND_GREEN_SHIELD", ["blau", "und gr�n"], "Schild", "es", "Schild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WHITE_HANDED_SHIELD", "Wei�handschild", "es", "Wei�handschild", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RED_EYED_SHIELD", "Rotaugenschild", "es", "Rotaugenschild", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_LARGE_ROUND_SHIELD", "gro�", "Rundschild", "es", "Rundschild", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_POLISHED_SILVER_SHIELD", "poliert", "Silberschild", "es", "Silberschild", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_OLD_GLOVES", "alt", "Handschuh", "es", "Handschuh", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_PADDED_GLOVES", "gef�ttert", "Handschuh", "es", "Handschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RIDING_GLOVES", "Reithandschuh", "es", "Reithandschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FENCING_GLOVES", "Fechthandschuh", "es", "Fechthandschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WALKING_SHOES", "Stra�enschuh", "es", "Stra�enschuh", "e", "maskulin"),
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
    dekliniere_substantiv("NOUN_WAND_SECRET_DOOR_DETECTION", "Geheimt�rfindung", "", "Geheimt�rfindung", "en", "feminin", "s"), # Geheimt�rersp�rung? Geheimt�rfindung netzhack T�rfindung
    dekliniere_substantiv("NOUN_WAND_ENLIGHTENMENT", "Erleuchtung", "", "Erleuchtung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_WAND_CREATE_MONSTER", "Monsterbeschw�rung", "", "Monsterbeschw�rung", "en", "feminin"), # Monstererschaffung, Monsterherbeirufung
    dekliniere_substantiv("NOUN_WAND_WISHING", "W�nschen", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_NOTHING", "Nichts", "", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_STRIKING", "Kraftsto�", "es", "Kraftst��", "e", "maskulin"), # js
    dekliniere_substantiv("NOUN_WAND_MAKE_INVISIBLE", "Unsichtbarkeit", "", "Unsichtbarkeit", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SLOW_MONSTER", "Verlangsamung", "", "Verlangsamung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SPEED_MONSTER", "Hast", "", "Hast", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_UNDEAD_TURNING", "Untotenwandlung", "", "Untotenwandlung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_WAND_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_WAND_POLYMORPH", "Polymorphie", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_CANCELLATION", "L�schung", "", "L�schung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_WAND_TELEPORTATION", "Teleportation", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_WAND_OPENING", "�ffnung", "", "�ffnung", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_LOCKING", "Schlie�en", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_PROBING", "Pr�fung", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_DIGGING", "Graben", "s", "", "", "neutrum"),
    dekliniere_nominalphrase("NOUN_WAND_MAGIC_MISSILE", "magisch", "Geschoss", "es", "Geschoss", "e", "neutrum"),  # magische Geschosse
    dekliniere_substantiv("NOUN_WAND_FIRE", "Feuer", "s", "Feuer", "", "neutrum"),
    dekliniere_substantiv("NOUN_WAND_COLD", "K�lte", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_WAND_SLEEP", "Schlaf", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAND_DEATH", "Tod", "es", "Tod", "e", "maskulin"),
    dekliniere_substantiv("NOUN_WAND_LIGHTNING", "Blitz", "es", "Blitz", "e", "maskulin"),
    "",
    "/* Rings, identified */",
    dekliniere_substantiv("NOUN_RING_ADORNMENT", "Zierde", "", "Zierde", "en", "feminin"), # der Versch�nerung, Zierde, Verzierung
    dekliniere_substantiv("NOUN_RING_GAIN_STRENGTH", "St�rke", "", "St�rke", "en", "feminin"), #  der St�rke, der Kraft, der Kampfkraft?
    dekliniere_substantiv("NOUN_RING_GAIN_CONSTITUTION", "Konstitution", "", "Konstitution", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_INCREASE_ACCURACY", "Treffgenauigkeit", "", "Treffgenauigkeit", "en", "feminin"), # der Treffgenauigkeit, der Pr�zision, der groesseren Praezision
    dekliniere_substantiv("NOUN_RING_INCREASE_DAMAGE", "Schaden", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_RING_PROTECTION", "Schutz", "es", "Schutz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RING_REGENERATION", "Regeneration", "", "Regeneration", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_SEARCHING", "Suchen", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_RING_STEALTH", "Schleichen", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_RING_SUSTAIN_ABILITY", "Fertigkeitenerhaltung", "", "Fertigkeitenerhaltung", "", "feminin"), # js
    dekliniere_substantiv("NOUN_RING_LEVITATION", "Levitation", "", "Levitation", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_HUNGER", "Hungern", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_RING_AGGRAVATE_MONSTER", "Monsterreizung", "", "Monsterreizung", "en", "feminin"), # ?Monsterreizung, Monsternanziehung?, Monsterzorn
    dekliniere_substantiv("NOUN_RING_CONFLICT", "Konflikt", "es", "Konflikt", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RING_WARNING", "Vorwarnung", "", "Vorwarnung", "en", "feminin"), # Warnung, Vorwarnung?
    dekliniere_substantiv("NOUN_RING_POISON_RESISTANCE", "Giftresistenz", "", "Giftresistenz", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_FIRE_RESISTANCE", "Feuerresistenz", "", "Feuerresistenz", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_COLD_RESISTANCE", "K�lteresistenz", "", "K�lteresistenz", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_SHOCK_RESISTANCE", "Stromresistenz", "", "Stromresistenz", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_FREE_ACTION", "Bewegungsfreiheit", "", "Bewegungsfreiheit", "en", "feminin"), # Freiheit, Bewegungsfreiheit, freien Bewegung
    dekliniere_nominalphrase("NOUN_RING_SLOW_DIGESTION", "langsam", "Verdauung", "", "Verdauung", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_TELEPORTATION", "Teleportation", "", "Teleportation", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_TELEPORT_CONTROL", "Teleportationskontrolle", "", "Teleportationskontrolle", "en", "feminin"), 
    dekliniere_substantiv("NOUN_RING_TELEPORT_CONTROL", "Teleportkontrolle", "", "Teleportkontrolle", "en", "feminin"), 
    dekliniere_substantiv("NOUN_RING_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_POLYMORPH", "Polymorphie", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_POLYMORPH_CONTROL", "Transformationskontrolle", "", "Transformationskontrolle", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_POLYMORPH_CONTROL", "Polymorphiekontrolle", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_INVISIBILITY", "Unsichtbarkeit", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_RING_SEE_INVISIBLE", "Enth�llung", "", "Enth�llung", "en", "feminin", "", "des Unsichtbaren"),
    dekliniere_substantiv("NOUN_RING_SEE_INVISIBLE", "Enth�llung", "", "Enth�llung", "en", "feminin"),
    dekliniere_substantiv("NOUN_RING_PROTECTION_FROM_SHAPE_CHANGERS", "Schutz", "es", "Schutz", "en", "feminin", "", "vor Formwandlern"),
    dekliniere_substantiv("NOUN_RING_PROTECTION_FROM_SHAPE_CHANGERS", "Schutz", "es", "Schutz", "en", "feminin", "", "vor Gestaltwandlern"),
    "",
    "/* Rings, unidentified */",
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_WOODEN",     "Holzring",       "es", "Holzring",       "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_GRANITE",    "Granitring",     "es", "Granitring",     "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_OPAL",       "Opalring",       "es", "Opalring",       "e",  "maskulin"),
    dekliniere_substantiv("NOUN_RING_UNIDENTIFIED_CLAY",       "Lehmring",        "es", "Lehmring",        "e",  "maskulin"),
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

    dekliniere_substantiv("RING_UNIDENTIFIED_WOODEN",     "Holz",        "es", "H�lz",        "er", "neutrum"),
    dekliniere_substantiv("RING_UNIDENTIFIED_GRANITE",    "Granit",      "s",  "Granit",      "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_OPAL",       "Opal",        "es", "Opal",        "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_CLAY",       "Lehm",         "es", "Lehm",         "e",  "maskulin"),
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
    dekliniere_substantiv("RING_UNIDENTIFIED_WIRE",       "Draht",       "es", "Dr�ht",       "e",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_ENGAGEMENT", "Verlobung",   "",   "Verlobung",   "en", "feminin"),
    "/* Don't want to do a special treatment for twisted and shiny, so these are replaced with other gems */ ",
    dekliniere_substantiv("RING_UNIDENTIFIED_TWISTED",    "Lapislazuli", "",   "Lapislazuli", "",  "maskulin"),
    dekliniere_substantiv("RING_UNIDENTIFIED_SHINY",      "Zirkon",      "s",  "Zirkon",      "e",  "maskulin"),
    "",
    "/* Amulets, identified */",
    dekliniere_substantiv("NOUN_AMULET_OF_ESP", "Amulett", "es", "Amulett", "e", "neutrum", "", 
                          "der Au�ersinnlichen Wahrnehmung"),
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
    dekliniere_substantiv("NOUN_AMULET_OF_UNCHANGING", "Amulett", "es", "Amulett", "e", "neutrum", "", "der Unwandlung"),
    dekliniere_substantiv("NOUN_AMULET_OF_UNCHANGING", "Amulett", "es", "Amulett", "e", "neutrum", "", "der Unwandelbarkeit"),
    dekliniere_substantiv("NOUN_AMULET_OF_UNCHANGING", "Amulett", "es", "Amulett", "e", "neutrum", "", "der Unver�nderlichkeit"),
    dekliniere_substantiv("NOUN_AMULET_OF_REFLECTION", "Amulett", "es", "Amulett", "e", "neutrum", "", "der Spiegelung"),
    dekliniere_substantiv("NOUN_AMULET_OF_REFLECTION", "Amulett", "es", "Amulett", "e", "neutrum", "", "der Reflektion"),
    dekliniere_substantiv("NOUN_AMULET_OF_MAGICAL_BREATHING", "Amulett", "es", "Amulett", "e", "neutrum", "", "des magischen Lebensodems"), # magischer Lebensodem, magischer Odem?
    dekliniere_substantiv("NOUN_AMULET_OF_MAGICAL_BREATHING", "Amulett", "es", "Amulett", "e", "neutrum", "", "des magischen Atems"),
    dekliniere_substantiv("NOUN_AMULET_OF_MAGICAL_BREATHING", "Amulett", "es", "Amulett", "e", "neutrum", "", "des magischen Atmens"),
    dekliniere_substantiv("NOUN_AMULET_OF_YENDOR", "Amulett", "es", "Amulett", "e", "neutrum", "", 
                          "von Yendor"),
    dekliniere_nominalphrase("NOUN_FAKE_AMULET_OF_YENDOR", "billig", "Plastikimitat", "es", "Plastikimitat", "e",
                             "neutrum", "", "des Amulettes von Yendor"),
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
    "/* Tools, identified */",
    dekliniere_substantiv("NOUN_LARGE_BOX", "Kiste", "", "Kiste", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_CHEST", "Truhe", "", "Truhe", "en", "feminin"),
    dekliniere_substantiv("NOUN_ICE_BOX", "K�hltruhe", "", "K�hltruhe", "en", "feminin"),
    dekliniere_substantiv("NOUN_ICE_BOX", "Eisbox", "", "Eisbox", "en", "feminin"),
    dekliniere_substantiv("NOUN_SACK", "Sack", "es", "S�ck", "e", "maskulin"),
    dekliniere_substantiv("NOUN_OILSKIN_SACK","�lzeugbeutel", "s", "�lzeugbeutel", "", "maskulin"),  # Wachstuch, Oilskin, Oelzeug, Regenhaut
    dekliniere_nominalphrase("NOUN_BAG_OF_HOLDING", "Nimmervoll", "Beutel","s","Beutel","","maskulin"), # Beutel der Verstauung
    dekliniere_substantiv("NOUN_BAG_OF_TRICKS", "Trickbeutel", "s", "Trickbeutel","","maskulin"),  # netzhack: Wundert�te
    dekliniere_substantiv("NOUN_SKELETON_KEY", "Passepartout", "s", "Passepartout", "", "maskulin"),
    dekliniere_substantiv("NOUN_SKELETON_KEY", "Generalschl�ssel", "s", "Generalschl�ssel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SKELETON_KEY", "Hauptschl�ssel", "s", "Hauptschl�ssel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LOCK_PICK", "Dietrich", "s", "Dietrich", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CREDIT_CARD", "Kreditkarte", "", "Kreditkarte", "en", "feminin"),
    dekliniere_substantiv("NOUN_TALLOW_CANDLE", "Talglicht", "es", "Talglicht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_WAX_CANDLE", "Wachskerze", "", "Wachskerze", "en", "feminin"),
    dekliniere_substantiv("NOUN_BRASS_LANTERN", "Laterne", "", "Laterne", "en", "feminin"),
    dekliniere_substantiv("NOUN_OIL_LAMP", "�llampe", "", "�llampe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_MAGIC_LAMP", "Wunderlampe", "", "Wunderlampe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_MAGIC_LAMP", "Zauberlampe", "", "Zauberlampe", "en", "feminin", "n"),
    dekliniere_nominalphrase("NOUN_EXPENSIVE_CAMERA", "teur", "Photoapparat", "es", "Photoapparat", "e", "maskulin", "s"),
    dekliniere_nominalphrase("NOUN_EXPENSIVE_CAMERA", "teur", "Fotoapparat", "es", "Fotoapparat", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_EXPENSIVE_CAMERA", "Fotoapparat", "es", "Fotoapparat", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_EXPENSIVE_CAMERA", "Photoapparat", "es", "Photoapparat", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_MIRROR","Spiegel","","Spiegel","","maskulin"),
    dekliniere_substantiv("NOUN_CRYSTAL_BALL", "Kristallkugel", "", "Kristallkugel", "en", "feminin"),
    dekliniere_substantiv("NOUN_LENSES", "Augenglas", "es", "Augengl�s", "er", "neutrum", ""),
    dekliniere_substantiv("NOUN_LENSES", "Kontaktlinse", "", "Kontaktlinse", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_BLINDFOLD", "Augenbinde",   "",   "Augenbinde",   "en", "feminin"),
    dekliniere_substantiv("NOUN_TOWEL", "Handtuch", "es", "Handt�ch", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SADDLE", "Sattel", "s", "S�ttel", "", "maskulin"),
    dekliniere_substantiv("NOUN_LEASH", "Leine", "", "Leine", "en", "feminin"),
    dekliniere_substantiv("NOUN_STETHOSCOPE", "Stethoskop", "s", "Stethoskop", "e", "neutrum"),
    dekliniere_substantiv("NOUN_TINNING_KIT", "Eindosungs-Set", "s", "Eindosungs-Set", "s", "neutrum"), # Eindosungs-Set, Dos-O-matic?, Eindos-O-matic?
    dekliniere_substantiv("NOUN_TIN_OPENER", "Dosen�ffner", "s", "Dosen�ffner", "", "maskulin"),
    dekliniere_substantiv("NOUN_TIN_OPENER", "B�chsen�ffner", "s", "B�chsen�ffner", "", "maskulin"),
    dekliniere_substantiv("NOUN_CAN_OF_GREASE", "Fettdose", "", "Fettdose", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_FIGURINE", "Statuette", "", "Statuette", "en", "feminin"),
    dekliniere_substantiv("NOUN_FIGURINE", "Figurine", "", "Figurine", "en", "feminin"),
    dekliniere_substantiv("NOUN_MAGIC_MARKER", "Zauberf�ller", "s", "Zauberf�ller", "", "maskulin"), # verzauberter Fueller? Zauberfueller?
    "/* 'Landmine' is a more generic term. I think 'Tretmine' is the more exact terminus for the mines in Nethack. */",
    dekliniere_substantiv("NOUN_LAND_MINE", "Tretmine", "", "Tretmine", "en", "feminin"),
    dekliniere_substantiv("NOUN_LAND_MINE", "Landmine", "", "Landmine", "en", "feminin"),
    dekliniere_substantiv("NOUN_BEARTRAP", "B�renfalle", "", "B�renfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_TIN_WHISTLE","Metallpfeife","","Metallpfeife","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIC_WHISTLE","Zauberpfeife","","Zauberpfeife","en","feminin"),
    dekliniere_substantiv("NOUN_WOODEN_FLUTE","Blockfl�te","","Blockfl�te","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIC_FLUTE","Zauberfl�te","","Zauberfl�te","en","feminin"),
    dekliniere_substantiv("NOUN_TOOLED_HORN", "Hifthorn", "es", "Hifth�rn", "er", "neutrum"), 
    dekliniere_substantiv("NOUN_FROST_HORN", "Eishorn", "es", "Eish�rn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_FIRE_HORN", "Feuerhorn", "es", "Feuerh�rn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_HORN_OF_PLENTY", "F�llhorn", "es", "F�llh�rn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_WOODEN_HARP","Lyra","","Lyren","fremder","feminin"),
    dekliniere_substantiv("NOUN_WOODEN_HARP","Holzharfe","","Holzharfe","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIC_HARP","Zauberharfe","","Zauberharfe","en","feminin"),
    dekliniere_substantiv("NOUN_BELL", "Gl�ckchen", "s", "Gl�ckchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_BELL", "Glocke", "s", "Glocke", "en", "feminin"),
    dekliniere_substantiv("NOUN_BUGLE", "Waldhorn", "es", "Waldh�rn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_LEATHER_DRUM", "Ledertrommel", "", "Ledertrommel", "en", "feminin"),
    dekliniere_substantiv("NOUN_DRUM_OF_EARTHQUAKE", "Erdbebentrommel", "", "Erdbebentrommel", "en", "feminin"),
    dekliniere_substantiv("NOUN_PICK_AXE", "Spitzhacke", "", "Spitzhacke", "en", "feminin"),
    dekliniere_substantiv("NOUN_GRAPPLING_HOOK", "Enterhaken","","","","maskulin"),
    dekliniere_substantiv("NOUN_UNICORN_HORN", "Ainkh�rn", "es", "Ainkh�rn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_UNICORN_HORN", "Einhorn-Horn", "es", "Einhorn-H�rn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_UNICORN_HORN", "Horn", "es", "H�rn", "er", "neutrum","","eines Einhornes"),
    dekliniere_substantiv("NOUN_UNICORN_HORN", "Horn", "es", "H�rn", "er", "neutrum","","eines Einhorns"),
    dekliniere_substantiv("NOUN_CANDELABRUM_OF_INVOCATION", "Kandelaber", "s", "Kandelaber", "", "maskulin","","der Anrufung"),
    dekliniere_substantiv("NOUN_BELL_OF_OPENING", "Gl�ckchen", "s", "Gl�ckchen", "", "neutrum", "", "der �ffnung"),
    "", 
    "/* Tools, unidentified */",
    dekliniere_substantiv("NOUN_BAG","Beutel","s","Beutel","","maskulin"),
    dekliniere_substantiv("NOUN_KEY","Schl�ssel","s","Schl�ssel","","maskulin"),
    dekliniere_substantiv("NOUN_CANDLE","Kerze","","Kerze","en","feminin"),
    dekliniere_substantiv("NOUN_LAMP","Lampe","","Lampe","en","feminin"),
    dekliniere_substantiv("NOUN_LOOKING_GLASS","Bronzeplatte","","Bronzeplatte","en","feminin"), # Bronzetablett, Silbertablett js
    dekliniere_substantiv("NOUN_GLASS_ORB","Glaskugel","","Glaskugel","en","feminin"),
    dekliniere_substantiv("NOUN_WHISTLE","Trillerpfeife","","Trillerpfeife","en","feminin"),
    dekliniere_substantiv("NOUN_FLUTE","Fl�te","","Fl�te","en","feminin"),
    dekliniere_substantiv("NOUN_HORN","Horn","es","H�rn","er","neutrum"),
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
    dekliniere_substantiv("NOUN_CORPSE","Leiche","","Leiche","en","feminin"),
    dekliniere_substantiv("NOUN_CORPSE","Leichnam","s","Leichnam","e","maskulin"),
    dekliniere_substantiv("NOUN_EGG","Ei","es","Ei","er","neutrum"),
    dekliniere_substantiv("NOUN_MEATBALL", "Hackb�llchen", "s", "Hackb�llchen", "", "neutrum"), # Kloesschen, Fleischklops, Frikadelle
    dekliniere_substantiv("NOUN_MEAT_STICK", "Salami", "", "Salami", "", "feminin"),
    dekliniere_substantiv("NOUN_HUGE_CHUNK_OF_MEAT", "Fleischberg", "es", "Fleischberg", "e", "maskulin"),
    dekliniere_substantiv("NOUN_MEAT_RING","Fleischring","es","Fleischring","e","maskulin"),
    "/* Kelpwedel, Kelpblatt? */",
    dekliniere_substantiv("NOUN_KELP_FROND","Seetangblatt","es","Seetangbl�tt","er","neutrum"),
    dekliniere_substantiv("NOUN_EUCALYPTUS_LEAF","Eukalyptusblatt","es","Eukalyptusbl�tt","er","neutrum"),
    dekliniere_substantiv("NOUN_APPLE","Apfel","s","�pfel","","maskulin"),
    dekliniere_substantiv("NOUN_ORANGE","Orange","","Orange","en","feminin"),
    dekliniere_substantiv("NOUN_ORANGE","Apfelsine","","Apfelsine","en","feminin"),
    dekliniere_substantiv("NOUN_PEAR","Birne","","Birne","en","feminin"),
    dekliniere_substantiv("NOUN_MELON","Melone","","Melone","en","feminin"),
    dekliniere_substantiv("NOUN_BANANA","Banane","","Banane","en","feminin"),
    dekliniere_substantiv("NOUN_CARROT","Karotte","","Karotte","en","feminin"),
    dekliniere_substantiv("NOUN_SPRIG_OF_WOLFSBANE","Wolfswurzzweig","es","Wolfswurzzweig","e","maskulin"),
    dekliniere_substantiv("NOUN_SPRIG_OF_WOLFSBANE","Wolfseisenhutzweig","es","Wolfseisenhutzweig","e","maskulin"),
    dekliniere_substantiv("NOUN_CLOVE_OF_GARLIC","Knoblauchzehe","","Knoblauchzehe","en","feminin"),
    dekliniere_substantiv("NOUN_SLIME_MOLD","Schleimpilz","es","Schleimpilz","e","maskulin"),
    dekliniere_substantiv("NOUN_LUMP_OF_ROYAL_JELLY", "St�ck", "es", "St�ck", "e", "neutrum", "", "Gel�e Royale"), # Gel�e Royale, Weiselfuttersaft K�niginfuttersaft, Bienenmutterbrei
    dekliniere_substantiv("NOUN_CREAM_PIE","Sahnetorte","","Sahnetorte","en","feminin"),
    dekliniere_substantiv("NOUN_CANDY_BAR","Schokoriegel","s","Schokoriegel","","maskulin"),
    dekliniere_substantiv("NOUN_FORTUNE_COOKIE","Gl�ckskeks","es","Gl�ckskeks","e","maskulin"),
    dekliniere_substantiv("NOUN_PANCAKE","Pfannkuchen","s","Pfannkuchen","","maskulin"),
    dekliniere_substantiv("NOUN_LEMBAS_WAFER","Lembasbrot","es","Lembasbrot","e","neutrum"),
    dekliniere_substantiv("NOUN_CRAM_RATION", "Ration", "", "Ration", "en", "feminin", "", "Cram"), # Zwergen-Lebensmittel aus HdR
    dekliniere_substantiv("NOUN_FOOD_RATION","Essensration","","Essensration","en","feminin"),
    dekliniere_substantiv("NOUN_K_RATION","Kampfration","","Kampfration","en","feminin"),
    dekliniere_substantiv("NOUN_C_RATION","Notration","","Notration","en","feminin"),
    dekliniere_substantiv("NOUN_TIN","Dose","","Dose","en","feminin"),
    "",
    "/* Potions, identified */",
    dekliniere_substantiv("NOUN_POT_GAIN_ABILITY", "Fertigkeitsgewinn", "es", "Fertigkeitsgewinn", "e", "maskulin"),
    dekliniere_substantiv("NOUN_POT_RESTORE_ABILITY", "Wiederherstellung", "", "Wiederherstellung", "en", "feminin"),
    dekliniere_substantiv("NOUN_POT_CONFUSION", "Verwirrung", "", "Verwirrung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_POT_BLINDNESS", "Blindheit", "", "", "", "feminin", "s"),
    dekliniere_substantiv("NOUN_POT_PARALYSIS", "L�hmung", "", "L�hmung", "en", "feminin", "s"), # der L�hmung, der Paralyse
    dekliniere_substantiv("NOUN_POT_SPEED", "Hast", "", "Hast", "en", "feminin"),
    dekliniere_substantiv("NOUN_POT_LEVITATION", "Levitation", "", "Levitation", "en", "feminin"), 
    dekliniere_substantiv("NOUN_POT_HALLUCINATION", "Halluzination", "", "Halluzination", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_POT_INVISIBILITY", "Unsichtbarkeit", "", "Unsichtbarkeit", "en", "feminin"),
    dekliniere_substantiv("NOUN_POT_SEE_INVISIBLE", "Enth�llung", "", "Enth�llung", "en", "feminin", "", "des Unsichtbaren"),
    dekliniere_substantiv("NOUN_POT_SEE_INVISIBLE", "Enth�llung", "", "Enth�llung", "en", "feminin"),
    dekliniere_substantiv("NOUN_POT_HEALING", "Linderung", "", "", "", "feminin", "s"),
    dekliniere_substantiv("NOUN_POT_EXTRA_HEALING", "Heilung", "", "Heilung", "en", "feminin", "s"),
    dekliniere_nominalphrase("NOUN_POT_FULL_HEALING", "vollst�ndig", "Heilung", "", "Heilung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_POT_GAIN_LEVEL", "Aufstieg", "s", "Aufstieg", "e", "maskulin"), # Stufenanstieg # Stufenaufstieg, Aufstieg js
    dekliniere_substantiv("NOUN_POT_ENLIGHTENMENT", "Erleuchtung", "", "Erleuchtung", "en", "feminin", "s"), # Erleuchtung, Klarheit?
    dekliniere_substantiv("NOUN_POT_MONSTER_DETECTION", "Monsterersp�rung", "", "Monsterersp�rung", "en", "feminin", "s"), # Monsterersp�ren, # Monsterentdecken, Monsterhellsicht Monstergespuer js
    dekliniere_substantiv("NOUN_POT_OBJECT_DETECTION", "Objektersp�rung", "", "Objektersp�rung", "en", "feminin", "s"), # Monsterersp�ren, # Monsterentdecken, Monsterhellsicht Monstergespuer js
    dekliniere_substantiv("NOUN_POT_GAIN_ENERGY", "Kraftgewinn", "s", "Kraftgewinn", "e", "maskulin"), # Energiegewinn?, Energiezunahme?, Energiegewinnung? # js
    dekliniere_substantiv("NOUN_POT_SLEEPING", "Schlaf", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_POT_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin", "s"), 
    dekliniere_substantiv("NOUN_POT_POLYMORPH", "Polymorphie", "", "", "en", "feminin"), 
    dekliniere_substantiv("NOUN_POT_BOOZE", "Schnaps", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_POT_BOOZE", "Schnapps", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_POT_BOOZE", "Fusel", "s", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_POT_SICKNESS", "�belkeit", "", "�belkeit", "en", "feminin"), # �belkeit ?
    dekliniere_substantiv("NOUN_POT_FRUIT_JUICE", "Fruchtsaft", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_POT_ACID", "S�ure", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_POT_OIL", "�l", "es", "", "e", "neutrum"), # �l, Brand�l ?
    dekliniere_substantiv("NOUN_POT_WATER", "Wasser", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_POT_HOLY_WATER", "Weihwasser", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_POT_UNHOLY_WATER", "Satanswasser", "s", "", "", "neutrum"),
    "",
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
    "/* Scrolls, identified */",
    dekliniere_substantiv("NOUN_SCR_ENCHANT_ARMOR", "R�stungszauber", "s", "", "en", "maskulin"), # Schriftrolle der verzauberten R�stung?, R�stungsverzauberung, R�stungszauber
    dekliniere_substantiv("NOUN_SCR_DESTROY_ARMOR", "R�stungszerst�rung", "", "", "en", "feminin"), # Schriftrolle der zerst�rten R�stung?, besch�digten R�stung? # js
    dekliniere_substantiv("NOUN_SCR_CONFUSE_MONSTER", "Monsterverwirrung", "", "Monsterverwirrung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SCR_SCARE_MONSTER", "Monsterschreck", "s", "", "en", "maskulin"), # Schriftrolle der Monsterfurcht, Monsterschreck(en), Monsterpanik
    dekliniere_substantiv("NOUN_SCR_REMOVE_CURSE", "Fluchbannung", "", "", "en", "feminin", "s"), # Schriftrolle des gebannten Fluches, des Fluchbannens, der Fluchbannung, Fluchbefreiung, Entfluchung,  netzhack: Entwuenschung
    # dekliniere_substantiv("NOUN_SCR_REMOVE_CURSE", "Entfluchung", "", "", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SCR_ENCHANT_WEAPON", "Waffenzauber", "s", "", "en", "maskulin"),  # Schriftrolle der verzauberten Waffe?, Waffenverzauberung, Waffenzauber
    dekliniere_substantiv("NOUN_SCR_CREATE_MONSTER", "Monsterbeschw�rung", "", "", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SCR_TAMING", "Z�hmung", "", "Z�hmung", "en", "feminin", "s"), # Z�hmens, Z�hmung
    dekliniere_substantiv("NOUN_SCR_GENOCIDE", "Genozid", "es", "", "e", "maskulin"), # genocide -> Genozid/Ausrottung, extinct -> ausgestorben
    dekliniere_substantiv("NOUN_SCR_GENOCIDE", "Ausrottung", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_SCR_LIGHT", "Licht", "es", "", "er", "neutrum"), 
    dekliniere_substantiv("NOUN_SCR_TELEPORTATION", "Teleportation", "", "", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SCR_GOLD_DETECTION", "Goldersp�rung", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_SCR_FOOD_DETECTION", "Nahrungsersp�rung", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_SCR_IDENTIFY", "Identifizierung", "", "", "en", "feminin", "s"),
    dekliniere_nominalphrase("NOUN_SCR_MAGIC_MAPPING", "magisch", "Kartieren", "s", "", "", "neutrum"), # Magiekartierung?
    dekliniere_substantiv("NOUN_SCR_AMNESIA", "Amnesie", "", "", "en", "feminin"), # netzhack: Vergessen
    dekliniere_substantiv("NOUN_SCR_FIRE", "Feuer", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SCR_EARTH", "Erde", "", "", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SCR_PUNISHMENT", "Bestrafung", "", "", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SCR_CHARGING", "Aufladung", "", "Aufladung", "en", "feminin"), # Laden, Ladungs?? # Aufladung js
    dekliniere_substantiv("NOUN_SCR_STINKING_CLOUD", "Gaswolke", "", "", "en", "feminin"), # Gaswolke? netzhack: Gestank
    #dekliniere_substantiv("NOUN_SCR_MAIL"
    dekliniere_substantiv("NOUN_SCR_MAIL", "Brief", "es", "", "e", "maskulin"),
    #dekliniere_substantiv("NOUN_SCR_BLANK_PAPER"
    dekliniere_nominalphrase("NOUN_SCR_BLANK_PAPER", "unbeschrieben", "Blatt", "es", "", "", "neutrum"),
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
    dekliniere_adjektiv("ADJEKTIV_SCR_STAMPED", "frankiert"),
    dekliniere_adjektiv("ADJEKTIV_SCR_UNLABELED", "unbeschriftet"),
    "",
    "/* Spellbooks, identified */",
    dekliniere_substantiv("NOUN_SPE_DIG", "Graben", "s", "", "", "neutrum"),
    dekliniere_nominalphrase("NOUN_SPE_MAGIC_MISSILE", "magisch", "Geschoss", "es", "Geschoss", "e", "neutrum"), # magische Geschosse
    dekliniere_substantiv("NOUN_SPE_FIREBALL", "Feuerball", "es", "Feuerb�ll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SPE_CONE_OF_COLD", "K�ltekegel", "s", "K�ltekegel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SPE_SLEEP", "Schlaf", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SPE_FINGER_OF_DEATH", "Finger", "s", "Finger", "", "maskulin", "", "des Todes"),
    dekliniere_substantiv("NOUN_SPE_LIGHT", "Licht", "es", "Licht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SPE_DETECT_MONSTERS", "Ersp�ren", "s", "", "", "neutrum", "", "von Monstern"),  # Monster finden, Monster ersp�ren
    dekliniere_substantiv("NOUN_SPE_HEALING", "Linderung", "", "", "", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_KNOCK", "Klopfen", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SPE_FORCE_BOLT", "Kraftsto�", "es", "Kraftst��", "e", "maskulin"), # Kraftstoss, Kraftschlag?, Energiestoss?
    dekliniere_substantiv("NOUN_SPE_CONFUSE_MONSTER", "Monsterverwirrung", "", "Monsterverwirrung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_CURE_BLINDNESS", "Kurieren", "s", "", "", "neutrum", "", "von Blindheit"), # Blindheitheilen
    dekliniere_substantiv("NOUN_SPE_DRAIN_LIFE", "Lebensentzug", "es", "", "e", "maskulin", "s"), # Lebensentzug, Blutsauger?
    dekliniere_substantiv("NOUN_SPE_SLOW_MONSTER", "Verlangsamung", "", "Verlangsamung", "en", "feminin", "s"), # Verlangsamung
    dekliniere_substantiv("NOUN_SPE_WIZARD_LOCK", "Zauberverschluss", "es", "Zauberverschl�ss", "e", "maskulin"), # Zauberverschluss
    dekliniere_substantiv("NOUN_SPE_CREATE_MONSTER", "Monsterbeschw�rung", "", "Monsterbeschw�rung", "en", "feminin"), # Monsterbeschw�rung
    dekliniere_substantiv("NOUN_SPE_DETECT_FOOD", "Ersp�ren", "s", "", "", "neutrum", "", "von Nahrung"), # Esswaren, Speise, Lebensmittel, Essen ersp�ren
    dekliniere_substantiv("NOUN_SPE_CAUSE_FEAR", "Furcht", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_SPE_CLAIRVOYANCE", "Hellsehen", "s", "", "", "neutrum"), # Hellsehen, Hellseherei?, Prophetie?
    dekliniere_substantiv("NOUN_SPE_CURE_SICKNESS", "Kurieren", "s", "", "", "neutrum", "", "von Erkrankungen"), # Krankheit/Erkrankung heilen/kurieren
    dekliniere_substantiv("NOUN_SPE_CHARM_MONSTER", "Monsterbezauberung", "", "Monsterbezauberung", "en", "feminin", "s"),    # Monster bezaubern
    dekliniere_substantiv("NOUN_SPE_HASTE_SELF", "Selbsthast", "", "Selbsthast", "en", "feminin"),
    dekliniere_substantiv("NOUN_SPE_DETECT_UNSEEN", "Ersp�ren", "s", "", "", "neutrum", "", "des Ungesehenen"),    # Ungesehenes ersp�ren
    dekliniere_substantiv("NOUN_SPE_LEVITATION", "Levitation", "", "Levitation", "en", "feminin"),
    dekliniere_substantiv("NOUN_SPE_EXTRA_HEALING", "Heilung", "", "Heilung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_RESTORE_ABILITY", "Wiederherstellen", "s", "", "", "neutrum", "", "von Fertigkeiten"),  # Fertigkeit wiederherstellen # IMPROVE ME
    dekliniere_substantiv("NOUN_SPE_INVISIBILITY", "Unsichtbarkeit", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_SPE_DETECT_TREASURE", "Ersp�ren", "s", "", "", "neutrum", "", "von Sch�tzen"),  # Sch�tze ersp�ren
    dekliniere_substantiv("NOUN_SPE_REMOVE_CURSE", "Fluchbannen", "s", "", "", "neutrum"),     # Fluchbannens
    dekliniere_nominalphrase("NOUN_SPE_MAGIC_MAPPING", "magisch", "Kartieren", "s", "", "", "neutrum"), # magisches Kartieren/Kartographieren?
    dekliniere_substantiv("NOUN_SPE_IDENTIFY", "Identifizierung", "", "Identifizierung", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_TURN_UNDEAD", "Untotenwandlung", "", "Untotenwandlung", "en", "feminin", "s"),      # Untotenwandlung, Untote umwandeln?
    dekliniere_substantiv("NOUN_SPE_POLYMORPH", "Transformation", "", "Transformation", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_POLYMORPH", "Polymorphie", "", "", "en", "feminin"),
    dekliniere_substantiv("NOUN_SPE_TELEPORT_AWAY", "Wegteleportieren", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SPE_TELEPORT_AWAY", "Teleportation", "", "", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_SPE_CREATE_FAMILIAR", "Haustierbeschw�rung", "", "Haustierbeschw�rung", "en", "feminin"), # ????beschw�rung # IMPROVE ME, gef�llt mir nicht so gut (Gef�hrte?)
    dekliniere_substantiv("NOUN_SPE_CANCELLATION", "L�schung", "", "L�schung", "en", "feminin", "s"),     # Annullierung, Entwertung, L�schung, Widerruf
    dekliniere_substantiv("NOUN_SPE_PROTECTION", "Schutz", "es", "Schutz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SPE_JUMPING", "Springen", "s", "Springen", "", "neutrum"),
    dekliniere_substantiv("NOUN_SPE_STONE_TO_FLESH", "Stein-zu-Fleisch", "", "", "", "maskulin"), # netzhack: Entsteinerung
    dekliniere_substantiv("NOUN_SPE_FLAME_SPHERE", "Flammenkugel", "", "Flammenkugel", "en", "feminin"), # Flammenkugel, Feuerkugel?, Flammensph�re?, Feuersph�re?
    dekliniere_substantiv("NOUN_SPE_FREEZE_SPHERE", "Frostkugel", "", "Frostkugel", "en", "feminin"),    # Frostkugel, Frostsph�re?
    dekliniere_nominalphrase("NOUN_SPE_BLANK_PAPER", "unbeschriftet", "Papier", "es", "Papier", "e", "neutrum"), # leeres Papier, unbeschriftetes Papier # FIX ME
    dekliniere_substantiv("NOUN_SPE_BOOK_OF_THE_DEAD", "Buch", "es", "B�ch", "er", "neutrum", "", "der Toten"), # "Das Buch der Toten"
    "",
    "/* Spellbooks, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_SPE_PARCHMENT", "pergamenten"), # pergamentartig?, Kandidat f�r 'aus Pergament'
    # dekliniere_substantiv("MADE_OF_SPE_VELLUM", "Velum", "s", "", "s", "neutrum"),
    # dekliniere_adjektiv("ADJEKTIV_SPE_VELLUM", "velin"), # Kandidat f�r 'aus Velum' # IMPROVE ME
    dekliniere_adjektiv("ADJEKTIV_SPE_VELLUM", "velinpapieren"), # Kandidat f�r 'aus Velum' # IMPROVE ME
    dekliniere_adjektiv("ADJEKTIV_SPE_RAGGED", "ausgefranst"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DOG_EARED", "eselsohrig"),
    dekliniere_adjektiv("ADJEKTIV_SPE_MOTTLED", "fleckig"),
    dekliniere_adjektiv("ADJEKTIV_SPE_STAINED", "beschmutzt"),
    # dekliniere_substantiv("MADE_OF_SPE_CLOTH", "Stoff", "es", "", "", "maskulin"),
    # dekliniere_adjektiv("ADJEKTIV_SPE_CLOTH", "stoffig"), # Kandidat f�r 'aus Stoff'
    dekliniere_adjektiv("ADJEKTIV_SPE_CLOTH", "stoffartig"), # Kandidat f�r 'aus Stoff'
    dekliniere_adjektiv("ADJEKTIV_SPE_LEATHER", "ledern"), # Kandidat f�r 'aus Leder'
    dekliniere_adjektiv("ADJEKTIV_SPE_WHITE", "wei�"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PINK", "rosarot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_RED", "rot"),
    dekliniere_adjektiv("ADJEKTIV_SPE_ORANGE", "orangen"), # Kandidat f�r unver�nderliche Konjugation
    dekliniere_adjektiv("ADJEKTIV_SPE_YELLOW", "gelb"),
    dekliniere_adjektiv("ADJEKTIV_SPE_VELVET", "samten"), # Kandidat f�r 'aus Samt'
    dekliniere_adjektiv("ADJEKTIV_SPE_LIGHT_GREEN", "hellgr�n"),
    dekliniere_adjektiv("ADJEKTIV_SPE_DARK_GREEN", "dunkelgr�n"),
    dekliniere_adjektiv("ADJEKTIV_SPE_TURQUOISE", "t�rkisfarben"), # Kandidat f�r unver�nderliche Konjugation
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
    dekliniere_adjektiv("ADJEKTIV_SPE_GLITTERING", "glei�end"), # LEO gibt gl�nzend,glanzvoll, glei�end, glitzernd, verlockend
    dekliniere_adjektiv("ADJEKTIV_SPE_SHINING", "gl�nzend"), # oder 'blank'?
    dekliniere_adjektiv("ADJEKTIV_SPE_DULL", "matt"), # oder 'stumpf' oder 'tr�be'?
    dekliniere_adjektiv("ADJEKTIV_SPE_THIN", "d�nn"),
    dekliniere_adjektiv("ADJEKTIV_SPE_THICK", "dick"),
    dekliniere_adjektiv("ADJEKTIV_SPE_CANVAS", "seiden"),
    dekliniere_adjektiv("ADJEKTIV_SPE_HARDCOVER", "gebunden"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PLAIN", "unbedruckt"),
    dekliniere_adjektiv("ADJEKTIV_SPE_PAPYRUS", "papyren"), # Kandidat f�r 'aus Papyrus'
    "",
    "/* Wands, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_WAND_GLASS", "gl�sern"),
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
    dekliniere_adjektiv("ADJEKTIV_WAND_STEEL", "st�hlern"),
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
    dekliniere_substantiv("NOUN_GOLD_PIECE","Goldst�ck","es","Goldst�ck","e","neutrum"),
    "",
    "/* Gems, identified */",
    dekliniere_substantiv("NOUN_GEM_DILITHIUM_CRYSTAL", "Dilithiumkristall", "es", "Dilithiumkristall", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_DIAMOND", "Diamant", "en", "Diamant", "en", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_RUBY", "Rubin", "s", "Rubin", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_JACINTH", "Hyazinth", "es", "Hyazinth", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_SAPPHIRE", "Saphir", "s", "Saphir", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_BLACK_OPAL", "Schwarzopal", "es", "Schwarzopal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_EMERALD", "Smaragd", "es", "Smaragd", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_TURQUOISE", "T�rkis", "es", "T�rkis", "e", "maskulin"),
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
    dekliniere_substantiv("NOUN_GEM_FLUORITE", "Flu�spat", "es", "Flu�spat", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_OBSIDIAN", "Obsidian", "s", "Obsidian", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_AGATE", "Achat", "es", "Achat", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_JADE", "Jadestein", "es", "Jadestein", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_GEM_WHITE_GLASS", ["wertlos","wei�"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_BLUE_GLASS", ["wertlos","blau"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_RED_GLASS", ["wertlos","rot"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_YELLOWISH_BROWN_GLASS", ["wertlos","gelblichbraun"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_ORANGE_GLASS", ["wertlos","orangen"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_YELLOW_GLASS", ["wertlos","gelb"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_BLACK_GLASS", ["wertlos","schwarz"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_GREEN_GLASS", ["wertlos","gr�n"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_VIOLET_GLASS", ["wertlos","violett"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_nominalphrase("NOUN_GEM_COLORED_GLASS", ["wertlos","gef�rbt"], "Glasst�ck", "es", "Glasst�ck", "e", "neutrum"),
    dekliniere_substantiv("NOUN_GEM_LUCKSTONE", "Gl�cksstein", "es", "Gl�cksstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_LOADSTONE", "Teufelsstein", "es", "Teufelsstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_TOUCHSTONE", "Pr�fstein", "es", "Pr�fstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_TOUCHSTONE", "Probierstein", "es", "Probierstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_FLINT", "Feuerstein", "es", "Feuerstein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GEM_ROCK", "Stein", "es", "Stein", "e", "maskulin"),
    "",
    "/* Gems, unidentified */",
    dekliniere_adjektiv("ADJEKTIV_GEM_WHITE", "wei�"),
    dekliniere_adjektiv("ADJEKTIV_GEM_RED", "rot"),
    dekliniere_adjektiv("ADJEKTIV_GEM_ORANGE", "orangen"),
    dekliniere_adjektiv("ADJEKTIV_GEM_BLUE", "blau"),
    dekliniere_adjektiv("ADJEKTIV_GEM_BLACK", "schwarz"),
    dekliniere_adjektiv("ADJEKTIV_GEM_GREEN", "gr�n"),
    dekliniere_adjektiv("ADJEKTIV_GEM_YELLOW", "gelb"),
    dekliniere_adjektiv("ADJEKTIV_GEM_YELLOWISH_BROWN", "gelblichbraun"),
    dekliniere_adjektiv("ADJEKTIV_GEM_VIOLET", "violett"),
    dekliniere_adjektiv("ADJEKTIV_GEM_GRAY", "grau"),
    "",
    "/* japanische Bezeichnungen */",
    dekliniere_substantiv("NOUN_JAP_WAKIZASHI", "Wakizashi", "", "Wakizashi", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_JAP_NINJA_TO", "Ninjato", "", "Ninjato", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_JAP_NUNCHAKU", "Nunchaku", "", "Nunchaku", "s", "neutrum"),
    dekliniere_substantiv("NOUN_JAP_NAGINATA", "Naginata", "", "Naginata", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_JAP_OSAKU", "Osaku", "", "Osaku", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_JAP_KOTO", "Koto", "", "Koto", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_JAP_SHITO", "Shito", "", "Shito", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_JAP_TANKO", "Tanko", "", "Tanko", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_JAP_KABUTO", "Kabuto", "", "Kabuto", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_JAP_YUGAKE", "Yugake", "", "Yugake", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_JAP_GUNYOKI", "Gunyoki", "", "Gunyoki", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_JAP_SAKE", "Sake", "", "", "fremder", "maskulin"),
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
    dekliniere_substantiv("NOUN_ACID_VENOM", "S�uregift","es","S�uregift","e","neutrum"),
    "",
    "/* ======================================================= */",
    "",
    "/* all monster names */",
    dekliniere_substantiv("NOUN_GIANT_ANT","Riesenameise","","Riesenameise","en","feminin","n"),
    dekliniere_substantiv("NOUN_KILLER_BEE","Killerbiene","","Killerbiene","en","feminin","n"),
    dekliniere_substantiv("NOUN_SOLDIER_ANT","Ameisensoldat","en","Ameisensoldat","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_FIRE_ANT","Feuerameise","","Feuerameise","en","feminin", "n"),
    dekliniere_substantiv("NOUN_GIANT_BEETLE", "Riesenk�fer", "s", "Riesenk�fer", "", "maskulin"),
    dekliniere_substantiv("NOUN_QUEEN_BEE","Bienenk�nigin","","Bienenk�niginn","en","feminin", "nen"),
    dekliniere_substantiv("NOUN_ACID_BLOB", "S�ureklumpen", "s", "S�ureklumpen", "", "maskulin"), # S�ureklumpen?, S�ureklotz??
    dekliniere_substantiv("NOUN_QUIVERING_BLOB", "Zitterklumpen", "s", "Zitterklumpen", "", "maskulin"), # zitternder/bebender Klumpen, Zitterklumpen
    dekliniere_substantiv("NOUN_BLOB", "Klumpen", "s", "Klumpen", "", "maskulin"),
    dekliniere_substantiv("NOUN_GELATINOUS_CUBE", "Gelatinew�rfel", "s", "Gelatinew�rfel", "", "maskulin"),
    dekliniere_substantiv("NOUN_CHICKATRICE", "Kikatrix", "", "Kikatrices", "fremder", "neutrum"), # Kikatrix? nach Adelung nieders�chsisch kiken/k�ken (angels�chsisch cicen, englisch chicken) f�r K�chlein (neuhochdeutsch K�ken)
    dekliniere_substantiv("NOUN_COCKATRICE", "Kokatrix", "", "Kokatrices", "fremder", "maskulin"), # Cocatrix? mittelalterliches Latein -> http://www.bartleby.com/61/43/C0444300.html Hollaendisch Slangdraak?
    dekliniere_substantiv("NOUN_PYROLISK", "Pyrolisk", "en", "Pyrolisk", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_JACKAL", "Schakal", "s", "Schakal", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FOX","Fuchs","es","F�chs","e","maskulin"),
    dekliniere_substantiv("NOUN_COYOTE","Kojote","en","Kojote","en","maskulin","n"),
    dekliniere_substantiv("NOUN_WEREJACKAL", "Werschakal", "s", "Werschakal", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_LITTLE_DOG", "klein", "Hund", "es", "Hund", "e", "maskulin","e"),
    dekliniere_substantiv("NOUN_DOG", "Hund", "es", "Hund", "e", "maskulin","e"),
    dekliniere_nominalphrase("NOUN_LARGE_DOG", "gro�", "Hund", "es", "Hund", "e", "maskulin","e"),
    dekliniere_substantiv("NOUN_DINGO","Dingo","s","Dingo","s","maskulin"),
    dekliniere_substantiv("NOUN_WOLF", "Wolf", "es", "W�lf", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_WEREWOLF","Werwolf","es","Werw�lf","e","maskulin", "s"),
    dekliniere_substantiv("NOUN_WARG", "Warg","", "Warg", "e", "maskulin"),
    dekliniere_adjektivisches_substantiv("NOUN_WINTER_WOLF_CUB", "Schneewolfjung", "neutrum"),
    dekliniere_substantiv("NOUN_WINTER_WOLF", "Schneewolf", "es", "Schneew�lf", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_HELL_HOUND_PUP", "H�llenhundwelpe", "en", "H�llenhundwelpe", "en", "maskulin","e"),
    dekliniere_substantiv("NOUN_HELL_HOUND", "H�llenhund", "es", "H�llenhund", "e", "maskulin","e"),
    dekliniere_substantiv("NOUN_CERBERUS", "Zerberus", "", "Zerberuss", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CERBERUS", "Kerberus", "", "Kerberuss", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CERBERUS", "Kerberos", "", "Kerbeross", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GAS_SPORE", "Gasspore", "", "Gasspore", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_FLOATING_EYE", "Schweb�ugler", "s", "Schweb�ugler", "", "maskulin"),
    dekliniere_substantiv("NOUN_FLOATING_EYE", "Schwebaug", "es", "Schwebaug", "en", "neutrum", "n"), # Schweb�ugler?, Schweb�uger?, was mit Ocellus?, nur�eugler, flug�ugler, flugauge,
    dekliniere_substantiv("NOUN_FREEZING_SPHERE", "Frostkugel", "", "Frostkugel", "en", "feminin"), # Frostkugel, Frostsph�re, Frostball
    dekliniere_substantiv("NOUN_FLAMING_SPHERE", "Flammenkugel", "", "Flammenkugel", "en", "feminin"), # Flammenkugel, Flammensph�re, Flammenball
    dekliniere_substantiv("NOUN_SHOCKING_SPHERE", "Elektrokugel", "", "Elektrokugel", "en", "feminin"), # Stosskugel?, Schlagkugel?, Stromkugel? Stromsph�re? Stromball? Elektrokugel?
    dekliniere_substantiv("NOUN_BEHOLDER", "Betrachter", "s", "Betrachter", "", "maskulin"), # Betrachter
    dekliniere_substantiv("NOUN_KITTEN", "K�tzchen", "s", "K�tzchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_HOUSECAT","Hauskatze","","Hauskatze","en","feminin","n"),
    dekliniere_substantiv("NOUN_JAGUAR","Jaguar","s","Jaguar","e","maskulin"),
    dekliniere_substantiv("NOUN_LYNX","Luchs","es","Luchs","e","maskulin"),
    dekliniere_substantiv("NOUN_PANTHER","Panther","s","Panther","","maskulin"),
    dekliniere_nominalphrase("NOUN_LARGE_CAT", "gro�", "Katze","","Katze","en","feminin","n"), # Grosskatze?
    dekliniere_substantiv("NOUN_TIGER","Tiger","s","Tiger","","maskulin"),
    dekliniere_substantiv("NOUN_GREMLIN", "Gremlin", "s", "Gremlin", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GARGOYLE", "Gargyle", "", "Gargyle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GARGOYLE", "Gargoyle", "s", "Gargoyle", "s", "maskulin","n"), # Gargoyle?, Gargil?, Gargula?, Gargyle?
    dekliniere_substantiv("NOUN_WINGED_GARGOYLE", "Fluggargyle", "", "Fluggargyle", "en", "feminin","n"),
    dekliniere_nominalphrase("NOUN_WINGED_GARGOYLE", "gefl�gelt", "Gargoyle", "s", "Gargoyle", "s", "maskulin","n"),
    dekliniere_substantiv("NOUN_HOBBIT", "Hobbit", "s", "Hobbit", "s", "maskulin"),
    dekliniere_substantiv("NOUN_DWARF","Zwerg","es","Zwerg","e","maskulin", "en"),
    dekliniere_substantiv("NOUN_BUGBEAR", "Grottenschrat","es","Grottenschrat","e","maskulin", "en"),
    dekliniere_substantiv("NOUN_BUGBEAR", "K�ferb�r","en","K�ferb�r","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_DWARF_LORD","Zwergenf�rst","en","Zwergenf�rst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_DWARF_KING","Zwergenk�nig","es","Zwergenk�nig","e","maskulin"),
    dekliniere_substantiv("NOUN_MIND_FLAYER", "Gedankendieb", "es", "Gedankendieb", "e", "maskulin", "s"), # Gedankenschinder, Gedankendieb
    dekliniere_substantiv("NOUN_MASTER_MIND_FLAYER", "Gedankenschinder", "s", "Gedankenschinder", "", "maskulin", "s"), # Superhirnschinder, Gedankenschinder
    dekliniere_substantiv("NOUN_MANES", "Mane", "en", "Mane", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HOMUNCULUS", "Homunkulus", "", "Homunkuli", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_HOMUNCULUS", "Homunculus", "", "Homunculi", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_IMP", "Imp", "", "Imp", "s", "maskulin"), # Teufelchen, Plagegeist? js 
    dekliniere_substantiv("NOUN_LEMURE", "Lemur", "en", "Lemur", "en", "maskulin"),
    dekliniere_substantiv("NOUN_QUASIT", "Quasit", "", "Quasit", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_TENGU", "Tengu", "", "Tengu", "fremder", "maskulin"),
    dekliniere_nominalphrase("NOUN_BLUE_JELLY", "blau", "Glibber", "s", "Glibber", "", "maskulin"), # Glibber, Gallert, Gelle
    dekliniere_nominalphrase("NOUN_SPOTTED_JELLY", "gesprenkelt", "Glibber", "s", "Glibber", "", "maskulin"), # gefleckt, gepunktet
    dekliniere_nominalphrase("NOUN_OCHRE_JELLY", "ockergelb", "Glibber", "s", "Glibber", "", "maskulin"), # ockergelb, beige, gelb, braun
    dekliniere_substantiv("NOUN_KOBOLD","Kobold","es","Kobold","e","maskulin", "s"),
    dekliniere_nominalphrase("NOUN_LARGE_KOBOLD","gro�", "Kobold","es","Kobold","e","maskulin", "s"),
    dekliniere_substantiv("NOUN_KOBOLD_LORD", "Koboldf�rst","en","Koboldf�rst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_KOBOLD_SHAMAN", "Koboldschamane", "en", "Koboldschamane", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_LEPRECHAUN", "Leprechaun", "", "Leprechaun", "", "maskulin", "en"), # Ampferwichtel?
    dekliniere_nominalphrase("NOUN_SMALL_MIMIC","klein", "Imitator","s","Imitator","en","maskulin", "en"),
    dekliniere_nominalphrase("NOUN_LARGE_MIMIC","gro�", "Imitator","s","Imitator","en","maskulin", "en"),
    dekliniere_nominalphrase("NOUN_GIANT_MIMIC","riesig", "Imitator","s","Imitator","en","maskulin", "en"),
		# Riesenimitator w�re etwas irref�hrend
    # dekliniere_substantiv("NOUN_GIANT_MIMIC", "Riesenimitator","","Riesenimitator","","maskulin", "en"),
    dekliniere_substantiv("NOUN_WOOD_NYMPH",     "Baumnymphe",   "", "Baumnymphe",   "en", "feminin","n"),
    dekliniere_substantiv("NOUN_WATER_NYMPH",    "Wassernymphe", "", "Wassernymphe", "en", "feminin","n"),
    dekliniere_substantiv("NOUN_MOUNTAIN_NYMPH", "Bergnymphe",   "", "Bergnymphe",   "en", "feminin","n"),
    "/* In reality goblin would be an appropriate translation for 'Kobold'. In tolkienspeek goblin is a synonym for orc.*/",
    dekliniere_substantiv("NOUN_GOBLIN", "Goblin", "s", "Goblin", "", "maskulin"),
    dekliniere_substantiv("NOUN_HOBGOBLIN", "Hobgoblin", "s", "Hobgoblin", "", "maskulin"),
    dekliniere_substantiv("NOUN_ORC","Ork","s","Ork","s","maskulin"),
    dekliniere_substantiv("NOUN_HILL_ORC","Bergork","s","Bergork","s","maskulin"),
    dekliniere_substantiv("NOUN_HILL_ORC","H�gelork","s","H�gelork","s","maskulin"),
    dekliniere_substantiv("NOUN_MORDOR_ORC","Mordorork","s","Mordorork","s","maskulin"),
    dekliniere_substantiv("NOUN_URUK_HAI","Uruk-Hai","","Uruk-Hai","fremder","maskulin"),
    dekliniere_substantiv("NOUN_ORC_SHAMAN", "Orkschamane", "en", "Orkschamane", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_ORC_CAPTAIN", "Orkkommandant", "en", "Orkkommandant", "en", "maskulin", "en"), # Orkkommandant/hauptmann
    dekliniere_substantiv("NOUN_ROCK_PIERCER", "Steinbohrer", "s", "Steinbohrer", "", "maskulin"),  # Steinbrecher?, Steinbohrer?, Steinschneider?
    dekliniere_substantiv("NOUN_IRON_PIERCER", "Eisenbohrer", "s", "Eisenbohrer", "", "maskulin"),  # Eisenbrecher?, Eisenbohrer?, Eisenschneider?
    dekliniere_substantiv("NOUN_GLASS_PIERCER", "Glasbohrer", "s", "Glasbohrer", "", "maskulin"), # Glasbrecher?, Glasbohrer?, Glasschneider?
    dekliniere_substantiv("NOUN_ROTHE", "Roth�", "", "Roth�", "fremder", "feminin"),
    dekliniere_substantiv("NOUN_MUMAK", "M�mak", "s", "M�makil", "fremder", "neutrum"), # Olifant, M�mak M�makil(pl)
    dekliniere_substantiv("NOUN_MUMAK", "Olifant", "en", "Olifant", "en", "maskulin"),
    dekliniere_substantiv("NOUN_LEOCROTTA", "Leocrotta", "", "Leocrottae", "fremder", "neutrum"),  # Leucrocotta? Leucrotta? Plinius 8.Buch der Natur
    dekliniere_substantiv("NOUN_WUMPUS", "Wumpus", "", "Wumpi", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_TITANOTHERE", "Titanotherium", "", "Titanotheria", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_BALUCHITHERIUM", "Baluchitherium", "", "Baluchitheria", "fremder", "neutrum"), # Paraceratherium, Baluchitherium, Indricotherium
    dekliniere_substantiv("NOUN_MASTODON", "Mastodon", "es", "Mastodon", "en", "neutrum"),
    dekliniere_substantiv("NOUN_SEWER_RAT", "Kanalratte", "", "Kanalratte", "en", "feminin", "n"),
    dekliniere_nominalphrase("NOUN_GIANT_RAT", "riesig", "Ratte", "", "Ratte", "en", "feminin", "n"),
    dekliniere_nominalphrase("NOUN_RABID_RAT", "tollw�tig", "Ratte", "", "Ratte", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WERERAT", "Werratte", "", "Werratte", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ROCK_MOLE", "Steinbei�er", "s", "Steinbei�er", "", "maskulin"),
    dekliniere_substantiv("NOUN_ROCK_MOLE", "Felsenbei�er", "s", "Felsenbei�er", "", "maskulin"),
    dekliniere_substantiv("NOUN_WOODCHUCK", "Murmeltier", "es", "Murmeltier", "e", "neutrum"),
    dekliniere_substantiv("NOUN_CAVE_SPIDER", "H�hlenspinne", "", "H�hlenspinne", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_CENTIPEDE", "Tausendf��ler", "s", "Tausendf��ler", "", "maskulin"),
    dekliniere_substantiv("NOUN_GIANT_SPIDER", "Riesenspinne", "", "Riesenspinne", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SCORPION", "Skorpion", "s", "Skorpion", "e", "maskulin","en"),
    dekliniere_substantiv("NOUN_LURKER_ABOVE", "Laurer", "s", "Laurer", "", "maskulin"),
    dekliniere_substantiv("NOUN_TRAPPER", "Fallensteller", "s", "Fallensteller", "", "maskulin"), 
    dekliniere_nominalphrase("NOUN_WHITE_UNICORN", "wei�", "Einhorn", "es", "Einh�rn", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_GRAY_UNICORN", "grau", "Einhorn", "es", "Einh�rn", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_BLACK_UNICORN", "schwarz", "Einhorn", "es", "Einh�rn", "er", "neutrum"),
    dekliniere_substantiv("NOUN_PONY","Pony","s","Pony","s","neutrum"),
    dekliniere_substantiv("NOUN_HORSE","Pferd","es","Pferd","e","neutrum", "e"),
    dekliniere_substantiv("NOUN_WARHORSE","Schlachtro�","es","Schlachtro�","e","neutrum"),
    dekliniere_substantiv("NOUN_FOG_CLOUD", "Nebelwolke", "", "Nebelwolke", "en", "feminin"),
    dekliniere_substantiv("NOUN_DUST_VORTEX", "Staubwirbel", "s", "Staubwirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_ICE_VORTEX", "Eiswirbel", "s", "Eiswirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_ENERGY_VORTEX", "Energiewirbel", "s", "Energiewirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_STEAM_VORTEX", "Dampfwirbel", "s", "Dampfwirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_FIRE_VORTEX", "Feuerwirbel", "s", "Feuerwirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_BABY_LONG_WORM", "Langw�rmchen", "s", "Langw�rmchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_PURPLE_WORM", "Purpurw�rmchen", "s", "Purpurw�rmchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_LONG_WORM", "Langwurm", "es", "Langw�rm", "er", "maskulin"),
    dekliniere_substantiv("NOUN_PURPLE_WORM", "Purpurwurm", "es", "Purpurw�rm", "er", "maskulin"),
    dekliniere_substantiv("NOUN_GRID_BUG", "Rastertaster", "s", "Rastertaster", "", "maskulin"), # Rastertaster, Matrixviech
    dekliniere_substantiv("NOUN_XAN", "Xan", "", "Xan", "", "maskulin"),
    dekliniere_substantiv("NOUN_YELLOW_LIGHT", "Gelblicht", "es", "Gelblicht", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_YELLOW_LIGHT", "gelb", "Licht", "es", "Licht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_BLACK_LIGHT", "Schwarzlicht", "es", "Schwarzlicht", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_BLACK_LIGHT", "schwarz", "Licht", "es", "Licht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_ZRUTY", "Zruty", "s", "Zruty", "s", "maskulin"),
    dekliniere_substantiv("NOUN_COUATL", "Coatl", "", "Coatl", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_ALEAX", "Aleax", "", "Aleax", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_ANGEL", "Engel", "s", "Engel", "", "maskulin"),
    dekliniere_substantiv("NOUN_KI_RIN", "Kirin", "s", "Kirin", "s", "neutrum"),
    dekliniere_substantiv("NOUN_ARCHON", "Archon", "s", "Archont", "en", "maskulin"),
    dekliniere_substantiv("NOUN_BAT", "Fledermaus", "", "Flederm�us", "e", "feminin"),
    dekliniere_substantiv("NOUN_GIANT_BAT", "Riesenfledermaus", "", "Riesenflederm�us", "e", "feminin"), # netzhack: Flughund? Andererseits sind Giant Bats Figuren aus d&d http://en.wikipedia.org/wiki/Giant_bat
    dekliniere_substantiv("NOUN_RAVEN", "Rabe", "en", "Rabe", "en", "maskulin"),
    dekliniere_substantiv("NOUN_VAMPIRE_BAT", "Vampirfledermaus", "", "Vampirflederm�us", "e", "feminin"),
    dekliniere_substantiv("NOUN_PLAINS_CENTAUR", "Steppenkentaur", "en", "Steppenkentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_FOREST_CENTAUR", "Waldkentaur", "en", "Waldkentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_MOUNTAIN_CENTAUR", "Bergkentaur", "en", "Bergkentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_BABY_GRAY_DRAGON", "Graudrachenbaby", "s", "Graudrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_SILVER_DRAGON", "Silberdrachenbaby", "s", "Silberdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_SHIMMERING_DRAGON", "Glanzdrachenbaby", "s", "Glanzdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_RED_DRAGON", "Rotdrachenbaby", "s", "Rotdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_WHITE_DRAGON", "Wei�drachenbaby", "s", "Wei�drachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_ORANGE_DRAGON", "Orangedrachenbaby", "s", "Orangedrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_BLACK_DRAGON", "Schwarzdrachenbaby", "s", "Schwarzdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_BLUE_DRAGON", "Blaudrachenbaby", "s", "Blaudrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_GREEN_DRAGON", "Gr�ndrachenbaby", "s", "Gr�ndrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_BABY_YELLOW_DRAGON", "Gelbdrachenbaby", "s", "Gelbdrachenbaby", "s", "neutrum"),
    dekliniere_substantiv("NOUN_GRAY_DRAGON", "Graudrache", "en", "Graudrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_SILVER_DRAGON", "Silberdrache", "en", "Silberdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_SHIMMERING_DRAGON", "Glanzdrache", "en", "Glanzdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_RED_DRAGON", "Rotdrache", "en", "Rotdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_WHITE_DRAGON", "Wei�drache", "en", "Wei�drache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_ORANGE_DRAGON", "Orangedrache", "en", "Orangedrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BLACK_DRAGON", "Schwarzdrache", "en", "Schwarzdrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_BLUE_DRAGON", "Blaudrache", "en", "Blaudrache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_GREEN_DRAGON", "Gr�ndrache", "en", "Gr�ndrache", "en", "maskulin", "n"),
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
    dekliniere_substantiv("NOUN_GREEN_MOLD", "Gr�nschimmel", "s", "Gr�nschimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_RED_MOLD", "Rotschimmel", "s", "Rotschimmel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SHRIEKER","Krakeeler", "s", "Krakeeler","", "maskulin"), # kreischen, schreien: Kreischer, Schreier, Schreihals, Krakeeler
    dekliniere_nominalphrase("NOUN_VIOLET_FUNGUS", "violett", "Pilz", "es", "Pilz", "e", "maskulin", ""),
    dekliniere_substantiv("NOUN_GNOME", "Gnom", "en", "Gnom", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_GNOME_LORD","Gnomenf�rst","en","Gnomenf�rst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_GNOMISH_WIZARD", "Gnomenzauberer", "s", "Gnomenzauberer", "", "maskulin"),
    dekliniere_substantiv("NOUN_GNOME_KING","Gnomenk�nig","es","Gnomenk�nig","e","maskulin"),
    dekliniere_substantiv("NOUN_GIANT",       "Riese", "en", "Riese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_STONE_GIANT", "Steinriese", "en", "Steinriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_HILL_GIANT",  "Bergriese", "en", "Bergriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_HILL_GIANT",  "H�gelriese", "en", "H�gelriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_FIRE_GIANT",  "Feuerriese", "en", "Feuerriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_FROST_GIANT", "Frostriese", "en", "Frostriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_STORM_GIANT", "Sturmriese", "en", "Sturmriese", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_ETTIN", "Ettin", "", "Ettin", "", "maskulin"),
    dekliniere_substantiv("NOUN_TITAN", "Titane", "en", "Titan", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_MINOTAUR","Minotaurus","","Minotaur","en","maskulin"),
    dekliniere_substantiv("NOUN_MINOTAUR","Minotaur","s","Minotaur","en","maskulin"),
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
    dekliniere_substantiv("NOUN_KOBOLD_MUMMY", "Koboldmumie", "", "Koboldmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GNOME_MUMMY", "Gnomenmumie", "", "Gnomenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ORC_MUMMY", "Orkmumie", "", "Orkmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_DWARF_MUMMY", "Zwergenmumie", "", "Zwergenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ELF_MUMMY", "Elbenmumie", "", "Elbenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_HUMAN_MUMMY", "Menschenmumie", "", "Menschenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_ETTIN_MUMMY", "Ettinmumie", "", "Ettinmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GIANT_MUMMY", "Riesenmumie", "", "Riesenmumie", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RED_NAGA_HATCHLING", "Rotnagaschl�pfling", "s", "Rotnagaschl�pfling", "s", "maskulin"), # Rotnagak�ken, Rotnagaschl�pfling
    dekliniere_substantiv("NOUN_BLACK_NAGA_HATCHLING", "Schwarznagaschl�pfling", "s", "Schwarznagaschl�pfling", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GOLDEN_NAGA_HATCHLING", "Goldnagaschl�pfling", "s", "Goldnagaschl�pfling", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GUARDIAN_NAGA_HATCHLING", "Schutznagaschl�pfling", "s", "Schutznagaschl�pfling", "s", "maskulin"),
    dekliniere_substantiv("NOUN_RED_NAGA", "Rotnaga", "s", "Rotnaga", "s", "feminin"), # Rotnaga, rote Naga, roter Naga?
    dekliniere_substantiv("NOUN_BLACK_NAGA", "Schwarznaga", "s", "Schwarznaga", "s", "feminin"), 
    dekliniere_substantiv("NOUN_GOLDEN_NAGA", "Goldnaga", "s", "Goldnaga", "s", "feminin"),
    dekliniere_substantiv("NOUN_GUARDIAN_NAGA", "Schutznaga", "s", "Schutznaga", "s", "feminin"), # W�chternaga, H�ternaga, Schutznaga
    dekliniere_substantiv("NOUN_NAGA", "Naga", "s", "Naga", "s", "feminin"), 
    dekliniere_substantiv("NOUN_OGRE", "Oger", "s", "Oger", "", "maskulin"),
    dekliniere_substantiv("NOUN_OGRE_LORD","Ogerf�rst","en","Ogerf�rst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_OGRE_KING","Ogerk�nig","es","Ogerk�nig","e","maskulin"),
    dekliniere_nominalphrase("NOUN_GRAY_OOZE", "Grau", "Schlamm", "es", "Schlamm", "e", "maskulin"), # grauer Schleim, Schlick?
    dekliniere_nominalphrase("NOUN_BROWN_PUDDING", "Braun", "Gr�tze", "", "Gr�tze", "en", "feminin"),
    dekliniere_substantiv("NOUN_BROWN_PUDDING", "Braunpudding", "s", "Braunpudding", "e", "feminin"), 
    dekliniere_nominalphrase("NOUN_BLACK_PUDDING", "Blutschwarz", "Gr�tze", "", "Gr�tze", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_BLACK_PUDDING", "Schwarz", "Gr�tze", "", "Gr�tze", "en", "feminin"),
    dekliniere_substantiv("NOUN_BLACK_PUDDING", "Schwarzpudding", "s", "Schwarzpudding", "e", "feminin"), 
    dekliniere_nominalphrase("NOUN_GREEN_SLIME", "Gr�n", "Schleim", "es", "Schleim", "e", "maskulin"),
    dekliniere_substantiv("NOUN_QUANTUM_MECHANIC", "Quantenteilchen", "s", "Quantenteilchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_RUST_MONSTER", "Rostmonster", "s", "Rostmonster", "", "neutrum"),
    dekliniere_substantiv("NOUN_DISENCHANTER", "Entzauberer", "s", "Entzauberer", "", "maskulin"),
    dekliniere_substantiv("NOUN_GARTER_SNAKE","Ringelnatter","","Ringelnatter","en","feminin"),
    dekliniere_substantiv("NOUN_SNAKE","Schlange","","Schlange","en","feminin", "n"),
    dekliniere_substantiv("NOUN_WATER_MOCCASIN","Wassermokassinotter", "", "Wassermokassinotter", "en", "feminin"),
    dekliniere_substantiv("NOUN_WATER_MOCCASIN","Wassermokassinschlange","","Wassermokassinschlange","en","feminin", "n"),
    dekliniere_substantiv("NOUN_PIT_VIPER", "Grubennatter", "", "Grubennatter", "en", "feminin"),
    dekliniere_substantiv("NOUN_PIT_VIPER", "Grubennotter", "", "Grubennotter", "en", "feminin"),
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
    dekliniere_substantiv("NOUN_VAMPIRE_LORD","Vampirf�rst","en","Vampirf�rst","en","maskulin", "en"),
    dekliniere_substantiv("NOUN_VAMPIRE_MAGE","Vampiermagier","s","Vampiermagier","","maskulin"),
    [unregelmaessiges_wort("NOUN_VLAD_THE_IMPALER", "Vlad der Pf�hler", $nom, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_VLAD_THE_IMPALER", "Vlad des Pf�hlers", $gen, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_VLAD_THE_IMPALER", "Vlad dem Pf�hler", $dat, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_VLAD_THE_IMPALER", "Vlad den Pf�hler", $akk, $mal, $sg)],
    dekliniere_substantiv("NOUN_BARROW_WIGHT", "Grabunhold", "es", "Grabunhold", "e", "maskulin"), # Grabunhold? , Wight: Gruftschrecken
    dekliniere_substantiv("NOUN_WRAITH", "Todesalb", "s", "Todesalb", "e", "maskulin"), # Todesalb, Schreckensalb
    dekliniere_substantiv("NOUN_NAZGUL","Nazg�l","","Nazg�l","","maskulin"),
    dekliniere_substantiv("NOUN_XORN","Xorn","","Xorn","","maskulin","en"),
    dekliniere_substantiv("NOUN_MONKEY", "�ffchen", "s", "�ffchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_APE", "Menschenaffe", "en", "Menschenaffe", "en", "maskulin"), # einer Menschenaffen stattdessen? Gibbon, Orang-Utan, Gorilla, Schimpanse. Gorilla scheint passend (ist auch der Vetegarischste der Menschenaffen)
    dekliniere_substantiv("NOUN_OWLBEAR", "Eulenb�r", "en", "Eulenb�r", "en", "maskulin"), # Netzhack: Uhlenb�r
    dekliniere_substantiv("NOUN_YETI","Yeti","s","Yeti","s","maskulin"),
    dekliniere_substantiv("NOUN_CARNIVOROUS_APE", "Blutaffe", "en", "Blutaffe", "en", "maskulin", "n"), # Blutaffe, Rei�zahngorilla js
    # dekliniere_substantiv("NOUN_SASQUATCH", "Sasquatch", "", "Sasquatch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SASQUATCH", "Sasquatch", "", "Sasquatches", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_KOBOLD_ZOMBIE", "Koboldzombie", "s", "Koboldzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GNOME_ZOMBIE", "Gnomenzombie", "s", "Gnomenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_ORC_ZOMBIE", "Orkzombie", "s", "Orkzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_DWARF_ZOMBIE", "Zwergenzombie", "s", "Zwergenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_ELF_ZOMBIE", "Elbenzombie", "s", "Elbenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HUMAN_ZOMBIE", "Menschenzombie", "s", "Menschenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_ETTIN_ZOMBIE", "Ettinzombie", "s", "Ettinzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GIANT_ZOMBIE", "Riesenzombie", "s", "Riesenzombie", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GHOUL", "Ghul", "s", "Ghul", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SKELETON", "Gerippe", "s", "Gerippe", "", "neutrum"),
    dekliniere_substantiv("NOUN_SKELETON", "Skelett", "es", "Skelett", "e", "neutrum"),
    dekliniere_substantiv("NOUN_STRAW_GOLEM", "Strohgolem", "s", "Strohgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_PAPER_GOLEM", "Papiergolem", "s", "Papiergolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_ROPE_GOLEM", "Seilgolem", "s", "Seilgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GOLD_GOLEM", "Goldgolem", "s", "Goldgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_LEATHER_GOLEM", "Ledergolem", "s", "Ledergolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_WOOD_GOLEM", "Holzgolem", "s", "Holzgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_FLESH_GOLEM", "Fleischgolem", "s", "Fleischgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_CLAY_GOLEM", "Lehmgolem", "s", "Lehmgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_CLAY_GOLEM", "Tongolem", "s", "Tongolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_STONE_GOLEM", "Steingolem", "s", "Steingolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GLASS_GOLEM", "Glasgolem", "s", "Glasgolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_IRON_GOLEM", "Eisengolem", "s", "Eisengolem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HUMAN", "Mensch", "en", "Mensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_ELF", "Elb", "es", "Elb", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_WOODLAND_ELF", "Waldelb", "es", "Waldelb", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_GREEN_ELF", "Gr�nelb", "es", "Gr�nelb", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_GREY_ELF", "Grauelb", "es", "Grauelb", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_ELF_LORD", "Elbenf�rst", "en", "Elbenf�rst", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_ELVENKING", "Elbenk�nig", "es", "Elbenk�nig", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_DOPPELGANGER", "Doppelg�nger", "s", "Doppelg�nger", "", "maskulin"),
    dekliniere_substantiv("NOUN_NURSE", "Krankenschwester", "", "Krankenschwester", "en", "feminin"),
    dekliniere_substantiv("NOUN_SHOPKEEPER", "Ladenbesitzer", "s", "Ladenbesitzer", "", "maskulin"), # Ladenbesitzer, Ladeninhaber, H�ndler?
    dekliniere_substantiv("NOUN_SHOPKEEPERIN", "Ladenbesitzerin", "", "Ladenbesitzerinn", "en", "feminin", "nen"), 
    dekliniere_substantiv("NOUN_GUARD", "Wache", "", "Wache", "en", "feminin"),
    dekliniere_adjektivisches_substantiv("NOUN_PRISONER", "Gefangen", "maskulin"),
    dekliniere_substantiv("NOUN_ORACLE", "Orakel", "s", "Orakel", "", "neutrum"),
    dekliniere_nominalphrase("NOUN_ALIGNED_PRIEST", "geweiht", "Priester", "s", "Priester", "", "maskulin"),
    dekliniere_substantiv("NOUN_HIGH_PRIEST", "Hohepriester", "s", "Hohepriester", "", "maskulin"),
    dekliniere_substantiv("NOUN_HIGH_PRIESTESS", "Hohepriesterin", "", "Hohepriesterinn", "en", "feminin","nen"),
    dekliniere_substantiv("NOUN_SOLDIER", "Soldat", "en", "Soldat", "en", "maskulin", "en"),
    "/* 'Korporal' exists as a military term in the Austrian 'Bundesheer' and\nthe Swiss Army. "+
    "'Leutnant' and 'Hauptmann' exist in these too and also in\nthe German 'Bundeswehr'.\n"+
    "Although not referring to the exactly same rank. */",
    dekliniere_substantiv("NOUN_SERGEANT", "Korporal", "s", "Korporal", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_LIEUTENANT", "Leutnant", "s", "Leutnant", "s", "maskulin", "s"),
    # dekliniere_substantiv("NOUN_CAPTAIN", "Hauptmann", "es", "Hauptm�nn", "er", "maskulin", "s"),
    dekliniere_substantiv("NOUN_CAPTAIN", "Hauptmann", "es", "Hauptleut", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_WATCHMAN", "Wachmann", "es", "Wachm�nn", "er", "maskulin", "s"),
    dekliniere_substantiv("NOUN_WATCH_CAPTAIN", "Wachkommandant", "en", "Wachkommandant", "en", "maskulin", "en"),
    dekliniere_eigenname("NOUN_MEDUSA", "Medusa", "feminin"),
    dekliniere_substantiv("NOUN_WIZARD_OF_YENDOR","Zauberer", "s", "Zauberer", "", "maskulin","","von Yendor"),
    dekliniere_eigenname("NOUN_CROESUS", "Kr�sus", "maskulin"),
    dekliniere_eigenname("NOUN_CHARON", "Charon", "maskulin"),
    dekliniere_substantiv("NOUN_GHOST", "Geist", "es", "Geist", "er", "maskulin", "er"),
    dekliniere_substantiv("NOUN_SHADE", "Nachtschatten", "s", "Nachtschatten", "", "maskulin"),
    dekliniere_substantiv("NOUN_WATER_DEMON", "Wasserd�mon", "s", "Wasserd�mon", "en", "maskulin"), # Wasserteufel, Wasserd�mon
    dekliniere_substantiv("NOUN_HORNED_DEVIL", "Hornteufel", "s", "Hornteufel", "", "maskulin"), # Hornteufel Malebranche
    dekliniere_substantiv("NOUN_SUCCUBUS", "Sukkubus", "", "Sukkuben", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_INCUBUS", "Incubus", "", "Incuben", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_ERINYS", "Erinnye", "", "Erinnye", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BARBED_DEVIL", "Dornteufel", "s", "Dornteufel", "", "maskulin"), # Klingenteufel, Stachelteufel, Dornteufel
    dekliniere_substantiv("NOUN_MARILITH", "Marilith", "", "Marilith", "fremder", "feminin"),
    dekliniere_substantiv("NOUN_VROCK", "Vrock", "", "Vrock", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_HEZROU","Hezrou", "", "Hezrou", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_BONE_DEVIL","Knochenteufel", "s", "Knochenteufel", "", "maskulin"),
    dekliniere_substantiv("NOUN_ICE_DEVIL","Eisteufel", "s", "Eisteufel", "", "maskulin"),
    dekliniere_substantiv("NOUN_NALFESHNEE","Nalfeshnee", "", "Nalfeshnee", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_PIT_FIEND", "Grubend�mon", "es", "Grubend�mon", "en", "maskulin", "en"), # H�llenschlundscheusal, Grubenunhold, Grubend�mon, Grubenteufel
    dekliniere_eigenname("NOUN_BALROG","Balrog","maskulin"),
    dekliniere_eigenname("NOUN_JUIBLEX","Juiblex","maskulin"),
    dekliniere_eigenname("NOUN_YEENOGHU","Yeenoghu","maskulin"),
    dekliniere_eigenname("NOUN_ORCUS", "Orkus", "maskulin"),
    dekliniere_eigenname("NOUN_ORCUS", "Orcus", "maskulin"),
    dekliniere_eigenname("NOUN_GERYON","Geryon","maskulin"),
    dekliniere_eigenname("NOUN_DISPATER","Dispater","maskulin"),
    dekliniere_eigenname("NOUN_BAALZEBUB","Beelzebub","maskulin"),
    dekliniere_eigenname("NOUN_BAALZEBUB","Baalzebub","maskulin"),
    dekliniere_eigenname("NOUN_ASMODEUS","Asmod�us","maskulin"),
    dekliniere_eigenname("NOUN_ASMODEUS","Asmodis","maskulin"),
    dekliniere_eigenname("NOUN_ASMODEUS","Asmodeus","maskulin"),
    dekliniere_eigenname("NOUN_DEMOGORGON", "Demogorgon", "maskulin"),
    dekliniere_eigenname("NOUN_DEATH","Tod","maskulin"),
    dekliniere_eigenname("NOUN_PESTILENCE","Pest","feminin"),
    dekliniere_eigenname("NOUN_FAMINE","Hunger","maskulin"),
    dekliniere_substantiv("NOUN_MAIL_DAEMON", "Briefd�mon", "s", "Briefd�mon", "en", "maskulin"),
    dekliniere_substantiv("NOUN_DJINNI", "Dschinn", "es", "Dschinn", "", "maskulin"),
    dekliniere_substantiv("NOUN_SANDESTIN","Sandestin", "", "Sandestin", "fremder", "maskulin"),
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
    dekliniere_substantiv("NOUN_CHAMELEON", "Cham�leon", "s", "Cham�leon", "s", "neutrum"),
    dekliniere_substantiv("NOUN_CROCODILE","Krokodil","s","Krokodil","e","neutrum","s"),
    dekliniere_substantiv("NOUN_SALAMANDER", "Salamander", "s", "Salamander", "", "maskulin"),
    dekliniere_substantiv("NOUN_LONG_WORM_TAIL","Langwurmschwanz","es","Langwurmschw�nz","e","maskulin"),
    dekliniere_substantiv("NOUN_ARCHAEOLOGE", "Arch�ologe", "en", "Arch�ologe", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_ARCHAEOLOGIN", "Arch�ologin", "", "Arch�ologinn", "en", "feminin"),
    dekliniere_substantiv("NOUN_BARBAR", "Barbar", "en", "Barbar", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_BARBARIN", "Barbarin", "", "Barbarinn", "en", "feminin"),
    dekliniere_substantiv("NOUN_HOEHLENMENSCH", "H�hlenmensch", "en", "H�hlenmensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_CAVEMAN", "H�hlenmensch", "en", "H�hlenmensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_CAVEWOMAN", "H�hlenmensch", "en", "H�hlenmensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_HEILER","Heiler","s","Heiler","","maskulin"),
    dekliniere_substantiv("NOUN_HEILERIN","Heilerin","","Heilerinn","en","feminin"),
    dekliniere_substantiv("NOUN_KNIGHT", "Paladin","s","Paladin","e","maskulin"),
    dekliniere_substantiv("NOUN_PALADIN","Paladin","s","Paladin","e","maskulin"),
    dekliniere_substantiv("NOUN_MONK",  "M�nch", "es", "M�nch", "e", "maskulin", "en"),
    dekliniere_substantiv("NOUN_MOENCH","M�nch", "es", "M�nch", "e", "maskulin", "en"),
    dekliniere_substantiv("NOUN_KANDIDATIN", "Kandidatin", "", "Kandidatinn", "en", "feminin"),
    dekliniere_substantiv("NOUN_PRIEST", "Priester", "s", "Priester", "", "maskulin"),
    dekliniere_substantiv("NOUN_PRIESTESS", "Priesterin", "", "Priesterinn", "en", "feminin","nen"),
    dekliniere_substantiv("NOUN_PRIESTER", "Priester", "s", "Priester", "", "maskulin"),
    dekliniere_substantiv("NOUN_PRIESTERIN", "Priesterin", "", "Priesterinn", "en", "feminin","nen"),
    dekliniere_substantiv("NOUN_RANGER","Waldl�ufer","s","Waldl�ufer","","maskulin"),
    dekliniere_substantiv("NOUN_WALDLAEUFER","Waldl�ufer","s","Waldl�ufer","","maskulin"),
    dekliniere_substantiv("NOUN_ROGUE","Dieb","es","Dieb","e","maskulin"),
    dekliniere_substantiv("NOUN_DIEB", "Dieb","es","Dieb","e","maskulin"),
    dekliniere_substantiv("NOUN_DIEBIN","Diebin","","Diebinn","en","feminin","nen"),
    dekliniere_substantiv("NOUN_SAMURAI", "Samurai", "", "Samurai", "", "maskulin"),
    dekliniere_substantiv("NOUN_SAMURAIIN", "Samurai", "", "Samurai", "", "feminin"),
    dekliniere_substantiv("NOUN_TOURIST", "Tourist", "en", "Tourist", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_TOURISTIN", "Touristin", "", "Touristinn", "en", "feminin", "nen"),
    dekliniere_substantiv("NOUN_VALKYRIE", "Walk�re", "", "Walk�re", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WIZARD","Magier","s","Magier","","maskulin"),
    dekliniere_eigenname("NOUN_LORD_CARNARVON","Lord Carnarvon","maskulin"),
    dekliniere_eigenname("NOUN_PELIAS","Pelias","maskulin"),
    [unregelmaessiges_wort("NOUN_SHAMAN_KARNOV", "Karnov der Schamane", $nom, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_SHAMAN_KARNOV", "Karnov des Schamanen", $gen, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_SHAMAN_KARNOV", "Karnov dem Schamanen", $dat, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_SHAMAN_KARNOV", "Karnov den Schamanen", $akk, $mal, $sg)],
    dekliniere_eigenname("NOUN_EARENDIL","E�rendil","maskulin"),
    dekliniere_eigenname("NOUN_ELWING","Elwing","feminin"),
    dekliniere_eigenname("NOUN_HIPPOCRATES","Hippokrates","maskulin"),
    dekliniere_eigenname("NOUN_KING_ARTHUR","K�nig Artus","maskulin"),
    dekliniere_substantiv("NOUN_GRAND_MASTER", "Gro�meister", "s", "Gro�meister", "", "maskulin"), 
    # dekliniere_substantiv("NOUN_ARCH_PRIEST", "Erzpriester", "s", "Erzpriester", "", "maskulin),
    dekliniere_substantiv("NOUN_ARCH_PRIEST", "Archipresbyter", "s", "Archipresbyter", "", "maskulin"),
    dekliniere_eigenname("NOUN_ORION", "Orion", "maskulin"),
    dekliniere_substantiv("NOUN_MASTER_OF_THIEVES", "K�nig", "s", "", "e", "maskulin","","der Diebe"), # Koenig der Diebe, Meister der Diebe? # netzhack: Der Meister der Diebesgilde
    dekliniere_eigenname("NOUN_LORD_SATO","Lord Sato","maskulin"), # F�rst Sato, netzhack: Sato-sama
    dekliniere_eigenname("NOUN_TWOFLOWER","Zweiblum","maskulin"),
    dekliniere_substantiv("NOUN_NORN","Norne", "", "Norn", "en", "feminin"), 
    [unregelmaessiges_wort("NOUN_NEFERET_THE_GREEN", "Neferet die Gr�ne", $nom, $fem, $sg)],
    [unregelmaessiges_wort("NOUN_NEFERET_THE_GREEN", "Neferet der Gr�nen", $gen, $fem, $sg)],
    [unregelmaessiges_wort("NOUN_NEFERET_THE_GREEN", "Neferet der Gr�nen", $dat, $fem, $sg)],
    [unregelmaessiges_wort("NOUN_NEFERET_THE_GREEN", "Neferet die Gr�ne", $akk, $fem, $sg)],
    dekliniere_substantiv("NOUN_MINION_OF_HUHETOTL","Mignon", "s", "Mignon", "", "maskulin","","des Huhetotl"), # netzhack: Lakai des Huetotl
    dekliniere_substantiv("NOUN_MINION_OF_HUHETOTL","Mignon", "s", "Mignon", "", "maskulin","","von Huhetotl"),
    dekliniere_substantiv("NOUN_MINION_OF_HUHETOTL","Mignon", "s", "Mignon", "", "maskulin","","Huhetotls"),
    dekliniere_eigenname("NOUN_THOTH_AMON","Thoth-Amon","maskulin"),
    dekliniere_nominalphrase("NOUN_CHROMATIC_DRAGON", "Chromatisch", "Drache", "en", "Drache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_GOBLIN_KING","Goblink�nig","es","Goblink�nig","e","maskulin"),
    dekliniere_substantiv("NOUN_CYCLOPS","Zyklop","en","Zyklop","en","maskulin","en"),
    dekliniere_substantiv("NOUN_CYCLOPS","Kyklop","en","Kyklop","en","maskulin","en"),
    dekliniere_eigenname("NOUN_IXOTH","Ixoth","maskulin"),
    dekliniere_eigenname("NOUN_MASTER_KAEN", "Meister Kaen", "maskulin"),
    dekliniere_eigenname("NOUN_NALZOK","Nalzok","maskulin"),
    dekliniere_eigenname("NOUN_SCORPIUS","Scorpius","maskulin"),
    dekliniere_substantiv("NOUN_MASTER_ASSASSIN", "Meister", "s", "", "", "maskulin", "", "der Assassinen"),
    dekliniere_eigenname("NOUN_ASHIKAGA_TAKAUJI", "Ashikaga Takauji", "maskulin"),
    dekliniere_eigenname("NOUN_LORD_SURTUR", "Lord Surtur", "maskulin"), # Surtur?
    dekliniere_substantiv("NOUN_DARK_ONE", "Finstere", "en", "", "en", "maskulin", "en"), # der Dunkle, der Duestere, der Lichtlose, der Schattenhafte, der Okkulte, netzhack: Finstere
    dekliniere_substantiv("NOUN_STUDENT", "Student", "en", "", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_CHIEFTAIN", "Stammesf�rst", "en", "Stammesf�rst", "en", "maskulin", "en"),
    dekliniere_eigenname("NOUN_NEANDERTHAL", "Neandertaler", "maskulin"),
    dekliniere_substantiv("NOUN_HIGH_ELF", "Hoch-Elb", "es", "Hoch-Elb", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_ATTENDANT", "Pfleger", "s", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_PAGE", "Page", "en", "", "en", "maskulin"),
    dekliniere_substantiv("NOUN_ABBOT", "Abt", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ACOLYTE", "Akolyth", "en", "", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HUNTER", "J�ger", "s", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_THUG", "Schl�ger", "s", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_NINJA", "Ninja", "", "", "s", "maskulin"),
    dekliniere_substantiv("NOUN_ROSHI", "Roshi", "", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_GUIDE", "Fremdenf�hrer", "s", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_WARRIOR", "Krieger", "s", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_APPRENTICE", "Lehrling", "s", "", "e", "maskulin"),
    "",
    "/* ======================================================= */",
    "/* drawing.c */",
    dekliniere_substantiv("NOUN_AIR","Luft","","L�ft","e","feminin","n"),
    dekliniere_substantiv("NOUN_ANT","Ameise","","Ameise","en","feminin","n"),
    dekliniere_substantiv("NOUN_ARACHNID","Spinnentier","es","Spinnentier","e","neutrum"),
    dekliniere_substantiv("NOUN_BEAR_TRAP", "B�renfalle", "", "B�renfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_BIRD","Vogel","s","V�gel","","maskulin",""),
    dekliniere_substantiv("NOUN_CAT","Katze","","Katze","en","feminin","n"),
    dekliniere_substantiv("NOUN_CLOUD","Wolke","","Wolke","en","neutrum","n"),
    dekliniere_substantiv("NOUN_CENTAUR", "Kentaur", "en", "Kentaur", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_CORRIDOR", "Korridor", "s", "Korridor", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DOORWAY", "Eingang", "es", "Eingang", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_DRAGON", "Drache", "en", "Drache", "en", "maskulin", "n"),
    dekliniere_substantiv("NOUN_FIRE_TRAP", "Feuerfalle", "", "Feuerfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_FALLING_ROCK_TRAP", "Steinschlagfalle", "", "Steinschlagfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_GOLEM", "Golem", "s", "Golem", "s", "maskulin"),
    dekliniere_substantiv("NOUN_GRAVE", "Grab", "es", "Gr�b", "er", "neutrum"),
    dekliniere_substantiv("NOUN_INSECT", "Insekt", "es", "Insekt", "en", "neutrum"),
    dekliniere_substantiv("NOUN_JELLY", "Glibber", "s", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_HOLE", "Loch", "es", "L�ch", "er", "neutrum"),
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
    dekliniere_substantiv("NOUN_PUDDING", "Gr�tze", "", "Gr�tze", "en", "feminin"),
    dekliniere_substantiv("NOUN_POLYMORPH_TRAP", "Transformationsfalle", "", "Transformationsfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_POLYMORPH_TRAP", "Polymorphiefalle", "", "Polymorphiefalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_QUADRUPED", "Vierbeiner", "s", "Vierbeiner", "", "maskulin"),
    #dekliniere_substantiv("NOUN_ROCK", "Stein", "es", "Stein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RODENT", "Nagetier", "es", "Nagetier", "e", "neutrum"),
    dekliniere_substantiv("NOUN_ROLLING_BOULDER_TRAP", "Felskugelfalle", "", "Felskugelfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_RUST_TRAP", "Rostfalle", "", "Rostfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SEA_MONSTER", "Seemonster", "s", "Seemonster", "", "neutrum"),
    dekliniere_substantiv("NOUN_SINK", "Sp�le", "", "Sp�le", "en", "feminin"),
    dekliniere_substantiv("NOUN_SLEEPING_GAS_TRAP", "Schlafgasfalle", "", "Schlafgasfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SPHERE", "Kugel", "", "Kugel", "en", "feminin"),
    dekliniere_substantiv("NOUN_SPIKED_PIT", "Stachelgrube", "", "Stachelgrube", "en", "feminin"),
    dekliniere_substantiv("NOUN_TELEPORTATION_TRAP", "Teleportationsfalle", "", "Teleportationsfalle", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_TRAP_DOOR", "Fallt�re", "", "Fallt�re", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_TREE", "Baum", "es", "B�um", "e", "maskulin", "n"),
    dekliniere_substantiv("NOUN_UNICORN", "Einhorn", "es", "Einh�rn", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_USEFUL_ITEM", "n�tzlich", "Ger�t", "es", "Ger�t", "e", "neutrum"),
    dekliniere_substantiv("NOUN_VORTEX", "Wirbel", "s", "Wirbel", "", "maskulin"),
    dekliniere_substantiv("NOUN_WATER", "Wasser", "s", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_ZOMBIE", "Zombie", "s", "Zombie", "s", "maskulin"),
    "",
    "/* ========== */",
    "/* Artefakte  */",
    "/* ========== */",
    dekliniere_substantiv("NOUN_EXCALIBUR","Excalibur","s","","","neutrum"),
    dekliniere_substantiv("NOUN_STORMBRINGER","Sturmbringer","s","","","maskulin"),
    dekliniere_substantiv("NOUN_MJOLLNIR","Mj�llnir","s","","","maskulin"),
    dekliniere_substantiv("NOUN_MJOLLNIR","Mj�lnir","s","","","maskulin"),
    dekliniere_substantiv("NOUN_CLEAVER","Fleischspalter","s","","","maskulin"), # Fleicherbeil, Hackbeil, Fleischspalter
    dekliniere_substantiv("NOUN_GRIMTOOTH","Grimmzahn","es","","","maskulin"),
    dekliniere_substantiv("NOUN_ORCRIST","Orkrist","s","","","neutrum"),
    dekliniere_substantiv("NOUN_ORCRIST","Orcrist","s","","","neutrum"),
    dekliniere_substantiv("NOUN_STING","Stich","s","","","maskulin"),
    dekliniere_substantiv("NOUN_MAGICBANE","Magiebrecher","s","","","maskulin"), # Magiet�ter, Magiebann, Magiebanner, Magiest�rer, Magiezerst�rer, Magieentkr�fter, Magieaufreiber, Magieauslauger, Magieverzehrer, Magiebrecher, Magieberster, Magiekrepierer, Magielazerator, Magienekator; netzhack: Magiefluch; xororand: Magieverderber, Magiezerm�rber, Magietormentor
    dekliniere_substantiv("NOUN_FROST_BRAND","Frostklinge","","","","feminin"),
    dekliniere_substantiv("NOUN_FROST_BRAND","Frostbrand","es","","","maskulin"),
    dekliniere_substantiv("NOUN_FIRE_BRAND","Flammenklinge","","","","feminin"), # Flammenklinge, Flammenschwert, Feuerklinge, Feuerschwert
    dekliniere_substantiv("NOUN_FIRE_BRAND","Feuerbrand","es","","","maskulin"),
    dekliniere_substantiv("NOUN_DRAGONBANE","Drachent�ter","s","","","maskulin"),
    dekliniere_substantiv("NOUN_DEMONBANE","D�monent�ter","s","","","maskulin"),
    dekliniere_substantiv("NOUN_WEREBANE","Wert�ter","s","","","maskulin"),
    dekliniere_substantiv("NOUN_GRAYSWANDIR","Grayswandir","s","","","neutrum"),
    dekliniere_substantiv("NOUN_GIANTSLAYER","Riesenschl�chter","s","","","maskulin"), # Riesenschl�chter, Riesenj�ger
    dekliniere_substantiv("NOUN_OGRESMASHER","Ogermantscher","s","","","maskulin"),
    dekliniere_substantiv("NOUN_TROLLSBANE","Trollt�ter","s","","","maskulin"),
    dekliniere_substantiv("NOUN_VORPAL_BLADE","Vorpahlklinge","","","","feminin"),
    dekliniere_substantiv("NOUN_VORPAL_BLADE","Vorpalklinge","","","","feminin"),
    dekliniere_substantiv("NOUN_SNICKERSNEE","Schnippelschnipp","es","","","neutrum"), # EAV
    dekliniere_substantiv("NOUN_SNICKERSNEE","Snickersnee","","","","neutrum"),
    dekliniere_substantiv("NOUN_SUNSWORD","Sonnenschwert","es","","","neutrum"),
    dekliniere_substantiv("NOUN_THE_ORB_OF_DETECTION","Kristallkugel","","","","feminin","","der Enth�llung"),
    dekliniere_substantiv("NOUN_THE_HEART_OF_AHRIMAN","Herz","ens","","","neutrum","","von Ahriman"),
    dekliniere_substantiv("NOUN_THE_SCEPTRE_OF_MIGHT","Szepter","s","","","neutrum","","der Macht"),
    dekliniere_substantiv("NOUN_THE_SCEPTRE_OF_MIGHT","Zepter","s","","","neutrum","","der Macht"),
    dekliniere_substantiv("NOUN_THE_STAFF_OF_AESCULAPIUS","Stab","es","","","maskulin","","des Asklepios"),
    dekliniere_substantiv("NOUN_THE_STAFF_OF_AESCULAPIUS","Stab","es","","","maskulin","","des �skulap"),
    dekliniere_substantiv("NOUN_THE_MAGIC_MIRROR_OF_MERLIN","Zauberspiegel","","","","maskulin","","Merlins"),
    dekliniere_substantiv("NOUN_THE_EYES_OF_THE_OVERWORLD","Auge","es","Auge","en","neutrum","","der �berwelt"),
    dekliniere_substantiv("NOUN_THE_MITRE_OF_HOLINESS","Mitra","","","","feminin","","der Heiligkeit"),
    dekliniere_substantiv("NOUN_THE_LONGBOW_OF_DIANA","Langbogen","s","","","maskulin","","der Diana"),
    dekliniere_substantiv("NOUN_THE_MASTER_KEY_OF_THIEVERY","Generalschl�ssel", "s", "", "", "maskulin","","der Dieberei"), # netzhack Meisterschl�ssel der Dieberey
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
    dekliniere_substantiv("NOUN_OBJ_DOOR", "T�re", "", "T�re", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_DOOR", "T�re", "", "T�re", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WALLET", "Geldb�rse", "", "Geldb�rse", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_MEAT", "Fleisch", "es", "", "e", "neutrum", ""),
    dekliniere_substantiv("NOUN_LUCK", "Gl�ck", "es", "Gl�ck", "e", "neutrum", "s"),
    dekliniere_substantiv("NOUN_PAAR", "Paar", "es", "Paar", "e", "neutrum"), 
    dekliniere_substantiv("NOUN_PAAR", "Paar", "es", "Paar", "e", "neutrum"), 
    dekliniere_substantiv("NOUN_BOOTS", "Stiefel", "s", "Stiefel", "", "maskulin"), 
    dekliniere_substantiv("NOUN_GLOVES", "Handschuh", "es", "Handschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CLOAK", "Umhang", "es", "Umh�ng", "e", "maskulin", "s"),
    dekliniere_substantiv("NOUN_HELMET", "Helm", "es", "Helm", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SAFT", "Saft", "es", "S�ft", "e", "maskulin"), 
    dekliniere_substantiv("NOUN_SHIELD", "Schild", "es", "Schild", "e", "maskulin"), 
    #dekliniere_substantiv("NOUN_STONE", "Stein", "es", "Stein", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SWORD", "Schwert", "es", "Schwert", "er", "neutrum"),
    dekliniere_substantiv("NOUN_SPELL", "Spruch", "es", "Spr�ch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_MEDALLION", "Medaillon", "s", "Medaillon", "s", "neutrum"),
    "",
    dekliniere_substantiv("NOUN_DRESSING_MANEUVER", "Bekleidungsakt", "es", "Bekleidungsakt", "e", "maskulin"),
    dekliniere_substantiv("NOUN_QUICKNESS", "Schnelligkeit", "", "Schnelligkeit", "en", "feminin", "s"),
    dekliniere_substantiv("NOUN_BLUT", "Blut", "es", "", "", "neutrum"),
    dekliniere_substantiv("NOUN_SAEURE", "S�ure", "", "S�ure", "en", "feminin"),
    dekliniere_substantiv("NOUN_RICHTUNG", "Richtung", "", "Richtung", "en", "feminin"),
    dekliniere_substantiv("NOUN_BILD", "Bild", "es", "Bild", "er", "neutrum"),
    "",
    dekliniere_substantiv("NOUN_BOLT", "Blitz", "es", "Blitz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BLITZ", "Blitz", "es", "Blitz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BOLT_OF_FIRE", "Feuerblitz", "es", "Feuerblitz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BOLT_OF_COLD", "K�lteblitz", "es", "K�lteblitz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BOLT_OF_LIGHTNING", "Blitzschlag", "es", "Blitzschl�g", "e", "maskulin"),
    dekliniere_substantiv("NOUN_RAY", "Strahl", "es", "Strahl", "en", "maskulin"),
    dekliniere_nominalphrase("NOUN_MAGIC_MISSILE", "magisch", "Geschoss", "es", "Geschoss", "e", "neutrum"),
    dekliniere_substantiv("NOUN_SLEEP_RAY", "Schlafstrahl", "es", "Schlafstrahl", "en", "maskulin"),
    dekliniere_substantiv("NOUN_DEATH_RAY", "Todesstrahl", "es", "Todesstrahl", "en", "maskulin"),
    dekliniere_substantiv("NOUN_FIREBALL", "Feuerball", "es", "Feuerb�ll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_CONE_OF_COLD", "K�ltekegel", "s", "K�ltekegel", "", "maskulin"),
    dekliniere_substantiv("NOUN_FINGER_OF_DEATH", "Finger", "s", "Finger", "", "maskulin", "", "des Todes"),
    "",
    dekliniere_substantiv("NOUN_HALS", "Hals", "es", "H�ls", "e", "maskulin"),
    dekliniere_substantiv("NOUN_BODY", "K�rper", "s", "K�rper", "", "maskulin"),
    dekliniere_substantiv("NOUN_HAUT", "Haut", "", "H�ut", "e", "feminin"),
    "",
    dekliniere_substantiv("NOUN_VERSUCH", "Versuch", "es", "Versuch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FINGER", "Finger", "s", "Finger", "", "maskulin"),
    dekliniere_substantiv("NOUN_HAND", "Hand", "", "H�nd", "e", "feminin"),
    dekliniere_substantiv("NOUN_HEAD", "Kopf", "es", "K�pf", "e", "maskulin"),
    dekliniere_substantiv("NOUN_LEG", "Bein", "es", "Bein", "e", "neutrum"),
    dekliniere_substantiv("NOUN_FACE", "Gesicht", "es", "Gesicht", "er", "neutrum"),
    dekliniere_substantiv("NOUN_FOOT", "Fu�", "es", "F��", "e", "maskulin"),
    "",
    dekliniere_substantiv("NOUN_DUST", "Staub", "es", "Staub", "e", "maskulin"),
    dekliniere_substantiv("NOUN_FLOOR", "Boden", "s", "B�den", "", "maskulin"),
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
    dekliniere_substantiv("NOUN_THRONE", "Thron", "es", "Thron", "e", "maskulin"),
    dekliniere_substantiv("NOUN_DRAWBRIDGE", "Zugbr�cke", "", "Zugbr�cke", "en", "feminin"),
    dekliniere_substantiv("NOUN_KLINGE", "Klinge", "", "Klinge", "en", "feminin"),
    dekliniere_substantiv("NOUN_LAVA", "Lava", "", "Lav", "en", "feminin", ""),
    dekliniere_substantiv("NOUN_BLICK", "Blick", "es", "Blick", "e", "maskulin"),
    dekliniere_substantiv("NOUN_STUFE", "Stufe", "", "Stufe", "en", "feminin"),
    dekliniere_substantiv("NOUN_MOVEMENT", "Bewegung", "", "Bewegung", "en", "feminin"),
    dekliniere_substantiv("NOUN_SCHLAG", "Schlag", "es", "Schl�g", "e", "maskulin"),
    dekliniere_substantiv("NOUN_STUHL", "Stuhl", "es", "St�hl", "e", "maskulin"),
    dekliniere_substantiv("NOUN_MENSCH", "Mensch", "en", "Mensch", "en", "maskulin", "en"),
    dekliniere_substantiv("NOUN_RAT", "Ratte", "", "Ratte", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_PURSE", "Portemonnaie", "s", "Portemonnaie", "s", "neutrum", ""),
    "",
    dekliniere_substantiv("NOUN_ERKRANKUNG", "Erkrankung", "", "Erkrankung", "en", "feminin", ""),
    dekliniere_substantiv("NOUN_ERSCHOEPFUNG", "Ersch�pfung", "", "Ersch�pfung", "en", "feminin", ""),
    dekliniere_substantiv("NOUN_HUNGERTOD", "Hungertod", "es", "Hungertod", "e", "maskulin", ""),
    
    dekliniere_substantiv("NOUN_ALTAR","Altar","es","Alt�r","e","maskulin"),
    dekliniere_substantiv("NOUN_FOUNTAIN","Springbrunnen","s","Springbrunnen","","maskulin"),
    dekliniere_substantiv("NOUN_HEADSTONE","Grabstein","es","Grabstein","e","maskulin"),

    dekliniere_substantiv("NOUN_HEADSCHMERZEN","Kopfschmerz","es","Kopfschmerz","en","maskulin"),
    dekliniere_substantiv("NOUN_VERLUST","Verlust","es","Verlust","e","feminin"),
    dekliniere_substantiv("NOUN_SPENDE","Spende","","Spende","en","feminin"),
    dekliniere_substantiv("NOUN_KNACKEN","Knacken","s","Knacken","","neutrum"),
    dekliniere_substantiv("NOUN_GESTALT","Gestalt","","Gestalt","en","feminin"),

		dekliniere_substantiv("NOUN_ARM","Arm","es","Arm","e","maskulin"),
		dekliniere_substantiv("NOUN_BRIDGE","Br�cke","","Br�cke","en","feminin"),
		dekliniere_substantiv("NOUN_CEILING","Decke","","Decke","en","feminin"),
		dekliniere_substantiv("NOUN_CHEMIE","Chemie","","Chemie","en","feminin"),
		dekliniere_substantiv("NOUN_EYE","Auge","es","Auge","en","neutrum"),
		dekliniere_substantiv("NOUN_FASSUNG","Fassung","","Fassung","en","feminin"),
		dekliniere_substantiv("NOUN_FEUER","Feuer","s","Feuer","","neutrum"),
		dekliniere_substantiv("NOUN_FINGERTIP","Fingerspitze","","Fingerspitze","en","feminin"),
		dekliniere_substantiv("NOUN_SCHNEE","Schnee","s","","","maskulin"),
		dekliniere_substantiv("NOUN_GEBET","Gebet","s","Gebet","e","neutrum"),
		dekliniere_substantiv("NOUN_GEGENWART","Gegenwart","","","","feminin"),
		dekliniere_substantiv("NOUN_GESCHMACK","Geschmack","es","Geschm�ck","e","maskulin"),
		dekliniere_substantiv("NOUN_GOLD","Gold","es","","","neutrum"),
		dekliniere_substantiv("NOUN_GRABWERKZEUG","Grabwerkzeug","es","Grabwerkzeug","e","neutrum"),
		dekliniere_substantiv("NOUN_HERZSCHLAG","Herzschlag","es","Herzschl�g","e","maskulin"),
		dekliniere_substantiv("NOUN_HERZ","Herz","ens","Herz","en","neutrum"),
		dekliniere_substantiv("NOUN_KNIE","Knie","s","Knie","","neutrum"),
		dekliniere_substantiv("NOUN_LEITUNGSWASSER", "Leitungswasser", "s", "", "", "neutrum"),
		dekliniere_substantiv("NOUN_LIGHT","Licht","es","Licht","er","neutrum"),
		dekliniere_substantiv("NOUN_LICHT","Licht","es","Licht","er","neutrum"),
		dekliniere_substantiv("NOUN_LOCH","Loch","es","L�ch","er","neutrum"),
		dekliniere_substantiv("NOUN_MUND","Mund","es","M�nd","er","maskulin"),
		dekliniere_substantiv("NOUN_PASSAGE","Korridor","s","Korridor","e","maskulin"),
		dekliniere_substantiv("NOUN_SACRIFICE","Opfer","s","Opfer","","neutrum"),
		dekliniere_substantiv("NOUN_SCHEPPERN","Scheppern","s","","","neutrum"),
		dekliniere_substantiv("NOUN_SEHKRAFT","Sehkraft","","","","feminin"),
		dekliniere_substantiv("NOUN_SKY","Himmel","s","","","maskulin"),
		dekliniere_substantiv("NOUN_SPINE","R�cken","s","R�cken","","maskulin"),
		dekliniere_substantiv("NOUN_STIMME","Stimme","","Stimme","en","feminin"),
		dekliniere_substantiv("NOUN_UMGEBUNG","Umgebung","","Umgebung","en","feminin"),
		dekliniere_substantiv("NOUN_ZEITLUPENTRITT","Zeitlupentritt","es","Zeitlupentritt","e","maskulin"),
		dekliniere_substantiv("NOUN_WOLKE","Wolke","","Wolke","en","feminin"),
		dekliniere_substantiv("NOUN_RAUCHWOLKE","Rauchwolke","","Rauchwolke","en","feminin"),
		dekliniere_substantiv("NOUN_RAUCHWOLKE","Rauchwolke","","Rauchwolke","en","feminin"),
		dekliniere_substantiv("NOUN_DUNST","Dunst","es","D�nst","e","feminin"),
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
		dekliniere_substantiv("NOUN_NAEHE","N�he","","","","feminin"),
		dekliniere_substantiv("NOUN_GEISTESSCHAERFE","Geistessch�rfe","","Geistessch�rfe","en","feminin"),
		dekliniere_substantiv("NOUN_FRESSLUST","Fresslust","","Fressl�st","e","feminin"),
		dekliniere_substantiv("NOUN_VERBUNDENHEIT","Verbundenheit","","Verbundenheit","en","feminin"),
		dekliniere_substantiv("NOUN_SCHULD","Schuld","","Schuld","en","feminin"),
		dekliniere_substantiv("NOUN_KAMERA","Kamera","","","","feminin"),
		dekliniere_substantiv("NOUN_HABE","Habe","","Habe","en","feminin"),

    dekliniere_substantiv("NOUN_ALIGNMENT","Gesinnung","","Gesinnung","en","feminin"),
    #dekliniere_substantiv("NOUN_LAW","","","","",""),
    #dekliniere_substantiv("NOUN_BALANCE","","","","",""),
    #dekliniere_substantiv("NOUN_CHAOS","","","","",""),
    dekliniere_substantiv("NOUN_RACE","Volk","es","V�lk","er","neutrum"),
    dekliniere_substantiv("NOUN_ROLE","Klasse","","Klasse","en","feminin"),
    dekliniere_substantiv("NOUN_GENDER","Geschlecht","es","","","neutrum"),
    dekliniere_substantiv("NOUN_NAME","Name","ens","Name","en","maskulin"),
    dekliniere_substantiv("NOUN_CHARACTER","Charakter","s","Charakter","e","maskulin"),

    "/* Shopbezeichnungen */",
    dekliniere_substantiv("NOUN_ANTIKWAFFENLAGER","Antikwaffenlager","s","","","neutrum"),
    dekliniere_substantiv("NOUN_ANTIQUARIAT","Antiquariat","es","","e","neutrum"),
    dekliniere_substantiv("NOUN_BOUTIQUE","Boutique","","","en","feminin"),
    dekliniere_substantiv("NOUN_BUECHERFLOHMARKT","B�cherflohmarkt","es","","e","maskulin"),
    dekliniere_substantiv("NOUN_DELIKATESSENLADEN","Delikatessenladen","s","","","maskulin"),
    dekliniere_substantiv("NOUN_EISENWARENHANDLUNG","Eisenwarenhandlung","","","en","feminin"),
    dekliniere_substantiv("NOUN_JUWELIERLADEN","Juwelierladen","s","","","maskulin"),
    dekliniere_substantiv("NOUN_KRAMLADEN","Kramladen","s","","","maskulin"),
    dekliniere_substantiv("NOUN_LICHTFACHMARKT","Lichtfachmarkt","es","","e","maskulin"),
    dekliniere_substantiv("NOUN_SCHUTZBEKLEIDUNGSFACHGESCHAEFT","Schutzbekleidungsfachgesch�ft","es","","e","neutrum"),
    dekliniere_substantiv("NOUN_SPIRITUOSENHANDLUNG","Spirituosenhandlung","","","en","feminin"),

    "/* Fallen */",
    dekliniere_substantiv("NOUN_ANTI_MAGIC_FIELD","Antimagiefeld","es","Antimagiefeld","er","neutrum"),

    "/* Klassen (role.c) */",
    "/* Arch�ologe */",
    # "NOUN_ARCHAEOLOGE", "NOUN_ARCHAEOLOGIN"
    dekliniere_substantiv("NOUN_HILFSKRAFT","Hilfskraft","","Hilfskr�ft","e","feminin"),
    dekliniere_substantiv("NOUN_FELDFORSCHER","Feldforscher","s","Feldforscher","","maskulin"),
    dekliniere_substantiv("NOUN_FELDFORSCHERIN","Feldforscherin","","Feldforscherinn","en","feminin"),
    dekliniere_substantiv("NOUN_EXHUMIERER","Exhumierer","s","Exhumierer","","maskulin"),
    dekliniere_substantiv("NOUN_EXHUMIERERIN","Exhumiererin","","Exhumiererinn","en","feminin"),
    dekliniere_substantiv("NOUN_AUSGRAEBER","Ausgr�ber","s","Ausgr�ber","","maskulin"),
    dekliniere_substantiv("NOUN_AUSGRAEBERIN","Ausgr�berin","","Ausgr�berinn","en","feminin"),
    dekliniere_substantiv("NOUN_PROJEKTLEITER","Projektleiter","s","Projektleiter","","maskulin"),
    dekliniere_substantiv("NOUN_PROJEKTLEITERIN","Projektleiterin","","Projektleiterinn","en","feminin"),
    dekliniere_substantiv("NOUN_HOEHLENGEHER","H�hlengeher","s","H�hlengeher","","maskulin"),
    dekliniere_substantiv("NOUN_HOEHLENGEHERIN","H�hlengeherin","","H�hlengeherinn","en","feminin"),
    dekliniere_substantiv("NOUN_SPELAEOLOGE","Spel�ologe","en","Spel�ologe","en","maskulin"),
    dekliniere_substantiv("NOUN_SPELAEOLOGIN","Spel�ologin","","Spel�ologinn","en","feminin"),
    dekliniere_substantiv("NOUN_SAMMLER","Sammler","s","Sammler","","maskulin"),
    dekliniere_substantiv("NOUN_SAMMLERIN","Sammlerin","","Sammlerinn","en","feminin"),
    dekliniere_substantiv("NOUN_KURATOR","Kurator","es","Kurator","en","maskulin"),
    dekliniere_substantiv("NOUN_KURATORIN","Kuratorin","","Kuratorinn","en","feminin"),

    "/* Barbar */",
    # "NOUN_BARBAR", "NOUN_BARBARIN"
    dekliniere_substantiv("NOUN_STRAUCHDIEB","Strauchdieb","es","Strauchdieb","e","maskulin"),
    dekliniere_substantiv("NOUN_STRAUCHDIEBIN","Strauchdiebin","","Strauchdiebinn","en","feminin"),
    dekliniere_substantiv("NOUN_BRANDSCHATZER","Brandschatzer","s","Brandschatzer","","maskulin"),
    dekliniere_substantiv("NOUN_BRANDSCHATZERIN","Brandsch�tzerin","","Brandsch�tzerinn","en","feminin"),
    dekliniere_substantiv("NOUN_BANDIT","Bandit","en","Bandit","en","maskulin","en"),
    dekliniere_substantiv("NOUN_BANDITIN","Banditin","","Banditinn","en","feminin"),
    dekliniere_substantiv("NOUN_BRIGANT","Brigant","en","Brigant","en","maskulin","en"),
    dekliniere_substantiv("NOUN_BRIGANTIN","Brigantin","","Brigantinn","en","feminin"),
    dekliniere_substantiv("NOUN_RAEUBER","R�uber","s","R�uber","","maskulin"),
    dekliniere_substantiv("NOUN_RAEUBERIN","R�uberin","","R�uberinn","en","feminin"),
    dekliniere_substantiv("NOUN_PLUENDERER","Pl�nderer","s","Pl�nderer","","maskulin"),
    dekliniere_substantiv("NOUN_PLUENDERIN","Pl�nderin","","Pl�nderinn","en","feminin"),
    dekliniere_substantiv("NOUN_TOTSCHLAEGER","Totschl�ger","s","Totschl�ger","","maskulin"),
    dekliniere_substantiv("NOUN_TOTSCHLAEGERIN","Totschl�gerin","","Totschl�gerinn","en","feminin"),
    dekliniere_substantiv("NOUN_STAMMESFUEHRER","Stammesf�hrer","s","Stammesf�hrer","","maskulin"),
    dekliniere_substantiv("NOUN_STAMMESFUEHRERIN","Stammesf�herin","","Stammesf�herinn","en","feminin"),
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
    dekliniere_substantiv("NOUN_MEDICUS_OSSIUM","Wundarzt","es","Wund�rzt","e","maskulin"), # Chirurgicus
    dekliniere_substantiv("NOUN_MEDICA_OSSIUM","Wund�rztin","","Wund�rztinn","en","feminin"),
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
    dekliniere_substantiv("NOUN_GALLANT","Schildtr�ger","s","Schildtr�ger","","maskulin"),
    dekliniere_substantiv("NOUN_GALLANT_IN","Schildtr�gerin","","Schildtr�gerinn","en","feminin"),
    dekliniere_substantiv("NOUN_JUNKER","Junker","s","Junker","","maskulin"),
    dekliniere_substantiv("NOUN_JUNKFRAU","Junkfrau","","Junkfrau","en","feminin"),
    dekliniere_substantiv("NOUN_KAVALIER","Kavalier","s","Kavalier","e","maskulin"),
    dekliniere_substantiv("NOUN_KAVALIERIN","Kavalierin","","Kavalierinn","en","feminin"),
    dekliniere_substantiv("NOUN_EDELMANN","Edelmann","s","Edelm�nn","er","maskulin"),
    dekliniere_substantiv("NOUN_EDELFRAU","Edelfrau","","Edelfrau","en","feminin"),
    dekliniere_substantiv("NOUN_RITTER","Ritter","s","Ritter","","maskulin"),
    dekliniere_substantiv("NOUN_DAME","Dame","","Dame","en","feminin"),
    dekliniere_substantiv("NOUN_BANNERHERR","Bannerherr","en","Bannerherr","en","maskulin"),
    dekliniere_substantiv("NOUN_BANNERFRAU","Bannerfrau","","Bannerfrau","en","feminin"),
    dekliniere_substantiv("NOUN_ORDENSRITTER","Ordensritter","s","Ordensritter","","maskulin"),
    dekliniere_substantiv("NOUN_ORDENSRITTERIN","Ordensritterin","","Ordensritterinn","en","feminin"),
    dekliniere_substantiv("NOUN_FREIHERR","Freiherr","en","Freiherr","en","maskulin"),
    dekliniere_substantiv("NOUN_FREIFRAU","Freifrau","","Freifrau","en","feminin"),
    dekliniere_substantiv("NOUN_PALADIN","Paladin","s","Paladin","e","maskulin"),
    dekliniere_substantiv("NOUN_PALADIN_IN","Paladin","","Paladin","e","feminin"),

    "/* M�nch */",
    # "NOUN_MOENCH"
    dekliniere_substantiv("NOUN_KANDIDAT","Kandidat","en","Kandidat","en","maskulin"),
    dekliniere_substantiv("NOUN_KANDIDATIN","Kandidatin","","Kandidatinn","en","feminin"),
    dekliniere_substantiv("NOUN_NOVIZE","Novize","en","Novize","en","maskulin"),
    dekliniere_substantiv("NOUN_NOVIZIN","Novizin","","Novizinn","en","feminin"),
    dekliniere_adjektivisches_substantiv("NOUN_GEWEIHTE","Geweiht","maskulin"),
    dekliniere_adjektivisches_substantiv("NOUN_GEWEIHTIN","Geweiht","feminin"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_STEINE","Sch�ler","s","Sch�ler","","maskulin","","der Steine"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_STEINE","Sch�lerin","","Sch�lerinn","en","feminin","","der Steine"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_WASSER","Sch�ler","s","Sch�ler","","maskulin","","der Wasser"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_WASSER","Sch�lerin","","Sch�lerinn","en","feminin","","der Wasser"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_METALLE","Sch�ler","s","Sch�ler","","maskulin","","der Metalle"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_METALLE","Sch�lerin","","Sch�lerinn","en","feminin","","der Metalle"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_WINDE","Sch�ler","s","Sch�ler","","maskulin","","der Winde"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_WINDE","Sch�lerin","","Sch�lerinn","en","feminin","","der Winde"),
    dekliniere_substantiv("NOUN_SCHUELER_DER_FEUER","Sch�ler","s","Sch�ler","","maskulin","","der Feuer"),
    dekliniere_substantiv("NOUN_SCHUELERIN_DER_FEUER","Sch�lerin","","Sch�lerinn","en","feminin","","der Feuer"),
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
    dekliniere_substantiv("NOUN_STRASSENRAEUBER","Stra�enr�uber","s","Stra�enr�uber","","maskulin"),
    dekliniere_substantiv("NOUN_STRASSENRAEUBERIN","Stra�enr�uberin","","Str�senr�uberinn","en","feminin"),
    dekliniere_substantiv("NOUN_LANGFINGER","Langfinger","s","Langfinger","","maskulin"),
    dekliniere_substantiv("NOUN_HALLODRI","Hallodri","s","Hallodris","s","maskulin"),
    dekliniere_substantiv("NOUN_GANOVE","Ganove","en","Ganove","en","maskulin"),
    dekliniere_substantiv("NOUN_GANOVIN","Ganovin","","Ganovinn","en","feminin"),
    # dekliniere_substantiv("NOUN_RAEUBER"
    # dekliniere_substantiv("NOUN_RAEUBERIN"
    dekliniere_substantiv("NOUN_EINBRECHER","Einbrecher","s","Einbrecher","","maskulin"),
    dekliniere_substantiv("NOUN_EINBRECHERIN","Einbrecherin","","Einbrecherinn","en","feminin"),
    dekliniere_substantiv("NOUN_BETRUEGER","Betr�ger","s","Betr�ger","","maskulin"),
    dekliniere_substantiv("NOUN_BETRUEGERIN","Betr�gerin","","Betr�gerinn","en","feminin"),
    dekliniere_substantiv("NOUN_GANGSTER","Gangster","s","Gangster","","maskulin"),
    dekliniere_substantiv("NOUN_GANGSTERBRAUT","Gangsterbraut","","Gangsterbr�ut","e","feminin"),
    # dekliniere_substantiv("NOUN_DIEB"
    # dekliniere_substantiv("NOUN_DIEBIN"

	 "/* Waldl�ufer */",
	 # "NOUN_RANGER"
    dekliniere_substantiv("NOUN_JUNGSPUND","Jungspund","es","Jungspund","e","maskulin"),
    dekliniere_substantiv("NOUN_AUSGUCK","Ausguck","es","Ausguck","e","maskulin"),
    dekliniere_substantiv("NOUN_VORREITER","Vorreiter","s","","Vorreiter","maskulin"),
    dekliniere_substantiv("NOUN_VORREITERIN","Vorreiterin","","Vorreiterinn","en","feminin"),
    dekliniere_substantiv("NOUN_SPAEHER","Sp�her","s","Sp�her","","maskulin"),
    dekliniere_substantiv("NOUN_SPAEHERIN","Sp�herin","","Sp�herinn","en","feminin"),
    dekliniere_substantiv("NOUN_KUNDSCHAFTER","Kundschafter","s","Kundschafter","","maskulin"),
    dekliniere_substantiv("NOUN_KUNDSCHAFTERIN","Kundschafterin","","Kundschafterinn","en","feminin"),
    dekliniere_substantiv("NOUN_ARMBRUSTER","Armbruster","s","Armbruster","","maskulin"),
    dekliniere_substantiv("NOUN_ARMBRUSTERIN","Armbrusterin","","Armbrusterinn","en","feminin"),
    dekliniere_substantiv("NOUN_BOGENSCHUETZE","Bogensch�tze","en","Bogensch�tze","en","maskulin"),
    dekliniere_substantiv("NOUN_BOGENSCHUETZIN","Bogensch�tzin","","Bogensch�tzinn","en","feminin"),
    dekliniere_substantiv("NOUN_SCHARFSCHUETZE","Scharfsch�tze","en","Scharfsch�tze","en","maskulin"),
    dekliniere_substantiv("NOUN_SCHARFSCHUETZIN","Scharfsch�tzin","","Scharfsch�tzinn","en","feminin"),
    dekliniere_substantiv("NOUN_MEISTERSCHUETZE","Meistersch�tze","en","Meistersch�tze","en","maskulin"),
    dekliniere_substantiv("NOUN_MEISTERSCHUETZIN","Meistersch�tzin","","Meistersch�tzinn","en","feminin"),

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
    dekliniere_substantiv("NOUN_AUSFLUEGLER","Ausfl�gler","s","Ausfl�gler","","maskulin"),
    dekliniere_substantiv("NOUN_AUSFLUEGLERIN","Ausfl�glerin","","Ausfl�glerinn","en","feminin"),
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

    "/* Walk�re */",
    # "NOUN_VALKYRIE"
    dekliniere_substantiv("NOUN_GRUENSCHNABEL","Gr�nschnabel","s","Gr�nschn�bel","","maskulin"),
    dekliniere_substantiv("NOUN_HAUDEGEN","Haudegen","s","Haudegen","","maskulin"),
    dekliniere_substantiv("NOUN_STREITER","Streiter","s","Streiter","","maskulin"),
    dekliniere_substantiv("NOUN_STREITERIN","Streiterin","","Streiterinn","en","feminin"),
    dekliniere_substantiv("NOUN_KAEMPFER","K�mpfer","s","K�mpfer","","maskulin"),
    dekliniere_substantiv("NOUN_KAEMPFERIN","K�mpferin","","K�mpferinn","en","feminin"),
    dekliniere_substantiv("NOUN_KRIEGER","Krieger","s","Krieger","","maskulin"),
    dekliniere_substantiv("NOUN_KRIEGERIN","Kriegerin","","Kriegerinn","en","feminin"),
    dekliniere_substantiv("NOUN_SAEBELRASSLER","S�belrassler","","S�belrassler","","maskulin"),
    dekliniere_substantiv("NOUN_SAEBELRASSLERIN","S�belrasslerin","","S�belrasslerinn","en","feminin"),
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
    dekliniere_substantiv("NOUN_BESCHWOERER","Beschw�rer","","Beschw�rer","","maskulin"),
    dekliniere_substantiv("NOUN_BESCHWOERERIN","Beschw�rerin","","Beschw�rerinn","en","feminin"),
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

    "/* Orte (role.c) */",
    dekliniere_nominalphrase("NOUN_GREAT_TEMPLE","Gro�","Tempel","s","","","maskulin"),
    dekliniere_nominalphrase("NOUN_ARC_INSTITUT","Arch�ologisch","Institut","es","","e","neutrum"),
    dekliniere_substantiv("NOUN_TOMB","Grabst�tte","","Grabst�tte","en","feminin","n"),
    dekliniere_substantiv("NOUN_CAVE","H�hle","","H�hle","en","feminin","en"),
    dekliniere_substantiv("NOUN_CAVE_OF_THE_WUMPUS","Wumpush�hle","","Wumpush�hle","en","feminin","en"),
    dekliniere_substantiv("NOUN_TEMPEL","Tempel","s","","","feminin"),
    dekliniere_substantiv("NOUN_INSEL","Insel","","Insel","en","feminin"),
    dekliniere_substantiv("NOUN_CAMELOT","Camelot","","","","neutrum"),
    dekliniere_substantiv("NOUN_ANKH_MORPORK","Ankh-Morpork","","","","maskulin"),
    dekliniere_substantiv("NOUN_SHRINE_OF_DESTINY","Schrein","es","","e","maskulin","","des Schicksals"),
    dekliniere_substantiv("NOUN_DUALI_OASIS","Duali-Oase","","","en","feminin"),
    dekliniere_substantiv("NOUN_CAMP","Lager","s","","","neutrum"),
    dekliniere_substantiv("NOUN_BURG","Burg","","","","neutrum"),
    dekliniere_substantiv("NOUN_MONASTERY","Kloster","s","","","neutrum"),
    dekliniere_substantiv("NOUN_DRAGONS_LAIR","Drachenhort","es","","e","maskulin"),

    "/* Material-Substantive */",
    dekliniere_substantiv("NOUN_CLOTH","Stoff","es","","e","maskulin"),
    dekliniere_substantiv("NOUN_FLUESSIGKEIT","Fl�ssigkeit","","","en","feminin"),
    dekliniere_substantiv("NOUN_GLAS","Glas","es","","er","neutrum"),
    dekliniere_substantiv("NOUN_HOLZ","Holz","es","","er","neutrum"),
    dekliniere_substantiv("NOUN_LEDER","Leder","s","","","neutrum"),
    dekliniere_substantiv("NOUN_METALL","Metall","es","","e","neutrum"),
    dekliniere_substantiv("NOUN_MITHRIL","Mithril","s","","","neutrum"),
    dekliniere_substantiv("NOUN_PAPIER","Papier","es","","e","neutrum"),
    dekliniere_substantiv("NOUN_PLASTIK","Plastik","s","","s","neutrum"),
    dekliniere_substantiv("NOUN_PLATIN","Platin","s","","s","neutrum"),
    dekliniere_nominalphrase("NOUN_RICH_FOOD","reichhaltig","Mahl","es","Mahl","e","neutrum"),
    dekliniere_substantiv("NOUN_SCHUPPE","Schuppe","","","en","feminin"),
    dekliniere_substantiv("NOUN_WACHS","Wachs","es","","e","neutrum"),

    "/* blast of foo */",
    dekliniere_substantiv("NOUN_ODEM","Odem","s","","","maskulin"),
    dekliniere_substantiv("NOUN_BLAST_OF_MISSILES","Geschossodem","s","","","maskulin"),
    dekliniere_substantiv("NOUN_BLAST_OF_FIRE","Feuerodem","s","","","maskulin"),
    dekliniere_substantiv("NOUN_BLAST_OF_FROST","Eisodem","s","","","maskulin"),
    dekliniere_substantiv("NOUN_BLAST_OF_SLEEP_GAS","Schlafgasodem","s","","","maskulin"),
    dekliniere_substantiv("NOUN_BLAST_OF_DISINTEGRATION","Desintegrationsodem","s","","","maskulin"),
    dekliniere_substantiv("NOUN_BLAST_OF_LIGHTNING","Blitzodem","s","","","maskulin"),
    dekliniere_substantiv("NOUN_BLAST_OF_POISON_GAS","Giftgasodem","s","","","maskulin"),
    dekliniere_substantiv("NOUN_BLAST_OF_ACID","S�ureodem","s","","","maskulin"),


    "/* Dungeon-Namen */",
    dekliniere_nominalphrase("NOUN_DUNGEON_THE_GNOMISH_MINES", "Gnomsch", "Mine", "", "Mine", "en", "feminin"), # koennte besser sein
    dekliniere_substantiv("NOUN_DUNGEON_THE_QUEST","Queste","","","en","feminin"),
    dekliniere_substantiv("NOUN_DUNGEON_VLADS_TOWER","Vlads Turm","s","","e","maskulin"),
    dekliniere_substantiv("NOUN_DUNGEON_THE_ELEMENTAL_PLANES","Elementarebene","","Elementarebene","en","feminin"),

    "/* sonstige Substantive */",
    dekliniere_adjektivisches_substantiv("NOUN_CANINE","Hundeartig","neutrum"),
    dekliniere_adjektivisches_substantiv("NOUN_FELINE","Katzenartig","neutrum"),
    dekliniere_adjektivisches_substantiv("NOUN_INNERE","Inner","neutrum"),
    dekliniere_adjektivisches_substantiv("NOUN_MAGENINNERE","Mageninner", "neutrum"),
    dekliniere_nominalphrase("NOUN_ANGELIC_BEING","engelhaft","Gesch�pf","es","Gesch�pf","e","neutrum"),
    dekliniere_nominalphrase("NOUN_ANGELIC_BEING","engelhaft","Gesch�pf","es","Gesch�pf","e","neutrum"),
    dekliniere_nominalphrase("NOUN_APELIKE_CREATURE","affenartig","Kreatur","","Kreatur","en","feminin"),
    dekliniere_nominalphrase("NOUN_CLOSED_DOOR", "geschlossen","T�re", "", "T�re", "en", "feminin", "n"),
    dekliniere_nominalphrase("NOUN_DARK_PART_OF_A_ROOM","unbeleuchtet","Teil","es","","","maskulin","","eines Raumes"),
    dekliniere_nominalphrase("NOUN_GIANT_HUMANOID","riesig", "Humanoider","s","","","maskulin"),
    dekliniere_nominalphrase("NOUN_LARGE_STONE", "gro�", "Stein", "es", "Stein", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_LOWERED_DRAWBRIDGE", "heruntergelassen", "Zugbr�cke", "", "Zugbr�cke", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_MAGIC_PORTAL","magisch","Portal","s","Portal","e","neutrum"),
    dekliniere_nominalphrase("NOUN_MAJOR_DEMON", "gr��er", "D�mon", "s", "D�mon", "en", "maskulin"),
    dekliniere_nominalphrase("NOUN_MINOR_DEMON", "kleiner", "D�mon", "s", "D�mon", "en", "maskulin"),
    dekliniere_nominalphrase("NOUN_MOLTEN_LAVA", "geschmolzen", "Lava", "", "Lav", "en", "feminin", ""),
    dekliniere_nominalphrase("NOUN_OPEN_DOOR", "offen","T�re", "", "T�re", "en", "feminin", "n"),
    dekliniere_nominalphrase("NOUN_OPULENT_THRONE", "opulent", "Thron", "es", "Thron", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_RAISED_DRAWBRIDGE", "hochgezogen", "Zugbr�cke", "", "Zugbr�cke", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_SQUEAKY_BOARD", "quietschend", "Diele", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_ABBILD","Abbild","es","Abbild","er","neutrum"),
    dekliniere_substantiv("NOUN_ACID","S�ure","","S�ure","en","feminin"),
    dekliniere_substantiv("NOUN_AEQUATOR","�quator","es","�quator","en","maskulin"),
    dekliniere_substantiv("NOUN_AIR_CURRENT", "Luftstr�mung", "", "Luftstr�mung", "en", "feminin"),
    dekliniere_substantiv("NOUN_ALMA_MATER","Alma Mater","","","fremder","feminin"),
    dekliniere_substantiv("NOUN_ANDACHT","Andacht","","Andacht","en","feminin"),
    dekliniere_substantiv("NOUN_ANSPRECHBARKEIT","Ansprechbarkeit","","","en","feminin"),
    dekliniere_substantiv("NOUN_ARROW_TRAP","Pfeilfalle","","Pfeilfalle","en","feminin"),
    dekliniere_substantiv("NOUN_ARTIFACT","Artefakt","es","Artefakt","e","neutrum"),
    dekliniere_substantiv("NOUN_ATTACK","Angriff","es","Angriffe","e","maskulin"),
    dekliniere_substantiv("NOUN_AUFGABE","Aufgabe","","Aufgabe","en","feminin"),
    dekliniere_substantiv("NOUN_AUGE","Auge","es","Auge","en","neutrum"),
    dekliniere_substantiv("NOUN_AUSRUESTUNG","Ausr�stung","","Ausr�stung","en","feminin"),
    dekliniere_substantiv("NOUN_AUSWAHL","Auswahl","","Auswahl","en","feminin"),
    dekliniere_substantiv("NOUN_AXOPODIUM","Axopodium","s","Axopodien","fremder","neutrum"),
    dekliniere_substantiv("NOUN_BEAM", "Strahl", "es", "Strahl", "en", "maskulin"),
    dekliniere_substantiv("NOUN_BEDECKUNG", "Bedeckung", "", "Bedeckung", "en", "feminin"),
    dekliniere_substantiv("NOUN_BEFEHL","Befehl","es","Befehl","e","maskulin"),
    dekliniere_substantiv("NOUN_BEFREIUNG","Befreiung","","Befreiung","en","feminin"),
    dekliniere_substantiv("NOUN_BEITRAG","Beitrag","es","Beitr�g","e","maskulin"),
    dekliniere_substantiv("NOUN_BELOHNUNG", "Belohnung", "", "Belohnung", "en", "feminin", ""),
    dekliniere_substantiv("NOUN_BEREICH","Bereich","es","Bereich","e","maskulin", "s"),
    dekliniere_substantiv("NOUN_BEMUEHUNG","Bem�hung","","Bem�hung","en","feminin"),
    dekliniere_substantiv("NOUN_BERUEHRUNG","Ber�hrung","","Ber�hrung","en","feminin"),
    dekliniere_substantiv("NOUN_BESITZTUM","Besitztum","s","Besitzt�m","er","neutrum"),
    dekliniere_substantiv("NOUN_BESUCH","Besuch","es","Besuch","e","maskulin"),
    dekliniere_substantiv("NOUN_BEWAFFNUNG","Bewaffnung","","Bewaffnung","en","feminin"),
    dekliniere_substantiv("NOUN_BEWEGUNG","Bewegung","","Bewegung","en","feminin"),
    dekliniere_substantiv("NOUN_BEWUSSTSEIN","Bewusstsein","s","","","neutrum"),
    dekliniere_substantiv("NOUN_BITE","Biss","es","Biss","e","maskulin"),
    dekliniere_substantiv("NOUN_BLADE","Klinge","","Klinge","en","feminin"),
    dekliniere_nominalphrase("NOUN_BLIND_IO", "Blind", "Io", "", "", "", "maskulin"),
    dekliniere_substantiv("NOUN_BLITZGEWITTER","Blitzgewitter","s","Blitzgewitter","","neutrum"),
    dekliniere_substantiv("NOUN_BOOK","Buch","es","B�ch","er","neutrum"),
    dekliniere_substantiv("NOUN_BRUDER","Bruder","s","Br�der","","maskulin"),
    dekliniere_substantiv("NOUN_BUCH","Buch","es","B�ch","er","neutrum"),
    dekliniere_substantiv("NOUN_CHAIN","Kette","","Kette","en","feminin"),
    dekliniere_substantiv("NOUN_CHAOS","Chaos","","","","neutrum"),
    dekliniere_substantiv("NOUN_CHARAKTER","Charakter","s","Charakter","e","maskulin"),
    dekliniere_substantiv("NOUN_CLAW","Klaue","","Klaue","en","feminin"),
    dekliniere_substantiv("NOUN_CONDUCT","Gebaren","s","","","neutrum"),
    dekliniere_substantiv("NOUN_CREATURE","Kreatur","","Kreatur","en","feminin"),
    dekliniere_substantiv("NOUN_DAME","Dame","","Damen","en","feminin"),
    dekliniere_substantiv("NOUN_DART_TRAP","Dartfalle","","Dartfalle","en","feminin"),
    dekliniere_substantiv("NOUN_DASEINSFORM","Daseinsform","","Daseinsform","en","feminin"),
    dekliniere_substantiv("NOUN_DEMON", "D�mon", "s", "D�mon", "en", "maskulin"),
    dekliniere_substantiv("NOUN_DESINTEGRATIONFELD","Desintegrationsfeld","es","Desintegrationsfeld","er","neutrum"),
    dekliniere_substantiv("NOUN_DINGSBUMS","Dingsbums","","","","neutrum"),
    dekliniere_substantiv("NOUN_DISPLACEDIMAGE","Abbild","es","Abbild","er","neutrum"),
    dekliniere_substantiv("NOUN_DURST","Durst","es","","","maskulin"),
    dekliniere_substantiv("NOUN_EIGENSCHAFT","Eigenschaft","","Eigenschaft","en","feminin"),
    dekliniere_substantiv("NOUN_EINGEWEIDE","Eingeweide","s","Eingeweide","","neutrum"),
    dekliniere_substantiv("NOUN_EINKAUF","Einkauf","es","Eink�uf","e","maskulin"),
    dekliniere_substantiv("NOUN_EISSTURM","Eissturm","es","Eisst�rm","e","maskulin"),
    dekliniere_substantiv("NOUN_ELLBOGEN","Ellbogen","s","Ellbogen","","maskulin"),
    dekliniere_substantiv("NOUN_ENERGIE","Energie","","Energie","en","feminin"),
    dekliniere_substantiv("NOUN_ENTLADUNG","Entladung","","Entladung","en","feminin"),
    dekliniere_substantiv("NOUN_ENTSCHAEDIGUNG","Entsch�digung","","Entsch�digung","en","feminin"),
    dekliniere_substantiv("NOUN_ERSTWAFFE","Erstwaffe","","Erstwaffe","en","feminin"),
    dekliniere_substantiv("NOUN_ESSEN","Essen","s","Essen","","neutrum"),
    dekliniere_substantiv("NOUN_EXPLOSION","Explosion","","Explosion","en","feminin"),
    dekliniere_substantiv("NOUN_EXTREMITAET","Extremit�t","","Extremit�t","en","feminin"),
    dekliniere_substantiv("NOUN_FAEHIGKEIT","F�higkeit","","F�higkeit","en","feminin"),
    dekliniere_substantiv("NOUN_FEATHER", "Feder", "", "Feder", "en", "feminin"),
    dekliniere_substantiv("NOUN_FEHLVERHALTEN","Fehlverhalten","s","","","neutrum"),
    dekliniere_substantiv("NOUN_FELL","Fell","es","Fell","e","neutrum"),
    dekliniere_substantiv("NOUN_FELSEN","Felsen","","Felsen","","maskulin"),
    dekliniere_substantiv("NOUN_FLAESCHCHEN","Fl�schchen","s","Fl�schchen","","neutrum"),
    dekliniere_substantiv("NOUN_FLAGELLE","Flagelle","","Flagelle","en","feminin"),
    dekliniere_substantiv("NOUN_FLAKON","Flakon","s","Flakon","s","neutrum"),
    dekliniere_substantiv("NOUN_FLAMMENSAEULE","Flammens�ule","","Flammens�ule","en","feminin"),
    dekliniere_substantiv("NOUN_FLECK","Fleck","es","Fleck","e","maskulin"),
    dekliniere_substantiv("NOUN_FLEISCH","Fleisch","es","","","neutrum"),
    dekliniere_substantiv("NOUN_FLOOR_OF_A_ROOM","Boden","s","","","maskulin","","eines Raumes"),
    dekliniere_substantiv("NOUN_FLOSSE","Flosse","","Flosse","en","feminin"),
    dekliniere_substantiv("NOUN_FLUEGEL","Fl�gel","s","Fl�gel","","maskulin"),
    dekliniere_substantiv("NOUN_FLUEGELSPITZE","Fl�gelspitze","","Fl�gelspitze","en","feminin"),
    dekliniere_substantiv("NOUN_FORTSATZ","Fortsatz","es","Fortsatz","e","maskulin"),
    dekliniere_substantiv("NOUN_FUEHLER","F�hler","s","F�hler","","maskulin"),
    dekliniere_substantiv("NOUN_FUNGUS","Pilz","es","","","maskulin"),
    dekliniere_substantiv("NOUN_GAME","Spiel","es","Spiel","e","neutrum"),
    dekliniere_substantiv("NOUN_GASWOLKE","Gaswolke","","Gaswolke","en","feminin"),
    dekliniere_substantiv("NOUN_GAS_SPORE_EXPLOSION","Gassporenexplosion","","Gassporenexplosion","en","feminin"),
    dekliniere_substantiv("NOUN_GAUNTLETS", "Panzerhandschuh", "es", "Panzerhandschuh", "e", "maskulin"),
    dekliniere_substantiv("NOUN_GAZE","Blick","es","Blick","e","maskulin"),
    dekliniere_substantiv("NOUN_GEAR","Zahnrad","es","Zahnr�d","er","neutrum"),
    dekliniere_substantiv("NOUN_GEGENSTAND","Gegenstand","es","Gegenst�nd","e","maskulin"),
    dekliniere_substantiv("NOUN_GEHIRN","Gehirn","es","Gehirn","e","neutrum"),
    dekliniere_substantiv("NOUN_GEIST","Geist","es","Geist","e","maskulin"),
    dekliniere_substantiv("NOUN_GELD","Geld","es","Geld","er","neutrum"),
    dekliniere_substantiv("NOUN_GEM_COLLECTION", "Edelsteinsammlung", "", "Edelsteinsammlung", "en", "feminin"),
    dekliniere_substantiv("NOUN_GESCHENK","Geschenk","es","Geschenk","e","neutrum"),
    dekliniere_substantiv("NOUN_GESCHWINDIGKEIT","Geschwindigkeit","","Geschwindigkeit","en","feminin"),
    dekliniere_substantiv("NOUN_GESUNDHEIT","Gesundheit","","","en","feminin"),
    dekliniere_substantiv("NOUN_GEWISSEN","Gewissen","s","Gewissen","","neutrum"),
    dekliniere_substantiv("NOUN_GIFT","Gift","es","Gift","e","neutrum"),
    dekliniere_substantiv("NOUN_GIFTGASWOLKE","Giftgaswolke","","Giftgaswolke","en","feminin"),
    dekliniere_substantiv("NOUN_GLANZ","Glanz","es","Glanz","e","maskulin"),
    dekliniere_substantiv("NOUN_GLEICHGEWICHT","Gleichgewicht","es","","e","neutrum"),
    dekliniere_substantiv("NOUN_GLIED","Glied","es","Glied","er","neutrum"),
    dekliniere_substantiv("NOUN_GLUECKSSTRAEHNE","Gl�cksstr�hne","","Gl�cksstr�hne","en","feminin"),
    dekliniere_substantiv("NOUN_GOETTIN","G�ttin","","G�ttinn","en","feminin"),
    dekliniere_substantiv("NOUN_GOTT","Gott","es","G�tt","er","maskulin"),
    dekliniere_substantiv("NOUN_GOTTHEIT","Gottheit","","Gottheit","en","feminin"),
    dekliniere_substantiv("NOUN_GRABUNG","Grabung","","Grabung","en","feminin"),
    dekliniere_substantiv("NOUN_GRIFF","Griff","es","Griff","e","maskulin"),
    dekliniere_substantiv("NOUN_GRIP","Griff","es","Griff","e","maskulin"),
    dekliniere_substantiv("NOUN_GRUBE","Grube","","Grube","en","feminin"),
    dekliniere_substantiv("NOUN_GRUND","Grund","es","Gr�nd","e","maskulin"),
    dekliniere_substantiv("NOUN_GUTHABEN","Guthaben","s","Guthaben","","neutrum"),
    dekliniere_substantiv("NOUN_HAAR","Haar","es","Haar","e","neutrum"),
    dekliniere_substantiv("NOUN_HAKEN","Haken","s","Haken","","maskulin"),
    dekliniere_substantiv("NOUN_HALBGOETTIN","Halbg�ttin","","Halbg�ttinn","en","feminin"),
    dekliniere_substantiv("NOUN_HALBGOTT","Halbgott","es","Halbg�tt","er","maskulin"),
    dekliniere_substantiv("NOUN_HALLE", "Halle", "", "Halle", "en", "feminin"),
    dekliniere_substantiv("NOUN_HAMMER","Hammer", "s", "H�mmer", "", "maskulin"),
    dekliniere_substantiv("NOUN_HANDBREIT","Handbreit","","Handbreit","en","feminin"),
    dekliniere_substantiv("NOUN_HANDLUNG","Handlung","","Handlung","en","feminin"),
    dekliniere_substantiv("NOUN_HAND_DES_TODES", "Hand", "", "H�nd", "e", "feminin", "", "des Todes"),
    dekliniere_substantiv("NOUN_HAZE","Nebel","s","Nebel","","maskulin"),
    dekliniere_substantiv("NOUN_HEIMAT","Heimat","","Heimat","en","feminin"),
    dekliniere_substantiv("NOUN_HERR","Herr","en","Herr","en","maskulin"),
    dekliniere_substantiv("NOUN_HIEB","Hieb","es","Hieb","e","maskulin"),
    dekliniere_substantiv("NOUN_HILFE","Hilfe","","Hilfe","en","feminin"),
    dekliniere_substantiv("NOUN_HIRNAREAL", "Hirnareal", "s", "Hirnareal", "e", "neutrum", "s"),
    dekliniere_substantiv("NOUN_HORNHAUT", "Hornhaut", "", "Hornh�ut", "e", "feminin"),
    dekliniere_substantiv("NOUN_HUF","Huf","es","Huf","e","maskulin"),
    dekliniere_substantiv("NOUN_HUMANOID","Humanoider","s","","","maskulin"),
    dekliniere_substantiv("NOUN_HYPHE","Hyphe","","Hyphe","en","feminin"),
    dekliniere_substantiv("NOUN_INHALT", "Inhalt", "es", "Inhalt", "e", "maskulin"),
    dekliniere_substantiv("NOUN_IRON_BALL", "Eisenkugel", "", "Eisenkugel", "en", "feminin"),
    dekliniere_substantiv("NOUN_KAELTE","K�lte","","","","feminin"),
    dekliniere_substantiv("NOUN_KANTE","Kante","","Kante","en","feminin"),
    dekliniere_substantiv("NOUN_KARAFFE","Karaffe","","Karaffe","en","feminin"),
    dekliniere_substantiv("NOUN_KICK","Tritt","es","Tritt","e","maskulin"),
    dekliniere_substantiv("NOUN_KIEL","Kiel","es","Kiel","e","maskulin"),
    dekliniere_substantiv("NOUN_KIEME","Kieme","","Kieme","en","feminin"),
    dekliniere_substantiv("NOUN_KLAUE","Klaue","","Klaue","en","feminin"),
    dekliniere_substantiv("NOUN_KLEID","Kleid","es","Kleid","er","neutrum"),
    dekliniere_substantiv("NOUN_KNOCHEN","Knochen","s","Knochen","","maskulin"),
    dekliniere_substantiv("NOUN_KNOWLEDGE","Kenntnis","","Kenntniss","e","feminin"),
    dekliniere_substantiv("NOUN_KOERPER","K�rper","s","K�rper","","maskulin"),
    dekliniere_substantiv("NOUN_KOLBEN","Kolben","s","Kolben","","maskulin"),
    dekliniere_substantiv("NOUN_KOLLISION","Kollision","","Kollision","en","feminin"),
    dekliniere_substantiv("NOUN_KOPF","Kopf","es","K�pf","e","maskulin"),
    dekliniere_substantiv("NOUN_KORRIDOR","Korridor","s","Korridor","e","maskulin"),
    dekliniere_substantiv("NOUN_KRAFT","Kraft","","Kr�ft","e","feminin"),
    dekliniere_substantiv("NOUN_KRAKELEI","Krakelei","","Krakelei","en","feminin"),
    dekliniere_substantiv("NOUN_KRATZSPUR","Kratzspur","","Kratzspur","en","feminin"),
    dekliniere_substantiv("NOUN_KRIBBELN","Kribbeln","s","","","neutrum"),
    dekliniere_substantiv("NOUN_KRUG","Krug","es","Kr�g","e","maskulin"),
    dekliniere_substantiv("NOUN_KUNDE","Kunde","","Kunde","en","maskulin"),
    dekliniere_substantiv("NOUN_KUNDIN","Kundin", "", "Kundinn", "en", "feminin", "nen"), 
    dekliniere_substantiv("NOUN_LADDER_DOWN", "Leiter", "", "Leiter", "en", "feminin","","hinab"),
    dekliniere_substantiv("NOUN_LADDER_UP", "Leiter", "", "Leiter", "en", "feminin","","hinauf"),
    dekliniere_substantiv("NOUN_LAENGE","L�nge","","L�nge","en","feminin"),
    dekliniere_substantiv("NOUN_LAMELLE","Lamelle","","Lamelle","en","feminin"),
    dekliniere_substantiv("NOUN_LANTERN","Laterne","","Laterne","en","feminin"),
    dekliniere_substantiv("NOUN_LAST","Last","","Last","en","feminin"),
    dekliniere_substantiv("NOUN_LAUF","Lauf","es","L�uf","e","maskulin"),
    dekliniere_substantiv("NOUN_LEBEN","Leben","s","Leben","","neutrum"),
    dekliniere_substantiv("NOUN_LEBENSKRAFT","Lebenskraft","","Lebenskr�ft","e","feminin"),
    dekliniere_substantiv("NOUN_LEIB","Leib","es","Leib","er","maskulin"),
    dekliniere_substantiv("NOUN_LEVEL_TELEPORTER","Levelteleporter","s","","","maskulin"),
    dekliniere_substantiv("NOUN_LICHTBLITZ","Lichtblitz","es","Lichtblitz","e","maskulin"),
    dekliniere_substantiv("NOUN_LOCK", "Schloss", "es", "Schl�ss", "er", "neutrum"),
    dekliniere_substantiv("NOUN_LUNGE","Lunge","","Lunge","en","feminin"),
    dekliniere_substantiv("NOUN_MACHT","Macht","","M�cht","e","feminin"),
    dekliniere_substantiv("NOUN_MAEHNE","M�hne","","M�hne","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIC_TRAP","Magiefalle","","Magiefalle","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIE","Magie","","","en","feminin"),
    dekliniere_substantiv("NOUN_MAGIEEXPLOSION","Magieexplosion","","Magieexplosion","en","feminin"),
    dekliniere_substantiv("NOUN_MAHL","Mahl","es","Mahl","e","neutrum"),
    dekliniere_substantiv("NOUN_MAHLZEIT","Mahlzeit","","Mahlzeit","en","feminin"),
    dekliniere_substantiv("NOUN_MAN", "Mann", "es", "M�nn", "er", "maskulin"),
    dekliniere_substantiv("NOUN_MARKER", "F�ller", "s", "F�ller", "", "maskulin"),
    dekliniere_substantiv("NOUN_MAUL","Maul","es","M�ul","er","neutrum"),
    dekliniere_substantiv("NOUN_MINE", "Mine", "", "Mine", "en", "feminin"),
    dekliniere_substantiv("NOUN_MISSILE", "Geschoss", "es", "Geschoss", "e", "neutrum"),
    dekliniere_substantiv("NOUN_MITOCHONDRIUM", "Mitochondrium", "s", "Mitochondrien", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_MITTE", "Mitte", "", "Mitte", "en", "feminin"),
    dekliniere_substantiv("NOUN_MOAT","Burggraben","s","Burggr�ben","","maskulin"), # Burggraben, Wassergraben
    dekliniere_substantiv("NOUN_MONSTER","Monster","s","Monster","","neutrum"),
    dekliniere_substantiv("NOUN_MUELL","M�ll","s","","","maskulin"),
    dekliniere_substantiv("NOUN_MUSKEL","Muskel","s","Muskel","en","maskulin"),
    dekliniere_substantiv("NOUN_MYZELIUM","Myzelium","s","Myzelien","fremder","neutrum"),
    dekliniere_substantiv("NOUN_NADEL","Nadel","","Nadel","en","feminin"),
    dekliniere_substantiv("NOUN_NASE","Nase","","Nase","en","feminin"),
    dekliniere_substantiv("NOUN_NASENHOEHLE","Nasenh�hle","","Nasenh�hle","en","feminin"),
    dekliniere_substantiv("NOUN_NERVUS_OLFACTORIUS","Nervus Olfactorius","","","","maskulin"),
    dekliniere_substantiv("NOUN_NETZ","Netz","es","Netz","e","neutrum"),
    dekliniere_substantiv("NOUN_OBERFLAECHE", "Oberfl�che", "", "Oberfl�che", "en", "feminin"), 
    dekliniere_substantiv("NOUN_OOZE", "Schlamm", "es", "Schlamm", "e", "maskulin"), 
    dekliniere_substantiv("NOUN_PACK","Rucksack","s","Rucks�ck","e","maskulin"),
    dekliniere_substantiv("NOUN_PAYMENT","Bezahlung","","Bezahlung","en","feminin"),
    dekliniere_substantiv("NOUN_PECHSTRAEHNE","Pechstr�hne","","Pechstr�hne","en","feminin"),
    dekliniere_substantiv("NOUN_PET","Mitstreiter","s","Mitstreiter","","maskulin"), # Haustier, Weggenosse netzhack: Begleittier
    dekliniere_substantiv("NOUN_PFAD","Pfad","es","Pfad","e","maskulin"),
    dekliniere_substantiv("NOUN_PFOTE","Pfote","","Pfote","en","feminin"),
    dekliniere_substantiv("NOUN_PHIOLE","Phiole","","Phiole","en","feminin"),
    dekliniere_substantiv("NOUN_PHOTOREZEPTOR","Photorezeptor","s","Photorezeptor","en","maskulin"),
    dekliniere_substantiv("NOUN_PHYSIK","Physik","","","en","feminin"),
    dekliniere_substantiv("NOUN_PIECE_OF_ARMOR","Teil","es","","","maskulin","","einer R�stung"),
    dekliniere_substantiv("NOUN_PIECE_OF_FOOD","Lebensmittel","s","","","maskulin"),
    dekliniere_substantiv("NOUN_PILE_OF_COIN","M�nzhaufen","s","","","maskulin"),
    dekliniere_substantiv("NOUN_PILZGEFLECHT","Pilzgeflecht","es","Pilzgeflecht","e","neutrum"),
    dekliniere_substantiv("NOUN_PLATZ","Platz","es","Pl�tz","e","maskulin"),
    dekliniere_substantiv("NOUN_PLUNDER","Plunder","s","Plunder","en","maskulin"),
    dekliniere_substantiv("NOUN_POISON","Gift","es","Gift","e","neutrum"),
    dekliniere_substantiv("NOUN_POLEARM", "Stangenwaffe", "", "Stangenwaffe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_POOL","T�mpel","s","T�mpel","","maskulin"),
    dekliniere_substantiv("NOUN_POOL_OF_WATER","Wassert�mpel","s","Wassert�mpel","","maskulin"),
    dekliniere_substantiv("NOUN_PORTCULLIS","Fallgatter","s","Fallgatter","","neutrum"),
    dekliniere_substantiv("NOUN_POSITION","Position","","Position","en","feminin"),
    dekliniere_substantiv("NOUN_PSEUDOPODIUM","Pseudopodium","s","Pseudopodien","fremder","neutrum"),
    dekliniere_substantiv("NOUN_PRAEMAXILLARE","Praemaxillare","","","","maskulin"),
    dekliniere_substantiv("NOUN_PROTOPLASMA","Protoplasma","s","Protoplasm","en","neutrum"),
    dekliniere_substantiv("NOUN_QUIVER","K�cher","s","K�cher","","maskulin"),
    dekliniere_substantiv("NOUN_RACHEN","Rachen","s","Rachen","","maskulin"),
    dekliniere_substantiv("NOUN_RAND","Rand","es","R�nd","er","maskulin"),
    dekliniere_substantiv("NOUN_RAUCH","Rauch","s","","","maskulin"),
    dekliniere_substantiv("NOUN_RECHNUNG","Rechnung","","Rechnung","en","feminin"),
    dekliniere_substantiv("NOUN_REFLECTION", "Spiegelbild", "es", "Spiegelbild", "er", "neutrum"),
    dekliniere_substantiv("NOUN_REFLEX", "Reflex", "es", "Reflex", "e", "maskulin"), 
    dekliniere_substantiv("NOUN_REITTIER", "Reittier", "es", "Reittier", "e", "neutrum"),
    dekliniere_substantiv("NOUN_REITUNFALL","Reitunfall","es","Reitunf�ll","e","maskulin"),
    dekliniere_substantiv("NOUN_REKORD","Rekord","es","Rekord","e","maskulin"),
    dekliniere_substantiv("NOUN_REPERTOIRE","Repertoire","s","Repertoire","s","neutrum"),
    dekliniere_substantiv("NOUN_RESULTAT","Resultat","es","Resultat","e","neutrum"),
    dekliniere_substantiv("NOUN_RHIZOM","Rhizom","s","Rhizom","e","neutrum"),
    dekliniere_substantiv("NOUN_RHIZOMORPHE","Rhizomorphe","","Rhizomorphe","en","feminin", "n"),
    dekliniere_substantiv("NOUN_RUCKSACK","Rucksack","s","Rucks�ck","e","maskulin"),
    dekliniere_substantiv("NOUN_RUESSEL","R�ssel","s","R�ssel","","maskulin"),
    dekliniere_substantiv("NOUN_RUNE","Rune","","Rune","en","feminin"),
    dekliniere_substantiv("NOUN_SABER", "S�bel", "s", "S�bel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SAEUREWOLKE","S�urewolke","","S�urewolke","en","feminin"),
    dekliniere_substantiv("NOUN_SCALES", "Schuppe", "", "Schuppe", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SCHAEDEL","Sch�del","s","Sch�del","","maskulin"),
    dekliniere_substantiv("NOUN_SCHICHT","Schicht","","Schicht","en","feminin"),
    dekliniere_substantiv("NOUN_SCHICKSAL","Schicksal","es","Schicksal","e","neutrum"),
    dekliniere_substantiv("NOUN_SCHLANGENGIFT","Schlangengift","es","Schlangengift","e","neutrum"),
    dekliniere_substantiv("NOUN_SCHNABEL","Schnabel","s","Schn�bel","","maskulin"),
    dekliniere_substantiv("NOUN_SCHOENSCHRIFT","Sch�nschrift","","Sch�nschrift","en","feminin"),
    dekliniere_substantiv("NOUN_SCHREIBVERSUCH","Schreibversuch","es","Schreibversuch","e","maskulin"),
    dekliniere_substantiv("NOUN_SCHRIFT","Schrift","","Schrift","en","feminin"),
    dekliniere_substantiv("NOUN_SCHUPPE","Schuppe","","Schuppe","en","feminin"),
    dekliniere_substantiv("NOUN_SCHUPPENRUESTUNG","Schuppenr�stung","","Schuppenr�stung","en","feminin"),
    dekliniere_substantiv("NOUN_SCHWALL","Schwall","es","Schwall","e","maskulin"),
    dekliniere_substantiv("NOUN_SCHWANZ","Schwanz","es","Schw�nz","e","maskulin"),
    dekliniere_substantiv("NOUN_SCHWANZSTIEL","Schwanzstiel","es","Schwanzstiel","e","maskulin"),
    dekliniere_substantiv("NOUN_SCHWESTER","Schwester","","Schwester","en","feminin"),
    dekliniere_substantiv("NOUN_SECRET_DOOR", "Geheimt�re", "", "Geheimt�re", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_SEHNERV","Sehnerv","es","Sehnerv","en","maskulin"),
    dekliniere_substantiv("NOUN_SEITE","Seite","","Seite","en","feminin", "n"),
    dekliniere_substantiv("NOUN_SELF_GENOCIDE", "Selbstgenozid", "es", "", "e", "maskulin"),
    dekliniere_substantiv("NOUN_SHIRT","Hemd","es","Hemd","en","neutrum"),
    dekliniere_substantiv("NOUN_SHOP","Laden","s","L�den","","maskulin"),
    dekliniere_substantiv("NOUN_SHOPS_CEILING","Ladendecke","","Ladendecke","en","feminin"),
    dekliniere_substantiv("NOUN_SIGHT","Sichtfeld","s","Sichtfeld","er","neutrum"),    
    dekliniere_substantiv("NOUN_SILBER","Silber","s","","","neutrum"),
    dekliniere_substantiv("NOUN_SINN","Sinn","es","Sinn","e","maskulin"),
    dekliniere_substantiv("NOUN_SKILL", "Fertigkeit", "", "Fertigkeit", "en", "feminin"),
    dekliniere_substantiv("NOUN_SMOCK", "Kittel", "s", "Kittel", "", "maskulin"),
    dekliniere_substantiv("NOUN_SNACK","Snack","s","Snack","s","maskulin"),
    dekliniere_substantiv("NOUN_SOHN","Sohn","es","S�hn","e","maskulin"),
    dekliniere_substantiv("NOUN_SONDIERUNG","Sondierung","","Sondierung","en","feminin"),
    dekliniere_substantiv("NOUN_SPIDER_WEB", "Spinnennetz", "es", "Spinnennetz", "e", "neutrum"),
    dekliniere_substantiv("NOUN_SPITZE","Spitze","","Spitze","en","maskulin"),
    dekliniere_substantiv("NOUN_SPORE","Spore","","Spore","en","feminin"),
    dekliniere_substantiv("NOUN_STACHEL", "Stachel", "es", "Stachel", "en", "maskulin"),
    dekliniere_substantiv("NOUN_STAIRCASE_DOWN", "Treppe", "", "", "", "feminin","","hinab"),
    dekliniere_substantiv("NOUN_STAIRCASE_UP", "Treppe", "", "", "", "feminin","","hinauf"),
    dekliniere_substantiv("NOUN_STAIRS", "Treppe", "", "Treppe", "en", "feminin"),
    dekliniere_substantiv("NOUN_STATUE_TRAP","Statuenfalle","","Statuenfalle","en","feminin"),
    dekliniere_substantiv("NOUN_STIEL","Stiel","es","Stiel","e","maskulin"),
    dekliniere_substantiv("NOUN_STIRN","Stirn","","","en","feminin"),
    dekliniere_substantiv("NOUN_STOMACH","Magen","s","M�gen","","maskulin"),
    dekliniere_substantiv("NOUN_STRICH","Strich","es","Strich","e","maskulin"),
    dekliniere_substantiv("NOUN_STROEMUNG","Str�mung","","Str�mung","en","feminin"),
    dekliniere_substantiv("NOUN_STROMSCHLAG","Stromschlag","es","Stromschl�g","e","maskulin"),
    dekliniere_substantiv("NOUN_STRUDEL","Strudel","s","Strudel","","maskulin"),
    dekliniere_substantiv("NOUN_STURZ","Sturz","es","St�rz","e","feminin"),
    dekliniere_substantiv("NOUN_SUIT","R�stung","","R�stung","en","feminin"),
    dekliniere_substantiv("NOUN_SUMMEN","Summen","s","","","neutrum"),
    dekliniere_substantiv("NOUN_SWAMP","Sumpf","es","S�mpf","e","maskulin"),
    dekliniere_substantiv("NOUN_TEIL","Teil","es","Teil","e","maskulin"),
    dekliniere_substantiv("NOUN_TELEPATHIE","Telepathie","","","en","feminin"),
    dekliniere_substantiv("NOUN_TEMPLES_CEILING","Tempeldecke","","Tempeldecke","en","feminin"),
    dekliniere_substantiv("NOUN_TENTAKEL","Tentakel","s","Tentakel", "","maskulin"),
    dekliniere_substantiv("NOUN_TOCHTER","Tochter","","T�chter","","feminin"),
    dekliniere_substantiv("NOUN_TODESFELD","Todesfeld","es","Todesfeld","er","neutrum"),
    dekliniere_substantiv("NOUN_TOUCH_OF_DEATH","Todesgriff","es","Todesgriff","e","maskulin"),
    dekliniere_substantiv("NOUN_TRANSFORMATION","Transformation","","Transformation","en","feminin"),
    dekliniere_substantiv("NOUN_TRAP","Falle","","Falle","en","feminin"),
    dekliniere_substantiv("NOUN_TREFFER","Treffer","s","Treffer","","maskulin"),
    dekliniere_substantiv("NOUN_TREPPENSTURZ","Treppensturz","es","Treppenst�rz","e","maskulin"),
    dekliniere_substantiv("NOUN_TRIGGER","Ausl�semechanismus","","Ausl�semechanismen","fremder","maskulin"),
    dekliniere_substantiv("NOUN_TUMBLER","Gesperre","s","Gesperre","","neutrum"),
    dekliniere_substantiv("NOUN_UEBERANSTRENGUNG","�beranstrengung","","�beranstrengung","en","feminin"),
    dekliniere_substantiv("NOUN_UMARMUNG","Umarmung","","Umarmung","en","maskulin"),
    dekliniere_substantiv("NOUN_UNFAEHIGKEIT","Unf�higkeit","","","","feminin"),
    dekliniere_substantiv("NOUN_UNSICHTBARKEIT","Unsichtbarkeit","","","","feminin"),
    dekliniere_substantiv("NOUN_VERSTAND","Verstand","es","","","maskulin"),
    dekliniere_substantiv("NOUN_VORFAHR","Vorfahr","en","Vorfahr","en","maskulin"),
    dekliniere_substantiv("NOUN_VORFAHRE","Vorfahr","en","Vorfahr","en","maskulin"),
    dekliniere_substantiv("NOUN_WAERME", "W�rme", "", "", "", "feminin"),
    dekliniere_substantiv("NOUN_WALL", "Wand", "", "W�nd", "e", "feminin"),
    dekliniere_substantiv("NOUN_WATERS_SURFACE", "Wasseroberfl�che", "", "Wasseroberfl�che", "en", "feminin"),
    dekliniere_substantiv("NOUN_WEB","Netz","es","Netz","e","neutrum"),
    dekliniere_substantiv("NOUN_WEG","Weg","es","Weg","e","maskulin"),
    dekliniere_substantiv("NOUN_WHIP", "Lederpeitsche", "", "Lederpeitsche", "en", "feminin", "n"),
    dekliniere_substantiv("NOUN_WIND","Wind","es","Wind","e","maskulin"),
    dekliniere_substantiv("NOUN_WOMAN", "Frau", "", "Frau", "en", "feminin"),
    dekliniere_substantiv("NOUN_WORM", "Wurm", "es", "W�rm", "er", "maskulin"),
    dekliniere_substantiv("NOUN_WRAPPING", "Binde", "", "Binde", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_WURZEL", "Wurzel", "", "Wurzel", "en", "feminin"),
    dekliniere_substantiv("NOUN_ZAHN", "Zahn", "es", "Z�hn", "e", "maskulin"),
    dekliniere_substantiv("NOUN_ZEHE","Zehe","","Zehe","en","feminin"),
    dekliniere_substantiv("NOUN_ZENTRUM","Zentrum","s","Zentren","fremder","neutrum"),
    dekliniere_substantiv("NOUN_ZEUGS","Zeugs","","","","neutrum"),
    dekliniere_substantiv("NOUN_ZILIE","Zilie","","Zilie","en","feminin"),
    dekliniere_substantiv("NOUN_ZISCHEN","Zischen","s","Zischen","","neutrum"),
    dekliniere_substantiv("NOUN_ZORKMID","Zorkmid","","Zorkmid","fremder","maskulin"),
    dekliniere_substantiv("NOUN_ZUKUNFT","Zukunft","","Zuk�nft","e","feminin"),
    dekliniere_substantiv("NOUN_ZUNGE", "Zunge", "", "Zunge", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_ZWANGSJACKE", "Zwangsjacke", "", "Zwangsjacke", "en", "feminin", "en"),
    dekliniere_substantiv("NOUN_ZYTOPLASMA", "Zytoplasma", "", "Zytoplasma", "en", "neutrum"),

    "/* ======================================================= */",
    "/* Hallucinated monsters */",
    "/* ======================================================= */",
    "/* proper names */",
    "/* pre-existing monsters */",
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_GODZILLA", "Godzilla", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_KING_KONG", "King Kong", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_MORGOTH", "Morgoth", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_AUDREY_II", "Audrey II", "feminin"),

    "/* monsters newly added for the German version */",
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_HERR_VON_BOEDEFELD", "Herr von B�defeld", $nom, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_HERR_VON_BOEDEFELD", "Herrn von B�defelds", $gen, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_HERR_VON_BOEDEFELD", "Herrn von B�defeld", [$dat, $akk], $mal, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_BERND_DAS_BROT", "Bernd das Brot", [$nom, $akk], $mal, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_BERND_DAS_BROT", "Bernd des Brots", $gen, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_BERND_DAS_BROT", "Bernd dem Brot", $dat, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_CHILI_DAS_SCHAF", "Chili das Schaf", [$nom, $akk], $fem, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_CHILI_DAS_SCHAF", "Chili des Schafs", $gen, $fem, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_CHILI_DAS_SCHAF", "Chili dem Schaf", $dat, $fem, $sg)],
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_FRANKENSTEIN", "Frankenstein", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_LORELEY", "Loreley", "feminin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_OETZI", "�tzi", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_GRENDEL", "Grendel", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_RUEBEZAHL", "R�bezahl", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_BABA_JAGA", "Baba Jaga", "feminin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_PUMUCKL", "Pumuckl", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_CAPRICORN", "Capricorn", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_GEVATTER_TOD", "Gevatter Tod", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_VAETERCHEN_FROST", "V�terchen Frost", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_KNECHT_RUPRECHT", "Knecht Ruprecht", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_HOMUNKOLOSS", "Homunkoloss", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_DR_CALIGARI", "Dr. Caligari", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_JACK_THE_RIPPER", "Jack the Ripper", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_ET", "E.T.", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_TINKY_WINKY", "Tinky-Winky", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_PITIPLATSCH", "Pitiplatsch", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_SEIFENSTEIN", "Seifenstein", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_VORMAERZ", "Vorm�rz", "maskulin"),
    dekliniere_eigenname("NOUN_HALLU_UNIQUE_BEUTOLOMAEUS", "Beutolom�us", "maskulin"),
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_IRRENHAUS_DER_DRITTE", "Irrenhaus der Dritte",  $nom, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_IRRENHAUS_DER_DRITTE", "Irrenhaus des Dritten", $gen, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_IRRENHAUS_DER_DRITTE", "Irrenhaus dem Dritten", $dat, $mal, $sg)],
    [unregelmaessiges_wort("NOUN_HALLU_UNIQUE_IRRENHAUS_DER_DRITTE", "Irrenhaus den Dritten", $akk, $mal, $sg)],

    "/* translated pre-existing monsters */",
    dekliniere_eigenname("NOUN_HALLU_HORLA", "Horla", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_TYRANNOSAURUS_REX","Tyrannosaurus Rex", "", "Tyrannosaurus Rex", "fremder", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_PTERODAKTYLUS","Pterodaktylus", "", "Pterodaktyl", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_KILLERPINGUIN","Killerpinguin", "s", "Killerpinguin", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_NACHTMAHR","Nachtmahr", "s", "Nachtmahr", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_SCHATTEN","Schatten", "s", "Schatten", "", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_ROBOTER","Roboter", "s", "Roboter", "", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_GRUE","Grue", "s", "Grue", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_SCHLUMPF","Schlumpf", "s", "Schl�mpf", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_KLINGON","Klingon", "s", "Klingon", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_BORG","Borg", "s", "Borg", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_EWOK","Ewok", "s", "Ewok", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_DALEK","Dalek", "s", "Dalek", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_VORLON","Vorlon", "s", "Vorlon", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_PREDATOR","Predator", "s", "Predator", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_TOTORO","Totoro", "s", "Totoro", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_ENT","Ent", "s", "Ent", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_STAATSANWALT","Staatsanwalt", "s", "Staatsanw�lt", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_TERMINATOR","Terminator", "s", "Terminator", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_SCHNATZ","Schnatz", "es", "Schnatz", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_BASILISK","Basilisk", "s", "Basilisk", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_HOFNARR","Hofnarr", "en", "Hofnarr", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_YOUMA","Youma", "s", "Youma", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_RIESENPYGMAEE","Riesenpygm�e", "en", "Riesenpygm�e", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_KILLERTOMATE","Killertomate", "", "Killertomat", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_AMAZONE","Amazone", "", "Amazon", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_HARPYIE","Harpyie", "", "Harpyi", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_VENUSFLIEGENFALLE","Venusfliegenfalle", "", "Venusfliegenfall", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_SIRENE","Sirene", "", "Siren", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_SCHWIEGERMUTTER","Schwiegermutter", "", "Schwiegerm�tt", "er", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_RIESENKAKERLAKE","Riesenkakerlake", "", "Riesenkakerlak", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_GNU","Gnu", "s", "Gnu", "s", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_HOLOGRAMM","Hologramm", "s", "Hologramm", "e", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_EMU","Emu", "s", "Emu", "s", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_NIUF","Nagetier", "s", "Nagetier", "e", "neutrum", "", "in ungew�hnlichem Format"),
    dekliniere_substantiv("NOUN_HALLU_RHINOVIRUS","Rhinovirus", "", "Rhinovir", "en", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_WEIHNACHTSBAUMMONSTER","Weihnachtsbaummonster", "s", "Weihnachtsbaummonster", "", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_KILLERKANINCHEN","Killerkaninchen", "s", "Killerkaninchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_STOSSMICH_ZIEHDICH","Sto�mich-Ziehdich", "s", "Sto�mich-Ziehdich", "s", "neutrum"),
    dekliniere_nominalphrase("NOUN_HALLU_GEFRAESSIGER_PLAPPERKAEFER","Gefr��ig", "Plapperk�fer", "s", "Plapperk�f", "er", "maskulin", "", "von Traal"),
    dekliniere_nominalphrase("NOUN_HALLU_HYDRA","siebenk�pfig", "Hydra", "", "Hydra", "s", "feminin"),
    dekliniere_nominalphrase("NOUN_HALLU_WEISSES_KANINCHEN","wei�", "Kaninchen", "s", "Kaninchen", "", "neutrum"),

    "/* monsters newly added for the German version */",
    dekliniere_substantiv("NOUN_HALLU_ELWETRITSCH","Elwetritsch", "es", "Elwetritsch", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_WOLPERTINGER","Wolpertinger", "s", "Wolpertinger", "", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_STEUERPRUEFER","Steuerpr�fer", "s", "Steuerpr�fer", "", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_GEZ_FAHNDER","GEZ-Fahnder", "s", "GEZ-Fahnder", "", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_GROSSINQUISITOR","Gro�inquisitor", "s", "Gro�inquisitor", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_KLABAUTERMANN","Klabautermann", "s", "Klabauterm�nn", "er", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_DODO","Dodo", "s", "Dodo", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_GRINCH","Grinch", "s", "Grinch", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_SANDMANN","Sandmann", "s", "Sandm�nn", "er", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_MUMMELRATZ","Mummelratz", "es", "Mummelratz", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_APPARATSCHIK","Apparatschik", "s", "Apparatschik", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_ERLKOENIG","Erlk�nig", "s", "Erlk�nig", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_RAEUBER_HOTZENPLOTZ","R�uber", "s", "R�uber", "", "maskulin", "", "Hotzenplotz"),
    dekliniere_substantiv("NOUN_HALLU_SENSENMANN","Sensenmann", "s", "Sensenm�nn", "er", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_MENSCHENFRESSER","Menschenfresser", "s", "Menschenfresser", "", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_RIESENGARTENZWERG","Riesengartenzwerg", "s", "Riesengartenzwerg", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_QUASTENFLOSSER","Quastenflosser", "s", "Quastenflosser", "", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_SAEBELZAHNTIGER","S�belzahntiger", "s", "S�belzahntiger", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_BIBABUTZEMANN","Bi-Ba-Butzemann", "s", "Bi-Ba-Butzem�nn", "er", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_RITTER_DER_KOKOSNUSS","Ritter", "s", "Ritter", "", "maskulin", "", "der Kokosnuss"),
    dekliniere_substantiv("NOUN_HALLU_ZONK","Zonk", "s", "Zonk", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_OTTIFANT","Ottifant", "en", "Ottifant", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_VOGONE","Vogone", "en", "Vogon", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_KLAMMERAFFE","Klammeraffe", "en", "Klammeraff", "en", "maskulin"),
    dekliniere_substantiv("NOUN_STOLLENTROLL", "Stollentroll",  "es", "Stollentroll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_EIDEET","Eideet", "en", "Eideet", "en", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_GRUEFFELO","Gr�ffelo", "s", "Gr�ffelo", "s", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_STOLLENTROLL", "Stollentroll",  "es", "Stollentroll", "e", "maskulin"),
    dekliniere_substantiv("NOUN_HALLU_LESERATTE","Leseratte", "", "Leseratt", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_RIESENWELLE","Riesenwelle", "", "Riesenwell", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_ZAHNFEE","Zahnfee", "", "Zahnfee", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_ROGGENMUHME","Roggenmuhme", "", "Roggenmuhm", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_DOPPELHELIX","Doppelhelix", "", "Doppelhelices", "fremder", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_STEINLAUS","Steinlaus", "", "Steinl�us", "e", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_TRUHE","Truhe", "", "Truh", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_CHIMAERE","Chim�re", "", "Chim�r", "en", "feminin"),
    dekliniere_substantiv("NOUN_HALLU_FAULTIER","Faultier", "s", "Faultier", "e", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_PANTOFFELTIERCHEN","Pantoffeltierchen", "s", "Pantoffeltierchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_UFO","UFO", "s", "UFO", "s", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_KARTOFFELPUE_REH","Kartoffelp�-Reh", "s", "Kartoffelp�-Reh", "e", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_MONDKALB","Mondkalb", "s", "Mondk�lb", "er", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_BLUMENTOPFERD","Blumentopferd", "s", "Blumentopferd", "e", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_TAMAGOCHI", "Tamagochi", "s", "Tamagochi", "s", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_KRUEMELMONSTER","Kr�melmonster", "s", "Kr�melmonster", "", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_SAMS","Sams", "", "Sams", "", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_SCHNABELTIER","Schnabeltier", "s", "Schnabeltier", "e", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_MAINZELMAENNCHEN","Mainzelm�nnchen", "s", "Mainzelm�nnchen", "", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_RHINOGRADENTIUM","Rhinogradentium", "s", "Rhinogradentia", "fremder", "neutrum"),
    dekliniere_substantiv("NOUN_HALLU_UNGEHEUER_VON_LOCH_NESS","Ungeheuer", "s", "Ungeheuer", "", "neutrum", "", "von Loch Ness"),
    dekliniere_nominalphrase("NOUN_HALLU_WEISSER_HAI","wei�", "Hai", "s", "Hai", "e", "maskulin"),
    dekliniere_nominalphrase("NOUN_HALLU_KOPFLOSER_REITER","kopflos", "Reiter", "s", "Reiter", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_HALLU_EINARMIGER_BANDIT","einarmig", "Bandit", "en", "Bandit", "en", "maskulin"),
    dekliniere_nominalphrase("NOUN_HALLU_SCHRUMPFHOERNIGER_SCHNARCHKACKLER","Schrumpfh�rnig", "Schnarchkackler", "s", "Schnarchkackler", "", "maskulin"),
    dekliniere_nominalphrase("NOUN_HALLU_EIERLEGENDE_WOLLMILCHSAU","eierlegend", "Wollmilchsau", "", "Wolchmilchsau", "en", "feminin"),
    dekliniere_nominalphrase("NOUN_HALLU_KLEINE_RAUPE_NIMMERSATT","klein", "Raupe", "", "Raup", "en", "feminin", "", "Nimmersatt"),
    dekliniere_nominalphrase("NOUN_HALLU_UNSICHTBARES_ROSA_EINHORN",["unsichtbar", "rosafarben"], "Einhorn", "s", "Einh�rn", "er", "neutrum"),
    dekliniere_nominalphrase("NOUN_HALLU_FLIEGENDES_SPAGHETTIMONSTER","Fliegend", "Spaghettimonster", "s", "Spaghettimonster", "", "neutrum"),

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
    dekliniere_adjektiv("ADJEKTIV_MAENNLICH", "m�nnlich"),
    dekliniere_adjektiv("ADJEKTIV_WEIBLICH", "weiblich"),
    dekliniere_adjektiv("ADJEKTIV_SAECHLICH", "s�chlich"),
    "",
    "/* Farbennamen */",
    dekliniere_substantiv("NOUN_FARBE_BLACK", "Schwarz", "s", "Schwarz", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_RED", "Rot", "s", "Rot", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_GREEN", "Gr�n", "s", "Gr�n", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_BROWN", "Braun", "s", "Braun", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_BLUE", "Blau", "s", "Blau", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_MAGENTA", "Tiefrot", "s", "Tiefrot", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_CYAN", "T�rkis", "s", "T�rkis", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_GRAY", "Grau", "s", "Grau", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_TRANSPARENT", "Durchsichtigkeit", "", "Durchsichtigkeit", "en", "feminin"),
    dekliniere_substantiv("NOUN_FARBE_ORANGE", "Orange", "s", "Orange", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_BRIGHT_GREEN", "Knallgr�n", "s", "Knallgr�n", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_YELLOW", "Gelb", "s", "Gelb", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_BRIGHT_BLUE", "Blau", "s", "Blau", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_BRIGHT_MAGENTA", "Knallrot", "s", "Knallrot", "", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_BRIGHT_CYAN", "Blaugr�n", "s", "", "Blaugr�n", "neutrum"),
    dekliniere_substantiv("NOUN_FARBE_WHITE", "Wei�", "s", "Wei�", "", "neutrum"),

    dekliniere_adjektiv("ADJEKTIV_FARBE_BLACK","schwarz"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_AMBER","bernsteinfarben"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GOLDEN","golden"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_LIGHT_BLUE","hellblau"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_RED","rot"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GREEN","gr�n"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_SILVER","silbern"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BLUE","blau"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_PURPLE","purpurn"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_WHITE","wei�"),

    dekliniere_adjektiv("ADJEKTIV_FARBE_BROWN","braun"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_MAGENTA","tiefrot"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_CYAN","t�rkis"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GRAY","grau"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_TRANSPARENT","durchsichtig"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_ORANGE","orangen"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BRIGHT_GREEN","knallgr�n"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_YELLOW","gelb"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BRIGHT_BLUE","knallblau"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BRIGHT_MAGENTA","knallrot"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BRIGHT_CYAN","blaugr�n"),
    '/* Halluzinierte "Farben" */',
    dekliniere_adjektiv("ADJEKTIV_FARBE_BANANIG", "bananig"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_BITTER", "bitter"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_DREIECKIG","dreieckig"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_FARBIG", "farbig"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GEBLUEMT", "gebl�mt"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GEFLECKT", "gefleckt"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GEPUNKTET", "gepunktet"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GESTREIFT", "gestreift"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GEWUNDEN", "gewunden"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_INFRAROT", "infrarot"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_KARIERT", "kariert"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_PFEFFERMINZIG", "pfefferminzig"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_PINK", "pink"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_QUADRATISCH","quadratisch"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_ROMANTISCH", "romantisch"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_RUND","rund"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_SALZIG", "salzig"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_SAUER", "sauer"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_SUESS", "s��"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_ULTRAVIOLETT", "ultraviolett"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_UNBUNT", "unbunt"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_VERDREHT", "verdreht"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_ZITRONIG", "zitronig"),

    "",
    "/* 'andere' ist eigentlich ein Pronomen, wird aber wie ein Adjektiv dekliniert */",
    dekliniere_adjektiv("ADJEKTIV_ANDERE","ander"),
    "",
    "/* Adjektive und Substantive f�r Nominalphrasen */",
    dekliniere_substantiv("NOUN_STANGENWAFFE","Stangenwaffe","","Stangenwaffe","en","feminin"),
    dekliniere_adjektiv("ADJEKTIV_GEWOEHNLICH","gew�hnlich"),

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
    "/* foo proof */",
    dekliniere_adjektiv("ADJEKTIV_RUSTPROOF","rostfrei"),
    # dekliniere_adjektiv("ADJEKTIV_ERODEPROOF",""),
    dekliniere_adjektiv("ADJEKTIV_CORRODEPROOF","korrosionsbest�ndig"),
    dekliniere_adjektiv("ADJEKTIV_CORRODEPROOF","korrosionsfest"),
    dekliniere_adjektiv("ADJEKTIV_CORRODEPROOF","korrosionssicher"),
    dekliniere_adjektiv("ADJEKTIV_FIREPROOF","feuerfest"),
    # dekliniere_adjektiv("ADJEKTIV_ROTPROOF",""),
    dekliniere_adjektiv("ADJEKTIV_FIXED","gebunden"),
    dekliniere_adjektiv("ADJEKTIV_FIXED","unverw�stlich"),
    dekliniere_adjektiv("ADJEKTIV_FIXED","impr�gniert"),
    dekliniere_adjektiv("ADJEKTIV_FIXED","fixiert"),
    "/* sonstige Adjektive */",
    dekliniere_adjektiv("ADJEKTIV_ABGESTANDEN","abgestanden"),
    dekliniere_adjektiv("ADJEKTIV_AETZEND","�tzend"),
    dekliniere_adjektiv("ADJEKTIV_AKTUELL","aktuell"),
    dekliniere_adjektiv("ADJEKTIV_ALCHEMISTISCH","alchemistisch"),
    dekliniere_adjektiv("ADJEKTIV_ALT","alt"),
    dekliniere_adjektiv("ADJEKTIV_ANFAENGLICH","anf�nglich"),
    dekliniere_adjektiv("ADJEKTIV_ANGEBRATEN","angebraten"),
    dekliniere_adjektiv("ADJEKTIV_ANGEFAULT","angefault"),
    dekliniere_adjektiv("ADJEKTIV_ANGEKNABBERT","angeknabbert"),
    dekliniere_adjektiv("ADJEKTIV_ANGESENGT","angesengt"),
    dekliniere_adjektiv("ADJEKTIV_ANTIK","antik"),
    dekliniere_adjektiv("ADJEKTIV_ASTREIN","astrein"),
    dekliniere_adjektiv("ADJEKTIV_BARE","blo�"),
    dekliniere_adjektiv("ADJEKTIV_BEHANDSCHUHT","behandschuht"),
    dekliniere_adjektiv("ADJEKTIV_BEISSEND","beissend"),
    dekliniere_adjektiv("ADJEKTIV_BERUFEN","berufen"),
    dekliniere_adjektiv("ADJEKTIV_BEVORZUGT","bevorzugt"),
    dekliniere_adjektiv("ADJEKTIV_BISHERIG","bisherig"),
    dekliniere_adjektiv("ADJEKTIV_BLIND","blind"),
    dekliniere_adjektiv("ADJEKTIV_BLUTRUENSTIG","blutr�nstig"),
    dekliniere_adjektiv("ADJEKTIV_BRENNEND","brennend"),
    dekliniere_adjektiv("ADJEKTIV_BRILLIANT","brilliant"),
    dekliniere_adjektiv("ADJEKTIV_CARNIVOROUS","fleischfressend"),
    dekliniere_adjektiv("ADJEKTIV_CORRODED","korrodiert"),
    dekliniere_adjektiv("ADJEKTIV_DERZEITIG","derzeitig"),
    dekliniere_adjektiv("ADJEKTIV_DISPLACED","disloziert"),
    dekliniere_adjektiv("ADJEKTIV_DRACHENHAEUTIG", "drachenh�utig"),
    dekliniere_adjektiv("ADJEKTIV_EATEN","verspeist"),
    dekliniere_adjektiv("ADJEKTIV_EDELSTEINERN", "edelsteinern"),
    dekliniere_adjektiv("ADJEKTIV_EHRENWERT","ehrenwert"),
    dekliniere_adjektiv("ADJEKTIV_EHRWUERDIG", "ehrw�rdig"),
    dekliniere_adjektiv("ADJEKTIV_EIGENE","eigen"),
    dekliniere_adjektiv("ADJEKTIV_EINFACH","einfach"),
    dekliniere_adjektiv("ADJEKTIV_EINGELEGT","eingelegt"),
    dekliniere_adjektiv("ADJEKTIV_EINIG","einig"),
    dekliniere_adjektiv("ADJEKTIV_EINSTUERZEND","einst�rzend"),
    dekliniere_adjektiv("ADJEKTIV_EISERN", "eisern"),
    dekliniere_adjektiv("ADJEKTIV_EISKALT","eiskalt"),
    dekliniere_adjektiv("ADJEKTIV_ELEKTRISCH","elektrisch"),
    dekliniere_adjektiv("ADJEKTIV_ELEMENTAR","elementar"),
    dekliniere_adjektiv("ADJEKTIV_ENTSTELLT", "entstellt"),
    dekliniere_adjektiv("ADJEKTIV_ENTZUENDET", "entz�ndet"),
    dekliniere_adjektiv("ADJEKTIV_ERSPUERT", "ersp�rt"),
    dekliniere_adjektiv("ADJEKTIV_ERST", "erst"),
    dekliniere_adjektiv("ADJEKTIV_EXPLODIEREND","explodierend"),
    dekliniere_adjektiv("ADJEKTIV_EXPLODIERT","explodiert"),
    dekliniere_adjektiv("ADJEKTIV_FALLEND","fallend"),
    dekliniere_adjektiv("ADJEKTIV_FARBLOS", "farblos"),
    dekliniere_adjektiv("ADJEKTIV_FEST","fest"),
    dekliniere_adjektiv("ADJEKTIV_FETTIG","fettig"),
    dekliniere_adjektiv("ADJEKTIV_FEUCHT","feucht"),
    dekliniere_adjektiv("ADJEKTIV_FINANZIELL", "finanziell"),
    dekliniere_adjektiv("ADJEKTIV_FLECKIG", "fleckig"),
    dekliniere_adjektiv("ADJEKTIV_FLEISCHERN", "fleischern"),
    dekliniere_adjektiv("ADJEKTIV_FLUESSIG","fl�ssig"),
    dekliniere_adjektiv("ADJEKTIV_FREI","frei"),
    dekliniere_adjektiv("ADJEKTIV_FRITTIERT","frittiert"),
    dekliniere_adjektiv("ADJEKTIV_GANZ","ganz"),
    dekliniere_adjektiv("ADJEKTIV_GEBRATEN","gebraten"),
    dekliniere_adjektiv("ADJEKTIV_GEFAEHRLICH","gef�hrlich"),
    dekliniere_adjektiv("ADJEKTIV_GEFANGEN", "gefangen"),
    dekliniere_adjektiv("ADJEKTIV_GEFUEHRT","gef�hrt"),
    dekliniere_adjektiv("ADJEKTIV_GEKOCHT","gekocht"),
    dekliniere_adjektiv("ADJEKTIV_GENOZIDIOESE","genozidi�s"),
    dekliniere_adjektiv("ADJEKTIV_GEOEFFNET","ge�ffnet"),
    dekliniere_adjektiv("ADJEKTIV_GERAEUCHERT","ger�uchert"),
    dekliniere_adjektiv("ADJEKTIV_GERUCHLOS","geruchlos"),
    dekliniere_adjektiv("ADJEKTIV_GESAMT","gesamt"),
    dekliniere_adjektiv("ADJEKTIV_GESCHAETZT","gesch�tzt"),
    dekliniere_adjektiv("ADJEKTIV_GESCHEITERT", "gescheitert"),
    dekliniere_adjektiv("ADJEKTIV_GESCHLOSSEN","geschlossen"),
    dekliniere_adjektiv("ADJEKTIV_GESPALTEN","gespalten"),
    dekliniere_adjektiv("ADJEKTIV_GESTOHLEN","gestohlen"),
    dekliniere_adjektiv("ADJEKTIV_GETRAGEN","getragen"),
    dekliniere_adjektiv("ADJEKTIV_GETROCKNET","getrocknet"),
    dekliniere_adjektiv("ADJEKTIV_GEWALTIG", "gewaltig"),
    dekliniere_adjektiv("ADJEKTIV_GEWORFEN","geworfen"),
    dekliniere_adjektiv("ADJEKTIV_GIFTGETRAENKT","giftgetr�nkt"),
    dekliniere_adjektiv("ADJEKTIV_GIFTIG","giftig"),
    dekliniere_adjektiv("ADJEKTIV_GLUEHEND","gl�hend"),
    dekliniere_adjektiv("ADJEKTIV_GNAEDIG", "gn�dig"),
    dekliniere_adjektiv("ADJEKTIV_GOTTLOS","gottlos"),
    dekliniere_adjektiv("ADJEKTIV_GREASED","gefettet"),
    dekliniere_adjektiv("ADJEKTIV_GROSS","gro�"),
    dekliniere_adjektiv("ADJEKTIV_GUT","gut"),
    dekliniere_adjektiv("ADJEKTIV_HAESSLICH", "h�sslich"),
    dekliniere_adjektiv("ADJEKTIV_HAUSGEMACHT","hausgemacht"),
    dekliniere_adjektiv("ADJEKTIV_HERABFALLEND", "herabfallend"),
    dekliniere_adjektiv("ADJEKTIV_HERABSTUERZEND", "herabst�rzend"),
    dekliniere_adjektiv("ADJEKTIV_HOELZERN","h�lzern"),
    dekliniere_adjektiv("ADJEKTIV_IMAGINAER","imagin�r"),
    dekliniere_adjektiv("ADJEKTIV_INVISIBLE","unsichtbar"),
    dekliniere_adjektiv("ADJEKTIV_JUENGST","j�ngst"),
    dekliniere_adjektiv("ADJEKTIV_KANDIERT","kandiert"),
    dekliniere_adjektiv("ADJEKTIV_KAPUTT","kaputt"),
    dekliniere_adjektiv("ADJEKTIV_KLEIN","klein"),
    dekliniere_adjektiv("ADJEKTIV_KNOECHERN","kn�chern"),
    dekliniere_adjektiv("ADJEKTIV_KOENIGLICH", "k�niglich"),
    dekliniere_adjektiv("ADJEKTIV_KONTAMINIERT","kontaminiert"),
    dekliniere_adjektiv("ADJEKTIV_KRAEFTIGER","kr�ftiger"),
    dekliniere_adjektiv("ADJEKTIV_KRAFTLOS","kraftlos"),
    dekliniere_adjektiv("ADJEKTIV_KRAFTVOLL", "kraftvoll"),
    dekliniere_adjektiv("ADJEKTIV_KUEHLEND","k�hlend"),
    dekliniere_adjektiv("ADJEKTIV_KUNSTSTOFFIG", "kunststoffig"),
    dekliniere_adjektiv("ADJEKTIV_KUPFERN", "kupfern"),
    dekliniere_adjektiv("ADJEKTIV_LATENT", "latent"),
    dekliniere_adjektiv("ADJEKTIV_LECKER","lecker"),
    dekliniere_adjektiv("ADJEKTIV_LEER","leer"),
    dekliniere_adjektiv("ADJEKTIV_LEICHT","leicht"),
    dekliniere_adjektiv("ADJEKTIV_LETZT","letzt"),
    dekliniere_adjektiv("ADJEKTIV_LIEGEND","liegend"),
    dekliniere_adjektiv("ADJEKTIV_LINK","link"),
    dekliniere_adjektiv("ADJEKTIV_LIT","entz�ndet"),
    dekliniere_adjektiv("ADJEKTIV_MAGIE_ABSORBIEREND","magie-absorbierend"),
    dekliniere_adjektiv("ADJEKTIV_MAGISCH","magisch"),
    dekliniere_adjektiv("ADJEKTIV_MARKIERT","markiert"),
    dekliniere_adjektiv("ADJEKTIV_METALLEN","metallen"),
    dekliniere_adjektiv("ADJEKTIV_MINERALISCH","mineralisch"),
    dekliniere_adjektiv("ADJEKTIV_MITHRILERN", "mithrilern"),
    dekliniere_adjektiv("ADJEKTIV_MODERAT","moderat"),
    dekliniere_adjektiv("ADJEKTIV_MYTHICAL","sagenhaft"),
    dekliniere_adjektiv("ADJEKTIV_NACHGEWACHSEN","nachgewachsen"),
    dekliniere_adjektiv("ADJEKTIV_NAHE","nah"),
    dekliniere_adjektiv("ADJEKTIV_NAMENLOS","namenlos"),
    dekliniere_adjektiv("ADJEKTIV_NATUERLICH","nat�rlich"),
    dekliniere_adjektiv("ADJEKTIV_NEU","neu"),
    dekliniere_adjektiv("ADJEKTIV_NORMAL","normal"),
    dekliniere_adjektiv("ADJEKTIV_ORGANISCH", "organisch"),
    dekliniere_adjektiv("ADJEKTIV_PAPIEREN","papieren"),
    dekliniere_adjektiv("ADJEKTIV_PLATINERN", "platinern"),
    dekliniere_adjektiv("ADJEKTIV_PLUMP","plump"),
    dekliniere_adjektiv("ADJEKTIV_POISONED","vergiftet"),
    dekliniere_adjektiv("ADJEKTIV_POOR","arm"),
    dekliniere_adjektiv("ADJEKTIV_PSIONISCH","psionisch"),
    dekliniere_adjektiv("ADJEKTIV_PUERIERT","p�riert"),
    dekliniere_adjektiv("ADJEKTIV_RAETSELHAFT", "raetselhaft"),
    dekliniere_adjektiv("ADJEKTIV_RASIERMESSERSCHARF","rasiermesserscharf"),
    dekliniere_adjektiv("ADJEKTIV_RECHT","recht"),
    dekliniere_adjektiv("ADJEKTIV_REICH","reich"),
    dekliniere_adjektiv("ADJEKTIV_RENEGADE", "abtr�nnig"),
    dekliniere_adjektiv("ADJEKTIV_RIESIG","riesig"),
    dekliniere_adjektiv("ADJEKTIV_ROSTIG","rostig"),
    dekliniere_adjektiv("ADJEKTIV_SADDLED","gesattelt"),
    dekliniere_adjektiv("ADJEKTIV_SAEUREHALTIG","s�urehaltig"),
    dekliniere_adjektiv("ADJEKTIV_SAUTIERT","sautiert"),
    dekliniere_adjektiv("ADJEKTIV_SCHLIESSEND","schlie�end"),

    dekliniere_adjektiv("ADJEKTIV_SCHOEN","sch�n"),
    dekliniere_adjektiv("ADJEKTIV_SCHWACH","schwach"),
    dekliniere_adjektiv("ADJEKTIV_SCHWER", "schwer"),
    dekliniere_adjektiv("ADJEKTIV_SCHWUNGVOLL", "schwungvoll"),
    dekliniere_adjektiv("ADJEKTIV_SELTSAM","seltsam"),
    dekliniere_adjektiv("ADJEKTIV_SIEDEND", "siedend"),
    dekliniere_adjektiv("ADJEKTIV_SLIPPERY", "rutschig"),
    dekliniere_adjektiv("ADJEKTIV_SOLCH","solch"),
    dekliniere_adjektiv("ADJEKTIV_SPEZIELL", "speziell"),
    dekliniere_adjektiv("ADJEKTIV_STATTLICH", "stattlich"),
    dekliniere_adjektiv("ADJEKTIV_STECHEND","stechend"),
    dekliniere_adjektiv("ADJEKTIV_STEINERN", "steinern"),
    dekliniere_adjektiv("ADJEKTIV_STOFFIG","stoffig"),
    dekliniere_adjektiv("ADJEKTIV_STRAHLEND","strahlend"),
    dekliniere_adjektiv("ADJEKTIV_SUESS_SAUER","s�ss-sauer"),
    dekliniere_adjektiv("ADJEKTIV_TOEDLICH","t�dlich"),
    dekliniere_adjektiv("ADJEKTIV_UEBERBACKEN", "�berbacken"),
    dekliniere_adjektiv("ADJEKTIV_UNATTRAKTIV", "unattraktiv"),
    dekliniere_adjektiv("ADJEKTIV_UNBEZAHLT", "unbezahlt"),
    dekliniere_adjektiv("ADJEKTIV_UNGESCHICKT","ungeschickt"),
    dekliniere_adjektiv("ADJEKTIV_UNGESCHUETZT", "ungesch�tzt"),
    dekliniere_adjektiv("ADJEKTIV_UNSICHTBAR","unsichtbar"),
    dekliniere_adjektiv("ADJEKTIV_UNTER","unter"),
    dekliniere_adjektiv("ADJEKTIV_VERBRENNEND","verbrennend"),
    dekliniere_adjektiv("ADJEKTIV_VERDUENNT","verd�nnt"),
    dekliniere_adjektiv("ADJEKTIV_VERFAULT", "verfault"),
    dekliniere_adjektiv("ADJEKTIV_VERGAMMELT","vergammelt"),
    dekliniere_adjektiv("ADJEKTIV_VERLANGT", "verlangt"),
    dekliniere_adjektiv("ADJEKTIV_VERLETZT","verletzt"),
    dekliniere_adjektiv("ADJEKTIV_VERMINDERT", "vermindert"),
    dekliniere_adjektiv("ADJEKTIV_VERROSTET", "verrostet"),
    dekliniere_adjektiv("ADJEKTIV_VERSENGT", "versengt"),
    dekliniere_adjektiv("ADJEKTIV_VERSTECKT", "versteckt"),
    dekliniere_adjektiv("ADJEKTIV_VERSTEINERND","versteinernd"),
    dekliniere_adjektiv("ADJEKTIV_VERZERRT","verzerrt"),
    dekliniere_adjektiv("ADJEKTIV_WACH", "wach"),
    dekliniere_adjektiv("ADJEKTIV_WAECHSERN","w�chsern"),
    dekliniere_adjektiv("ADJEKTIV_WEITENTFERNT","weitentfernt"),
    dekliniere_adjektiv("ADJEKTIV_WEITER", "weiter"),
    dekliniere_adjektiv("ADJEKTIV_WELTBERUEHMT", "weltber�hmt"),
    dekliniere_adjektiv("ADJEKTIV_WIEDERAUFBEREITET","wiederaufbereitet"),
    dekliniere_adjektiv("ADJEKTIV_WOHLVERDIENT","wohlverdient"),
    dekliniere_adjektiv("ADJEKTIV_WUETEND","w�tend"),
    dekliniere_adjektiv("ADJEKTIV_ZERSPRUNGEN", "zersprungen"),
    dekliniere_adjektiv("ADJEKTIV_ZUSCHNAPPEND", "zuschnappend"),
    dekliniere_adjektiv("ADJEKTIV_ZUSAETZLICH", "zus�tzlich"),
    dekliniere_adjektiv("ADJEKTIV_ZWEIHAENDIG", "zweih�ndig"),
    dekliniere_adjektiv("ADJEKTIV_ZWEIHAENDISCH","zweih�ndisch"),

    dekliniere_adjektiv("ADJEKTIV_FARBE_BRAUN","braun"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_DARK_RED","dunkelrot"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_GOLDEN","golden"),
    dekliniere_adjektiv("ADJEKTIV_FARBE_SILBERN", "silbern"),
    dekliniere_adjektiv("ADJEKTIV_ART_GLAESERN", "gl�sern"),
    dekliniere_adjektiv("ADJEKTIV_ART_LEDERN","ledern"),

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
