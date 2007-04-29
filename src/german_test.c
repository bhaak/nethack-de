#include <check.h>

#include <stdlib.h>
#include <string.h>

//#include "hack.h"
//#include "extern.h"
//#include "decl.h"
//#include "config.h"
//#include "obj.h"
//#include "objclass.h"
//#include "prop.h"
//#include "skills.h"

#include "german.h"

void check_strings(char* text[][2], int size) {
	int i;
	char *ret = NULL;

	for (i=0; i<size; i++) {
		ret = german(text[i][0]);
		fail_unless((strcmp(ret, text[i][1])==0),
								"failed\nto convert: >%s<\nconverted:  >%s<\nexpected:   >%s<\n",
								text[i][0],ret,text[i][1]);}
}

//#if 0
START_TEST (test_tincontent)
{
	char *text[][2] = {{"a - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_TIN PARTIKEL_OF NOUN_SPINACH.",
											"a - eine nicht verfluchte Dose mit Spinat."},
										 {"b - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_TIN PARTIKEL_OF NOUN_FOX NOUN_MEAT.",
											"b - eine nicht verfluchte Dose mit Fuchsfleisch."},
										 {"c - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_TIN PARTIKEL_OF NOUN_COYOTE NOUN_MEAT.",
											"c - eine nicht verfluchte Dose mit Kojotenfleisch."},
										 {"d - ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED NOUN_TIN PARTIKEL_OF NOUN_LARGE_DOG NOUN_MEAT.",
											"d - eine verfluchte Dose mit Fleisch vom grossen Hunde."}
	};

	check_strings(text, sizeof(text)/8);
}
END_TEST

#if 0
/* nicht mehr per Programmcode gel�st */
START_TEST (test_linking_elements) {
	char *text[][2] = {
		{"X - ARTIKEL_UNBESTIMMTER NOUN_RING_UNIDENTIFIED_CORAL NOUN_RING",
		 "X - ein Korallenring"},
		{"X - ARTIKEL_UNBESTIMMTER NOUN_RING_UNIDENTIFIED_WOODEN NOUN_RING",
		 "X - ein Holzring"},
		 /* Nouns ending in -ung have to get a 's' to link to the following noun */
		{"X - ARTIKEL_UNBESTIMMTER NOUN_RING_UNIDENTIFIED_ENGAGEMENT NOUN_RING",
		 "X - ein Verlobungsring"}};

	check_strings(text, sizeof(text)/8);
} END_TEST
#endif

START_TEST (test_rings) {
	char *text[][2] = {
		{"a NOUN_RING KASUS_GENITIV ARTIKEL_BESTIMMTER NOUN_RING_CONFLICT (RING_UNIDENTIFIED_CLAY)",
		 "a Ring des Konfliktes (Ton)"},
		{"b NOUN_RING KASUS_GENITIV ARTIKEL_BESTIMMTER NOUN_RING_CONFLICT (RING_UNIDENTIFIED_ENGAGEMENT)",
		 "b Ring des Konfliktes (Verlobung)"},
		{"c - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_RING KASUS_GENITIV ARTIKEL_BESTIMMTER NOUN_RING_STEALTH.",
		 "c - ein nicht verfluchter Ring des Schleichens."},
		{"d NOUN_RING PARTIKEL_OF NOUN_RING_STEALTH (RING_UNIDENTIFIED_TWISTED)",
		 "d Ring des Schleichens (Lapislazuli)"},
		{"e - ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED RING_UNIDENTIFIED_TIGER_EYE NOUN_RING",
		 "e - ein verfluchter Tigeraugenring"},
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_wands) {
	char *text[][2] = {
		{"l - ARTIKEL_UNBESTIMMTER MADE_OF_WAND_ALUMINUM NOUN_WAND",
		 "l - ein Zauberstab aus Aluminium"},
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
		{"N - ARTIKEL_UNBESTIMMTER ADJEKTIV_SPE_VELLUM NOUN_SPELLBOOK.",
		 "N - ein velinpapierenes Zauberbuch."},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_SLEEP (ADJEKTIV_SPE_VELLUM)",
		 "Zauberbuch des Schlafes (velinpapieren)"},
		{"K - ARTIKEL_UNBESTIMMTER ADJEKTIV_WAND_CURVED NOUN_WAND",
		 "K - ein gebogener Zauberstab"}};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_identified_spellbooks) {
	char *text[][2] = {
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_DIG",             "Zauberbuch des Grabens"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_MAGIC_MISSILE",   "Zauberbuch des magischen Geschosses"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_FIREBALL",        "Zauberbuch des Feuerballes"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CONE_OF_COLD",    "Zauberbuch des K�ltekegels"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_SLEEP",           "Zauberbuch des Schlafes"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_FINGER_OF_DEATH", "Zauberbuch des Fingers des Todes"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_LIGHT",           "Zauberbuch des Lichtes"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_DETECT_MONSTERS", "Zauberbuch des Ersp�rens von Monstern"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_HEALING",         "Zauberbuch der Linderung"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_KNOCK",           "Zauberbuch des Klopfens"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_FORCE_BOLT",      "Zauberbuch des Kraftsto�es"},
		{"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CONFUSE_MONSTER", "Zauberbuch der Monsterverwirrung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CURE_BLINDNESS",  "Zauberbuch des Kurierens von Blindheit"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_DRAIN_LIFE",      "Zauberbuch des Lebensentzuges"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_SLOW_MONSTER",    "Zauberbuch der Verlangsamung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_WIZARD_LOCK",     "Zauberbuch des Zauberverschlusses"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CREATE_MONSTER",  "Zauberbuch der Monsterbeschw�rung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_DETECT_FOOD",     "Zauberbuch des Ersp�rens von Nahrung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CAUSE_FEAR",      "Zauberbuch der Furcht"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CLAIRVOYANCE",    "Zauberbuch des Hellsehens"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CURE_SICKNESS",   "Zauberbuch des Kurierens von Erkrankungen"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CHARM_MONSTER",   "Zauberbuch der Monsterbezauberung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_HASTE_SELF",      "Zauberbuch der Selbsthast"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_DETECT_UNSEEN",   "Zauberbuch des Ersp�rens des Ungesehenen"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_LEVITATION",      "Zauberbuch der Levitation"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_EXTRA_HEALING",   "Zauberbuch der Heilung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_RESTORE_ABILITY", "Zauberbuch des Wiederherstellens von Fertigkeiten"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_INVISIBILITY",    "Zauberbuch der Unsichtbarkeit"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_DETECT_TREASURE", "Zauberbuch des Ersp�rens von Sch�tzen"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_REMOVE_CURSE",    "Zauberbuch des Fluchbannens"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_MAGIC_MAPPING",   "Zauberbuch des magischen Kartierens"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_IDENTIFY",        "Zauberbuch der Identifizierung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_TURN_UNDEAD",     "Zauberbuch der Untotenwandlung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_POLYMORPH",       "Zauberbuch der Transformation"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_TELEPORT_AWAY",   "Zauberbuch des Wegteleportierens"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CREATE_FAMILIAR", "Zauberbuch der Haustierbeschw�rung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_CANCELLATION",    "Zauberbuch der L�schung"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_PROTECTION",      "Zauberbuch des Schutzes"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_JUMPING",         "Zauberbuch des Springens"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_STONE_TO_FLESH",  "Zauberbuch des Stein-zu-Fleisch"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_FLAME_SPHERE",    "Zauberbuch der Flammenkugel"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_FREEZE_SPHERE",   "Zauberbuch der Frostkugel"},
    {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_BLANK_PAPER",     "Zauberbuch des unbeschrifteten Papieres"},
		{"ARTIKEL_BESTIMMTER NOUN_SPE_BOOK_OF_THE_DEAD",        "Das Buch der Toten"}
	};
	
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
		 "Du isst die sterblichen �berreste des Goblins"},
		{"SUBJECT PRONOMEN_POSSESSIV MODIFIER_CORPSE NOUN_GREY_ELF NOUN_CORPSE VERB_WERDEN pl�tzlich lebendig!",
		 "Deine sterblichen �berreste eines Grauelbes werden pl�tzlich lebendig!"}
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_potions) {
	char *text[][2] = {
		{"  NOUN_POTION PARTIKEL_OF NOUN_POT_HEALING (ADJEKTIV_POT_SWIRLY)",
		 "  Trank der Linderung (verwirbelt)"},
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_BLINDNESS.",
		 "v - ein nicht verfluchter Trank der Blindheit."},
		{"SUBJECT 3 ADJEKTIV_POT_SKY_BLUE NOUN_POTIONs",
		 "3 himmelblaue Tr�nke"},
// NOUN_POT_GAIN_ABILITY"
// NOUN_POT_RESTORE_ABILITY"
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_CONFUSION",
		 "v - ein nicht verfluchter Trank der Verwirrung"},
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_BLINDNESS", "v - ein nicht verfluchter Trank der Blindheit"},
// NOUN_POT_PARALYSIS" # der L�hmung, der Paralyse
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_SPEED", "v - ein nicht verfluchter Trank der Hast"},
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_LEVITATION", "v - ein nicht verfluchter Trank der Levitation"},
// NOUN_POT_HALLUCINATION" # der Halluzination
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_INVISIBILITY", "v - ein nicht verfluchter Trank der Unsichtbarkeit"},
// NOUN_POT_SEE_INVISIBLE" # Enth�llung des Unsichtbaren
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_HEALING", "v - ein nicht verfluchter Trank der Linderung"},
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_EXTRA_HEALING", "v - ein nicht verfluchter Trank der Heilung"},
		{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_FULL_HEALING", "v - ein nicht verfluchter Trank der vollst�ndigen Heilung"},
		//{"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_GAIN_LEVEL", ""},
// NOUN_POT_ENLIGHTENMENT", "Erleuchtung", "", "Erleuchtung", "en", "feminin", "s"),     # Erleuchtung, Klarheit?
// NOUN_POT_MONSTER_DETECTION" # Monsterersp�ren
// NOUN_POT_OBJECT_DETECTION"  # Objektersp�rung, Objektersp�rens
// NOUN_POT_GAIN_ENERGY"       # Energiegewinnung?
// NOUN_POT_SICKNESS"          # �belkeit
		{"v - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_SLEEPING", "v - ein Trank des Schlafes"},
		{"v - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_POLYMORPH", "v - ein Trank der Transformation"},
		{"v - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_WATER",
     "v - eine Flasche Wasser"},
		{"v - 2 NOUN_POTIONs PARTIKEL_OF NOUN_POT_WATER",
     "v - 2 Flaschen Wasser"},
		{"v - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_BOOZE",
     "v - eine Flasche Schnaps"},
		{"v - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_FRUIT_JUICE",
     "v - eine Flasche Fruchtsaft"},
		{"v - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_ACID",
     "v - eine Flasche S�ure"},
		{"v - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_OIL",
     "v - eine Flasche �l"},
		{"v - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_HOLY_WATER",
     "v - eine Flasche Weihwasser"},
		{"v - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_UNHOLY_WATER",
     "v - eine Flasche Satanswasser"}
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_called_named_labeled) {
	char *text[][2] = {
		{"a - ARTIKEL_UNBESTIMMTER NOUN_SCROLL PARTIKEL_LABELED \"NOUN_SCR_READ_ME\"",
		 "a - eine Schriftrolle \"LIES MICH\"" },
		{"b - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_CALLED vielleicht Wasser?",
		 "b - ein Trank genannt vielleicht Wasser?"},
		{"c - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_SACK PARTIKEL_NAMED Necessaire",
		 "c - ein nicht verfluchter Sack namens Necessaire"}};

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
		 "Deine Geldb�rse ist leer."},
    {"SUBJECT PRONOMEN_PERSONAL VERB_ABBRECHEN OBJECT PRONOMEN_POSSESSIV NOUN_VERSUCH, die B�chse zu �ffnen, SATZKLAMMER.",
     "Du brichst deinen Versuch, die B�chse zu �ffnen, ab."},
    {"KASUS_DATIV PRONOMEN_PERSONAL SUBJECT_IM_SATZ VERB_GELINGEN NOUN_IT, die Dose zu �ffnen.",
     "Dir gelingt es, die Dose zu �ffnen."},
  };

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_complete_sentences2) {
	char *text[][2] = {
		{"ARTIKEL_BESTIMMTER NOUN_PONY VERB_SEIN nicht ADJEKTIV_SADDLED.",
		 "Das Pony ist nicht gesattelt."},
		{"SUBJECT PRONOMEN_POSSESSIV NOUN_RINGs VERB_RUTSCHEN von KASUS_DATIV PRONOMEN_POSSESSIV NOUN_FINGERs.",
		 "Deine Ringe rutschen von deinen Fingern."},
		{"SUBJECT PRONOMEN_POSSESSIV NOUN_SWORDs VERB_GLEITEN KASUS_DATIV PRONOMEN_PERSONAL aus KASUS_DATIV ARTIKEL_BESTIMMTER NOUN_HANDs.",
		 "Deine Schwerter gleiten dir aus den H�nden."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_NEHMEN OBJECT MODIFIER_CORPSE NOUN_CHICKATRICE NOUN_CORPSE in KASUS_AKKUSATIV PRONOMEN_POSSESSIV ADJEKTIV_BARE NOUN_HANDs.",
		 "Du nimmst die sterblichen �berreste eines Kikatrix in deine blo�en H�nde."},
		{"SUBJECT PRONOMEN_POSSESSIV NOUN_HALS VERB_ZUSCHNUEREN sich SATZKLAMMER!",
		 "Dein Hals schn�rt sich zu!"},
		{"Pl�tzlich VERB_SEIN SUBJECT_IM_SATZ PRONOMEN_PERSONAL durchsichtig!",
		 "Pl�tzlich bist du durchsichtig!"},
		{"SUBJECT PRONOMEN_PERSONAL VERB_KOENNEN OBJECT PRONOMEN_KEIN NOUN_SHIELD tragen, NEUER_SATZ w�hrend SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT ARTIKEL_UNBESTIMMTER ADJEKTIV_ZWEIHAENDIG NOUN_SWORD VERB_FUEHREN.",
		 "Du kannst keinen Schild tragen, w�hrend du ein zweih�ndiges Schwert f�hrst."},
		{"SUBJECT NOUN_ETWAS VERB_SEIN hier in KASUS_AKKUSATIV ARTIKEL_BESTIMMTER NOUN_DUST geschrieben.",
		 "Etwas ist hier in den Staub geschrieben."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_HAVE OBJECT PRONOMEN_KEIN NOUN_HAND frei um damit zu schreiben!",
		 "Du hast keine Hand frei um damit zu schreiben!"},
		{"SUBJECT PRONOMEN_PERSONAL VERB_BEENDEN OBJECT PRONOMEN_POSSESSIV NOUN_DRESSING_MANEUVER.",
		 "Du beendest deinen Bekleidungsakt."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_OBJ_DOOR VERB_ABSORBIEREN OBJECT PRONOMEN_POSSESSIV NOUN_SPELL!",
		 "Die T�re absorbiert deinen Spruch!"},
		//{"SUBJECT ARTIKEL_BESTIMMTER NOUN_LONG_WORM VERB_OEFFNEN OBJECT PRONOMEN_POSSESSIV NOUN_HAND!",
		//"Der Langwurm �ffnet seinen Mund!"},
		{"SUBJECT PRONOMEN_PERSONAL VERB_HAUEN OBJECT PRONOMEN_PERSONAL selbst!",
		 "Du haust dich selbst!"},
		{"SUBJECT PRONOMEN_PERSONAL VERB_PLATZIEREN OBJECT ARTIKEL_UNBESTIMMTER NOUN_FIREBALL auf KASUS_DATIV PRONOMEN_PERSONAL selbst!",
		 "Du platzierst einen Feuerball auf dir selbst!"},
		{"OBJECT KASUS_DATIV PRONOMEN_PERSONAL MODIFIER_VERB_DRITTE_PERSON VERB_SEIN ziemlich warm.",
		 "Dir ist ziemlich warm."},
		{"SUBJECT NOUN_IT VERB_JUCKEN OBJECT PRONOMEN_PERSONAL stark unter OBJECT KASUS_DATIV PRONOMEN_POSSESSIV NOUN_MUMMY_WRAPPINGs.",
		 "Es juckt dich stark unter deinen Mumienbinden."},
	};

	check_strings(text, sizeof(text)/8);
} END_TEST
 

START_TEST (test_complete_sentences3) {
	char *text[][2] = {
		{"a - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED +0 NOUN_MUMMY_WRAPPINGs (VERB_WORN)",
		 "a - nicht verfluchte +0 Mumienbinden (getragen)"},
		{"Vor Schrecken VERB_ERSCHAUDERN SUBJECT_IM_SATZ PRONOMEN_PERSONAL.",
		 "Vor Schrecken erschauderst du."},
		{"SUBJECT PRONOMEN_POSSESSIV NOUN_AMULET erw�rmt sich einen Moment lang und k�hlt wieder ab.",
		 "Dein Amulett erw�rmt sich einen Moment lang und k�hlt wieder ab."},
		{"SUBJECT NOUN_BLUT VERB_TROPFEN OBJECT auf PRONOMEN_POSSESSIV NOUN_FACE.",
		 "Blut tropft auf dein Gesicht."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_SAEURE VERB_BRENNEN!",
		 "Die S�ure brennt!"},
		{"SUBJECT ARTIKEL_UNBESTIMMTER NOUN_SCROLL VERB_VERBRENNEN.",
		 "Eine Schriftrolle verbrennt."},
		{"Aus irgendeinem Grund VERB_SEIN SUBJECT_IM_SATZ PRONOMEN_PERSONAL nicht betroffen.",
		 "Aus irgendeinem Grund bist du nicht betroffen."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_BOLT_OF_FIRE VERB_VORBEIZISCHEN an KASUS_DATIV PRONOMEN_PERSONAL SATZKLAMMER!",
		 "Der Feuerblitz zischt an dir vorbei!"},
		{"SUBJECT PRONOMEN_PERSONAL VERB_FRIEREN.",
		 "Du frierst."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_ORACLE VERB_NEHMEN ver�chtlich OBJECT PRONOMEN_POSSESSIV ganzes Geld und VERB_SAGEN:",
		 "Das Orakel nimmt ver�chtlich dein ganzes Geld und sagt:"},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_ORACLE VERB_MEDITIEREN einen Moment und VERB_VERKUENDEN dann:",
		 "Das Orakel meditiert einen Moment und verk�ndet dann:"},
		{"SUBJECT PRONOMEN_PERSONAL VERB_BEMERKEN, dass SUBJECT_IM_SATZ PRONOMEN_PERSONAL kein Gold mehr VERB_HAVE!",
		 "Du bemerkst, dass du kein Gold mehr hast!"},
		{"SUBJECT PRONOMEN_PERSONAL VERB_CAN OBJECT PRONOMEN_PERSONAL nicht richtig hinsetzen!",
		 "Du kannst dich nicht richtig hinsetzen!"},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_THRON VERB_AUFLOESEN sich in ein Logikw�lkchen SATZKLAMMER.",
		 "Der Thron l�st sich in ein Logikw�lkchen auf."},
		{"ARTIKEL_UNBESTIMMTER NOUN_IRON_BARs oder SUBJECT_IM_SATZ ARTIKEL_UNBESTIMMTER NOUN_TREE",
		 "Eisenstangen oder ein Baum"},
		{"Wasserturbulenzen beeintr�chtigen KASUS_AKKUSATIV PRONOMEN_POSSESSIV NOUN_MOVEMENTs.",
		 "Wasserturbulenzen beeintr�chtigen deine Bewegungen."},
		//{"SUBJECT ARTIKEL_UNBESTIMMTER ADJEKTIV_ELEKTRISCH NOUN_SCHLAG VERB_SCHIESSEN OBJECT durch PRONOMEN_POSSESSIV NOUN_BODY!",
		 //""},
		//{"ARTIKEL_BESTIMMTER NOUN_DOG VERB_BEMERKEN OBJECT ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_FOOD_RATION.", ""}
		{"VERB_WOLLEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT PRONOMEN_PERSONAL in OBJECT ARTIKEL_UNBESTIMMTER NOUN_WOLF verwandeln?",
		 "Willst du dich in einen Wolf verwandeln?"},
		{"SUBJECT NOUN_JEMAND VERB_VERSUCHEN OBJECT PRONOMEN_PERSONAL zu bestehlen aber VERB_FINDEN nichts.",
		"Jemand versucht dich zu bestehlen aber findet nichts."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_NYMPH VERB_VERSUCHEN OBJECT PRONOMEN_PERSONAL zu bestehlen, aber bei OBJECT KASUS_DATIV PRONOMEN_PERSONAL gibt es nichts zu holen!",
		"Die Nymphe versucht dich zu bestehlen, aber bei dir gibt es nichts zu holen!"},
		{"Der Gedanke an Piroschka VERB_LASSEN OBJECT PRONOMEN_PERSONAL alles vergessen.",
		 "Der Gedanke an Piroschka l�sst dich alles vergessen."},
		{"SUBJECT ARTIKEL_UNBESTIMMTER NOUN_OBJEKT VERB_ERSCHEINEN OBJECT vor KASUS_DATIV PRONOMEN_POSSESSIV NOUN_FOOTs!",
		 "Ein Objekt erscheint vor deinen F��en!"},
		{"Pl�tzlich VERB_VERSCHWINDEN SUBJECT_IM_SATZ ARTIKEL_BESTIMMTER NOUN_WOLF.",
		 "Pl�tzlich verschwindet der Wolf."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_FALLEN in eine Erdspalte!",
		 "Du f�llst in eine Erdspalte!"},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_DOG VERB_ZUHOEREN fr�hlich der Musik SATZKLAMMER und VERB_ERSCHEINEN danach ruhiger.",
		 "Der Hund h�rt fr�hlich der Musik zu und erscheint danach ruhiger."},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_SUCCUBUS VERB_HERANMACHEN sich an OBJECT PRONOMEN_PERSONAL SATZKLAMMER.",
		 "Der Sukkubus macht sich an dich heran."},
		{"Seltsamerweise VERB_SICH_FUEHLEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT PRONOMEN_PERSONAL besser als vorher.",
		 "Seltsamerweise f�hlst du dich besser als vorher."},

	};

	check_strings(text, sizeof(text)/8);
} END_TEST


START_TEST (test_hoeren) {
	char *text[][2] = {
		{"Nur schwach VERB_HEAR SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT ARTIKEL_UNBESTIMMTER NOUN_WOLFs den Mond anheulen.",
		 "Nur schwach h�rst du W�lfe den Mond anheulen."},
		{"Im Traum VERB_HEAR SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT ARTIKEL_UNBESTIMMTER NOUN_WOLF den Mond anheulen.",
		 "Im Traum h�rst du einen Wolf den Mond anheulen."},
		{"SUBJECT PRONOMEN_PERSONAL VERB_HEAR OBJECT ARTIKEL_UNBESTIMMTER NOUN_WOLF den Mond anheulen.",
		 "Du h�rst einen Wolf den Mond anheulen."},
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
		 "Die T�re geht auf."},
		{"Hier ist niemand, den SUBJECT_IM_SATZ PRONOMEN_PERSONAL um Rat fragen MODIFIER_KONJUNKTIV_II VERB_KOENNEN.",
		 "Hier ist niemand, den du um Rat fragen k�nntest."},
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
		{names[ 3], "R�stzeug"},
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
//#endif

void check_german2meta(char* text[][2], int size) {
	int i;
	char result[128];

	for (i=0; i<size; i++) {
		german2meta(text[i][0],result);
		fail_unless((strcmp(result, text[i][1])==0),
								"failed german2meta\nto convert: >%s<\nconverted:  >%s<\nexpected:   >%s<\n",
								text[i][0],result,text[i][1]);
	}
}

START_TEST (test_german2meta) {
	char *text[][2] = {{"Augenbinde",  "NOUN_BLINDFOLD"},
										 {"Augenbinden", "NOUN_BLINDFOLDs"},
										 {"eine Augenbinden", "ARTIKEL_UNBESTIMMTER NOUN_BLINDFOLDs"},
										 {"der Hund", "ARTIKEL_BESTIMMTER NOUN_DOG"},
										 {"einen grauen Stein", "ARTIKEL_UNBESTIMMTER ADJEKTIV_GEM_GRAY NOUN_GEM_ROCK"},
										 {"einen geheiligter Rubin", "ARTIKEL_UNBESTIMMTER ADJEKTIV_BLESSED NOUN_GEM_RUBY"},
										 {"ein geheiligter rubinroter Trank", "ARTIKEL_UNBESTIMMTER ADJEKTIV_BLESSED ADJEKTIV_POT_RUBY NOUN_POTION"},
										 {"eine halb verspeiste Essensration", "ARTIKEL_UNBESTIMMTER halb ADJEKTIV_EATEN NOUN_FOOD_RATION"},
										 {"eine rote Zauberbuches", "ARTIKEL_UNBESTIMMTER ADJEKTIV_SPE_RED NOUN_SPELLBOOK"},
										 {"einen roten Stein", "ARTIKEL_UNBESTIMMTER ADJEKTIV_GEM_RED NOUN_GEM_ROCK"},
										 {"2 rote Zauberb�cher", "2 ADJEKTIV_SPE_RED NOUN_SPELLBOOKs"},
										 {"Ring des Schleichens", "NOUN_RING PARTIKEL_OF NOUN_RING_STEALTH"},
										 {"ein verfluchter grauer Stein", "ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED ADJEKTIV_GEM_GRAY NOUN_GEM_ROCK"},
										 {"geheiligte lange Samuraischwerter", "ADJEKTIV_BLESSED NOUN_LONG_SAMURAI_SWORDs"},
										 {"einen Perlenring", "ARTIKEL_UNBESTIMMTER NOUN_RING_UNIDENTIFIED_PEARL"},
										 {"ein geheiligter silberner Zauberstab", "ARTIKEL_UNBESTIMMTER ADJEKTIV_BLESSED ADJEKTIV_WAND_SILVER NOUN_WAND"},
										 {"ein Zauberstab aus Platin", "ARTIKEL_UNBESTIMMTER MADE_OF_WAND_PLATINUM NOUN_WAND"},
										 {"ein nicht verfluchtes Stethoskop", "ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_STETHOSCOPE"},
										 {"5 nicht verfluchte �pfel", "5 ADJEKTIV_UNCURSED NOUN_APPLEs"},
										 {"ein nicht verfluchtes +1 Paar Lederhandschuhe", "ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED +1 NOUN_PAAR NOUN_LEATHER_GLOVESs"},
										 {"eine Flasche Wasser", "ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_WATER"},
										 {"Flaschen Wasser", "NOUN_POTIONs PARTIKEL_OF NOUN_POT_WATER"},
										 {"eine Flasche Weihwasser", "ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_HOLY_WATER"},
										 {"eine Flasche Satanswasser", "ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_UNHOLY_WATER"},
										 {"eine sechseckiges Amulett", "ARTIKEL_UNBESTIMMTER ADJEKTIV_AMULET_HEXAGONAL NOUN_AMULET"},
										 {"die sterblichen �berreste einer Vampirfledermaus", "MODIFIER_CORPSE ARTIKEL_UNBESTIMMTER NOUN_VAMPIRE_BAT NOUN_CORPSE"},
										 {"Leichen von Vampirflederm�usen", "MODIFIER_CORPSE PARTIKEL_VON NOUN_VAMPIRE_BATs NOUN_CORPSE"},
										 {"Zauberstab der Monsterbeschw�rung", "NOUN_WAND PARTIKEL_OF NOUN_WAND_CREATE_MONSTER"},
										 {"ein Zauberstab der Hast (0:4)", "ARTIKEL_UNBESTIMMTER NOUN_WAND PARTIKEL_OF NOUN_WAND_SPEED_MONSTER (0:4)"},
										 {"Trank des Schlafes", "NOUN_POTION PARTIKEL_OF NOUN_POT_SLEEPING"},
										 {"Zauberbuch des Schlafes", "NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_SLEEP"},
										 {"verfluchtes Schwert", "ADJEKTIV_CURSED NOUN_SWORD"},

										 //{"eine nicht verfluchte Dose mit Spinat", "ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_TIN PARTIKEL_OF NOUN_SPINACH"},
	};

	check_german2meta(text, sizeof(text)/8);
} END_TEST

//#if 0
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
		{"SUBJECT ARTIKEL_BESTIMMTER sex2: f NOUN_DOG VERB_SCHLAGEN mit KASUS_DATIV PRONOMEN_3P_F_POSSESSIV NOUN_BOW nach KASUS_DATIV ARTIKEL_BESTIMMTER sex2: m NOUN_DOG.",
		 ""},
		{"SUBJECT ARTIKEL_BESTIMMTER NOUN_KOP_SERGEANT VERB_SCHLAGEN mit KASUS_DATIV PRONOMEN_3P_MN_POSSESSIV NOUN_RUBBER_HOSE nach KASUS_DATIV ARTIKEL_BESTIMMTER NOUN_KOP_SERGEANT.",
		 ""},
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_nominal_phrasen) {
	char *text[][2] = {
		{"NOUN_VULGAR_POLEARM", "Gew�hnliche Stangenwaffe"},
		{"NOUN_BAG_OF_HOLDING", "Nimmervoller Beutel"},
		{"NOUN_BAG_OF_HOLDING", "Nimmervoller Beutel"},
		{"NOUN_LUCERN_HAMMER", "Luzerner Hammer"},
		{"NOUN_LONG_SAMURAI_SWORDs", "Lange Samuraischwerter"},
		{"ARTIKEL_BESTIMMTER NOUN_DOG", "Der Hund"},
		{"ARTIKEL_BESTIMMTER NOUN_DOGs", "Die Hunde"},
		{"ARTIKEL_UNBESTIMMTER NOUN_DOG", "Ein Hund"},
		{"ARTIKEL_NULL NOUN_DOG", "Hund"},
		{"ARTIKEL_NULL NOUN_DOGs", "Hunde"},
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_scrolls) {
	char *text[][2] = {
		{"n - 2 NOUN_SCROLLs PARTIKEL_OF NOUN_SCR_FIRE",
		 "n - 2 Schriftrollen des Feuers"},
		 //"n - 2 Schriftrollen \"Feuer\""},
		{"m - ARTIKEL_UNBESTIMMTER NOUN_SCROLL PARTIKEL_OF NOUN_SCR_LIGHT PARTIKEL_NAMED unbekannt",
		 "m - eine Schriftrolle des Lichtes namens unbekannt"},
		{"m - ARTIKEL_UNBESTIMMTER NOUN_SCROLL PARTIKEL_LABELED \"NOUN_SCR_ELBIB_YLOH\" PARTIKEL_NAMED unbekannt",
		 "m - eine Schriftrolle \"TFIRHCS EGILIEH\" namens unbekannt"},
		{"p - ARTIKEL_UNBESTIMMTER NOUN_SCROLL PARTIKEL_CALLED teuer",
		 "p - eine Schriftrolle genannt teuer"},

		{"q - ARTIKEL_UNBESTIMMTER NOUN_SCROLL PARTIKEL_LABELED \"NOUN_SCR_READ_ME\".",
		 "q - eine Schriftrolle \"LIES MICH\"."},
		{"r - 3 NOUN_SCROLLs PARTIKEL_LABELED \"NOUN_SCR_READ_ME\".",
		 "r - 3 Schriftrollen \"LIES MICH\"."}
	};

	check_strings(text, sizeof(text)/8);
} END_TEST

START_TEST (test_gems) {
	char *text[][2] = {
		{"M - ARTIKEL_UNBESTIMMTER ADJEKTIV_GEM_GREEN NOUN_GEM", "M - ein gr�ner Schmuckstein"},
    {"N - ARTIKEL_UNBESTIMMTER ADJEKTIV_GEM_VIOLET NOUN_GEM", "N - ein violetter Schmuckstein"},
    {"O - ARTIKEL_UNBESTIMMTER ADJEKTIV_GEM_GRAY NOUN_STONE", "O - ein grauer Stein"},
    {"P - ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED ADJEKTIV_GEM_GRAY NOUN_STONE", "P - ein verfluchter grauer Stein"},
    {"Q - ARTIKEL_UNBESTIMMTER ADJEKTIV_GEM_GRAY NOUN_STONE", "Q - ein grauer Stein"},
    {"R - ARTIKEL_UNBESTIMMTER ADJEKTIV_GEM_GRAY NOUN_STONE", "R - ein grauer Stein"},
    {"S - ARTIKEL_UNBESTIMMTER NOUN_GEM_ROCK", "S - ein Stein"}
	};

	check_strings(text, sizeof(text)/8);
} END_TEST



START_TEST (test_get_meta_substantiv_with) {
	struct substantiv_oder_adjekiv_struct *result = get_meta_substantiv_with("rot", "ADJEKTIV_SPE_");
	fail_unless(strcmp(result->typ,"ADJEKTIV_SPE_RED")==0);

	result = get_meta_substantiv_with("rot", "ADJEKTIV_GEM_");
	fail_unless(strcmp(result->typ,"ADJEKTIV_GEM_RED")==0);
} END_TEST



START_TEST (test_paar) {
	char *text[][2] = {

		// Paar + Nix
		{"a - ARTIKEL_UNBESTIMMTER NOUN_PAAR NOUN_OLD_GLOVES",
		 "a - ein Paar alte Handschuhe"},
		{"b - KASUS_GENITIV ARTIKEL_UNBESTIMMTER NOUN_PAAR NOUN_OLD_GLOVES",
		 "b - eines Paares alter Handschuhe"},
		{"c - mit KASUS_DATIV ARTIKEL_UNBESTIMMTER NOUN_PAAR NOUN_OLD_GLOVES",
		 "c - mit einem Paare alter Handschuhe"},
		{"d - f�r KASUS_AKKUSATIV ARTIKEL_UNBESTIMMTER NOUN_PAAR NOUN_OLD_GLOVES",
		 "d - f�r ein Paar alter Handschuhe"},

		// Paar + Kasus von Paar
		{"e - ARTIKEL_UNBESTIMMTER NOUN_PAAR NUMERUS_PLURAL ARTIKEL_NULL NOUN_OLD_GLOVES",
		 "e - ein Paar alte Handschuhe"},
		{"f - KASUS_GENITIV ARTIKEL_BESTIMMTER NOUN_PAAR NUMERUS_PLURAL ARTIKEL_NULL NOUN_OLD_GLOVES",
		 "f - des Paares alter Handschuhe"},
		{"g - mit KASUS_DATIV ARTIKEL_UNBESTIMMTER NOUN_PAAR KASUS_DATIV NUMERUS_PLURAL ARTIKEL_NULL NOUN_OLD_GLOVES",
		 "g - mit einem Paare alten Handschuhen"},
		{"h - f�r ARTIKEL_UNBESTIMMTER NOUN_PAAR KASUS_AKKUSATIV NUMERUS_PLURAL ARTIKEL_NULL NOUN_OLD_GLOVES",
		 "h - f�r ein Paar alte Handschuhe"},

		// Paar + Genitiv
		{"i - ARTIKEL_UNBESTIMMTER NOUN_PAAR KASUS_GENITIV NUMERUS_PLURAL ARTIKEL_NULL NOUN_OLD_GLOVES",
		 "i - ein Paar alter Handschuhe"},
		{"j - KASUS_GENITIV ARTIKEL_BESTIMMTER NOUN_PAAR KASUS_GENITIV NUMERUS_PLURAL ARTIKEL_NULL NOUN_OLD_GLOVES",
		 "j - des Paares alter Handschuhe"},
		{"k - mit KASUS_DATIV ARTIKEL_UNBESTIMMTER NOUN_PAAR KASUS_GENITIV NUMERUS_PLURAL ARTIKEL_NULL NOUN_OLD_GLOVES",
		 "k - mit einem Paare alter Handschuhe"},
		{"l - f�r ARTIKEL_UNBESTIMMTER NOUN_PAAR KASUS_GENITIV NUMERUS_PLURAL ARTIKEL_NULL NOUN_OLD_GLOVES",
		 "l - f�r ein Paar alter Handschuhe"},

		// Paar + Substantiv
		{"m - ARTIKEL_UNBESTIMMTER NOUN_PAAR NOUN_GLOVES",
		 "m - ein Paar Handschuhe"},
		{"n - KASUS_GENITIV ARTIKEL_UNBESTIMMTER NOUN_PAAR NOUN_GLOVES",
		 "n - eines Paares Handschuhe"},
		{"o - mit KASUS_DATIV ARTIKEL_UNBESTIMMTER NOUN_PAAR NOUN_GLOVES",
		 "o - mit einem Paare Handschuhe"},
		{"p - f�r KASUS_AKKUSATIV ARTIKEL_UNBESTIMMTER NOUN_PAAR NOUN_GLOVES",
		 "p - f�r ein Paar Handschuhe"},

	};

	check_strings(text, sizeof(text)/8);
} END_TEST

//#endif

Suite *test_suite(void)
{
  Suite *s = suite_create("all tests");
  TCase *tc_core = tcase_create("Nethack");

  suite_add_tcase (s, tc_core);
  tcase_add_test(tc_core, test_complete_sentences3);

	if (0) {
		//tcase_add_test(tc_core, test_german2meta);
	tcase_add_test(tc_core, test_get_meta_substantiv_with);
	tcase_add_test(tc_core, test_paar);
  tcase_add_test(tc_core, test_identified_spellbooks);
	tcase_add_test(tc_core, test_nominal_phrasen);
	tcase_add_test(tc_core, test_possessiv);
	tcase_add_test(tc_core, test_statues);
	tcase_add_test(tc_core, test_verbs);
	//tcase_add_test(tc_core, test_linking_elements);
	tcase_add_test(tc_core, test_wands);
  tcase_add_test(tc_core, test_spellbooks);
  tcase_add_test(tc_core, test_potions);
  tcase_add_test(tc_core, test_called_named_labeled);
  tcase_add_test(tc_core, test_level_sounds);
  tcase_add_test(tc_core, test_tools);
  tcase_add_test(tc_core, test_passiv);
  tcase_add_test(tc_core, test_complete_sentences);
  tcase_add_test(tc_core, test_complete_sentences2);
  tcase_add_test(tc_core, test_complete_sentences3);
  tcase_add_test(tc_core, test_corpses);
	tcase_add_test(tc_core, test_tincontent);
	tcase_add_test(tc_core, test_inventory_names);
	tcase_add_test(tc_core, test_casus_and_modifier);
	tcase_add_test(tc_core, test_rings);
	tcase_add_test(tc_core, test_scrolls);
	tcase_add_test(tc_core, test_gems);
	tcase_add_test(tc_core, test_paar);
  tcase_add_test(tc_core, test_hoeren);
	}

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
