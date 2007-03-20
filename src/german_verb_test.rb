
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
    assert_equal("hät", Verb.umlaute("hat"))
    assert_equal("brauch", Verb.umlaute("brauch"))
    assert_equal("fäll", Verb.umlaute("fall"))
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
    checkVerbPraeteritumKonjunktiv(haben, ["hätte", "hättest", "hätte", "hätten", "hättet", "hätten"])
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
    checkVerbPraeteritumKonjunktiv(verb, ["würde", "würdest", "würde", "würden", "würdet", "würden"])
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
    verb.ablaut = true

    assert_equal("fallen", verb.infinitiv)
    assert_equal("fall", verb.singular.imperativ)
    assert_equal("fallt", verb.plural.imperativ)
    assert_equal("fallend", verb.partizip_praesens)
    assert_equal("gefallen", verb.partizip_perfekt)

    checkVerbPraesens(verb, ["falle", "fällst", "fällt", "fallen", "fallt", "fallen"])
    checkVerbPraesensKonjunktiv(verb, ["falle", "fallest", "falle", "fallen", "fallet", "fallen"])
    checkVerbPraeteritum(verb, ["fiel", "fielst", "fiel", "fielen", "fielt", "fielen"])
    checkVerbPraeteritumKonjunktiv(verb, ["fiele", "fielest", "fiele", "fielen", "fielet", "fielen"])
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
