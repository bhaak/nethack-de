
enum Casus   { nominativ=1, genitiv=2, dativ=4, akkusativ=8 };
enum Genus   { maskulin=1, feminin=2, neutrum=4 };
enum Numerus { n_singular=1, n_plural=2 };
enum Person  { erstePerson=1, zweitePerson=2, drittePerson=4 };
enum Artikel { ohne=1, bestimmter=2, unbestimmter=4 };

struct substantiv {
	const char  *wort;
	const char  *typ;
	enum Casus   casus;
	enum Genus   genus;
	enum Numerus numerus;
};

struct substantiv worte[] = {
	{"der", "ARTIKEL_BESTIMMTER", nominativ, maskulin, n_singular},
	{"des", "ARTIKEL_BESTIMMTER", genitiv,   maskulin, n_singular},
	{"dem", "ARTIKEL_BESTIMMTER", dativ,     maskulin, n_singular},
	{"den", "ARTIKEL_BESTIMMTER", akkusativ, maskulin, n_singular},

	{"die", "ARTIKEL_BESTIMMTER", nominativ|akkusativ, feminin,  n_singular},
	{"der", "ARTIKEL_BESTIMMTER", genitiv|dativ,       feminin,  n_singular},

	{"das", "ARTIKEL_BESTIMMTER", nominativ|akkusativ, neutrum,  n_singular},
	{"des", "ARTIKEL_BESTIMMTER", genitiv,   neutrum,  n_singular},
	{"dem", "ARTIKEL_BESTIMMTER", dativ,     neutrum,  n_singular},

	{"Hund",     "NOUN_DOG",   nominativ|akkusativ, maskulin,  n_singular},
	{"Hundes",   "NOUN_DOG",   genitiv,             maskulin,  n_singular},
	{"Hunde",    "NOUN_DOG",   dativ,               maskulin,  n_singular},

	{"es",    "NOUN_IT",   nominativ|akkusativ, neutrum,  n_singular},

	{"Jackal",      "NOUN_JACKAL",   nominativ|dativ|akkusativ, maskulin,  n_singular},
	{"Jackal",      "NOUN_JACKAL",   nominativ|akkusativ, maskulin,  n_singular},
	{"Riesenkäfer", "NOUN_GIANT_BEETLE",   nominativ|dativ|akkusativ, maskulin,  n_singular},
	{"Riesenkäfers", "NOUN_GIANT_BEETLE",   genitiv, maskulin,  n_singular},

	{"Augenbinde",  "NOUN_BLINDFOLD",   nominativ|genitiv|dativ|akkusativ, feminin,  n_singular},

	{"Troll",     "NOUN_TROLL",   nominativ|dativ|akkusativ, maskulin,  n_singular},
	{"Trolls",    "NOUN_TROLL",   genitiv, maskulin,  n_singular},

	{"Flechte",    "NOUN_LICHEN",    nominativ|genitiv|dativ|akkusativ, feminin,  n_singular},
	{"Flechten",   "NOUN_LICHENs",   nominativ|genitiv|dativ|akkusativ, feminin,  n_plural},
	{"Türe",       "NOUN_OBJ_DOOR",  nominativ|genitiv|dativ|akkusativ, feminin,  n_singular},
	{"Kanalratte", "NOUN_SEWER_RAT", nominativ|genitiv|dativ|akkusativ, feminin,  n_singular},

	/* change me to sterbliche Überreste */
	//{"die sterblichen Überreste",    "NOUN_CORPSE",    nominativ|akkusativ, feminin,  n_singular},
	{"die Leiche",    "NOUN_CORPSE",    nominativ|akkusativ, feminin,  n_singular},
	{"der Leiche",    "NOUN_CORPSE",    genitiv|dativ, feminin,  n_singular},
	{"die Leiche von",    "NOUN_CORPSE",    nominativ|akkusativ, feminin,  n_plural},
	{"der Leiche von",    "NOUN_CORPSE",    genitiv|dativ, feminin,  n_plural},

	{"Trank",   "NOUN_POTION", nominativ|akkusativ, maskulin,  n_singular},
	{"Trank",   "NOUN_POTION", dativ, maskulin,  n_singular}, /* moderner */
	{"Tranks",  "NOUN_POTION", genitiv, maskulin,  n_singular}, /* moderner */
	{"Tranke",  "NOUN_POTION", dativ, maskulin,  n_singular}, /* altertümlicher */
	{"Tränke",  "NOUN_POTIONs", nominativ|genitiv|akkusativ, maskulin,  n_plural},
	{"Tränken", "NOUN_POTIONs", dativ, maskulin,  n_plural},

	{"Zauberbuch", "NOUN_SPELLBOOK", nominativ, maskulin, n_singular},
	{"Zauberbuch",   "NOUN_SPELLBOOK", nominativ|akkusativ, neutrum,  n_singular},
	{"Schriftrolle", "NOUN_SCROLL", nominativ|akkusativ, feminin,  n_singular},
	{"Amulet", "NOUN_AMULET", nominativ|akkusativ, neutrum,  n_singular},
	{"Zauberstab", "NOUN_WAND", nominativ|akkusativ, maskulin,  n_singular},
	{"Zauberstabes", "NOUN_WAND", genitiv, maskulin,  n_singular},
	{"Zauberstabe", "NOUN_WAND", dativ, maskulin,  n_singular},

	{"Tod", "NOUN_WAND_DEATH", nominativ|akkusativ, maskulin,  n_singular},
	{"Todes", "NOUN_WAND_DEATH", genitiv, maskulin,  n_singular},
	{"Tode", "NOUN_WAND_DEATH", dativ, maskulin,  n_singular},

	{"Blindheit", "NOUN_AMULET", nominativ|akkusativ, neutrum,  n_singular},


	{"Sack",   "NOUN_SACK", nominativ|akkusativ, maskulin,  n_singular},
	{"Sacke",  "NOUN_SACK", dativ, maskulin,  n_singular},
	{"Sackes", "NOUN_SACK", genitiv, maskulin,  n_singular},

	{"Schwarzdrache", "NOUN_BLACK_DRAGON",   nominativ, maskulin,  n_singular},/* fix me: der blaue drache vs ein blauer drache oder Blaudrache?*/
	{"Schwarzdrachen", "NOUN_BLACK_DRAGON",   genitiv|dativ|akkusativ, maskulin,  n_singular},

	{"ein",   "ARTIKEL_UNBESTIMMTER", nominativ, maskulin|neutrum, n_singular},
	{"eines", "ARTIKEL_UNBESTIMMTER", genitiv,   maskulin|neutrum, n_singular},
	{"einem", "ARTIKEL_UNBESTIMMTER", dativ,     maskulin|neutrum, n_singular},
	{"einen", "ARTIKEL_UNBESTIMMTER", akkusativ, maskulin, n_singular},
	{"ein",   "ARTIKEL_UNBESTIMMTER", akkusativ, neutrum, n_singular},
	{"eine",  "ARTIKEL_UNBESTIMMTER", nominativ|akkusativ, feminin, n_singular},
	{"einer", "ARTIKEL_UNBESTIMMTER", genitiv|dativ,       feminin, n_singular},
	
	{"du",  "PRONOMEN_PERSONAL", nominativ, maskulin|feminin|neutrum,  n_singular},
	{"er",  "PRONOMEN_PERSONAL", nominativ, maskulin,  n_singular},
	{"sie", "PRONOMEN_PERSONAL", nominativ, feminin,   n_singular},
	{"es",  "PRONOMEN_PERSONAL", nominativ, neutrum,   n_singular},
	{"ihr", "PRONOMEN_PERSONAL", nominativ, maskulin|feminin|neutrum,  n_plural},

	{"dich", "PRONOMEN_PERSONAL", akkusativ, maskulin|feminin|neutrum,  n_singular},
	{"Euch", "PRONOMEN_PERSONAL", akkusativ, maskulin|feminin|neutrum,  n_plural},

	{"kein",   "PRONOMEN_KEIN", nominativ, maskulin|neutrum,  n_singular},
	{"keine",  "PRONOMEN_KEIN", nominativ, feminin,  n_singular},
	{"keines", "PRONOMEN_KEIN", genitiv, maskulin|neutrum,  n_singular},
	{"keinem", "PRONOMEN_KEIN", dativ, maskulin|neutrum,  n_singular},
	{"keiner", "PRONOMEN_KEIN", genitiv|dativ, feminin,  n_singular},
	{"keinen", "PRONOMEN_KEIN", akkusativ, maskulin,  n_singular},
	{"kein",   "PRONOMEN_KEIN", akkusativ, neutrum,  n_singular},
	{"keine",  "PRONOMEN_KEIN", akkusativ, feminin,  n_singular},

	{"dein",   "PRONOMEN_POSSESSIV", nominativ, maskulin|neutrum,  n_singular},
	{"deines", "PRONOMEN_POSSESSIV", genitiv,   maskulin|neutrum,  n_singular},
	{"deinem", "PRONOMEN_POSSESSIV", dativ,     maskulin|neutrum,  n_singular},
	{"deinen", "PRONOMEN_POSSESSIV", akkusativ, maskulin,          n_singular},
	{"dein",   "PRONOMEN_POSSESSIV", akkusativ, neutrum,           n_singular},

	{"deine",  "PRONOMEN_POSSESSIV", nominativ|akkusativ, feminin,  n_singular},
	{"deiner", "PRONOMEN_POSSESSIV", genitiv|dativ, feminin,  n_singular},

	{"genannt", "PARTIKEL_NAMED", nominativ|genitiv|dativ|akkusativ, maskulin|feminin|neutrum,  n_singular|n_plural},
	{"bezeichnet mit", "PARTIKEL_CALLED", nominativ|genitiv|dativ|akkusativ, maskulin|feminin|neutrum,  n_singular|n_plural},

	{NULL, NULL, 0, 0, 0}
};


struct verb {
	const char *verb;
	const char *typ;
	enum Person person;
	enum Numerus numerus;
	enum Casus casus;
};

struct verb verben[] = {
	{"bist",     "VERB_SEIN", zweitePerson, n_singular, akkusativ},
	{"ist",      "VERB_SEIN", drittePerson, n_singular, akkusativ},
	{"seid",     "VERB_SEIN", zweitePerson, n_plural, akkusativ},

	{"kannst",    "VERB_CAN", zweitePerson, n_singular, akkusativ},
	{"kann",      "VERB_CAN", drittePerson, n_singular, akkusativ},
	{"könnt",     "VERB_CAN", zweitePerson, n_plural, akkusativ},

	{"merkst",    "VERB_MERKEN", zweitePerson, n_singular, akkusativ},
	{"merkt",     "VERB_MERKEN", drittePerson, n_singular, akkusativ},
	{"merkt",     "VERB_MERKEN", zweitePerson, n_plural, akkusativ},

	{"riechst",    "VERB_SMELL", zweitePerson, n_singular, akkusativ},
	{"riecht",     "VERB_SMELL", drittePerson, n_singular, akkusativ},
	{"riecht",     "VERB_SMELL", zweitePerson, n_plural, akkusativ},

	{"hört",     "VERB_HEAR", drittePerson, n_singular, akkusativ},
	{"hörst",    "VERB_HEAR", zweitePerson, n_singular, akkusativ},
	{"hört",     "VERB_HEAR", zweitePerson, n_plural,   akkusativ},

	{"trittst",  "VERB_KICK", zweitePerson, n_singular, akkusativ},
	{"tritt",    "VERB_KICK", drittePerson, n_singular, akkusativ},
	{"beißt2",    "VERB_BITE", zweitePerson, n_singular, akkusativ},
	{"beißt3",    "VERB_BITE", drittePerson, n_singular, akkusativ},

	{"verfehlst", "VERB_MISS", zweitePerson, n_singular, akkusativ},
	{"verfehlt",  "VERB_MISS", drittePerson, n_singular, akkusativ},
	{"verfehlt",  "VERB_MISS", zweitePerson, n_plural, akkusativ},

	{"tötest",  "VERB_KILL", zweitePerson, n_singular, akkusativ},
	{"tötet",  "VERB_KILL", drittePerson, n_singular, akkusativ},
	{"tötet",  "VERB_KILL", zweitePerson, n_plural, akkusativ},

	{"triffst",  "VERB_HIT", zweitePerson, n_singular, akkusativ},
	{"trifft",  "VERB_HIT", drittePerson, n_singular, akkusativ},
	{"trefft",  "VERB_HIT", zweitePerson, n_plural, akkusativ},

	{"siehst",  "VERB_SEE", zweitePerson, n_singular, akkusativ},
	{"sieht",   "VERB_SEE", drittePerson, n_singular, akkusativ},
	{"seht",    "VERB_SEE", zweitePerson, n_plural, akkusativ},

	{"spürst",  "VERB_FEEL", zweitePerson, n_singular, akkusativ},
	{"spürt",   "VERB_FEEL", drittePerson, n_singular, akkusativ},
	{"spürt",   "VERB_FEEL", zweitePerson, n_plural, akkusativ},

	{"berührst",  "VERB_TOUCH", zweitePerson, n_singular, akkusativ},
	{"berührt",   "VERB_TOUCH", drittePerson, n_singular, akkusativ},
	{"berührt",   "VERB_TOUCH", zweitePerson, n_plural, akkusativ},

	{"getragen",   "VERB_WORN", zweitePerson|drittePerson, n_singular|n_plural, akkusativ},

	{NULL, NULL, 0, 0}
};

struct adjektiv {
	const char  *wort;
	const char  *typ;
	enum Casus   casus;
	enum Genus   genus;
	enum Numerus numerus;
	enum Artikel artikel;
};

struct adjektiv adjektive[] = {
	{"klarer", "ADJEKTIV_POT_CLEAR", nominativ, maskulin, n_singular, unbestimmter},
	{"klare",  "ADJEKTIV_POT_CLEAR", nominativ, maskulin, n_singular, bestimmter},
	{"klarer", "ADJEKTIV_POT_CLEAR", nominativ, maskulin, n_singular, ohne},

	{"himmelblauer", "ADJEKTIV_POT_SKY_BLUE", nominativ, maskulin, n_singular, unbestimmter},
	{"himmelblaue",  "ADJEKTIV_POT_SKY_BLUE", nominativ, maskulin, n_singular, bestimmter},
	{"himmelblaue",  "ADJEKTIV_POT_SKY_BLUE", nominativ, maskulin|feminin|neutrum, n_plural, ohne},

	{"verfluchter", "ADJEKTIV_CURSED", nominativ, maskulin, n_singular, ohne},
	{"verfluchte",  "ADJEKTIV_CURSED", nominativ, feminin, n_singular, ohne},
	{"verfluchtes", "ADJEKTIV_CURSED", nominativ, neutrum, n_singular, ohne},
	{"verfluchte",  "ADJEKTIV_CURSED", nominativ, maskulin|feminin|neutrum, n_plural, ohne},

	{"verfluchte",  "ADJEKTIV_CURSED", nominativ, maskulin|feminin|neutrum, n_singular, bestimmter},
	{"verfluchten",  "ADJEKTIV_CURSED", nominativ, maskulin|feminin|neutrum, n_plural, bestimmter},

	{"verfluchter",  "ADJEKTIV_CURSED", nominativ, maskulin, n_singular, unbestimmter},
	{"verfluchte",  "ADJEKTIV_CURSED", nominativ, feminin, n_singular, unbestimmter},
	{"verfluchtes",  "ADJEKTIV_CURSED", nominativ, neutrum, n_singular, unbestimmter},
	{"verfluchten",  "ADJEKTIV_CURSED", nominativ, maskulin|feminin|neutrum, n_plural, unbestimmter},

	{"nicht verfluchter", "ADJEKTIV_UNCURSED", nominativ, maskulin, n_singular, ohne},
	{"nicht verfluchte",  "ADJEKTIV_UNCURSED", nominativ, feminin, n_singular, ohne},
	{"nicht verfluchtes", "ADJEKTIV_UNCURSED", nominativ, neutrum, n_singular, ohne},
	{"nicht verfluchte",  "ADJEKTIV_UNCURSED", nominativ, maskulin|feminin|neutrum, n_plural, ohne},

	{"nicht verfluchte",  "ADJEKTIV_UNCURSED", nominativ, maskulin|feminin|neutrum, n_singular, bestimmter},
	{"nicht verfluchten",  "ADJEKTIV_UNCURSED", nominativ, maskulin|feminin|neutrum, n_plural, bestimmter},

	{"nicht verfluchter",  "ADJEKTIV_UNCURSED", nominativ, maskulin, n_singular, unbestimmter},
	{"nicht verfluchte",  "ADJEKTIV_UNCURSED", nominativ, feminin, n_singular, unbestimmter},
	{"nicht verfluchtes",  "ADJEKTIV_UNCURSED", nominativ, neutrum, n_singular, unbestimmter},
	{"nicht verfluchten",  "ADJEKTIV_UNCURSED", nominativ, maskulin|feminin|neutrum, n_plural, unbestimmter},

	{NULL, NULL, 0, 0, 0, 0}
};
