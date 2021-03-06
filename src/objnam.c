/*	SCCS Id: @(#)objnam.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef GERMAN
# include "german.h"
# include <ctype.h>
#endif

/* "an uncursed greased partly eaten guardian naga hatchling [corpse]" */
#define PREFIX	110	/* (56) */ /* EN #define PREFIX	80	*/
#define SCHAR_LIM 127
#define NUMOBUF 12

STATIC_DCL char *FDECL(strprepend,(char *,const char *));
#ifdef OVLB
static boolean FDECL(wishymatch, (const char *,const char *,BOOLEAN_P));
#endif
static char *NDECL(nextobuf);
static void FDECL(add_erosion_words, (struct obj *, char *));

struct Jitem {
	int item;
	const char *name;
};

/* true for gems/rocks that should have " stone" appended to their names */
#define GemStone(typ)	(typ == FLINT ||				\
			 (objects[typ].oc_material == GEMSTONE &&	\
			  (typ != DILITHIUM_CRYSTAL && typ != RUBY &&	\
			   typ != DIAMOND && typ != SAPPHIRE &&		\
			   typ != BLACK_OPAL && 	\
			   typ != EMERALD && typ != OPAL)))

#ifndef OVLB

STATIC_DCL struct Jitem Japanese_items[];

#else /* OVLB */

STATIC_OVL struct Jitem Japanese_items[] = {
	{ SHORT_SWORD, "NOUN_JAP_WAKIZASHI" }, /* EN { SHORT_SWORD, "wakizashi" }, */
	{ BROADSWORD, "NOUN_JAP_NINJA_TO" }, /* EN { BROADSWORD, "ninja-to" }, */
	{ FLAIL, "NOUN_JAP_NUNCHAKU" }, /* EN { FLAIL, "nunchaku" }, */
	{ GLAIVE, "NOUN_JAP_NAGINATA" }, /* EN { GLAIVE, "naginata" }, */
	{ LOCK_PICK, "NOUN_JAP_OSAKU" }, /* EN { LOCK_PICK, "osaku" }, */
	{ WOODEN_HARP, "NOUN_JAP_KOTO" }, /* EN { WOODEN_HARP, "koto" }, */
	{ KNIFE, "NOUN_JAP_SHITO" }, /* EN { KNIFE, "shito" }, */
	{ PLATE_MAIL, "NOUN_JAP_TANKO" }, /* EN { PLATE_MAIL, "tanko" }, */
	{ HELMET, "NOUN_JAP_KABUTO" }, /* EN { HELMET, "kabuto" }, */
	{ LEATHER_GLOVES, "NOUN_JAP_YUGAKE" }, /* EN { LEATHER_GLOVES, "yugake" }, */
	{ FOOD_RATION, "NOUN_JAP_GUNYOKI" }, /* EN { FOOD_RATION, "gunyoki" }, */
	{ POT_BOOZE, "NOUN_JAP_SAKE" }, /* EN { POT_BOOZE, "sake" }, */
	{0, "" }
};

#endif /* OVLB */

STATIC_DCL const char *FDECL(Japanese_item_name,(int i));

#ifdef OVL1

STATIC_OVL char *
strprepend(s,pref)
register char *s;
register const char *pref;
{
	register int i = (int)strlen(pref);

	if(i > PREFIX) {
		impossible("PREFIX %s too short (for %d) %s.", pref, i, s); /* EN impossible("PREFIX too short (for %d).", i); */
		return(s);
	}
	s -= i;
	(void) strncpy(s, pref, i);	/* do not copy trailing 0 */
	return(s);
}

#endif /* OVL1 */
#ifdef OVLB

/* manage a pool of BUFSZ buffers, so callers don't have to */
static char *
nextobuf()
{
	static char NEARDATA bufs[NUMOBUF][BUFSZ];
	static int bufidx = 0;

	bufidx = (bufidx + 1) % NUMOBUF;
	return bufs[bufidx];
}

char *
obj_typename(otyp)
register int otyp;
{
	char *buf = nextobuf();
	register struct objclass *ocl = &objects[otyp];
	register const char *actualn = OBJ_NAME(*ocl);
	register const char *dn = OBJ_DESCR(*ocl);
	register const char *un = ocl->oc_uname;
	register int nn = ocl->oc_name_known;

	if (Role_if(PM_SAMURAI) && Japanese_item_name(otyp))
		actualn = Japanese_item_name(otyp);
	switch(ocl->oc_class) {
	case COIN_CLASS:
		Strcpy(buf, "NOUN_COIN");
		break;
	case POTION_CLASS:
		Strcpy(buf, "NOUN_POTION");
		break;
	case SCROLL_CLASS:
		Strcpy(buf, "NOUN_SCROLL");
		break;
	case WAND_CLASS:
		Strcpy(buf, "NOUN_WAND");
		break;
	case SPBOOK_CLASS:
		Strcpy(buf, "NOUN_SPELLBOOK");
		break;
	case RING_CLASS:
		Strcpy(buf, "NOUN_RING");
		break;
	case AMULET_CLASS:
		if(nn)
			Strcpy(buf,actualn);
		else
			Strcpy(buf,"NOUN_AMULET");
		if(un)
			Sprintf(eos(buf)," PARTIKEL_CALLED %s",un);
		if(dn)
			Sprintf(eos(buf)," (%s)",dn);
		return(buf);
	default:
		if(nn) {
			Strcpy(buf, actualn);
			if (GemStone(otyp))
				Strcat(buf, " stone"); /* EN Strcat(buf, " stone"); */ // TODO DE
			if(un)
				Sprintf(eos(buf), " PARTIKEL_CALLED %s", un);
			if(dn)
				Sprintf(eos(buf), " (%s)", dn);
		} else {
			Strcpy(buf, dn ? dn : actualn);
			if(ocl->oc_class == GEM_CLASS)
				Strcat(buf, (ocl->oc_material == MINERAL) ?
						" stone" : " gem"); /* EN " stone" : " gem"); */ // TODO DE
			if(un)
				Sprintf(eos(buf), " PARTIKEL_CALLED %s", un);
		}
		return(buf);
	}
	/* here for ring/scroll/potion/wand */
	if(nn) {
	    if (ocl->oc_unique)
		Strcpy(buf, actualn); /* avoid spellbook of Book of the Dead */
	    else
		Sprintf(eos(buf), " PARTIKEL_OF %s", actualn);
	}
	if(un)
		Sprintf(eos(buf), " PARTIKEL_CALLED %s", un);
	if(dn)
		Sprintf(eos(buf), " (%s)", dn);
	return(buf);
}

/* less verbose result than obj_typename(); either the actual name
   or the description (but not both); user-assigned name is ignored */
char *
simple_typename(otyp)
int otyp;
{
    char *bufp, *pp, *save_uname = objects[otyp].oc_uname;

    objects[otyp].oc_uname = 0;		/* suppress any name given by user */
    bufp = obj_typename(otyp);
    objects[otyp].oc_uname = save_uname;
    if ((pp = strstri(bufp, " (")) != 0)
	*pp = '\0';		/* strip the appended description */
    return bufp;
}

boolean
obj_is_pname(obj)
register struct obj *obj;
{
    return((boolean)(obj->dknown && obj->known && obj->onamelth &&
		     /* Since there aren't any objects which are both
		        artifacts and unique, the last check is redundant. */
		     obj->oartifact && !objects[obj->otyp].oc_unique));
}

/* Give the name of an object seen at a distance.  Unlike xname/doname,
 * we don't want to set dknown if it's not set already.  The kludge used is
 * to temporarily set Blind so that xname() skips the dknown setting.  This
 * assumes that we don't want to do this too often; if this function becomes
 * frequently used, it'd probably be better to pass a parameter to xname()
 * or doname() instead.
 */
char *
distant_name(obj, func)
register struct obj *obj;
char *FDECL((*func), (OBJ_P));
{
	char *str;

	long save_Blinded = Blinded;
	Blinded = 1;
	str = (*func)(obj);
	Blinded = save_Blinded;
	return str;
}

/* convert player specified fruit name into corresponding fruit juice name
   ("slice of pizza" -> "pizza juice" rather than "slice of pizza juice") */
char *
fruitname(juice)
boolean juice;	/* whether or not to append " juice" to the name */
{
    char *buf = nextobuf();
    char *fruit_nam = strstri(pl_fruit, " PARTIKEL_OF "); /* EN const char *fruit_nam = strstri(pl_fruit, " of "); */

    if (fruit_nam)
	fruit_nam += 4;		/* skip past " of " */
    else
	fruit_nam = pl_fruit;	/* use it as is */

#ifdef GERMAN
    fruit_nam[0] = toupper(fruit_nam[0]);
#endif
    Sprintf(buf, "%s%s", makesingular(fruit_nam), juice ? "-NOUN_SAFT" : ""); /* EN Sprintf(buf, "%s%s", makesingular(fruit_nam), juice ? " juice" : ""); */
    return buf;
}

#endif /* OVLB */
#ifdef OVL1

char *
xname(obj)
register struct obj *obj;
{
	register char *buf;
	register int typ = obj->otyp;
	register struct objclass *ocl = &objects[typ];
	register int nn = ocl->oc_name_known;
	register const char *actualn = OBJ_NAME(*ocl);
	register const char *dn = OBJ_DESCR(*ocl);
	register const char *un = ocl->oc_uname;

	buf = nextobuf() + PREFIX;	/* leave room for "17 -3 " */
	if (Role_if(PM_SAMURAI) && Japanese_item_name(typ))
		actualn = Japanese_item_name(typ);

	buf[0] = '\0';
	/*
	 * clean up known when it's tied to oc_name_known, eg after AD_DRIN
	 * This is only required for unique objects since the article
	 * printed for the object is tied to the combination of the two
	 * and printing the wrong article gives away information.
	 */
	if (!nn && ocl->oc_uses_known && ocl->oc_unique) obj->known = 0;
	if (!Blind) obj->dknown = TRUE;
	if (Role_if(PM_PRIEST)) obj->bknown = TRUE;
	if (obj_is_pname(obj))
	    goto nameit;
	switch (obj->oclass) {
	    case AMULET_CLASS:
		if (!obj->dknown)
			Strcpy(buf, "NOUN_AMULET");
		else if (typ == AMULET_OF_YENDOR ||
			 typ == FAKE_AMULET_OF_YENDOR)
			/* each must be identified individually */
			Strcpy(buf, obj->known ? actualn : dn);
		else if (nn)
			Strcpy(buf, actualn);
		else if (un)
			Sprintf(buf,"NOUN_AMULET PARTIKEL_CALLED %s", un);
		else
			Sprintf(buf,"%s NOUN_AMULET", dn);
		break;
	    case WEAPON_CLASS:
		if (is_poisonable(obj) && obj->opoisoned)
			Strcpy(buf, "ADJEKTIV_POISONED "); /* EN Strcpy(buf, "poisoned "); */
	    case VENOM_CLASS:
	    case TOOL_CLASS:
		if (typ == LENSES)
			Strcpy(buf, "NOUN_PAAR ");

		if (!obj->dknown)
			Strcat(buf, dn ? dn : actualn);
		else if (nn)
			Strcat(buf, actualn);
		else if (un) {
			Strcat(buf, dn ? dn : actualn);
			Strcat(buf, " PARTIKEL_CALLED ");
			Strcat(buf, un);
		} else
			Strcat(buf, dn ? dn : actualn);
		/* If we use an() here we'd have to remember never to use */
		/* it whenever calling doname() or xname(). */
		if (typ == FIGURINE)
		    Sprintf(eos(buf), " PARTIKEL_OF ARTIKEL_UNBESTIMMTER %s",
			//index(vowels,*(mons[obj->corpsenm].mname)) ? "n" : "",
			mons[obj->corpsenm].mname);
		break;
	    case ARMOR_CLASS:
		/* depends on order of the dragon scales objects */
		if (typ >= GRAY_DRAGON_SCALES && typ <= YELLOW_DRAGON_SCALES) {
			Sprintf(buf, "set of %s", actualn); /* EN Sprintf(buf, "set of %s", actualn); */ // TODO DE
			break;
		}
		/* CHECK ME: Ich bin nicht sicher, ob das stimmt. 
		 * Sind die Substantive nach 'Paar' in einem bestimmten Fall,
		 * oder in demselben wie 'Paar'? */
		if(is_boots(obj) || is_gloves(obj)) Strcpy(buf,"NOUN_PAAR ");

		if(obj->otyp >= ELVEN_SHIELD && obj->otyp <= ORCISH_SHIELD
				&& !obj->dknown) {
			Strcpy(buf, "NOUN_SHIELD");
			break;
		}
		if(obj->otyp == SHIELD_OF_REFLECTION && !obj->dknown) {
			Strcpy(buf, "smooth shield"); /* EN Strcpy(buf, "smooth shield"); */ // TODO DE
			break;
		}
#ifdef GERMAN
		if(obj->otyp == MUMMY_WRAPPING) {
			Strcpy(buf, "NOUN_MUMMY_WRAPPINGs");
			break;
		}
#endif

		if(nn)	Strcat(buf, actualn);
		else if(un) {
			if(is_boots(obj))
				Strcat(buf,"NUMERUS_PLURAL NOUN_BOOTS");
			else if(is_gloves(obj))
				Strcat(buf,"NUMERUS_PLURAL NOUN_GLOVES");
			else if(is_cloak(obj))
				Strcpy(buf,"NOUN_CLOAK");
			else if(is_helmet(obj))
				Strcpy(buf,"NOUN_HELMET");
			else if(is_shield(obj))
				Strcpy(buf,"NOUN_SHIELD");
			else
				Strcpy(buf,"NOUN_ARMOR"); /* EN Strcpy(buf,"armor"); */
			Strcat(buf, " PARTIKEL_CALLED ");
			Strcat(buf, un);
		} else	Strcat(buf, dn);
		break;
	    case FOOD_CLASS:
		if (typ == SLIME_MOLD) {
			register struct fruit *f;

			for(f=ffruit; f; f = f->nextf) {
				if(f->fid == obj->spe) {
					Strcpy(buf, f->fname);
					break;
				}
			}
			if (!f) impossible("Bad fruit #%d?", obj->spe);
			break;
		}

		Strcpy(buf, actualn);
		if (typ == TIN && obj->known) {
		    if(obj->spe > 0)
			Strcat(buf, " PARTIKEL_OF NOUN_SPINACH");
		    else if (obj->corpsenm == NON_PM)
		        Strcpy(buf, "ADJEKTIV_LEER NOUN_TIN"); /* EN Strcpy(buf, "empty tin"); */
		    else if (vegetarian(&mons[obj->corpsenm]))
			Sprintf(eos(buf), " PARTIKEL_OF %s", mons[obj->corpsenm].mname);
		    else
			Sprintf(eos(buf), " PARTIKEL_OF %s NOUN_MEAT ", mons[obj->corpsenm].mname);
		}
		break;
	    case COIN_CLASS:
	    case CHAIN_CLASS:
		Strcpy(buf, actualn);
		break;
	    case ROCK_CLASS:
		if (typ == STATUE)
		    Sprintf(buf, "%s%s PARTIKEL_OF %s%s",
			(Role_if(PM_ARCHEOLOGIST) && (obj->spe & STATUE_HISTORIC)) ? "ADJEKTIV_ANTIK " : "" , /* EN (Role_if(PM_ARCHEOLOGIST) && (obj->spe & STATUE_HISTORIC)) ? "historic " : "" , */
			actualn,
			type_is_pname(&mons[obj->corpsenm]) ? "" :
			  (mons[obj->corpsenm].geno & G_UNIQ) ? "ARTIKEL_BESTIMMTER " : /* EN (mons[obj->corpsenm].geno & G_UNIQ) ? "the " : */
			    (index(vowels,*(mons[obj->corpsenm].mname)) ?
								"ARTIKEL_UNBESTIMMTER " : "ARTIKEL_UNBESTIMMTER "),
			mons[obj->corpsenm].mname);
		else Strcpy(buf, actualn);
		break;
	    case BALL_CLASS:
		Sprintf(buf, "%sADJEKTIV_SCHWER NOUN_IRON_BALL", /* EN Sprintf(buf, "%sheavy iron ball", */
			(obj->owt > ocl->oc_weight) ? "sehr " : ""); /* EN (obj->owt > ocl->oc_weight) ? "very " : ""); */
		break;
	    case POTION_CLASS:
		if (obj->dknown && obj->odiluted)
			Strcpy(buf, "ADJEKTIV_VERDUENNT "); /* EN Strcpy(buf, "diluted "); */
		if(nn || un || !obj->dknown) {
			Strcat(buf, "NOUN_POTION");
			if(!obj->dknown) break;
			if(nn) {
				Strcat(buf, " PARTIKEL_OF ");
				if (typ == POT_WATER &&
						obj->bknown && (obj->blessed || obj->cursed)) {
					Strcat(buf, obj->blessed ? "NOUN_POT_HOLY_WATER" : "NOUN_POT_UNHOLY_WATER");
				} else {
					Strcat(buf, actualn);
				}
			} else {
				Strcat(buf, " PARTIKEL_CALLED ");
				Strcat(buf, un);
			}
		} else {
			Strcat(buf, dn);
			Strcat(buf, " NOUN_POTION");
		}
		break;
	case SCROLL_CLASS:
		Strcpy(buf, "NOUN_SCROLL");
		if(!obj->dknown) break;
		if(nn) {
			Strcat(buf, " PARTIKEL_OF ");
			Strcat(buf, actualn);
		} else if(un) {
			Strcat(buf, " PARTIKEL_CALLED ");
			Strcat(buf, un);
		} else if (ocl->oc_magic) {
			Strcat(buf, " PARTIKEL_LABELED ");
			Strcat(buf, "\"");
			//Strcat(buf, "�");
			Strcat(buf, dn);
			//Strcat(buf, "�");
			Strcat(buf, "\"");
			//fprintf(stderr, "buf: %s\n", buf);
		} else {
			Strcpy(buf, dn);
			Strcat(buf, " NOUN_SCROLL");
		}
		break;
	case WAND_CLASS:
		if(!obj->dknown)
			Strcpy(buf, "NOUN_WAND");
		else if(nn)
			Sprintf(buf, "NOUN_WAND PARTIKEL_OF %s", actualn);
		else if(un)
			Sprintf(buf, "NOUN_WAND PARTIKEL_CALLED %s", un);
		else
			Sprintf(buf, "%s NOUN_WAND", dn);
		break;
	case SPBOOK_CLASS:
		if (!obj->dknown) {
			Strcpy(buf, "NOUN_SPELLBOOK");
		} else if (nn) {
			if (typ != SPE_BOOK_OF_THE_DEAD)
			    Strcpy(buf, "NOUN_SPELLBOOK PARTIKEL_OF ");
			Strcat(buf, actualn);
		} else if (un) {
			Sprintf(buf, "NOUN_SPELLBOOK PARTIKEL_CALLED %s", un);
		} else
			Sprintf(buf, "%s NOUN_SPELLBOOK", dn);
		break;
	case RING_CLASS:
		if(!obj->dknown)
			Strcpy(buf, "NOUN_RING");
		else if(nn)
			//Sprintf(buf, "NOUN_RING PARTIKEL_OF %s", actualn);
			Sprintf(buf, "NOUN_RING KASUS_GENITIV ARTIKEL_BESTIMMTER %s", actualn);
		else if(un)
			Sprintf(buf, "NOUN_RING PARTIKEL_CALLED %s", un);
		else
			//Sprintf(buf, "%s NOUN_RING", dn);
			Sprintf(buf, "NOUN_%s", dn);
		break;
	case GEM_CLASS:
	    {
		const char *rock =
			    (ocl->oc_material == MINERAL) ? "NOUN_GEM_ROCK" : "NOUN_GEM";
		if (!obj->dknown) {
		    Strcpy(buf, rock);
		} else if (!nn) {
		    if (un) Sprintf(buf,"%s PARTIKEL_CALLED %s", rock, un);
		    else Sprintf(buf, "%s %s", dn, rock);
		} else {
		    Strcpy(buf, actualn);
// "Stein" is already attached to the name of the stone where necessary
#ifndef GERMAN
		    if (GemStone(typ)) Strcat(buf, " stone");
#endif
		}
		break;
	    }
	default:
		Sprintf(buf,"glorkum %d %d %d", obj->oclass, typ, obj->spe);
	}
	if (obj->quan != 1L) Strcpy(buf, makeplural(buf));

	if (obj->onamelth && obj->dknown) {
		Strcat(buf, " PARTIKEL_NAMED "); /* EN Strcat(buf, " named "); */
nameit:
		Strcat(buf, german(ONAME(obj))); /* EN Strcat(buf, ONAME(obj)); */
	}

	if (!strncmpi(buf, "the ", 4)) buf += 4; /* EN if (!strncmpi(buf, "the ", 4)) buf += 4; */ // TODO DE
	return(buf);
}

/* xname() output augmented for multishot missile feedback */
char *
mshot_xname(obj)
struct obj *obj;
{
    char tmpbuf[BUFSZ];
    char *onm = xname(obj);

    if (m_shot.n > 1 && m_shot.o == obj->otyp) {
	/* copy xname's result so that we can reuse its return buffer */
	Strcpy(tmpbuf, onm);
	/* "the Nth arrow"; value will eventually be passed to an() or
	   The(), both of which correctly handle this "the " prefix */
	Sprintf(onm, "ARTIKEL_BESTIMMTER %d%s %s", m_shot.i, ordin(m_shot.i), tmpbuf); /* EN Sprintf(onm, "the %d%s %s", m_shot.i, ordin(m_shot.i), tmpbuf); */
    }

    return onm;
}

#endif /* OVL1 */
#ifdef OVL0

/* used for naming "the unique_item" instead of "a unique_item" */
boolean
the_unique_obj(obj)
register struct obj *obj;
{
    if (!obj->dknown)
	return FALSE;
    else if (obj->otyp == FAKE_AMULET_OF_YENDOR && !obj->known)
	return TRUE;		/* lie */
    else
	return (boolean)(objects[obj->otyp].oc_unique &&
			 (obj->known || obj->otyp == AMULET_OF_YENDOR));
}

static void
add_erosion_words(obj,prefix)
struct obj *obj;
char *prefix;
{
	boolean iscrys = (obj->otyp == CRYSKNIFE);


	if (!is_damageable(obj) && !iscrys) return;

#ifdef GERMAN
	char *oeroded_text[][2] = {
		// obj->oeroded != 3,   obj->oeroded == 3
		{"ADJEKTIV_ROSTIG ",    "ADJEKTIV_VERROSTET "}, //  is_rustprone(obj)
		{"ADJEKTIV_ANGESENGT ", "ADJEKTIV_VERSENGT "},  // !is_rustprone(obj)
	};
	char *oeroded2_text[][2] = {
		// obj->oeroded2 != 3,   obj->oeroded2 == 3
		{"ADJEKTIV_CORRODED ",  "ADJEKTIV_CORRODED "}, //  is_corrodeable(obj)
		{"ADJEKTIV_ANGEFAULT ", "ADJEKTIV_VERFAULT "}, // !is_corrodeable(obj)
	};
#endif
	/* The only cases where any of these bits do double duty are for
	 * rotted food and diluted potions, which are all not is_damageable().
	 */
	if (obj->oeroded && !iscrys) {
		switch (obj->oeroded) {
			case 2:	Strcat(prefix, "stark "); break; /* EN case 2:	Strcat(prefix, "very "); break; */
			case 3:	Strcat(prefix, "ganz "); break; /* EN case 3:	Strcat(prefix, "thoroughly "); break; */
		}			
		Strcat(prefix, oeroded_text[(is_rustprone(obj) ? 0 : 1)][(obj->oeroded != 3 ? 0 : 1)]); /* EN Strcat(prefix, is_rustprone(obj) ? "rusty " : "burnt "); */
	}
	if (obj->oeroded2 && !iscrys) {
		switch (obj->oeroded2) {
			case 2:	Strcat(prefix, "stark "); break; /* EN case 2:	Strcat(prefix, "very "); break; */
			case 3:	Strcat(prefix, "ganz "); break; /* EN case 3:	Strcat(prefix, "thoroughly "); break; */
		}			
		Strcat(prefix, /* EN Strcat(prefix, is_corrodeable(obj) ? "corroded " : */
			oeroded2_text[(is_corrodeable(obj) ? 0 : 1)][(obj->oeroded2 != 3 ? 0 : 1)]); /* EN "rotted "); */
	}
	if (obj->rknown && obj->oerodeproof)
		Strcat(prefix,
		       iscrys ? "ADJEKTIV_FIXED " : /* EN iscrys ? "fixed " : */
		       is_rustprone(obj) ? "ADJEKTIV_RUSTPROOF " : /* EN is_rustprone(obj) ? "rustproof " : */
		       is_corrodeable(obj) ? "ADJEKTIV_CORRODEPROOF " :	/* "stainless"? */ /* EN is_corrodeable(obj) ? "corrodeproof " :	*/
		       is_flammable(obj) ? "ADJEKTIV_FIREPROOF " : ""); /* EN is_flammable(obj) ? "fireproof " : ""); */
}

static char *
doname_base(obj, with_price)
register struct obj *obj;
boolean with_price;
{
	boolean ispoisoned = FALSE;
	char prefix[PREFIX];
	char tmpbuf[PREFIX+1];
	/* when we have to add something at the start of prefix instead of the
	 * end (Strcat is used on the end)
	 */
	register char *bp = xname(obj);

	/* When using xname, we want "poisoned arrow", and when using
	 * doname, we want "poisoned +0 arrow".  This kludge is about the only
	 * way to do it, at least until someone overhauls xname() and doname(),
	 * combining both into one function taking a parameter.
	 */
	/* must check opoisoned--someone can have a weirdly-named fruit */
	if (!strncmp(bp, "ADJEKTIV_POISONED ", 18) && obj->opoisoned) { /* EN if (!strncmp(bp, "poisoned ", 9) && obj->opoisoned) { */
		bp += 18; /* EN bp += 9; */
		ispoisoned = TRUE;
	}

	if(obj->quan != 1L)
		Sprintf(prefix, "%ld ", obj->quan);
	else if (obj_is_pname(obj) || the_unique_obj(obj)) {
		if (!strncmpi(bp, "ARTIKEL_BESTIMMTER ", 19))
		    bp += 19;
		Strcpy(prefix, "ARTIKEL_BESTIMMTER ");
	} else
		Strcpy(prefix, "ARTIKEL_UNBESTIMMTER ");

#ifdef INVISIBLE_OBJECTS
	if (obj->oinvis) Strcat(prefix,"ADJEKTIV_INVISIBLE "); /* EN if (obj->oinvis) Strcat(prefix,"invisible "); */
#endif

	if (obj->bknown &&
	    obj->oclass != COIN_CLASS &&
	    (obj->otyp != POT_WATER || !objects[POT_WATER].oc_name_known
		|| (!obj->cursed && !obj->blessed))) {
	    /* allow 'blessed clear potion' if we don't know it's holy water;
	     * always allow "uncursed potion of water"
	     */
	    if (obj->cursed)
		Strcat(prefix, "ADJEKTIV_CURSED ");
	    else if (obj->blessed)
		Strcat(prefix, "ADJEKTIV_BLESSED ");
	    else if ((!obj->known || !objects[obj->otyp].oc_charged ||
		      (obj->oclass == ARMOR_CLASS ||
		       obj->oclass == RING_CLASS))
		/* For most items with charges or +/-, if you know how many
		 * charges are left or what the +/- is, then you must have
		 * totally identified the item, so "uncursed" is unneccesary,
		 * because an identified object not described as "blessed" or
		 * "cursed" must be uncursed.
		 *
		 * If the charges or +/- is not known, "uncursed" must be
		 * printed to avoid ambiguity between an item whose curse
		 * status is unknown, and an item known to be uncursed.
		 */
#ifdef MAIL
			&& obj->otyp != SCR_MAIL
#endif
			&& obj->otyp != FAKE_AMULET_OF_YENDOR
			&& obj->otyp != AMULET_OF_YENDOR
			&& !Role_if(PM_PRIEST))
		Strcat(prefix, "ADJEKTIV_UNCURSED ");
	}

	if (obj->greased) Strcat(prefix, "ADJEKTIV_GREASED "); /* EN if (obj->greased) Strcat(prefix, "greased "); */

	switch(obj->oclass) {
	case AMULET_CLASS:
		if(obj->owornmask & W_AMUL)
			Strcat(bp, " (ADJEKTIV_GETRAGEN)"); /* EN Strcat(bp, " (being worn)"); */
		break;
	case WEAPON_CLASS:
		if(ispoisoned)
			Strcat(prefix, "ADJEKTIV_POISONED "); /* EN Strcat(prefix, "poisoned "); */
plus:
		add_erosion_words(obj, prefix);
		if(obj->known) {
			Strcat(prefix, sitoa(obj->spe));
			Strcat(prefix, " ");
		}
		break;
	case ARMOR_CLASS:
		if(obj->owornmask & W_ARMOR)
			Strcat(bp, (obj == uskin) ? german(" (KASUS_DATIV in PRONOMEN_POSSESSIV NOUN_HAUT eingebettet)") : /* EN Strcat(bp, (obj == uskin) ? " (embedded in your skin)" : */
				" (ADJEKTIV_GETRAGEN)");
		goto plus;
	case TOOL_CLASS:
		/* weptools already get this done when we go to the +n code */
		if (!is_weptool(obj))
		    add_erosion_words(obj, prefix);
		if(obj->owornmask & (W_TOOL /* blindfold */
#ifdef STEED
				| W_SADDLE
#endif
				)) {
			Strcat(bp, " (ADJEKTIV_GETRAGEN)");
			break;
		}
		if (obj->otyp == LEASH && obj->leashmon != 0) {
			Strcat(bp, " (in Benutzung)"); /* EN Strcat(bp, " (in use)"); */
			break;
		}
		if (is_weptool(obj))
			goto plus;
		if (obj->otyp == CANDELABRUM_OF_INVOCATION) {
			if (!obj->spe)
			    Strcpy(tmpbuf, "PRONOMEN_KEIN"); /* EN Strcpy(tmpbuf, "no"); */
			else
			    Sprintf(tmpbuf, "%d", obj->spe);
			Sprintf(eos(bp), " (NEUER_SATZ SUBJECT_IM_SATZ %s NOUN_CANDLE%s%s)", /* EN Sprintf(eos(bp), " (%s candle%s%s)", */
				tmpbuf, plur(obj->spe),
				!obj->lamplit ? " befestigt" : ", entz�ndet"); /* EN !obj->lamplit ? " attached" : ", lit"); */
			break;
		} else if (obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
			obj->otyp == BRASS_LANTERN || Is_candle(obj)) {
			if (Is_candle(obj) &&
			    obj->age < 20L * (long)objects[obj->otyp].oc_cost)
				Strcat(prefix, "partly used "); /* EN Strcat(prefix, "partly used "); */ // TODO DE
			if(obj->lamplit)
				Strcat(bp, " (ADJEKTIV_LIT)"); /* EN Strcat(bp, " (lit)"); */
			break;
		}
		if(objects[obj->otyp].oc_charged)
		    goto charges;
		break;
	case WAND_CLASS:
		add_erosion_words(obj, prefix);
charges:
		if(obj->known)
		    Sprintf(eos(bp), " (%d:%d)", (int)obj->recharged, obj->spe);
		break;
	case POTION_CLASS:
		if (obj->otyp == POT_OIL && obj->lamplit)
		    Strcat(bp, " (ADJEKTIV_LIT)"); /* EN Strcat(bp, " (lit)"); */
		break;
	case RING_CLASS:
		add_erosion_words(obj, prefix);
ring:
		if(obj->owornmask & W_RINGR) Strcat(bp, " (KASUS_DATIV _an_ ARTIKEL_BESTIMMTER ADJEKTIV_RECHT "); /* EN if(obj->owornmask & W_RINGR) Strcat(bp, " (on right "); */ // TODO DE
		if(obj->owornmask & W_RINGL) Strcat(bp, " (KASUS_DATIV _an_ ARTIKEL_BESTIMMTER ADJEKTIV_LINK "); /* EN if(obj->owornmask & W_RINGL) Strcat(bp, " (on left "); */ // TODO DE
		if(obj->owornmask & W_RING) {
		    Strcat(bp, body_part(HAND));
		    Strcat(bp, ")");
		}
		if(obj->known && objects[obj->otyp].oc_charged) {
			Strcat(prefix, sitoa(obj->spe));
			Strcat(prefix, " ");
		}
		break;
	case FOOD_CLASS:
		if (obj->oeaten)
		    Strcat(prefix, "halb ADJEKTIV_EATEN "); /* EN Strcat(prefix, "partly eaten "); */
		if (obj->otyp == CORPSE) {
		    if (mons[obj->corpsenm].geno & G_UNIQ) {
			Sprintf(prefix, "MODIFIER_CORPSE %s %s %s", /* EN Sprintf(prefix, "%s%s ", */
				(type_is_pname(&mons[obj->corpsenm]) ?
					"MODIFIER_EIGENNAME" : "ARTIKEL_BESTIMMTER"), /* EN "" : "the "), */
				mons[obj->corpsenm].mname, /* EN s_suffix(mons[obj->corpsenm].mname)); */
				(obj->oeaten) ? "halb ADJEKTIV_EATEN " : "");
			 /* EN if (obj->oeaten) Strcat(prefix, "partly eaten "); */
		    } else {
			Strcat(prefix, "MODIFIER_CORPSE "); /* EN */
			Strcat(prefix, mons[obj->corpsenm].mname);
			Strcat(prefix, " ");
		    }
		} else if (obj->otyp == EGG) {
#if 0	/* corpses don't tell if they're stale either */
		    if (obj->known && stale_egg(obj))
			Strcat(prefix, "stale ");
#endif
		    if (obj->corpsenm >= LOW_PM &&
			    (obj->known ||
			    mvitals[obj->corpsenm].mvflags & MV_KNOWS_EGG)) {
			Strcat(prefix, fugenwort(mons[obj->corpsenm].mname)); /* EN Strcat(prefix, mons[obj->corpsenm].mname); */
			Strcat(prefix, "--"); /* EN Strcat(prefix, " "); */
			if (obj->spe)
			    Strcat(bp, " (selbstgelegt)"); /* EN Strcat(bp, " (laid by you)"); */
		    }
		}
		if (obj->otyp == MEAT_RING) goto ring;
		break;
	case BALL_CLASS:
	case CHAIN_CLASS:
		add_erosion_words(obj, prefix);
		if(obj->owornmask & W_BALL)
			Strcat(bp, german(" (an KASUS_AKKUSATIV PRONOMEN_PERSONAL gekettet)")); /* EN Strcat(bp, " (chained to you)"); */
			break;
	}

	if((obj->owornmask & W_WEP) && !mrg_to_wielded) {
		if (obj->quan != 1L) {
			Strcat(bp, " (gef�hrt)"); /* EN Strcat(bp, " (wielded)"); */
		} else {
			const char *hand_s = body_part(HAND);

			if (bimanual(obj)) hand_s = makeplural(hand_s);
			Sprintf(eos(bp), " (KASUS_DATIV in Waffen-%s gef�hrt)", hand_s); /* EN Sprintf(eos(bp), " (weapon in %s)", hand_s); */
		}
	}
	if(obj->owornmask & W_SWAPWEP) {
		if (u.twoweap)
			Sprintf(eos(bp), " (KASUS_DATIV in Neben-%s gef�hrt)", /* EN Sprintf(eos(bp), " (wielded in other %s)", */
				body_part(HAND));
		else
			Strcat(bp, " (Zweitwaffe; nicht gef�hrt)");
	}
	if(obj->owornmask & W_QUIVER) Strcat(bp, " (im K�cher)");
	if(obj->unpaid) {
		xchar ox, oy; 
		long quotedprice = unpaid_cost(obj);
		struct monst *shkp = (struct monst *)0;

		if (Has_contents(obj) &&
		    get_obj_location(obj, &ox, &oy, BURIED_TOO|CONTAINED_TOO) &&
		    costly_spot(ox, oy) &&
		    (shkp = shop_keeper(*in_rooms(ox, oy, SHOPBASE))))
			quotedprice += contained_cost(obj, shkp, 0L, FALSE, TRUE);
		Sprintf(eos(bp), " (unbezahlt, %ld %s)", /* EN Sprintf(eos(bp), " (unpaid, %ld %s)", */
			quotedprice, currency(quotedprice));
	} else if (with_price) {
		long price = get_cost_of_shop_item(obj);
		if (price > 0) {
			Sprintf(eos(bp), " (%ld %s)", price, currency(price));
		}
	}
#ifndef GERMAN
	if (!strncmp(prefix, "a ", 2) &&
			index(vowels, *(prefix+2) ? *(prefix+2) : *bp)
			&& (*(prefix+2) || (strncmp(bp, "uranium", 7)
				&& strncmp(bp, "unicorn", 7)
				&& strncmp(bp, "eucalyptus", 10)))) {
		Strcpy(tmpbuf, prefix);
		Strcpy(prefix, "an ");
		Strcpy(prefix+3, tmpbuf+2);
	}
#endif
	bp = strprepend(bp, prefix);
	return(bp);
}

#endif /* OVL0 */
#ifdef OVLB

/** Wrapper function for vanilla behaviour. */
char *
doname(obj)
register struct obj *obj;
{
	return doname_base(obj, FALSE);
}

/** Name of object including price. */
char *
doname_with_price(obj)
register struct obj *obj;
{
	return doname_base(obj, TRUE);
}

/* used from invent.c */
boolean
not_fully_identified(otmp)
register struct obj *otmp;
{
#ifdef GOLDOBJ
    /* gold doesn't have any interesting attributes [yet?] */
    if (otmp->oclass == COIN_CLASS) return FALSE;	/* always fully ID'd */
#endif
    /* check fundamental ID hallmarks first */
    if (!otmp->known || !otmp->dknown ||
#ifdef MAIL
	    (!otmp->bknown && otmp->otyp != SCR_MAIL) ||
#else
	    !otmp->bknown ||
#endif
	    !objects[otmp->otyp].oc_name_known)	/* ?redundant? */
	return TRUE;
    if (otmp->oartifact && undiscovered_artifact(otmp->oartifact))
	return TRUE;
    /* otmp->rknown is the only item of interest if we reach here */
       /*
	*  Note:  if a revision ever allows scrolls to become fireproof or
	*  rings to become shockproof, this checking will need to be revised.
	*  `rknown' ID only matters if xname() will provide the info about it.
	*/
    if (otmp->rknown || (otmp->oclass != ARMOR_CLASS &&
			 otmp->oclass != WEAPON_CLASS &&
			 !is_weptool(otmp) &&		    /* (redunant) */
			 otmp->oclass != BALL_CLASS))	    /* (useless) */
	return FALSE;
    else	/* lack of `rknown' only matters for vulnerable objects */
	return (boolean)(is_rustprone(otmp) ||
			 is_corrodeable(otmp) ||
			 is_flammable(otmp));
}

char *
corpse_xname(otmp, ignore_oquan)
struct obj *otmp;
boolean ignore_oquan;	/* to force singular */
{
	char *nambuf = nextobuf();

	Sprintf(nambuf, "MODIFIER_CORPSE %s NOUN_CORPSE", mons[otmp->corpsenm].mname); /* EN Sprintf(nambuf, "%s corpse", mons[otmp->corpsenm].mname); */
	if (ignore_oquan || otmp->quan < 2)
	    return nambuf;
	else
	    return makeplural(nambuf);
}

/* xname, unless it's a corpse, then corpse_xname(obj, FALSE) */
char *
cxname(obj)
struct obj *obj;
{
	if (obj->otyp == CORPSE)
	    return corpse_xname(obj, FALSE);
	return xname(obj);
}

/* treat an object as fully ID'd when it might be used as reason for death */
char *
killer_xname(obj)
struct obj *obj;
{
    struct obj save_obj;
    unsigned save_ocknown;
    char *buf, *save_ocuname;

    /* remember original settings for core of the object;
       oname and oattached extensions don't matter here--since they
       aren't modified they don't need to be saved and restored */
    save_obj = *obj;
    /* killer name should be more specific than general xname; however, exact
       info like blessed/cursed and rustproof makes things be too verbose */
    obj->known = obj->dknown = 1;
    obj->bknown = obj->rknown = obj->greased = 0;
    /* if character is a priest[ess], bknown will get toggled back on */
    obj->blessed = obj->cursed = 0;
    /* "killed by poisoned <obj>" would be misleading when poison is
       not the cause of death and "poisoned by poisoned <obj>" would
       be redundant when it is, so suppress "poisoned" prefix */
    obj->opoisoned = 0;
    /* strip user-supplied name; artifacts keep theirs */
    if (!obj->oartifact) obj->onamelth = 0;
    /* temporarily identify the type of object */
    save_ocknown = objects[obj->otyp].oc_name_known;
    objects[obj->otyp].oc_name_known = 1;
    save_ocuname = objects[obj->otyp].oc_uname;
    objects[obj->otyp].oc_uname = 0;	/* avoid "foo called bar" */

    buf = xname(obj);
    if (obj->quan == 1L) buf = obj_is_pname(obj) ? the(buf) : an(buf);

    objects[obj->otyp].oc_name_known = save_ocknown;
    objects[obj->otyp].oc_uname = save_ocuname;
    *obj = save_obj;	/* restore object's core settings */

    return buf;
}

/*
 * Used if only one of a collection of objects is named (e.g. in eat.c).
 */
const char *
singular(otmp, func)
register struct obj *otmp;
char *FDECL((*func), (OBJ_P));
{
	long savequan;
	char *nam;

	/* Note: using xname for corpses will not give the monster type */
	if (otmp->otyp == CORPSE && func == xname)
		return corpse_xname(otmp, TRUE);

	savequan = otmp->quan;
	otmp->quan = 1L;
	nam = (*func)(otmp);
	otmp->quan = savequan;
	return nam;
}

char *
an(str)
register const char *str;
{
	char *buf = nextobuf();

	buf[0] = '\0';

	if (strncmpi(str, "ARTIKEL_BESTIMMTER ", 19) && /* EN if (strncmpi(str, "the ", 4) && */
	    strcmp(str, "NOUN_MOLTEN_LAVA") && /* EN strcmp(str, "molten lava") && */
	    strcmp(str, "NOUN_IRON_BARs") && /* EN strcmp(str, "iron bars") && */
	    strcmp(str, "NOUN_ICE")) { /* EN strcmp(str, "ice")) { */
	#ifndef GERMAN
		if (index(vowels, *str) &&
		    strncmp(str, "one-", 4) && 
		    strncmp(str, "useful", 6) && 
		    strncmp(str, "unicorn", 7) &&
		    strncmp(str, "uranium", 7) &&
		    strncmp(str, "eucalyptus", 10))
			Strcpy(buf, "an ");
		else
	#endif
			Strcpy(buf, "ARTIKEL_UNBESTIMMTER "); /* EN Strcpy(buf, "a "); */
	}

	Strcat(buf, str);
	return buf;
}

char *
An(str)
const char *str;
{
	register char *tmp = an(str);
	*tmp = highc(*tmp);
	return tmp;
}

/*
 * Prepend "the" if necessary; assumes str is a subject derived from xname.
 * Use type_is_pname() for monster names, not the().  the() is idempotent.
 */
char *
the(str)
const char *str;
{
	char *buf = nextobuf();
	boolean insert_the = FALSE;

#ifdef GERMAN
	if (!strncmpi(str, "ARTIKEL_BESTIMMTER ", 19)) {
		insert_the = FALSE;
	} else {
		insert_the = german_the(str);
	}
#else
	if (!strncmpi(str, "ARTIKEL_BESTIMMTER ", 19)) {
	    buf[0] = lowc(*str);
	    Strcpy(&buf[1], str+1);
	    return buf;
	} else if (*str < 'A' || *str > 'Z') {
	    /* not a proper name, needs an article */
	    insert_the = TRUE;
	} else {
	    /* Probably a proper name, might not need an article */
	    register char *tmp, *named, *called;
	    int l;

	    /* some objects have capitalized adjectives in their names */
	    if(((tmp = rindex(str, ' ')) || (tmp = rindex(str, '-'))) &&
	       (tmp[1] < 'A' || tmp[1] > 'Z'))
		insert_the = TRUE;
	    else if (tmp && index(str, ' ') < tmp) {	/* has spaces */
		/* it needs an article if the name contains "of" */
		tmp = strstri(str, " PARTIKEL_OF ");
		named = strstri(str, " PARTIKEL_NAMED ");
		called = strstri(str, " PARTIKEL_CALLED ");
		if (called && (!named || called < named)) named = called;

		if (tmp && (!named || tmp < named))	/* found an "of" */
		    insert_the = TRUE;
		/* stupid special case: lacks "of" but needs "the" */
		else if (!named && (l = strlen(str)) >= 31 &&
		      !strcmp(&str[l - 31], "Platinum Yendorian Express Card")) /* EN !strcmp(&str[l - 31], "Platinum Yendorian Express Card")) */ // TODO DE
		    insert_the = TRUE;
	    }
	}
#endif
	if (insert_the)
	    Strcpy(buf, "ARTIKEL_BESTIMMTER ");
	else
	    buf[0] = '\0';
	Strcat(buf, str);

	return buf;
}

char *
The(str)
const char *str;
{
    register char *tmp = the(str);
    *tmp = highc(*tmp);
    return tmp;
}

/* returns "count cxname(otmp)" or just cxname(otmp) if count == 1 */
char *
aobjnam(otmp,verb)
register struct obj *otmp;
register const char *verb;
{
	register char *bp = cxname(otmp);
	char prefix[PREFIX];

	if(otmp->quan != 1L) {
		Sprintf(prefix, "%ld ", otmp->quan);
		bp = strprepend(bp, prefix);
	}

	if(verb) {
	    Strcat(bp, " ");
	    Strcat(bp, otense(otmp, verb));
	}
	return(bp);
}

/* like aobjnam, but prepend "The", not count, and use xname */
char *
Tobjnam(otmp, verb)
register struct obj *otmp;
register const char *verb;
{
	char *bp = The(xname(otmp));

	if(verb) {
	    Strcat(bp, " ");
	    Strcat(bp, otense(otmp, verb));
	}
	return(bp);
}

/* return form of the verb (input plural) if xname(otmp) were the subject */
char *
otense(otmp, verb)
register struct obj *otmp;
register const char *verb;
{
	char *buf;

	/*
	 * verb is given in plural (without trailing s).  Return as input
	 * if the result of xname(otmp) would be plural.  Don't bother
	 * recomputing xname(otmp) at this time.
	 */
	if (!is_plural(otmp))
	    return vtense((char *)0, verb);

	buf = nextobuf();
	Strcpy(buf, verb);
	return buf;
}

/* various singular words that vtense would otherwise categorize as plural */
static const char * const special_subjs[] = {
// TODO DE ?
	"erinys",
	"manes",		/* this one is ambiguous */
	"Cyclops",
	"Hippocrates",
	"Pelias",
	"aklys",
	"amnesia",
	"paralysis",
	0
};

/* return form of the verb (input plural) for present tense 3rd person subj */
char *
vtense(subj, verb)
register const char *subj;
register const char *verb;
{
	char *buf = nextobuf();
	int len, ltmp;
	const char *sp, *spot;
	const char * const *spec;

#ifdef GERMAN
		return strcpy(buf, verb);
#else
	/*
	 * verb is given in plural (without trailing s).  Return as input
	 * if subj appears to be plural.  Add special cases as necessary.
	 * Many hard cases can already be handled by using otense() instead.
	 * If this gets much bigger, consider decomposing makeplural.
	 * Note: monster names are not expected here (except before corpse).
	 *
	 * special case: allow null sobj to get the singular 3rd person
	 * present tense form so we don't duplicate this code elsewhere.
	 */
	if (subj) {
	    if (!strncmpi(subj, "ARTIKEL_UNBESTIMMTER ", 2) || !strncmpi(subj, "ARTIKEL_UNBESTIMMTER ", 3))
		goto sing;
	    spot = (const char *)0;
	    for (sp = subj; (sp = index(sp, ' ')) != 0; ++sp) {
		if (!strncmp(sp, " PARTIKEL_OF ", 4) ||
		    !strncmp(sp, " from ", 6) ||
		    !strncmp(sp, " PARTIKEL_CALLED ", 8) ||
		    !strncmp(sp, " PARTIKEL_NAMED ", 7) ||
		    !strncmp(sp, " labeled ", 9)) {
		    if (sp != subj) spot = sp - 1;
		    break;
		}
	    }
	    len = (int) strlen(subj);
	    if (!spot) spot = subj + len - 1;

	    /*
	     * plural: anything that ends in 's', but not '*us' or '*ss'.
	     * Guess at a few other special cases that makeplural creates.
	     */
	    if ((*spot == 's' && spot != subj &&
			(*(spot-1) != 'u' && *(spot-1) != 's')) ||
		((spot - subj) >= 4 && !strncmp(spot-3, "eeth", 4)) ||
		((spot - subj) >= 3 && !strncmp(spot-3, "feet", 4)) ||
		((spot - subj) >= 2 && !strncmp(spot-1, "ia", 2)) ||
		((spot - subj) >= 2 && !strncmp(spot-1, "ae", 2))) {
		/* check for special cases to avoid false matches */
		len = (int)(spot - subj) + 1;
		for (spec = special_subjs; *spec; spec++) {
		    ltmp = strlen(*spec);
		    if (len == ltmp && !strncmpi(*spec, subj, len)) goto sing;
		    /* also check for <prefix><space><special_subj>
		       to catch things like "the invisible erinys" */
		    if (len > ltmp && *(spot - ltmp) == ' ' &&
			   !strncmpi(*spec, spot - ltmp + 1, ltmp)) goto sing;
		}

		return strcpy(buf, verb);
	    }
	    /*
	     * 3rd person plural doesn't end in telltale 's';
	     * 2nd person singular behaves as if plural.
	     */
	    if (!strcmpi(subj, "they") || !strcmpi(subj, "you"))
		return strcpy(buf, verb);
	}

 sing:
	len = strlen(verb);
	spot = verb + len - 1;

	if (!strcmp(verb, "are"))
	    Strcpy(buf, "is");
	else if (!strcmp(verb, "have"))
	    Strcpy(buf, "has");
	else if (index("zxs", *spot) ||
		 (len >= 2 && *spot=='h' && index("cs", *(spot-1))) ||
		 (len == 2 && *spot == 'o')) {
	    /* Ends in z, x, s, ch, sh; add an "es" */
	    Strcpy(buf, verb);
	    Strcat(buf, "es");
	} else if (*spot == 'y' && (!index(vowels, *(spot-1)))) {
	    /* like "y" case in makeplural */
	    Strcpy(buf, verb);
	    Strcpy(buf + len - 1, "ies");
	} else {
	    Strcpy(buf, verb);
	    Strcat(buf, "s");
	}
#endif

	return buf;
}

/* capitalized variant of doname() */
char *
Doname2(obj)
register struct obj *obj;
{
	register char *s = doname(obj);

	*s = highc(*s);
	return(s);
}

/* returns "your xname(obj)" or "Foobar's xname(obj)" or "the xname(obj)" */
char *
yname(obj)
struct obj *obj;
{
	char *outbuf = nextobuf();
	char *s = shk_your(outbuf, obj);	/* assert( s == outbuf ); */
	int space_left = BUFSZ - strlen(s) - sizeof " ";

	return strncat(strcat(s, " "), cxname(obj), space_left);
}

/* capitalized variant of yname() */
char *
Yname2(obj)
struct obj *obj;
{
	char *s = yname(obj);

	*s = highc(*s);
	return s;
}

/* returns "your simple_typename(obj->otyp)"
 * or "Foobar's simple_typename(obj->otyp)"
 * or "the simple_typename(obj-otyp)"
 */
char *
ysimple_name(obj)
struct obj *obj;
{
	char *outbuf = nextobuf();
	char *s = shk_your(outbuf, obj);	/* assert( s == outbuf ); */
	int space_left = BUFSZ - strlen(s) - sizeof " ";

	return strncat(strcat(s, " "), simple_typename(obj->otyp), space_left);
}

/* capitalized variant of ysimple_name() */
char *
Ysimple_name2(obj)
struct obj *obj;
{
	char *s = ysimple_name(obj);

	*s = highc(*s);
	return s;
}

static const char *wrp[] = {
	"NOUN_WAND", "NOUN_RING", "NOUN_POTION", "NOUN_SCROLL", "NOUN_GEM", "NOUN_AMULET",
	"NOUN_SPELLBOOK", "NOUN_SPELLBOOK",
	/* for non-specific wishes */
	"NOUN_WEAPON", "NOUN_ARMOR", "NOUN_ARMOR", "NOUN_TOOL", "NOUN_FOOD", "NOUN_FOOD",
};
static const char wrpsym[] = {
	WAND_CLASS, RING_CLASS, POTION_CLASS, SCROLL_CLASS, GEM_CLASS,
	AMULET_CLASS, SPBOOK_CLASS, SPBOOK_CLASS,
	WEAPON_CLASS, ARMOR_CLASS, ARMOR_CLASS, TOOL_CLASS, FOOD_CLASS,
	FOOD_CLASS
};

#endif /* OVLB */
#ifdef OVL0

/* Plural routine; chiefly used for user-defined fruits.  We have to try to
 * account for everything reasonable the player has; something unreasonable
 * can still break the code.  However, it's still a lot more accurate than
 * "just add an s at the end", which Rogue uses...
 *
 * Also used for plural monster names ("Wiped out all homunculi.")
 * and body parts.
 *
 * Also misused by muse.c to convert 1st person present verbs to 2nd person.
 */
char *
makeplural(oldstr)
const char *oldstr;
{
	/* Note: cannot use strcmpi here -- it'd give MATZot, CAVEMeN,... */
	register char *spot;
	char *str = nextobuf();
	const char *excess = (char *)0;
	int len;

	while (*oldstr==' ') oldstr++;
	if (!oldstr || !*oldstr) {
		impossible("plural of null?");
		Strcpy(str, "s");
		return str;
	}
	Strcpy(str, oldstr);

	/*
	 * Skip changing "pair of" to "pairs of".  According to Webster, usual
	 * English usage is use pairs for humans, e.g. 3 pairs of dancers,
	 * and pair for objects and non-humans, e.g. 3 pair of boots.  We don't
	 * refer to pairs of humans in this game so just skip to the bottom.
	 */
	if (!strncmp(str, "pair of ", 8))
		goto bottom;

	/* Search for common compounds, ex. lump of royal jelly */
	for(spot=str; *spot; spot++) {
		if (!strncmp(spot, " of ", 4)
				|| !strncmp(spot, " labeled ", 9)
				|| !strncmp(spot, " PARTIKEL_CALLED ", 8)
				|| !strncmp(spot, " PARTIKEL_NAMED ", 7)
				|| !strcmp(spot, " above") /* lurkers above */
				|| !strncmp(spot, " versus ", 8)
				|| !strncmp(spot, " from ", 6)
				|| !strncmp(spot, " in ", 4)
				|| !strncmp(spot, " on ", 4)
				|| !strncmp(spot, " a la ", 6)
				|| !strncmp(spot, " with", 5)	/* " with "? */
				|| !strncmp(spot, " de ", 4)
				|| !strncmp(spot, " d'", 3)
				|| !strncmp(spot, " du ", 4)) {
			excess = oldstr + (int) (spot - str);
			*spot = 0;
			break;
		}
	}
	spot--;
	while (*spot==' ') spot--; /* Strip blanks from end */
	*(spot+1) = 0;
	/* Now spot is the last character of the string */

	len = strlen(str);

	/* Single letters */
	if (len==1 || !letter(*spot)) {
		Strcpy(spot+1, "'s");
		goto bottom;
	}

	/* Same singular and plural; mostly Japanese words except for "manes" */
	if ((len == 2 && !strcmp(str, "ya")) ||
	    (len >= 2 && !strcmp(spot-1, "ai")) || /* samurai, Uruk-hai */
	    (len >= 3 && !strcmp(spot-2, " ya")) ||
	    (len >= 4 &&
	     (!strcmp(spot-3, "fish") || !strcmp(spot-3, "tuna") ||
	      !strcmp(spot-3, "deer") || !strcmp(spot-3, "yaki"))) ||
	    (len >= 5 && (!strcmp(spot-4, "sheep") ||
			!strcmp(spot-4, "ninja") ||
			!strcmp(spot-4, "ronin") ||
			!strcmp(spot-4, "shito") ||
			!strcmp(spot-7, "shuriken") ||
			!strcmp(spot-4, "tengu") ||
			!strcmp(spot-4, "manes"))) ||
	    (len >= 6 && !strcmp(spot-5, "ki-rin")) ||
	    (len >= 7 && !strcmp(spot-6, "gunyoki")))
		goto bottom;

	/* man/men ("Wiped out all cavemen.") */
	if (len >= 3 && !strcmp(spot-2, "man") &&
			(len<6 || strcmp(spot-5, "shaman")) &&
			(len<5 || strcmp(spot-4, "human"))) {
		*(spot-1) = 'e';
		goto bottom;
	}

	/* tooth/teeth */
	if (len >= 5 && !strcmp(spot-4, "tooth")) {
		Strcpy(spot-3, "eeth");
		goto bottom;
	}

	/* knife/knives, etc... */
	if (!strcmp(spot-1, "fe")) {
		Strcpy(spot-1, "ves");
		goto bottom;
	} else if (*spot == 'f') {
		if (index("lr", *(spot-1)) || index(vowels, *(spot-1))) {
			Strcpy(spot, "ves");
			goto bottom;
		} else if (len >= 5 && !strncmp(spot-4, "staf", 4)) {
			Strcpy(spot-1, "ves");
			goto bottom;
		}
	}

	/* foot/feet (body part) */
	if (len >= 4 && !strcmp(spot-3, "foot")) {
		Strcpy(spot-2, "eet");
		goto bottom;
	}

	/* ium/ia (mycelia, baluchitheria) */
	if (len >= 3 && !strcmp(spot-2, "ium")) {
		*(spot--) = (char)0;
		*spot = 'a';
		goto bottom;
	}

	/* algae, larvae, hyphae (another fungus part) */
	if ((len >= 4 && !strcmp(spot-3, "alga")) ||
	    (len >= 5 &&
	     (!strcmp(spot-4, "hypha") || !strcmp(spot-4, "larva")))) {
		Strcpy(spot, "ae");
		goto bottom;
	}

	/* fungus/fungi, homunculus/homunculi, but buses, lotuses, wumpuses */
	if (len > 3 && !strcmp(spot-1, "us") &&
	    (len < 5 || (strcmp(spot-4, "lotus") &&
			 (len < 6 || strcmp(spot-5, "wumpus"))))) {
		*(spot--) = (char)0;
		*spot = 'i';
		goto bottom;
	}

	/* vortex/vortices */
	if (len >= 6 && !strcmp(spot-3, "rtex")) {
		Strcpy(spot-1, "ices");
		goto bottom;
	}

	/* djinni/djinn (note: also efreeti/efreet) */
	if (len >= 6 && !strcmp(spot-5, "djinni")) {
		*spot = (char)0;
		goto bottom;
	}

	/* mumak/mumakil */
	if (len >= 5 && !strcmp(spot-4, "mumak")) {
		Strcpy(spot+1, "il");
		goto bottom;
	}

	/* sis/ses (nemesis) */
	if (len >= 3 && !strcmp(spot-2, "sis")) {
		*(spot-1) = 'e';
		goto bottom;
	}

	/* erinys/erinyes */
	if (len >= 6 && !strcmp(spot-5, "erinys")) {
		Strcpy(spot, "es");
		goto bottom;
	}

	/* mouse/mice,louse/lice (not a monster, but possible in food names) */
	if (len >= 5 && !strcmp(spot-3, "ouse") && index("MmLl", *(spot-4))) {
		Strcpy(spot-3, "ice");
		goto bottom;
	}

	/* matzoh/matzot, possible food name */
	if (len >= 6 && (!strcmp(spot-5, "matzoh")
					|| !strcmp(spot-5, "matzah"))) {
		Strcpy(spot-1, "ot");
		goto bottom;
	}
	if (len >= 5 && (!strcmp(spot-4, "matzo")
					|| !strcmp(spot-5, "matza"))) {
		Strcpy(spot, "ot");
		goto bottom;
	}

	/* child/children (for wise guys who give their food funny names) */
	if (len >= 5 && !strcmp(spot-4, "child")) {
		Strcpy(spot, "dren");
		goto bottom;
	}

	/* note: -eau/-eaux (gateau, bordeau...) */
	/* note: ox/oxen, VAX/VAXen, goose/geese */

	/* Ends in z, x, s, ch, sh; add an "es" */
	if (index("zxs", *spot)
			|| (len >= 2 && *spot=='h' && index("cs", *(spot-1)))
	/* Kludge to get "tomatoes" and "potatoes" right */
			|| (len >= 4 && !strcmp(spot-2, "ato"))) {
		Strcpy(spot+1, "es");
		goto bottom;
	}

	/* Ends in y preceded by consonant (note: also "qu") change to "ies" */
	if (*spot == 'y' &&
	    (!index(vowels, *(spot-1)))) {
		Strcpy(spot, "ies");
		goto bottom;
	}

	/* Default: append an 's' */
	Strcpy(spot+1, "s");

bottom:	if (excess) Strcpy(eos(str), excess);
	return str;
}

#endif /* OVL0 */

struct o_range {
	const char *name, oclass;
	int  f_o_range, l_o_range;
};

#ifndef OVLB

STATIC_DCL const struct o_range o_ranges[];

#else /* OVLB */

/* wishable subranges of objects */
STATIC_OVL NEARDATA const struct o_range o_ranges[] = {
// TODO DE
	{ "NOUN_BAG",	TOOL_CLASS,   SACK,	      BAG_OF_TRICKS },
	{ "NOUN_LAMP",	TOOL_CLASS,   OIL_LAMP,	      MAGIC_LAMP },
	{ "NOUN_CANDLE",	TOOL_CLASS,   TALLOW_CANDLE,  WAX_CANDLE },
	{ "NOUN_HORN",	TOOL_CLASS,   TOOLED_HORN,    HORN_OF_PLENTY },
	{ "NOUN_SHIELD",	ARMOR_CLASS,  SMALL_SHIELD,   SHIELD_OF_REFLECTION },
	{ "NOUN_HELMET",	ARMOR_CLASS,  ELVEN_LEATHER_HELM, HELM_OF_TELEPATHY },
	{ "NOUN_GLOVES",	ARMOR_CLASS,  LEATHER_GLOVES, GAUNTLETS_OF_DEXTERITY },
	{ "gauntlets",	ARMOR_CLASS,  LEATHER_GLOVES, GAUNTLETS_OF_DEXTERITY },
	{ "NOUN_BOOTS",	ARMOR_CLASS,  LOW_BOOTS,      LEVITATION_BOOTS },
	{ "shoes",	ARMOR_CLASS,  LOW_BOOTS,      IRON_SHOES },
	{ "NOUN_CLOAK",	ARMOR_CLASS,  MUMMY_WRAPPING, CLOAK_OF_DISPLACEMENT },
#ifdef TOURIST
	{ "shirt",	ARMOR_CLASS,  HAWAIIAN_SHIRT, T_SHIRT },
#endif
	{ "dragon scales",
			ARMOR_CLASS,  GRAY_DRAGON_SCALES, YELLOW_DRAGON_SCALES },
	{ "dragon scale mail",
			ARMOR_CLASS,  GRAY_DRAGON_SCALE_MAIL, YELLOW_DRAGON_SCALE_MAIL },
	{ "NOUN_SWORD",	WEAPON_CLASS, SHORT_SWORD,    KATANA },
#ifdef WIZARD
	{ "NOUN_SCHLANGENGIFT",	VENOM_CLASS,  BLINDING_VENOM, ACID_VENOM }, /* EN { "venom",	VENOM_CLASS,  BLINDING_VENOM, ACID_VENOM }, */
#endif
	{ "gray stone",	GEM_CLASS,    LUCKSTONE,      FLINT },
	{ "grey stone",	GEM_CLASS,    LUCKSTONE,      FLINT },
};

#define BSTRCMP(base,ptr,string) ((ptr) < base || strcmp((ptr),string))
#define BSTRCMPI(base,ptr,string) ((ptr) < base || strcmpi((ptr),string))
#define BSTRNCMP(base,ptr,string,num) ((ptr)<base || strncmp((ptr),string,num))
#define BSTRNCMPI(base,ptr,string,num) ((ptr)<base||strncmpi((ptr),string,num))

/*
 * Singularize a string the user typed in; this helps reduce the complexity
 * of readobjnam, and is also used in pager.c to singularize the string
 * for which help is sought.
 */
char *
makesingular(oldstr)
const char *oldstr;
{
	register char *p, *bp;
	char *str = nextobuf();

	if (!oldstr || !*oldstr) {
		impossible("singular of null?");
		str[0] = 0;
		return str;
	}
	Strcpy(str, oldstr);
	bp = str;

	while (*bp == ' ') bp++;
	/* find "cloves of garlic", "worthless pieces of blue glass" */
	if ((p = strstri(bp, "s PARTIKEL_OF ")) != 0) {
	    /* but don't singularize "gauntlets", "boots", "Eyes of the.." */
	    if (BSTRNCMPI(bp, p-3, "Eye", 3) &&
		BSTRNCMP(bp, p-4, "boot", 4) &&
		BSTRNCMP(bp, p-8, "gauntlet", 8))
		while ((*p = *(p+1)) != 0) p++;
	    return bp;
	}

	/* remove -s or -es (boxes) or -ies (rubies) */
	p = eos(bp);
	if (p >= bp+1 && p[-1] == 's') {
		if (p >= bp+2 && p[-2] == 'e') {
			if (p >= bp+3 && p[-3] == 'i') {
				if(!BSTRCMP(bp, p-7, "cookies") ||
				   !BSTRCMP(bp, p-4, "pies"))
					goto mins;
				Strcpy(p-3, "y");
				return bp;
			}

			/* note: cloves / knives from clove / knife */
			if(!BSTRCMP(bp, p-6, "knives")) {
				Strcpy(p-3, "fe");
				return bp;
			}
			if(!BSTRCMP(bp, p-6, "staves")) {
				Strcpy(p-3, "ff");
				return bp;
			}
			if (!BSTRCMPI(bp, p-6, "leaves")) {
				Strcpy(p-3, "f");
				return bp;
			}
			if (!BSTRCMP(bp, p-8, "vortices")) {
				Strcpy(p-4, "ex");
				return bp;
			}

			/* note: nurses, axes but boxes */
			if (!BSTRCMP(bp, p-5, "boxes") ||
			    !BSTRCMP(bp, p-4, "ches")) {
				p[-2] = '\0';
				return bp;
			}

			if (!BSTRCMP(bp, p-6, "gloves") ||
			    !BSTRCMP(bp, p-6, "lenses") ||
			    !BSTRCMP(bp, p-5, "shoes") ||
			    !BSTRCMP(bp, p-6, "scales"))
				return bp;

		} else if (!BSTRCMP(bp, p-5, "boots") ||
			   !BSTRCMP(bp, p-9, "gauntlets") ||
			   !BSTRCMP(bp, p-6, "tricks") ||
			   !BSTRCMP(bp, p-9, "paralysis") ||
			   !BSTRCMP(bp, p-5, "glass") ||
			   !BSTRCMP(bp, p-4, "ness") ||
			   !BSTRCMP(bp, p-14, "shape changers") ||
			   !BSTRCMP(bp, p-15, "detect monsters") ||
			   !BSTRCMPI(bp, p-11, "Aesculapius") || /* staff */
			   !BSTRCMP(bp, p-10, "eucalyptus") ||
#ifdef WIZARD
			   !BSTRCMP(bp, p-9, "iron bars") ||
#endif
			   !BSTRCMP(bp, p-5, "aklys") ||
			   !BSTRCMP(bp, p-6, "fungus"))
				return bp;
	mins:
		p[-1] = '\0';

	} else {

		if(!BSTRCMP(bp, p-5, "teeth")) {
			Strcpy(p-5, "tooth");
			return bp;
		}

		if (!BSTRCMP(bp, p-5, "fungi")) {
			Strcpy(p-5, "fungus");
			return bp;
		}

		/* here we cannot find the plural suffix */
	}
	return bp;
}

/* compare user string against object name string using fuzzy matching */
static boolean
wishymatch(u_str, o_str, retry_inverted)
const char *u_str;	/* from user, so might be variant spelling */
const char *o_str;	/* from objects[], so is in canonical form */
boolean retry_inverted;	/* optional extra "of" handling */
{
	/* special case: wizards can wish for traps.  The object is "beartrap"
	 * and the trap is "bear trap", so to let wizards wish for both we
	 * must not fuzzymatch.
	 */
	//fprintf(stderr, "wishymatch u_str: -%s- o_str: -%s-\n", u_str, o_str); // DE DEBUG
#ifdef WIZARD
	if (wizard && !strcmp(o_str, "beartrap"))
	    return !strncmpi(o_str, u_str, 8);
#endif

	/* ignore spaces & hyphens and upper/lower case when comparing */
	if (fuzzymatch(u_str, o_str, " -\"", TRUE)) return TRUE; /* EN if (fuzzymatch(u_str, o_str, " -", TRUE)) return TRUE; */

	if (retry_inverted) {
	    const char *u_of, *o_of;
	    char *p, buf[BUFSZ];

	    /* when just one of the strings is in the form "foo of bar",
	       convert it into "bar foo" and perform another comparison */
	    u_of = strstri(u_str, " of ");
	    o_of = strstri(o_str, " of ");
	    if (u_of && !o_of) {
		Strcpy(buf, u_of + 4);
		p = eos(strcat(buf, " "));
		while (u_str < u_of) *p++ = *u_str++;
		*p = '\0';
		return fuzzymatch(buf, o_str, " -", TRUE);
	    } else if (o_of && !u_of) {
		Strcpy(buf, o_of + 4);
		p = eos(strcat(buf, " "));
		while (o_str < o_of) *p++ = *o_str++;
		*p = '\0';
		return fuzzymatch(u_str, buf, " -", TRUE);
	    }
	}

	/* [note: if something like "elven speed boots" ever gets added, these
	   special cases should be changed to call wishymatch() recursively in
	   order to get the "of" inversion handling] */
	if (!strncmp(o_str, "dwarvish ", 9)) {
	    if (!strncmpi(u_str, "dwarven ", 8))
		return fuzzymatch(u_str + 8, o_str + 9, " -", TRUE);
	} else if (!strncmp(o_str, "elven ", 6)) {
	    if (!strncmpi(u_str, "elvish ", 7))
		return fuzzymatch(u_str + 7, o_str + 6, " -", TRUE);
	    else if (!strncmpi(u_str, "elfin ", 6))
		return fuzzymatch(u_str + 6, o_str + 6, " -", TRUE);
	} else if (!strcmp(o_str, "aluminum")) {
		/* this special case doesn't really fit anywhere else... */
		/* (note that " wand" will have been stripped off by now) */
	    if (!strcmpi(u_str, "aluminium"))
		return fuzzymatch(u_str + 9, o_str + 8, " -", TRUE);
	}

	return FALSE;
}

/* alternate spellings; if the difference is only the presence or
   absence of spaces and/or hyphens (such as "pickaxe" vs "pick axe"
   vs "pick-axe") then there is no need for inclusion in this list;
   likewise for ``"of" inversions'' ("boots of speed" vs "speed boots") */
struct alt_spellings {
	const char *sp;
	int ob;
} spellings[] = {
	{ "pickax", PICK_AXE },
	{ "whip", BULLWHIP },
	{ "saber", SILVER_SABER },
	{ "silver sabre", SILVER_SABER },
	{ "smooth shield", SHIELD_OF_REFLECTION },
	{ "grey dragon scale mail", GRAY_DRAGON_SCALE_MAIL },
	{ "grey dragon scales", GRAY_DRAGON_SCALES },
	{ "enchant armour", SCR_ENCHANT_ARMOR },
	{ "destroy armour", SCR_DESTROY_ARMOR },
	{ "scroll of enchant armour", SCR_ENCHANT_ARMOR },
	{ "scroll of destroy armour", SCR_DESTROY_ARMOR },
	{ "leather armour", LEATHER_ARMOR },
	{ "studded leather armour", STUDDED_LEATHER_ARMOR },
	{ "iron ball", HEAVY_IRON_BALL },
	{ "lantern", BRASS_LANTERN },
	{ "mattock", DWARVISH_MATTOCK },
	{ "amulet of poison resistance", AMULET_VERSUS_POISON },
	{ "stone", ROCK },
#ifdef TOURIST
	{ "camera", EXPENSIVE_CAMERA },
	{ "tee shirt", T_SHIRT },
#endif
	{ "can", TIN },
	{ "can opener", TIN_OPENER },
	{ "kelp", KELP_FROND },
	{ "eucalyptus", EUCALYPTUS_LEAF },
	{ "grapple", GRAPPLING_HOOK },
	{ (const char *)0, 0 },
};

/*
 * Return something wished for.  Specifying a null pointer for
 * the user request string results in a random object.  Otherwise,
 * if asking explicitly for "nothing" (or "nil") return no_wish;
 * if not an object return &zeroobj; if an error (no matching object),
 * return null.
 * If from_user is false, we're reading from the wizkit, nothing was typed in.
 */
struct obj *
readobjnam(bp, no_wish, from_user)
register char *bp;
struct obj *no_wish;
boolean from_user;
{
	register char *p;
	register int i;
	register struct obj *otmp;
	int cnt, spe, spesgn, typ, very, rechrg;
	int blessed, uncursed, iscursed, ispoisoned, isgreased;
	int eroded, eroded2, erodeproof;
#ifdef INVISIBLE_OBJECTS
	int isinvisible;
#endif
	int halfeaten, mntmp, contents;
	int islit, unlabeled, ishistoric, isdiluted;
	struct fruit *f;
	int ftype = current_fruit;
	char fruitbuf[BUFSZ];
	/* Fruits may not mess up the ability to wish for real objects (since
	 * you can leave a fruit in a bones file and it will be added to
	 * another person's game), so they must be checked for last, after
	 * stripping all the possible prefixes and seeing if there's a real
	 * name in there.  So we have to save the full original name.  However,
	 * it's still possible to do things like "uncursed burnt Alaska",
	 * or worse yet, "2 burned 5 course meals", so we need to loop to
	 * strip off the prefixes again, this time stripping only the ones
	 * possible on food.
	 * We could get even more detailed so as to allow food names with
	 * prefixes that _are_ possible on food, so you could wish for
	 * "2 3 alarm chilis".  Currently this isn't allowed; options.c
	 * automatically sticks 'candied' in front of such names.
	 */

	char oclass;
	char *un, *dn, *actualn;
	char german_str[BUFSZ];
	const char *name=0;
#ifdef GERMAN
	int stamped = 0;
#endif

	cnt = spe = spesgn = typ = very = rechrg =
		blessed = uncursed = iscursed =
#ifdef INVISIBLE_OBJECTS
		isinvisible =
#endif
		ispoisoned = isgreased = eroded = eroded2 = erodeproof =
		halfeaten = islit = unlabeled = ishistoric = isdiluted = 0;
	mntmp = NON_PM;
#define UNDEFINED 0
#define EMPTY 1
#define SPINACH 2
	contents = UNDEFINED;
	oclass = 0;
	actualn = dn = un = 0;

	//fprintf(stderr, "readobjnam 1 oclass: %d\n", oclass); // DE DEBUG
	//printf("readobjnam 2 oclass: %d\n", oclass); // DE DEBUG

	if (!bp) goto any;
	//fprintf(stderr, "readobjnam 2 oclass: %d\n", oclass); // DE DEBUG
	//printf("readobjnam 3 oclass: %d\n", oclass); // DE DEBUG
	/* first, remove extra whitespace they may have typed */
	(void)mungspaces(bp);
	/* allow wishing for "nothing" to preserve wishless conduct...
	   [now requires "wand of nothing" if that's what was really wanted] */
	if (!strcmpi(bp, "nothing") ||
	    !strcmpi(bp, "nil") ||
	    !strcmpi(bp, "none") || 
	    !strcmpi(bp, "nada") ||   // russki
	    !strcmpi(bp, "rien") ||   // francais
	    !strcmpi(bp, "niente") || // italiano
	    !strcmpi(bp, "n�t") ||    // schwizerd�tsch
	    !strcmpi(bp, "nix") ||    // deutsch ugs
	    !strcmpi(bp, "nichts"))   // deutsch
		return no_wish;
#ifdef GERMAN
	//fprintf(stderr, "readobjnam 3 oclass: %d\n", oclass); // DE DEBUG
	german2meta(bp, german_str);
	//fprintf(stderr, "readobjnam 4 german2meta returned%s\n", german_str); // DE DEBUG
	//pline("wishing1: %s",bp);
	//pline("wishing2: %s",german_str);
	bp = german_str;
	//pline("wishing3: %s",bp);
	//pline("wishing4: %s",german_str);
#endif
	/* save the [nearly] unmodified choice string */
	Strcpy(fruitbuf, bp);

	for(;;) {
		register int l;

		//pline("wishing5: %s", bp); // DE DEBUG
		//fprintf(stderr, "readobjnam 4.5 l=%d, %s\n", l, bp); // DE DEBUG
		if (!bp || !*bp) goto any;
		if (!strncmpi(bp, "MODIFIER_CORPSE ", l=16)) {
		} else if (!strncmpi(bp, "PARTIKEL_VON ", l=13)) {
		} else if (!strncmpi(bp, "ARTIKEL_UNBESTIMMTER ", l=21)) { /* EN } else if (!strncmpi(bp, "a ", l=2)) { */
			cnt = 1;
			/* FIXME: move following line, when all is translated */
			//do { bp++; } while(*(bp+l) != ' ');
		} else if (!strncmpi(bp, "ARTIKEL_BESTIMMTER ", l=19) || /* EN } else if (!strncmpi(bp, "ARTIKEL_BESTIMMTER ", l=19) || */
		           !strncmpi(bp, "ARTIKEL_BESTIMMTER ", l=19) || /* EN !strncmpi(bp, "ARTIKEL_BESTIMMTER ", l=19) || */
		           !strncmpi(bp, " ARTIKEL_BESTIMMTER ", l=19)) { /* EN !strncmpi(bp, " ARTIKEL_BESTIMMTER ", l=19)) { */
			;	/* just increment `bp' by `l' below */
		} else if (!cnt && digit(*bp) && strcmp(bp, "0")) {
			cnt = atoi(bp);
			while(digit(*bp)) bp++;
			while(*bp == ' ') bp++;
			l = 0;
		} else if (*bp == '+' || *bp == '-') {
			spesgn = (*bp++ == '+') ? 1 : -1;
			spe = atoi(bp);
			while(digit(*bp)) bp++;
			while(*bp == ' ') bp++;
			l = 0;
		} else if (!strncmpi(bp, "ADJEKTIV_BLESSED ", l=17) || /* EN } else if (!strncmpi(bp, "blessed ", l=8) || */
			   !strncmpi(bp, "ADJEKTIV_BLESSED ", l=17)) { /* EN !strncmpi(bp, "holy ", l=5)) { */
			blessed = 1;
			//fprintf(stderr, "\n\nblessed %s\n", bp); // DE DEBUG
		} else if (!strncmpi(bp, "ADJEKTIV_CURSED ", l=16) || /* EN } else if (!strncmpi(bp, "cursed ", l=7) || */
			   !strncmpi(bp, "ADJEKTIV_CURSED ", l=16)) { /* EN !strncmpi(bp, "unholy ", l=7)) { */
			iscursed = 1;
		} else if (!strncmpi(bp, "ADJEKTIV_UNCURSED ", l=18)) { /* EN } else if (!strncmpi(bp, "uncursed ", l=9)) { */
			uncursed = 1;
#ifdef INVISIBLE_OBJECTS
		} else if (!strncmpi(bp, "ADJEKTIV_INVISIBLE ", l=19)) { /* EN } else if (!strncmpi(bp, "invisible ", l=10)) { */
			isinvisible = 1;
#endif
		} else if (!strncmpi(bp, "ADJEKTIV_RUSTPROOF ", l=19) || /* EN } else if (!strncmpi(bp, "rustproof ", l=10) ||  */
			   !strncmpi(bp, "ADJEKTIV_ERODEPROOF ", l=20) || /* EN !strncmpi(bp, "erodeproof ", l=11) ||  */
			   !strncmpi(bp, "ADJEKTIV_CORRODEPROOF ", l=22) || /* EN !strncmpi(bp, "corrodeproof ", l=13) || */
			   !strncmpi(bp, "ADJEKTIV_FIXED ", l=15) || /* EN !strncmpi(bp, "fixed ", l=6) || */
			   !strncmpi(bp, "ADJEKTIV_FIREPROOF ", l=19) || /* EN !strncmpi(bp, "fireproof ", l=10) || */
			   !strncmpi(bp, "ADJEKTIV_ROTPROOF ", l=18)) { /* EN !strncmpi(bp, "rotproof ", l=9)) { */
			erodeproof = 1;
		} else if (!strncmpi(bp,"ADJEKTIV_LIT ", l=13) || /* EN } else if (!strncmpi(bp,"lit ", l=4) || */
			   !strncmpi(bp,"ADJEKTIV_BRENNEND ", l=18)) { /* EN !strncmpi(bp,"burning ", l=8)) { */
			islit = 1;
		} else if (!strncmpi(bp,"unlit ", l=6) || /* EN } else if (!strncmpi(bp,"unlit ", l=6) || */ // TODO DE
			   !strncmpi(bp,"extinguished ", l=13)) { /* EN !strncmpi(bp,"extinguished ", l=13)) { */ // TODO DE
			islit = 0;
		/* "unlabeled" and "blank" are synonymous */
		} else if (!strncmpi(bp,"ADJEKTIV_SCR_UNLABELED ", l=23) || /* EN } else if (!strncmpi(bp,"unlabeled ", l=10) || */
			   !strncmpi(bp,"ADJEKTIV_SPE_PLAIN ", l=19) || /* EN !strncmpi(bp,"unlabelled ", l=11) || */
			   !strncmpi(bp,"ADJEKTIV_LEER ", l=14)) { /* EN !strncmpi(bp,"blank ", l=6)) { */
			unlabeled = 1;
#ifdef GERMAN
		} else if (!strncmpi(bp,"ADJEKTIV_SCR_STAMPED ", l=21)) {
			stamped = 1;
#endif
		} else if(!strncmpi(bp, "ADJEKTIV_POISONED ",l=18) /* EN } else if(!strncmpi(bp, "poisoned ",l=9) */
#ifdef WIZARD
			  || (wizard && !strncmpi(bp, "trapped ",l=8)) /* EN || (wizard && !strncmpi(bp, "trapped ",l=8)) */ // TODO DE
#endif
			  ) {
			ispoisoned=1;
		} else if(!strncmpi(bp, "ADJEKTIV_GREASED ",l=17)) { /* EN } else if(!strncmpi(bp, "greased ",l=8)) { */ // TODO DE
			isgreased=1;
		} else if (!strncmpi(bp, "sehr ", l=5) || !strncmpi(bp, "stark ", l=6)) { /* EN } else if (!strncmpi(bp, "very ", l=5)) { */
			/* very rusted very heavy iron ball */
			very = 1;
		} else if (!strncmpi(bp, "vollstaendig ", l=13) || !strncmpi(bp, "ganz ", l=5)) { /* EN } else if (!strncmpi(bp, "thoroughly ", l=11)) { */
			very = 2;
		} else if (!strncmpi(bp, "ADJEKTIV_ROSTIG ", l=16) || /* EN } else if (!strncmpi(bp, "rusty ", l=6) || */
			   !strncmpi(bp, "ADJEKTIV_VERROSTET ", l=19) || /* EN !strncmpi(bp, "rusted ", l=7) || */
			   !strncmpi(bp, "ADJEKTIV_ANGESENGT ", l=19) || /* EN !strncmpi(bp, "burnt ", l=6) || */
			   !strncmpi(bp, "ADJEKTIV_VERSENGT ", l=18)) { /* EN !strncmpi(bp, "burned ", l=7)) { */
			eroded = 1 + very;
			very = 0;
		} else if (!strncmpi(bp, "ADJEKTIV_CORRODED ", l=18) || /* EN } else if (!strncmpi(bp, "corroded ", l=9) || */
			   !strncmpi(bp, "rotted ", l=7)) { /* EN !strncmpi(bp, "rotted ", l=7)) { */ // TODO DE
			eroded2 = 1 + very;
			very = 0;
		} else if (!strncmpi(bp, "halb ADJEKTIV_EATEN ", l=20)) { /* EN } else if (!strncmpi(bp, "half eaten ", l=11)) { */
			halfeaten = 1;
		} else if (!strncmpi(bp, "ADJEKTIV_ANTIK ", l=15)) { /* EN } else if (!strncmpi(bp, "historic ", l=9)) { */
			ishistoric = 1;
		} else if (!strncmpi(bp, "diluted ", l=8)) { /* EN } else if (!strncmpi(bp, "diluted ", l=8)) { */ // TODO DE
			isdiluted = 1;
		} else if(!strncmpi(bp, "ADJEKTIV_LEER ", l=14)) { /* EN } else if(!strncmpi(bp, "empty ", l=6)) { */
			contents = EMPTY;
		} else { /*pline("wishing, NICHT erkannt: %s",bp);*/ break; }
		//pline("wishing, erkannt: %s",bp);
		//fprintf(stderr, "readobjnam 4.5 l=%d, %s\n", l, bp); // DE DEBUG
		bp += l;
		//fprintf(stderr, "readobjnam 4.5 l=%d, %s\n", l, bp); // DE DEBUG
		//pline("wishing, erkannt: %s",bp);
	}
	if(!cnt) cnt = 1;		/* %% what with "gems" etc. ? */
	if (strlen(bp) > 1) {
	    if ((p = rindex(bp, '(')) != 0) {
		if (p > bp && p[-1] == ' ') p[-1] = 0;
		else *p = 0;
		p++;
		if (!strcmpi(p, "ADJEKTIV_LIT)")) { /* EN if (!strcmpi(p, "lit)")) { */
		    islit = 1;
		} else {
		    spe = atoi(p);
		    while (digit(*p)) p++;
		    if (*p == ':') {
			p++;
			rechrg = spe;
			spe = atoi(p);
			while (digit(*p)) p++;
		    }
		    if (*p != ')') {
			spe = rechrg = 0;
		    } else {
			spesgn = 1;
			p++;
			if (*p) Strcat(bp, p);
		    }
		}
	    }
	}
	//fprintf(stderr, "readobjnam 5 %s\n", bp); // DE DEBUG
/*
   otmp->spe is type schar; so we don't want spe to be any bigger or smaller.
   also, spe should always be positive  -- some cheaters may try to confuse
   atoi()
*/
	if (spe < 0) {
		spesgn = -1;	/* cheaters get what they deserve */
		spe = abs(spe);
	}
	if (spe > SCHAR_LIM)
		spe = SCHAR_LIM;
	if (rechrg < 0 || rechrg > 7) rechrg = 7;	/* recharge_limit */

	/* now we have the actual name, as delivered by xname, say
		green potions called whisky
		scrolls labeled "QWERTY"
		egg
		fortune cookies
		very heavy iron ball named hoei
		wand of wishing
		elven cloak
	*/
	if ((p = strstri(bp, " PARTIKEL_NAMED ")) != 0) {
		*p = 0;
		name = p+7;
	}
	if ((p = strstri(bp, " PARTIKEL_CALLED ")) != 0) {
		*p = 0;
		un = p+8;
		/* "helmet called telepathy" is not "helmet" (a specific type)
		 * "shield called reflection" is not "shield" (a general type)
		 */
		for(i = 0; i < SIZE(o_ranges); i++)
		    if(!strcmpi(bp, o_ranges[i].name)) {
			oclass = o_ranges[i].oclass;
			goto srch;
		    }
	}
	if ((p = strstri(bp, " labeled ")) != 0) {
		*p = 0;
		dn = p+9;
	} else if ((p = strstri(bp, " labelled ")) != 0) {
		*p = 0;
		dn = p+10;
	}
#ifdef GERMAN
	if ((p = strstri(bp, " \"")) != 0) {
		*p = 0;
		dn = p+2;
	}
#endif

	if ((p = strstri(bp, " of spinach")) != 0) {
		*p = 0;
		contents = SPINACH;
	}

	/*
	Skip over "pair of ", "pairs of", "set of" and "sets of".

	Accept "3 pair of boots" as well as "3 pairs of boots". It is valid
	English either way.  See makeplural() for more on pair/pairs.

	We should only double count if the object in question is not
	refered to as a "pair of".  E.g. We should double if the player
	types "pair of spears", but not if the player types "pair of
	lenses".  Luckily (?) all objects that are refered to as pairs
	-- boots, gloves, and lenses -- are also not mergable, so cnt is
	ignored anyway.
	*/
	
	if(!strncmpi(bp, "pair of ",8)) {
		bp += 8;
		cnt *= 2;
	} else if(cnt > 1 && !strncmpi(bp, "pairs of ",9)) {
		bp += 9;
		cnt *= 2;
	} else if (!strncmpi(bp, "set of ",7)) {
		bp += 7;
	} else if (!strncmpi(bp, "sets of ",8)) {
		bp += 8;
	} else if (strncmpi(bp, "NOUN_PAAR ",10)==0) {
		bp += 10;
	}
	//fprintf(stderr, "readobjnam 6 %s\n", bp);
	//fprintf(stderr, "readobjnam 6 %s\n", bp);

	/*
	 * Find corpse type using "of" (figurine of an orc, tin of orc meat)
	 * Don't check if it's a wand or spellbook.
	 * (avoid "wand/finger of death" confusion).
	 */
	if (!strstri(bp, "NOUN_WAND ") /* EN if (!strstri(bp, "wand ") */
	 && !strstri(bp, "NOUN_SPELLBOOK ") /* EN && !strstri(bp, "spellbook ") */
	 && !strstri(bp, "NOUN_FINGER_OF_DEATH ")) { /* EN && !strstri(bp, "finger ")) { */
	//fprintf(stderr, "readobjnam 6.25 %s\n", bp);
	    if ((p = strstri(bp, " PARTIKEL_OF ")) != 0 /* EN if ((p = strstri(bp, " of ")) != 0 */
		&& (mntmp = name_to_mon(p+13)) >= LOW_PM) /* EN && (mntmp = name_to_mon(p+4)) >= LOW_PM) */
		*p = 0;
	}
	//fprintf(stderr, "readobjnam 6.5 %s\n", bp);
	/* Find corpse type w/o "of" (red dragon scale mail, yeti corpse) */
	if (strncmpi(bp, "samurai sword", 13)) /* not the "samurai" monster! */
	if (strncmpi(bp, "wizard lock", 11)) /* not the "wizard" monster! */
	if (strncmpi(bp, "ninja-to", 8)) /* not the "ninja" rank */
	if (strncmpi(bp, "master key", 10)) /* not the "master" rank */
	if (strncmpi(bp, "magenta", 7)) /* not the "mage" rank */
	if (mntmp < LOW_PM && strlen(bp) > 2 &&
	    (mntmp = name_to_mon(bp)) >= LOW_PM) {
		int mntmptoo, mntmplen;	/* double check for rank title */
		char *obp = bp;
		mntmptoo = title_to_mon(bp, (int *)0, &mntmplen);
		bp += mntmp != mntmptoo ? (int)strlen(mons[mntmp].mname) : mntmplen;
		if (*bp == ' ') bp++;
		else if (!strncmpi(bp, "s ", 2)) bp += 2;
		else if (!strncmpi(bp, "es ", 3)) bp += 3;
		else if (!*bp && !actualn && !dn && !un && !oclass) {
		    /* no referent; they don't really mean a monster type */
		    bp = obp;
		    mntmp = NON_PM;
		}
	}
	//fprintf(stderr, "readobjnam 7\n"); // DE DEBUG

	/* first change to singular if necessary */
	if (*bp) {
		char *sng = makesingular(bp);
		//fprintf(stderr, "readobjnam bp: %s; sng: %s\n", bp, sng); // DE DEBUG
		if (strcmp(bp, sng)) {
			if (cnt == 1) cnt = 2;
			Strcpy(bp, sng);
		}
	}
	//fprintf(stderr, "readobjnam 8\n"); // DE DEBUG

	/* Alternate spellings (pick-ax, silver sabre, &c) */
    {
	struct alt_spellings *as = spellings;

	while (as->sp) {
		if (fuzzymatch(bp, as->sp, " -", TRUE)) {
			typ = as->ob;
			goto typfnd;
		}
		as++;
	}
	//fprintf(stderr, "0####### dn: -%s-\n", dn); // DE DEBUG
	/* can't use spellings list for this one due to shuffling */
	if (!strncmpi(bp, "grey spell", 10))
		*(bp + 2) = 'a';
    }

	//fprintf(stderr, "0####### dn: -%s-\n", dn); // DE DEBUG
	/* dragon scales - assumes order of dragons */
	if(!strcmpi(bp, "scales") &&
			mntmp >= PM_GRAY_DRAGON && mntmp <= PM_YELLOW_DRAGON) {
		typ = GRAY_DRAGON_SCALES + mntmp - PM_GRAY_DRAGON;
		mntmp = NON_PM;	/* no monster */
		goto typfnd;
	}

	//fprintf(stderr, "6####### dn: -%s-\n", dn); // DE DEBUG
	//fprintf(stderr, "6a####### bp: -%s-\n", bp); // DE DEBUG
	p = eos(bp);
	if(!BSTRCMPI(bp, p-21, "NOUN_POT_UNHOLY_WATER")) {
		//fprintf(stderr, "6ba###### bp: -%s-\n", bp);
		typ = POT_WATER;
		iscursed = 1; /* unholy water */
		goto typfnd;
	}
	if(!BSTRCMPI(bp, p-19, "NOUN_POT_HOLY_WATER")) {
		//fprintf(stderr, "6bb###### bp: -%s-\n", bp);
		typ = POT_WATER;
		blessed = 1; /* holy water */
		goto typfnd;
	}
	if(!BSTRCMPI(bp, p-10, "holy water")) {
		typ = POT_WATER;
		if ((p-bp) >= 12 && *(p-12) == 'u')
			iscursed = 1; /* unholy water */
		else blessed = 1;
		goto typfnd;
	}
	//fprintf(stderr, "7####### dn: -%s-\n", dn); // DE DEBUG
	if(unlabeled && !BSTRCMPI(bp, p-11, "NOUN_SCROLL")) {
		typ = SCR_BLANK_PAPER;
		goto typfnd;
	}
#if defined(GERMAN) && defined(MAIL)
	if(stamped && !BSTRCMPI(bp, p-11, "NOUN_SCROLL")) {
		typ = SCR_MAIL;
		goto typfnd;
	}
#endif
	//fprintf(stderr, "8####### dn: -%s-\n", dn); // DE DEBUG
	if(unlabeled && !BSTRCMPI(bp, p-14, "NOUN_SPELLBOOK")) {
		typ = SPE_BLANK_PAPER;
		goto typfnd;
	}
	//fprintf(stderr, "9####### dn: -%s-\n", dn); // DE DEBUG
	//fprintf(stderr, "10###### -%s-\n", bp); // DE DEBUG
	if(strncmp(bp, "NOUN_RING_UNIDENTIFIED_", 23)==0) {
		bp += 5; dn = bp;
		oclass = RING_CLASS;
		goto srch;
	}
	//fprintf(stderr, "11###### %s\n", bp);
	/*
	 * NOTE: Gold pieces are handled as objects nowadays, and therefore
	 * this section should probably be reconsidered as well as the entire
	 * gold/money concept.  Maybe we want to add other monetary units as
	 * well in the future. (TH)
	 */
	if(!BSTRCMPI(bp, p-10, "gold piece") || !BSTRCMPI(bp, p-7, "zorkmid") ||
	   !strcmpi(bp, "gold") || !strcmpi(bp, "money") ||
	   !strcmpi(bp, "coin") || *bp == GOLD_SYM) {
			if (cnt > 5000
#ifdef WIZARD
					&& !wizard
#endif
						) cnt=5000;
		if (cnt < 1) cnt=1;
#ifndef GOLDOBJ
		if (from_user)
		    pline("%d NOUN_GOLD_PIECE%s.", cnt, plur(cnt)); /* EN pline("%d gold piece%s.", cnt, plur(cnt)); */
		u.ugold += cnt;
		flags.botl=1;
		return (&zeroobj);
#else
                otmp = mksobj(GOLD_PIECE, FALSE, FALSE);
		otmp->quan = cnt;
                otmp->owt = weight(otmp);
		flags.botl=1;
		return (otmp);
#endif
	}
	//fprintf(stderr, "2####### %s\n", bp);
	if (strlen(bp) == 1 &&
	   (i = def_char_to_objclass(*bp)) < MAXOCLASSES && i > ILLOBJ_CLASS
#ifdef WIZARD
	    && (wizard || i != VENOM_CLASS)
#else
	    && i != VENOM_CLASS
#endif
	    ) {
		oclass = i;
				//fprintf(stderr, "######## %d\n", bp);
		goto any;
	}
	//fprintf(stderr, "3####### %s\n", bp);

	/* Search for class names: XXXXX potion, scroll of XXXXX.  Avoid */
	/* false hits on, e.g., rings for "ring mail". */
	if(strncmpi(bp, "NOUN_SCR_ENCHANT_", 17) && /* EN if(strncmpi(bp, "enchant ", 8) && */
	   strncmpi(bp, "NOUN_SCR_DESTROY_", 17) && /* EN strncmpi(bp, "destroy ", 8) && */
	   strncmpi(bp, "NOUN_SCR_FOOD_DETECTION", 23) && /* EN strncmpi(bp, "food detection", 14) && */
	   strncmpi(bp, "NOUN_RING_MAIL", 14) && /* EN strncmpi(bp, "ring mail", 9) && */
	   strncmpi(bp, "NOUN_STUDDED_LEATHER_ARM", 24) && /* EN strncmpi(bp, "studded leather arm", 19) && */
	   strncmpi(bp, "NOUN_LEATHER_ARM", 16) && /* EN strncmpi(bp, "leather arm", 11) && */
	   strncmpi(bp, "NOUN_TOOLED_HORN", 16) && /* EN strncmpi(bp, "tooled horn", 11) && */
	   strncmpi(bp, "NOUN_FOOD_RATION", 16) && /* EN strncmpi(bp, "food ration", 11) && */
	   strncmpi(bp, "NOUN_MEAT_RING", 14) /* EN strncmpi(bp, "meat ring", 9) */
	)
	for (i = 0; i < (int)(sizeof wrpsym); i++) {
		register int j = strlen(wrp[i]);
		//fprintf(stderr, "4a###### %s\n", bp);
		if(!strncmpi(bp, wrp[i], j)){
			oclass = wrpsym[i];
			//fprintf(stderr, "4####### %s\n", bp);
			if ((oclass != AMULET_CLASS) && (oclass != GEM_CLASS) && (oclass != FOOD_CLASS)) { /* EN if (oclass != AMULET_CLASS) { */
			    bp += j;
					//fprintf(stderr, "5####### %s\n", bp);
			    if(!strncmpi(bp, " PARTIKEL_OF ", 13)) actualn = bp+13;
					//fprintf(stderr, "5b###### %s\n", actualn);
			    if(!strncmpi(bp, " ARTIKEL_BESTIMMTER ", 20)) actualn = bp+20;
			    /* else if(*bp) ?? */
			} else {
			    actualn = bp;
			}
			//fprintf(stderr, "6b###### %s\n", bp);
			goto srch;
		}
		if(!BSTRCMPI(bp, p-j, wrp[i])){
			//fprintf(stderr, "7######## %s\n", wrp[i]);
			
			oclass = wrpsym[i];
			p -= j;
			*p = 0;
			if(p > bp && p[-1] == ' ') p[-1] = 0;
			actualn = dn = bp;

			// TODO remove
			if (oclass == RING_CLASS) {
				Strcpy(bp, "NOUN_");
				//fprintf(stderr, "######## %d\n", bp);
			}
			goto srch;
		}
	}
	//fprintf(stderr, "7####### %s\n", bp);

	/* "grey stone" check must be before general "stone" */
	for (i = 0; i < SIZE(o_ranges); i++)
	    if(!strcmpi(bp, o_ranges[i].name)) {
		typ = rnd_class(o_ranges[i].f_o_range, o_ranges[i].l_o_range);
		goto typfnd;
	    }

	if (!BSTRCMPI(bp, p-14, " NOUN_GEM_ROCK")) { /* EN if (!BSTRCMPI(bp, p-6, " stone")) { */
		p[-14] = 0; /* EN p[-6] = 0; */
		oclass = GEM_CLASS;
		dn = actualn = bp;
		//fprintf(stderr, "y####### dn:      %s\n", dn);
		goto srch;
#ifdef GERMAN
	} else if (!BSTRCMPI(bp, bp+9, "NOUN_GEM_")) {
		oclass = GEM_CLASS;
		dn = actualn = bp;
		//fprintf(stderr, "z####### dn:      %s\n", dn);
		goto srch;
#endif
	} else if (!strcmpi(bp, "looking glass")) {
		;	/* avoid false hit on "* glass" */
	} else if (!BSTRCMPI(bp, p-6, " glass") || !strcmpi(bp, "glass")) {
		register char *g = bp;
		if (strstri(g, "broken")) return (struct obj *)0;
		if (!strncmpi(g, "worthless ", 10)) g += 10;
		if (!strncmpi(g, "piece of ", 9)) g += 9;
		if (!strncmpi(g, "colored ", 8)) g += 8;
		else if (!strncmpi(g, "coloured ", 9)) g += 9;
		if (!strcmpi(g, "glass")) {	/* choose random color */
			/* 9 different kinds */
			typ = LAST_GEM + rnd(9);
			if (objects[typ].oc_class == GEM_CLASS) goto typfnd;
			else typ = 0;	/* somebody changed objects[]? punt */
		} else {		/* try to construct canonical form */
			char tbuf[BUFSZ];
			Strcpy(tbuf, "worthless piece of ");
			Strcat(tbuf, g);  /* assume it starts with the color */
			Strcpy(bp, tbuf);
		}
	}

	actualn = bp;
	if (!dn) dn = actualn; /* ex. "skull cap" */
srch:
	//fprintf(stderr, "####\n");
	//fprintf(stderr, "srch#### dn:      %s\n", dn);
	//fprintf(stderr, "srch#### bp:      %s\n", bp);
	//fprintf(stderr, "srch#### actualn: %s\n", actualn);
	//fprintf(stderr, "srch#### oclass:  %d\n", oclass);
	/* check real names of gems first */
	if(!oclass && actualn) {
		//fprintf(stderr, "a00#######\n");
		//fprintf(stderr, "a01####### bases[GEM_CLASS]: %d\n", bases[GEM_CLASS]);
		//fprintf(stderr, "a02####### bases: %d, GEM_CLASS: %d, LAST_GEM: %d\n", bases, GEM_CLASS, LAST_GEM);
		for(i = bases[GEM_CLASS]; i <= LAST_GEM; i++) {
			register const char *zn;

			if((zn = OBJ_NAME(objects[i])) && !strcmpi(actualn, zn)) {
		    typ = i;
		    goto typfnd;
			}
		}
	}
	i = oclass ? bases[(int)oclass] : 1;
	while(i < NUM_OBJECTS && (!oclass || objects[i].oc_class == oclass)){
		register const char *zn;

		//fprintf(stderr, "b####### i: %d actualn: %s; dn: %s; un: %s\n", i, actualn, dn, un);
		//fprintf(stderr, "b####### i: %d zn: %s\n", i, OBJ_NAME(objects[i]));
		//fprintf(stderr, "b####### i: %d desc: %s\n", i, OBJ_DESCR(objects[i]));
		if (actualn && (zn = OBJ_NAME(objects[i])) != 0 &&
			    wishymatch(actualn, zn, TRUE)) {
			//fprintf(stderr, "c####### %s\n", zn);
			typ = i;
			goto typfnd;
		}
		if (dn && (zn = OBJ_DESCR(objects[i])) != 0 &&
			    wishymatch(dn, zn, FALSE)) {
			/* don't match extra descriptions (w/o real name) */
			//fprintf(stderr, "d####### %s\n", zn);
			if (!OBJ_NAME(objects[i])) return (struct obj *)0;
			typ = i;
			goto typfnd;
		}
		if (un && (zn = objects[i].oc_uname) != 0 &&
			    wishymatch(un, zn, FALSE)) {
			//fprintf(stderr, "d wishymatch ####### %s\n", zn);
			typ = i;
			goto typfnd;
		}
		i++;
	}
	if (actualn) {
		struct Jitem *j = Japanese_items;
		while(j->item) {
			if (actualn && !strcmpi(actualn, j->name)) {
				typ = j->item;
				goto typfnd;
			}
			j++;
		}
	}
	if (!strcmpi(bp, "spinach")) {
		contents = SPINACH;
		typ = TIN;
		goto typfnd;
	}
	/* Note: not strncmpi.  2 fruits, one capital, one not, are possible. */
	{
	    char *fp;
	    int l, cntf;
	    int blessedf, iscursedf, uncursedf, halfeatenf;

	    blessedf = iscursedf = uncursedf = halfeatenf = 0;
	    cntf = 0;

	    fp = fruitbuf;
	    for(;;) {
		if (!fp || !*fp) break;
		if (!strncmpi(fp, "an ", l=3) ||
		    !strncmpi(fp, "a ", l=2)) {
			cntf = 1;
		} else if (!cntf && digit(*fp)) {
			cntf = atoi(fp);
			while(digit(*fp)) fp++;
			while(*fp == ' ') fp++;
			l = 0;
		} else if (!strncmpi(fp, "blessed ", l=8)) {
			blessedf = 1;
		} else if (!strncmpi(fp, "cursed ", l=7)) {
			iscursedf = 1;
		} else if (!strncmpi(fp, "uncursed ", l=9)) {
			uncursedf = 1;
		} else if (!strncmpi(fp, "halb verspeis", l=13)) {
			halfeatenf = 1;
		} else {
			//pline("wishing: unerkannt: -%s-\n",fp);
			break;
		}
		fp += l;
		//pline("wishing: =%s=\n",fp);
	    }

	    for(f=ffruit; f; f = f->nextf) {
		char *f1 = f->fname, *f2 = makeplural(f->fname);

		if(!strncmp(fp, f1, strlen(f1)) ||
					!strncmp(fp, f2, strlen(f2))) {
			typ = SLIME_MOLD;
			blessed = blessedf;
			iscursed = iscursedf;
			uncursed = uncursedf;
			halfeaten = halfeatenf;
			cnt = cntf;
			ftype = f->fid;
			goto typfnd;
		}
	    }
	}

	//fprintf(stderr, "readobjnam 500 typ: %d; oclass: %d\n", typ, oclass); // DE DEBUG

	if(!oclass && actualn) {
	    short objtyp;

	    /* Perhaps it's an artifact specified by name, not type */
#ifdef GERMAN
	    /* Spezialbehandlung, da die Augen der Anderswelt im Plural sein muessen */
	    if (strcmp(actualn, "NOUN_THE_EYES_OF_THE_OVERWORLD")==0) {
	      name = artifact_name("NOUN_THE_EYES_OF_THE_OVERWORLDs", &objtyp);
	    } else
#endif
	    name = artifact_name(actualn, &objtyp);
	    if(name) {
		typ = objtyp;
		goto typfnd;
	    }
	}
#ifdef WIZARD
	/* Let wizards wish for traps --KAA */
	/* must come after objects check so wizards can still wish for
	 * trap objects like beartraps
	 */
	if (wizard && from_user) {
		int trap;

		for (trap = NO_TRAP+1; trap < TRAPNUM; trap++) {
			const char *tname;

			tname = defsyms[trap_to_defsym(trap)].explanation;
			if (!strncmpi(tname, bp, strlen(tname))) {
				/* avoid stupid mistakes */
				if((trap == TRAPDOOR || trap == HOLE)
				      && !Can_fall_thru(&u.uz)) trap = ROCKTRAP;
				(void) maketrap(u.ux, u.uy, trap);
				pline("SUBJECT %s.", An(tname)); /* EN pline("%s.", An(tname)); */
				return(&zeroobj);
			}
		}
		/* or some other dungeon features -dlc */
		p = eos(bp);
		if(!BSTRCMP(bp, p-13, "NOUN_FOUNTAIN")) { /* EN if(!BSTRCMP(bp, p-8, "fountain")) { */
			levl[u.ux][u.uy].typ = FOUNTAIN;
			level.flags.nfountains++;
			if(!strncmpi(bp, "ADJEKTIV_MAGISCH ", 17)) /* EN if(!strncmpi(bp, "magic ", 6)) */
				levl[u.ux][u.uy].blessedftn = 1;
			pline("SUBJECT ARTIKEL_UNBESTIMMTER %sNOUN_FOUNTAIN.", /* EN pline("A %sfountain.", */
			      levl[u.ux][u.uy].blessedftn ? "ADJEKTIV_MAGISCH " : ""); /* EN levl[u.ux][u.uy].blessedftn ? "magic " : ""); */
			newsym(u.ux, u.uy);
			return(&zeroobj);
		}
		if(!BSTRCMP(bp, p-11, "NOUN_THRONE")) { /* EN if(!BSTRCMP(bp, p-6, "throne")) { */
			levl[u.ux][u.uy].typ = THRONE;
			pline("SUBJECT ARTIKEL_UNBESTIMMTER NOUN_THRONE."); /* EN pline("A throne."); */
			newsym(u.ux, u.uy);
			return(&zeroobj);
		}
# ifdef SINKS
		if(!BSTRCMP(bp, p-9, "NOUN_SINK")) { /* EN if(!BSTRCMP(bp, p-4, "sink")) { */
			levl[u.ux][u.uy].typ = SINK;
			level.flags.nsinks++;
			pline("SUBJECT ARTIKEL_UNBESTIMMTER NOUN_SINK."); /* EN pline("A sink."); */
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
# endif
		if(!BSTRCMP(bp, p-9, "NOUN_POOL")) { /* EN if(!BSTRCMP(bp, p-4, "pool")) { */
			levl[u.ux][u.uy].typ = POOL;
			del_engr_at(u.ux, u.uy);
			pline("SUBJECT ARTIKEL_UNBESTIMMTER NOUN_POOL."); /* EN pline("A pool."); */
			/* Must manually make kelp! */
			water_damage(level.objects[u.ux][u.uy], FALSE, TRUE);
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if (!BSTRCMP(bp, p-9, "NOUN_LAVA")) {  /* also matches "molten lava" */ /* EN if (!BSTRCMP(bp, p-4, "lava")) {  */
			levl[u.ux][u.uy].typ = LAVAPOOL;
			del_engr_at(u.ux, u.uy);
			pline("Ein Becken voll KASUS_DATIV ADJEKTIV_FLUESSIG NOUN_LAVA."); /* EN pline("A pool of molten lava."); */
			if (!(Levitation || Flying)) (void) lava_effects();
			newsym(u.ux, u.uy);
			return &zeroobj;
		}

		if(!BSTRCMP(bp, p-10, "NOUN_ALTAR")) { /* EN if(!BSTRCMP(bp, p-5, "altar")) { */
		    aligntyp al;

		    levl[u.ux][u.uy].typ = ALTAR;
		    if(!strncmpi(bp, "ADJEKTIV_CHAOTIC ", 17)) /* EN if(!strncmpi(bp, "chaotic ", 8)) */
			al = A_CHAOTIC;
		    else if(!strncmpi(bp, "ADJEKTIV_NEUTRAL ", 17)) /* EN else if(!strncmpi(bp, "neutral ", 8)) */
			al = A_NEUTRAL;
		    else if(!strncmpi(bp, "ADJEKTIV_LAWFUL ", 16)) /* EN else if(!strncmpi(bp, "lawful ", 7)) */
			al = A_LAWFUL;
		    else if(!strncmpi(bp, "ADJEKTIV_UNALIGNED ", 19)) /* EN else if(!strncmpi(bp, "unaligned ", 10)) */
			al = A_NONE;
		    else /* -1 - A_CHAOTIC, 0 - A_NEUTRAL, 1 - A_LAWFUL */
			al = (!rn2(6)) ? A_NONE : rn2((int)A_LAWFUL+2) - 1;
		    levl[u.ux][u.uy].altarmask = Align2amask( al );
		    pline("SUBJECT %s NOUN_ALTAR.", An(align_str(al))); /* EN pline("%s altar.", An(align_str(al))); */
		    newsym(u.ux, u.uy);
		    return(&zeroobj);
		}

		if(!BSTRCMP(bp, p-10, "NOUN_GRAVE") || !BSTRCMP(bp, p-14, "NOUN_HEADSTONE")) { /* EN if(!BSTRCMP(bp, p-5, "grave") || !BSTRCMP(bp, p-9, "headstone")) { */
		    make_grave(u.ux, u.uy, (char *) 0);
		    pline("SUBJECT ARTIKEL_UNBESTIMMTER NOUN_GRAVE."); /* EN pline("A grave."); */
		    newsym(u.ux, u.uy);
		    return(&zeroobj);
		}

		if(!BSTRCMP(bp, p-9, "NOUN_TREE")) { /* EN if(!BSTRCMP(bp, p-4, "tree")) { */
		    levl[u.ux][u.uy].typ = TREE;
		    pline("SUBJECT ARTIKEL_UNBESTIMMTER NOUN_TREE."); /* EN pline("A tree."); */
		    newsym(u.ux, u.uy);
		    block_point(u.ux, u.uy);
		    return &zeroobj;
		}

		if(!BSTRCMP(bp, p-13, "NOUN_IRON_BAR")) { /* EN if(!BSTRCMP(bp, p-4, "bars")) { */
		    levl[u.ux][u.uy].typ = IRONBARS;
		    pline("SUBJECT NOUN_IRON_BARs."); /* EN pline("Iron bars."); */
		    newsym(u.ux, u.uy);
		    return &zeroobj;
		}
	}
#endif
	//fprintf(stderr, "readobjnam 600 oclass: %d\n", oclass); // DE DEBUG
	if(!oclass) return((struct obj *)0);
	//fprintf(stderr, "readobjnam 601 oclass: %d\n", oclass); // DE DEBUG
any:
	if(!oclass) oclass = wrpsym[rn2((int)sizeof(wrpsym))];
	//fprintf(stderr, "readobjnam 602 oclass: %d\n", oclass); // DE DEBUG
typfnd:
	//fprintf(stderr, "readobjnam 603 oclass: %d\n", oclass); // DE DEBUG
	if (typ) oclass = objects[typ].oc_class;

	/* check for some objects that are not allowed */
	if (typ && objects[typ].oc_unique) {
#ifdef WIZARD
	    if (wizard)
		;	/* allow unique objects */
	    else
#endif
	    switch (typ) {
		case AMULET_OF_YENDOR:
		    typ = FAKE_AMULET_OF_YENDOR;
		    break;
		case CANDELABRUM_OF_INVOCATION:
		    typ = rnd_class(TALLOW_CANDLE, WAX_CANDLE);
		    break;
		case BELL_OF_OPENING:
		    typ = BELL;
		    break;
		case SPE_BOOK_OF_THE_DEAD:
		    typ = SPE_BLANK_PAPER;
		    break;
	    }
	}

	/* catch any other non-wishable objects */
	if (objects[typ].oc_nowish
#ifdef WIZARD
	    && !wizard
#endif
	    )
	    return((struct obj *)0);

	/* convert magic lamps to regular lamps before lighting them or setting
	   the charges */
	if (typ == MAGIC_LAMP
#ifdef WIZARD
				&& !wizard
#endif
						)
	    typ = OIL_LAMP;

	//fprintf(stderr, "readobjnam 620 typ: %d; oclass: %d\n", typ, oclass); // DE DEBUG
	//fprintf(stderr, "readobjnam 620 typ: %d; oclass: %d\n", typ, oclass); // DE DEBUG
	//fprintf(stderr, "readobjnam 630 typ: %d\n", typ); // DE DEBUG
	if(typ) {
		//fprintf(stderr, "readobjnam 631 oclass: %d\n", oclass); // DE DEBUG
		otmp = mksobj(typ, TRUE, FALSE);
		//fprintf(stderr, "readobjnam 632 oclass: %d\n", oclass); // DE DEBUG
	} else {
		//fprintf(stderr, "readobjnam 633 oclass: %d\n", oclass); // DE DEBUG
		otmp = mkobj(oclass, FALSE);
		//fprintf(stderr, "readobjnam 634 oclass: %d\n", oclass); // DE DEBUG
		if (otmp) typ = otmp->otyp;
		//fprintf(stderr, "readobjnam 635 oclass: %d\n", oclass); // DE DEBUG
	}

	if (islit &&
		(typ == OIL_LAMP || typ == MAGIC_LAMP || typ == BRASS_LANTERN ||
		 Is_candle(otmp) || typ == POT_OIL)) {
	    place_object(otmp, u.ux, u.uy);  /* make it viable light source */
	    begin_burn(otmp, FALSE);
	    obj_extract_self(otmp);	 /* now release it for caller's use */
	}

	if(cnt > 0 && objects[typ].oc_merge && oclass != SPBOOK_CLASS &&
		(cnt < rnd(6) ||
#ifdef WIZARD
		wizard ||
#endif
		 (cnt <= 7 && Is_candle(otmp)) ||
		 (cnt <= 20 &&
		  ((oclass == WEAPON_CLASS && is_ammo(otmp))
				|| typ == ROCK || is_missile(otmp)))))
			otmp->quan = (long) cnt;

#ifdef WIZARD
	if (oclass == VENOM_CLASS) otmp->spe = 1;
#endif

	if (spesgn == 0) spe = otmp->spe;
#ifdef WIZARD
	else if (wizard) /* no alteration to spe */ ;
#endif
	else if (oclass == ARMOR_CLASS || oclass == WEAPON_CLASS ||
		 is_weptool(otmp) ||
			(oclass==RING_CLASS && objects[typ].oc_charged)) {
		if(spe > rnd(5) && spe > otmp->spe) spe = 0;
		if(spe > 2 && Luck < 0) spesgn = -1;
	} else {
		if (oclass == WAND_CLASS) {
			if (spe > 1 && spesgn == -1) spe = 1;
		} else {
			if (spe > 0 && spesgn == -1) spe = 0;
		}
		if (spe > otmp->spe) spe = otmp->spe;
	}

	if (spesgn == -1) spe = -spe;

	/* set otmp->spe.  This may, or may not, use spe... */
	switch (typ) {
		case TIN: if (contents==EMPTY) {
				otmp->corpsenm = NON_PM;
				otmp->spe = 0;
			} else if (contents==SPINACH) {
				otmp->corpsenm = NON_PM;
				otmp->spe = 1;
			}
			break;
		case SLIME_MOLD: otmp->spe = ftype;
			/* Fall through */
		case SKELETON_KEY: case CHEST: case LARGE_BOX:
		case HEAVY_IRON_BALL: case IRON_CHAIN: case STATUE:
			/* otmp->cobj already done in mksobj() */
				break;
#ifdef MAIL
		case SCR_MAIL: otmp->spe = 1; break;
#endif
		case WAN_WISHING:
#ifdef WIZARD
			if (!wizard) {
#endif
				otmp->spe = (rn2(10) ? -1 : 0);
				break;
#ifdef WIZARD
			}
			/* fall through, if wizard */
#endif
		default: otmp->spe = spe;
	}

	//fprintf(stderr, "readobjnam 700 typ: %d\n", typ); // DE DEBUG
	/* set otmp->corpsenm or dragon scale [mail] */
	if (mntmp >= LOW_PM) {
		if (mntmp == PM_LONG_WORM_TAIL) mntmp = PM_LONG_WORM;

		switch (typ) {
		case TIN:
			otmp->spe = 0; /* No spinach */
			if (dead_species(mntmp, FALSE)) {
			    otmp->corpsenm = NON_PM;	/* it's empty */
			} else if (!(mons[mntmp].geno & G_UNIQ) &&
				   !(mvitals[mntmp].mvflags & G_NOCORPSE) &&
				   mons[mntmp].cnutrit != 0) {
			    otmp->corpsenm = mntmp;
			}
			break;
		case CORPSE:
			if (!(mons[mntmp].geno & G_UNIQ) &&
				   !(mvitals[mntmp].mvflags & G_NOCORPSE)) {
			    /* beware of random troll or lizard corpse,
			       or of ordinary one being forced to such */
			    if (otmp->timed) obj_stop_timers(otmp);
			    if (mons[mntmp].msound == MS_GUARDIAN)
			    	otmp->corpsenm = genus(mntmp,1);
			    else
				otmp->corpsenm = mntmp;
			    start_corpse_timeout(otmp);
			}
			break;
		case FIGURINE:
			if (!(mons[mntmp].geno & G_UNIQ)
			    && !is_human(&mons[mntmp])
#ifdef MAIL
			    && mntmp != PM_MAIL_DAEMON
#endif
							)
				otmp->corpsenm = mntmp;
			break;
		case EGG:
			mntmp = can_be_hatched(mntmp);
			if (mntmp != NON_PM) {
			    otmp->corpsenm = mntmp;
			    if (!dead_species(mntmp, TRUE))
				attach_egg_hatch_timeout(otmp);
			    else
				kill_egg(otmp);
			}
			break;
		case STATUE: otmp->corpsenm = mntmp;
			if (Has_contents(otmp) && verysmall(&mons[mntmp]))
			    delete_contents(otmp);	/* no spellbook */
			otmp->spe = ishistoric ? STATUE_HISTORIC : 0;
			break;
		case SCALE_MAIL:
			/* Dragon mail - depends on the order of objects */
			/*		 & dragons.			 */
			if (mntmp >= PM_GRAY_DRAGON &&
						mntmp <= PM_YELLOW_DRAGON)
			    otmp->otyp = GRAY_DRAGON_SCALE_MAIL +
						    mntmp - PM_GRAY_DRAGON;
			break;
		}
	}

	/* set blessed/cursed -- setting the fields directly is safe
	 * since weight() is called below and addinv() will take care
	 * of luck */
	if (iscursed) {
		curse(otmp);
	} else if (uncursed) {
		otmp->blessed = 0;
		otmp->cursed = (Luck < 0
#ifdef WIZARD
					 && !wizard
#endif
							);
	} else if (blessed) {
		otmp->blessed = (Luck >= 0
#ifdef WIZARD
					 || wizard
#endif
							);
		otmp->cursed = (Luck < 0
#ifdef WIZARD
					 && !wizard
#endif
							);
	} else if (spesgn < 0) {
		curse(otmp);
	}

#ifdef INVISIBLE_OBJECTS
	if (isinvisible) otmp->oinvis = 1;
#endif

	/* set eroded */
	if (is_damageable(otmp) || otmp->otyp == CRYSKNIFE) {
	    if (eroded && (is_flammable(otmp) || is_rustprone(otmp)))
		    otmp->oeroded = eroded;
	    if (eroded2 && (is_corrodeable(otmp) || is_rottable(otmp)))
		    otmp->oeroded2 = eroded2;

	    /* set erodeproof */
	    if (erodeproof && !eroded && !eroded2)
		    otmp->oerodeproof = (Luck >= 0
#ifdef WIZARD
					     || wizard
#endif
					);
	}

	/* set otmp->recharged */
	if (oclass == WAND_CLASS) {
	    /* prevent wishing abuse */
	    if (otmp->otyp == WAN_WISHING
#ifdef WIZARD
		    && !wizard
#endif
		) rechrg = 1;
	    otmp->recharged = (unsigned)rechrg;
	}

	/* set poisoned */
	if (ispoisoned) {
	    if (is_poisonable(otmp))
		otmp->opoisoned = (Luck >= 0);
	    else if (Is_box(otmp) || typ == TIN)
		otmp->otrapped = 1;
	    else if (oclass == FOOD_CLASS)
		/* try to taint by making it as old as possible */
		otmp->age = 1L;
	}

	if (isgreased) otmp->greased = 1;

	if (isdiluted && otmp->oclass == POTION_CLASS &&
			otmp->otyp != POT_WATER)
		otmp->odiluted = 1;

	if (name) {
		const char *aname;
		short objtyp;

		/* an artifact name might need capitalization fixing */
		aname = artifact_name(name, &objtyp);
		if (aname && objtyp == otmp->otyp) name = aname;

		otmp = oname(otmp, name);
		if (otmp->oartifact) {
			otmp->quan = 1L;
			u.uconduct.wisharti++;	/* KMH, conduct */
		}
	}

	/* more wishing abuse: don't allow wishing for certain artifacts */
	/* and make them pay; charge them for the wish anyway! */
	if ((is_quest_artifact(otmp) ||
	     (otmp->oartifact && rn2(nartifact_exist()) > 1))
#ifdef WIZARD
	    && !wizard
#endif
	    ) {
	    artifact_exists(otmp, ONAME(otmp), FALSE);
	    obfree(otmp, (struct obj *) 0);
	    otmp = &zeroobj;
	    pline("Ganz kurz SUBJECT_IM_SATZ VERB_SPUEREN PRONOMEN_PERSONAL OBJECT %s NEUES_OBJECT OBJECT KASUS_DATIV in PRONOMEN_POSSESSIV %s, aber es verschwindet wieder!", /* EN pline("For a moment, you feel %s in your %s, but it disappears!", */
		  something,
		  makeplural(body_part(HAND)));
	}

	if (halfeaten && otmp->oclass == FOOD_CLASS) {
		if (otmp->otyp == CORPSE)
			otmp->oeaten = mons[otmp->corpsenm].cnutrit;
		else otmp->oeaten = objects[otmp->otyp].oc_nutrition;
		/* (do this adjustment before setting up object's weight) */
		consume_oeaten(otmp, 1);
	}
	otmp->owt = weight(otmp);
	if (very && otmp->otyp == HEAVY_IRON_BALL) otmp->owt += 160;

	//fprintf(stderr, "readobjnam 1000 oclass: %d\n", oclass); // DE DEBUG
	return(otmp);
}

int
rnd_class(first,last)
int first,last;
{
	int i, x, sum=0;

	if (first == last)
	    return (first);
	for(i=first; i<=last; i++)
		sum += objects[i].oc_prob;
	if (!sum) /* all zero */
		return first + rn2(last-first+1);
	x = rnd(sum);
	for(i=first; i<=last; i++)
		if (objects[i].oc_prob && (x -= objects[i].oc_prob) <= 0)
			return i;
	return 0;
}

STATIC_OVL const char *
Japanese_item_name(i)
int i;
{
	struct Jitem *j = Japanese_items;

	while(j->item) {
		if (i == j->item)
			return j->name;
		j++;
	}
	return (const char *)0;
}

const char *
cloak_simple_name(cloak)
struct obj *cloak;
{
    if (cloak) {
	switch (cloak->otyp) {
	case ROBE:
	    return "NOUN_ROBE";
	case MUMMY_WRAPPING:
	    return "NOUN_WRAPPING";
	case ALCHEMY_SMOCK:
	    return (objects[cloak->otyp].oc_name_known &&
			cloak->dknown) ? "NOUN_SMOCK" : "NOUN_APRON";
	default:
	    break;
	}
    }
    return "NOUN_CLOAK";
}

const char *
mimic_obj_name(mtmp)
struct monst *mtmp;
{
	if (mtmp->m_ap_type == M_AP_OBJECT && mtmp->mappearance != STRANGE_OBJECT) {
		int idx = objects[mtmp->mappearance].oc_descr_idx;
		if (mtmp->mappearance == GOLD_PIECE) return "NOUN_GOLD"; /* EN if (mtmp->mappearance == GOLD_PIECE) return "gold"; */
		return obj_descr[idx].oc_name;
	}
	return "NOUN_DINGSBUMS"; /* EN return "whatcha-may-callit"; */
}
#endif /* OVLB */

/*objnam.c*/
