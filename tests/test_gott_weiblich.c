#include <check.h>

#include "test_gott_weiblich.h"

#include "../src/german.c"

#include <stdlib.h>
#include <string.h>

START_TEST (test_gott_weiblich) {
	fail_unless(!gott_weiblich("den Gott kenne ich nicht"));

	fail_unless(!gott_weiblich("Quetzalcoatl"));
	fail_unless(!gott_weiblich("Camaxtli"));
	fail_unless(!gott_weiblich("Huhetotl"));
	fail_unless(!gott_weiblich("Mitra"));
	fail_unless(!gott_weiblich("Crom"));
	fail_unless(!gott_weiblich("Set"));
	fail_unless(!gott_weiblich("Anu"));
	fail_unless( gott_weiblich("Ishtar"));
	fail_unless(!gott_weiblich("Anshar"));
	fail_unless( gott_weiblich("Athena"));
	fail_unless(!gott_weiblich("Hermes"));
	fail_unless(!gott_weiblich("Poseidon"));
	fail_unless(!gott_weiblich("Lugh"));
	fail_unless( gott_weiblich("Brigit"));
	fail_unless(!gott_weiblich("Manannan Mac Lir"));
	fail_unless(!gott_weiblich("Shan Lai Ching"));
	fail_unless(!gott_weiblich("Chih Sung-tzu"));
	fail_unless(!gott_weiblich("Huan Ti"));
	fail_unless(!gott_weiblich("Issek"));
	fail_unless(!gott_weiblich("Mog"));
	fail_unless(!gott_weiblich("Kos"));
	fail_unless(!gott_weiblich("Mercury"));
	fail_unless( gott_weiblich("Venus"));
	fail_unless(!gott_weiblich("Mars"));
	fail_unless( gott_weiblich("Amaterasu Omikami"));
	fail_unless(!gott_weiblich("Raijin"));
	fail_unless(!gott_weiblich("Susanowo"));
	fail_unless(!gott_weiblich("Blind Io"));
	fail_unless( gott_weiblich("The Lady"));
	fail_unless(!gott_weiblich("Offler"));
	fail_unless(!gott_weiblich("Tyr"));
	fail_unless(!gott_weiblich("Odin"));
	fail_unless(!gott_weiblich("Loki"));
	fail_unless(!gott_weiblich("Ptah"));
	fail_unless(!gott_weiblich("Thoth"));
	fail_unless(!gott_weiblich("Anhur"));
} END_TEST

START_TEST (test_gott_genitiv) {
	fail_unless(!strcmp("des Quetzalcoatl", gott_genitiv("Quetzalcoatl")));
	fail_unless(!strcmp("des Camaxtli", gott_genitiv("Camaxtli")));
	fail_unless(!strcmp("des Huhetotl", gott_genitiv("Huhetotl")));
	fail_unless(!strcmp("des Mitra", gott_genitiv("Mitra")));
	fail_unless(!strcmp("des Crom", gott_genitiv("Crom")));
	fail_unless(!strcmp("des Set", gott_genitiv("Set")));
	fail_unless(!strcmp("des Anu", gott_genitiv("Anu")));
	fail_unless(!strcmp("der Ishtar", gott_genitiv("Ishtar")));
	fail_unless(!strcmp("des Anshar", gott_genitiv("Anshar")));
	fail_unless(!strcmp("der Athena", gott_genitiv("Athena")));
	fail_unless(!strcmp("des Hermes", gott_genitiv("Hermes")));
	fail_unless(!strcmp("des Poseidon", gott_genitiv("Poseidon")));
	fail_unless(!strcmp("des Lugh", gott_genitiv("Lugh")));
	fail_unless(!strcmp("der Brigit", gott_genitiv("Brigit")));
	fail_unless(!strcmp("des Manannan Mac Lir", gott_genitiv("Manannan Mac Lir")));
	fail_unless(!strcmp("des Shan Lai Ching", gott_genitiv("Shan Lai Ching")));
	fail_unless(!strcmp("des Chih Sung-tzu", gott_genitiv("Chih Sung-tzu")));
	fail_unless(!strcmp("des Huan Ti", gott_genitiv("Huan Ti")));
	fail_unless(!strcmp("des Issek", gott_genitiv("Issek")));
	fail_unless(!strcmp("des Mog", gott_genitiv("Mog")));
	fail_unless(!strcmp("des Kos", gott_genitiv("Kos")));
	fail_unless(!strcmp("des Mercury", gott_genitiv("Mercury")));
	fail_unless(!strcmp("der Venus", gott_genitiv("Venus")));
	fail_unless(!strcmp("des Mars", gott_genitiv("Mars")));
	fail_unless(!strcmp("der Amaterasu Omikami", gott_genitiv("Amaterasu Omikami")));
	fail_unless(!strcmp("des Raijin", gott_genitiv("Raijin")));
	fail_unless(!strcmp("des Susanowo", gott_genitiv("Susanowo")));
	fail_unless(!strcmp("des Offler", gott_genitiv("Offler")));
	fail_unless(!strcmp("des Tyr", gott_genitiv("Tyr")));
	fail_unless(!strcmp("des Odin", gott_genitiv("Odin")));
	fail_unless(!strcmp("des Loki", gott_genitiv("Loki")));
	fail_unless(!strcmp("des Ptah", gott_genitiv("Ptah")));
	fail_unless(!strcmp("des Thoth", gott_genitiv("Thoth")));
	fail_unless(!strcmp("des Anhur", gott_genitiv("Anhur")));
	/* Spezialfaelle */
	fail_unless(!strcmp("der Lady", gott_genitiv("The Lady")));
	fail_unless(!strcmp("der Lady", gott_genitiv("ARTIKEL_BESTIMMTER NOUN_LADY")));
	// fail_unless(!strcmp("des Blinden Io", gott_genitiv("Blind Io"))); // TODO
} END_TEST

Suite *test_suite(void)
{
	Suite *s = suite_create("gott_weiblich");
	TCase *tc_core = tcase_create("Nethack");

	suite_add_tcase (s, tc_core);
	tcase_set_timeout(tc_core, 20);
	tcase_add_test(tc_core, test_gott_weiblich);
	tcase_add_test(tc_core, test_gott_genitiv);

  return s;
}

int main(int argc, char *argv[]) {
	int nf;
	Suite *s = test_suite();
	SRunner *sr = srunner_create(s);
	srunner_set_xml(sr, "german_test_gott_weiblich.xml");
	srunner_run_all(sr, CK_VERBOSE);
	nf = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
