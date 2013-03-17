/*	SCCS Id: @(#)role.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef GERMAN
# include "german.h"
# include <ctype.h>
#endif

/*** Table of all roles ***/
/* According to AD&D, HD for some classes (ex. Wizard) should be smaller
 * (4-sided for wizards).  But this is not AD&D, and using the AD&D
 * rule here produces an unplayable character.  Thus I have used a minimum
 * of an 10-sided hit die for everything.  Another AD&D change: wizards get
 * a minimum strength of 4 since without one you can't teleport or cast
 * spells. --KAA
 *
 * As the wizard has been updated (wizard patch 5 jun '96) their HD can be
 * brought closer into line with AD&D. This forces wizards to use magic more
 * and distance themselves from their attackers. --LSZ
 *
 * With the introduction of races, some hit points and energy
 * has been reallocated for each race.  The values assigned
 * to the roles has been reduced by the amount allocated to
 * humans.  --KMH
 *
 * God names use a leading underscore to flag goddesses.
 */
/* http://www.tamoon.ch/titel.html */
const struct Role roles[] = {
{	{"NOUN_ARCHAEOLOGE", "NOUN_ARCHAEOLOGIN"}, { /* EN {	{"Archeologist", 0}, { */
	{"NOUN_HILFSKRAFT",      0}, /* EN {"Digger",      0}, */
	{"NOUN_FELDFORSCHER","NOUN_FELDFORSCHERIN"}, /* EN {"Field Worker",0}, */
	{"NOUN_EXHUMIERER","NOUN_EXHUMIERERIN"}, /* EN {"Investigator",0}, */
	{"NOUN_AUSGRAEBER","NOUN_AUSGRAEBERIN"}, /* EN {"Exhumer",     0}, */
	{"NOUN_PROJEKTLEITER",   "NOUN_PROJEKTLEITERIN"}, /* EN {"Excavator",   0}, */
	{"NOUN_HOEHLENGEHER",   "NOUN_HOEHLENGEHERIN"}, /* EN {"Spelunker",   0}, */ /* Hoehlengeher / Hoehlenforscher */
	{"NOUN_SPELAEOLOGE","NOUN_SPELAEOLOGIN"}, /* EN {"Speleologist",0}, */ /* http://de.wikipedia.org/wiki/Speläologie#Spel.C3.A4ologe.2FH.C3.B6hlenforscher */
	{"NOUN_SAMMLER",   "NOUN_SAMMLERIN"}, /* EN {"Collector",   0}, */
	{"NOUN_KURATOR",     "NOUN_KURATORIN"} }, /* EN {"Curator",     0} }, */
	"Quetzalcoatl", "Camaxtli", "Huhetotl", /* Central American */
	"Arc", "ARTIKEL_BESTIMMTER NOUN_ARC_INSTITUT", "ARTIKEL_BESTIMMTER NOUN_TOMB der toltekischen Könige", /* EN "Arc", "the College of Archeology", "the Tomb of the Toltec Kings", */
	PM_ARCHEOLOGIST, NON_PM, NON_PM,
	PM_LORD_CARNARVON, PM_STUDENT, PM_MINION_OF_HUHETOTL,
	NON_PM, PM_HUMAN_MUMMY, S_SNAKE, S_MUMMY,
	ART_ORB_OF_DETECTION,
	MH_HUMAN|MH_DWARF|MH_GNOME | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10, 10,  7,  7,  7 },
	{  20, 20, 20, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_MAGIC_MAPPING,   -4
},
{	{"NOUN_BARBAR", "NOUN_BARBARIN"}, { /* EN {	{"Barbarian", 0}, { */
	{"NOUN_STRAUCHDIEB",   "NOUN_STRAUCHDIEBIN"}, /* EN {"Plunderer",   "Plunderess"}, */
	{"NOUN_BRANDSCHATZER", "NOUN_BRANDSCHATZERIN"}, /* EN {"Pillager",    0}, */
	{"NOUN_BANDIT", "NOUN_BANDITIN"}, /* EN {"Bandit",      0}, */
	{"NOUN_BRIGANT", "NOUN_BRIGANTIN"}, /* EN {"Brigand",     0}, */
	{"NOUN_RAEUBER", "NOUN_RAEUBERIN"}, /* EN {"Raider",      0}, */
	{"NOUN_PLUENDERER", "NOUN_PLUENDERIN"}, /* EN {"Reaver",      0}, */
	{"NOUN_TOTSCHLAEGER", "NOUN_TOTSCHLAEGERIN"}, /* EN {"Slayer",      0}, */
	{"NOUN_STAMMESFUEHRER",   "NOUN_STAMMESFUEHRERIN"}, /* EN {"Chieftain",   "Chieftainess"}, */ /* Haeuptling / Stammesführer */
	{"NOUN_EROBERER",   "NOUN_EROBERIN"} }, /* EN {"Conqueror",   "Conqueress"} }, */
	"Mitra", "Crom", "Set", /* Hyborian */
	"Bar", "ARTIKEL_BESTIMMTER NOUN_CAMP des Stammes der Duali", "ARTIKEL_BESTIMMTER NOUN_DUALI_OASIS", /* EN "Bar", "the Camp of the Duali Tribe", "the Duali Oasis", */
	PM_BARBARIAN, NON_PM, NON_PM,
	PM_PELIAS, PM_CHIEFTAIN, PM_THOTH_AMON,
	PM_OGRE, PM_TROLL, S_OGRE, S_TROLL,
	ART_HEART_OF_AHRIMAN,
	MH_HUMAN|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  16,  7,  7, 15, 16,  6 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 14, 0,  0,10,  2, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 14, 0, 0,  8, A_INT, SPE_HASTE_SELF,      -4
},
{	{"NOUN_CAVEMAN", 0}, { /* EN {	{"Caveman", "Cavewoman"}, { */
	{"NOUN_TROGLODYT",    "NOUN_TROGLODYTIN"}, /* EN {"Troglodyte",  0}, */
	{"NOUN_EINGEBORENER", "NOUN_EINGEBORENE"}, /* EN {"Aborigine",   0}, */
	{"NOUN_HERUMTREIBER", "NOUN_HERUMTREIBERIN"}, /* EN {"Wanderer",    0}, */
	{"NOUN_LANDSTREICHER","NOUN_LANDSTREICHERIN"}, /* EN {"Vagrant",     0}, */
	{"NOUN_VAGABUND",     "NOUN_VAGABUNDIN"}, /* EN {"Wayfarer",    0}, */
	{"NOUN_VAGANT",       "NOUN_VAGANTIN"}, /* EN {"Roamer",      0}, */
	{"NOUN_NOMADE",       "NOUN_NOMADIN"}, /* EN {"Nomad",       0}, */
	{"NOUN_WANDERER",     "NOUN_WANDERIN"}, /* EN {"Rover",       0}, */
	{"NOUN_PIONIER",      "NOUN_PIONIERIN"} }, /* EN {"Pioneer",     0} }, */
	"Anu", "_Ishtar", "Anshar", /* Babylonian */
	"Cav", "ARTIKEL_BESTIMMTER NOUN_CAVEs der Vorfahren", "ARTIKEL_BESTIMMTER NOUN_DRAGONS_LAIR", /* EN "Cav", "the Caves of the Ancestors", "the Dragon's Lair", */
	PM_CAVEMAN, PM_CAVEWOMAN, PM_LITTLE_DOG,
	PM_SHAMAN_KARNOV, PM_NEANDERTHAL, PM_CHROMATIC_DRAGON,
	PM_BUGBEAR, PM_HILL_GIANT, S_HUMANOID, S_GIANT,
	ART_SCEPTRE_OF_MIGHT,
	MH_HUMAN|MH_DWARF|MH_GNOME | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7,  8,  6 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  2, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 12, 0, 1,  8, A_INT, SPE_DIG,             -4
},
{	{"NOUN_HEILER", "NOUN_HEILERIN"}, { /* EN {	{"Healer", 0}, { */
	{"NOUN_RHIZOTOMIST",    "NOUN_RHIZOTOMISTIN"}, /* EN {"Rhizotomist",    0}, */
	{"NOUN_EMPIRIC",        "NOUN_EMPIRICIN"}, /* EN {"Empiric",        0}, */ /* http://de.wikipedia.org/wiki/Empiriker_%28%C3%84rzteschule%29 */
	{"NOUN_EMBALMER",       "NOUN_EMBALMERIN"}, /* EN {"Embalmer",       0}, */
	{"NOUN_DRESSER",        "NOUN_DRESSERIN"}, /* EN {"Dresser",        0}, */ /* http://cancerweb.ncl.ac.uk/cgi-bin/omd?dresser */
	{"NOUN_MEDICUS_OSSIUM", "NOUN_MEDICA_OSSIUM"}, /* EN {"Medicus ossium", "Medica ossium"}, */
	{"NOUN_HERBALIST",      "NOUN_HERBALISTIN"}, /* EN {"Herbalist",      0}, */
	{"NOUN_MAGISTER",       "NOUN_MAGISTRA"}, /* EN {"Magister",       "Magistra"}, */
	{"NOUN_PHYSICIAN",      "NOUN_PHYSICIANIN"}, /* EN {"Physician",      0}, */
	{"NOUN_CHIRURGEON",     "NOUN_CHIRURGEON"} }, /* EN {"Chirurgeon",     0} }, */
	"_Athena", "Hermes", "Poseidon", /* Greek */
	"Hea", "ARTIKEL_BESTIMMTER NOUN_TEMPEL von Epidauros", "ARTIKEL_BESTIMMTER NOUN_TEMPEL des Koios", /* EN "Hea", "the Temple of Epidaurus", "the Temple of Coeus", */
	PM_HEALER, NON_PM, NON_PM, 
	PM_HIPPOCRATES, PM_ATTENDANT, PM_CYCLOPS,
	PM_GIANT_RAT, PM_SNAKE, S_RODENT, S_YETI,
	ART_STAFF_OF_AESCULAPIUS,
	MH_HUMAN|MH_GNOME | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 13,  7, 11, 16 },
	{  15, 20, 20, 15, 25, 5 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_CURE_SICKNESS,   -4
},
{	{"NOUN_KNIGHT", 0}, { /* EN {	{"Knight", 0}, { */
	{"NOUN_GALLANT",      "NOUN_GALLANT_IN"}, /* EN {"Gallant",     0}, */
	{"NOUN_JUNKER",       "NOUN_JUNKFRAU"}, /* EN {"Esquire",     0}, */
	{"NOUN_KAVALIER",     "NOUN_KAVALIERIN"}, /* EN {"Bachelor",    0}, */
	{"NOUN_EDELMANN",     "NOUN_EDELFRAU"}, /* EN {"Sergeant",    0}, */
	{"NOUN_RITTER",       "NOUN_DAME"}, /* EN {"Knight",      0}, */
	{"NOUN_BANNERHERR",   "NOUN_BANNERFRAU"}, /* EN {"Banneret",    0}, */
	{"NOUN_ORDENSRITTER", "NOUN_ORDENSRITTERIN"}, /* EN {"Chevalier",   "Chevaliere"}, */
	{"NOUN_FREIHERR",     "NOUN_FREIFRAU"}, /* EN {"Seignieur",   "Dame"}, */
	{"NOUN_PALADIN",      "NOUN_PALADIN_IN"} }, /* EN {"Paladin",     0} }, */
	"Lugh", "_Brigit", "Manannan Mac Lir", /* Celtic */
	"Kni", "NOUN_CAMELOT", "ARTIKEL_BESTIMMTER NOUN_INSEL aus Glas", /* EN "Kni", "Camelot Castle", "the Isle of Glass", */
	PM_KNIGHT, NON_PM, PM_PONY,
	PM_KING_ARTHUR, PM_PAGE, PM_IXOTH,
	PM_QUASIT, PM_OCHRE_JELLY, S_IMP, S_JELLY,
	ART_MAGIC_MIRROR_OF_MERLIN,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  13,  7, 14,  8, 10, 17 },
	{  30, 15, 15, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  2, 0 },	/* Hit points */
	{  1, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4
},
{	{"NOUN_MOENCH", 0}, { /* EN {	{"Monk", 0}, { */
	{"NOUN_KANDIDAT",         "NOUN_KANDIDATIN"}, /* EN {"Candidate",         0}, */
	{"NOUN_NOVIZE",            "NOUN_NOVIZIN"}, /* EN {"Novice",            0}, */
	{"NOUN_GEWEIHTE",          "NOUN_GEWEIHTIN"}, /* EN {"Initiate",          0}, */
	{"NOUN_SCHUELER_DER_STEINE", "NOUN_SCHUELERIN_DER_STEINE"}, /* EN {"Student of Stones", 0}, */
	{"NOUN_SCHUELER_DER_WASSER", "NOUN_SCHUELERIN_DER_WASSER"}, /* EN {"Student of Waters", 0}, */
	{"NOUN_SCHUELER_DER_METALLE", "NOUN_SCHUELERIN_DER_METALLE"}, /* EN {"Student of Metals", 0}, */
	{"NOUN_SCHUELER_DER_WINDE",  "NOUN_SCHUELERIN_DER_WINDE"}, /* EN {"Student of Winds",  0}, */
	{"NOUN_SCHUELER_DER_FEUER",   "NOUN_SCHUELERIN_DER_FEUER"}, /* EN {"Student of Fire",   0}, */
	{"NOUN_MEISTER",            "NOUN_MEISTERIN"} }, /* EN {"Master",            0} }, */
	"Shan Lai Ching", "Chih Sung-tzu", "Huan Ti", /* Chinese */
	"Mon", "ARTIKEL_BESTIMMTER NOUN_MONASTERY von Chan-Sun", /* EN "Mon", "the Monastery of Chan-Sune", */
	  "ARTIKEL_BESTIMMTER NOUN_MONASTERY des Erdfürsten", /* EN "the Monastery of the Earth-Lord", */
	PM_MONK, NON_PM, NON_PM,
	PM_GRAND_MASTER, PM_ABBOT, PM_MASTER_KAEN,
	PM_EARTH_ELEMENTAL, PM_XORN, S_ELEMENTAL, S_XORN,
	ART_EYES_OF_THE_OVERWORLD,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  8,  8,  7,  7 },
	{  25, 10, 20, 20, 15, 10 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 2,  0, 2,  0, 2 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_RESTORE_ABILITY, -4
},
{	{"NOUN_PRIESTER",    "NOUN_PRIESTERIN"}, { /* EN {	{"Priest", "Priestess"}, { */
	{"NOUN_ASPIRANT",    "NOUN_ASPIRANTIN"}, /* EN {"Aspirant",    0}, */
	{"NOUN_ACOLYTE",     "NOUN_ACOLYTEIN"}, /* EN {"Acolyte",     0}, */
	{"NOUN_ADEPT",       "NOUN_ADEPTIN"}, /* EN {"Adept",       0}, */
	{"NOUN_PRIESTER",    "NOUN_PRIESTERIN"}, /* EN {"Priest",      "Priestess"}, */
	{"NOUN_CURATE",      "NOUN_CURATEIN"}, /* EN {"Curate",      0}, */
	{"NOUN_CANON",       "NOUN_CANONESS"}, /* EN {"Canon",       "Canoness"}, */
	{"NOUN_LAMA",        "NOUN_LAMAIN"}, /* EN {"Lama",        0}, */
	{"NOUN_PATRIARCH",   "NOUN_MATRIARCH"}, /* EN {"Patriarch",   "Matriarch"}, */
	{"NOUN_HIGH_PRIEST", "NOUN_HIGH_PRIESTESS"} }, /* EN {"High Priest", "High Priestess"} }, */
	0, 0, 0,	/* chosen randomly from among the other roles */
	"Pri", "ARTIKEL_BESTIMMTER NOUN_GREAT_TEMPLE", "ARTIKEL_BESTIMMTER NOUN_TEMPEL des Nalzok", /* EN "Pri", "the Great Temple", "the Temple of Nalzok", */
	PM_PRIEST, PM_PRIESTESS, NON_PM,
	PM_ARCH_PRIEST, PM_ACOLYTE, PM_NALZOK,
	PM_HUMAN_ZOMBIE, PM_WRAITH, S_ZOMBIE, S_WRAITH,
	ART_MITRE_OF_HOLINESS,
	MH_HUMAN|MH_ELF | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 10,  7,  7,  7 },
	{  15, 10, 30, 15, 20, 10 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 8,  1, 0 },	/* Hit points */
	{  4, 3,  0, 2,  0, 2 },10,	/* Energy */
	0, 3,-2, 2, 10, A_WIS, SPE_REMOVE_CURSE,    -4
},
  /* Note:  Rogue precedes Ranger so that use of `-R' on the command line
     retains its traditional meaning. */
{	{"NOUN_ROGUE", 0}, { /* EN {	{"Rogue", 0}, { */
	{"NOUN_STRASSENRAEUBER",     "NOUN_STRASSENRAEUBERIN"}, /* EN {"Footpad",     0}, */
	{"NOUN_LANGFINGER",    0}, /* EN {"Cutpurse",    0}, */
	{"NOUN_HALLODRI",       0}, /* EN {"Rogue",       0}, */
	{"NOUN_GANOVE",    "NOUN_GANOVIN"}, /* EN {"Pilferer",    0}, */
	{"NOUN_RAEUBER",      "NOUN_RAEUBERIN"}, /* EN {"Robber",      0}, */
	{"NOUN_EINBRECHER",     "NOUN_EINBRECHERIN"}, /* EN {"Burglar",     0}, */
	{"NOUN_BETRUEGER",     "NOUN_BETRUEGERIN"}, /* EN {"Filcher",     0}, */
	{"NOUN_GANGSTER",     "NOUN_GANGSTERBRAUT"}, /* EN {"Magsman",     "Magswoman"}, */
	{"NOUN_DIEB",       "NOUN_DIEBIN"} }, /* EN {"Thief",       0} }, */
	"Issek", "Mog", "Kos", /* Nehwon */
	"Rog", "ARTIKEL_BESTIMMTER NOUN_HALLE der Diebesgilde", "ARTIKEL_BESTIMMTER NOUN_HALLE der Assassinengilde", /* EN "Rog", "the Thieves' Guild Hall", "the Assassins' Guild Hall", */
	PM_ROGUE, NON_PM, NON_PM,
	PM_MASTER_OF_THIEVES, PM_THUG, PM_MASTER_ASSASSIN,
	PM_LEPRECHAUN, PM_GUARDIAN_NAGA, S_NYMPH, S_NAGA,
	ART_MASTER_KEY_OF_THIEVERY,
	MH_HUMAN|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7, 10,  7,  6 },
	{  20, 10, 10, 30, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_DETECT_TREASURE, -4
},
{	{"NOUN_RANGER", 0}, { /* EN {	{"Ranger", 0}, { */
#if 0	/* OBSOLETE */
	{"Edhel",       "Elleth"},
	{"Edhel",       "Elleth"},      /* elf-maid */
	{"Ohtar",       "Ohtie"},       /* warrior */
	{"Kano",			/* commander (Q.) ['a] */
			"Kanie"},	/* educated guess, until further research- SAC */
	{"Arandur",			/* king's servant, minister (Q.) - guess */
			"Aranduriel"},	/* educated guess */
	{"Hir",         "Hiril"},       /* lord, lady (S.) ['ir] */
	{"Aredhel",     "Arwen"},       /* noble elf, maiden (S.) */
	{"Ernil",       "Elentariel"},  /* prince (S.), elf-maiden (Q.) */
	{"Elentar",     "Elentari"},	/* Star-king, -queen (Q.) */
	"Solonor Thelandira", "Aerdrie Faenya", "Lolth", /* Elven */
#endif
	{"NOUN_JUNGSPUND",    0}, /* EN {"Tenderfoot",    0}, */ 
	{"NOUN_AUSGUCK",       0}, /* EN {"Lookout",       0}, */ /* Ausguck, Schmiere-Steher */
	{"NOUN_VORREITER",   "NOUN_VORREITERIN"}, /* EN {"Trailblazer",   0}, */ // TODO DE Bahnbrecher/Pionier/Pistensucher/Vorreiter/Wegbereiter
	{"NOUN_SPAEHER", "NOUN_SPAEHERIN"}, /* EN {"Reconnoiterer", "Reconnoiteress"}, */
	{"NOUN_KUNDSCHAFTER",         "NOUN_KUNDSCHAFTERIN"}, /* EN {"Scout",         0}, */
	{"NOUN_ARMBRUSTER",    "NOUN_ARMBRUSTERIN"},	/* One skilled at crossbows */ /* EN {"Arbalester",    0}, */ /* Armbruster / Armbrustschütze */
	{"NOUN_BOGENSCHUETZE",        "NOUN_BOGENSCHUETZIN"}, /* EN {"Archer",        0}, */
	{"NOUN_SCHARFSCHUETZE",  "NOUN_SCHARFSCHUETZIN"}, /* EN {"Sharpshooter",  0}, */
	{"NOUN_MEISTERSCHUETZE",      "NOUN_MEISTERSCHUETZIN"} }, /* EN {"Marksman",      "Markswoman"} }, */
	"Merkur", "_Venus", "Mars", /* Roman/planets */ /* EN "Mercury", "_Venus", "Mars", */
	"Ran", "Orions NOUN_CAMP", "ARTIKEL_BESTIMMTER NOUN_CAVE_OF_THE_WUMPUS", /* EN "Ran", "Orion's camp", "the cave of the wumpus", */
	PM_RANGER, NON_PM, PM_LITTLE_DOG /* Orion & canis major */,
	PM_ORION, PM_HUNTER, PM_SCORPIUS,
	PM_FOREST_CENTAUR, PM_SCORPION, S_CENTAUR, S_SPIDER,
	ART_LONGBOW_OF_DIANA,
	MH_HUMAN|MH_ELF|MH_GNOME|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  13, 13, 13,  9, 13,  7 },
	{  30, 10, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 13, 0,  0, 6,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -4
},
{	{"NOUN_SAMURAI", "NOUN_SAMURAIIN"}, { /* EN {	{"Samurai", 0}, { */
	{"NOUN_HATAMOTO", "NOUN_HATAMOTOIN"},  /* Banner Knight */ /* EN {"Hatamoto",    0}, */
	{"NOUN_RONIN", "NOUN_RONININ"},  /* no allegiance */ /* EN {"Ronin",       0}, */
	{"NOUN_NINJA", "NOUN_KUNOICHI"},  /* secret society */ /* EN {"Ninja",       "Kunoichi"}, */
	{"NOUN_JOSHU", "NOUN_JOSHUIN"},  /* heads a castle */ /* EN {"Joshu",       0}, */
	{"NOUN_RYOSHU", "NOUN_RYOSHUIN"},  /* has a territory */ /* EN {"Ryoshu",      0}, */
	{"NOUN_KOKUSHU", "NOUN_KOKUSHUIN"},  /* heads a province */ /* EN {"Kokushu",     0}, */
	{"NOUN_DAIMYO", "NOUN_DAIMYOIN"},  /* a samurai lord */ /* EN {"Daimyo",      0}, */
	{"NOUN_KUGE", "NOUN_KUGEIN"},  /* Noble of the Court */ /* EN {"Kuge",        0}, */
	{"NOUN_SHOGUN", "NOUN_SHOGUNIN"} },/* supreme commander, warlord */ /* EN {"Shogun",      0} }, */
	"_Amaterasu Omikami", "Raijin", "Susanowo", /* Japanese */
	"Sam", "ARTIKEL_BESTIMMTER NOUN_BURG des Taro-Clans", "ARTIKEL_BESTIMMTER NOUN_BURG des Shoguns", /* EN "Sam", "the Castle of the Taro Clan", "the Shogun's Castle", */
	PM_SAMURAI, NON_PM, PM_LITTLE_DOG,
	PM_LORD_SATO, PM_ROSHI, PM_ASHIKAGA_TAKAUJI,
	PM_WOLF, PM_STALKER, S_DOG, S_ELEMENTAL,
	ART_TSURUGI_OF_MURAMASA,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  10,  8,  7, 10, 17,  6 },
	{  30, 10,  8, 30, 14,  8 },
	/* Init   Lower  Higher */
	{ 13, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_CLAIRVOYANCE,    -4
},
#ifdef TOURIST
{	{"NOUN_TOURIST", "NOUN_TOURISTIN"}, { /* EN {	{"Tourist", 0}, { */
	{"NOUN_WANDERER", "NOUN_WANDERIN"     }, /* EN {"Rambler",     0}, */
	{"NOUN_SCHAULUSTIGE", "NOUN_SCHAULUSTIGIN"}, /* EN {"Sightseer",   0}, */
	{"NOUN_AUSFLUEGLER", "NOUN_AUSFLUEGLERIN"}, /* EN {"Excursionist",0}, */
	{"NOUN_REISENDE", "NOUN_REISENDEIN"}, /* EN {"Peregrinator","Peregrinatrix"}, */
	{"NOUN_GLOBETROTTER", "NOUN_GLOBETROTTERIN"}, /* EN {"Traveler",    0}, */
	{"NOUN_WELTENBUMMLER", "NOUN_WELTENBUMMLERIN"}, /* EN {"Journeyer",   0}, */
	{"NOUN_SEEFAHRER", "NOUN_SEEFAHRERIN"}, /* EN {"Voyager",     0}, */
	{"NOUN_ERFORSCHER", "NOUN_ERFORSCHERIN"}, /* EN {"Explorer",    0}, */
	{"NOUN_ABENTEURER", "NOUN_ABENTEURERIN"} }, /* EN {"Adventurer",  0} }, */
	"Blind Io", "_The Lady", "Offler", /* Discworld */ /* EN "Blind Io", "_The Lady", "Offler", */ // TODO DE
	"Tou", "NOUN_ANKH_MORPORK", "ARTIKEL_BESTIMMTER NOUN_HALLE der Diebesgilde", /* EN "Tou", "Ankh-Morpork", "the Thieves' Guild Hall", */
	PM_TOURIST, NON_PM, NON_PM,
	PM_TWOFLOWER, PM_GUIDE, PM_MASTER_OF_THIEVES,
	PM_GIANT_SPIDER, PM_FOREST_CENTAUR, S_SPIDER, S_CENTAUR,
	ART_YENDORIAN_EXPRESS_CARD,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  6,  7,  7, 10 },
	{  15, 10, 10, 15, 30, 20 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 8,  0, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },14,	/* Energy */
	0, 5, 1, 2, 10, A_INT, SPE_CHARM_MONSTER,   -4
},
#endif
{	{"NOUN_VALKYRIE", 0}, { /* EN {	{"Valkyrie", 0}, { */
	{"NOUN_GRUENSCHNABEL",   0}, /* EN {"Stripling",   0}, */
	{"NOUN_HAUDEGEN",  0}, /* EN {"Skirmisher",  0}, */
	{"NOUN_STREITER",     "NOUN_STREITERIN"}, /* EN {"Fighter",     0}, */
	{"NOUN_KAEMPFER", "NOUN_KAEMPFERIN"}, /* EN {"Man-at-arms", "Woman-at-arms"}, */
	{"NOUN_KRIEGER",     "NOUN_KRIEGERIN"}, /* EN {"Warrior",     0}, */
	{"NOUN_SAEBELRASSLER","NOUN_SAEBELRASSLERIN"}, /* EN {"Swashbuckler",0}, */
	{"NOUN_HELD",        "NOUN_HELDIN"}, /* EN {"Hero",        "Heroine"}, */
	{"NOUN_KRIEGERPRINZ",    "NOUN_KRIEGERPRINZESSIN"}, /* EN {"Champion",    0}, */
	{"NOUN_LORD",        "NOUN_LADY"} }, /* EN {"Lord",        "Lady"} }, */
	"Tyr", "Odin", "Loki", /* Norse */
	"Val", "ARTIKEL_BESTIMMTER NOUN_SHRINE_OF_DESTINY", "ARTIKEL_BESTIMMTER NOUN_CAVE von Surtur", /* EN "Val", "the Shrine of Destiny", "the cave of Surtur", */ /* netzhack: Surtshellir */
	PM_VALKYRIE, NON_PM, NON_PM /*PM_WINTER_WOLF_CUB*/,
	PM_NORN, PM_WARRIOR, PM_LORD_SURTUR,
	PM_FIRE_ANT, PM_FIRE_GIANT, S_ANT, S_GIANT,
	ART_ORB_OF_FATE,
	MH_HUMAN|MH_DWARF | ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7, 10,  7 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  2, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 10,-2, 0,  9, A_WIS, SPE_CONE_OF_COLD,    -4
},
{	{"NOUN_WIZARD", 0}, { /* EN {	{"Wizard", 0}, { */
	{"NOUN_BESCHWOERER", "NOUN_BESCHWOERERIN"}, /* EN {"Evoker",      0}, */
	{"NOUN_GEISTERBANNER", "NOUN_GEISTERBANNERIN"}, /* EN {"Conjurer",    0}, */
	{"NOUN_THAUMATURG", "NOUN_THAUMATURGIN"}, /* EN {"Thaumaturge", 0}, */
	{"NOUN_ILLUSIONIST", "NOUN_ILLUSIONISTIN"}, /* EN {"Magician",    0}, */
	{"NOUN_HEXER", "NOUN_HEXER"}, /* EN {"Enchanter",   "Enchantress"}, */
	{"NOUN_HEXENMEISTER", "NOUN_HEXENMEISTERIN"}, /* EN {"Sorcerer",    "Sorceress"}, */
	{"NOUN_NEKROMANT", "NOUN_NEKROMANTIN"}, /* EN {"Necromancer", 0}, */
	{"NOUN_ZAUBERER", "NOUN_ZAUBERERIN"}, /* EN {"Wizard",      0}, */
	{"NOUN_MAGIER", "NOUN_MAGIERIN"} }, /* EN {"Mage",        0} }, */
	"Ptah", "Thoth", "Anhur", /* Egyptian */
	"Wiz", "ARTIKEL_BESTIMMTER NOUN_LONELY_TOWER", "ARTIKEL_BESTIMMTER NOUN_TOWER_OF_DARKNESS", /* EN "Wiz", "the Lonely Tower", "the Tower of Darkness", */
	PM_WIZARD, NON_PM, PM_KITTEN,
	PM_NEFERET_THE_GREEN, PM_APPRENTICE, PM_DARK_ONE,
	PM_VAMPIRE_BAT, PM_XORN, S_BAT, S_WRAITH,
	ART_EYE_OF_THE_AETHIOPICA,
	MH_HUMAN|MH_ELF|MH_GNOME|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  7,  7,  7,  7 },
	{  10, 30, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  4, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 3, 10, A_INT, SPE_MAGIC_MISSILE,   -4
},
/* Array terminator */
{{0, 0}}
};


/* The player's role, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Role urole =
{	{"Undefined", 0}, { {0, 0}, {0, 0}, {0, 0},
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	"L", "N", "C", "Xxx", "home", "locate",
	NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM,
	NON_PM, NON_PM, 0, 0, 0, 0,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7,  7,  7,  7 },
	{  20, 15, 15, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 3 },14,	/* Energy */
	0, 10, 0, 0,  4, A_INT, 0, -3
};



/* Table of all races */
const struct Race races[] = {
{	"NOUN_HUMAN", "ADJEKTIV_MENSCHLICH", "NOUN_HUMANITY", "Hum", /* EN {	"human", "human", "humanity", "Hum", */
	{"NOUN_MAN", "NOUN_WOMAN"},
	PM_HUMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"NOUN_ELF", "ADJEKTIV_ELBISCH", "NOUN_ELVENKIND", "Elb", /* EN {	"elf", "elven", "elvenkind", "Elf", */
	{0, 0},
	PM_ELF, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MH_ELF, MH_ELF, MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{   18,    20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 }		/* Energy */
},
// TODO DE
{	"NOUN_DWARF", "ADJEKTIV_ZWERGISCH", "NOUN_DWARVENKIND", "Zwe", /* EN {	"dwarf", "dwarven", "dwarvenkind", "Dwa", */
	{0, 0},
	PM_DWARF, NON_PM, PM_DWARF_MUMMY, PM_DWARF_ZOMBIE,
	MH_DWARF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	MH_DWARF, MH_DWARF|MH_GNOME, MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 16, 16, 20, 20, 16 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  2, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"NOUN_GNOME", "ADJEKTIV_GNOMISCH", "NOUN_GNOMEHOOD", "Gno", /* EN {	"gnome", "gnomish", "gnomehood", "Gno", */
	{0, 0},
	PM_GNOME, NON_PM, PM_GNOME_MUMMY, PM_GNOME_ZOMBIE,
	MH_GNOME | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	MH_GNOME, MH_DWARF|MH_GNOME, MH_HUMAN,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{STR18(50),19, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"NOUN_ORC", "ADJEKTIV_ORKSCH", "NOUN_ORCDOM", "Ork", /* EN {	"orc", "orcish", "orcdom", "Orc", */
	{0, 0},
	PM_ORC, NON_PM, PM_ORC_MUMMY, PM_ORC_ZOMBIE,
	MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MH_ORC, 0, MH_HUMAN|MH_ELF|MH_DWARF,
	/*  Str    Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{STR18(50),16, 16, 18, 18, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
/* Array terminator */
{ 0, 0, 0, 0 }};


/* The player's race, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Race urace =
{	"something", "undefined", "something", "Xxx",
	{0, 0},
	NON_PM, NON_PM, NON_PM, NON_PM,
	0, 0, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
};


/* Table of all genders */
const struct Gender genders[] = {
#ifndef GERMAN
	{"male",	"he",	"him",	"his",	"Mal",	ROLE_MALE},
	{"female",	"she",	"her",	"her",	"Fem",	ROLE_FEMALE},
	{"neuter",	"it",	"it",	"its",	"Ntr",	ROLE_NEUTER}
#else
	{"male", "ADJEKTIV_MAENNLICH", "NOUN_PRONOMEN_3P_M_PERSONAL",
						"NOUN_PRONOMEN_3P_M_PERSONAL",
						"PRONOMEN_3P_MN_POSSESSIV",
						"Mal",	ROLE_MALE},
	{"female", "ADJEKTIV_WEIBLICH", "NOUN_PRONOMEN_3P_F_PERSONAL",
							"NOUN_PRONOMEN_3P_F_PERSONAL",
							"PRONOMEN_3P_F_POSSESSIV",
							"Fem",	ROLE_FEMALE},
	{"neuter", "ADJEKTIV_SAECHLICH", "NOUN_PRONOMEN_3P_N_PERSONAL",
							"NOUN_PRONOMEN_3P_N_PERSONAL",
							"PRONOMEN_3P_MN_POSSESSIV",
							"Ntr",	ROLE_NEUTER}
#endif
};

/* Table of all alignments */
const struct Align aligns[] = {
	{"law",		"lawful",	"Law",	ROLE_LAWFUL,	A_LAWFUL}, /* EN {"law",		"lawful",	"Law",	ROLE_LAWFUL,	A_LAWFUL}, */ // TODO DE
	{"balance",	"neutral",	"Neu",	ROLE_NEUTRAL,	A_NEUTRAL}, /* EN {"balance",	"neutral",	"Neu",	ROLE_NEUTRAL,	A_NEUTRAL}, */ // TODO DE
	{"chaos",	"chaotic",	"Cha",	ROLE_CHAOTIC,	A_CHAOTIC}, /* EN {"chaos",	"chaotic",	"Cha",	ROLE_CHAOTIC,	A_CHAOTIC}, */ // TODO DE
	{"evil",	"unaligned",	"Una",	0,		A_NONE} /* EN {"evil",	"unaligned",	"Una",	0,		A_NONE} */ // TODO DE
};

STATIC_DCL char * FDECL(promptsep, (char *, int));
STATIC_DCL int FDECL(role_gendercount, (int));
STATIC_DCL int FDECL(race_alignmentcount, (int));

/* used by str2XXX() */
static char NEARDATA randomstr[] = "random";


boolean
validrole(rolenum)
	int rolenum;
{
	return (rolenum >= 0 && rolenum < SIZE(roles)-1);
}


int
randrole()
{
	return (rn2(SIZE(roles)-1));
}


int
str2role(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; roles[i].name.m; i++) {
	    /* Does it match the male name? */
	    if (!strncmpi(str, roles[i].name.m, len))
		return i;
	    /* Or the female name? */
	    if (roles[i].name.f && !strncmpi(str, roles[i].name.f, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, roles[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validrace(rolenum, racenum)
	int rolenum, racenum;
{
	/* Assumes validrole */
	return (racenum >= 0 && racenum < SIZE(races)-1 &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_RACEMASK));
}


int
randrace(rolenum)
	int rolenum;
{
	int i, n = 0;

	/* Count the number of valid races */
	for (i = 0; races[i].noun; i++)
	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK)
	    	n++;

	/* Pick a random race */
	/* Use a factor of 100 in case of bad random number generators */
	if (n) n = rn2(n*100)/100;
	for (i = 0; races[i].noun; i++)
	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role has no permitted races? */
	return (rn2(SIZE(races)-1));
}


int
str2race(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; races[i].noun; i++) {
	    /* Does it match the noun? */
	    if (!strncmpi(str, races[i].noun, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, races[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validgend(rolenum, racenum, gendnum)
	int rolenum, racenum, gendnum;
{
	/* Assumes validrole and validrace */
	return (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		(roles[rolenum].allow & races[racenum].allow &
		 genders[gendnum].allow & ROLE_GENDMASK));
}


int
randgend(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid genders */
	for (i = 0; i < ROLE_GENDERS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK)
	    	n++;

	/* Pick a random gender */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_GENDERS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role/race has no permitted genders? */
	return (rn2(ROLE_GENDERS));
}


int
str2gend(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_GENDERS; i++) {
	    /* Does it match the adjective? */
#ifdef GERMAN
	    if (!strncmpi(str, genders[i].adj_en, len))
		return i;
#endif
	    if (!strncmpi(str, genders[i].adj, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, genders[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validalign(rolenum, racenum, alignnum)
	int rolenum, racenum, alignnum;
{
	/* Assumes validrole and validrace */
	return (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		(roles[rolenum].allow & races[racenum].allow &
		 aligns[alignnum].allow & ROLE_ALIGNMASK));
}


int
randalign(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid alignments */
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK)
	    	n++;

	/* Pick a random alignment */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role/race has no permitted alignments? */
	return (rn2(ROLE_ALIGNS));
}


int
str2align(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    /* Does it match the adjective? */
	    if (!strncmpi(str, aligns[i].adj, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, aligns[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}

/* is rolenum compatible with any racenum/gendnum/alignnum constraints? */
boolean
ok_role(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (rolenum >= 0 && rolenum < SIZE(roles)-1) {
	allow = roles[rolenum].allow;
	if (racenum >= 0 && racenum < SIZE(races)-1 &&
		!(allow & races[racenum].allow & ROLE_RACEMASK))
	    return FALSE;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(roles)-1; i++) {
	    allow = roles[i].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1 &&
		    !(allow & races[racenum].allow & ROLE_RACEMASK))
		continue;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random role subject to any racenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a role is returned only if there is  */
/* a single possibility */
int
pick_role(racenum, gendnum, alignnum, pickhow)
int racenum, gendnum, alignnum, pickhow;
{
    int i;
    int roles_ok = 0;

    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum))
	    roles_ok++;
    }
    if (roles_ok == 0 || (roles_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    roles_ok = rn2(roles_ok);
    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum)) {
	    if (roles_ok == 0)
		return i;
	    else
		roles_ok--;
	}
    }
    return ROLE_NONE;
}

/* is racenum compatible with any rolenum/gendnum/alignnum constraints? */
boolean
ok_race(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (racenum >= 0 && racenum < SIZE(races)-1) {
	allow = races[racenum].allow;
	if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		!(allow & roles[rolenum].allow & ROLE_RACEMASK))
	    return FALSE;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(races)-1; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		    !(allow & roles[rolenum].allow & ROLE_RACEMASK))
		continue;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random race subject to any rolenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a race is returned only if there is  */
/* a single possibility */
int
pick_race(rolenum, gendnum, alignnum, pickhow)
int rolenum, gendnum, alignnum, pickhow;
{
    int i;
    int races_ok = 0;

    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum))
	    races_ok++;
    }
    if (races_ok == 0 || (races_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    races_ok = rn2(races_ok);
    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum)) {
	    if (races_ok == 0)
		return i;
	    else
		races_ok--;
	}
    }
    return ROLE_NONE;
}

/* is gendnum compatible with any rolenum/racenum/alignnum constraints? */
/* gender and alignment are not comparable (and also not constrainable) */
boolean
ok_gend(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (gendnum >= 0 && gendnum < ROLE_GENDERS) {
	allow = genders[gendnum].allow;
	if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		!(allow & roles[rolenum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (racenum >= 0 && racenum < SIZE(races)-1 &&
		!(allow & races[racenum].allow & ROLE_GENDMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_GENDERS; i++) {
	    allow = genders[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		    !(allow & roles[rolenum].allow & ROLE_GENDMASK))
		continue;
	    if (racenum >= 0 && racenum < SIZE(races)-1 &&
		    !(allow & races[racenum].allow & ROLE_GENDMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random gender subject to any rolenum/racenum/alignnum constraints */
/* gender and alignment are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID a gender is returned only if there is  */
/* a single possibility */
int
pick_gend(rolenum, racenum, alignnum, pickhow)
int rolenum, racenum, alignnum, pickhow;
{
    int i;
    int gends_ok = 0;

    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum))
	    gends_ok++;
    }
    if (gends_ok == 0 || (gends_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    gends_ok = rn2(gends_ok);
    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum)) {
	    if (gends_ok == 0)
		return i;
	    else
		gends_ok--;
	}
    }
    return ROLE_NONE;
}

/* is alignnum compatible with any rolenum/racenum/gendnum constraints? */
/* alignment and gender are not comparable (and also not constrainable) */
boolean
ok_align(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (alignnum >= 0 && alignnum < ROLE_ALIGNS) {
	allow = aligns[alignnum].allow;
	if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		!(allow & roles[rolenum].allow & ROLE_ALIGNMASK))
	    return FALSE;
	if (racenum >= 0 && racenum < SIZE(races)-1 &&
		!(allow & races[racenum].allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		    !(allow & roles[rolenum].allow & ROLE_ALIGNMASK))
		continue;
	    if (racenum >= 0 && racenum < SIZE(races)-1 &&
		    !(allow & races[racenum].allow & ROLE_ALIGNMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random alignment subject to any rolenum/racenum/gendnum constraints */
/* alignment and gender are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID an alignment is returned only if there is  */
/* a single possibility */
int
pick_align(rolenum, racenum, gendnum, pickhow)
int rolenum, racenum, gendnum, pickhow;
{
    int i;
    int aligns_ok = 0;

    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i))
	    aligns_ok++;
    }
    if (aligns_ok == 0 || (aligns_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    aligns_ok = rn2(aligns_ok);
    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i)) {
	    if (aligns_ok == 0)
		return i;
	    else
		aligns_ok--;
	}
    }
    return ROLE_NONE;
}

void
rigid_role_checks()
{
    /* Some roles are limited to a single race, alignment, or gender and
     * calling this routine prior to XXX_player_selection() will help
     * prevent an extraneous prompt that actually doesn't allow
     * you to choose anything further. Note the use of PICK_RIGID which
     * causes the pick_XX() routine to return a value only if there is one
     * single possible selection, otherwise it returns ROLE_NONE.
     *
     */
    if (flags.initrole == ROLE_RANDOM) {
	/* If the role was explicitly specified as ROLE_RANDOM
	 * via -uXXXX-@ then choose the role in here to narrow down
	 * later choices. Pick a random role in this case.
	 */
	flags.initrole = pick_role(flags.initrace, flags.initgend,
					flags.initalign, PICK_RANDOM);
	if (flags.initrole < 0)
	    flags.initrole = randrole();
    }
    if (flags.initrole != ROLE_NONE) {
	if (flags.initrace == ROLE_NONE)
	     flags.initrace = pick_race(flags.initrole, flags.initgend,
						flags.initalign, PICK_RIGID);
	if (flags.initalign == ROLE_NONE)
	     flags.initalign = pick_align(flags.initrole, flags.initrace,
						flags.initgend, PICK_RIGID);
	if (flags.initgend == ROLE_NONE)
	     flags.initgend = pick_gend(flags.initrole, flags.initrace,
						flags.initalign, PICK_RIGID);
    }
}

#define BP_ALIGN	0
#define BP_GEND		1
#define BP_RACE		2
#define BP_ROLE		3
#define NUM_BP		4

STATIC_VAR char pa[NUM_BP], post_attribs;

STATIC_OVL char *
promptsep(buf, num_post_attribs)
char *buf;
int num_post_attribs;
{
	const char *conj = "und "; /* EN const char *conj = "and "; */
	if (num_post_attribs > 1
	    && post_attribs < num_post_attribs && post_attribs > 1)
	 	Strcat(buf, ","); 
	Strcat(buf, " ");
	--post_attribs;
	if (!post_attribs && num_post_attribs > 1) Strcat(buf, conj);
	return buf;
}

STATIC_OVL int
role_gendercount(rolenum)
int rolenum;
{
	int gendcount = 0;
	if (validrole(rolenum)) {
		if (roles[rolenum].allow & ROLE_MALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_FEMALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_NEUTER) ++gendcount;
	}
	return gendcount;
}

STATIC_OVL int
race_alignmentcount(racenum)
int racenum;
{
	int aligncount = 0;
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (races[racenum].allow & ROLE_CHAOTIC) ++aligncount;
		if (races[racenum].allow & ROLE_LAWFUL) ++aligncount;
		if (races[racenum].allow & ROLE_NEUTRAL) ++aligncount;
	}
	return aligncount;
}

char *
root_plselection_prompt(suppliedbuf, buflen, rolenum, racenum, gendnum, alignnum)
char *suppliedbuf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
	int k, gendercount = 0, aligncount = 0;
	char buf[BUFSZ];
	static char err_ret[] = " character's"; /* EN static char err_ret[] = " character's"; */ // TODO DE
	boolean donefirst = FALSE;

	if (!suppliedbuf || buflen < 1) return err_ret;

	/* initialize these static variables each time this is called */
	post_attribs = 0;
	for (k=0; k < NUM_BP; ++k)
		pa[k] = 0;
	buf[0] = '\0';
	*suppliedbuf = '\0';
	
	/* How many alignments are allowed for the desired race? */
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM)
		aligncount = race_alignmentcount(racenum);

	if (alignnum != ROLE_NONE && alignnum != ROLE_RANDOM) {
		/* if race specified, and multiple choice of alignments for it */
		if ((racenum >= 0) && (aligncount > 1)) {
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, aligns[alignnum].adj);
			donefirst = TRUE;
		} else {
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, aligns[alignnum].adj);
			donefirst = TRUE;
		}
	} else {
		/* if alignment not specified, but race is specified
			and only one choice of alignment for that race then
			don't include it in the later list */
		if ((((racenum != ROLE_NONE && racenum != ROLE_RANDOM) &&
			ok_race(rolenum, racenum, gendnum, alignnum))
		      && (aligncount > 1))
		     || (racenum == ROLE_NONE || racenum == ROLE_RANDOM)) {
			pa[BP_ALIGN] = 1;
			post_attribs++;
		}
	}
	/* <your lawful> */

	/* How many genders are allowed for the desired role? */
	if (validrole(rolenum))
		gendercount = role_gendercount(rolenum);

	if (gendnum != ROLE_NONE  && gendnum != ROLE_RANDOM) {
		if (validrole(rolenum)) {
		     /* if role specified, and multiple choice of genders for it,
			and name of role itself does not distinguish gender */
			if ((rolenum != ROLE_NONE) && (gendercount > 1)
						&& !roles[rolenum].name.f) {
				if (donefirst) Strcat(buf, " ");
				Strcat(buf, genders[gendnum].adj);
				donefirst = TRUE;
			}
	        } else {
			if (donefirst) Strcat(buf, " ");
	        	Strcat(buf, genders[gendnum].adj);
			donefirst = TRUE;
	        }
	} else {
		/* if gender not specified, but role is specified
			and only one choice of gender then
			don't include it in the later list */
		if ((validrole(rolenum) && (gendercount > 1)) || !validrole(rolenum)) {
			pa[BP_GEND] = 1;
			post_attribs++;
		}
	}
	/* <your lawful female> */

	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (validrole(rolenum) && ok_race(rolenum, racenum, gendnum, alignnum)) {
			if (donefirst) Strcat(buf, " "); 
			Strcat(buf, (rolenum == ROLE_NONE) ?
				races[racenum].noun :
				races[racenum].adj);
			donefirst = TRUE;
		} else if (!validrole(rolenum)) {
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, races[racenum].noun);
			donefirst = TRUE;
		} else {
			pa[BP_RACE] = 1;
			post_attribs++;
		}
	} else {
		pa[BP_RACE] = 1;
		post_attribs++;
	}
	/* <your lawful female gnomish> || <your lawful female gnome> */

	if (validrole(rolenum)) {
		if (donefirst) Strcat(buf, " ");
		if (gendnum != ROLE_NONE) {
		    if (gendnum == 1  && roles[rolenum].name.f)
			Strcat(buf, roles[rolenum].name.f);
		    else
  			Strcat(buf, roles[rolenum].name.m);
		} else {
			if (roles[rolenum].name.f) {
				Strcat(buf, roles[rolenum].name.m);
#ifndef GERMAN
				Strcat(buf, "-/-");
				Strcat(buf, roles[rolenum].name.f);
#endif
			} else 
				Strcat(buf, roles[rolenum].name.m);
		}
		donefirst = TRUE;
	} else if (rolenum == ROLE_NONE) {
		pa[BP_ROLE] = 1;
		post_attribs++;
	}
	
	if ((racenum == ROLE_NONE || racenum == ROLE_RANDOM) && !validrole(rolenum)) {
		if (donefirst) Strcat(buf, " ");
		Strcat(buf, "NOUN_CHARACTER"); /* EN Strcat(buf, "character"); */
		donefirst = TRUE;
	}
	/* <your lawful female gnomish cavewoman> || <your lawful female gnome>
	 *    || <your lawful female character>
	 */
	if (buflen > (int) (strlen(buf) + 1)) {
		Strcpy(suppliedbuf, buf);
		return suppliedbuf;
	} else
		return err_ret;
}

char *
build_plselection_prompt(buf, buflen, rolenum, racenum, gendnum, alignnum)
char *buf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
	const char *defprompt = "Soll ich einen Charakter für dich erstellen? [ynq] "; /* EN const char *defprompt = "Shall I pick a character for you? [ynq] "; */
	int num_post_attribs = 0;
	char tmpbuf[BUFSZ];
	
	if (buflen < QBUFSZ)
		return (char *)defprompt;

	Strcpy(buf, "Soll ich"); /* EN Strcpy(tmpbuf, "Shall I pick "); */
#ifndef GERMAN
	if (racenum != ROLE_NONE || validrole(rolenum))
		Strcat(tmpbuf, "your ");
	else {
		Strcat(tmpbuf, "a ");
	}
	/* <your> */
#endif

#ifdef GERMAN
	Strcpy(tmpbuf, ""); /* Workaround um post_attribs zu setzen*/
#endif
	(void)  root_plselection_prompt(eos(tmpbuf), buflen - strlen(tmpbuf),
					rolenum, racenum, gendnum, alignnum);
#ifndef GERMAN
	Sprintf(buf, "%s", s_suffix(tmpbuf));
#endif

	/* buf should now be:
	 * < your lawful female gnomish cavewoman's> || <your lawful female gnome's>
	 *    || <your lawful female character's>
	 *
         * Now append the post attributes to it
	 */

	num_post_attribs = post_attribs;
	if (post_attribs) {
		if (pa[BP_RACE]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, german("NOUN_RACE")); /* EN Strcat(buf, "race"); */
		}
		if (pa[BP_ROLE]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, german("NOUN_ROLE")); /* EN Strcat(buf, "role"); */
		}
		if (pa[BP_GEND]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, german("NOUN_GENDER")); /* EN Strcat(buf, "gender"); */
		}
		if (pa[BP_ALIGN]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, german("NOUN_ALIGNMENT")); /* EN Strcat(buf, "alignment"); */
		}
	}

#ifdef GERMAN
	Strcpy(tmpbuf, " KASUS_GENITIV PRONOMEN_POSSESSIV ");

	(void)  root_plselection_prompt(eos(tmpbuf), buflen - strlen(tmpbuf),
					rolenum, racenum, gendnum, alignnum);
	Strcat(buf, german(tmpbuf));
#endif

	Strcat(buf, " für dich auswählen? [ynq] "); /* EN Strcat(buf, " for you? [ynq] "); */
	return buf;
}

#undef BP_ALIGN
#undef BP_GEND
#undef BP_RACE
#undef BP_ROLE
#undef NUM_BP

void
plnamesuffix()
{
	char *sptr, *eptr;
	int i;

	/* Look for tokens delimited by '-' */
	if ((eptr = index(plname, '-')) != (char *) 0)
	    *eptr++ = '\0';
	while (eptr) {
	    /* Isolate the next token */
	    sptr = eptr;
	    if ((eptr = index(sptr, '-')) != (char *)0)
		*eptr++ = '\0';

	    /* Try to match it to something */
	    if ((i = str2role(sptr)) != ROLE_NONE)
		flags.initrole = i;
	    else if ((i = str2race(sptr)) != ROLE_NONE)
		flags.initrace = i;
	    else if ((i = str2gend(sptr)) != ROLE_NONE)
		flags.initgend = i;
	    else if ((i = str2align(sptr)) != ROLE_NONE)
		flags.initalign = i;
	}
	if(!plname[0]) {
	    askname();
	    plnamesuffix();
	}

	/* commas in the plname confuse the record file, convert to spaces */
	for (sptr = plname; *sptr; sptr++) {
		if (*sptr == ',') *sptr = ' ';
	}
#ifdef GERMAN
	plname[0] = toupper(plname[0]);
#endif
}


/*
 *	Special setup modifications here:
 *
 *	Unfortunately, this is going to have to be done
 *	on each newgame or restore, because you lose the permonst mods
 *	across a save/restore.  :-)
 *
 *	1 - The Rogue Leader is the Tourist Nemesis.
 *	2 - Priests start with a random alignment - convert the leader and
 *	    guardians here.
 *	3 - Elves can have one of two different leaders, but can't work it
 *	    out here because it requires hacking the level file data (see
 *	    sp_lev.c).
 *
 * This code also replaces quest_init().
 */
void
role_init()
{
	int alignmnt;

	/* Strip the role letter out of the player name.
	 * This is included for backwards compatibility.
	 */
	plnamesuffix();

	/* Check for a valid role.  Try flags.initrole first. */
	if (!validrole(flags.initrole)) {
	    /* Try the player letter second */
	    if ((flags.initrole = str2role(pl_character)) < 0)
	    	/* None specified; pick a random role */
	    	flags.initrole = randrole();
	}

	/* We now have a valid role index.  Copy the role name back. */
	/* This should become OBSOLETE */
	Strcpy(pl_character, roles[flags.initrole].name.m);
	pl_character[PL_CSIZ-1] = '\0';

	/* Check for a valid race */
	if (!validrace(flags.initrole, flags.initrace))
	    flags.initrace = randrace(flags.initrole);

	/* Check for a valid gender.  If new game, check both initgend
	 * and female.  On restore, assume flags.female is correct. */
	if (flags.pantheon == -1) {	/* new game */
	    if (!validgend(flags.initrole, flags.initrace, flags.female))
		flags.female = !flags.female;
	}
	if (!validgend(flags.initrole, flags.initrace, flags.initgend))
	    /* Note that there is no way to check for an unspecified gender. */
	    flags.initgend = flags.female;

	/* Check for a valid alignment */
	if (!validalign(flags.initrole, flags.initrace, flags.initalign))
	    /* Pick a random alignment */
	    flags.initalign = randalign(flags.initrole, flags.initrace);
	alignmnt = aligns[flags.initalign].value;

	/* Initialize urole and urace */
	urole = roles[flags.initrole];
	urace = races[flags.initrace];

	/* Fix up the quest leader */
	if (urole.ldrnum != NON_PM) {
	    mons[urole.ldrnum].msound = MS_LEADER;
	    mons[urole.ldrnum].mflags2 |= (M2_PEACEFUL);
	    mons[urole.ldrnum].mflags3 |= M3_CLOSE;
	    mons[urole.ldrnum].maligntyp = alignmnt * 3;
	}

	/* Fix up the quest guardians */
	if (urole.guardnum != NON_PM) {
	    mons[urole.guardnum].mflags2 |= (M2_PEACEFUL);
	    mons[urole.guardnum].maligntyp = alignmnt * 3;
	}

	/* Fix up the quest nemesis */
	if (urole.neminum != NON_PM) {
	    mons[urole.neminum].msound = MS_NEMESIS;
	    mons[urole.neminum].mflags2 &= ~(M2_PEACEFUL);
	    mons[urole.neminum].mflags2 |= (M2_NASTY|M2_STALK|M2_HOSTILE);
	    mons[urole.neminum].mflags3 |= M3_WANTSARTI | M3_WAITFORU;
	}

	/* Fix up the god names */
	if (flags.pantheon == -1) {		/* new game */
	    flags.pantheon = flags.initrole;	/* use own gods */
	    while (!roles[flags.pantheon].lgod)	/* unless they're missing */
		flags.pantheon = randrole();
	}
	if (!urole.lgod) {
	    urole.lgod = roles[flags.pantheon].lgod;
	    urole.ngod = roles[flags.pantheon].ngod;
	    urole.cgod = roles[flags.pantheon].cgod;
	}

	/* Fix up infravision */
	if (mons[urace.malenum].mflags3 & M3_INFRAVISION) {
	    /* although an infravision intrinsic is possible, infravision
	     * is purely a property of the physical race.  This means that we
	     * must put the infravision flag in the player's current race
	     * (either that or have separate permonst entries for
	     * elven/non-elven members of each class).  The side effect is that
	     * all NPCs of that class will have (probably bogus) infravision,
	     * but since infravision has no effect for NPCs anyway we can
	     * ignore this.
	     */
	    mons[urole.malenum].mflags3 |= M3_INFRAVISION;
	    if (urole.femalenum != NON_PM)
	    	mons[urole.femalenum].mflags3 |= M3_INFRAVISION;
	}

	/* Artifacts are fixed in hack_artifacts() */

	/* Success! */
	return;
}

const char *
Hello(mtmp)
struct monst *mtmp;
{
	switch (Role_switch) {
	case PM_KNIGHT:
	    return ("Seid gegrüßt"); /* Olde English */ /* EN return ("Salutations"); */
	case PM_SAMURAI:
	    return (mtmp && mtmp->data == &mons[PM_SHOPKEEPER] ?
	    		"Irasshaimase" : "Konnichi wa"); /* Japanese */
#ifdef TOURIST
	case PM_TOURIST:
	    return ("Aloha");       /* Hawaiian */
#endif
	case PM_VALKYRIE:
	    return (
#ifdef MAIL
	    		mtmp && mtmp->data == &mons[PM_MAIL_DAEMON] ? "Hallo" :
#endif
	    		"Velkommen");   /* Norse */
	default:
	    return ("Hallo"); /* EN return ("Hello"); */
	}
}

const char *
Goodbye()
{
	switch (Role_switch) {
	case PM_KNIGHT:
	    return ("Gehabt Euch wohl");  /* Olde English */ /* EN return ("Fare thee well"); */
	case PM_SAMURAI:
	    return ("Sayonara");        /* Japanese */
#ifdef TOURIST
	case PM_TOURIST:
	    return ("Aloha");           /* Hawaiian */
#endif
	case PM_VALKYRIE:
	    return ("Farvel");          /* Norse */
	default:
	    return ("Auf Wiedersehen"); /* EN return ("Goodbye"); */
	}
}

/* role.c */
