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

#include "pm.h"


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

START_TEST (test_genitivattribut_zu_wort) {
	struct monst *mtmp;
	int i;
	char *str = NULL;

	mtmp = makemon(&mons[PM_MEDUSA], u.ux, u.uy, NO_MM_FLAGS);
	str = genitivattribut_zu_wort(noit_mon_nam(mtmp), "NOUN_GEHIRN");
	printf("%s\n", str);
	fail_unless(strcmp("Medusas NOUN_GEHIRN", str)==0);

	mtmp = makemon(&mons[PM_GNOME], u.ux, u.uy, NO_MM_FLAGS);
	str = genitivattribut_zu_wort(noit_mon_nam(mtmp), "NOUN_GEHIRN");
	printf("%s\n", str);
	fail_unless(strcmp("ARTIKEL_BESTIMMTER NOUN_GEHIRN des Gnomen", str)==0);

	str = genitivattribut_zu_wort("NOUN_PSEUDO_MAENNLICH Izchak", "NOUN_GEHIRN");
	printf("%s\n", str);
	fail_unless(strcmp("NOUN_PSEUDO_MAENNLICH Izchaks NOUN_GEHIRN", str)==0);

	str = genitivattribut_zu_wort("NOUN_IT", "NOUN_GEHIRN");
	printf("%s\n", str);
	fail_unless(strcmp("PRONOMEN_SEIN NOUN_GEHIRN", str)==0);
} END_TEST

#undef canspotmon
#define canspotmon(mon) (0)

START_TEST (test_naming) {
	struct monst *mtmp;
	int i;

	printf("1\n");
	int monster[] = {PM_MEDUSA, PM_ORACLE, PM_GUARD};
	for (i = 0; i < 3; i++) {
		mtmp = makemon(&mons[monster[i]], u.ux, u.uy, NO_MM_FLAGS);
		printf("%s\n", m_monnam(mtmp));
		printf("%s\n", noit_mon_nam(mtmp));
	}

} END_TEST

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
	strcpy(buf, "ein Ring der Transformation");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == RING_CLASS);
	fail_unless(obj->quan == 1);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	printf("\ndn: %s\n", dn);
	fail_unless(strcmp("NOUN_RING_POLYMORPH", dn)==0);

	strcpy(buf, "ein Ring der Zierde");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == RING_CLASS);
	fail_unless(obj->quan == 1);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(strcmp("NOUN_RING_ADORNMENT", dn)==0);

	strcpy(buf, "ein blauer Schmuckstein");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == GEM_CLASS);
	fail_unless(obj->quan == 1);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_DESCR(*ocl);
	fail_unless(strcmp("ADJEKTIV_GEM_BLUE", dn)==0);

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

	strcpy(buf, "Jadestein");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	fail_unless(obj->oclass == GEM_CLASS);
	fail_unless(obj->quan == 1);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_DESCR(*ocl);
	fail_unless(strcmp("ADJEKTIV_GEM_GREEN", dn)==0);

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

	strcpy(buf, "die Kadaver einer Vampirfledermaus");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == FOOD_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("NOUN_CORPSE", dn)==0);

	/*strcpy(buf, "die Leiche eines Schwebauges");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == FOOD_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("NOUN_CORPSE", dn)==0);*/

	strcpy(buf, "ein verfluchtes Schwert");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == WEAPON_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(obj->blessed == 0);
	fail_unless(obj->cursed == 1);
	fail_if(obj->otyp < SHORT_SWORD);
	fail_if(obj->otyp > KATANA);

	strcpy(buf, "eine Essensration");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == FOOD_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("NOUN_FOOD_RATION", dn)==0);

	strcpy(buf, "eine geheiligte Zauberbuch der Monsterbezauberung");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == SPBOOK_CLASS);
	fail_unless(obj->quan == 1);
	fail_unless(strcmp("NOUN_SPE_CHARM_MONSTER", dn)==0);

	strcpy(buf, "Tränke der Verwirrung");
	obj = readobjnam(buf, &nothing, TRUE);
	fail_if(obj == NULL);
	typ = obj->otyp;
	ocl = &objects[typ];
	dn = OBJ_NAME(*ocl);
	fail_unless(obj->oclass == POTION_CLASS);
	fail_unless(obj->quan == 2);
	fail_unless(strcmp("NOUN_POT_CONFUSION", dn)==0);

	// Statuette eines Goblins
	
} END_TEST

Suite *test_suite(void)
{
  Suite *s = suite_create("all tests");
  TCase *tc_core = tcase_create("Nethack");

  suite_add_tcase (s, tc_core);
//	tcase_add_test(tc_core, test_wishing);
	tcase_add_test(tc_core, test_naming);
	tcase_add_test(tc_core, test_genitivattribut_zu_wort);

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
