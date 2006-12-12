
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

    assert_equal("machest", machen.konjunktiv.form)

    assert_equal("machte", machen.erstePerson.indikativ.praeteritum.form)
    assert_equal("machtest", machen.zweitePerson.indikativ.praeteritum.form)
    assert_equal("machte", machen.drittePerson.indikativ.praeteritum.form)

    assert_equal("machtest", machen.zweitePerson.indikativ.praeteritum.form)

    assert_equal("machend", machen.partizip_praesens)
    assert_equal("gemacht", machen.partizip_perfekt)
  end

  def testRegelmaessigZusammengesetzteZeiten
  end

  def testHaben
    haben = VerbHaben.new("hab", "hat")

    assert_equal("haben", haben.infinitiv)

    assert_equal("hab", haben.singular.imperativ)

    assert_equal("habt", haben.plural.imperativ)

    assert_equal("habe", haben.singular.erstePerson.form)
    assert_equal("hast", haben.singular.zweitePerson.form)
    assert_equal("hat", haben.singular.drittePerson.form)

    assert_equal("habest", haben.konjunktiv.zweitePerson.form)

    assert_equal("hatte", haben.erstePerson.indikativ.praeteritum.form)
    assert_equal("hattest", haben.zweitePerson.indikativ.praeteritum.form)
    assert_equal("hatte", haben.drittePerson.indikativ.praeteritum.form)

    assert_equal("hättest", haben.zweitePerson.konjunktiv.praeteritum.form)

    assert_equal("habend", haben.partizip_praesens)
    assert_equal("gehabt", haben.partizip_perfekt)
  end

end
