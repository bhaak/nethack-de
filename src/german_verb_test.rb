
require 'test/unit' 
require 'german_verb' 

# from 'watir/testUnitAddons'
module Test::Unit::Assertions
	def assert_false(boolean, message=nil)
		_wrap_assertion do
			assert_block("assert should not be called with a block.") { !block_given? }
			assert_block(build_message(message, "<?> is not false.", boolean)) { !boolean }
		end
	end
end # module test

class TestVerb < Test::Unit::TestCase
  @output_levels = Test::Unit::UI::VERBOSE

  def testPrintVerb
    verb = Verb.verb("","empfangen")

    puts
    puts "Infinitiv: "+ verb.infinitiv
    puts "E-Erweiterung: " + verb.e_erweiterung.to_s
    puts "Umlaut: " + verb.umlaut.to_s
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

  def testDenken
    verb = Verb.verb("", "denken")

    assert_equal("denken", verb.infinitiv)
    assert_equal("denk", verb.singular.imperativ)
    assert_equal("denkt", verb.plural.imperativ)
    assert_equal("denkend", verb.partizip_praesens)
    assert_equal("gedacht", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["denke", "denkst", "denkt", "denken", "denkt", "denken"])
    checkVerbPraesensKonjunktiv(verb, ["denke", "denkest", "denke", "denken", "denket", "denken"])
    checkVerbPraeteritum(verb, ["dachte", "dachtest", "dachte", "dachten", "dachtet", "dachten"])
    checkVerbPraeteritumKonjunktiv(verb, ["d�chte", "d�chtest", "d�chte", "d�chten", "d�chtet", "d�chten"])
  end

  def testScheinen
    verb = Verb.verb("", "scheinen")

    assert_equal("scheinen", verb.infinitiv)
    assert_equal("schein", verb.singular.imperativ)
    assert_equal("scheint", verb.plural.imperativ)
    assert_equal("scheinend", verb.partizip_praesens)
    assert_equal("geschienen", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["scheine", "scheinst", "scheint", "scheinen", "scheint", "scheinen"])
    checkVerbPraesensKonjunktiv(verb, ["scheine", "scheinest", "scheine", "scheinen", "scheinet", "scheinen"])
    checkVerbPraeteritum(verb, ["schien", "schienst", "schien", "schienen", "schient", "schienen"])
    checkVerbPraeteritumKonjunktiv(verb, ["schiene", "schienest", "schiene", "schienen", "schienet", "schienen"])
  end

  def testBrennen
    verb = Verb.verb("", "brennen")

    assert_equal("brennen", verb.infinitiv)
    assert_equal("brenn", verb.singular.imperativ)
    assert_equal("brennt", verb.plural.imperativ)
    assert_equal("brennend", verb.partizip_praesens)
    assert_equal("gebrannt", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["brenne", "brennst", "brennt", "brennen", "brennt", "brennen"])
    checkVerbPraesensKonjunktiv(verb, ["brenne", "brennest", "brenne", "brennen", "brennet", "brennen"])
    checkVerbPraeteritum(verb, ["brannte", "branntest", "brannte", "brannten", "branntet", "brannten"])
    checkVerbPraeteritumKonjunktiv(verb, ["brennte", "brenntest", "brennte", "brennten", "brenntet", "brennten"])
  end

  def testVerbrennen
    verb = Verb.verb("", "verbrennen")

    assert_equal("verbrennen", verb.infinitiv)
    assert_equal("verbrenn", verb.singular.imperativ)
    assert_equal("verbrennt", verb.plural.imperativ)
    assert_equal("verbrennend", verb.partizip_praesens)
    assert_equal("verbrannt", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["verbrenne", "verbrennst", "verbrennt", "verbrennen", "verbrennt", "verbrennen"])
    checkVerbPraesensKonjunktiv(verb, ["verbrenne", "verbrennest", "verbrenne", "verbrennen", "verbrennet", "verbrennen"])
    checkVerbPraeteritum(verb, ["verbrannte", "verbranntest", "verbrannte", "verbrannten", "verbranntet", "verbrannten"])
    checkVerbPraeteritumKonjunktiv(verb, ["verbrennte", "verbrenntest", "verbrennte", "verbrennten", "verbrenntet", "verbrennten"])
  end

  def testSpazieren
    verb = Verb.verb("", "spazieren")

    assert_equal("spazieren", verb.infinitiv)
    assert_equal("spazier", verb.singular.imperativ)
    assert_equal("spaziert", verb.plural.imperativ)
    assert_equal("spazierend", verb.partizip_praesens)
    assert_equal("spaziert", verb.partizip_perfekt)

    #checkVerbPraesens(verb, ["", "", "", "", "", ""])
    #checkVerbPraesensKonjunktiv(verb, ["", "", "", "", "", ""])
    #checkVerbPraeteritum(verb, ["", "", "", "", "", ""])
    #checkVerbPraeteritumKonjunktiv(verb, ["", "", "", "", "", ""])
  end

  def testWerfen
    verb = Verb.verb("", "werfen")

    assert_equal("werfen", verb.infinitiv)
    assert_equal("wirf", verb.singular.imperativ)
    assert_equal("werft", verb.plural.imperativ)
    assert_equal("werfend", verb.partizip_praesens)
    assert_equal("geworfen", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["werfe", "wirfst", "wirft", "werfen", "werft", "werfen"])
    checkVerbPraesensKonjunktiv(verb, ["werfe", "werfest", "werfe", "werfen", "werfet", "werfen"])
    checkVerbPraeteritum(verb, ["warf", "warfst", "warf", "warfen", "warft", "warfen"])
    checkVerbPraeteritumKonjunktiv(verb, ["w�rfe", "w�rfest", "w�rfe", "w�rfen", "w�rfet", "w�rfen"])
  end

  def testTraellern
    verb = Verb.verb("", "tr�llern")

    assert_equal("tr�llern", verb.infinitiv)
    assert_equal("tr�llere", verb.singular.imperativ)
    assert_equal("tr�llert", verb.plural.imperativ)
    assert_equal("tr�llernd", verb.partizip_praesens)
    assert_equal("getr�llert", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["tr�llere", "tr�llerst", "tr�llert", "tr�llern", "tr�llert", "tr�llern"])
    checkVerbPraesensKonjunktiv(verb, ["tr�llere", "tr�llerst", "tr�llere", "tr�llern", "tr�llert", "tr�llern"])
    checkVerbPraeteritum(verb, ["tr�llerte", "tr�llertest", "tr�llerte", "tr�llerten", "tr�llertet", "tr�llerten"])
    checkVerbPraeteritumKonjunktiv(verb, ["tr�llerte", "tr�llertest", "tr�llerte", "tr�llerten", "tr�llertet", "tr�llerten"])
  end

  def testVerwandeln
    verb = Verb.verb("", "verwandeln")

    assert_equal("verwandeln", verb.infinitiv)
    assert_equal("verwandele", verb.singular.imperativ)
    #assert_equal("verwandle", verb.singular.imperativ)
    assert_equal("verwandelt", verb.plural.imperativ)
    assert_equal("verwandelnd", verb.partizip_praesens)
    #assert_equal("verwandelt", verb.partizip_perfekt)

    #checkVerbPraesens(verb, ["verwandle", "verwandelst", "verwandelt", "verwandeln", "verwandelt", "verwandeln"])
    #checkVerbPraesensKonjunktiv(verb, ["verwandle", "verwandelst", "verwandle", "verwandeln", "verwandelt", "verwandeln"])
    checkVerbPraesens(verb, ["verwandele", "verwandelst", "verwandelt", "verwandeln", "verwandelt", "verwandeln"])
    checkVerbPraesensKonjunktiv(verb, ["verwandele", "verwandelst", "verwandele", "verwandeln", "verwandelt", "verwandeln"])
    checkVerbPraeteritum(verb, ["verwandelte", "verwandeltest", "verwandelte", "verwandelten", "verwandeltet", "verwandelten"])
    checkVerbPraeteritumKonjunktiv(verb, ["verwandelte", "verwandeltest", "verwandelte", "verwandelten", "verwandeltet", "verwandelten"])
  end
	
  def testVerletzen
    verb = Verb.verb("", "verletzen")

    assert_equal("verletzen", verb.infinitiv)
    assert_equal("verletz", verb.singular.imperativ)
    assert_equal("verletzt", verb.plural.imperativ)
    assert_equal("verletzend", verb.partizip_praesens)
    assert_equal("verletzt", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["verletze", "verletzt", "verletzt", "verletzen", "verletzt", "verletzen"])
    checkVerbPraesensKonjunktiv(verb, ["verletze", "verletzest", "verletze", "verletzen", "verletzet", "verletzen"])
    checkVerbPraeteritum(verb, ["verletzte", "verletztest", "verletzte", "verletzten", "verletztet", "verletzten"])
    checkVerbPraeteritumKonjunktiv(verb, ["verletzte", "verletztest", "verletzte", "verletzten", "verletztet", "verletzten"])
  end

  def testWeigern
    verb = Verb.verb("", "weigern")

    assert_equal("weigern", verb.infinitiv)
    assert_equal("weigere", verb.singular.imperativ)
    assert_equal("weigert", verb.plural.imperativ)
    assert_equal("weigernd", verb.partizip_praesens)
    assert_equal("geweigert", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["weigere", "weigerst", "weigert", "weigern", "weigert", "weigern"])
    checkVerbPraesensKonjunktiv(verb, ["weigere", "weigerst", "weigere", "weigern", "weigert", "weigern"])
    checkVerbPraeteritum(verb, ["weigerte", "weigertest", "weigerte", "weigerten", "weigertet", "weigerten"])
    checkVerbPraeteritumKonjunktiv(verb, ["weigerte", "weigertest", "weigerte", "weigerten", "weigertet", "weigerten"])
  end

  def testWissen
    verb = Verb.verb("", "wissen")

    assert_equal("wissen", verb.infinitiv)
    #assert_equal("wisse", verb.singular.imperativ)
    #assert_equal("wisst", verb.plural.imperativ)
    assert_equal("wissend", verb.partizip_praesens)
    assert_equal("gewusst", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["wei�", "wei�t", "wei�", "wissen", "wisst", "wissen"])
    checkVerbPraesensKonjunktiv(verb, ["wisse", "wissest", "wisse", "wissen", "wisset", "wissen"])
    checkVerbPraeteritum(verb, ["wusste", "wusstest", "wusste", "wussten", "wusstet", "wussten"])
    checkVerbPraeteritumKonjunktiv(verb, ["w�sste", "w�sstest", "w�sste", "w�ssten", "w�sstet", "w�ssten"])
  end

  def testSchreien
    verb = Verb.verb("", "schreien")

    assert_equal("schreien", verb.infinitiv)
    assert_equal("schrei", verb.singular.imperativ)
    assert_equal("schreit", verb.plural.imperativ)
    assert_equal("schreiend", verb.partizip_praesens)
    assert_equal("geschrien", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["schreie", "schreist", "schreit", "schreien", "schreit", "schreien"])
    checkVerbPraesensKonjunktiv(verb, ["schreie", "schreiest", "schreie", "schreien", "schreiet", "schreien"])
    checkVerbPraeteritum(verb, ["schrie", "schriest", "schrie", "schrien", "schriet", "schrien"])
    checkVerbPraeteritumKonjunktiv(verb, ["schriee", "schrieest", "schriee", "schrieen", "schrieet", "schrieen"])
  end

  def testMoegen
    verb = Verb.verb("", "m�gen")

    assert_equal("m�gen", verb.infinitiv)
    #assert_equal("wisse", verb.singular.imperativ)
    #assert_equal("wisst", verb.plural.imperativ)
    assert_equal("m�gend", verb.partizip_praesens)
    assert_equal("gemocht", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["mag", "magst", "mag", "m�gen", "m�gt", "m�gen"])
    checkVerbPraesensKonjunktiv(verb, ["m�ge", "m�gest", "m�ge", "m�gen", "m�get", "m�gen"])
    checkVerbPraeteritum(verb, ["mochte", "mochtest", "mochte", "mochten", "mochtet", "mochten"])
    checkVerbPraeteritumKonjunktiv(verb, ["m�chte", "m�chtest", "m�chte", "m�chten", "m�chtet", "m�chten"])
  end

  def testMuessen
    verb = Verb.verb("", "m�ssen")

    assert_equal("m�ssen", verb.infinitiv)
    assert_equal("", verb.singular.imperativ)
    assert_equal("", verb.plural.imperativ)
    assert_equal("m�ssend", verb.partizip_praesens)
    assert_equal("gemusst", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["muss", "musst", "muss", "m�ssen", "m�sst", "m�ssen"])
    checkVerbPraesensKonjunktiv(verb, ["m�sse", "m�ssest", "m�sse", "m�ssen", "m�sset", "m�ssen"])
    checkVerbPraeteritum(verb, ["musste", "musstest", "musste", "mussten", "musstet", "mussten"])
    checkVerbPraeteritumKonjunktiv(verb, ["m�sste", "m�sstest", "m�sste", "m�ssten", "m�sstet", "m�ssten"])
  end

  def testSollen
    verb = Verb.verb("", "sollen")

    assert_equal("sollen", verb.infinitiv)
    assert_equal("", verb.singular.imperativ)
    assert_equal("", verb.plural.imperativ)
    assert_equal("sollend", verb.partizip_praesens)
    assert_equal("gesollt", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["soll", "sollst", "soll", "sollen", "sollt", "sollen"])
    checkVerbPraesensKonjunktiv(verb, ["solle", "sollest", "solle", "sollen", "sollet", "sollen"])
    checkVerbPraeteritum(verb, ["sollte", "solltest", "sollte", "sollten", "solltet", "sollten"])
    checkVerbPraeteritumKonjunktiv(verb, ["sollte", "solltest", "sollte", "sollten", "solltet", "sollten"])
  end

  def testEnthalten
    verb = Verb.verb("", "enthalten")

    assert_equal("enthalten", verb.infinitiv)
    assert_equal("enthalte", verb.singular.imperativ)
    assert_equal("enthaltet", verb.plural.imperativ)
    assert_equal("enthaltend", verb.partizip_praesens)
    assert_equal("enthalten", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["enthalte", "enth�ltst", "enth�lt", "enthalten", "enthaltet", "enthalten"])
    checkVerbPraesensKonjunktiv(verb, ["enthalte", "enthaltest", "enthalte", "enthalten", "enthaltet", "enthalten"])
    checkVerbPraeteritum(verb, ["enthielt", "enthieltst", "enthielt", "enthielten", "enthieltet", "enthielten"])
    checkVerbPraeteritumKonjunktiv(verb, ["enthielte", "enthieltest", "enthielte", "enthielten", "enthieltet", "enthielten"])
  end

  def testPartizipPerfektOhneGe
    assert_equal("versaut", Verb.verb("", "versauen").partizip_perfekt)
    assert_equal("ver�tzt", Verb.verb("", "ver�tzen").partizip_perfekt)
    assert_equal("enthauptet", Verb.verb("", "enthaupten").partizip_perfekt)
    assert_equal("ersetzt", Verb.verb("", "ersetzen").partizip_perfekt)
    assert_equal("verletzt", Verb.verb("", "verletzen").partizip_perfekt)
    assert_equal("verarbeitet", Verb.verb("", "verarbeiten").partizip_perfekt)
    assert_equal("bekommen", Verb.verb("", "bekommen").partizip_perfekt)
    assert_equal("entgangen", Verb.verb("", "entgehen").partizip_perfekt)
    assert_equal("enthalten", Verb.verb("", "enthalten").partizip_perfekt)
    assert_equal("verschlungen", Verb.verb("", "verschlingen").partizip_perfekt)
    assert_equal("gewonnen", Verb.verb("", "gewinnen").partizip_perfekt)
    assert_equal("widersetzt", Verb.verb("", "widersetzen").partizip_perfekt)
	end

  def testPartizipPerfekt
    assert_equal("geschossen", Verb.verb("", "schie�en").partizip_perfekt)
    assert_equal("begonnen", Verb.verb("", "beginnen").partizip_perfekt)
    assert_equal("geblieben", Verb.verb("", "bleiben").partizip_perfekt)
    assert_equal("gesessen", Verb.verb("", "sitzen").partizip_perfekt)
    assert_equal("gemieden", Verb.verb("", "meiden").partizip_perfekt)
    assert_equal("gestiegen", Verb.verb("", "steigen").partizip_perfekt)
    assert_equal("herausgew�rgt", Verb.verb("", "w�rgen", "heraus").partizip_perfekt)
  end

  def testPartizipPraesens
    assert_equal("schie�end", Verb.verb("", "schie�en").partizip_praesens)
    assert_equal("beginnend", Verb.verb("", "beginnen").partizip_praesens)
    assert_equal("bleibend", Verb.verb("", "bleiben").partizip_praesens)
    assert_equal("sitzend", Verb.verb("", "sitzen").partizip_praesens)
    assert_equal("meidend", Verb.verb("", "meiden").partizip_praesens)
    assert_equal("steigend", Verb.verb("", "steigen").partizip_praesens)
    assert_equal("herausw�rgend", Verb.verb("", "w�rgen", "heraus").partizip_praesens)
  end

  def testAtmen
    verb = Verb.verb("", "atmen")

    assert_equal("atmen", verb.infinitiv)
    assert_equal("atme", verb.singular.imperativ)
    assert_equal("atmet", verb.plural.imperativ)
    assert_equal("atmend", verb.partizip_praesens)
    assert_equal("geatmet", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["atme", "atmest", "atmet", "atmen", "atmet", "atmen"])
    checkVerbPraesensKonjunktiv(verb, ["atme", "atmest", "atme", "atmen", "atmet", "atmen"])
    checkVerbPraeteritum(verb, ["atmete", "atmetest", "atmete", "atmeten", "atmetet", "atmeten"])
    checkVerbPraeteritumKonjunktiv(verb, ["atmete", "atmetest", "atmete", "atmeten", "atmetet", "atmeten"])
  end

  def testBringen
    verb = Verb.verb("", "bringen")

    assert_equal("bringen", verb.infinitiv)
    assert_equal("bring", verb.singular.imperativ)
    assert_equal("bringt", verb.plural.imperativ)
    assert_equal("bringend", verb.partizip_praesens)
    assert_equal("gebracht", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["bringe", "bringst", "bringt", "bringen", "bringt", "bringen"])
    checkVerbPraeteritum(verb, ["brachte", "brachtest", "brachte", "brachten", "brachtet", "brachten"])
    checkVerbPraesensKonjunktiv(verb, ["bringe", "bringest", "bringe", "bringen", "bringet", "bringen"])
    checkVerbPraeteritumKonjunktiv(verb, ["br�chte", "br�chtest", "br�chte", "br�chten", "br�chtet", "br�chten"])
  end

  def testEErweiterung
    assert_false(Verb.verb("", "sein").e_erweiterung)
    assert_false(Verb.verb("", "haben").e_erweiterung)
    assert(Verb.verb("", "�ffnen").e_erweiterung)
    assert(Verb.verb("", "wappnen").e_erweiterung)
    assert(Verb.verb("", "atmen").e_erweiterung)
  end

  def testInfinitive
    assert_equal("sein",Verb.verb("", "sein").infinitiv)
    assert_equal("aufh�ren",Verb.verb("", "h�ren","auf").infinitiv)
    assert_equal("hinzuf�gen",Verb.verb("", "f�gen","hinzu").infinitiv)
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

