
require 'test/unit' 
require 'german_verb' 


class TestVerb < Test::Unit::TestCase
  @output_levels = Test::Unit::UI::VERBOSE

  def testVerb
    machen = Verb.verb("", "machen")

    assert(machen.singular.singular?)
    assert(machen.plural.plural?)
    assert(machen.praesens.praesens?)
    assert(machen.praeteritum.praeteritum?)
    assert(machen.indikativ.indikativ?)
    assert(machen.konjunktiv.konjunktiv?)
    assert(machen.erstePerson.erstePerson?)
    assert(machen.zweitePerson.zweitePerson?)
    assert(machen.drittePerson.drittePerson?)
  end

  def testUmgelautet
    assert_equal("h�t", Verb.umlaute("hat"))
    assert_equal("brauch", Verb.umlaute("brauch"))
    assert_equal("f�ll", Verb.umlaute("fall"))
    assert_equal("l�ss", Verb.umlaute("lass"))
    assert_equal("l�uf", Verb.umlaute("lauf"))
    assert_equal("��", Verb.umlaute("a�"))
  end

  def testRegelmaessigEinfacheZeiten
    machen = Verb.verb("", "machen")

    assert_equal("machen", machen.infinitiv)
    assert_equal("VERB_MACHEN", machen.kennung)

    assert_equal("mach", machen.singular.imperativ)
    assert_equal("macht", machen.plural.imperativ)

    assert_equal("machst", machen.singular.form)

    checkVerbPraesensKonjunktiv(machen, ["mache", "machest", "mache", "machen", "machet", "machen"])

    assert_equal("machte", machen.erstePerson.singular.indikativ.praeteritum.form)
    assert_equal("machtest", machen.zweitePerson.indikativ.praeteritum.form)
    assert_equal("machte", machen.drittePerson.indikativ.praeteritum.form)

    assert_equal("machtest", machen.zweitePerson.indikativ.praeteritum.form)

    assert_equal("machend", machen.partizip_praesens)
    assert_equal("gemacht", machen.partizip_perfekt)
  end

  def testRegelmaessigZusammengesetzteZeiten
  end

  def testHaben
    haben = Verb.verb("", "haben")

    assert_equal("haben", haben.infinitiv)
    assert_equal("hab", haben.singular.imperativ)
    assert_equal("habt", haben.plural.imperativ)
    assert_equal("habend", haben.partizip_praesens)
    assert_equal("gehabt", haben.partizip_perfekt)

    checkVerbPraesens(haben, ["habe", "hast", "hat", "haben", "habt", "haben"])
    checkVerbPraesensKonjunktiv(haben, ["habe", "habest", "habe", "haben", "habet", "haben"])
    checkVerbPraeteritum(haben, ["hatte", "hattest", "hatte", "hatten", "hattet", "hatten"])
    checkVerbPraeteritumKonjunktiv(haben, ["h�tte", "h�ttest", "h�tte", "h�tten", "h�ttet", "h�tten"])
  end

  def testSein
    sein = Verb.verb("", "sein")

    assert_equal("sein", sein.infinitiv)
    assert_equal("sei", sein.singular.imperativ)
    assert_equal("seit", sein.plural.imperativ)
    assert_equal("seiend", sein.partizip_praesens)
    assert_equal("gewesen", sein.partizip_perfekt)

    checkVerbPraesens(sein, ["bin", "bist", "ist", "sind", "seid", "sind"])
    checkVerbPraesensKonjunktiv(sein, ["sei", "seiest", "sei", "seien", "seiet", "seien"])
    checkVerbPraeteritum(sein, ["war", "warst", "war", "waren", "wart", "waren"])
  end

  def testWerden
    verb = Verb.verb("", "werden")

    assert_equal("werden", verb.infinitiv)
    assert_equal("werde", verb.singular.imperativ)
    assert_equal("werdet", verb.plural.imperativ)
    assert_equal("werdend", verb.partizip_praesens)
    assert_equal("geworden", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["werde", "wirst", "wird", "werden", "werdet", "werden"])
    checkVerbPraesensKonjunktiv(verb, ["werde", "werdest", "werde", "werden", "werdet", "werden"])
    checkVerbPraeteritum(verb, ["wurde", "wurdest", "wurde", "wurden", "wurdet", "wurden"])
    checkVerbPraeteritumKonjunktiv(verb, ["w�rde", "w�rdest", "w�rde", "w�rden", "w�rdet", "w�rden"])
  end

  def testGehen
    verb = Verb.verb("", "gehen")

    assert_equal("gehen", verb.infinitiv)
    assert_equal("geh", verb.singular.imperativ)
    assert_equal("geht", verb.plural.imperativ)
    assert_equal("gehend", verb.partizip_praesens)
    assert_equal("gegangen", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["gehe", "gehst", "geht", "gehen", "geht", "gehen"])
    checkVerbPraesensKonjunktiv(verb, ["gehe", "gehest", "gehe", "gehen", "gehet", "gehen"])
    checkVerbPraeteritum(verb, ["ging", "gingst", "ging", "gingen", "gingt", "gingen"])
    checkVerbPraeteritumKonjunktiv(verb, ["ginge", "gingest", "ginge", "gingen", "ginget", "gingen"])
  end

  def testFallen
    verb = Verb.verb("","fallen")

    assert_equal("fallen", verb.infinitiv)
    assert_equal("fall", verb.singular.imperativ)
    assert_equal("fallt", verb.plural.imperativ)
    assert_equal("fallend", verb.partizip_praesens)
    assert_equal("gefallen", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["falle", "f�llst", "f�llt", "fallen", "fallt", "fallen"])
    checkVerbPraesensKonjunktiv(verb, ["falle", "fallest", "falle", "fallen", "fallet", "fallen"])
    checkVerbPraeteritum(verb, ["fiel", "fielst", "fiel", "fielen", "fielt", "fielen"])
    checkVerbPraeteritumKonjunktiv(verb, ["fiele", "fielest", "fiele", "fielen", "fielet", "fielen"])
  end

  def testPrintVerb
    verb = VerbUnregelmaessig.new("hei�", "hie�", "hei�")
    verb = VerbUnregelmaessig.new("frier", "fror", "fror")
    verb = VerbUnregelmaessig.new("lauf","lief","lauf")
    verb = Verb.verb("","essen")

    puts
    puts "Infinitiv: "+ verb.infinitiv
    puts "E-Erweiterung: " + verb.e_erweiterung.to_s
    #puts "Umlaut: " + verb.umlaut.to_s
    puts
    puts "Imperativ"
    puts "Singular:  "+ verb.singular.imperativ
    puts "Plural:    "+ verb.plural.imperativ
    puts
    puts "Partizip Pr�sens: "+verb.partizip_praesens
    puts "Partizip Perfekt: " + verb.partizip_perfekt
    puts

    puts "Pr�sens Indikativ"
    verb.praesens.indikativ
    printFormen(verb)
    puts

    puts "Pr�sens Konjunktiv"
    verb.praesens.konjunktiv
    printFormen(verb)
    puts

    puts "Pr�teritum Indikativ"
    verb.praeteritum.indikativ
    printFormen(verb)
    puts

    puts "Pr�teritum Konjunktiv"
    verb.praeteritum.konjunktiv
    printFormen(verb)
    puts
  end

  def printFormen(verb)
    puts "ich "+verb.singular.erstePerson.form
    puts "du  "+verb.singular.zweitePerson.form
    puts "er  "+verb.singular.drittePerson.form
    puts "wir "+verb.plural.erstePerson.form
    puts "ihr "+verb.plural.zweitePerson.form
    puts "sie "+verb.plural.drittePerson.form
  end

  def testOeffnen
	  # http://www.canoo.net/services/OnlineGrammar/InflectionRules/FRegeln-V/Texte/e-Erweiterung.html
    verb = Verb.verb("", "�ffnen")

    assert_equal("�ffnen", verb.infinitiv)
    assert_equal("�ffne", verb.singular.imperativ)
    assert_equal("�ffnet", verb.plural.imperativ)
    assert_equal("�ffnend", verb.partizip_praesens)
    assert_equal("ge�ffnet", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["�ffne", "�ffnest", "�ffnet", "�ffnen", "�ffnet", "�ffnen"])
    checkVerbPraesensKonjunktiv(verb, ["�ffne", "�ffnest", "�ffne", "�ffnen", "�ffnet", "�ffnen"])
    checkVerbPraeteritum(verb, ["�ffnete", "�ffnetest", "�ffnete", "�ffneten", "�ffnetet", "�ffneten"])
    checkVerbPraeteritumKonjunktiv(verb, ["�ffnete", "�ffnetest", "�ffnete", "�ffneten", "�ffnetet", "�ffneten"])
  end

  def testFinden
    verb = Verb.verb("", "finden")

    assert_equal("finden", verb.infinitiv)
    assert_equal("finde", verb.singular.imperativ)
    assert_equal("findet", verb.plural.imperativ)
    assert_equal("findend", verb.partizip_praesens)
    assert_equal("gefunden", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["finde", "findest", "findet", "finden", "findet", "finden"])
    checkVerbPraesensKonjunktiv(verb, ["finde", "findest", "finde", "finden", "findet", "finden"])
    checkVerbPraeteritum(verb, ["fand", "fandst", "fand", "fanden", "fandet", "fanden"])
    checkVerbPraeteritumKonjunktiv(verb, ["f�nde", "f�ndest", "f�nde", "f�nden", "f�ndet", "f�nden"])
  end

  def testSetzen
    verb = Verb.verb("", "setzen")

    assert_equal("setzen", verb.infinitiv)
    assert_equal("setz", verb.singular.imperativ)
    assert_equal("setzt", verb.plural.imperativ)
    assert_equal("setzend", verb.partizip_praesens)
    assert_equal("gesetzt", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["setze", "setzt", "setzt", "setzen", "setzt", "setzen"])
    checkVerbPraesensKonjunktiv(verb, ["setze", "setzest", "setze", "setzen", "setzet", "setzen"])
    checkVerbPraeteritum(verb, ["setzte", "setztest", "setzte", "setzten", "setztet", "setzten"])
    checkVerbPraeteritumKonjunktiv(verb, ["setzte", "setztest", "setzte", "setzten", "setztet", "setzten"])
  end

  def testHeissen
    verb = Verb.verb("", "hei�en")

    assert_equal("hei�en", verb.infinitiv)
    assert_equal("hei�", verb.singular.imperativ)
    assert_equal("hei�t", verb.plural.imperativ)
    assert_equal("hei�end", verb.partizip_praesens)
    assert_equal("gehei�en", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["hei�e", "hei�t", "hei�t", "hei�en", "hei�t", "hei�en"])
    checkVerbPraesensKonjunktiv(verb, ["hei�e", "hei�est", "hei�e", "hei�en", "hei�et", "hei�en"])
    checkVerbPraeteritum(verb, ["hie�", "hie�est", "hie�", "hie�en", "hie�t", "hie�en"])
    checkVerbPraeteritumKonjunktiv(verb, ["hie�e", "hie�est", "hie�e", "hie�en", "hie�et", "hie�en"])
  end

  def testFangen
    verb = Verb.verb("", "fangen")

    assert_equal("fangen", verb.infinitiv)
    assert_equal("fang", verb.singular.imperativ)
    assert_equal("fangt", verb.plural.imperativ)
    assert_equal("fangend", verb.partizip_praesens)
    assert_equal("gefangen", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["fange", "f�ngst", "f�ngt", "fangen", "fangt", "fangen"])
    checkVerbPraesensKonjunktiv(verb, ["fange", "fangest", "fange", "fangen", "fanget", "fangen"])
    checkVerbPraeteritum(verb, ["fing", "fingst", "fing", "fingen", "fingt", "fingen"])
    checkVerbPraeteritumKonjunktiv(verb, ["finge", "fingest", "finge", "fingen", "finget", "fingen"])
  end

  def testKoennen
    verb = Verb.verb("", "k�nnen")

    assert_equal("k�nnen", verb.infinitiv)
    assert_equal("", verb.singular.imperativ)
    assert_equal("", verb.plural.imperativ)
    assert_equal("k�nnend", verb.partizip_praesens)
    assert_equal("gekonnt", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["kann", "kannst", "kann", "k�nnen", "k�nnt", "k�nnen"])
    checkVerbPraesensKonjunktiv(verb, ["k�nne", "k�nnest", "k�nne", "k�nnen", "k�nnet", "k�nnen"])
    checkVerbPraeteritum(verb, ["konnte", "konntest", "konnte", "konnten", "konntet", "konnten"])
    checkVerbPraeteritumKonjunktiv(verb, ["k�nnte", "k�nntest", "k�nnte", "k�nnten", "k�nntet", "k�nnten"])
  end

  def testNehmen
    verb = Verb.verb("","nehmen")

    assert_equal("nehmen", verb.infinitiv)
    assert_equal("nimm", verb.singular.imperativ)
    assert_equal("nehmt", verb.plural.imperativ)
    assert_equal("nehmend", verb.partizip_praesens)
    assert_equal("genommen", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["nehme", "nimmst", "nimmt", "nehmen", "nehmt", "nehmen"])
    checkVerbPraesensKonjunktiv(verb, ["nehme", "nehmest", "nehme", "nehmen", "nehmet", "nehmen"])
    checkVerbPraeteritum(verb, ["nahm", "nahmst", "nahm", "nahmen", "nahmt", "nahmen"])
    checkVerbPraeteritumKonjunktiv(verb, ["n�hme", "n�hmest", "n�hme", "n�hmen", "n�hmet", "n�hmen"])
  end

  def testTreten
    verb = Verb.verb("", "treten")

    assert_equal("treten", verb.infinitiv)
    assert_equal("tritt", verb.singular.imperativ)
    assert_equal("tretet", verb.plural.imperativ)
    assert_equal("tretend", verb.partizip_praesens)
    assert_equal("getreten", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["trete", "trittst", "tritt", "treten", "tretet", "treten"])
    checkVerbPraesensKonjunktiv(verb, ["trete", "tretest", "trete", "treten", "tretet", "treten"])
    checkVerbPraeteritum(verb, ["trat", "tratst", "trat", "traten", "tratet", "traten"])
    checkVerbPraeteritumKonjunktiv(verb, ["tr�te", "tr�test", "tr�te", "tr�ten", "tr�tet", "tr�ten"])
  end

  def testLassen
    verb = Verb.verb("", "lassen")

    assert_equal("lassen", verb.infinitiv)
    assert_equal("lass", verb.singular.imperativ)
    assert_equal("lasst", verb.plural.imperativ)
    assert_equal("lassend", verb.partizip_praesens)
    assert_equal("gelassen", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["lasse", "l�sst", "l�sst", "lassen", "lasst", "lassen"])
    #checkVerbPraesensKonjunktiv(verb, ["", "", "", "", "", ""])
    #checkVerbPraeteritum(verb, ["", "", "", "", "", ""])
    #checkVerbPraeteritumKonjunktiv(verb, ["", "", "", "", "", ""])
  end

  def testEssen
    verb = Verb.verb("", "essen")

    assert_equal("essen", verb.infinitiv)
    assert_equal("iss", verb.singular.imperativ)
    assert_equal("esst", verb.plural.imperativ)
    assert_equal("essend", verb.partizip_praesens)
    assert_equal("gegessen", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["esse", "isst", "isst", "essen", "esst", "essen"])
    checkVerbPraesensKonjunktiv(verb, ["esse", "essest", "esse", "essen", "esset", "essen"])
    checkVerbPraeteritum(verb, ["a�", "a�est", "a�", "a�en", "a�t", "a�en"])
    checkVerbPraeteritumKonjunktiv(verb, ["��e", "��est", "��e", "��en", "��et", "��en"])
  end

  def estVerb
    verb = Verb.verb("", "en")

    assert_equal("", verb.infinitiv)
    assert_equal("", verb.singular.imperativ)
    assert_equal("", verb.plural.imperativ)
    assert_equal("", verb.partizip_praesens)
    assert_equal("", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["", "", "", "", "", ""])
    checkVerbPraesensKonjunktiv(verb, ["", "", "", "", "", ""])
    checkVerbPraeteritum(verb, ["", "", "", "", "", ""])
    checkVerbPraeteritumKonjunktiv(verb, ["", "", "", "", "", ""])
  end

  def checkVerbPraeteritum(verb, formen)
    verb.praeteritum.indikativ
    checkVerbFormen(verb, formen)
  end

  def checkVerbPraeteritumKonjunktiv(verb, formen)
    verb.praeteritum.konjunktiv
    checkVerbFormen(verb, formen)
  end

  def checkVerbPraesens(verb, formen)
    verb.praesens.indikativ
    checkVerbFormen(verb, formen)
  end

  def checkVerbPraesensKonjunktiv(verb, formen)
    verb.praesens.konjunktiv
    checkVerbFormen(verb, formen)
  end

  def checkVerbFormen(verb, formen)
    assert_equal(formen[0], verb.singular.erstePerson.form)
    assert_equal(formen[1], verb.singular.zweitePerson.form)
    assert_equal(formen[2], verb.singular.drittePerson.form)

    assert_equal(formen[3], verb.plural.erstePerson.form)
    assert_equal(formen[4], verb.plural.zweitePerson.form)
    assert_equal(formen[5], verb.plural.drittePerson.form)
  end
end
