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
	strcpy(buf, "Zauberbuch");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == SPBOOK_CLASS);
	fail_unless(obj->quan == 1);


	strcpy(buf, "eine Schriftrolle");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == SCROLL_CLASS);
	fail_unless(obj->quan == 1);

	strcpy(buf, "3 Steine");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == GEM_CLASS);
	fail_unless(obj->quan == 3);


	strcpy(buf, "2 rote Zauberbücher");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == SPBOOK_CLASS);
	fail_unless(obj->quan == 1);

	strcpy(buf, "2 Zirkonringes");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	char *dn = OBJ_DESCR(*ocl);
	fail_unless(obj->oclass == RING_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("RING_UNIDENTIFIED_SHINY", dn)==0);

	strcpy(buf, "ein geheiligter silberner Zauberstab");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_DESCR(*ocl);
	fail_unless(obj->oclass == WAND_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(obj->blessed == 1);
	fail_unless(strcmp("ADJEKTIV_WAND_SILVER", dn)==0);

	strcpy(buf, "ein Stethoskop");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == TOOL_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(obj->blessed == 0);
	fail_unless(obj->cursed == 0);
	fail_unless(strcmp("NOUN_STETHOSCOPE", dn)==0);


	strcpy(buf, "1 verfluchte Apfel");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == FOOD_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(obj->blessed == 0);
	fail_unless(obj->cursed == 1);
	fail_unless(strcmp("NOUN_APPLE", dn)==0);

	strcpy(buf, "ein nicht verfluchtes +1 Paar Lederhandschuhe");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	//printf("41 dn: %s\n", dn);
	fail_unless(obj->oclass == ARMOR_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(obj->blessed == 0);
	fail_unless(obj->cursed == 0);
	fail_unless(strcmp("NOUN_LEATHER_GLOVES", dn)==0);

	strcpy(buf, "eine Flasche Wasser");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == POTION_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("NOUN_POT_WATER", dn)==0);

	strcpy(buf, "Weihwasser");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == POTION_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(obj->blessed == 1);
	fail_unless(obj->cursed == 0);
	fail_unless(strcmp("NOUN_POT_WATER", dn)==0);

	strcpy(buf, "eine Flasche Satanswasser");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == POTION_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(obj->blessed == 0);
	fail_unless(obj->cursed == 1);
	fail_unless(strcmp("NOUN_POT_WATER", dn)==0);

	strcpy(buf, "ein sechseckiges Amulett");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_DESCR(*ocl);
	fail_unless(obj->oclass == AMULET_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("ADJEKTIV_AMULET_HEXAGONAL", dn)==0);

	strcpy(buf, "ein Amulett der Strangulation");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == AMULET_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("NOUN_AMULET_OF_STRANGULATION", dn)==0);
	}

	strcpy(buf, "die sterblichen Überreste einer Vampirfledermaus");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == FOOD_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("NOUN_CORPSE", dn)==0);

	strcpy(buf, "die Leiche eines Schwebauges");
	printf("\n110\n");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	printf("111 dn: %s\n", dn);
	fail_unless(obj->oclass == FOOD_CLASS);
	printf("112\n");
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("NOUN_CORPSE", dn)==0);
	printf("113\n");
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
