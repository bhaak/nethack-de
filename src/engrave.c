/*	SCCS Id: @(#)engrave.c	3.4	2001/11/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"
#include <ctype.h>

#ifdef GERMAN
# include "german.h"
#endif

STATIC_VAR NEARDATA struct engr *head_engr;

#ifdef OVLB
/* random engravings */
static const char *random_mesg[] = {
	"Elbereth",
	/* trap engravings */
	"Vlad war hier", "Ad Aerarium", /* EN "Vlad was here", "ad aerarium", */
	/* take-offs and other famous engravings */
	"Owlbreath", "Galadriel",
	"Kilroy war hier", /* EN "Kilroy was here", */
	"A.S. ->", "<- A.S.", /* Journey to the Center of the Earth */
	"You won't get it up the steps", /* EN "You won't get it up the steps", */ /* Adventure */ // TODO DE
	"Lasciate ogni speranza o voi ch'entrate.", /* Inferno */
	"Willkommen", /* EN "Well Come", */ /* Prisoner */
	"Wir entschuldigen uns f�r die Unannehmlichkeiten.", /* EN "We apologize for the inconvenience.", */ /* So Long... */
	"See you next Wednesday", /* EN "See you next Wednesday", */ /* Thriller */ // TODO DE
	"Notary Sojak", /* EN "notary sojak", */ /* Smokey Stover */
	"For a good time call 8?7-5309", /* EN "For a good time call 8?7-5309", */ // TODO DE
	"Tiere bitte nicht f�ttern.", /* EN "Please don't feed the animals.", */ /* Various zoos around the world */
	"Ein Neger mit Gazelle zagt im Regen nie.", /* A palindrome */ /* EN "Madam, in Eden, I'm Adam.", */
	"Und immer ein Bit �ber behalten!", /* die zwei Wolfgangs */ /* EN "Two thumbs up!", */ /* Siskel & Ebert */
	"Hello, World!", /* The First C Program */
#ifdef MAIL
	"Sie haben Post!", /* EN "You've got mail!", */ /* AOL */
#endif
	"Was steht an?", /* Kim Possible */ /* EN "As if!", */ /* Clueless */
};

char *
random_engraving(outbuf)
char *outbuf;
{
	const char *rumor;

	/* a random engraving may come from the "rumors" file,
	   or from the list above */
	if (!rn2(4) || !(rumor = getrumor(0, outbuf, TRUE)) || !*rumor)
	    Strcpy(outbuf, random_mesg[rn2(SIZE(random_mesg))]);

	wipeout_text(outbuf, (int)(strlen(outbuf) / 4), 0);
	return outbuf;
}

/* Partial rubouts for engraving characters. -3. */
static const struct {
	char		wipefrom;
	const char *	wipeto;
} rubouts[] = {
	{'A', "^"},     {'B', "Pb["},   {'C', "("},     {'D', "|)["},
	{'E', "|FL[_"}, {'F', "|-"},    {'G', "C("},    {'H', "|-"},
	{'I', "|"},     {'K', "|<"},    {'L', "|_"},    {'M', "|"},
	{'N', "|\\"},   {'O', "C("},    {'P', "F"},     {'Q', "C("},
	{'R', "PF"},    {'T', "|"},     {'U', "J"},     {'V', "/\\"},
	{'W', "V/\\"},  {'Z', "/"},
	{'b', "|"},     {'d', "c|"},    {'e', "c"},     {'g', "c"},
	{'h', "n"},     {'j', "i"},     {'k', "|"},     {'l', "|"},
	{'m', "nr"},    {'n', "r"},     {'o', "c"},     {'q', "c"},
	{'w', "v"},     {'y', "v"},
	{':', "."},     {';', ","},
	{'0', "C("},    {'1', "|"},     {'6', "o"},     {'7', "/"},
	{'8', "3o"}
};

void
wipeout_text(engr, cnt, seed)
char *engr;
int cnt;
unsigned seed;		/* for semi-controlled randomization */
{
	char *s;
	int i, j, nxt, use_rubout, lth = (int)strlen(engr);

	if (lth && cnt > 0) {
	    while (cnt--) {
		/* pick next character */
		if (!seed) {
		    /* random */
		    nxt = rn2(lth);
		    use_rubout = rn2(4);
		} else {
		    /* predictable; caller can reproduce the same sequence by
		       supplying the same arguments later, or a pseudo-random
		       sequence by varying any of them */
		    nxt = seed % lth;
		    seed *= 31,  seed %= (BUFSZ-1);
		    use_rubout = seed & 3;
		}
		s = &engr[nxt];
		if (*s == ' ') continue;

		/* rub out unreadable & small punctuation marks */
		if (index("?.,'`-|_", *s)) {
		    *s = ' ';
		    continue;
		}

		if (!use_rubout)
		    i = SIZE(rubouts);
		else
		    for (i = 0; i < SIZE(rubouts); i++)
			if (*s == rubouts[i].wipefrom) {
			    /*
			     * Pick one of the substitutes at random.
			     */
			    if (!seed)
				j = rn2(strlen(rubouts[i].wipeto));
			    else {
				seed *= 31,  seed %= (BUFSZ-1);
				j = seed % (strlen(rubouts[i].wipeto));
			    }
			    *s = rubouts[i].wipeto[j];
			    break;
			}

		/* didn't pick rubout; use '?' for unreadable character */
		if (i == SIZE(rubouts)) *s = '?';
	    }
	}

	/* trim trailing spaces */
	while (lth && engr[lth-1] == ' ') engr[--lth] = 0;
}

boolean
can_reach_floor()
{
	return (boolean)(!u.uswallow &&
#ifdef STEED
			/* Restricted/unskilled riders can't reach the floor */
			!(u.usteed && P_SKILL(P_RIDING) < P_BASIC) &&
#endif
			 (!Levitation ||
			  Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)));
}
#endif /* OVLB */
#ifdef OVL0

const char *
surface(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];

	if ((x == u.ux) && (y == u.uy) && u.uswallow &&
		is_animal(u.ustuck->data))
	    return "NOUN_MAGENINNERE"; /* EN return "maw"; */
	else if (IS_AIR(lev->typ) && Is_airlevel(&u.uz))
	    return "NOUN_AIR"; /* EN return "air"; */
	else if (is_pool(x,y))
	    return (Underwater && !Is_waterlevel(&u.uz)) ? "NOUN_GRUND" : "NOUN_WATER";
	else if (is_ice(x,y))
	    return "NOUN_ICE"; /* EN return "ice"; */
	else if (is_lava(x,y))
	    return "NOUN_LAVA"; /* EN return "lava"; */
	else if (lev->typ == DRAWBRIDGE_DOWN)
	    return "NOUN_BRIDGE"; /* EN return "bridge"; */
	else if(IS_ALTAR(levl[x][y].typ))
	    return "NOUN_ALTAR"; /* EN return "altar"; */
	else if(IS_GRAVE(levl[x][y].typ))
	    return "NOUN_HEADSTONE"; /* EN return "headstone"; */
	else if(IS_FOUNTAIN(levl[x][y].typ))
	    return "NOUN_FOUNTAIN"; /* EN return "fountain"; */
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
	    return "NOUN_FLOOR"; /* EN return "floor"; */
	else
	    return "NOUN_GROUND"; /* EN return "ground"; */
}

#ifdef GERMAN
const char *
zu_Boden(x, y)
register int x, y;
{
	const char *floor = german(surface(x, y));
	if (!strcmp(floor, "Mageninneres")) {
		return "ins Mageninnere";
	} else if (!strcmp(floor, "Luft")) {
		return "in die Luft";
	} else if (!strcmp(floor, "Grund")) {
		return "auf den Grund";
	} else if (!strcmp(floor, "Wasser")) {
		return "aufs Wasser";
	} else if (!strcmp(floor, "Eis")) {
		return "aufs Eis";
	} else if (!strcmp(floor, "Lava")) {
		return "auf die Lava";
	} else if (!strcmp(floor, "Br�cke")) {
		return "auf die Br�cke";
	} else if (!strcmp(floor, "Altar")) {
		return "auf den Altar";
	} else if (!strcmp(floor, "Grabstein")) {
		return "auf den Grabstein";
	} else if (!strcmp(floor, "Springbrunnen")) {
		return "auf den Springbrunnen";
	} else if (!strcmp(floor, "Boden")) {
		return "zu Boden";
	} else if (!strcmp(floor, "Erde")) {
		return "zur Erde";
	}
	impossible("zu_Boden: \"%s\" ist unbekannte Bodenart!", floor);
	return floor;
}

const char *
auf_dem_Boden(x, y)
register int x, y;
{
	const char *floor = german(surface(x, y));
	if (!strcmp(floor, "Mageninneres")) {
		return "im Mageninneren";
	} else if (!strcmp(floor, "Luft")) {
		return "in der Luft";
	} else if (!strcmp(floor, "Grund")) {
		return "auf dem Grund";
	} else if (!strcmp(floor, "Wasser")) {
		return "auf dem Wasser";
	} else if (!strcmp(floor, "Eis")) {
		return "auf dem Eis";
	} else if (!strcmp(floor, "Lava")) {
		return "auf der Lava";
	} else if (!strcmp(floor, "Br�cke")) {
		return "auf der Br�cke";
	} else if (!strcmp(floor, "Altar")) {
		return "auf dem Altar";
	} else if (!strcmp(floor, "Grabstein")) {
		return "auf dem Grabstein";
	} else if (!strcmp(floor, "Springbrunnen")) {
		return "auf dem Springbrunnen";
	} else if (!strcmp(floor, "Boden")) {
		return "auf dem Boden";
	} else if (!strcmp(floor, "Erde")) {
		return "auf der Erde";
	}
	impossible("auf_dem_Boden: \"%s\" ist unbekannte Bodenart!", floor);
	return floor;
}

const char *
auf_den_Boden(x, y)
register int x, y;
{
	const char *floor = german(surface(x, y));
	if (!strcmp(floor, "Mageninneres")) {
		return "ins Mageninneren";
	} else if (!strcmp(floor, "Luft")) {
		return "in die Luft";
	} else if (!strcmp(floor, "Grund")) {
		return "auf den Grund";
	} else if (!strcmp(floor, "Wasser")) {
		return "aufs Wasser";
	} else if (!strcmp(floor, "Eis")) {
		return "aufs Eis";
	} else if (!strcmp(floor, "Lava")) {
		return "auf die Lava";
	} else if (!strcmp(floor, "Br�cke")) {
		return "auf die Br�cke";
	} else if (!strcmp(floor, "Altar")) {
		return "auf den Altar";
	} else if (!strcmp(floor, "Grabstein")) {
		return "auf den Grabstein";
	} else if (!strcmp(floor, "Springbrunnen")) {
		return "auf den Springbrunnen";
	} else if (!strcmp(floor, "Boden")) {
		return "auf den Boden";
	} else if (!strcmp(floor, "Erde")) {
		return "auf die Erde";
	}
	impossible("auf_den_Boden: \"%s\" ist unbekannte Bodenart!", floor);
	return floor;
}

const char *
in_den_Boden(x, y)
register int x, y;
{
	const char *floor = german(surface(x, y));
	if (!strcmp(floor, "Mageninneres")) {
		return "ins Mageninnere";
	} else if (!strcmp(floor, "Luft")) {
		return "in die Luft";
	} else if (!strcmp(floor, "Grund")) {
		return "in den Grund";
	} else if (!strcmp(floor, "Wasser")) {
		return "ins Wasser";
	} else if (!strcmp(floor, "Eis")) {
		return "ins Eis";
	} else if (!strcmp(floor, "Lava")) {
		return "in die Lava";
	} else if (!strcmp(floor, "Br�cke")) {
		return "in die Br�cke";
	} else if (!strcmp(floor, "Altar")) {
		return "in den Altar";
	} else if (!strcmp(floor, "Grabstein")) {
		return "in den Grabstein";
	} else if (!strcmp(floor, "Springbrunnen")) {
		return "in den Springbrunnen";
	} else if (!strcmp(floor, "Boden")) {
		return "in den Boden";
	} else if (!strcmp(floor, "Erde")) {
		return "in die Erde";
	}
	impossible("in_den_Boden: \"%s\" ist unbekannte Bodenart!", floor);
	return floor;
}

const char *
im_Boden(x, y)
register int x, y;
{
	const char *floor = german(surface(x, y));
	if (!strcmp(floor, "Mageninneres")) {
		return "im Mageninneren";
	} else if (!strcmp(floor, "Luft")) {
		return "in der Luft";
	} else if (!strcmp(floor, "Grund")) {
		return "im Grund";
	} else if (!strcmp(floor, "Wasser")) {
		return "im Wasser";
	} else if (!strcmp(floor, "Eis")) {
		return "im Eis";
	} else if (!strcmp(floor, "Lava")) {
		return "in der Lava";
	} else if (!strcmp(floor, "Br�cke")) {
		return "in der Br�cke";
	} else if (!strcmp(floor, "Altar")) {
		return "im Altar";
	} else if (!strcmp(floor, "Grabstein")) {
		return "im Grabstein";
	} else if (!strcmp(floor, "Springbrunnen")) {
		return "im Springbrunnen";
	} else if (!strcmp(floor, "Boden")) {
		return "im Boden";
	} else if (!strcmp(floor, "Erde")) {
		return "in der Erde";
	}
	impossible("im_Boden: \"%s\" ist unbekannte Bodenart!", floor);
	return floor;
}

const char *
vom_Boden(x, y)
register int x, y;
{
	const char *floor = german(surface(x, y));
	if (!strcmp(floor, "Mageninneres")) {
		return "aus dem Mageninneren";
	} else if (!strcmp(floor, "Luft")) {
		return "aus der Luft";
	} else if (!strcmp(floor, "Grund")) {
		return "vom Grund";
	} else if (!strcmp(floor, "Wasser")) {
		return "vom Wasser";
	} else if (!strcmp(floor, "Eis")) {
		return "vom Eis";
	} else if (!strcmp(floor, "Lava")) {
		return "von der Lava";
	} else if (!strcmp(floor, "Br�cke")) {
		return "von der Br�cke";
	} else if (!strcmp(floor, "Altar")) {
		return "vom Altar";
	} else if (!strcmp(floor, "Grabstein")) {
		return "vom Grabstein";
	} else if (!strcmp(floor, "Springbrunnen")) {
		return "vom Springbrunnen";
	} else if (!strcmp(floor, "Boden")) {
		return "vom Boden";
	} else if (!strcmp(floor, "Erde")) {
		return "von der Erde";
	}
	impossible("vom_Boden: \"%s\" ist unbekannte Bodenart!", floor);
	return floor;
}
#endif

const char *
ceiling(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];
	const char *what;

	/* other room types will no longer exist when we're interested --
	 * see check_special_room()
	 */
	if (*in_rooms(x,y,VAULT))
	    what = "NOUN_VAULTS_CEILING"; /* EN what = "vault's ceiling"; */
	else if (*in_rooms(x,y,TEMPLE))
	    what = "NOUN_TEMPLES_CEILING"; /* EN what = "temple's ceiling"; */
	else if (*in_rooms(x,y,SHOPBASE))
	    what = "NOUN_SHOPS_CEILING"; /* EN what = "shop's ceiling"; */
	else if (IS_AIR(lev->typ))
	    what = "NOUN_SKY"; /* EN what = "sky"; */
	else if (Underwater)
	    what = "NOUN_WATERS_SURFACE"; /* EN what = "water's surface"; */
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
	    what = "NOUN_CEILING"; /* EN what = "ceiling"; */
	else
	    what = "NOUN_ROCK_ABOVE"; /* EN what = "rock above"; */

	return what;
}

struct engr *
engr_at(x, y)
xchar x, y;
{
	register struct engr *ep = head_engr;

	while(ep) {
		if(x == ep->engr_x && y == ep->engr_y)
			return(ep);
		ep = ep->nxt_engr;
	}
	return((struct engr *) 0);
}

#ifdef ELBERETH
/* Decide whether a particular string is engraved at a specified
 * location; a case-insensitive substring match used.
 * Ignore headstones, in case the player names herself "Elbereth".
 */
int
sengr_at(s, x, y)
	const char *s;
	xchar x, y;
{
	register struct engr *ep = engr_at(x,y);

	return (ep && ep->engr_type != HEADSTONE &&
		ep->engr_time <= moves && strstri(ep->engr_txt, s) != 0);
}
#endif /* ELBERETH */

#endif /* OVL0 */
#ifdef OVL2

void
u_wipe_engr(cnt)
register int cnt;
{
	if (can_reach_floor())
		wipe_engr_at(u.ux, u.uy, cnt);
}

#endif /* OVL2 */
#ifdef OVL1

void
wipe_engr_at(x,y,cnt)
register xchar x,y,cnt;
{
	register struct engr *ep = engr_at(x,y);

	/* Headstones are indelible */
	if(ep && ep->engr_type != HEADSTONE){
	    if(ep->engr_type != BURN || is_ice(x,y)) {
		if(ep->engr_type != DUST && ep->engr_type != ENGR_BLOOD) {
			cnt = rn2(1 + 50/(cnt+1)) ? 0 : 1;
		}
		wipeout_text(ep->engr_txt, (int)cnt, 0);
		while(ep->engr_txt[0] == ' ')
			ep->engr_txt++;
		if(!ep->engr_txt[0]) del_engr(ep);
	    }
	}
}

#endif /* OVL1 */
#ifdef OVL2

void
read_engr_at(x,y)
register int x,y;
{
	register struct engr *ep = engr_at(x,y);
	register int	sensed = 0;
	char buf[BUFSZ];
	
	/* Sensing an engraving does not require sight,
	 * nor does it necessarily imply comprehension (literacy).
	 */
	if(ep && ep->engr_txt[0]) {
	    switch(ep->engr_type) {
	    case DUST:
		if(!Blind) {
			sensed = 1;
			pline("SUBJECT %s VERB_SEIN hier in OBJECT ARTIKEL_BESTIMMTER %s geschrieben.", Something, /* EN pline("%s is written here in the %s.", Something, */
				is_ice(x,y) ? "NOUN_SCHNEE" : "NOUN_DUST"); /* EN is_ice(x,y) ? "frost" : "dust"); */
		}
		break;
	    case ENGRAVE:
	    case HEADSTONE:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
			pline("SUBJECT %s ist hier %s eingeritzt.", /* EN pline("%s is engraved here on the %s.", */
				Something,
				in_den_Boden(x,y)); /* EN surface(x,y)); */
		}
		break;
	    case BURN:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
			pline("Hier wurden Worte KASUS_AKKUSATIV %s %s.", /* EN pline("Some text has been %s into the %s here.", */
				in_den_Boden(x,y), /* EN is_ice(x,y) ? "melted" : "burned", */
				is_ice(x,y) ? "eingeschmolzen" : "eingebrannt"); /* EN surface(x,y)); */
		}
		break;
	    case MARK:
		if(!Blind) {
			sensed = 1;
			pline("Hier befindet sich Graffiti %s.", /* EN pline("There's some graffiti on the %s here.", */
				auf_dem_Boden(x,y)); /* EN surface(x,y)); */
		}
		break;
	    case ENGR_BLOOD:
		/* "It's a message!  Scrawled in blood!"
		 * "What's it say?"
		 * "It says... `See you next Wednesday.'" -- Thriller
		 */
		if(!Blind) {
			sensed = 1;
			You("VERB_SEE hier eine mit Blut geschriebene Nachricht."); /* EN You("see a message scrawled in blood here."); */
		}
		break;
	    default:
		impossible("%s ist auf sehr seltsame Art geschrieben.", /* EN impossible("%s is written in a very strange way.", */
				Something);
		sensed = 1;
	    }
	    if (sensed) {
	    	char *et;
	    	unsigned maxelen = BUFSZ - sizeof("Ihr f�hlt die Worte: \"\". "); // TODO DE EN
	    	if (strlen(ep->engr_txt) > maxelen) {
	    		(void) strncpy(buf,  ep->engr_txt, (int)maxelen);
			buf[maxelen] = '\0';
			et = buf;
		} else
			et = ep->engr_txt;
		pline("%s: \"%s\".", /* EN You("%s: \"%s\".", */
		      (Blind) ? "SUBJECT PRONOMEN_PERSONAL VERB_SPUEREN die Worte" : "Da steht",  et); // TODO ersp�rst ? TODO DE EN
		if(flags.run > 1) nomul(0);
	    }
	}
}

#endif /* OVL2 */
#ifdef OVLB

void
make_engr_at(x,y,s,e_time,e_type)
register int x,y;
register const char *s;
register long e_time;
register xchar e_type;
{
	register struct engr *ep;

	if ((ep = engr_at(x,y)) != 0)
	    del_engr(ep);
	ep = newengr(strlen(s) + 1);
	ep->nxt_engr = head_engr;
	head_engr = ep;
	ep->engr_x = x;
	ep->engr_y = y;
	ep->engr_txt = (char *)(ep + 1);
	Strcpy(ep->engr_txt, s);
	/* engraving Elbereth shows wisdom */
	if (!in_mklev && !strcmp(s, "Elbereth")) exercise(A_WIS, TRUE);
	ep->engr_time = e_time;
	ep->engr_type = e_type > 0 ? e_type : rnd(N_ENGRAVE-1);
	ep->engr_lth = strlen(s) + 1;
}

/* delete any engraving at location <x,y> */
void
del_engr_at(x, y)
int x, y;
{
	register struct engr *ep = engr_at(x, y);

	if (ep) del_engr(ep);
}

/*
 *	freehand - returns true if player has a free hand
 */
int
freehand()
{
	return(!uwep || !welded(uwep) ||
	   (!bimanual(uwep) && (!uarms || !uarms->cursed)));
/*	if ((uwep && bimanual(uwep)) ||
	    (uwep && uarms))
		return(0);
	else
		return(1);*/
}

static NEARDATA const char styluses[] =
	{ ALL_CLASSES, ALLOW_NONE, TOOL_CLASS, WEAPON_CLASS, WAND_CLASS,
	  GEM_CLASS, RING_CLASS, 0 };

/* Mohs' Hardness Scale:
 *  1 - Talc		 6 - Orthoclase
 *  2 - Gypsum		 7 - Quartz
 *  3 - Calcite		 8 - Topaz
 *  4 - Fluorite	 9 - Corundum
 *  5 - Apatite		10 - Diamond
 *
 * Since granite is a igneous rock hardness ~ 7, anything >= 8 should
 * probably be able to scratch the rock.
 * Devaluation of less hard gems is not easily possible because obj struct
 * does not contain individual oc_cost currently. 7/91
 *
 * steel     -	5-8.5	(usu. weapon)
 * diamond    - 10			* jade	     -	5-6	 (nephrite)
 * ruby       -  9	(corundum)	* turquoise  -	5-6
 * sapphire   -  9	(corundum)	* opal	     -	5-6
 * topaz      -  8			* glass      - ~5.5
 * emerald    -  7.5-8	(beryl)		* dilithium  -	4-5??
 * aquamarine -  7.5-8	(beryl)		* iron	     -	4-5
 * garnet     -  7.25	(var. 6.5-8)	* fluorite   -	4
 * agate      -  7	(quartz)	* brass      -	3-4
 * amethyst   -  7	(quartz)	* gold	     -	2.5-3
 * jasper     -  7	(quartz)	* silver     -	2.5-3
 * onyx       -  7	(quartz)	* copper     -	2.5-3
 * moonstone  -  6	(orthoclase)	* amber      -	2-2.5
 */

/* return 1 if action took 1 (or more) moves, 0 if error or aborted */
int
doengrave()
{
	boolean dengr = FALSE;	/* TRUE if we wipe out the current engraving */
	boolean doblind = FALSE;/* TRUE if engraving blinds the player */
	boolean doknown = FALSE;/* TRUE if we identify the stylus */
	boolean eow = FALSE;	/* TRUE if we are overwriting oep */
	boolean jello = FALSE;	/* TRUE if we are engraving in slime */
	boolean ptext = TRUE;	/* TRUE if we must prompt for engrave text */
	boolean teleengr =FALSE;/* TRUE if we move the old engraving */
	boolean zapwand = FALSE;/* TRUE if we remove a wand charge */
	xchar type = DUST;	/* Type of engraving made */
	char buf[BUFSZ];	/* Buffer for final/poly engraving text */
	char ebuf[BUFSZ];	/* Buffer for initial engraving text */
	char qbuf[QBUFSZ];	/* Buffer for query text */
	char post_engr_text[BUFSZ]; /* Text displayed after engraving prompt */
	const char *everb;	/* Present tense of engraving type */
	const char *eloc;	/* Where the engraving is (ie dust/floor/...) */
#ifdef GERMAN
	const char *eengraving;	/* Inschriftart */
	const char *eetwas;	/* Wenn engraving ein Objekt braucht */
	const char *ewohin;	/* Welche Pr�position beim Schreiben verwendet wird */
	char tbuf[BUFSZ];	/* Buffer fuer temporaere Satzteile */
#endif
	char *sp;		/* Place holder for space count of engr text */
	int len;		/* # of nonspace chars of new engraving text */
	int maxelen;		/* Max allowable length of engraving text */
	struct engr *oep = engr_at(u.ux,u.uy);
				/* The current engraving */
	struct obj *otmp;	/* Object selected with which to engrave */
	char *writer;

	multi = 0;		/* moves consumed */
	nomovemsg = (char *)0;	/* occupation end message */

	buf[0] = (char)0;
	ebuf[0] = (char)0;
	post_engr_text[0] = (char)0;
	maxelen = BUFSZ - 1;
	if (is_demon(youmonst.data) || youmonst.data->mlet == S_VAMPIRE)
	    type = ENGR_BLOOD;

	/* Can the adventurer engrave at all? */

	if(u.uswallow) {
		if (is_animal(u.ustuck->data)) {
			pline("Was VERB_WOLLEN PRONOMEN_PERSONAL schreiben?  \"Jonas war hier\"?"); /* EN pline("What would you write?  \"Jonah was here\"?"); */
			return(0);
		} else if (is_whirly(u.ustuck->data)) {
			You_cant("reach the %s.", surface(u.ux,u.uy)); /* EN You_cant("reach the %s.", surface(u.ux,u.uy)); */ // TODO DE
			return(0);
		} else
			jello = TRUE;
	} else if (is_lava(u.ux, u.uy)) {
		You_cant("auf Lava schreiben!"); /* EN You_cant("write on the lava!"); */
		return(0);
	} else if (is_pool(u.ux,u.uy) || IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		You_cant("auf Wasser schreiben!"); /* EN You_cant("write on the water!"); */
		return(0);
	}
	if(Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)/* in bubble */) {
		You_cant("in die Luft schreiben!"); /* EN You_cant("write in thin air!"); */
		return(0);
	}
	if (cantwield(youmonst.data)) {
		You("VERB_KOENNEN gar nichts halten!"); /* EN You_cant("even hold anything!"); */
		return(0);
	}
	if (check_capacity((char *)0)) return (0);

	/* One may write with finger, or weapon, or wand, or..., or...
	 * Edited by GAN 10/20/86 so as not to change weapon wielded.
	 */

	otmp = getobj(styluses, "schreiben"); /* EN otmp = getobj(styluses, "write with"); */
	if(!otmp) return(0);		/* otmp == zeroobj if fingers */

	if (otmp == &zeroobj) writer = makeplural(body_part(FINGER));
	else writer = xname(otmp);

	/* There's no reason you should be able to write with a wand
	 * while both your hands are tied up.
	 */
	if (!freehand() && otmp != uwep && !otmp->owornmask) {
		You("VERB_HAVE OBJECT PRONOMEN_KEIN %s frei um damit zu schreiben!", body_part(HAND)); /* EN You("have no free %s to write with!", body_part(HAND)); */
		return(0);
	}

	if (jello) {
		You("VERB_KITZELN OBJECT %s OBJECT KASUS_DATIV mit PRONOMEN_POSSESSIV %s.", mon_nam(u.ustuck), writer); /* EN You("tickle %s with your %s.", mon_nam(u.ustuck), writer); */
		Your("message dissolves..."); /* EN Your("message dissolves..."); */
		return(0);
	}
	if (otmp->oclass != WAND_CLASS && !can_reach_floor()) {
		You_cant("reach the %s!", surface(u.ux,u.uy)); /* EN You_cant("reach the %s!", surface(u.ux,u.uy)); */
		return(0);
	}
	if (IS_ALTAR(levl[u.ux][u.uy].typ)) {
		You("make a motion towards the altar with your %s.", writer); /* EN You("make a motion towards the altar with your %s.", writer); */ // TODO DE
		altar_wrath(u.ux, u.uy);
		return(0);
	}
	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (otmp == &zeroobj) { /* using only finger */
		You("MODIFIER_KONJUNKTIV_II VERB_WERDEN nur einen kleinen Fleck %s hinterlassen.", /* EN You("would only make a small smudge on the %s.", */
			auf_dem_Boden(u.ux, u.uy)); /* EN surface(u.ux, u.uy)); */
		return(0);
	    } else if (!levl[u.ux][u.uy].disturbed) {
		You("VERB_STOEREN die Ruhe der Untoten!"); /* EN You("disturb the undead!"); */
		levl[u.ux][u.uy].disturbed = 1;
		(void) makemon(&mons[PM_GHOUL], u.ux, u.uy, NO_MM_FLAGS);
		exercise(A_WIS, FALSE);
		return(1);
	    }
	}

	/* SPFX for items */

	switch (otmp->oclass) {
	    default:
	    case AMULET_CLASS:
	    case CHAIN_CLASS:
	    case POTION_CLASS:
	    case COIN_CLASS:
		break;

	    case RING_CLASS:
		/* "diamond" rings and others should work */
	    case GEM_CLASS:
		/* diamonds & other hard gems should work */
		if (objects[otmp->otyp].oc_tough) {
			type = ENGRAVE;
			break;
		}
		break;

	    case ARMOR_CLASS:
		if (is_boots(otmp)) {
			type = DUST;
			break;
		}
		/* fall through */
	    /* Objects too large to engrave with */
	    case BALL_CLASS:
	    case ROCK_CLASS:
		You("VERB_KOENNEN mit einem so gro�en Objekt nichts einritzen!"); /* EN You_cant("engrave with such a large object!"); */
		ptext = FALSE;
		break;

	    /* Objects too silly to engrave with */
	    case FOOD_CLASS:
	    case SCROLL_CLASS:
	    case SPBOOK_CLASS:
		Your("%s MODIFIER_KONJUNKTIV_II VERB_WERDEN %s werden.", xname(otmp), /* EN Your("%s would get %s.", xname(otmp), */
			is_ice(u.ux,u.uy) ? "nur eisgek�hlt" : "zu dreckig"); /* EN is_ice(u.ux,u.uy) ? "all frosty" : "too dirty"); */
		ptext = FALSE;
		break;

	    case RANDOM_CLASS:	/* This should mean fingers */
		break;

	    /* The charge is removed from the wand before prompting for
	     * the engraving text, because all kinds of setup decisions
	     * and pre-engraving messages are based upon knowing what type
	     * of engraving the wand is going to do.  Also, the player
	     * will have potentially seen "You wrest .." message, and
	     * therefore will know they are using a charge.
	     */
	    case WAND_CLASS:
		if (zappable(otmp)) {
		    check_unpaid(otmp);
		    zapwand = TRUE;
		    if (Levitation) ptext = FALSE;

		    switch (otmp->otyp) {
		    /* DUST wands */
		    default:
			break;

			/* NODIR wands */
		    case WAN_LIGHT:
		    case WAN_SECRET_DOOR_DETECTION:
		    case WAN_CREATE_MONSTER:
		    case WAN_WISHING:
		    case WAN_ENLIGHTENMENT:
			zapnodir(otmp);
			break;

			/* IMMEDIATE wands */
			/* If wand is "IMMEDIATE", remember to affect the
			 * previous engraving even if turning to dust.
			 */
		    case WAN_STRIKING:
			Strcpy(post_engr_text,
			"SUBJECT ARTIKEL_BESTIMMTER NOUN_WAND VERB_WIDERSETZEN sich erfolglos OBJECT KASUS_DATIV PRONOMEN_POSSESSIV NOUN_SCHREIBVERSUCH!" /* EN "The wand unsuccessfully fights your attempt to write!" */
			);
			break;
		    case WAN_SLOW_MONSTER:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "Das Ungeziefer %s wuselt langsamer!", /* EN "The bugs on the %s slow down!", */
				   auf_dem_Boden(u.ux, u.uy)); /* EN surface(u.ux, u.uy)); */
			}
			break;
		    case WAN_SPEED_MONSTER:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "Das Ungeziefer %s wuselt schneller!", /* EN "The bugs on the %s speed up!", */
				   auf_dem_Boden(u.ux, u.uy)); /* EN surface(u.ux, u.uy)); */
			}
			break;
		    case WAN_POLYMORPH:
			if(oep)  {
			    if (!Blind) {
				type = (xchar)0;	/* random */
				(void) random_engraving(buf);
			    }
			    dengr = TRUE;
			}
			break;
		    case WAN_NOTHING:
		    case WAN_UNDEAD_TURNING:
		    case WAN_OPENING:
		    case WAN_LOCKING:
		    case WAN_PROBING:
			break;

			/* RAY wands */
		    case WAN_MAGIC_MISSILE:
			ptext = TRUE;
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "SUBJECT ARTIKEL_BESTIMMTER %s VERB_AUSSEHEN SATZKLAMMER wie von Kugeln durchsiebt!", /* EN "The %s is riddled by bullet holes!", */
				   surface(u.ux, u.uy));
			}
			break;

		    /* can't tell sleep from death - Eric Backus */
		    case WAN_SLEEP:
		    case WAN_DEATH:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "Das Ungeziefer %s bleiben stehen!", /* EN "The bugs on the %s stop moving!", */
				   auf_dem_Boden(u.ux, u.uy)); /* EN surface(u.ux, u.uy)); */
			}
			break;

		    case WAN_COLD:
			if (!Blind)
			    Strcpy(post_engr_text,
				"Ein paar Eisw�rfel bilden sich an der Spitze des Stabes."); /* EN "A few ice cubes drop from the wand."); */
			if(!oep || (oep->engr_type != BURN))
			    break;
		    case WAN_CANCELLATION:
		    case WAN_MAKE_INVISIBLE:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
				pline("Die Inschrift %s verschwindet!", /* EN pline_The("engraving on the %s vanishes!", */
					auf_dem_Boden(u.ux,u.uy)); /* EN surface(u.ux,u.uy)); */
			    dengr = TRUE;
			}
			break;
		    case WAN_TELEPORTATION:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
				pline("Die Inschrift %s verschwindet!", /* EN pline_The("engraving on the %s vanishes!", */
					auf_dem_Boden(u.ux,u.uy)); /* EN surface(u.ux,u.uy)); */
			    teleengr = TRUE;
			}
			break;

		    /* type = ENGRAVE wands */
		    case WAN_DIGGING:
			ptext = TRUE;
			type  = ENGRAVE;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("SUBJECT PRONOMEN_DIESER %s VERB_SEIN OBJECT KASUS_NOMINATIV ARTIKEL_UNBESTIMMTER NOUN_WAND PARTIKEL_OF NOUN_WAND_DIGGING!", /* EN pline("This %s is a wand of digging!", */
				xname(otmp));
			    doknown = TRUE;
			}
			if (!Blind)
			    Strcpy(post_engr_text,
				IS_GRAVE(levl[u.ux][u.uy].typ) ?
				"Ein paar Steinbrocken platzen vom Grabstein ab." : /* EN "Chips fly out from the headstone." : */
				is_ice(u.ux,u.uy) ?
				"Eissplitter fliegen von der Eisoberfl�che hoch!" : /* EN "Ice chips fly up from the ice surface!" : */
				"Kies fliegt vom Boden hoch."); /* EN "Gravel flies up from the floor."); */
			else
			    Strcpy(post_engr_text, "SUBJECT PRONOMEN_PERSONAL Gebohre!"); /* EN Strcpy(post_engr_text, "You hear drilling!"); */
			break;

		    /* type = BURN wands */
		    case WAN_FIRE:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			if (flags.verbose)
			    pline("SUBJECT PRONOMEN_DIESER %s VERB_SEIN OBJECT KASUS_NOMINATIV ARTIKEL_UNBESTIMMTER NOUN_WAND PARTIKEL_OF NOUN_WAND_FIRE!", xname(otmp)); /* EN pline("This %s is a wand of fire!", xname(otmp)); */
			    doknown = TRUE;
			}
			Strcpy(post_engr_text,
				Blind ? "SUBJECT PRONOMEN_PERSONAL VERB_SPUEREN, wie sich der Stab erw�rmt." : /* EN Blind ? "You feel the wand heat up." : */
					"Flammen schie�en aus dem Stab."); /* EN "Flames fly from the wand."); */
			break;
		    case WAN_LIGHTNING:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("SUBJECT PRONOMEN_DIESER %s VERB_SEIN OBJECT KASUS_NOMINATIV ARTIKEL_UNBESTIMMTER NOUN_WAND PARTIKEL_OF NOUN_WAND_LIGHTNING!", /* EN pline("This %s is a wand of lightning!", */
					xname(otmp));
			    doknown = TRUE;
			}
			if (!Blind) {
			    Strcpy(post_engr_text,
				    "Blitze schie�en aus dem Stab."); /* EN "Lightning arcs from the wand."); */
			    doblind = TRUE;
			} else
			    Strcpy(post_engr_text, "SUBJECT PRONOMEN_PERSONAL VERB_HEAR Geknacke!"); /* EN Strcpy(post_engr_text, "You hear crackling!"); */
			break;

		    /* type = MARK wands */
		    /* type = ENGR_BLOOD wands */
		    }
		} else /* end if zappable */
		    if (!can_reach_floor()) {
			You_cant("reach the %s!", surface(u.ux,u.uy)); /* EN You_cant("reach the %s!", surface(u.ux,u.uy)); */ // TODO DE
			return(0);
		    }
		break;

	    case WEAPON_CLASS:
		if (is_blade(otmp)) {
		    if ((int)otmp->spe > -3)
			type = ENGRAVE;
		    else
			Your("%s ist f�rs Einritzen zu stumpf.", aobjnam(otmp,"VERB_SEIN")); /* EN Your("%s too dull for engraving.", aobjnam(otmp,"are")); */
		}
		break;

	    case TOOL_CLASS:
		if(otmp == ublindf) {
		    pline(
		"Damit zu schreiben, ist etwas schwierig, SUBJECT_IM_SATZ VERB_MEINEN PRONOMEN_PERSONAL nicht auch?"); /* EN "That is a bit difficult to engrave with, don't you think?"); */
		    return(0);
		}
		switch (otmp->otyp)  {
		    case MAGIC_MARKER:
			if (otmp->spe <= 0)
			    Deinem("NOUN_MARKER ist die Tinte ausgegangen."); /* EN Your("marker has dried out."); */
			else
			    type = MARK;
			break;
		    case TOWEL:
			/* Can't really engrave with a towel */
			ptext = FALSE;
			if (oep)
			    if ((oep->engr_type == DUST ) ||
				(oep->engr_type == ENGR_BLOOD) ||
				(oep->engr_type == MARK )) {
				if (!Blind)
				    You("VERB_WEGWISCHEN die Kritzelei SATZKLAMMER."); /* EN You("wipe out the message here."); */
				else
				    Your("%s %s %s.", xname(otmp),
					 otense(otmp, "VERB_WERDEN"), /* EN otense(otmp, "get"), */
					 is_ice(u.ux,u.uy) ?
					 "tiefgek�hlt" : "dreckig"); /* EN "frosty" : "dusty"); */
				dengr = TRUE;
			    } else
				Your("%s VERB_KOENNEN diese Inschrift nicht wegwischen.", /* EN Your("%s can't wipe out this engraving.", */
				     xname(otmp));
			else
			    Your("%s %s %s.", xname(otmp), otense(otmp, "VERB_WERDEN"), /* EN Your("%s %s %s.", xname(otmp), otense(otmp, "get"), */
				  is_ice(u.ux,u.uy) ? "tiefgek�hlt" : "dreckig"); /* EN is_ice(u.ux,u.uy) ? "frosty" : "dusty"); */
			break;
		    default:
			break;
		}
		break;

	    case VENOM_CLASS:
#ifdef WIZARD
		if (wizard) {
		    pline("Writing a poison pen letter??"); /* EN pline("Writing a poison pen letter??"); */ // TODO DE
		    break;
		}
#endif
	    case ILLOBJ_CLASS:
		impossible("You're engraving with an illegal object!");
		break;
	}

	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (type == ENGRAVE || type == 0)
		type = HEADSTONE;
	    else {
		/* ensures the "cannot wipe out" case */
		type = DUST;
		dengr = FALSE;
		teleengr = FALSE;
		buf[0] = (char)0;
	    }
	}

	/* End of implement setup */

	/* Identify stylus */
	if (doknown) {
	    makeknown(otmp->otyp);
	    more_experienced(0,10);
	}

	if (teleengr) {
	    rloc_engr(oep);
	    oep = (struct engr *)0;
	}

	if (dengr) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Something has changed the engraving here */
	if (*buf) {
	    make_engr_at(u.ux, u.uy, buf, moves, type);
	    pline("Der Text lautet jetzt: \"%s\".", buf); /* EN pline_The("engraving now reads: \"%s\".", buf); */
	    ptext = FALSE;
	}

	if (zapwand && (otmp->spe < 0)) {
	    pline("SUBJECT %s %szerf�llt zu Staub.", /* EN pline("%s %sturns to dust.", */
		  The(xname(otmp)), Blind ? "" : "VERB_LEUCHTEN grell auf und "); /* EN The(xname(otmp)), Blind ? "" : "glows violently, then "); */
	    if (!IS_GRAVE(levl[u.ux][u.uy].typ))
		pline("Der Versuch, KASUS_DATIV mit PRONOMEN_POSSESSIV NOUN_DUST KASUS_AKKUSATIV in ARTIKEL_BESTIMMTER %s zu schreiben, ist zum Scheitern verurteilt.", /* EN You("are not going to get anywhere trying to write in the %s with your dust.", */
		    is_ice(u.ux,u.uy) ? "NOUN_SCHNEE" : "NOUN_DUST"); /* EN is_ice(u.ux,u.uy) ? "frost" : "dust"); */
	    useup(otmp);
	    ptext = FALSE;
	}

	if (!ptext) {		/* Early exit for some implements. */
	    if (otmp->oclass == WAND_CLASS && !can_reach_floor())
		You_cant("reach the %s!", surface(u.ux,u.uy)); /* EN You_cant("reach the %s!", surface(u.ux,u.uy)); */ // TODO DE
	    return(1);
	}

	/* Special effects should have deleted the current engraving (if
	 * possible) by now.
	 */

	if (oep) {
	    register char c = 'n';

	    /* Give player the choice to add to engraving. */

	    if (type == HEADSTONE) {
		/* no choice, only append */
		c = 'y';
	    } else if ( (type == oep->engr_type) && (!Blind ||
		 (oep->engr_type == BURN) || (oep->engr_type == ENGRAVE)) ) {
		c = yn_function("SUBJECT MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL dem aktuellen Text etwas hinzuf�gen?", /* EN c = yn_function("Do you want to add to the current engraving?", */
				ynqchars, 'y');
		if (c == 'q') {
		    pline(Never_mind);
		    return(0);
		}
	    }

	    if (c == 'n' || Blind) {

		if( (oep->engr_type == DUST) || (oep->engr_type == ENGR_BLOOD) ||
		    (oep->engr_type == MARK) ) {
		    if (!Blind) {
			You("VERB_WEGWISCHEN die Kritzelei SATZKLAMMER, die hier %sgeschrieben stand.", /* EN You("wipe out the message that was %s here.", */
			    ((oep->engr_type == DUST)  ? "im Staub " : /* EN ((oep->engr_type == DUST)  ? "written in the dust" : */
			    ((oep->engr_type == ENGR_BLOOD) ? "mit Blut "   : /* EN ((oep->engr_type == ENGR_BLOOD) ? "scrawled in blood"   : */
							 ""))); /* EN "written"))); */
			del_engr(oep);
			oep = (struct engr *)0;
		    } else
		   /* Don't delete engr until after we *know* we're engraving */
			eow = TRUE;
		} else
		    if ( (type == DUST) || (type == MARK) || (type == ENGR_BLOOD) ) {
			You(
			 "VERB_KOENNEN die Kritzelei, die hier %s %s ist, nicht wegwischen.", in_den_Boden(u.ux,u.uy), /* EN "cannot wipe out the message that is %s the %s here.", */
			 oep->engr_type == BURN ?
			   (is_ice(u.ux,u.uy) ? "eingeschmolzen" : "eingebrannt") : /* EN (is_ice(u.ux,u.uy) ? "melted into" : "burned into") : */
			   "eingeritzt"); /* EN "engraved in", surface(u.ux,u.uy)); */
			return(1);
		    } else
			if ( (type != oep->engr_type) || (c == 'n') ) {
			    if (!Blind || can_reach_floor())
				You("VERB_UEBERSCHREIBEN den derzeitigen Text."); /* EN You("will overwrite the current message."); */
			    eow = TRUE;
			}
	    }
	}

	eloc = surface(u.ux,u.uy);
#ifdef GERMAN
	eetwas = (oep && !eow ? " etwas" : "");
#endif
	switch(type){
	    default:
#ifdef GERMAN
		eengraving = (oep && !eow ? "der seltsamen Kritzelei " : "");
		ewohin = "auf ARTIKEL_BESTIMMTER";
#endif
		everb = (oep && !eow ? "VERB_HINZUFUEGEN" : /* EN everb = (oep && !eow ? "add to the weird writing on" : */
				       "VERB_SCHREIBEN"); /* EN "write strangely on"); */
		break;
	    case DUST:
#ifdef GERMAN
		eengraving = (oep && !eow ? "der Kritzelei OBJECT KASUS_DATIV " : "");
		ewohin = "in ARTIKEL_BESTIMMTER";
#endif
		everb = (oep && !eow ? "VERB_HINZUFUEGEN" : /* EN everb = (oep && !eow ? "add to the writing in" : */
				       "VERB_SCHREIBEN"); /* EN "write in"); */
		eloc = is_ice(u.ux,u.uy) ? "NOUN_SCHNEE" : "NOUN_DUST"; /* EN eloc = is_ice(u.ux,u.uy) ? "frost" : "dust"; */
		break;
	    case HEADSTONE:
#ifdef GERMAN
		eengraving = (oep && !eow ? "der Grabinschrift OBJECT KASUS_DATIV " : "");
		ewohin = "auf ARTIKEL_BESTIMMTER";
		eetwas = " etwas"; 
#endif
		everb = (oep && !eow ? "VERB_HINZUFUEGEN" : /* EN everb = (oep && !eow ? "add to the epitaph on" : */
				       "VERB_EINRITZEN"); /* EN "engrave on"); */
		break;
	    case ENGRAVE:
#ifdef GERMAN
		eengraving = (oep && !eow ? "der Einritzung OBJECT KASUS_DATIV " : "");
		ewohin = "in ARTIKEL_BESTIMMTER";
		eetwas = " etwas"; 
#endif
		everb = (oep && !eow ? "VERB_HINZUFUEGEN" : /* EN everb = (oep && !eow ? "add to the engraving in" : */
				       "VERB_EINRITZEN"); /* EN "engrave in"); */
		break;
	    case BURN:
#ifdef GERMAN
		eengraving = (oep && !eow ?
		             (is_ice(u.ux,u.uy) ? "der eingeschmolzenen Schrift OBJECT KASUS_DATIV " : "der eingebrannten Schrift OBJECT KASUS_DATIV ") : "");
		ewohin = "in ARTIKEL_BESTIMMTER";
		eetwas = " etwas"; 
#endif
		everb = (oep && !eow ?
			( is_ice(u.ux,u.uy) ? "VERB_HINZUFUEGEN" : /* EN ( is_ice(u.ux,u.uy) ? "add to the text melted into" : */
					      "VERB_HINZUFUEGEN") : /* EN "add to the text burned into") : */
			( is_ice(u.ux,u.uy) ? "VERB_EINSCHMELZEN" : "VERB_EINBRENNEN")); /* EN ( is_ice(u.ux,u.uy) ? "melt into" : "burn into")); */
		break;
	    case MARK:
#ifdef GERMAN
		eengraving = (oep && !eow ? "dem Graffiti OBJECT KASUS_DATIV " : "");
		ewohin = "auf ARTIKEL_BESTIMMTER";
#endif
		everb = (oep && !eow ? "VERB_HINZUFUEGEN" : /* EN everb = (oep && !eow ? "add to the graffiti on" : */
				       "VERB_KRITZELN"); /* EN "scribble on"); */
		break;
	    case ENGR_BLOOD:
#ifdef GERMAN
		eengraving = (oep && !eow ? "der blutigen Krakelei OBJECT KASUS_DATIV " : "");
		ewohin = "auf ARTIKEL_BESTIMMTER";
#endif
		everb = (oep && !eow ? "VERB_HINZUFUEGEN" : /* EN everb = (oep && !eow ? "add to the scrawl on" : */
				       "VERB_KRAKELN"); /* EN "scrawl on"); */
		break;
	}

	/* Tell adventurer what is going on */
	if (otmp != &zeroobj)
	    You("%s OBJECT %s%s %s OBJECT KASUS_DATIV mit %s%s SATZKLAMMER.", everb, eengraving, ewohin, eloc, doname(otmp), eetwas); /* EN You("%s the %s with %s.", everb, eloc, doname(otmp)); */
	else
#ifdef GERMAN
	    You("%s %s OBJECT%s %s%s %s%s SATZKLAMMER.", everb,
		german(strcat(strcpy(tbuf, "KASUS_DATIV mit PRONOMEN_POSSESSIV "), makeplural(body_part(FINGER)))),
		(oep && !eow) ? " KASUS_DATIV" : "",
		eengraving, ewohin, eloc, eetwas);
#else
	    You("%s the %s with your %s.", everb, eloc,
		makeplural(body_part(FINGER)));
#endif

	/* Prompt for engraving! */
#ifdef GERMAN
	if (oep && !eow) {
		Sprintf(qbuf,"Was MODIFIER_KONJUNKTIV_II VERB_MOEGEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL hier %sMODIFIER_VERB_INFINITIV %s?", eengraving, everb); /* hinzuf�gen */
	} else {
		Sprintf(qbuf,"Was MODIFIER_KONJUNKTIV_II VERB_MOEGEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL hier OBJECT %s %s MODIFIER_VERB_INFINITIV %s?", ewohin, eloc, everb); /* schreiben */
	}
#else
	Sprintf(qbuf,"What do you want to %s the %s here?", everb, eloc);
#endif
	getlin(qbuf, ebuf);

	/* Count the actual # of chars engraved not including spaces */
	len = strlen(ebuf);
	for (sp = ebuf; *sp; sp++) if (isspace(*sp)) len -= 1;

	if (len == 0 || index(ebuf, '\033')) {
	    if (zapwand) {
		if (!Blind)
		    pline("SUBJECT %s und %s wieder.", /* EN pline("%s, then %s.", */
			  Tobjnam(otmp, "VERB_ERSTRAHLEN"), otense(otmp, "VERB_ERLOESCHEN")); /* EN Tobjnam(otmp, "glow"), otense(otmp, "fade")); */
		return(1);
	    } else {
		pline(Never_mind);
		return(0);
	    }
	}

	/* A single `x' is the traditional signature of an illiterate person */
	if (len != 1 || (!index(ebuf, 'x') && !index(ebuf, 'X')))
	    u.uconduct.literate++;

	/* Mix up engraving if surface or state of mind is unsound.
	   Note: this won't add or remove any spaces. */
	for (sp = ebuf; *sp; sp++) {
	    if (isspace(*sp)) continue;
	    if (((type == DUST || type == ENGR_BLOOD) && !rn2(25)) ||
		    (Blind && !rn2(11)) || (Confusion && !rn2(7)) ||
		    (Stunned && !rn2(4)) || (Hallucination && !rn2(2)))
		*sp = ' ' + rnd(96 - 2);	/* ASCII '!' thru '~'
						   (excludes ' ' and DEL) */
	}

	/* Previous engraving is overwritten */
	if (eow) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Figure out how long it took to engrave, and if player has
	 * engraved too much.
	 */
	switch(type){
	    default:
		multi = -(len/10);
		if (multi) nomovemsg = "SUBJECT PRONOMEN_PERSONAL VERB_STELLEN OBJECT PRONOMEN_POSSESSIV ADJEKTIV_SELTSAM NOUN_ENGRAVING fertig."; /* EN if (multi) nomovemsg = "You finish your weird engraving."; */
		break;
	    case DUST:
		multi = -(len/10);
		if (multi) nomovemsg = "SUBJECT PRONOMEN_PERSONAL VERB_SEIN mit dem Schreiben im Staub fertig."; /* EN if (multi) nomovemsg = "You finish writing in the dust."; */
		break;
	    case HEADSTONE:
	    case ENGRAVE:
		multi = -(len/10);
		if ((otmp->oclass == WEAPON_CLASS) &&
		    ((otmp->otyp != ATHAME) || otmp->cursed)) {
		    multi = -len;
		    maxelen = ((otmp->spe + 3) * 2) + 1;
			/* -2 = 3, -1 = 5, 0 = 7, +1 = 9, +2 = 11
			 * Note: this does not allow a +0 anything (except
			 *	 an athame) to engrave "Elbereth" all at once.
			 *	 However, you could now engrave "Elb", then
			 *	 "ere", then "th".
			 */
		    Your("%s stumpf.", aobjnam(otmp, "VERB_WERDEN")); /* EN Your("%s dull.", aobjnam(otmp, "get")); */
		    if (otmp->unpaid) {
			struct monst *shkp = shop_keeper(*u.ushops);
			if (shkp) {
			    You("damage it, you pay for it!"); /* EN You("damage it, you pay for it!"); */ // TODO DE
			    bill_dummy_object(otmp);
			}
		    }
		    if (len > maxelen) {
			multi = -maxelen;
			otmp->spe = -3;
		    } else if (len > 1)
			otmp->spe -= len >> 1;
		    else otmp->spe -= 1; /* Prevent infinite engraving */
		} else
		    if ( (otmp->oclass == RING_CLASS) ||
			 (otmp->oclass == GEM_CLASS) )
			multi = -len;
		if (multi) nomovemsg = "SUBJECT PRONOMEN_PERSONAL VERB_SEIN mit dem Einritzen fertig."; /* EN if (multi) nomovemsg = "You finish engraving."; */
		break;
	    case BURN:
		multi = -(len/10);
		if (multi)
		    nomovemsg = is_ice(u.ux,u.uy) ?
			"SUBJECT PRONOMEN_PERSONAL VERB_HABEN die Worte ins Eis geschmolzen.": /* EN "You finish melting your message into the ice.": */
			"SUBJECT PRONOMEN_PERSONAL VERB_HABEN OBJECT die Worte in den Boden gebrannt."; /* EN "You finish burning your message into the floor."; */
		break;
	    case MARK:
		multi = -(len/10);
		if ((otmp->oclass == TOOL_CLASS) &&
		    (otmp->otyp == MAGIC_MARKER)) {
		    maxelen = (otmp->spe) * 2; /* one charge / 2 letters */
		    if (len > maxelen) {
			Deinem("NOUN_MARKER geht die Tinte aus."); /* EN Your("marker dries out."); */
			otmp->spe = 0;
			multi = -(maxelen/10);
		    } else
			if (len > 1) otmp->spe -= len >> 1;
			else otmp->spe -= 1; /* Prevent infinite grafitti */
		}
		if (multi) nomovemsg = "SUBJECT PRONOMEN_PERSONAL VERB_SEIN mit der Verschandelung des Dungeons fertig."; /* EN if (multi) nomovemsg = "You finish defacing the dungeon."; */
		break;
	    case ENGR_BLOOD:
		multi = -(len/10);
		if (multi) nomovemsg = "SUBJECT PRONOMEN_PERSONAL VERB_SEIN mit OBJECT KASUS_DATIV PRONOMEN_POSSESSIV NOUN_KRAKELEI fertig."; /* EN if (multi) nomovemsg = "You finish scrawling."; */
		break;
	}

	/* Chop engraving down to size if necessary */
	if (len > maxelen) {
	    for (sp = ebuf; (maxelen && *sp); sp++)
		if (!isspace(*sp)) maxelen--;
	    if (!maxelen && *sp) {
		*sp = (char)0;
		if (multi) nomovemsg = "SUBJECT PRONOMEN_PERSONAL VERB_KOENNEN nichts mehr schreiben."; /* EN if (multi) nomovemsg = "You cannot write any more."; */
		You("VERB_KOENNEN nur noch \"%s\" schreiben", ebuf); /* EN You("only are able to write \"%s\"", ebuf); */
	    }
	}

	/* Add to existing engraving */
	if (oep) Strcpy(buf, oep->engr_txt);

	(void) strncat(buf, ebuf, (BUFSZ - (int)strlen(buf) - 1));

	make_engr_at(u.ux, u.uy, buf, (moves - multi), type);

	if (post_engr_text[0]) pline(post_engr_text);

	if (doblind && !resists_blnd(&youmonst)) {
	    pline("Der Blitz blendet KASUS_AKKUSATIV PRONOMEN_PERSONAL!"); /* EN You("are blinded by the flash!"); */
	    make_blinded((long)rnd(50),FALSE);
	    if (!Blind) Your(vision_clears);
	}

	return(1);
}

void
save_engravings(fd, mode)
int fd, mode;
{
	register struct engr *ep = head_engr;
	register struct engr *ep2;
	unsigned no_more_engr = 0;

	while (ep) {
	    ep2 = ep->nxt_engr;
	    if (ep->engr_lth && ep->engr_txt[0] && perform_bwrite(mode)) {
		bwrite(fd, (genericptr_t)&(ep->engr_lth), sizeof(ep->engr_lth));
		bwrite(fd, (genericptr_t)ep, sizeof(struct engr) + ep->engr_lth);
	    }
	    if (release_data(mode))
		dealloc_engr(ep);
	    ep = ep2;
	}
	if (perform_bwrite(mode))
	    bwrite(fd, (genericptr_t)&no_more_engr, sizeof no_more_engr);
	if (release_data(mode))
	    head_engr = 0;
}

void
rest_engravings(fd)
int fd;
{
	register struct engr *ep;
	unsigned lth;

	head_engr = 0;
	while(1) {
		mread(fd, (genericptr_t) &lth, sizeof(unsigned));
		if(lth == 0) return;
		ep = newengr(lth);
		mread(fd, (genericptr_t) ep, sizeof(struct engr) + lth);
		ep->nxt_engr = head_engr;
		head_engr = ep;
		ep->engr_txt = (char *) (ep + 1);	/* Andreas Bormann */
		/* mark as finished for bones levels -- no problem for
		 * normal levels as the player must have finished engraving
		 * to be able to move again */
		ep->engr_time = moves;
	}
}

void
del_engr(ep)
register struct engr *ep;
{
	if (ep == head_engr) {
		head_engr = ep->nxt_engr;
	} else {
		register struct engr *ept;

		for (ept = head_engr; ept; ept = ept->nxt_engr)
		    if (ept->nxt_engr == ep) {
			ept->nxt_engr = ep->nxt_engr;
			break;
		    }
		if (!ept) {
		    impossible("Error in del_engr?");
		    return;
		}
	}
	dealloc_engr(ep);
}

/* randomly relocate an engraving */
void
rloc_engr(ep)
struct engr *ep;
{
	int tx, ty, tryct = 200;

	do  {
	    if (--tryct < 0) return;
	    tx = rn1(COLNO-3,2);
	    ty = rn2(ROWNO);
	} while (engr_at(tx, ty) ||
		!goodpos(tx, ty, (struct monst *)0, 0));

	ep->engr_x = tx;
	ep->engr_y = ty;
}


/* Epitaphs for random headstones */
static const char *epitaphs[] = {
	"Ruhe in Frieden", /* EN "Rest in peace", */
	"R.I.P.",
	"Muhe in Frieden", /* EN "Rest In Pieces", */
	"Hinweis -- Es gibt KEINE wertvollen Sachen in diesem Grab", /* EN "Note -- there are NO valuable items in this grave", */
	"1994 - 1995. Der langlebigste Hacker von allen", /* EN "1994-1995. The Longest-Lived Hacker Ever", */
	"Das Grab des unbekannten Hackers", /* EN "The Grave of the Unknown Hacker", */
	"Wir waren nicht sicher, was es war, aber wir begruben es trotzdem", /* EN "We weren't sure who this was, but we buried him here anyway", */
	"Fido -- er war ein ganz braver Hund", /* EN "Sparky -- he was a very good dog", */
	"�bertreten der Gleise verboten", /* EN "Beware of Electric Third Rail", */
	"Made in Taiwan",
	"Og Freund. Og guter Kerl. Og stirbt. Og jetzt Essen", /* EN "Og friend. Og good dude. Og died. Og now food", */
	"Beetlejuice, Beetlejuice, Beetlejuice", /* EN "Beetlejuice Beetlejuice Beetlejuice", */
	"Achtung, unter dir!", /* EN "Look out below!", */
	"Bitte grabt mich nicht aus. Mir geht's fantastisch hier unten. -- Der Bewohner", /* EN "Please don't dig me up. I'm perfectly happy down here. -- Resident", */
	"Lieber Postbote, bitte Nachsenden an: Gehennom, Asmodeus' Festung, f�nfter Lemure von links", /* EN "Postman, please note forwarding address: Gehennom, Asmodeus's Fortress, fifth lemure on the left", */
	"Mary had a little lamb/Its fleece was white as snow/When Mary was in trouble/The lamb was first to go", /* EN "Mary had a little lamb/Its fleece was white as snow/When Mary was in trouble/The lamb was first to go", */ // TODO DE
	"Pass auf oder Dir passiert dasselbe!", /* EN "Be careful, or this could happen to you!", */
	"Bald wirst Du diesem Kameraden in der H�lle Gesellschaft leisten! -- der Zauberer von Yendor", /* EN "Soon you'll join this fellow in hell! -- the Wizard of Yendor", */
	"Warnung! Dieses Grab enth�lt Giftm�ll", /* EN "Caution! This grave contains toxic waste", */
	"Sum quod eris",
	"Hier liegt ein Atheist, fertig f�r das Leben nach dem Tod, doch wo soll das stattfinden?", /* EN "Here lies an Atheist, all dressed up and no place to go", */
	"Hier ruht Ezechiel, Alter 102.  Quem di diligunt, adolescens moritur.", /* EN "Here lies Ezekiel, age 102.  The good die young.", */
	"Hier ruht mein Weib, Gott sei's gedankt!  Solang sie lebte, war nur Zank.  Geh, Wandrer, gehe flugs von hier; sonst steht sie auf und zankt mit Dir!", /* EN "Here lies my wife: Here let her lie! Now she's at rest and so am I.", */
	"Here lies Johnny Yeast. Pardon me for not rising.", /* EN "Here lies Johnny Yeast. Pardon me for not rising.", */ // TODO DE
	"He always lied while on the earth and now he's lying in it", /* EN "He always lied while on the earth and now he's lying in it", */ // TODO DE
	"Ich wollte aber einge�schert werden!", /* EN "I made an ash of myself", */
	"Soon ripe. Soon rotten. Soon gone. But not forgotten.", /* EN "Soon ripe. Soon rotten. Soon gone. But not forgotten.", */ // TODO DE
	"Hier ruht Jonathan Maas. Statt auf die Bremse trat er auf's Gas.", /* EN "Here lies the body of Jonathan Blake. Stepped on the gas instead of the brake.", */
	"Lass mich in Ruhe!" /* EN "Go away!" */
};

/* Create a headstone at the given location.
 * The caller is responsible for newsym(x, y).
 */
void
make_grave(x, y, str)
int x, y;
const char *str;
{
	/* Can we put a grave here? */
	if ((levl[x][y].typ != ROOM && levl[x][y].typ != GRAVE) || t_at(x,y)) return;

	/* Make the grave */
	levl[x][y].typ = GRAVE;

	/* Engrave the headstone */
	if (!str) str = epitaphs[rn2(SIZE(epitaphs))];
	del_engr_at(x, y);
	make_engr_at(x, y, str, 0L, HEADSTONE);
	return;
}


#endif /* OVLB */

/*engrave.c*/
