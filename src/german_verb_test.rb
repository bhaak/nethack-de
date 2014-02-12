
require 'test/unit' 
require 'german_verb' 


class TestVerb < Test::Unit::TestCase
  @output_levels = Test::Unit::UI::VERBOSE

  def testVerb
    machen = Verb.new("mach")

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
  end

  def testRegelmaessigEinfacheZeiten
    machen = Verb.new("mach")

    assert_equal("machen", machen.infinitiv)

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
    haben = VerbHaben.new

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
    sein = VerbSein.new

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
    verb = VerbWerden.new

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
    verb = VerbUnregelmaessig.new("geh", "ging", "gang")

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
    verb = VerbUnregelmaessig.new("fall", "fiel", "fall")
    verb.umlaut = true

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
    #verb = VerbUnregelmaessig.new("�ffn", "", "")
    #verb = Verb.new("am�sier")
    verb = VerbUnregelmaessig.new("find", "fand", "fund")
    #verb = VerbSein.new

    puts
    puts "Infinitiv: "+ verb.infinitiv
    puts "E-Erweiterung: " + verb.e_erweiterung.to_s
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
    verb = Verb.new("�ffn")

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
    verb = VerbUnregelmaessig.new("find", "fand", "fund")

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

  def estVerb
    verb = VerbUnregelmaessig.new("", "", "")

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
