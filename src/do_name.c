/*	SCCS Id: @(#)do_name.c	3.4	2003/01/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef GERMAN
# include "german.h"
#endif

#ifdef OVLB

static void FDECL(getpos_help, (BOOLEAN_P,const char *));

extern const char what_is_an_unknown_object[];		/* from pager.c */

/* the response for '?' help request in getpos() */
static void
getpos_help(force, goal)
boolean force;
const char *goal;
{
    char sbuf[BUFSZ];
    boolean doing_what_is;
    winid tmpwin = create_nhwindow(NHW_MENU);

    Sprintf(sbuf, "SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_BENUTZEN [%s] um den Cursor OBJECT KASUS_DATIV zu %s zu steuern.", /* EN Sprintf(sbuf, "Use [%s] to move the cursor to %s.", */
	    iflags.num_pad ? "2468" : "hjkl", goal);
    putstr(tmpwin, 0, sbuf);
    putstr(tmpwin, 0, "SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_BENUTZEN [HJKL] um jeweils 8 Felder zu springen."); /* EN putstr(tmpwin, 0, "Use [HJKL] to move the cursor 8 units at a time."); */
    putstr(tmpwin, 0, "Oder MODIFIER_VERB_IMPERATIV VERB_DRUECKEN ein Hintergrund-Symbol (z.B. <)."); /* EN putstr(tmpwin, 0, "Or enter a background symbol (ex. <)."); */
    /* disgusting hack; the alternate selection characters work for any
       getpos call, but they only matter for dowhatis (and doquickwhatis) */
    doing_what_is = (goal == what_is_an_unknown_object);
    Sprintf(sbuf, "SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_DRUECKEN ein .%s wenn die gewünschte Stelle ausgewählt ist.", /* EN Sprintf(sbuf, "Type a .%s when you are at the right place.", */
            doing_what_is ? " oder , oder ; oder :" : ""); /* EN doing_what_is ? " or , or ; or :" : ""); */
    putstr(tmpwin, 0, sbuf);
    if (!force)
	putstr(tmpwin, 0, "SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_DRUECKEN die Leertaste oder Escape um abzubrechen."); /* EN putstr(tmpwin, 0, "Type Space or Escape when you're done."); */
    putstr(tmpwin, 0, "");
    display_nhwindow(tmpwin, TRUE);
    destroy_nhwindow(tmpwin);
}

int
getpos(cc, force, goal)
coord *cc;
boolean force;
const char *goal;
{
    int result = 0;
    int cx, cy, i, c;
    int sidx, tx, ty;
    boolean msg_given = TRUE;	/* clear message window by default */
    static const char pick_chars[] = ".,;:";
    const char *cp;
    const char *sdp;
    if(iflags.num_pad) sdp = ndir; else sdp = sdir;	/* DICE workaround */

    if (flags.verbose) {
	pline("(Für Hinweise, MODIFIER_VERB_IMPERATIV VERB_DRUECKEN ?)"); /* EN pline("(For instructions type a ?)"); */
	msg_given = TRUE;
    }
    cx = cc->x;
    cy = cc->y;
#ifdef CLIPPING
    cliparound(cx, cy);
#endif
    curs(WIN_MAP, cx,cy);
    flush_screen(0);
#ifdef MAC
    lock_mouse_cursor(TRUE);
#endif
    for (;;) {
	c = nh_poskey(&tx, &ty, &sidx);
	if (c == '\033') {
	    cx = cy = -10;
	    msg_given = TRUE;	/* force clear */
	    result = -1;
	    break;
	}
	if(c == 0) {
	    if (!isok(tx, ty)) continue;
	    /* a mouse click event, just assign and return */
	    cx = tx;
	    cy = ty;
	    break;
	}
	if ((cp = index(pick_chars, c)) != 0) {
	    /* '.' => 0, ',' => 1, ';' => 2, ':' => 3 */
	    result = cp - pick_chars;
	    break;
	}
	for (i = 0; i < 8; i++) {
	    int dx, dy;

	    if (sdp[i] == c) {
		/* a normal movement letter or digit */
		dx = xdir[i];
		dy = ydir[i];
	    } else if (sdir[i] == lowc((char)c)) {
		/* a shifted movement letter */
		dx = 8 * xdir[i];
		dy = 8 * ydir[i];
	    } else
		continue;

	    /* truncate at map edge; diagonal moves complicate this... */
	    if (cx + dx < 1) {
		dy -= sgn(dy) * (1 - (cx + dx));
		dx = 1 - cx;		/* so that (cx+dx == 1) */
	    } else if (cx + dx > COLNO-1) {
		dy += sgn(dy) * ((COLNO-1) - (cx + dx));
		dx = (COLNO-1) - cx;
	    }
	    if (cy + dy < 0) {
		dx -= sgn(dx) * (0 - (cy + dy));
		dy = 0 - cy;		/* so that (cy+dy == 0) */
	    } else if (cy + dy > ROWNO-1) {
		dx += sgn(dx) * ((ROWNO-1) - (cy + dy));
		dy = (ROWNO-1) - cy;
	    }
	    cx += dx;
	    cy += dy;
	    goto nxtc;
	}

	if(c == '?'){
	    getpos_help(force, goal);
	} else {
	    if (!index(quitchars, c)) {
		char matching[MAXPCHARS];
		int pass, lo_x, lo_y, hi_x, hi_y, k = 0;
		(void)memset((genericptr_t)matching, 0, sizeof matching);
		for (sidx = 1; sidx < MAXPCHARS; sidx++)
		    if (c == defsyms[sidx].sym || c == (int)showsyms[sidx])
			matching[sidx] = (char) ++k;
		if (k) {
		    for (pass = 0; pass <= 1; pass++) {
			/* pass 0: just past current pos to lower right;
			   pass 1: upper left corner to current pos */
			lo_y = (pass == 0) ? cy : 0;
			hi_y = (pass == 0) ? ROWNO - 1 : cy;
			for (ty = lo_y; ty <= hi_y; ty++) {
			    lo_x = (pass == 0 && ty == lo_y) ? cx + 1 : 1;
			    hi_x = (pass == 1 && ty == hi_y) ? cx : COLNO - 1;
			    for (tx = lo_x; tx <= hi_x; tx++) {
				/* look at dungeon feature, not at user-visible glyph */
				k = back_to_glyph(tx,ty);
				/* uninteresting background glyph */
				if (glyph_is_cmap(k) &&
				    (IS_DOOR(levl[tx][ty].typ) || /* monsters mimicking a door */
				     glyph_to_cmap(k) == S_room ||
				     glyph_to_cmap(k) == S_corr ||
				     glyph_to_cmap(k) == S_litcorr)) {
					/* what the user remembers to be at tx,ty */
					k = glyph_at(tx, ty);
				}
				/* TODO: - open doors are only matched with '-' */
				/* should remembered or seen items be matched? */
				if (glyph_is_cmap(k) &&
					matching[glyph_to_cmap(k)] &&
					levl[tx][ty].seenv && /* only if already seen */
					(!IS_WALL(levl[tx][ty].typ) &&
					 (levl[tx][ty].typ != SDOOR) &&
					 glyph_to_cmap(k) != S_room &&
					 glyph_to_cmap(k) != S_corr &&
					 glyph_to_cmap(k) != S_litcorr)
				    ) {
				    cx = tx,  cy = ty;
				    if (msg_given) {
					clear_nhwindow(WIN_MESSAGE);
					msg_given = FALSE;
				    }
				    goto nxtc;
				}
			    }	/* column */
			}	/* row */
		    }		/* pass */
		    pline("Kann Kartensymbol '%c' nicht finden.", c); /* EN pline("Can't find dungeon feature '%c'.", c); */
		    msg_given = TRUE;
		    goto nxtc;
		} else {
		    pline("Unbekannte Richtung: '%s' (MODIFIER_VERB_IMPERATIV %s).", /* EN pline("Unknown direction: '%s' (%s).", */
			  visctrl((char)c),
			  !force ? "abgebrochen" : /* EN !force ? "aborted" : */
			  iflags.num_pad ? "VERB_BENUTZEN 2468 or ." : "VERB_BENUTZEN hjkl or ."); /* EN iflags.num_pad ? "use 2468 or ." : "use hjkl or ."); */
		    msg_given = TRUE;
		} /* k => matching */
	    } /* !quitchars */
	    if (force) goto nxtc;
	    pline("Fertig."); /* EN pline("Done."); */
	    msg_given = FALSE;	/* suppress clear */
	    cx = -1;
	    cy = 0;
	    result = 0;	/* not -1 */
	    break;
	}
    nxtc:	;
#ifdef CLIPPING
	cliparound(cx, cy);
#endif
	curs(WIN_MAP,cx,cy);
	flush_screen(0);
    }
#ifdef MAC
    lock_mouse_cursor(FALSE);
#endif
    if (msg_given) clear_nhwindow(WIN_MESSAGE);
    cc->x = cx;
    cc->y = cy;
    return result;
}

struct monst *
christen_monst(mtmp, name)
struct monst *mtmp;
const char *name;
{
	int lth;
	struct monst *mtmp2;
	char buf[PL_PSIZ];

	/* dogname & catname are PL_PSIZ arrays; object names have same limit */
	lth = *name ? (int)(strlen(name) + 1) : 0;
	if(lth > PL_PSIZ){
		lth = PL_PSIZ;
		name = strncpy(buf, name, PL_PSIZ - 1);
		buf[PL_PSIZ - 1] = '\0';
	}
	if (lth == mtmp->mnamelth) {
		/* don't need to allocate a new monst struct */
		if (lth) Strcpy(NAME(mtmp), name);
		return mtmp;
	}
	mtmp2 = newmonst(mtmp->mxlth + lth);
	*mtmp2 = *mtmp;
	(void) memcpy((genericptr_t)mtmp2->mextra,
		      (genericptr_t)mtmp->mextra, mtmp->mxlth);
	mtmp2->mnamelth = lth;
	if (lth) Strcpy(NAME(mtmp2), name);
	replmon(mtmp,mtmp2);
	return(mtmp2);
}

int
do_mname()
{
	char buf[BUFSZ];
	coord cc;
	register int cx,cy;
	register struct monst *mtmp;
	char qbuf[QBUFSZ];

	if (Hallucination) {
		You("MODIFIER_KONJUNKTIV_II VERB_WERDEN es sowieso nicht wiedererkennen."); /* EN You("would never recognize it anyway."); */
		return 0;
	}
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, FALSE, "dem zu benennenden Monster") < 0 || /* EN if (getpos(&cc, FALSE, "the monster you want to name") < 0 || */
			(cx = cc.x) < 0)
		return 0;
	cy = cc.y;

	if (cx == u.ux && cy == u.uy) {
#ifdef STEED
	    if (u.usteed && canspotmon(u.usteed))
		mtmp = u.usteed;
	    else {
#endif
		pline("Diese %s Gestalt heisst %s und kann nicht umbenannt werden.", /* EN pline("This %s creature is called %s and cannot be renamed.", */
		ACURR(A_CHA) > 14 ?
		(flags.female ? "schöne" : "stattliche") : /* EN (flags.female ? "beautiful" : "handsome") : */
		"hässliche", /* EN "ugly", */
		plname);
		return(0);
#ifdef STEED
	    }
#endif
	} else
	    mtmp = m_at(cx, cy);

	if (!mtmp || (!sensemon(mtmp) &&
			(!(cansee(cx,cy) || see_with_infrared(mtmp)) || mtmp->mundetected
			|| mtmp->m_ap_type == M_AP_FURNITURE
			|| mtmp->m_ap_type == M_AP_OBJECT
			|| (mtmp->minvis && !See_invisible)))) {
		pline("Ich kann dort kein Monster erkennen."); /* EN pline("I see no monster there."); */
		return(0);
	}
	/* special case similar to the one in lookat() */
	(void) distant_monnam(mtmp, ARTICLE_THE, buf);
	Sprintf(qbuf, "Wie SUBJECT_IM_SATZ MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL OBJECT %s MODIFIER_VERB_INFINITIV VERB_BENENNEN?", buf); /* EN Sprintf(qbuf, "What do you want to call %s?", buf); */
	getlin(qbuf,buf);
	if(!*buf || *buf == '\033') return(0);
	/* strip leading and trailing spaces; unnames monster if all spaces */
	(void)mungspaces(buf);

	if (mtmp->data->geno & G_UNIQ)
		pline("SUBJECT %s mag keine Spitznamen!", Monnam(mtmp)); /* EN pline("%s doesn't like being called names!", Monnam(mtmp));*/
	else
	    (void) christen_monst(mtmp, buf);
	return(0);
}

/*
 * This routine changes the address of obj. Be careful not to call it
 * when there might be pointers around in unknown places. For now: only
 * when obj is in the inventory.
 */
void
do_oname(obj)
register struct obj *obj;
{
	char buf[BUFSZ], qbuf[QBUFSZ];
#ifdef GERMAN
	char tmp_buf[BUFSZ];
	char *buf_ptr = buf;
#endif
	const char *aname;
	short objtyp;

	Sprintf(qbuf, "Wie SUBJECT_IM_SATZ MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL OBJECT %s %s benamsen?", /* EN Sprintf(qbuf, "What do you want to name %s %s?", */
		is_plural(obj) ? "PRONOMEN_DIESER" : "PRONOMEN_DIESER", xname(obj)); /* EN is_plural(obj) ? "these" : "this", xname(obj)); */
	getlin(qbuf, buf);
	if(!*buf || *buf == '\033')	return;
	/* strip leading and trailing spaces; unnames item if all spaces */
	(void)mungspaces(buf);

	/* relax restrictions over proper capitalization for artifacts */
	if ((aname = artifact_name(buf, &objtyp)) != 0 && objtyp == obj->otyp)
		Strcpy(buf, aname);

	if (obj->oartifact) {
		pline_The("NOUN_ARTIFACT VERB_SCHEINEN dem Versuch zu widerstehen."); /* EN pline_The("artifact seems to resist the attempt."); */
		return;
	} else if (restrict_name(obj, buf) || exist_artifact(obj->otyp, buf)) { /* EN } else if (restrict_name(obj, buf) || exist_artifact(obj->otyp, buf)) { */ // TODO DE
		int n = rn2((int)strlen(buf));
		register char c1, c2;

		c1 = lowc(buf[n]);
		do c2 = 'a' + rn2('z'-'a'); while (c1 == c2);
		buf[n] = (buf[n] == c1) ? c2 : highc(c2);  /* keep same case */
		pline("Beim Einritzen rutscht KASUS_DATIV PRONOMEN_PERSONAL KASUS_NOMINATIV ARTIKEL_BESTIMMTER %s aus.", body_part(HAND)); /* EN pline("While engraving your %s slips.", body_part(HAND)); */
		display_nhwindow(WIN_MESSAGE, FALSE);
		You("VERB_EINRITZEN \"%s\" SATZKLAMMER.",buf); /* EN You("engrave: \"%s\".",buf); */
	}
#ifdef GERMAN
	else {
		german2meta(buf, tmp_buf);
		buf_ptr = tmp_buf;
	}
	obj = oname(obj, buf_ptr);
#else
	obj = oname(obj, buf);
#endif
}

/*
 * Allocate a new and possibly larger storage space for an obj.
 */
struct obj *
realloc_obj(obj, oextra_size, oextra_src, oname_size, name)
struct obj *obj;
int oextra_size;		/* storage to allocate for oextra            */
genericptr_t oextra_src;
int oname_size;			/* size of name string + 1 (null terminator) */
const char *name;
{
	struct obj *otmp;

	otmp = newobj(oextra_size + oname_size);
	*otmp = *obj;	/* the cobj pointer is copied to otmp */
	if (oextra_size) {
	    if (oextra_src)
		(void) memcpy((genericptr_t)otmp->oextra, oextra_src,
							oextra_size);
	} else {
	    otmp->oattached = OATTACHED_NOTHING;
	}
	otmp->oxlth = oextra_size;

	otmp->onamelth = oname_size;
	otmp->timed = 0;	/* not timed, yet */
	otmp->lamplit = 0;	/* ditto */
	/* __GNUC__ note:  if the assignment of otmp->onamelth immediately
	   precedes this `if' statement, a gcc bug will miscompile the
	   test on vax (`insv' instruction used to store bitfield does
	   not set condition codes, but optimizer behaves as if it did).
	   gcc-2.7.2.1 finally fixed this. */
	if (oname_size) {
	    if (name)
		Strcpy(ONAME(otmp), name);
	}

	if (obj->owornmask) {
		boolean save_twoweap = u.twoweap;
		/* unwearing the old instance will clear dual-wield mode
		   if this object is either of the two weapons */
		setworn((struct obj *)0, obj->owornmask);
		setworn(otmp, otmp->owornmask);
		u.twoweap = save_twoweap;
	}

	/* replace obj with otmp */
	replace_object(obj, otmp);

	/* fix ocontainer pointers */
	if (Has_contents(obj)) {
		struct obj *inside;

		for(inside = obj->cobj; inside; inside = inside->nobj)
			inside->ocontainer = otmp;
	}

	/* move timers and light sources from obj to otmp */
	if (obj->timed) obj_move_timers(obj, otmp);
	if (obj->lamplit) obj_move_light_source(obj, otmp);

	/* objects possibly being manipulated by multi-turn occupations
	   which have been interrupted but might be subsequently resumed */
	if (obj->oclass == FOOD_CLASS)
	    food_substitution(obj, otmp);	/* eat food or open tin */
	else if (obj->oclass == SPBOOK_CLASS)
	    book_substitution(obj, otmp);	/* read spellbook */

	/* obfree(obj, otmp);	now unnecessary: no pointers on bill */
	dealloc_obj(obj);	/* let us hope nobody else saved a pointer */
	return otmp;
}

struct obj *
oname(obj, name)
struct obj *obj;
const char *name;
{
	int lth;
	char buf[PL_PSIZ];

	lth = *name ? (int)(strlen(name) + 1) : 0;
	if (lth > PL_PSIZ) {
		lth = PL_PSIZ;
		name = strncpy(buf, name, PL_PSIZ - 1);
		buf[PL_PSIZ - 1] = '\0';
	}
	/* If named artifact exists in the game, do not create another.
	 * Also trying to create an artifact shouldn't de-artifact
	 * it (e.g. Excalibur from prayer). In this case the object
	 * will retain its current name. */
	if (obj->oartifact || (lth && exist_artifact(obj->otyp, name)))
		return obj;

	if (lth == obj->onamelth) {
		/* no need to replace entire object */
		if (lth) Strcpy(ONAME(obj), name);
	} else {
		obj = realloc_obj(obj, obj->oxlth,
			      (genericptr_t)obj->oextra, lth, name);
	}
	if (lth) artifact_exists(obj, name, TRUE);
	if (obj->oartifact) {
	    /* can't dual-wield with artifact as secondary weapon */
	    if (obj == uswapwep) untwoweapon();
	    /* activate warning if you've just named your weapon "Sting" */
	    if (obj == uwep) set_artifact_intrinsic(obj, TRUE, W_WEP);
	}
	if (carried(obj)) update_inventory();
	return obj;
}

static NEARDATA const char callable[] = {
	SCROLL_CLASS, POTION_CLASS, WAND_CLASS, RING_CLASS, AMULET_CLASS,
	GEM_CLASS, SPBOOK_CLASS, ARMOR_CLASS, TOOL_CLASS, 0 };

int
ddocall()
{
	register struct obj *obj;
#ifdef REDO
	char	ch;
#endif
	char allowall[2];

	switch(
#ifdef REDO
		ch =
#endif
		ynq("Ein einzelnes Objekt benamsen?")) { /* EN ynq("Name an individual object?")) { */
	case 'q':
		break;
	case 'y':
#ifdef REDO
		savech(ch);
#endif
		allowall[0] = ALL_CLASSES; allowall[1] = '\0';
		obj = getobj(allowall, "benamsen"); /* EN obj = getobj(allowall, "name"); */
		if(obj) do_oname(obj);
		break;
	default :
#ifdef REDO
		savech(ch);
#endif
		obj = getobj(callable, "VERB_BENENNEN"); /* EN obj = getobj(callable, "call"); */
		if (obj) {
			/* behave as if examining it in inventory;
			   this might set dknown if it was picked up
			   while blind and the hero can now see */
			(void) xname(obj);

			if (!obj->dknown) {
				You("MODIFIER_KONJUNKTIV_II VERB_WERDEN kein anderes Stück davon erkennen."); /* EN You("would never recognize another one."); */
				return 0;
			}
			docall(obj);
		}
		break;
	}
	return 0;
}

void
docall(obj)
register struct obj *obj;
{
	char buf[BUFSZ], qbuf[QBUFSZ];
	struct obj otemp;
	register char **str1;

	if (!obj->dknown) return; /* probably blind */
	otemp = *obj;
	otemp.quan = 1L;
	otemp.onamelth = 0;
	otemp.oxlth = 0;
	if (objects[otemp.otyp].oc_class == POTION_CLASS && otemp.fromsink)
	    /* kludge, meaning it's sink water */
	    Sprintf(qbuf,"SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_BENENNEN OBJECT ARTIKEL_UNBESTIMMTER %s NOUN_FLUESSIGKEIT:", /* EN Sprintf(qbuf,"Call a stream of %s fluid:", */
		    OBJ_DESCR(objects[otemp.otyp]));
	else
	    Sprintf(qbuf, "SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_BENENNEN %s:", an(xname(&otemp))); /* EN Sprintf(qbuf, "Call %s:", an(xname(&otemp))); */
	getlin(qbuf, buf);
	if(!*buf || *buf == '\033')
		return;

	/* clear old name */
	str1 = &(objects[obj->otyp].oc_uname);
	if(*str1) free((genericptr_t)*str1);

	/* strip leading and trailing spaces; uncalls item if all spaces */
	(void)mungspaces(buf);
	if (!*buf) {
	    if (*str1) {	/* had name, so possibly remove from disco[] */
		/* strip name first, for the update_inventory() call
		   from undiscover_object() */
		*str1 = (char *)0;
		undiscover_object(obj->otyp);
	    }
	} else {
	    *str1 = strcpy((char *) alloc((unsigned)strlen(buf)+1), buf);
	    discover_object(obj->otyp, FALSE, TRUE); /* possibly add to disco[] */
	}
}

#endif /*OVLB*/
#ifdef OVL0

static const char * const ghostnames[] = {
	/* these names should have length < PL_NSIZ */
	/* Capitalize the names for aesthetics -dgk */
	// TODO DE?
	"Adri", "Andries", "Andreas", "Bert", "David", "Dirk", "Emile",
	"Frans", "Fred", "Greg", "Hether", "Jay", "John", "Jon", "Karnov",
	"Kay", "Kenny", "Kevin", "Maud", "Michiel", "Mike", "Peter", "Robert",
	"Ron", "Tom", "Wilmar", "Nick Danger", "Phoenix", "Jiro", "Mizue",
	"Stephan", "Lance Braccus", "Shadowhawk"
#ifdef GERMAN
	, /* from #nethack.de on irc.freenode.net */
	"Airmack", "Bhaak", "Blackpenguin", "Deki", "Lord Hoto", "Nightwulf",
	"Peutri", "Robbi", "Rossi", "Schnippi", "Squolly", "Xororand"
#endif
};

/* ghost names formerly set by x_monnam(), now by makemon() instead */
const char *
rndghostname()
{
    return rn2(7) ? ghostnames[rn2(SIZE(ghostnames))] : (const char *)plname;
}

/* Monster naming functions:
 * x_monnam is the generic monster-naming function.
 *		  seen	      unseen	   detected		  named
 * mon_nam:	the newt	it	the invisible orc	Fido
 * noit_mon_nam:the newt (as if detected) the invisible orc	Fido
 * l_monnam:	newt		it	invisible orc		dog called fido
 * Monnam:	The newt	It	The invisible orc	Fido
 * noit_Monnam: The newt (as if detected) The invisible orc	Fido
 * Adjmonnam:	The poor newt	It	The poor invisible orc	The poor Fido
 * Amonnam:	A newt		It	An invisible orc	Fido
 * a_monnam:	a newt		it	an invisible orc	Fido
 * m_monnam:	newt		xan	orc			Fido
 * y_monnam:	your newt     your xan	your invisible orc	Fido
 */

/* Bug: if the monster is a priest or shopkeeper, not every one of these
 * options works, since those are special cases.
 */
char *
x_monnam(mtmp, article, adjective, suppress, called)
register struct monst *mtmp;
int article;
/* ARTICLE_NONE, ARTICLE_THE, ARTICLE_A: obvious
 * ARTICLE_YOUR: "your" on pets, "the" on everything else
 *
 * If the monster would be referred to as "it" or if the monster has a name
 * _and_ there is no adjective, "invisible", "saddled", etc., override this
 * and always use no article.
 */
const char *adjective;
int suppress;
/* SUPPRESS_IT, SUPPRESS_INVISIBLE, SUPPRESS_HALLUCINATION, SUPPRESS_SADDLE.
 * EXACT_NAME: combination of all the above
 */
boolean called;
{
#ifdef LINT	/* static char buf[BUFSZ]; */
	char buf[BUFSZ];
#else
	static char buf[BUFSZ];
#endif
	struct permonst *mdat = mtmp->data;
	boolean do_hallu, do_invis, do_it, do_saddle;
	boolean name_at_start, has_adjectives;
	char *bp;

	if (program_state.gameover)
	    suppress |= SUPPRESS_HALLUCINATION;
	if (article == ARTICLE_YOUR && !mtmp->mtame)
	    article = ARTICLE_THE;

	do_hallu = Hallucination && !(suppress & SUPPRESS_HALLUCINATION);
	do_invis = mtmp->minvis && !(suppress & SUPPRESS_INVISIBLE);
	do_it = !canspotmon(mtmp) && 
	    article != ARTICLE_YOUR &&
	    !program_state.gameover &&
#ifdef STEED
	    mtmp != u.usteed &&
#endif
	    !(u.uswallow && mtmp == u.ustuck) &&
	    !(suppress & SUPPRESS_IT);
	do_saddle = !(suppress & SUPPRESS_SADDLE);

	buf[0] = 0;

	/* unseen monsters, etc.  Use "it" */
	if (do_it) {
	    Strcpy(buf, "NOUN_IT"); /* EN Strcpy(buf, "it"); */
	    return buf;
	}

	//printf("a: %s\n", mtmp->data->mname);
	/*
	if (is_male(mdat)) { Strcpy(buf, "sex: m "); }
	else if (is_female(mdat)) { Strcpy(buf, "sex: f "); }
	else if (is_neuter(mdat)) { Strcpy(buf, "sex: n "); }
	else { Strcpy(buf, "sex: gar nix "); }

	if (is_neuter(mdat)) { Strcpy(buf, "sex2: n "); }
	else if (mtmp->female) { Strcpy(buf, "sex2: f "); }
	else { Strcpy(buf, "sex2: m "); }

	if (article == ARTICLE_YOUR) { Strcat(buf, " :article_your1:"); }
	if (article == ARTICLE_THE)  { Strcat(buf, " :article_the1:"); }
	if (article == ARTICLE_NONE) { Strcat(buf, " :article_none1:"); }


 if (type_is_pname(mdat)) { Strcat(buf, " :name: "); }
  else { Strcat(buf, " :kein name: "); }
	*/

	/* priests and minions: don't even use this function */
	if (mtmp->ispriest || mtmp->isminion) {
	    char priestnambuf[BUFSZ];
	    char *name;
	    long save_prop = EHalluc_resistance;
	    unsigned save_invis = mtmp->minvis;

	    /* when true name is wanted, explicitly block Hallucination */
	    if (!do_hallu) EHalluc_resistance = 1L;
	    if (!do_invis) mtmp->minvis = 0;
	    name = priestname(mtmp, priestnambuf);
	    EHalluc_resistance = save_prop;
	    mtmp->minvis = save_invis;
	    if (article == ARTICLE_NONE && !strncmp(name, "ARTIKEL_BESTIMMTER ", 19)) /* EN if (article == ARTICLE_NONE && !strncmp(name, "the ", 4)) */
		name += 19;
	    return strcpy(buf, name);
	}

	/* Shopkeepers: use shopkeeper name.  For normal shopkeepers, just
	 * "Asidonhopo"; for unusual ones, "Asidonhopo the invisible
	 * shopkeeper" or "Asidonhopo the blue dragon".  If hallucinating,
	 * none of this applies.
	 */
	if (mtmp->isshk && !do_hallu) {
	    if (adjective && article == ARTICLE_THE) {
		/* pathological case: "the angry Asidonhopo the blue dragon"
		   sounds silly */
		Strcpy(buf, "ARTIKEL_BESTIMMTER "); /* EN Strcpy(buf, "the "); */
		Strcat(strcat(buf, adjective), " ");
		Strcat(buf, shkname(mtmp));
		return buf;
	    }
	    Strcat(buf, shkname(mtmp));
	    if (mdat == &mons[PM_SHOPKEEPER] && !do_invis)
		return buf;
	    Strcat(buf, " ARTIKEL_BESTIMMTER "); /* EN Strcat(buf, " the "); */
	    if (do_invis) 
		Strcat(buf, "ADJEKTIV_INVISIBLE "); /* EN Strcat(buf, "invisible "); */
	    Strcat(buf, mdat->mname);
#ifdef GERMAN
	    if (mtmp->female) { Strcat(buf, "IN"); }
#endif
	    return buf;
	}

	/* Put the adjectives in the buffer */
	if (adjective)
	    Strcat(strcat(buf, adjective), " ");
	if (do_invis)
	    Strcat(buf, "ADJEKTIV_INVISIBLE "); /* EN Strcat(buf, "invisible "); */
#ifdef STEED
	if (do_saddle && (mtmp->misc_worn_check & W_SADDLE) &&
	    !Blind && !Hallucination)
	    Strcat(buf, "ADJEKTIV_SADDLED "); /* EN Strcat(buf, "saddled "); */
#endif
	if (buf[0] != 0)
	    has_adjectives = TRUE;
	else
	    has_adjectives = FALSE;

	/* Put the actual monster name or type into the buffer now */
	/* Be sure to remember whether the buffer starts with a name */
	if (do_hallu) {
	    Strcat(buf, rndmonnam());
	    name_at_start = FALSE;
#ifdef GERMAN
	    /* Hallucinated monster names containing the keyword 'UNIQUE'
	       don't need an article (jpeg) unless they have adjectives. */
	    if (strstri(buf, "UNIQUE") && !has_adjectives) {
		name_at_start = TRUE;
		article = ARTICLE_NONE;
	    }
#endif
	} else if (mtmp->mnamelth) {
	    char *name = NAME(mtmp);

	    if (mdat == &mons[PM_GHOST]) {
		Sprintf(eos(buf), "%s NOUN_GHOST", s_suffix(name)); /* EN Sprintf(eos(buf), "%s ghost", s_suffix(name)); */
		name_at_start = TRUE;
	    } else if (called) {
		Sprintf(eos(buf), "%s genannt %s", mdat->mname, name); /* EN Sprintf(eos(buf), "%s called %s", mdat->mname, name); */
		name_at_start = (boolean)type_is_pname(mdat);
	    } else if (is_mplayer(mdat) && (bp = strstri(name, " ARTIKEL_BESTIMMTER ")) != 0) { /* EN } else if (is_mplayer(mdat) && (bp = strstri(name, " the ")) != 0) { */
		/* <name> the <adjective> <invisible> <saddled> <rank> */
		char pbuf[BUFSZ];

		Strcpy(pbuf, name);
		pbuf[bp - name + 20] = '\0'; /* adjectives right after " the " */
		if (has_adjectives)
		    Strcat(pbuf, buf);
		Strcat(pbuf, bp + 20);	/* append the rest of the name */
		Strcpy(buf, pbuf);
		article = ARTICLE_NONE;
		name_at_start = TRUE;
	    } else {
	        //printf("name_at_start\n");
#ifdef GERMAN
		Strcat(buf, (mtmp->female) ? "NOUN_PSEUDO_WEIBLICH " : "NOUN_PSEUDO_MAENNLICH ");
#endif
		Strcat(buf, name);
		name_at_start = TRUE;
	    }
	} else if (is_mplayer(mdat) && !In_endgame(&u.uz)) {
	    char pbuf[BUFSZ];
	    Strcpy(pbuf, rank_of((int)mtmp->m_lev,
				 monsndx(mdat),
				 (boolean)mtmp->female));
#ifdef GERMAN
	    Strcat(buf, pbuf); // ansonsten ergibt Ctrl-G Priester ein Monster namens noun_priester
#else
	    Strcat(buf, lcase(pbuf));
#endif
	    name_at_start = FALSE;
	} else {
	    Strcat(buf, mdat->mname);
	    name_at_start = (boolean)type_is_pname(mdat);
	}

	/*
		if (name_at_start) { Strcat(buf, " :name_at_start: "); }
		if (has_adjectives) { Strcat(buf, " :has_adjectives: "); }
		*/

	if (name_at_start && (article == ARTICLE_YOUR || !has_adjectives)) {
		/* Strcat(buf, " :1: "); */
	    if (mdat == &mons[PM_WIZARD_OF_YENDOR])
		article = ARTICLE_THE;
	    else {
		article = ARTICLE_NONE;
			}
	} else if ((mdat->geno & G_UNIQ) && article == ARTICLE_A) {
		/* Strcat(buf, " :2: "); */
	    article = ARTICLE_THE;
	}

	{
	    char buf2[BUFSZ];

	    switch(article) {
		case ARTICLE_YOUR:
				/* Strcpy(buf2, " :ARTICLE_YOUR: "); */
		    Strcpy(buf2, "PRONOMEN_POSSESSIV "); /* EN Strcpy(buf2, "your "); */
		    Strcat(buf2, buf);
		    Strcpy(buf, buf2);
		    return buf;
		case ARTICLE_THE:
				/* Strcat(buf2, " :ARTICLE_THE: "); */
		    Strcpy(buf2, "ARTIKEL_BESTIMMTER "); /* EN Strcpy(buf2, "the "); */
		    Strcat(buf2, buf);
		    Strcpy(buf, buf2);
		    return buf;
		case ARTICLE_A:
				/* Strcat(buf2, " :ARTICLE_A: "); */
			/* return(an(buf)); */
		    Strcpy(buf2, "ARTIKEL_UNBESTIMMTER ");
		    Strcat(buf2, buf);
		    Strcpy(buf, buf2);
		case ARTICLE_NONE:
				/* Strcat(buf2, " :ARTICLE_NONE: "); */
		default:
		    return buf;
	    }
	}
}

#endif /* OVL0 */
#ifdef OVLB

char *
l_monnam(mtmp)
register struct monst *mtmp;
{
	return(x_monnam(mtmp, ARTICLE_NONE, (char *)0, 
		mtmp->mnamelth ? SUPPRESS_SADDLE : 0, TRUE));
}

#endif /* OVLB */
#ifdef OVL0

char *
mon_nam(mtmp)
register struct monst *mtmp;
{
	return(x_monnam(mtmp, ARTICLE_THE, (char *)0,
		mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE));
}

/* print the name as if mon_nam() was called, but assume that the player
 * can always see the monster--used for probing and for monsters aggravating
 * the player with a cursed potion of invisibility
 */
char *
noit_mon_nam(mtmp)
register struct monst *mtmp;
{
	return(x_monnam(mtmp, ARTICLE_THE, (char *)0,
		mtmp->mnamelth ? (SUPPRESS_SADDLE|SUPPRESS_IT) :
		    SUPPRESS_IT, FALSE));
}

char *
Monnam(mtmp)
register struct monst *mtmp;
{
	register char *bp = mon_nam(mtmp);

	*bp = highc(*bp);
	return(bp);
}

char *
noit_Monnam(mtmp)
register struct monst *mtmp;
{
	register char *bp = noit_mon_nam(mtmp);

	*bp = highc(*bp);
	return(bp);
}

/* monster's own name */
char *
m_monnam(mtmp)
struct monst *mtmp;
{
	return x_monnam(mtmp, ARTICLE_NONE, (char *)0, EXACT_NAME, FALSE);
}

/* pet name: "your little dog" */
char *
y_monnam(mtmp)
struct monst *mtmp;
{
	int prefix, suppression_flag;

	prefix = mtmp->mtame ? ARTICLE_YOUR : ARTICLE_THE;
	suppression_flag = (mtmp->mnamelth
#ifdef STEED
			    /* "saddled" is redundant when mounted */
			    || mtmp == u.usteed
#endif
			    ) ? SUPPRESS_SADDLE : 0;

	return x_monnam(mtmp, prefix, (char *)0, suppression_flag, FALSE);
}

#endif /* OVL0 */
#ifdef OVLB

char *
Adjmonnam(mtmp, adj)
register struct monst *mtmp;
register const char *adj;
{
	register char *bp = x_monnam(mtmp, ARTICLE_THE, adj,
		mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE);

	*bp = highc(*bp);
	return(bp);
}

char *
a_monnam(mtmp)
register struct monst *mtmp;
{
	return x_monnam(mtmp, ARTICLE_A, (char *)0,
		mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE);
}

char *
Amonnam(mtmp)
register struct monst *mtmp;
{
	register char *bp = a_monnam(mtmp);

	*bp = highc(*bp);
	return(bp);
}

/* used for monster ID by the '/', ';', and 'C' commands to block remote
   identification of the endgame altars via their attending priests */
char *
distant_monnam(mon, article, outbuf)
struct monst *mon;
int article;	/* only ARTICLE_NONE and ARTICLE_THE are handled here */
char *outbuf;
{
    /* high priest(ess)'s identity is concealed on the Astral Plane,
       unless you're adjacent (overridden for hallucination which does
       its own obfuscation) */
    if (mon->data == &mons[PM_HIGH_PRIEST] && !Hallucination &&
	    Is_astralevel(&u.uz) && distu(mon->mx, mon->my) > 2) {
	Strcpy(outbuf, article == ARTICLE_THE ? "ARTIKEL_BESTIMMTER " : ""); /* EN Strcpy(outbuf, article == ARTICLE_THE ? "the " : ""); */
	Strcat(outbuf, mon->female ? "NOUN_HIGH_PRIESTESS" : "NOUN_HIGH_PRIEST"); /* EN Strcat(outbuf, mon->female ? "high priestess" : "high priest"); */
    } else {
	Strcpy(outbuf, x_monnam(mon, article, (char *)0, 0, TRUE));
    }
    return outbuf;
}

#if ENGLISH
static const char * const bogusmons[] = {
	/* Translated (or used as-is) below */
	"giant pigmy", "gnu", "killer penguin", "giant cockroach", "giant slug",
	"maggot", "pterodactyl", "tyrannosaurus rex", "basilisk", "nightmare",
	"bookworm", "shadow", "hologram", "jester", "attorney", "killer tomato",
	"amazon", "robot", "rhinovirus", "harpy",  /* misc */
	"grue", "Christmas-tree monster",	/* Quendor (Zork, &c.) */
	"emu", "venus flytrap",			/* Rogue */
	"hydra", "siren",			/* Greek legend */
	"killer bunny",				/* Monty Python */
	"rodent of unusual size",		/* The Princess Bride */
	"Ent",					/* Lord of the Rings */
	"white rabbit", "snark",		/* Lewis Carroll */
	"pushmi-pullyu",			/* Dr. Doolittle */
	"smurf",				/* The Smurfs */
	"Klingon", "Borg",			/* Star Trek */
	"Ewok",					/* Star Wars */
	"Godzilla", "King Kong",		/* monster movies */
	"Terminator",				/* The Terminator */
	"Dalek",				/* Dr. Who ("Exterminate!") */
	"Ravenous Bugblatter Beast of Traal",	/* HGttG */
	"Morgoth",				/* Angband */
	"mother-in-law",			/* common pest */
	"Vorlon",				/* Babylon 5 */
	"Predator",				/* Movie */
	"Totoro",				/* Tonari no Totoro */
	"Luggage",				/* Discworld */
	"Audrey II",				/* Little Shop of Horrors */
	"youma",				/* Sailor Moon */

	/* Untranslated (unused) */
	"jumbo shrimp", "beholder", "efreeti", "marid", "rot grub",
	"master lichen", "sleazoid", "battlemech", "lion-dog", "rat-ant",
	"Y2K bug",				/* misc. */
	"luck sucker", "paskald", "brogmoid", "dornbeast",	/* Quendor (Zork, &c.) */
	"Ancient Multi-Hued Dragon", "Evil Iggy",	/* Moria */
	"kestrel", "xeroc",			/* Rogue */
	"creeping coins",			/* Wizardry */
	"Smokey the bear",			/* "Only you can prevent forest fires!" */
	"tangle tree", "nickelpede", "wiggle",	/* Xanth */
	"tribble",				/* Star Trek */
	"ohmu",					/* Nausicaa */
	"nyaasu",				/* Pokemon (Meowth) */
	"earthquake beast",			/* old L of SH */
	"Invid",				/* Robotech */
	"boomer",				/* Bubblegum Crisis */
	"microscopic space fleet",		/* HGttG */
	"teenage mutant ninja turtle",		/* TMNT */
	"samurai rabbit",			/* Usagi Yojimbo */
	"aardvark",				/* Cerebus */
	"witch doctor", "one-eyed one-horned flying purple people eater",
						/* 50's rock 'n' roll */
	"Barney the dinosaur",			/* saccharine kiddy TV */
	"questing beast",			/* King Arthur */
};
#else
	/* http://de.wikipedia.org/wiki/Liste_der_Fabelwesen#Deutschland_.26_.C3.96sterreich */

	// Blaubär
	// Natiftoffen ?

#if 0
	// Eigennamen: "X trifft. Du verfehlst X."
	// Horla, Godzilla, King Kong, Morgoth
	// neu: Frankenstein, Loreley, Ötzi, Rübezahl, Grendel,
	//      Baba Jaga, Väterchen Frost, Pumuckl, Knecht Ruprecht,
	//      Capricorn  // Tintenherz
	//      Bernd das Brot, Chili das Schaf  // Chili TV
	//      Gevatter Tod  // Scheibenwelt
	//      Homunkoloss  // Die Stadt der träumenden Bücher
	//      Dr. Caligari, Jack the Ripper, ET
	//      Herr von Bödefeld  // Sesamstraße
	
	// maskulin: "Der X trifft. Du verfehlst den X."
	// Basilisk, Hofnarr, Riesenpygmäe, Gefräßiger Plapperkäfer (von/aus Traal),
	// Tyrannosaurus Rex, Killerpinguin, Pterodaktylus, Nachtmahr/Alptraum,
	// Bücherwurm, Schatten, Roboter, Grue, Schlumpf, (Staats)anwalt, Klingon,
	// Borg, Ewok, Terminator, Ent, Dalek, Schnatz
	// neu: Ottifant, Elwetritsch, Wolpertinger, Klammeraffe, weißer Hai,
	//      Vogone  // Per Anhalter durch die Galaxis
	//      Eideet  // Moers' Zamonienliteratur
	//      Schrumpfhörniger Schnarchkackler  // Harry Potter
	//      Steuerprüfer, GEZ-Fahnder, Großinquisitor, Killerklown,
	//      Klabautermann, Dodo, Grinch, Sandmann, Mummelratz, Apparatschik,
	//      Erlkönig, Räuber Hotzenplotz, Sensenmann, Menschenfresser,
	//      Riesengartenzwerg, Quastenflosser, Säbelzahntiger
	//      Bi-Ba-Butzemann  // Kinderreim
	//      Ritter der Kokosnuß  // Monty Python
	//      Zonk  // Geh aufs Ganze!
	
	// femininum: "Die X trifft. Du verfehlst die X."
	// Killertomate, Amazone, Harpyie, Venusfliegenfalle, Hydra, Sirene,
	// Schwiegermutter, Riesenkakerlake, Riesenschnecke, Made
	// neu: eierlegende Wollmilchsau, Leseratte, Riesenwelle, Zahnfee,
	//      Roggenmuhme, Doppelhelix
	//      Steinlaus  // Loriot
	//      kleine Raupe Nimmersatt  // Kinderbuch
	
	// neutrum: "Das X trifft. Du verfehlst das X."
	// Gnu, Hologram, Emu, Nagetier in ungewöhnlichem Format, Rhinovirus,
	// Weihnachtsbaummonster, weißes Kaninchen, Killerkarnickel,
	// Stoßmich-Ziehdich
	// neu: Unsichtbares rosafarbenes Einhorn, Fliegendes Spaghettimonster,
	//      Faultier, Pantoffeltierchen, UFO, Kartoffelpü-Reh, Mondkalb
	//      Blumentopferd, Krümelmonster, Sams, Vakuum, Heinzelmännchen,
	//      Ungeheuer von Loch Ness, Rhinogradentium, Alter Ego, Schnabeltier
#endif

static const char * const bogusmons[] = {
	/* names kept or translated from original NetHack */
	/* proper names */
	"NOUN_HALLU_UNIQUE_GODZILLA", "NOUN_HALLU_UNIQUE_KING_KONG", /* monster movies */
	"NOUN_HALLU_UNIQUE_MORGOTH",		/* Angband */
	"NOUN_HALLU_UNIQUE_AUDREY_II",		/* Little Shop of Horrors */
	/* other */
	"NOUN_HALLU_TYRANNOSAURUS_REX", "NOUN_HALLU_PTERODAKTYLUS",
	"NOUN_HALLU_KILLERPINGUIN", "NOUN_HALLU_NACHTMAHR", "NOUN_HALLU_SCHATTEN",
	"NOUN_HALLU_ROBOTER", "NOUN_HALLU_TERMINATOR", "NOUN_HALLU_HOFNARR",
	"NOUN_HALLU_HORLA",			/* Guy de Maupassant */
	"NOUN_HALLU_GRUE",			/* Zork */
	"NOUN_HALLU_SCHLUMPF",			/* Die Schlümpfe */
	"NOUN_HALLU_KLINGON",
	"NOUN_HALLU_BORG",			/* Star Trek */
	"NOUN_HALLU_EWOK",			/* Star Wars */
	"NOUN_HALLU_DALEK",			/* Dr. Who */
	"NOUN_HALLU_VORLON",			/* Babylon 5 */
	"NOUN_HALLU_TOTORO",			/* Tonari no Totoro */
	"NOUN_HALLU_PREDATOR",			/* movie */
	"NOUN_HALLU_ENT",			/* Herr der Ringe */
	"NOUN_HALLU_SCHNATZ",			/* translation of 'Snark', Lewis Carroll */
	"NOUN_HALLU_BASILISK", "NOUN_HALLU_HARPYIE", "NOUN_HALLU_HYDRA",
	"NOUN_HALLU_SIRENE",			/* Greek stories */
	"NOUN_HALLU_GEFRAESSIGER_PLAPPERKAEFER", /* HGttG */
	"NOUN_HALLU_EMU",
	"NOUN_HALLU_VENUSFLIEGENFALLE",		/* Rogue */
	"NOUN_HALLU_STAATSANWALT",
	"NOUN_HALLU_SCHWIEGERMUTTER",		/* real-world pests */
	"NOUN_HALLU_NIUF",			/* Princess Bride */
	"NOUN_HALLU_STOSSMICH_ZIEHDICH",	/* Dr. Doolittle */
	"NOUN_HALLU_KILLERKANINCHEN",		/* Monty Python */
	"NOUN_HALLU_RIESENPYGMAEE",
	"NOUN_HALLU_KILLERTOMATE",
	"NOUN_HALLU_AMAZONE",
	"NOUN_HALLU_RIESENKAKERLAKE",
	"NOUN_HALLU_GNU",
	"NOUN_HALLU_HOLOGRAMM",
	"NOUN_HALLU_RHINOVIRUS",
	"NOUN_HALLU_WEIHNACHTSBAUMMONSTER",
	
	/* new in NetHack-De */
	/* proper names */
	"NOUN_HALLU_UNIQUE_FRANKENSTEIN", "NOUN_HALLU_UNIQUE_DR_CALIGARI",
	"NOUN_HALLU_UNIQUE_ET", "NOUN_HALLU_UNIQUE_LORELEY",
	"NOUN_HALLU_UNIQUE_RUEBEZAHL", "NOUN_HALLU_UNIQUE_GRENDEL",
	"NOUN_HALLU_UNIQUE_BABA_JAGA", "NOUN_HALLU_UNIQUE_VAETERCHEN_FROST",
	"NOUN_HALLU_UNIQUE_OETZI", "NOUN_HALLU_UNIQUE_JACK_THE_RIPPER",
	"NOUN_HALLU_UNIQUE_KNECHT_RUPRECHT",
	"NOUN_HALLU_UNIQUE_PUMUCKL",		/* Pumuckl */
	"NOUN_HALLU_UNIQUE_HERR_VON_BOEDEFELD",
	"NOUN_HALLU_KRUEMELMONSTER",		/* Sesamstraße */
	"NOUN_HALLU_UNIQUE_BERND_DAS_BROT", "NOUN_HALLU_UNIQUE_CHILI_DAS_SCHAF", /* Chili TV */
	"NOUN_HALLU_UNIQUE_CAPRICORN",		/* Tintenherz */
	"NOUN_HALLU_UNIQUE_GEVATTER_TOD",	/* Scheibenwelt */
	"NOUN_HALLU_UNIQUE_HOMUNKOLOSS",	/* Walter Moers */
	"NOUN_HALLU_UNIQUE_TINKY_WINKY",	/* Teletubbies */
	"NOUN_HALLU_UNIQUE_PITIPLATSCH",	/* Der Sandmann */
	"NOUN_HALLU_UNIQUE_SEIFENSTEIN",
	"NOUN_HALLU_UNIQUE_VORMAERZ",		/* Wir sind Kaiser */
	"NOUN_HALLU_UNIQUE_BEUTOLOMAEUS",
	"NOUN_HALLU_UNIQUE_IRRENHAUS_DER_DRITTE", /* http://www.youtube.com/user/Irrenhaus3 */
	/* other */
	"NOUN_HALLU_STEUERPRUEFER",
	"NOUN_HALLU_GEZ_FAHNDER",		/* real-world pests */
	"NOUN_HALLU_GROSSINQUISITOR", "NOUN_HALLU_MENSCHENFRESSER",
	"NOUN_HALLU_KLABAUTERMANN", "NOUN_HALLU_SENSENMANN",
	"NOUN_HALLU_RIESENGARTENZWERG", "NOUN_HALLU_WEISSER_HAI",
	"NOUN_HALLU_KLAMMERAFFE", "NOUN_HALLU_EINARMIGER_BANDIT",
	"NOUN_HALLU_ELWETRITSCH", "NOUN_HALLU_WOLPERTINGER",
	"NOUN_HALLU_MUMMELRATZ",
	"NOUN_HALLU_ROGGENMUHME",		/* German fairytale critters */
	"NOUN_HALLU_DODO", "NOUN_HALLU_QUASTENFLOSSER",
	"NOUN_HALLU_SAEBELZAHNTIGER",		/* extinct animals */
	"NOUN_HALLU_GRINCH",			/* The Grinch */
	"NOUN_HALLU_APPARATSCHIK",		/* GDR term */
	"NOUN_HALLU_ERLKOENIG",			/* Goethe */
	"NOUN_HALLU_RAEUBER_HOTZENPLOTZ",	/* Räuber Hotzenplot */
	"NOUN_HALLU_BIBABUTZEMANN",		/* children's game */
	"NOUN_HALLU_RITTER_DER_KOKOSNUSS",	/* Monty Python */
	"NOUN_HALLU_ZONK",			/* Geh aufs Ganze! */
	"NOUN_HALLU_OTTIFANT",			/* Otto */
	"NOUN_HALLU_VOGONE",			/* Per Anhalter durch die Galaxis */
	"NOUN_HALLU_STOLLENTROLL",
	"NOUN_HALLU_EIDEET",			/* Walter Moers */
	"NOUN_HALLU_SCHRUMPFHOERNIGER_SCHNARCHKACKLER", /* Harry Potter */
	"NOUN_HALLU_TRUHE",			/* Scheibenwelt */
	"NOUN_HALLU_YOUMA",			/* Sailor Moon */
	"NOUN_HALLU_STEINLAUS",			/* Loriot */
	"NOUN_HALLU_GRUEFFELO", "NOUN_HALLU_KLEINE_RAUPE_NIMMERSATT",
	"NOUN_HALLU_SAMS",			/* Kinderbuch */
	"NOUN_HALLU_BLUMENTOPFERD",		/* Blumentopf-Erde */
	"NOUN_HALLU_KOPFLOSER_REITER",
	"NOUN_HALLU_CHIMAERE",
	"NOUN_HALLU_SANDMANN",
	"NOUN_HALLU_LESERATTE",
	"NOUN_HALLU_RIESENWELLE",
	"NOUN_HALLU_ZAHNFEE",
	"NOUN_HALLU_DOPPELHELIX",
	"NOUN_HALLU_EIERLEGENDE_WOLLMILCHSAU",
	"NOUN_HALLU_FAULTIER",
	"NOUN_HALLU_PANTOFFELTIERCHEN",
	"NOUN_HALLU_SCHNABELTIER",
	"NOUN_HALLU_UFO",
	"NOUN_HALLU_KARTOFFELPUE_REH",
	"NOUN_HALLU_MONDKALB",
	"NOUN_HALLU_TAMAGOCHI",
	"NOUN_HALLU_MAINZELMAENNCHEN",
	"NOUN_HALLU_RHINOGRADENTIUM",
	"NOUN_HALLU_UNGEHEUER_VON_LOCH_NESS",
	"NOUN_HALLU_UNSICHTBARES_ROSA_EINHORN",	/* http://de.wikipedia.org/wiki/Unsichtbares_rosafarbenes_Einhorn */
};
#endif

/* Return a random monster name, for hallucination.
 * KNOWN BUG: May be a proper name (Godzilla, Barney), may not
 * (the Terminator, a Dalek).  There's no elegant way to deal
 * with this without radically modifying the calling functions.
 */
const char *
rndmonnam()
{
	int name;

	do {
	    name = rn1(SPECIAL_PM + SIZE(bogusmons) - LOW_PM, LOW_PM);
	} while (name < SPECIAL_PM &&
	    (type_is_pname(&mons[name]) || (mons[name].geno & G_NOGEN)));

	if (name >= SPECIAL_PM) return bogusmons[name - SPECIAL_PM];
	return mons[name].mname;
}

#ifdef REINCARNATION
const char *
roguename() /* Name of a Rogue player */
{
	char *i, *opts;

	if ((opts = nh_getenv("ROGUEOPTS")) != 0) {
		for (i = opts; *i; i++)
			if (!strncmp("name=",i,5)) {
				char *j;
				if ((j = index(i+5,',')) != 0)
					*j = (char)0;
				return i+5;
			}
	}
	return rn2(3) ? (rn2(2) ? "Michael Toy" : "Kenneth Arnold")
		: "Glenn Wichman";
}
#endif /* REINCARNATION */
#endif /* OVLB */

#ifdef OVL2

static NEARDATA const char * const hcolors[] = {
#ifdef GERMAN
	"kohlpechraben-ADJEKTIV_FARBE_BLACK", "schloh-ADJEKTIV_FARBE_WHITE", "ADJEKTIV_FARBE_UNBUNT",
	"oxydschmelzpunkt-ADJEKTIV_FARBE_FARBIG", /* Isogramm */
#endif
	"ADJEKTIV_FARBE_ULTRAVIOLETT", "ADJEKTIV_FARBE_INFRAROT", "bläulich-ADJEKTIV_FARBE_ORANGE", /* EN "ultraviolet", "infrared", "bluish-orange", */
	"rot-ADJEKTIV_FARBE_GREEN", "dunkel-ADJEKTIV_FARBE_WHITE", "hell-ADJEKTIV_FARBE_BLACK", "himmelblau--ADJEKTIV_FARBE_PINK", /* EN "reddish-green", "dark white", "light black", "sky blue-pink", */
	"ADJEKTIV_FARBE_SALZIG", "ADJEKTIV_FARBE_SUESS", "ADJEKTIV_FARBE_SAUER", "ADJEKTIV_FARBE_BITTER", "umami" /* EN "salty", "sweet", "sour", "bitter", */
	"ADJEKTIV_FARBE_GESTREIFT", "ADJEKTIV_FARBE_GEWUNDEN", "ADJEKTIV_FARBE_VERDREHT", "ADJEKTIV_FARBE_GEBLUEMT", "ADJEKTIV_FARBE_KARIERT", /* EN "striped", "spiral", "swirly", "plaid", "checkered", "argyle", */
	"ADJEKTIV_FARBE_GEFLECKT", "ADJEKTIV_FARBE_GEPUNKTET", /* EN "paisley", "blotchy", "guernsey-spotted", "polka-dotted", */
	"ADJEKTIV_FARBE_QUADRATISCH", "ADJEKTIV_FARBE_RUND", "ADJEKTIV_FARBE_DREIECKIG", /* EN "square", "round", "triangular", */
	"preußisch-ADJEKTIV_FARBE_BLUE", "zinnober-ADJEKTIV_FARBE_RED", "indisch-ADJEKTIV_FARBE_YELLOW", "paynes-ADJEKTIV_FARBE_BLUE", /* EN "cabernet", "sangria", "fuchsia", "wisteria", */
	"zitronen-ADJEKTIV_FARBE_ZITRONIG", "erdbeer--ADJEKTIV_FARBE_BANANIG", "ADJEKTIV_FARBE_PFEFFERMINZIG", /* EN "lemon-lime", "strawberry-banana", "peppermint", */
	"ADJEKTIV_FARBE_ROMANTISCH", "weiß-ADJEKTIV_GLUEHEND" /* EN "romantic", "incandescent" */
};

const char *
hcolor(colorpref)
const char *colorpref;
{
	return (Hallucination || !colorpref) ?
		hcolors[rn2(SIZE(hcolors))] : colorpref;
}

/* return a random real color unless hallucinating */
const char *
rndcolor()
{
	int k = rn2(CLR_MAX);
	return Hallucination ? hcolor((char *)0) : (k == NO_COLOR) ?
		"ADJEKTIV_FARBLOS" : c_obj_colors[k]; /* EN "colorless" : c_obj_colors[k]; */
}

/* Aliases for road-runner nemesis
 */
static const char * const coynames[] = {
// TODO DE
	"Carnivorous Vulgaris","Road-Runnerus Digestus",
	"Eatibus Anythingus"  ,"Famishus-Famishus",
	"Eatibus Almost Anythingus","Eatius Birdius",
	"Famishius Fantasticus","Eternalii Famishiis",
	"Famishus Vulgarus","Famishius Vulgaris Ingeniusi",
	"Eatius-Slobbius","Hardheadipus Oedipus",
	"Carnivorous Slobbius","Hard-Headipus Ravenus",
	"Evereadii Eatibus","Apetitius Giganticus",
	"Hungrii Flea-Bagius","Overconfidentii Vulgaris",
	"Caninus Nervous Rex","Grotesques Appetitus",
	"Nemesis Riduclii","Canis latrans"
};
	
char *
coyotename(mtmp, buf)
struct monst *mtmp;
char *buf;
{
    if (mtmp && buf) {
	Sprintf(buf, "%s - %s",
	    x_monnam(mtmp, ARTICLE_NONE, (char *)0, 0, TRUE),
	    mtmp->mcan ? coynames[SIZE(coynames)-1] : coynames[rn2(SIZE(coynames)-1)]);
    }
    return buf;
}
#endif /* OVL2 */

#ifdef GERMAN
char *
genitivattribut(mtmp)		/* return a name converted to possessive */
struct monst *mtmp;
{
    static char buf[BUFSZ];
    static char tmp_name[BUFSZ];

		if (type_is_pname(mtmp->data)) {
      /* Eigename */
      //Strcpy(buf, "KASUS_GENITIV ARTIKEL_BESTIMMTER ");
      Strcpy(buf, "KASUS_DATIV ");
	    Strcat(buf, "von ");
		} else if ((mtmp->isshk && !Hallucination)) {
      /* Ladenbesitzer */
      Strcpy(buf, "KASUS_DATIV ");
	    //Strcat(buf, mtmp->female ? "der " : "des ");
	    Strcat(buf, "von ");
		} else {
      Strcpy(buf, "KASUS_GENITIV ");
	  }

		Strcpy(tmp_name, mon_nam(mtmp));
		if (!strcmp(tmp_name, "NOUN_IT")) {
	    Strcpy(buf, "von irgendwas");
		} else {
	  	Strcat(buf, tmp_name);
		}
    return buf;
}
#endif

/*do_name.c*/
