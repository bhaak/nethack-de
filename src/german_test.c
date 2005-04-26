#include <check.h>
//#include "german.h"

#include <stdlib.h>
#include <string.h>


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

START_TEST (test_tincontent)
{
	char *text[][2] = {{"a - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_TIN PARTIKEL_OF NOUN_SPINACH.",
											"a - eine nicht verfluchte Dose mit Spinat."},
										 {"b - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_TIN PARTIKEL_OF NOUN_FOX NOUN_MEAT.",
											"b - eine nicht verfluchte Dose mit Fuchsfleisch."},
										 {"c - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_TIN PARTIKEL_OF NOUN_COYOTE NOUN_MEAT.",
											"c - eine nicht verfluchte Dose mit Kojotenfleisch."},
										 {"d - ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED NOUN_TIN PARTIKEL_OF NOUN_LARGE_DOG NOUN_MEAT.",
											"d - eine verfluchte Dose mit Hundefleisch."}
	};

	check_strings(text, sizeof(text)/8);
}
END_TEST

START_TEST (test_linking_elements) {
	char *text[][2] = {
		{"X - ARTIKEL_UNBESTIMMTER RING_UNIDENTIFIED_CORAL NOUN_RING",
		 "X - ein Korallenring"},
		{"X - ARTIKEL_UNBESTIMMTER RING_UNIDENTIFIED_WOODEN NOUN_RING",
		 "X - ein Holzring"},
		/* Nouns ending in -ung have to get a 's' to link to the following noun */
		{"X - ARTIKEL_UNBESTIMMTER RING_UNIDENTIFIED_ENGAGEMENT NOUN_RING",
		 "X - ein Verlobungsring"},
		{"  NOUN_RING PARTIKEL_OF ADJEKTIV_RING_CONFLICT (RING_UNIDENTIFIED_ENGAGEMENT)",
		 "  Ring des ADJEKTIV_RING_CONFLICT (Verlobung)"}};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_wands) {
	char *text[][2] = {
		{"z - ARTIKEL_UNBESTIMMTER MADE_OF_WAND_MAPLE NOUN_WAND",
		 "z - ein Zauberstab aus Ahornholz"},
		{"  NOUN_WAND PARTIKEL_OF NOUN_WAND_LIGHT (MADE_OF_WAND_MAPLE)",
		 "  Zauberstab des Lichtes (Ahornholz)"},
		{"K - ARTIKEL_UNBESTIMMTER ADJEKTIV_WAND_CURVED NOUN_WAND",
		 "K - ein gebogener Zauberstab"},
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED NOUN_WAND PARTIKEL_OF NOUN_WAND_DEATH (0:4)",
		 "v - ein verfluchter Zauberstab des Todes (0:4)"}
	};
		
	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_spellbooks) {
	char *text[][2] = {
		{"N - ARTIKEL_UNBESTIMMTER MADE_OF_SPE_VELLUM NOUN_SPELLBOOK.",
		 "N - ein Zauberbuch aus Velum."},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_SLEEP (MADE_OF_SPE_VELLUM)",
		 "Zauberbuch des Schlafes (Velum)"},
		{"K - ARTIKEL_UNBESTIMMTER ADJEKTIV_WAND_CURVED NOUN_WAND",
		 "K - ein gebogener Zauberstab"}};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_corpses) {
	char *text[][2] = {
		{"SUBJECT PRONOMEN_PERSONAL VERB_DROP OBJECT MODIFIER_CORPSE 2 ADJEKTIV_UNCURSED NOUN_DINGO NOUN_CORPSEs SATZKLAMMER.",
		 "Du l�sst die sterblichen �berreste von 2 nicht verfluchten Dingos fallen."},
		{"a - MODIFIER_CORPSE NOUN_COYOTE NOUN_CORPSE",
		 "a - die sterblichen �berreste eines Kojoten"},
		{"b - MODIFIER_CORPSE NOUN_FOX NOUN_CORPSE",
		 "b - die sterblichen �berreste eines Fuchses"},
		{"c - MODIFIER_CORPSE 2 NOUN_FOX NOUN_CORPSEs",
		 "c - die sterblichen �berreste von 2 F�chsen"},
		{"d - MODIFIER_CORPSE ARTIKEL_UNBESTIMMTER halb ADJEKTIV_EATEN NOUN_FOX NOUN_CORPSE",
		 "d - die sterblichen �berreste eines halb verspeisten Fuchses"},
		{"SUBJECT PRONOMEN_PERSONAL VERB_DROP OBJECT MODIFIER_CORPSE 2 ADJEKTIV_UNCURSED NOUN_DINGO NOUN_CORPSEs SATZKLAMMER.",
		 "Du l�sst die sterblichen �berreste von 2 nicht verfluchten Dingos fallen."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_DROP OBJECT MODIFIER_CORPSE 2 NOUN_DINGO NOUN_CORPSEs SATZKLAMMER.",
		 "Du l�sst die sterblichen �berreste von 2 Dingos fallen."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_SEE hier OBJECT MODIFIER_CORPSE ARTIKEL_UNBESTIMMTER NOUN_JACKAL NOUN_CORPSE.",
		 "Du siehst hier die sterblichen �berreste eines Schakals."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_SEE hier OBJECT MODIFIER_CORPSE 2 NOUN_LICHEN NOUN_CORPSEs.", 
		 "Du siehst hier die sterblichen �berreste von 2 Flechten."},
	// SUBJECT PRONOMEN_PERSONAL finish eating OBJECT ARTIKEL_BESTIMMTER NOUN_GOBLIN NOUN_CORPSE"
		{"SUBJECT PRONOMEN_PERSONAL VERB_EAT OBJECT MODIFIER_CORPSE ARTIKEL_BESTIMMTER NOUN_GOBLIN NOUN_CORPSE",
		 "Du isst die sterblichen �berreste des Goblins"}};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_potions) {
	char *text[][2] = {
		{"  NOUN_POTION PARTIKEL_OF NOUN_POT_HEALING (ADJEKTIV_POT_SWIRLY)",
		 "  Trank der Heilung (verwirbelt)"},
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_BLINDNESS.",
		 "v - ein nicht verfluchter Trank der Blindheit."},
		{"l - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_BLINDFOLD",
		 "l - eine nicht verfluchte Augenbinde"},
		{"SUBJECT 3 ADJEKTIV_POT_SKY_BLUE NOUN_POTIONs",
		 "3 himmelblaue Tr�nke"}};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_called_named_labeled) {
	char *text[][2] = {
		{"t - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_CALLED vielleicht Wasser?",
		 "t - ein Trank bezeichnet mit vielleicht Wasser?"},
		{"f - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_SACK PARTIKEL_NAMED Necessaire",
		 "f - ein nicht verfluchter Sack genannt Necessaire"}};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_complete_sentences) {
	char *text[][2] = {
		{"SUBJECT PRONOMEN_PERSONAL VERB_HAVE kein Gold.",
		 "Du hast kein Gold."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_CAN wieder sehen.",
		 "Du kannst wieder sehen."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_SEIN zu klein um die T�re zuzuziehen.",
		 "Du bist zu klein um die T�re zuzuziehen."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_JACKAL VERB_BITE!",
		 "Der Schakal bei�t!"},
		{"SUBJECT PRONOMEN_POSSESSIV NOUN_TROLL VERB_KICK OBJECT PRONOMEN_POSSESSIV NOUN_DOG.",
		 "Dein Troll tritt deinen Hund."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_KILL OBJECT ARTIKEL_BESTIMMTER NOUN_SEWER_RAT!",
		 "Du t�test die Kanalratte!"},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_TROLL VERB_MISS OBJECT ARTIKEL_UNBESTIMMTER NOUN_DOG.",
		 "Der Troll verfehlt einen Hund."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_TROLL VERB_MISS OBJECT PRONOMEN_PERSONAL.",
		 "Der Troll verfehlt dich."},
		{"SUBJECT ARTIKEL_UNBESTIMMTER NOUN_LICHEN VERB_KICK!", 
		 "Eine Flechte tritt!"},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_LICHEN VERB_KICK!",
		 "Die Flechte tritt!"},
		{"SUBJECT ARTIKEL_UNBESTIMMTER NOUN_BLACK_DRAGON VERB_MISS OBJECT ARTIKEL_UNBESTIMMTER NOUN_BLACK_DRAGON.",
		 "Ein Schwarzdrache verfehlt einen Schwarzdrachen."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_BLACK_DRAGON VERB_HIT!",
		 "Der Schwarzdrache trifft!"},
		{"SUBJECT PRONOMEN_PERSONAL VERB_SEE hier OBJECT ARTIKEL_UNBESTIMMTER NOUN_EGG.",
		 "Du siehst hier ein Ei."},
		{"SUBJECT PRONOMEN_POSSESSIV NOUN_WALLET ist leer.",
		 "Deine Geldb�rse ist leer."}};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_complete_sentences2) {
	char *text[][2] = {
		{"ARTIKEL_BESTIMMTER NOUN_PONY ist nicht ADJEKTIV_SADDLED.",
		 "Das Pony ist nicht gesattelt."}
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_passiv) {
	char *text[][2] = {
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_FOX VERB_PASSIV_DESTROY!", 
		 "Der Fuchs VERB_PASSIV_DESTROY!"}};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_level_sounds) {
	char *text[][2] = {
		{"SUBJECT PRONOMEN_PERSONAL VERB_HEAR jemanden Geld z�hlen.",
		 "Du h�rst jemanden Geld z�hlen."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_HEAR ged�mpfte Ger�usche.",
		 "Du h�rst ged�mpfte Ger�usche."}};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_verbs) {
	char *text[][2] = {
		{"SUBJECT PRONOMEN_PERSONAL VERB_DACHTEST, dass SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT PRONOMEN_POSSESSIV RING_UNIDENTIFIED_OPAL NOUN_RING im Sp�lbecken verloren VERB_HAETTEST, aber da liegt er doch!",
		 "Du dachtest, dass du deinen Opalring im Sp�lbecken verloren h�ttest, aber da liegt er doch!"},
		{"SUBJECT ARTIKEL_UNBESTIMMTER RING_UNIDENTIFIED_BRONZE NOUN_RING VERB_LANDEN im Ausguss.",
		 "Ein Bronzering landet im Ausguss."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_SEEM schneller SATZKLAMMER.",
		 "Du scheinst schneller zu sein."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_SICH_FUEHLEN OBJECT PRONOMEN_PERSONAL au�ergew�hnlich gesund.",
		 "Du f�hlst dich au�ergew�hnlich gesund."},
		{"NOUN_OOPS! SUBJECT ARTIKEL_BESTIMMTER 2 NOUN_GEMs VERB_OBJECT_DROPS zu Boden!",
		 "Hoppla! Die 2 Schmucksteine fallen zu Boden!"},
		{"SUBJECT ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED ADJEKTIV_POT_SKY_BLUE NOUN_POTION VERB_OBJECT_DROPS zu Boden.",
		 "Ein verfluchter himmelblauer Trank f�llt zu Boden."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_DROP OBJECT ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_TRIPE_RATION SATZKLAMMER.",
		 "Du l�sst einen nicht verfluchten Pansen fallen."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_DROP OBJECT 2 ADJEKTIV_POT_BRILLIANT_BLUE NOUN_POTIONs SATZKLAMMER.",
		 "Du l�sst 2 blaugl�nzende Tr�nke fallen."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_OBJ_DOOR VERB_OPEN SATZKLAMMER.",
		 "Die T�re geht auf."}
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_tools) {
	char *text[][2] = {
		{"l - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_BLINDFOLD",
		 "l - eine nicht verfluchte Augenbinde"},
		{"SUBJECT ARTIKEL_UNBESTIMMTER NOUN_BLINDFOLD",
		 "Eine Augenbinde"}};

	check_strings(text, sizeof(text)/8);
} END_TEST


START_TEST (test_statues) {
	char *text[][2] = {
		{"a - ARTIKEL_UNBESTIMMTER NOUN_STATUE PARTIKEL_OF ARTIKEL_UNBESTIMMTER NOUN_DINGO",
		 "a - eine Statue eines Dingos"},
		{"b - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_FIGURINE PARTIKEL_OF ARTIKEL_UNBESTIMMTER NOUN_DINGO",
		 "b - eine nicht verfluchte Statuette eines Dingos"}};

	check_strings(text, sizeof(text)/8);
} END_TEST

/* copy from invent.c */
char *names[] = { 0,
  "Illegal objects", "NOUN_WEAPONs", "NOUN_ARMOR", "NOUN_RINGs", "NOUN_AMULETs",
  "NOUN_TOOLs", "NOUN_COMESTIBLE", "NOUN_POTIONs", "NOUN_SCROLLs", "NOUN_SPELLBOOKs",
  "NOUN_WANDs", "NOUN_COINs", "NOUN_GEMs", "NOUN_BOULDERs, NOUN_STATUEs", "Iron balls",
  "Chains", "Venoms"
};

START_TEST (test_inventory_names) {
	char *text[][2] = {
		{names[ 2], "Waffen"},
		{names[ 3], "Schutzbekleidung"},
		{names[ 4], "Ringe"},
		{names[ 5], "Amulette"},
		{names[ 6], "Werkzeuge"},
		{names[ 7], "Essbares"},
		{names[ 8], "Tr�nke"},
		{names[ 9], "Schriftrollen"},
		{names[10], "Zauberb�cher"},
		{names[11], "Zauberst�be"},
		{names[12], "M�nzen"},
		{names[13], "Schmucksteine"},
		{names[14], "Felsbrocken, Statuen"}
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

void check_german2meta(char* text[][2], int size) {
	int i;
	char result[128];

	for (i=0; i<size; i++) {
		german2meta(text[i][0],result);
		fail_unless((strcmp(result, text[i][1])==0),
								"failed german2meta\nto convert: >%s<\nconverted:  >%s<\nexpected:   >%s<\n",
								text[i][0],result,text[i][1]);}
}

START_TEST (test_wishing) {
	char *text[][2] = {{"Augenbinde",  "NOUN_BLINDFOLD"},
										 {"Augenbinden", "NOUN_BLINDFOLDs"},
										 {"eine Augenbinden", "ARTIKEL_UNBESTIMMTER NOUN_BLINDFOLDs"}};

	check_german2meta(text, sizeof(text)/8);
} END_TEST

START_TEST (test_casus_and_modifier) {
	char *text[][2] = {
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_GENDARMERIE VERB_SEIN hinter OBJECT KASUS_DATIV PRONOMEN_PERSONAL her!",
		 "Die Gendarmerie ist hinter dir her!"},
		{"SUBJECT KASUS_GENITIV PRONOMEN_PERSONAL MODIFIER_FEMININ ARTIKEL_UNBESTIMMTER",
		 "Deiner einer"}
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_possessiv) {
	char *text[][2] = {
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_KOP_SERGEANT VERB_SCHLAGEN mit KASUS_DATIV PRONOMEN_3P_MN_POSSESSIV NOUN_RUBBER_HOSE nach KASUS_DATIV ARTIKEL_BESTIMMTER NOUN_KOP_SERGEANT.",
		 ""},
	};

	check_strings(text, sizeof(text)/8);
} END_TEST


Suite *test_suite(void)
{
  Suite *s = suite_create("all tests");
  TCase *tc_core = tcase_create("Nethack");

  suite_add_tcase (s, tc_core);
	tcase_add_test(tc_core, test_possessiv);
	/*tcase_add_test(tc_core, test_wishing);
	tcase_add_test(tc_core, test_statues);
	tcase_add_test(tc_core, test_verbs);
	tcase_add_test(tc_core, test_linking_elements);
	tcase_add_test(tc_core, test_wands);
  tcase_add_test(tc_core, test_spellbooks);
  tcase_add_test(tc_core, test_potions);
  tcase_add_test(tc_core, test_called_named_labeled);
  tcase_add_test(tc_core, test_level_sounds);
  tcase_add_test(tc_core, test_tools);
  tcase_add_test(tc_core, test_passiv);
  tcase_add_test(tc_core, test_complete_sentences);
  tcase_add_test(tc_core, test_complete_sentences2);
  tcase_add_test(tc_core, test_corpses);
	tcase_add_test(tc_core, test_tincontent);
	tcase_add_test(tc_core, test_inventory_names);
	tcase_add_test(tc_core, test_casus_and_modifier);*/

  return s;
}

int main() {
  int nf;
  Suite *s = test_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_VERBOSE);
  //srunner_run_all(sr, CK_NORMAL);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}