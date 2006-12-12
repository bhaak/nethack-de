
require 'test/unit' 
require 'german_verb' 


class TestVerb < Test::Unit::TestCase
  @output_levels = Test::Unit::UI::VERBOSE

  def testRegelmaessigEinfacheZeiten
    machen = Verb.new("mach")

    assert_equal("machen", machen.infinitiv)

    machen.set_singular
    assert_equal("mach", machen.imperativ)

    machen.set_plural
    assert_equal("macht", machen.imperativ)

    machen.set_singular
    assert_equal("machst", machen.form)

    machen.set_konjunktiv
    assert_equal("machest", machen.form)

    machen.set_indikativ
    machen.set_praeteritum
    assert_equal("machtest", machen.form)

    assert_equal("machend", machen.partizip_praesens)
    assert_equal("gemacht", machen.partizip_perfekt)
  end

  def testRegelmaessigZusammengesetzteZeiten
  end

  def testHaben
    haben = Verb.new("hab")

    assert_equal("haben", haben.infinitiv)

    haben.set_singular
    assert_equal("hab", haben.imperativ)

    haben.set_plural
    assert_equal("habt", haben.imperativ)

    haben.set_singular
    assert_equal("hast", haben.form)

    haben.set_konjunktiv
    assert_equal("habest", haben.form)

    haben.set_indikativ
    haben.set_praeteritum
    assert_equal("hättest", haben.form)

    assert_equal("habend", haben.partizip_praesens)
    assert_equal("gehabt", haben.partizip_perfekt)
  end
end
