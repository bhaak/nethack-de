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

Suite *test_suite(void)
{
	Suite *s = suite_create("gott_weiblich");
	TCase *tc_core = tcase_create("Nethack");

	suite_add_tcase (s, tc_core);
	tcase_set_timeout(tc_core, 20);
	tcase_add_test(tc_core, test_gott_weiblich);

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
