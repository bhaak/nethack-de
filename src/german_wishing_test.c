#include <check.h>
//#include "german.h"

#include <stdlib.h>
#include <string.h>

#include "hack.h"
#include "extern.h"
#include "decl.h"
#include "config.h"
#include "obj.h"
#include "objclass.h"
//#include "prop.h"
//#include "skills.h"


char* german(const char *line);
void german2meta(char *str, char *result);

void check_strings(char* text[][2], int size) {
	int i;
	char *ret = NULL;

	for (i=0; i<size; i++) {
		ret = german(text[i][0]);
		fail_unless((strcmp(ret, text[i][1])==0),
								"failed\nto convert: >%s<\nconverted:  >%s<\nexpected:   >%s<\n",
								text[i][0],ret,text[i][1]);}
}

START_TEST (test_wishing) {
	struct obj *obj, *obj2;
  struct obj *otmp, nothing;

	int typ = 0;
	struct objclass *ocl = NULL;
	char *dn;

	char buf[BUFSZ];
	strcpy(buf, "nichts");

	//fprintf(stderr, "obj: %d; nothing: %d\n", obj, nothing);
	//fprintf(stderr, "&obj: %d; &nothing: %d\n", &obj, &nothing);
	//obj = readobjnam(buf, &nothing, TRUE);

	//fprintf(stderr, "obj: %d; nothing: %d\n", obj, nothing);
	//fprintf(stderr, "&obj: %d; &nothing: %d\n", &obj, &nothing);
	//fprintf(stderr, "&obj: %d; nothing: %d\n", &obj, nothing);
	//fail_if(&obj != &nothing);

	if (1) {
	fprintf(stderr, "\n3\n");
	strcpy(buf, "Zauberbuch");
	fprintf(stderr, "\n4\n");
	obj = readobjnam(buf, &nothing, TRUE);
	fprintf(stderr, "\n5\n");
	fprintf(stderr, "&obj: %d; &nothing: %d\n", &obj, &nothing);
	fprintf(stderr, "obj: %d; nothing: %d\n", obj, nothing);
	fprintf(stderr, "\n6\n");
	fprintf(stderr, "obj->oclass: %d\n", obj->oclass);
	fail_unless(obj->oclass == SPBOOK_CLASS);
	fail_unless(obj->quan == 1);
	fprintf(stderr, "\n7\n");


	strcpy(buf, "eine Schriftrolle");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_unless(obj->oclass == SCROLL_CLASS);
	fail_unless(obj->quan == 1);

	strcpy(buf, "2 Tränke");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == POTION_CLASS);
	fail_unless(obj->quan == 2);


	strcpy(buf, "2 rote Zauberbücher");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == SPBOOK_CLASS);
	fail_unless(obj->quan == 1);

	strcpy(buf, "2 Zirkonringes");
	obj = readobjnam(buf, &nothing, TRUE);
	typ = obj->otyp;
	ocl = &objects[typ];
	char *dn = OBJ_DESCR(*ocl);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == RING_CLASS);
	printf("\n21 dn: %s\n", dn);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("RING_UNIDENTIFIED_SHINY", dn)==0);
	printf("\n22\n");
	}

	strcpy(buf, "ein geheiligter silberner Zauberstab");
	printf("\n23\n");
	obj = readobjnam(buf, &nothing, TRUE);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_DESCR(*ocl);
	printf("24 dn: %s\n", dn);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == WAND_CLASS);
	printf("25\n");
	fail_unless(obj->quan == 1);
	fail_unless(obj->blessed == 1);
	fail_unless(strcmp("ADJEKTIV_WAND_SILVER", dn)==0);
	printf("26\n");
} END_TEST

Suite *test_suite(void)
{
  Suite *s = suite_create("all tests");
  TCase *tc_core = tcase_create("Nethack");

  suite_add_tcase (s, tc_core);
	tcase_add_test(tc_core, test_wishing);

  return s;
}

int testsuite() {
  int nf;
  Suite *s = test_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_VERBOSE);
  //srunner_run_all(sr, CK_NORMAL);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main2() {
	return testsuite();
}
