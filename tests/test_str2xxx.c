#include <check.h>

#include "test_str2xxx.h"

START_TEST (test_str2gend) {
	fail_unless(str2gend("ADJEKTIV_MAENNLICH") == 0);
	fail_unless(str2gend("ADJEKTIV_WEIBLICH") == 1);
	fail_unless(str2gend("mal") == 0);
	fail_unless(str2gend("fem") == 1);

	fail_unless(str2gend("male") == 0);
	fail_unless(str2gend("female") == 1);
} END_TEST

Suite *test_suite(void)
{
	Suite *s = suite_create("str2xxx");
	TCase *tc_core = tcase_create("Nethack");

	suite_add_tcase (s, tc_core);
	tcase_set_timeout(tc_core, 20);
	tcase_add_test(tc_core, test_str2gend);

  return s;
}

int main(int argc, char *argv[]) {
	int nf;
	Suite *s = test_suite();
	SRunner *sr = srunner_create(s);
	srunner_set_xml(sr, "german_test_str2xxx.xml");
	srunner_run_all(sr, CK_VERBOSE);
	nf = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
