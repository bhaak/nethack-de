/*	SCCS Id: @(#)trap.c	3.4	2003/05/25	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

extern const char * const destroy_strings[];	/* from zap.c */

STATIC_DCL void FDECL(dofiretrap, (struct obj *));
STATIC_DCL void NDECL(domagictrap);
STATIC_DCL boolean FDECL(emergency_disrobe,(boolean *));
STATIC_DCL int FDECL(untrap_prob, (struct trap *ttmp));
STATIC_DCL void FDECL(cnv_trap_obj, (int, int, struct trap *));
STATIC_DCL void FDECL(move_into_trap, (struct trap *));
STATIC_DCL int FDECL(try_disarm, (struct trap *,BOOLEAN_P));
STATIC_DCL void FDECL(reward_untrap, (struct trap *, struct monst *));
STATIC_DCL int FDECL(disarm_holdingtrap, (struct trap *));
STATIC_DCL int FDECL(disarm_landmine, (struct trap *));
STATIC_DCL int FDECL(disarm_squeaky_board, (struct trap *));
STATIC_DCL int FDECL(disarm_shooting_trap, (struct trap *, int));
STATIC_DCL int FDECL(try_lift, (struct monst *, struct trap *, int, BOOLEAN_P));
STATIC_DCL int FDECL(help_monster_out, (struct monst *, struct trap *));
STATIC_DCL boolean FDECL(thitm, (int,struct monst *,struct obj *,int,BOOLEAN_P));
STATIC_DCL int FDECL(mkroll_launch,
			(struct trap *,XCHAR_P,XCHAR_P,SHORT_P,long));
STATIC_DCL boolean FDECL(isclearpath,(coord *, int, SCHAR_P, SCHAR_P));
#ifdef STEED
STATIC_OVL int FDECL(steedintrap, (struct trap *, struct obj *));
STATIC_OVL boolean FDECL(keep_saddle_with_steedcorpse,
			(unsigned, struct obj *, struct obj *));
#endif

#ifndef OVLB
STATIC_VAR const char *a_your[2];
STATIC_VAR const char *A_Your[2];
STATIC_VAR const char *the_your[2];
STATIC_VAR const char tower_of_flame[];
STATIC_VAR const char *A_gush_of_water_hits;
STATIC_VAR const char * const blindgas[6];

#else

STATIC_VAR const char * const a_your[2] = { "ARTIKEL_UNBESTIMMTER", "PRONOMEN_POSSESSIV" }; /* EN STATIC_VAR const char * const a_your[2] = { "a", "your" }; */
STATIC_VAR const char * const A_Your[2] = { "SUBJECT ARTIKEL_UNBESTIMMTER", "SUBJECT PRONOMEN_POSSESSIV" }; /* EN STATIC_VAR const char * const A_Your[2] = { "A", "Your" }; */
STATIC_VAR const char * const the_your[2] = { "ARTIKEL_BESTIMMTER", "PRONOMEN_POSSESSIV" }; /* EN STATIC_VAR const char * const the_your[2] = { "the", "your" }; */
STATIC_VAR const char tower_of_flame[] = "tower of flame"; /* EN STATIC_VAR const char tower_of_flame[] = "tower of flame"; */ // TODO DE
STATIC_VAR const char * const A_gush_of_water_hits = "SUBJECT ARTIKEL_UNBESTIMMTER NOUN_SCHWALL Wasser VERB_HIT"; /* EN STATIC_VAR const char * const A_gush_of_water_hits = "A gush of water hits"; */
STATIC_VAR const char * const blindgas[6] = 
	{"humid", "odorless", "pungent", "chilling", "acrid", "biting"}; /* EN {"humid", "odorless", "pungent", "chilling", "acrid", "biting"}; */ // TODO DE

#endif /* OVLB */

#ifdef OVLB

/* called when you're hit by fire (dofiretrap,buzz,zapyourself,explode) */
boolean			/* returns TRUE if hit on torso */
burnarmor(victim)
struct monst *victim;
{
    struct obj *item;
    char buf[BUFSZ];
    int mat_idx;
    
    if (!victim) return 0;
#define burn_dmg(obj,descr) rust_dmg(obj, descr, 0, FALSE, victim)
    while (1) {
	switch (rn2(5)) {
	case 0:
	    item = (victim == &youmonst) ? uarmh : which_armor(victim, W_ARMH);
	    if (item) {
		mat_idx = objects[item->otyp].oc_material;
	    	Sprintf(buf,"%s NOUN_HELMET", materialnm[mat_idx] ); /* EN Sprintf(buf,"%s helmet", materialnm[mat_idx] ); */
	    }
	    if (!burn_dmg(item, item ? buf : "NOUN_HELMET")) continue; /* EN if (!burn_dmg(item, item ? buf : "helmet")) continue; */
	    break;
	case 1:
	    item = (victim == &youmonst) ? uarmc : which_armor(victim, W_ARMC);
	    if (item) {
		(void) burn_dmg(item, cloak_simple_name(item));
		return TRUE;
	    }
	    item = (victim == &youmonst) ? uarm : which_armor(victim, W_ARM);
	    if (item) {
		(void) burn_dmg(item, xname(item));
		return TRUE;
	    }
#ifdef TOURIST
	    item = (victim == &youmonst) ? uarmu : which_armor(victim, W_ARMU);
	    if (item)
		(void) burn_dmg(item, "NOUN_SHIRT"); /* EN (void) burn_dmg(item, "shirt"); */
#endif
	    return TRUE;
	case 2:
	    item = (victim == &youmonst) ? uarms : which_armor(victim, W_ARMS);
	    if (!burn_dmg(item, "wooden shield")) continue; /* EN if (!burn_dmg(item, "wooden shield")) continue; */ // TODO DE
	    break;
	case 3:
	    item = (victim == &youmonst) ? uarmg : which_armor(victim, W_ARMG);
	    if (!burn_dmg(item, "NOUN_GLOVESs")) continue; /* EN if (!burn_dmg(item, "gloves")) continue; */
	    break;
	case 4:
	    item = (victim == &youmonst) ? uarmf : which_armor(victim, W_ARMF);
	    if (!burn_dmg(item, "NOUN_BOOTSs")) continue; /* EN if (!burn_dmg(item, "boots")) continue; */
	    break;
	}
	break; /* Out of while loop */
    }
    return FALSE;
#undef burn_dmg
}

/* Generic rust-armor function.  Returns TRUE if a message was printed;
 * "print", if set, means to print a message (and thus to return TRUE) even
 * if the item could not be rusted; otherwise a message is printed and TRUE is
 * returned only for rustable items.
 */
boolean
rust_dmg(otmp, ostr, type, print, victim)
register struct obj *otmp;
register const char *ostr;
int type;
boolean print;
struct monst *victim;
{
	static NEARDATA const char * const action[] = { "smoulder", "VERB_ROSTEN", "rot", "corrode" }; /* EN static NEARDATA const char * const action[] = { "smoulder", "rust", "rot", "corrode" }; */ // TODO DE
#ifdef GERMAN
	static NEARDATA const char * const action_completely[] = { "smoulder", "VERB_VERROSTEN", "rot", "corrode" }; // TODO DE
#endif
	static NEARDATA const char * const msg[] =  { "burnt", "verrostet", "rotten", "corroded" }; /* EN static NEARDATA const char * const msg[] =  { "burnt", "rusted", "rotten", "corroded" }; */ // TODO DE
	boolean vulnerable = FALSE;
	boolean grprot = FALSE;
	boolean is_primary = TRUE;
	boolean vismon = (victim != &youmonst) && canseemon(victim);
	int erosion;

	if (!otmp) return(FALSE);
	switch(type) {
		case 0: vulnerable = is_flammable(otmp);
			break;
		case 1: vulnerable = is_rustprone(otmp);
			grprot = TRUE;
			break;
		case 2: vulnerable = is_rottable(otmp);
			is_primary = FALSE;
			break;
		case 3: vulnerable = is_corrodeable(otmp);
			grprot = TRUE;
			is_primary = FALSE;
			break;
	}
	erosion = is_primary ? otmp->oeroded : otmp->oeroded2;

	if (!print && (!vulnerable || otmp->oerodeproof || erosion == MAX_ERODE))
		return FALSE;

	if (!vulnerable) {
	    if (flags.verbose) {
		if (victim == &youmonst)
		    Your("%s %s not affected.", ostr, vtense(ostr, "VERB_SEIN")); /* EN Your("%s %s not affected.", ostr, vtense(ostr, "are")); */ // TODO DE
		else if (vismon)
			pline("SUBJECT ARTIKEL_BESTIMMTER %s KASUS_GENITIV %s %s not affected. Rost 2", ostr, mon_nam(victim), /* EN pline("%s's %s %s not affected.", Monnam(victim), ostr, */ // TODO DE
			  vtense(ostr, "VERB_SEIN")); /* EN vtense(ostr, "are")); */
	    }
	} else if (erosion < MAX_ERODE) {
	    if (grprot && otmp->greased) {
		grease_protect(otmp,ostr,victim);
	    } else if (otmp->oerodeproof || (otmp->blessed && !rnl(4))) {
		if (flags.verbose) {
		    if (victim == &youmonst)
			pline("Somehow, your %s %s not affected. Rost 3", /* EN pline("Somehow, your %s %s not affected.", */ // TODO DE
			      ostr, vtense(ostr, "VERB_SEIN")); /* EN ostr, vtense(ostr, "are")); */
		    else if (vismon)
			pline("Somehow, %s's %s %s not affected. Rost 4", /* EN pline("Somehow, %s's %s %s not affected.", */ // TODO DE
			      mon_nam(victim), ostr, vtense(ostr, "VERB_SEIN")); /* EN mon_nam(victim), ostr, vtense(ostr, "are")); */
		}
	    } else {
		if (victim == &youmonst)
			Your("%s %s%s!", ostr, /* EN Your("%s %s%s!", ostr, */
			 erosion+1 == MAX_ERODE ? vtense(ostr, action_completely[type]) : vtense(ostr, action[type]), /* EN vtense(ostr, action[type]), */
			 erosion+1 == MAX_ERODE ? " völlig" : /* EN erosion+1 == MAX_ERODE ? " completely" : */
			    erosion ? " weiter" : ""); /* EN erosion ? " further" : ""); */
		else if (vismon)
			pline("SUBJECT ARTIKEL_BESTIMMTER %s KASUS_GENITIV %s %s%s!", ostr, mon_nam(victim), /* EN pline("%s %s %s%s!", Monnam(victim), ostr, */
			 erosion+1 == MAX_ERODE ? vtense(ostr, action_completely[type]) : vtense(ostr, action[type]), /* EN vtense(ostr, action[type]), */
			erosion+1 == MAX_ERODE ? " völlig" : /* EN erosion+1 == MAX_ERODE ? " completely" : */
			  erosion ? " weiter" : ""); /* EN erosion ? " further" : ""); */
		if (is_primary)
		    otmp->oeroded++;
		else
		    otmp->oeroded2++;
		update_inventory();
	    }
	} else {
	    if (flags.verbose) {
		if (victim == &youmonst)
		    Your("%s %s völlig %s SATZKLAMMER.", ostr, /* EN Your("%s %s completely %s.", ostr, */
			 vtense(ostr, Blind ? "VERB_ANFUEHLEN sich" : "VERB_AUSSEHEN"), /* EN vtense(ostr, Blind ? "feel" : "look"), */
			 msg[type]);
		else if (vismon)
			pline("SUBJECT ARTIKEL_BESTIMMTER %s KASUS_GENITIV %s %s völlig %s SATZKLAMMER.", /* EN pline("%s's %s %s completely %s.", */
						Monnam(victim), ostr,
						vtense(ostr, "VERB_AUSSEHEN"), msg[type]); /* EN vtense(ostr, "look"), msg[type]); */
	    }
	}
	return(TRUE);
}

void
grease_protect(otmp,ostr,victim)
register struct obj *otmp;
register const char *ostr;
struct monst *victim;
{
	static const char txt[] = "durch die Fettschicht geschützt!"; /* EN static const char txt[] = "protected by the layer of grease!"; */
	boolean vismon = victim && (victim != &youmonst) && canseemon(victim);

	if (ostr) {
	    if (victim == &youmonst)
		Your("%s %s %s", ostr, vtense(ostr, "VERB_SEIN"), txt); /* EN Your("%s %s %s", ostr, vtense(ostr, "are"), txt); */
	    else if (vismon)
		pline("SUBJECT %s KASUS_GENITIV %s %s %s", ostr, /* EN pline("%s's %s %s %s", Monnam(victim), */
		    mon_nam(victim), vtense(ostr, "VERB_SEIN"), txt); /* EN ostr, vtense(ostr, "are"), txt); */
	} else {
	    if (victim == &youmonst)
		Your("%s %s",aobjnam(otmp,"VERB_SEIN"), txt); /* EN Your("%s %s",aobjnam(otmp,"are"), txt); */
	    else if (vismon)
		pline("SUBJECT %s KASUS_GENITIV %s %s %s", aobjnam(otmp,(char *)0), Monnam(victim), "VERB_SEIN", txt); /* EN pline("%s's %s %s", Monnam(victim), aobjnam(otmp,"are"), txt); */
	}
	if (!rn2(2)) {
	    otmp->greased = 0;
	    if (carried(otmp)) {
		pline("Das Fett ist verbraucht."); /* EN pline_The("grease dissolves."); */
		update_inventory();
	    }
	}
}

struct trap *
maketrap(x,y,typ)
register int x, y, typ;
{
	register struct trap *ttmp;
	register struct rm *lev;
	register boolean oldplace;

	if ((ttmp = t_at(x,y)) != 0) {
	    if (ttmp->ttyp == MAGIC_PORTAL) return (struct trap *)0;
	    oldplace = TRUE;
	    if (u.utrap && (x == u.ux) && (y == u.uy) &&
	      ((u.utraptype == TT_BEARTRAP && typ != BEAR_TRAP) ||
	      (u.utraptype == TT_WEB && typ != WEB) ||
	      (u.utraptype == TT_PIT && typ != PIT && typ != SPIKED_PIT)))
		    u.utrap = 0;
	} else {
	    oldplace = FALSE;
	    ttmp = newtrap();
	    ttmp->tx = x;
	    ttmp->ty = y;
	    ttmp->launch.x = -1;	/* force error if used before set */
	    ttmp->launch.y = -1;
	}
	ttmp->ttyp = typ;
	switch(typ) {
	    case STATUE_TRAP:	    /* create a "living" statue */
	      { struct monst *mtmp;
		struct obj *otmp, *statue;

		statue = mkcorpstat(STATUE, (struct monst *)0,
					&mons[rndmonnum()], x, y, FALSE);
		mtmp = makemon(&mons[statue->corpsenm], 0, 0, NO_MM_FLAGS);
		if (!mtmp) break; /* should never happen */
		while(mtmp->minvent) {
		    otmp = mtmp->minvent;
		    otmp->owornmask = 0;
		    obj_extract_self(otmp);
		    (void) add_to_container(statue, otmp);
		}
		statue->owt = weight(statue);
		mongone(mtmp);
		break;
	      }
	    case ROLLING_BOULDER_TRAP:	/* boulder will roll towards trigger */
		(void) mkroll_launch(ttmp, x, y, BOULDER, 1L);
		break;
	    case HOLE:
	    case PIT:
	    case SPIKED_PIT:
	    case TRAPDOOR:
		lev = &levl[x][y];
		if (*in_rooms(x, y, SHOPBASE) &&
			((typ == HOLE || typ == TRAPDOOR) ||
			 IS_DOOR(lev->typ) || IS_WALL(lev->typ)))
		    add_damage(x, y,		/* schedule repair */
			       ((IS_DOOR(lev->typ) || IS_WALL(lev->typ))
				&& !flags.mon_moving) ? 200L : 0L);
		lev->doormask = 0;	/* subsumes altarmask, icedpool... */
		if (IS_ROOM(lev->typ)) /* && !IS_AIR(lev->typ) */
		    lev->typ = ROOM;

		/*
		 * some cases which can happen when digging
		 * down while phazing thru solid areas
		 */
		else if (lev->typ == STONE || lev->typ == SCORR)
		    lev->typ = CORR;
		else if (IS_WALL(lev->typ) || lev->typ == SDOOR)
		    lev->typ = level.flags.is_maze_lev ? ROOM :
			       level.flags.is_cavernous_lev ? CORR : DOOR;

		unearth_objs(x, y);
		break;
	}
	if (ttmp->ttyp == HOLE) ttmp->tseen = 1;  /* You can't hide a hole */
	else ttmp->tseen = 0;
	ttmp->once = 0;
	ttmp->madeby_u = 0;
	ttmp->dst.dnum = -1;
	ttmp->dst.dlevel = -1;
	if (!oldplace) {
	    ttmp->ntrap = ftrap;
	    ftrap = ttmp;
	}
	return(ttmp);
}

void
fall_through(td)
boolean td;	/* td == TRUE : trap door or hole */
{
	d_level dtmp;
	char msgbuf[BUFSZ];
	const char *dont_fall = 0;
	register int newlevel = dunlev(&u.uz);

	/* KMH -- You can't escape the Sokoban level traps */
	if(Blind && Levitation && !In_sokoban(&u.uz)) return;

	do {
	    newlevel++;
	} while(!rn2(4) && newlevel < dunlevs_in_dungeon(&u.uz));

	if(td) {
	    struct trap *t=t_at(u.ux,u.uy);
	    seetrap(t);
	    if (!In_sokoban(&u.uz)) {
		if (t->ttyp == TRAPDOOR)
			pline("SUBJECT ARTIKEL_UNBESTIMMTER NOUN_TRAP_DOOR VERB_OEFFNEN sich unter KASUS_DATIV PRONOMEN_PERSONAL!"); /* EN pline("A trap door opens up under you!"); */
		else 
			pline("There's a gaping hole under you!"); /* EN pline("There's a gaping hole under you!"); */ // TODO DE
	    }
	} else pline_The("%s VERB_OEFFNEN sich unter KASUS_DATIV PRONOMEN_PERSONAL!", surface(u.ux,u.uy)); /* EN } else pline_The("%s opens up under you!", surface(u.ux,u.uy)); */

	if (In_sokoban(&u.uz) && Can_fall_thru(&u.uz))
	    ;	/* KMH -- You can't escape the Sokoban level traps */
	else if(Levitation || u.ustuck || !Can_fall_thru(&u.uz)
	   || Flying || is_clinger(youmonst.data)
	   || (Inhell && !u.uevent.invoked &&
					newlevel == dunlevs_in_dungeon(&u.uz))
		) {
	    dont_fall = "VERB_FALLEN nicht rein."; /* EN dont_fall = "don't fall in."; */
	} else if (youmonst.data->msize >= MZ_HUGE) {
	    dont_fall = "VERB_PASSEN nicht durch."; /* EN dont_fall = "don't fit through."; */
	} else if (!next_to_u()) {
	    dont_fall = "are jerked back by your pet!"; /* EN dont_fall = "are jerked back by your pet!"; */ // TODO DE
	}
	if (dont_fall) {
	    You(dont_fall);
	    /* hero didn't fall through, but any objects here might */
	    impact_drop((struct obj *)0, u.ux, u.uy, 0);
	    if (!td) {
		display_nhwindow(WIN_MESSAGE, FALSE);
		pline_The("opening under you closes up."); /* EN pline_The("opening under you closes up."); */ // TODO DE
	    }
	    return;
	}

	if(*u.ushops) shopdig(1);
	if (Is_stronghold(&u.uz)) {
	    find_hell(&dtmp);
	} else {
	    dtmp.dnum = u.uz.dnum;
	    dtmp.dlevel = newlevel;
	}
	if (!td)
	    Sprintf(msgbuf, "SUBJECT ARTIKEL_BESTIMMTER NOUN_HOLE OBJECT KASUS_DATIV _in_ ARTIKEL_BESTIMMTER %s über OBJECT KASUS_DATIV PRONOMEN_PERSONAL VERB_SCHLIESSEN sich.", /* EN Sprintf(msgbuf, "The hole in the %s above you closes up.", */ // TODO DE
		    ceiling(u.ux,u.uy));
	schedule_goto(&dtmp, FALSE, TRUE, 0,
		      (char *)0, !td ? msgbuf : (char *)0);
}

/*
 * Animate the given statue.  May have been via shatter attempt, trap,
 * or stone to flesh spell.  Return a monster if successfully animated.
 * If the monster is animated, the object is deleted.  If fail_reason
 * is non-null, then fill in the reason for failure (or success).
 *
 * The cause of animation is:
 *
 *	ANIMATE_NORMAL  - hero "finds" the monster
 *	ANIMATE_SHATTER - hero tries to destroy the statue
 *	ANIMATE_SPELL   - stone to flesh spell hits the statue
 *
 * Perhaps x, y is not needed if we can use get_obj_location() to find
 * the statue's location... ???
 */
struct monst *
animate_statue(statue, x, y, cause, fail_reason)
struct obj *statue;
xchar x, y;
int cause;
int *fail_reason;
{
	struct permonst *mptr;
	struct monst *mon = 0;
	struct obj *item;
	coord cc;
	boolean historic = (Role_if(PM_ARCHEOLOGIST) && !flags.mon_moving && (statue->spe & STATUE_HISTORIC));
	char statuename[BUFSZ];

	Strcpy(statuename,the(xname(statue)));

	if (statue->oxlth && statue->oattached == OATTACHED_MONST) {
	    cc.x = x,  cc.y = y;
	    mon = montraits(statue, &cc);
	    if (mon && mon->mtame && !mon->isminion)
		wary_dog(mon, TRUE);
	} else {
	    /* statue of any golem hit with stone-to-flesh becomes flesh golem */
	    if (is_golem(&mons[statue->corpsenm]) && cause == ANIMATE_SPELL)
	    	mptr = &mons[PM_FLESH_GOLEM];
	    else
		mptr = &mons[statue->corpsenm];
	    /*
	     * Guard against someone wishing for a statue of a unique monster
	     * (which is allowed in normal play) and then tossing it onto the
	     * [detected or guessed] location of a statue trap.  Normally the
	     * uppermost statue is the one which would be activated.
	     */
	    if ((mptr->geno & G_UNIQ) && cause != ANIMATE_SPELL) {
	        if (fail_reason) *fail_reason = AS_MON_IS_UNIQUE;
	        return (struct monst *)0;
	    }
	    if (cause == ANIMATE_SPELL &&
		((mptr->geno & G_UNIQ) || mptr->msound == MS_GUARDIAN)) {
		/* Statues of quest guardians or unique monsters
		 * will not stone-to-flesh as the real thing.
		 */
		mon = makemon(&mons[PM_DOPPELGANGER], x, y,
			NO_MINVENT|MM_NOCOUNTBIRTH|MM_ADJACENTOK);
		if (mon) {
			/* makemon() will set mon->cham to
			 * CHAM_ORDINARY if hero is wearing
			 * ring of protection from shape changers
			 * when makemon() is called, so we have to
			 * check the field before calling newcham().
			 */
			if (mon->cham == CHAM_DOPPELGANGER)
				(void) newcham(mon, mptr, FALSE, FALSE);
		}
	    } else
		mon = makemon(mptr, x, y, (cause == ANIMATE_SPELL) ?
			(NO_MINVENT | MM_ADJACENTOK) : NO_MINVENT);
	}

	if (!mon) {
	    if (fail_reason) *fail_reason = AS_NO_MON;
	    return (struct monst *)0;
	}

	/* allow statues to be of a specific gender */
	if (statue->spe & STATUE_MALE)
	    mon->female = FALSE;
	else if (statue->spe & STATUE_FEMALE)
	    mon->female = TRUE;
	/* if statue has been named, give same name to the monster */
	if (statue->onamelth)
	    mon = christen_monst(mon, ONAME(statue));
	/* transfer any statue contents to monster's inventory */
	while ((item = statue->cobj) != 0) {
	    obj_extract_self(item);
	    (void) add_to_minv(mon, item);
	}
	m_dowear(mon, TRUE);
	delobj(statue);
	/* mimic statue becomes seen mimic; other hiders won't be hidden */
	if (mon->m_ap_type) seemimic(mon);
	else mon->mundetected = FALSE;
	if ((x == u.ux && y == u.uy) || cause == ANIMATE_SPELL) {
	    const char *comes_to_life = nonliving(mon->data) ?
					"moves" : "comes to life"; /* EN "moves" : "comes to life";  */ // TODO DE
	    if (cause == ANIMATE_SPELL)
	    	pline("%s %s!", upstart(statuename),
	    		canspotmon(mon) ? comes_to_life : "disappears"); /* EN canspotmon(mon) ? comes_to_life : "disappears"); */ // TODO DE
	    else
		pline_The("statue %s!", /* EN pline_The("statue %s!", */ // TODO DE
			canspotmon(mon) ? comes_to_life : "disappears"); /* EN canspotmon(mon) ? comes_to_life : "disappears"); */ // TODO DE
	    if (historic) {
		    You_feel("guilty that the historic statue is now gone."); /* EN You_feel("guilty that the historic statue is now gone."); */ // TODO DE
		    adjalign(-1);
	    }
	} else if (cause == ANIMATE_SHATTER)
	    pline("Instead of shattering, the statue suddenly %s!", /* EN pline("Instead of shattering, the statue suddenly %s!", */ // TODO DE
		canspotmon(mon) ? "comes to life" : "disappears"); /* EN canspotmon(mon) ? "comes to life" : "disappears"); */ // TODO DE
	else { /* cause == ANIMATE_NORMAL */
	    You("find %s posing as a statue.", /* EN You("find %s posing as a statue.", */ // TODO DE
		canspotmon(mon) ? a_monnam(mon) : something);
	    stop_occupation();
	}
	/* avoid hiding under nothing */
	if (x == u.ux && y == u.uy &&
		Upolyd && hides_under(youmonst.data) && !OBJ_AT(x, y))
	    u.uundetected = 0;

	if (fail_reason) *fail_reason = AS_OK;
	return mon;
}

/*
 * You've either stepped onto a statue trap's location or you've triggered a
 * statue trap by searching next to it or by trying to break it with a wand
 * or pick-axe.
 */
struct monst *
activate_statue_trap(trap, x, y, shatter)
struct trap *trap;
xchar x, y;
boolean shatter;
{
	struct monst *mtmp = (struct monst *)0;
	struct obj *otmp = sobj_at(STATUE, x, y);
	int fail_reason;

	/*
	 * Try to animate the first valid statue.  Stop the loop when we
	 * actually create something or the failure cause is not because
	 * the mon was unique.
	 */
	deltrap(trap);
	while (otmp) {
	    mtmp = animate_statue(otmp, x, y,
		    shatter ? ANIMATE_SHATTER : ANIMATE_NORMAL, &fail_reason);
	    if (mtmp || fail_reason != AS_MON_IS_UNIQUE) break;

	    while ((otmp = otmp->nexthere) != 0)
		if (otmp->otyp == STATUE) break;
	}

	if (Blind) feel_location(x, y);
	else newsym(x, y);
	return mtmp;
}

#ifdef STEED
STATIC_OVL boolean
keep_saddle_with_steedcorpse(steed_mid, objchn, saddle)
unsigned steed_mid;
struct obj *objchn, *saddle;
{
	if (!saddle) return FALSE;
	while(objchn) {
		if(objchn->otyp == CORPSE &&
		   objchn->oattached == OATTACHED_MONST && objchn->oxlth) {
			struct monst *mtmp = (struct monst *)objchn->oextra;
			if (mtmp->m_id == steed_mid) {
				/* move saddle */
				xchar x,y;
				if (get_obj_location(objchn, &x, &y, 0)) {
					obj_extract_self(saddle);
					place_object(saddle, x, y);
					stackobj(saddle);
				}
				return TRUE;
			}
		}
		if (Has_contents(objchn) &&
		    keep_saddle_with_steedcorpse(steed_mid, objchn->cobj, saddle))
			return TRUE;
		objchn = objchn->nobj;
	}
	return FALSE;
}
#endif /*STEED*/

void
dotrap(trap, trflags)
register struct trap *trap;
unsigned trflags;
{
	register int ttype = trap->ttyp;
	register struct obj *otmp;
	boolean already_seen = trap->tseen;
	boolean webmsgok = (!(trflags & NOWEBMSG));
	boolean forcebungle = (trflags & FORCEBUNGLE);

	nomul(0);

	/* KMH -- You can't escape the Sokoban level traps */
	if (In_sokoban(&u.uz) &&
			(ttype == PIT || ttype == SPIKED_PIT || ttype == HOLE ||
			ttype == TRAPDOOR)) {
	    /* The "air currents" message is still appropriate -- even when
	     * the hero isn't flying or levitating -- because it conveys the
	     * reason why the player cannot escape the trap with a dexterity
	     * check, clinging to the ceiling, etc.
	     */
	    pline("Air currents pull you down into %s %s!", /* EN pline("Air currents pull you down into %s %s!", */ // TODO DE
	    	a_your[trap->madeby_u],
	    	defsyms[trap_to_defsym(ttype)].explanation);
	    /* then proceed to normal trap effect */
	} else if (already_seen) {
	    if ((Levitation || Flying) &&
		    (ttype == PIT || ttype == SPIKED_PIT || ttype == HOLE ||
		    ttype == BEAR_TRAP)) {
		You("%s OBJECT KASUS_DATIV über %s %s.", /* EN You("%s over %s %s.", */
		    Levitation ? "VERB_SCHWEBEN" : "VERB_FLIEGEN", /* EN Levitation ? "float" : "fly", */
		    a_your[trap->madeby_u],
		    defsyms[trap_to_defsym(ttype)].explanation);
		return;
	    }
	    if(!Fumbling && ttype != MAGIC_PORTAL &&
		ttype != ANTI_MAGIC && !forcebungle &&
		(!rn2(5) ||
	    ((ttype == PIT || ttype == SPIKED_PIT) && is_clinger(youmonst.data)))) {
		You("VERB_ENTGEHEN OBJECT KASUS_DATIV %s %s.", /* EN You("escape %s %s.", */
		    (ttype == ARROW_TRAP && !trap->madeby_u) ? "ARTIKEL_UNBESTIMMTER" : /* EN (ttype == ARROW_TRAP && !trap->madeby_u) ? "an" : */
			a_your[trap->madeby_u],
		    defsyms[trap_to_defsym(ttype)].explanation);
		return;
	    }
	}

#ifdef STEED
	if (u.usteed) u.usteed->mtrapseen |= (1 << (ttype-1));
#endif

	switch(ttype) {
	    case ARROW_TRAP:
		if (trap->once && trap->tseen && !rn2(15)) {
		    You_hear("ein lautes Klicken!"); /* EN You_hear("a loud click!"); */
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("An arrow shoots out at you!"); /* EN pline("An arrow shoots out at you!"); */ // TODO DE
		otmp = mksobj(ARROW, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		otmp->opoisoned = 0;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
		if (thitu(8, dmgval(otmp, &youmonst), otmp, "NOUN_ARROW")) { /* EN if (thitu(8, dmgval(otmp, &youmonst), otmp, "arrow")) { */
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
		}
		break;
	    case DART_TRAP:
		if (trap->once && trap->tseen && !rn2(15)) {
		    You_hear("ein leises Klicken."); /* EN You_hear("a soft click."); */
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
		pline("A little dart shoots out at you!"); /* EN pline("A little dart shoots out at you!"); */ // TODO DE
		otmp = mksobj(DART, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		if (!rn2(6)) otmp->opoisoned = 1;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
		if (thitu(7, dmgval(otmp, &youmonst), otmp, "little dart")) { /* EN if (thitu(7, dmgval(otmp, &youmonst), otmp, "little dart")) { */ // TODO DE
		    if (otmp->opoisoned)
			poisoned("dart", A_CON, "little dart", -10); /* EN poisoned("dart", A_CON, "little dart", -10); */ // TODO DE
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
		}
		break;
	    case ROCKTRAP:
		if (trap->once && trap->tseen && !rn2(15)) {
		    pline("A trap door in %s opens, but nothing falls out!", /* EN pline("A trap door in %s opens, but nothing falls out!", */ // TODO DE
			  the(ceiling(u.ux,u.uy)));
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		} else {
		    int dmg = d(2,6); /* should be std ROCK dmg? */

		    trap->once = 1;
		    seetrap(trap);
		    otmp = mksobj_at(ROCK, u.ux, u.uy, TRUE, FALSE);
		    otmp->quan = 1L;
		    otmp->owt = weight(otmp);

		    pline("A trap door in %s opens and %s falls on your %s!", /* EN pline("A trap door in %s opens and %s falls on your %s!", */ // TODO DE
			  the(ceiling(u.ux,u.uy)),
			  an(xname(otmp)),
			  body_part(HEAD));

		    if (uarmh) {
			if(is_metallic(uarmh)) {
			    pline("SUBJECT Glücklicherweise VERB_TRAGEN PRONOMEN_PERSONAL einen schützenden Helm."); /* EN pline("Fortunately, you are wearing a hard helmet."); */
			    dmg = 2;
			} else if (flags.verbose) {
			    Your("%s VERB_SCHUETZEN OBJECT PRONOMEN_PERSONAL nicht.", xname(uarmh)); /* EN Your("%s does not protect you.", xname(uarmh)); */
			}
		    }

		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux,u.uy);	/* map the rock */

		    losehp(dmg, "falling rock", KILLED_BY_AN); /* EN losehp(dmg, "falling rock", KILLED_BY_AN); */ // TODO DE
		    exercise(A_STR, FALSE);
		}
		break;

	    case SQKY_BOARD:	    /* stepped on a squeaky board */
		if (Levitation || Flying) {
		    if (!Blind) {
			seetrap(trap);
			if (Hallucination)
				You("notice a crease in the linoleum."); /* EN You("notice a crease in the linoleum."); */ // TODO DE
			else
				You("notice a loose board below you."); /* EN You("notice a loose board below you."); */ // TODO DE
		    }
		} else {
		    seetrap(trap);
		    pline("A board beneath you squeaks loudly."); /* EN pline("A board beneath you squeaks loudly."); */ // TODO DE
		    wake_nearby();
		}
		break;

	    case BEAR_TRAP:
		if(Levitation || Flying) break;
		seetrap(trap);
		if(amorphous(youmonst.data) || is_whirly(youmonst.data) ||
						    unsolid(youmonst.data)) {
		    pline("%s bear trap closes harmlessly through you.", /* EN pline("%s bear trap closes harmlessly through you.", */ // TODO DE
			    A_Your[trap->madeby_u]);
		    break;
		}
		if(
#ifdef STEED
		   !u.usteed &&
#endif
		   youmonst.data->msize <= MZ_SMALL) {
		    pline("%s bear trap closes harmlessly over you.", /* EN pline("%s bear trap closes harmlessly over you.", */ // TODO DE
			    A_Your[trap->madeby_u]);
		    break;
		}
		u.utrap = rn1(4, 4);
		u.utraptype = TT_BEARTRAP;
#ifdef STEED
		if (u.usteed) {
		    pline("SUBJECT %s bear trap closes on %s!", /* EN pline("%s bear trap closes on %s %s!", */ // TODO DE
			A_Your[trap->madeby_u], genitivattribut_zu_wort(mon_nam(u.usteed), /* EN A_Your[trap->madeby_u], s_suffix(mon_nam(u.usteed)), */
			mbodypart(u.usteed, FOOT))); /* EN mbodypart(u.usteed, FOOT)); */
		} else
#endif
		{
		    pline("SUBJECT %s NOUN_BEAR_TRAP VERB_ZUSCHNAPPEN on your %s!", /* EN pline("%s bear trap closes on your %s!", */ // TODO DE
			    A_Your[trap->madeby_u], body_part(FOOT));
		    if(u.umonnum == PM_OWLBEAR || u.umonnum == PM_BUGBEAR)
			You("howl in anger!"); /* EN You("howl in anger!"); */ // TODO DE
		}
		exercise(A_DEX, FALSE);
		break;

	    case SLP_GAS_TRAP:
		seetrap(trap);
		if(Sleep_resistance || breathless(youmonst.data)) {
		    You("are enveloped in a cloud of gas!"); /* EN You("are enveloped in a cloud of gas!"); */ // TODO DE
		    break;
		}
		pline("A cloud of gas puts you to sleep!"); /* EN pline("A cloud of gas puts you to sleep!"); */ // TODO DE
		fall_asleep(-rnd(25), TRUE);
#ifdef STEED
		(void) steedintrap(trap, (struct obj *)0);
#endif
		break;

	    case RUST_TRAP:
		seetrap(trap);
		if (u.umonnum == PM_IRON_GOLEM) {
		    pline("%s OBJECT PRONOMEN_PERSONAL!", A_gush_of_water_hits); /* EN pline("%s you!", A_gush_of_water_hits); */
		    You("are covered with rust!"); /* EN You("are covered with rust!"); */ // TODO DE
		    rehumanize();
		    break;
		} else if (u.umonnum == PM_GREMLIN && rn2(3)) {
		    pline("%s OBJECT PRONOMEN_PERSONAL!", A_gush_of_water_hits); /* EN pline("%s you!", A_gush_of_water_hits); */
		    (void)split_mon(&youmonst, (struct monst *)0);
		    break;
		}

	    /* Unlike monsters, traps cannot aim their rust attacks at
	     * you, so instead of looping through and taking either the
	     * first rustable one or the body, we take whatever we get,
	     * even if it is not rustable.
	     */
		switch (rn2(5)) {
		    case 0:
			pline("%s OBJECT PRONOMEN_PERSONAL _an_ KASUS_DATIV ARTIKEL_BESTIMMTER %s!", A_gush_of_water_hits, /* EN pline("%s you on the %s!", A_gush_of_water_hits, */
				    body_part(HEAD));
			(void) rust_dmg(uarmh, "NOUN_HELMET", 1, TRUE, &youmonst); /* EN (void) rust_dmg(uarmh, "helmet", 1, TRUE, &youmonst); */
			break;
		    case 1:
			pline("%s OBJECT PRONOMEN_POSSESSIV ADJEKTIV_LINK %s!", A_gush_of_water_hits, /* EN pline("%s your left %s!", A_gush_of_water_hits, */
				    body_part(ARM));
			if (rust_dmg(uarms, "NOUN_SHIELD", 1, TRUE, &youmonst)) /* EN if (rust_dmg(uarms, "shield", 1, TRUE, &youmonst)) */
			    break;
			if (u.twoweap || (uwep && bimanual(uwep)))
			    erode_obj(u.twoweap ? uswapwep : uwep, FALSE, TRUE);
glovecheck:		(void) rust_dmg(uarmg, "gauntlets", 1, TRUE, &youmonst); /* EN glovecheck:		(void) rust_dmg(uarmg, "gauntlets", 1, TRUE, &youmonst); */ // TODO DE
			/* Not "metal gauntlets" since it gets called
			 * even if it's leather for the message
			 */
			break;
		    case 2:
			pline("%s OBJECT PRONOMEN_POSSESSIV ADJEKTIV_RECHT %s!", A_gush_of_water_hits, /* EN pline("%s your right %s!", A_gush_of_water_hits, */
				    body_part(ARM));
			erode_obj(uwep, FALSE, TRUE);
			goto glovecheck;
		    default:
			pline("%s OBJECT PRONOMEN_PERSONAL!", A_gush_of_water_hits); /* EN pline("%s you!", A_gush_of_water_hits); */
			for (otmp=invent; otmp; otmp = otmp->nobj)
				    (void) snuff_lit(otmp);
			if (uarmc)
			    (void) rust_dmg(uarmc, cloak_simple_name(uarmc),
						1, TRUE, &youmonst);
			else if (uarm)
			    (void) rust_dmg(uarm, "NOUN_ARMOR", 1, TRUE, &youmonst); /* EN (void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst); */
#ifdef TOURIST
			else if (uarmu)
			    (void) rust_dmg(uarmu, "NOUN_SHIRT", 1, TRUE, &youmonst); /* EN (void) rust_dmg(uarmu, "shirt", 1, TRUE, &youmonst); */
#endif
		}
		update_inventory();
		break;

	    case FIRE_TRAP:
		seetrap(trap);
		dofiretrap((struct obj *)0);
		break;

	    case PIT:
	    case SPIKED_PIT:
		/* KMH -- You can't escape the Sokoban level traps */
		if (!In_sokoban(&u.uz) && (Levitation || Flying)) break;
		seetrap(trap);
		if (!In_sokoban(&u.uz) && is_clinger(youmonst.data)) {
		    if(trap->tseen) {
			You("see %s %s below you.", a_your[trap->madeby_u], /* EN You("see %s %spit below you.", a_your[trap->madeby_u], */ // TODO DE
			    ttype == SPIKED_PIT ? "NOUN_SPIKED_PIT" : "NOUN_PIT"); /* EN ttype == SPIKED_PIT ? "spiked " : ""); */
		    } else {
			pline("%s pit %sopens up under you!", /* EN pline("%s pit %sopens up under you!", */ // TODO DE
			    A_Your[trap->madeby_u],
			    ttype == SPIKED_PIT ? "full of spikes " : ""); /* EN ttype == SPIKED_PIT ? "full of spikes " : ""); */ // TODO DE
			You("VERB_FALLEN nicht rein!"); /* EN You("don't fall in!"); */
		    }
		    break;
		}
		if (!In_sokoban(&u.uz)) {
		    char verbbuf[BUFSZ];
#ifdef STEED
		    if (u.usteed) {
		    	if ((trflags & RECURSIVETRAP) != 0)
			    Sprintf(verbbuf, "und SUBJECT_IM_SATZ %s MODIFIER_VERB_DRITTE_PERSON MODIFIER_VERB_PLURAL VERB_FALLEN", /* EN Sprintf(verbbuf, "and %s fall", */
				x_monnam(u.usteed,
				    u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				    (char *)0, SUPPRESS_SADDLE, FALSE));
			else
			    Sprintf(verbbuf,"VERB_FUEHREN OBJECT %s", /* EN Sprintf(verbbuf,"lead %s", */
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "ADJEKTIV_POOR", SUPPRESS_SADDLE, FALSE)); /* EN "poor", SUPPRESS_SADDLE, FALSE)); */
		    } else
#endif
		    Strcpy(verbbuf,"VERB_FALLEN"); /* EN Strcpy(verbbuf,"fall"); */
		    You("%s NEUES_OBJECT OBJECT in %s NOUN_PIT!", verbbuf, a_your[trap->madeby_u]); /* EN You("%s into %s pit!", verbbuf, a_your[trap->madeby_u]); */
		}
		/* wumpus reference */
		if (Role_if(PM_RANGER) && !trap->madeby_u && !trap->once &&
			In_quest(&u.uz) && Is_qlocate(&u.uz)) {
		    pline("Fortunately it has a bottom after all..."); /* EN pline("Fortunately it has a bottom after all..."); */ // TODO DE
		    trap->once = 1;
		} else if (u.umonnum == PM_PIT_VIPER ||
			u.umonnum == PM_PIT_FIEND)
		    pline("How pitiful.  Isn't that the pits?"); /* EN pline("How pitiful.  Isn't that the pits?"); */ // TODO DE
		if (ttype == SPIKED_PIT) {
		    const char *predicament = "on a set of sharp iron spikes"; /* EN const char *predicament = "on a set of sharp iron spikes"; */ // TODO DE
#ifdef STEED
		    if (u.usteed) {
			pline("SUBJECT %s lands %s!", /* EN pline("%s lands %s!", */ // TODO DE
				upstart(x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
					 "ADJEKTIV_POOR", SUPPRESS_SADDLE, FALSE)), /* EN "poor", SUPPRESS_SADDLE, FALSE)), */
			      predicament);
		    } else
#endif
		    You("land %s!", predicament); /* EN You("land %s!", predicament); */ // TODO DE
		}
		if (!Passes_walls)
		    u.utrap = rn1(6,2);
		u.utraptype = TT_PIT;
#ifdef STEED
		if (!steedintrap(trap, (struct obj *)0)) {
#endif
		if (ttype == SPIKED_PIT) {
		    losehp(rnd(10),"fell into a pit of iron spikes", /* EN losehp(rnd(10),"fell into a pit of iron spikes", */ // TODO DE
			NO_KILLER_PREFIX);
		    if (!rn2(6))
			poisoned("spikes", A_STR, "fall onto poison spikes", 8); /* EN poisoned("spikes", A_STR, "fall onto poison spikes", 8); */ // TODO DE
		} else
		    losehp(rnd(6),"fiel in eine Grube", NO_KILLER_PREFIX); /* EN losehp(rnd(6),"fell into a pit", NO_KILLER_PREFIX); */
		if (Punished && !carried(uball)) {
		    unplacebc();
		    ballfall();
		    placebc();
		}
		selftouch("Falling, you"); /* EN selftouch("Falling, you"); */ // TODO DE
		vision_full_recalc = 1;	/* vision limits change */
		exercise(A_STR, FALSE);
		exercise(A_DEX, FALSE);
#ifdef STEED
		}
#endif
		break;
	    case HOLE:
	    case TRAPDOOR:
		if (!Can_fall_thru(&u.uz)) {
		    seetrap(trap);	/* normally done in fall_through */
		    impossible("dotrap: %ss cannot exist on this level.",
			       defsyms[trap_to_defsym(ttype)].explanation);
		    break;		/* don't activate it after all */
		}
		fall_through(TRUE);
		break;

	    case TELEP_TRAP:
		seetrap(trap);
		tele_trap(trap);
		break;
	    case LEVEL_TELEP:
		seetrap(trap);
		level_tele_trap(trap);
		break;

	    case WEB: /* Our luckless player has stumbled into a web. */
		seetrap(trap);
		if (amorphous(youmonst.data) || is_whirly(youmonst.data) ||
						    unsolid(youmonst.data)) {
		    if (acidic(youmonst.data) || u.umonnum == PM_GELATINOUS_CUBE ||
			u.umonnum == PM_FIRE_ELEMENTAL) {
			if (webmsgok)
			    You("%s %s OBJECT NOUN_SPIDER_WEB!", /* EN You("%s %s spider web!", */
				(u.umonnum == PM_FIRE_ELEMENTAL) ? "VERB_VERBRENNEN" : "dissolve", /* EN (u.umonnum == PM_FIRE_ELEMENTAL) ? "burn" : "dissolve", */ // TODO DE
				a_your[trap->madeby_u]);
			deltrap(trap);
			newsym(u.ux,u.uy);
			break;
		    }
		    if (webmsgok) You("flow through %s NOUN_SPIDER_WEB.", /* EN if (webmsgok) You("flow through %s spider web.", */ // TODO DE
			    a_your[trap->madeby_u]);
		    break;
		}
		if (webmaker(youmonst.data)) {
		    if (webmsgok)
		    	pline(trap->madeby_u ? "You take a walk on your web." /* EN pline(trap->madeby_u ? "You take a walk on your web." */ // TODO DE
					 : "There is a NOUN_SPIDER_WEB here."); /* EN : "There is a spider web here."); */ // TODO DE
		    break;
		}
		if (webmsgok) {
		    char verbbuf[BUFSZ];
		    verbbuf[0] = '\0';
#ifdef STEED
		    if (u.usteed)
		   	Sprintf(verbbuf,"VERB_FUEHREN OBJECT %s", /* EN Sprintf(verbbuf,"lead %s", */
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "ADJEKTIV_POOR", SUPPRESS_SADDLE, FALSE)); /* EN "poor", SUPPRESS_SADDLE, FALSE)); */
		    else
#endif
			
		    Sprintf(verbbuf, "%s", Levitation ? (const char *)"float" : /* EN Sprintf(verbbuf, "%s", Levitation ? (const char *)"float" : */ // TODO DE
		      		locomotion(youmonst.data, "stumble")); /* EN locomotion(youmonst.data, "stumble")); */ // TODO DE
		    You("%s NEUES_OBJECT OBJECT in %s NOUN_SPIDER_WEB!", /* EN You("%s into %s spider web!", */
			verbbuf, a_your[trap->madeby_u]);
		}
		u.utraptype = TT_WEB;

		/* Time stuck in the web depends on your/steed strength. */
		{
		    register int str = ACURR(A_STR);

#ifdef STEED
		    /* If mounted, the steed gets trapped.  Use mintrap
		     * to do all the work.  If mtrapped is set as a result,
		     * unset it and set utrap instead.  In the case of a
		     * strongmonst and mintrap said it's trapped, use a
		     * short but non-zero trap time.  Otherwise, monsters
		     * have no specific strength, so use player strength.
		     * This gets skipped for webmsgok, which implies that
		     * the steed isn't a factor.
		     */
		    if (u.usteed && webmsgok) {
			/* mtmp location might not be up to date */
			u.usteed->mx = u.ux;
			u.usteed->my = u.uy;

			/* mintrap currently does not return 2(died) for webs */
			if (mintrap(u.usteed)) {
			    u.usteed->mtrapped = 0;
			    if (strongmonst(u.usteed->data)) str = 17;
			} else {
			    break;
			}

			webmsgok = FALSE; /* mintrap printed the messages */
		    }
#endif
		    if (str <= 3) u.utrap = rn1(6,6);
		    else if (str < 6) u.utrap = rn1(6,4);
		    else if (str < 9) u.utrap = rn1(4,4);
		    else if (str < 12) u.utrap = rn1(4,2);
		    else if (str < 15) u.utrap = rn1(2,2);
		    else if (str < 18) u.utrap = rnd(2);
		    else if (str < 69) u.utrap = 1;
		    else {
			u.utrap = 0;
			if (webmsgok)
			    You("tear through %s web!", a_your[trap->madeby_u]); /* EN You("tear through %s web!", a_your[trap->madeby_u]); */ // TODO DE
			deltrap(trap);
			newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    }
		}
		break;

	    case STATUE_TRAP:
		(void) activate_statue_trap(trap, u.ux, u.uy, FALSE);
		break;

	    case MAGIC_TRAP:	    /* A magic trap. */
		seetrap(trap);
		if (!rn2(30)) {
		    deltrap(trap);
		    newsym(u.ux,u.uy);	/* update position */
		    You("are caught in a magical explosion!"); /* EN You("are caught in a magical explosion!"); */ // TODO DE
		    losehp(rnd(10), "magical explosion", KILLED_BY_AN); /* EN losehp(rnd(10), "magical explosion", KILLED_BY_AN); */ // TODO DE
		    Your("body absorbs some of the magical energy!"); /* EN Your("body absorbs some of the magical energy!"); */ // TODO DE
		    u.uen = (u.uenmax += 2);
		} else domagictrap();
#ifdef STEED
		(void) steedintrap(trap, (struct obj *)0);
#endif
		break;

	    case ANTI_MAGIC:
		seetrap(trap);
		if(Antimagic) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily lethargic."); /* EN You_feel("momentarily lethargic."); */ // TODO DE
		} else drain_en(rnd(u.ulevel) + 1);
		break;

	    case POLY_TRAP: {
	        char verbbuf[BUFSZ];
		seetrap(trap);
#ifdef STEED
		if (u.usteed)
			Sprintf(verbbuf, "VERB_FUEHREN OBJECT %s", /* EN Sprintf(verbbuf, "lead %s", */
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 (char *)0, SUPPRESS_SADDLE, FALSE));
		else
#endif
		 Sprintf(verbbuf,"%s",
		    Levitation ? (const char *)"float" : /* EN Levitation ? (const char *)"float" : */ // TODO DE
		    locomotion(youmonst.data, "step")); /* EN locomotion(youmonst.data, "step")); */ // TODO DE
		You("%s onto a polymorph trap!", verbbuf); /* EN You("%s onto a polymorph trap!", verbbuf); */ // TODO DE
		if(Antimagic || Unchanging) {
		    shieldeff(u.ux, u.uy);
		    You_feel("momentarily different."); /* EN You_feel("momentarily different."); */ // TODO DE
		    /* Trap did nothing; don't remove it --KAA */
		} else {
#ifdef STEED
		    (void) steedintrap(trap, (struct obj *)0);
#endif
		    deltrap(trap);	/* delete trap before polymorph */
		    newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    You_feel("a change coming over you."); /* EN You_feel("a change coming over you."); */ // TODO DE
		    polyself(FALSE);
		}
		break;
	    }
	    case LANDMINE: {
#ifdef STEED
		unsigned steed_mid = 0;
		struct obj *saddle;
#endif
		if (Levitation || Flying) {
		    if (!already_seen && rn2(3)) break;
		    seetrap(trap);
		    pline("%s %s in a pile of soil below you.", /* EN pline("%s %s in a pile of soil below you.", */ // TODO DE
			    already_seen ? "There is" : "You discover", /* EN already_seen ? "There is" : "You discover", */ // TODO DE
			    trap->madeby_u ? "the trigger of your mine" : /* EN trap->madeby_u ? "the trigger of your mine" : */ // TODO DE
					     "a trigger"); /* EN "a trigger"); */ // TODO DE
		    if (already_seen && rn2(3)) break;
		    pline("KAABLAMM!!!  %s %s%s off!", /* EN pline("KAABLAMM!!!  %s %s%s off!", */ // TODO DE
		    	    forcebungle ? "Your inept attempt sets" : /* EN forcebungle ? "Your inept attempt sets" : */ // TODO DE
		    	    		  "The air currents set", /* EN "The air currents set", */ // TODO DE
			    already_seen ? a_your[trap->madeby_u] : "",
			    already_seen ? " land mine" : "it"); /* EN already_seen ? " land mine" : "it"); */ // TODO DE
		} else {
#ifdef STEED
		    /* prevent landmine from killing steed, throwing you to
		     * the ground, and you being affected again by the same
		     * mine because it hasn't been deleted yet
		     */
		    static boolean recursive_mine = FALSE;

		    if (recursive_mine) break;
#endif
		    seetrap(trap);
		    pline("KAABLAMM!!!  You triggered %s land mine!", /* EN pline("KAABLAMM!!!  You triggered %s land mine!", */ // TODO DE
					    a_your[trap->madeby_u]);
#ifdef STEED
		    if (u.usteed) steed_mid = u.usteed->m_id;
		    recursive_mine = TRUE;
		    (void) steedintrap(trap, (struct obj *)0);
		    recursive_mine = FALSE;
		    saddle = sobj_at(SADDLE,u.ux, u.uy);
#endif
		    set_wounded_legs(LEFT_SIDE, rn1(35, 41));
		    set_wounded_legs(RIGHT_SIDE, rn1(35, 41));
		    exercise(A_DEX, FALSE);
		}
		blow_up_landmine(trap);
#ifdef STEED
		if (steed_mid && saddle && !u.usteed)
			(void)keep_saddle_with_steedcorpse(steed_mid, fobj, saddle);
#endif
		newsym(u.ux,u.uy);		/* update trap symbol */
		losehp(rnd(16), "NOUN_LAND_MINE", KILLED_BY_AN); /* EN losehp(rnd(16), "land mine", KILLED_BY_AN); */
		/* fall recursively into the pit... */
		if ((trap = t_at(u.ux, u.uy)) != 0) dotrap(trap, RECURSIVETRAP);
		fill_pit(u.ux, u.uy);
		break;
	    }
	    case ROLLING_BOULDER_TRAP: {
		int style = ROLL | (trap->tseen ? LAUNCH_KNOWN : 0);

		seetrap(trap);
		pline("Klick! SUBJECT PRONOMEN_PERSONAL VERB_AUSLOESEN OBJECT ARTIKEL_UNBESTIMMTER NOUN_ROLLING_BOULDER_TRAP SATZKLAMMER!"); /* EN pline("Click! You trigger a rolling boulder trap!"); */
		if(!launch_obj(BOULDER, trap->launch.x, trap->launch.y,
		      trap->launch2.x, trap->launch2.y, style)) {
		    deltrap(trap);
		    newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    pline("Fortunately for you, no boulder was released."); /* EN pline("Fortunately for you, no boulder was released."); */ // TODO DE
		}
		break;
	    }
	    case MAGIC_PORTAL:
		seetrap(trap);
		domagicportal(trap);
		break;

	    default:
		seetrap(trap);
		impossible("You hit a trap of type %u", trap->ttyp);
	}
}

#ifdef STEED
STATIC_OVL int
steedintrap(trap, otmp)
struct trap *trap;
struct obj *otmp;
{
	struct monst *mtmp = u.usteed;
	struct permonst *mptr;
	int tt;
	boolean in_sight;
	boolean trapkilled = FALSE;
	boolean steedhit = FALSE;

	if (!u.usteed || !trap) return 0;
	mptr = mtmp->data;
	tt = trap->ttyp;
	mtmp->mx = u.ux;
	mtmp->my = u.uy;

	in_sight = !Blind;
	switch (tt) {
		case ARROW_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant arrow?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case DART_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant dart?");
				return 0;
			}
			if (thitm(7, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case SLP_GAS_TRAP:
		    if (!resists_sleep(mtmp) && !breathless(mptr) &&
				!mtmp->msleeping && mtmp->mcanmove) {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(25);
			    if (in_sight) {
				pline("SUBJECT %s VERB_EINSCHLAFEN plötzlich SATZKLAMMER!", /* EN pline("%s suddenly falls asleep!", */
				      Monnam(mtmp));
			    }
			}
			steedhit = TRUE;
			break;
		case LANDMINE:
			if (thitm(0, mtmp, (struct obj *)0, rnd(16), FALSE))
			    trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case PIT:
		case SPIKED_PIT:
			if (mtmp->mhp <= 0 ||
				thitm(0, mtmp, (struct obj *)0,
				      rnd((tt == PIT) ? 6 : 10), FALSE))
			    trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case POLY_TRAP: 
		    if (!resists_magm(mtmp)) {
			if (!resist(mtmp, WAND_CLASS, 0, NOTELL)) {
			(void) newcham(mtmp, (struct permonst *)0,
				       FALSE, FALSE);
			if (!can_saddle(mtmp) || !can_ride(mtmp)) {
				dismount_steed(DISMOUNT_POLY);
			} else {
				You("have to adjust yourself in the saddle on %s.", /* EN You("have to adjust yourself in the saddle on %s.", */ // TODO DE
					x_monnam(mtmp,
					 mtmp->mnamelth ? ARTICLE_NONE : ARTICLE_A,
				 	 (char *)0, SUPPRESS_SADDLE, FALSE));
			}
				
		    }
		    steedhit = TRUE;
		    break;
		default:
			return 0;
	    }
	}
	if(trapkilled) {
		dismount_steed(DISMOUNT_POLY);
		return 2;
	}
	else if(steedhit) return 1;
	else return 0;
}
#endif /*STEED*/

/* some actions common to both player and monsters for triggered landmine */
void
blow_up_landmine(trap)
struct trap *trap;
{
	(void)scatter(trap->tx, trap->ty, 4,
		MAY_DESTROY | MAY_HIT | MAY_FRACTURE | VIS_EFFECTS,
		(struct obj *)0);
	del_engr_at(trap->tx, trap->ty);
	wake_nearto(trap->tx, trap->ty, 400);
	if (IS_DOOR(levl[trap->tx][trap->ty].typ))
	    levl[trap->tx][trap->ty].doormask = D_BROKEN;
	/* TODO: destroy drawbridge if present */
	/* caller may subsequently fill pit, e.g. with a boulder */
	trap->ttyp = PIT;		/* explosion creates a pit */
	trap->madeby_u = FALSE;		/* resulting pit isn't yours */
	seetrap(trap);			/* and it isn't concealed */
}

#endif /* OVLB */
#ifdef OVL3

/*
 * Move obj from (x1,y1) to (x2,y2)
 *
 * Return 0 if no object was launched.
 *        1 if an object was launched and placed somewhere.
 *        2 if an object was launched, but used up.
 */
int
launch_obj(otyp, x1, y1, x2, y2, style)
short otyp;
register int x1,y1,x2,y2;
int style;
{
	register struct monst *mtmp;
	register struct obj *otmp, *otmp2;
	register int dx,dy;
	struct obj *singleobj;
	boolean used_up = FALSE;
	boolean otherside = FALSE;
	int dist;
	int tmp;
	int delaycnt = 0;

	otmp = sobj_at(otyp, x1, y1);
	/* Try the other side too, for rolling boulder traps */
	if (!otmp && otyp == BOULDER) {
		otherside = TRUE;
		otmp = sobj_at(otyp, x2, y2);
	}
	if (!otmp) return 0;
	if (otherside) {	/* swap 'em */
		int tx, ty;

		tx = x1; ty = y1;
		x1 = x2; y1 = y2;
		x2 = tx; y2 = ty;
	}

	if (otmp->quan == 1L) {
	    obj_extract_self(otmp);
	    singleobj = otmp;
	    otmp = (struct obj *) 0;
	} else {
	    singleobj = splitobj(otmp, 1L);
	    obj_extract_self(singleobj);
	}
	newsym(x1,y1);
	/* in case you're using a pick-axe to chop the boulder that's being
	   launched (perhaps a monster triggered it), destroy context so that
	   next dig attempt never thinks you're resuming previous effort */
	if ((otyp == BOULDER || otyp == STATUE) &&
	    singleobj->ox == digging.pos.x && singleobj->oy == digging.pos.y)
	    (void) memset((genericptr_t)&digging, 0, sizeof digging);

	dist = distmin(x1,y1,x2,y2);
	bhitpos.x = x1;
	bhitpos.y = y1;
	dx = sgn(x2 - x1);
	dy = sgn(y2 - y1);
	switch (style) {
	    case ROLL|LAUNCH_UNSEEN:
			if (otyp == BOULDER) {
			    You_hear(Hallucination ?
				     "jemanden kegeln." : /* EN "someone bowling." : */
				     "rumbling in the distance."); /* EN "rumbling in the distance."); */ // TODO DE
			}
			style &= ~LAUNCH_UNSEEN;
			goto roll;
	    case ROLL|LAUNCH_KNOWN:
			/* use otrapped as a flag to ohitmon */
			singleobj->otrapped = 1;
			style &= ~LAUNCH_KNOWN;
			/* fall through */
	    roll:
	    case ROLL:
			delaycnt = 2;
			/* fall through */
	    default:
			if (!delaycnt) delaycnt = 1;
			if (!cansee(bhitpos.x,bhitpos.y)) curs_on_u();
			tmp_at(DISP_FLASH, obj_to_glyph(singleobj));
			tmp_at(bhitpos.x, bhitpos.y);
	}

	/* Set the object in motion */
	while(dist-- > 0 && !used_up) {
		struct trap *t;
		tmp_at(bhitpos.x, bhitpos.y);
		tmp = delaycnt;

		/* dstage@u.washington.edu -- Delay only if hero sees it */
		if (cansee(bhitpos.x, bhitpos.y))
			while (tmp-- > 0) delay_output();

		bhitpos.x += dx;
		bhitpos.y += dy;
		t = t_at(bhitpos.x, bhitpos.y);
		
		if ((mtmp = m_at(bhitpos.x, bhitpos.y)) != 0) {
			if (otyp == BOULDER && throws_rocks(mtmp->data)) {
			    if (rn2(3)) {
				pline("SUBJECT %s VERB_PACKEN sich OBJECT ARTIKEL_BESTIMMTER NOUN_BOULDER.", /* EN pline("%s snatches the boulder.", */
					Monnam(mtmp));
				singleobj->otrapped = 0;
				(void) mpickobj(mtmp, singleobj);
				used_up = TRUE;
				break;
			    }
			}
			if (ohitmon(mtmp,singleobj,
					(style==ROLL) ? -1 : dist, FALSE)) {
				used_up = TRUE;
				break;
			}
		} else if (bhitpos.x == u.ux && bhitpos.y == u.uy) {
			if (multi) nomul(0);
			if (thitu(9 + singleobj->spe,
				  dmgval(singleobj, &youmonst),
				  singleobj, (char *)0))
			    stop_occupation();
		}
		if (style == ROLL) {
		    if (down_gate(bhitpos.x, bhitpos.y) != -1) {
		        if(ship_object(singleobj, bhitpos.x, bhitpos.y, FALSE)){
				used_up = TRUE;
				break;
			}
		    }
		    if (t && otyp == BOULDER) {
			switch(t->ttyp) {
			case LANDMINE:
			    if (rn2(10) > 2) {
			  	pline(
				  "KAABLAMM!!!%s", /* EN "KAABLAMM!!!%s", */ // TODO DE
				  cansee(bhitpos.x, bhitpos.y) ?
					" The rolling boulder triggers a land mine." : ""); /* EN " The rolling boulder triggers a land mine." : ""); */ // TODO DE
				deltrap(t);
				del_engr_at(bhitpos.x,bhitpos.y);
				place_object(singleobj, bhitpos.x, bhitpos.y);
				singleobj->otrapped = 0;
				fracture_rock(singleobj);
				(void)scatter(bhitpos.x,bhitpos.y, 4,
					MAY_DESTROY|MAY_HIT|MAY_FRACTURE|VIS_EFFECTS,
					(struct obj *)0);
				if (cansee(bhitpos.x,bhitpos.y))
					newsym(bhitpos.x,bhitpos.y);
			        used_up = TRUE;
			    }
			    break;		
			case LEVEL_TELEP:
			case TELEP_TRAP:
			    if (cansee(bhitpos.x, bhitpos.y))
			    	pline("Suddenly the rolling boulder disappears!"); /* EN pline("Suddenly the rolling boulder disappears!"); */ // TODO DE
			    else
			    	You_hear("a rumbling stop abruptly."); /* EN You_hear("a rumbling stop abruptly."); */ // TODO DE
			    singleobj->otrapped = 0;
			    if (t->ttyp == TELEP_TRAP)
				rloco(singleobj);
			    else {
				int newlev = random_teleport_level();
				d_level dest;

				if (newlev == depth(&u.uz) || In_endgame(&u.uz))
				    continue;
				add_to_migration(singleobj);
				get_level(&dest, newlev);
				singleobj->ox = dest.dnum;
				singleobj->oy = dest.dlevel;
				singleobj->owornmask = (long)MIGR_RANDOM;
			    }
		    	    seetrap(t);
			    used_up = TRUE;
			    break;
			case PIT:
			case SPIKED_PIT:
			case HOLE:
			case TRAPDOOR:
			    /* the boulder won't be used up if there is a
			       monster in the trap; stop rolling anyway */
			    x2 = bhitpos.x,  y2 = bhitpos.y;  /* stops here */
			    if (flooreffects(singleobj, x2, y2, "VERB_FALLEN")) /* EN if (flooreffects(singleobj, x2, y2, "fall")) */
				used_up = TRUE;
			    dist = -1;	/* stop rolling immediately */
			    break;
			}
			if (used_up || dist == -1) break;
		    }
		    if (flooreffects(singleobj, bhitpos.x, bhitpos.y, "VERB_FALLEN")) { /* EN if (flooreffects(singleobj, bhitpos.x, bhitpos.y, "fall")) { */
			used_up = TRUE;
			break;
		    }
		    if (otyp == BOULDER &&
		       (otmp2 = sobj_at(BOULDER, bhitpos.x, bhitpos.y)) != 0) {
			const char *bmsg =
				     " as one boulder sets another in motion"; /* EN " as one boulder sets another in motion"; */ // TODO DE

			if (!isok(bhitpos.x + dx, bhitpos.y + dy) || !dist ||
			    IS_ROCK(levl[bhitpos.x + dx][bhitpos.y + dy].typ))
			    bmsg = " as one boulder hits another"; /* EN bmsg = " as one boulder hits another"; */ // TODO DE

			You_hear("a loud crash%s!", /* EN You_hear("a loud crash%s!", */ // TODO DE
				cansee(bhitpos.x, bhitpos.y) ? bmsg : "");
			obj_extract_self(otmp2);
			/* pass off the otrapped flag to the next boulder */
			otmp2->otrapped = singleobj->otrapped;
			singleobj->otrapped = 0;
			place_object(singleobj, bhitpos.x, bhitpos.y);
			singleobj = otmp2;
			otmp2 = (struct obj *)0;
			wake_nearto(bhitpos.x, bhitpos.y, 10*10);
		    }
		}
		if (otyp == BOULDER && closed_door(bhitpos.x,bhitpos.y)) {
			if (cansee(bhitpos.x, bhitpos.y))
				pline_The("NOUN_BOULDER VERB_DONNERN durch eine Tür."); /* EN pline_The("boulder crashes through a door."); */
			levl[bhitpos.x][bhitpos.y].doormask = D_BROKEN;
			if (dist) unblock_point(bhitpos.x, bhitpos.y);
		}

		/* if about to hit iron bars, do so now */
		if (dist > 0 && isok(bhitpos.x + dx,bhitpos.y + dy) &&
			levl[bhitpos.x + dx][bhitpos.y + dy].typ == IRONBARS) {
		    x2 = bhitpos.x,  y2 = bhitpos.y;	/* object stops here */
		    if (hits_bars(&singleobj, x2, y2, !rn2(20), 0)) {
			if (!singleobj) used_up = TRUE;
			break;
		    }
		}
	}
	tmp_at(DISP_END, 0);
	if (!used_up) {
		singleobj->otrapped = 0;
		place_object(singleobj, x2,y2);
		newsym(x2,y2);
		return 1;
	} else
		return 2;
}

#endif /* OVL3 */
#ifdef OVLB

void
seetrap(trap)
	register struct trap *trap;
{
	if(!trap->tseen) {
	    trap->tseen = 1;
	    newsym(trap->tx, trap->ty);
	}
}

#endif /* OVLB */
#ifdef OVL3

STATIC_OVL int
mkroll_launch(ttmp, x, y, otyp, ocount)
struct trap *ttmp;
xchar x,y;
short otyp;
long ocount;
{
	struct obj *otmp;
	register int tmp;
	schar dx,dy;
	int distance;
	coord cc;
	coord bcc;
	int trycount = 0;
	boolean success = FALSE;
	int mindist = 4;

	if (ttmp->ttyp == ROLLING_BOULDER_TRAP) mindist = 2;
	distance = rn1(5,4);    /* 4..8 away */
	tmp = rn2(8);		/* randomly pick a direction to try first */
	while (distance >= mindist) {
		dx = xdir[tmp];
		dy = ydir[tmp];
		cc.x = x; cc.y = y;
		/* Prevent boulder from being placed on water */
		if (ttmp->ttyp == ROLLING_BOULDER_TRAP
				&& is_pool(x+distance*dx,y+distance*dy))
			success = FALSE;
		else success = isclearpath(&cc, distance, dx, dy);
		if (ttmp->ttyp == ROLLING_BOULDER_TRAP) {
			boolean success_otherway;
			bcc.x = x; bcc.y = y;
			success_otherway = isclearpath(&bcc, distance,
						-(dx), -(dy));
			if (!success_otherway) success = FALSE;
		}
		if (success) break;
		if (++tmp > 7) tmp = 0;
		if ((++trycount % 8) == 0) --distance;
	}
	if (!success) {
	    /* create the trap without any ammo, launch pt at trap location */
		cc.x = bcc.x = x;
		cc.y = bcc.y = y;
	} else {
		otmp = mksobj(otyp, TRUE, FALSE);
		otmp->quan = ocount;
		otmp->owt = weight(otmp);
		place_object(otmp, cc.x, cc.y);
		stackobj(otmp);
	}
	ttmp->launch.x = cc.x;
	ttmp->launch.y = cc.y;
	if (ttmp->ttyp == ROLLING_BOULDER_TRAP) {
		ttmp->launch2.x = bcc.x;
		ttmp->launch2.y = bcc.y;
	} else
		ttmp->launch_otyp = otyp;
	newsym(ttmp->launch.x, ttmp->launch.y);
	return 1;
}

STATIC_OVL boolean
isclearpath(cc,distance,dx,dy)
coord *cc;
int distance;
schar dx,dy;
{
	uchar typ;
	xchar x, y;

	x = cc->x;
	y = cc->y;
	while (distance-- > 0) {
		x += dx;
		y += dy;
		typ = levl[x][y].typ;
		if (!isok(x,y) || !ZAP_POS(typ) || closed_door(x,y))
			return FALSE;
	}
	cc->x = x;
	cc->y = y;
	return TRUE;
}
#endif /* OVL3 */
#ifdef OVL1

int
mintrap(mtmp)
register struct monst *mtmp;
{
	register struct trap *trap = t_at(mtmp->mx, mtmp->my);
	boolean trapkilled = FALSE;
	struct permonst *mptr = mtmp->data;
	struct obj *otmp;

	if (!trap) {
	    mtmp->mtrapped = 0;	/* perhaps teleported? */
	} else if (mtmp->mtrapped) {	/* is currently in the trap */
	    if (!trap->tseen &&
		cansee(mtmp->mx, mtmp->my) && canseemon(mtmp) &&
		(trap->ttyp == SPIKED_PIT || trap->ttyp == BEAR_TRAP ||
		 trap->ttyp == HOLE || trap->ttyp == PIT ||
		 trap->ttyp == WEB)) {
		/* If you come upon an obviously trapped monster, then
		 * you must be able to see the trap it's in too.
		 */
		seetrap(trap);
	    }
		
	    if (!rn2(40)) {
		if (sobj_at(BOULDER, mtmp->mx, mtmp->my) &&
			(trap->ttyp == PIT || trap->ttyp == SPIKED_PIT)) {
		    if (!rn2(2)) {
			mtmp->mtrapped = 0;
			if (canseemon(mtmp))
			    pline("SUBJECT %s VERB_REISSEN sich los ...", Monnam(mtmp)); /* EN pline("%s pulls free...", Monnam(mtmp)); */
			fill_pit(mtmp->mx, mtmp->my);
		    }
		} else {
		    mtmp->mtrapped = 0;
		}
	    } else if (metallivorous(mptr)) {
		if (trap->ttyp == BEAR_TRAP) {
		    if (canseemon(mtmp))
			pline("SUBJECT %s VERB_EAT OBJECT ARTIKEL_UNBESTIMMTER NOUN_BEAR_TRAP!", Monnam(mtmp)); /* EN pline("%s eats a bear trap!", Monnam(mtmp)); */
		    deltrap(trap);
		    mtmp->meating = 5;
		    mtmp->mtrapped = 0;
		} else if (trap->ttyp == SPIKED_PIT) {
		    if (canseemon(mtmp))
			pline("%s munches on some spikes!", Monnam(mtmp)); /* EN pline("%s munches on some spikes!", Monnam(mtmp)); */ // TODO DE
		    trap->ttyp = PIT;
		    mtmp->meating = 5;
		}
	    }
	} else {
	    register int tt = trap->ttyp;
	    boolean in_sight, tear_web, see_it,
		    inescapable = ((tt == HOLE || tt == PIT) &&
				   In_sokoban(&u.uz) && !trap->madeby_u);
	    const char *fallverb;

#ifdef STEED
	    /* true when called from dotrap, inescapable is not an option */
	    if (mtmp == u.usteed) inescapable = TRUE;
#endif
	    if (!inescapable &&
		    ((mtmp->mtrapseen & (1 << (tt-1))) != 0 ||
			(tt == HOLE && !mindless(mtmp->data)))) {
		/* it has been in such a trap - perhaps it escapes */
		if(rn2(4)) return(0);
	    } else {
		mtmp->mtrapseen |= (1 << (tt-1));
	    }
	    /* Monster is aggravated by being trapped by you.
	       Recognizing who made the trap isn't completely
	       unreasonable; everybody has their own style. */
	    if (trap->madeby_u && rnl(5)) setmangry(mtmp);

	    in_sight = canseemon(mtmp);
	    see_it = cansee(mtmp->mx, mtmp->my);
#ifdef STEED
	    /* assume hero can tell what's going on for the steed */
	    if (mtmp == u.usteed) in_sight = TRUE;
#endif
	    switch (tt) {
		case ARROW_TRAP:
			if (trap->once && trap->tseen && !rn2(15)) {
			    if (in_sight && see_it)
				pline("SUBJECT %s VERB_AKTIVIEREN eine Falle aber nichts geschieht.", /* EN pline("%s triggers a trap but nothing happens.", */
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(ARROW, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (in_sight) seetrap(trap);
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			break;
		case DART_TRAP:
			if (trap->once && trap->tseen && !rn2(15)) {
			    if (in_sight && see_it)
				pline("%s triggers a trap but nothing happens.", /* EN pline("%s triggers a trap but nothing happens.", */ // TODO DE
				      Monnam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(DART, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			if (!rn2(6)) otmp->opoisoned = 1;
			if (in_sight) seetrap(trap);
			if (thitm(7, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			break;
		case ROCKTRAP:
			if (trap->once && trap->tseen && !rn2(15)) {
			    if (in_sight && see_it)
				pline("A trap door above %s opens, but nothing falls out!", /* EN pline("A trap door above %s opens, but nothing falls out!", */ // TODO DE
				      mon_nam(mtmp));
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(ROCK, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			if (in_sight) seetrap(trap);
			if (thitm(0, mtmp, otmp, d(2, 6), FALSE))
			    trapkilled = TRUE;
			break;

		case SQKY_BOARD:
			if(is_flyer(mptr)) break;
			/* stepped on a squeaky board */
			if (in_sight) {
			    pline("A board beneath %s squeaks loudly.", mon_nam(mtmp)); /* EN pline("A board beneath %s squeaks loudly.", mon_nam(mtmp)); */ // TODO DE
			    seetrap(trap);
			} else
			   You_hear("a distant squeak."); /* EN You_hear("a distant squeak."); */ // TODO DE
			/* wake up nearby monsters */
			wake_nearto(mtmp->mx, mtmp->my, 40);
			break;

		case BEAR_TRAP:
			if(mptr->msize > MZ_SMALL &&
				!amorphous(mptr) && !is_flyer(mptr) &&
				!is_whirly(mptr) && !unsolid(mptr)) {
			    mtmp->mtrapped = 1;
			    if(in_sight) {
				pline("%s is caught in %s bear trap!", /* EN pline("%s is caught in %s bear trap!", */ // TODO DE
				      Monnam(mtmp), a_your[trap->madeby_u]);
				seetrap(trap);
			    } else {
				if((mptr == &mons[PM_OWLBEAR]
				    || mptr == &mons[PM_BUGBEAR])
				   && flags.soundok)
				    You_hear("the roaring of an angry bear!"); /* EN You_hear("the roaring of an angry bear!"); */ // TODO DE
			    }
			}
			break;

		case SLP_GAS_TRAP:
		    if (!resists_sleep(mtmp) && !breathless(mptr) &&
				!mtmp->msleeping && mtmp->mcanmove) {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(25);
			    if (in_sight) {
				pline("SUBJECT %s VERB_EINSCHLAFEN plötzlich SATZKLAMMER!", /* EN pline("%s suddenly falls asleep!", */
				      Monnam(mtmp));
				seetrap(trap);
			    }
			}
			break;

		case RUST_TRAP:
		    {
			struct obj *target;

			if (in_sight)
			    seetrap(trap);
			switch (rn2(5)) {
			case 0:
			    if (in_sight)
				pline("%s OBJECT %s KASUS_DATIV _an_ ARTIKEL_BESTIMMTER %s!", A_gush_of_water_hits, /* EN pline("%s %s on the %s!", A_gush_of_water_hits, */
				    mon_nam(mtmp), mbodypart(mtmp, HEAD));
			    target = which_armor(mtmp, W_ARMH);
			    (void) rust_dmg(target, "NOUN_HELMET", 1, TRUE, mtmp); /* EN (void) rust_dmg(target, "helmet", 1, TRUE, mtmp); */
			    break;
			case 1:
			    if (in_sight)
				pline("%s OBJECT ADJEKTIV_LINK %s NEUES_OBJECT OBJECT KASUS_GENITIV %s!", A_gush_of_water_hits, /* EN pline("%s %s's left %s!", A_gush_of_water_hits, */
				    mbodypart(mtmp, ARM), mon_nam(mtmp)); /* EN mon_nam(mtmp), mbodypart(mtmp, ARM)); */
			    target = which_armor(mtmp, W_ARMS);
			    if (rust_dmg(target, "NOUN_SHIELD", 1, TRUE, mtmp)) /* EN if (rust_dmg(target, "shield", 1, TRUE, mtmp)) */
				break;
			    target = MON_WEP(mtmp);
			    if (target && bimanual(target))
				erode_obj(target, FALSE, TRUE);
glovecheck:		    target = which_armor(mtmp, W_ARMG);
			    (void) rust_dmg(target, "gauntlets", 1, TRUE, mtmp); /* EN (void) rust_dmg(target, "gauntlets", 1, TRUE, mtmp); */ // TODO DE
			    break;
			case 2:
			    if (in_sight)
				pline("%s OBJECT ADJEKTIV_RECHT %s NEUES_OBJECT OBJECT KASUS_GENITIV %s!", A_gush_of_water_hits, /* EN pline("%s %s's right %s!", A_gush_of_water_hits, */
				    mbodypart(mtmp, ARM), mon_nam(mtmp)); /* EN mon_nam(mtmp), mbodypart(mtmp, ARM)); */
			    erode_obj(MON_WEP(mtmp), FALSE, TRUE);
			    goto glovecheck;
			default:
			    if (in_sight)
				pline("%s OBJECT %s!", A_gush_of_water_hits, /* EN pline("%s %s!", A_gush_of_water_hits, */
				    mon_nam(mtmp));
			    for (otmp=mtmp->minvent; otmp; otmp = otmp->nobj)
				(void) snuff_lit(otmp);
			    target = which_armor(mtmp, W_ARMC);
			    if (target)
				(void) rust_dmg(target, cloak_simple_name(target),
						 1, TRUE, mtmp);
			    else {
				target = which_armor(mtmp, W_ARM);
				if (target)
				    (void) rust_dmg(target, "NOUN_ARMOR", 1, TRUE, mtmp); /* EN (void) rust_dmg(target, "armor", 1, TRUE, mtmp); */
#ifdef TOURIST
				else {
				    target = which_armor(mtmp, W_ARMU);
				    (void) rust_dmg(target, "NOUN_SHIRT", 1, TRUE, mtmp); /* EN (void) rust_dmg(target, "shirt", 1, TRUE, mtmp); */
				}
#endif
			    }
			}
			if (mptr == &mons[PM_IRON_GOLEM]) {
				if (in_sight)
				    pline("%s falls to pieces!", Monnam(mtmp)); /* EN pline("%s falls to pieces!", Monnam(mtmp)); */ // TODO DE
				else if(mtmp->mtame)
				    pline("Möge %s in Frieden rosten.", /* EN pline("May %s rust in peace.", */
								mon_nam(mtmp));
				mondied(mtmp);
				if (mtmp->mhp <= 0)
					trapkilled = TRUE;
			} else if (mptr == &mons[PM_GREMLIN] && rn2(3)) {
				(void)split_mon(mtmp, (struct monst *)0);
			}
			break;
		    }
		case FIRE_TRAP:
 mfiretrap:
			if (in_sight)
			    pline("A %s erupts from the %s under %s!", /* EN pline("A %s erupts from the %s under %s!", */ // TODO DE
				  tower_of_flame,
				  surface(mtmp->mx,mtmp->my), mon_nam(mtmp));
			else if (see_it)  /* evidently `mtmp' is invisible */
			    You("see a %s erupt from the %s!", /* EN You("see a %s erupt from the %s!", */ // TODO DE
				tower_of_flame, surface(mtmp->mx,mtmp->my));

			if (resists_fire(mtmp)) {
			    if (in_sight) {
				shieldeff(mtmp->mx,mtmp->my);
				pline("%s is uninjured.", Monnam(mtmp)); /* EN pline("%s is uninjured.", Monnam(mtmp)); */ // TODO DE
			    }
			} else {
			    int num = d(2,4), alt;
			    boolean immolate = FALSE;

			    /* paper burns very fast, assume straw is tightly
			     * packed and burns a bit slower */
			    switch (monsndx(mtmp->data)) {
			    case PM_PAPER_GOLEM:   immolate = TRUE;
						   alt = mtmp->mhpmax; break;
			    case PM_STRAW_GOLEM:   alt = mtmp->mhpmax / 2; break;
			    case PM_WOOD_GOLEM:    alt = mtmp->mhpmax / 4; break;
			    case PM_LEATHER_GOLEM: alt = mtmp->mhpmax / 8; break;
			    default: alt = 0; break;
			    }
			    if (alt > num) num = alt;

			    if (thitm(0, mtmp, (struct obj *)0, num, immolate))
				trapkilled = TRUE;
			    else
				/* we know mhp is at least `num' below mhpmax,
				   so no (mhp > mhpmax) check is needed here */
				mtmp->mhpmax -= rn2(num + 1);
			}
			if (burnarmor(mtmp) || rn2(3)) {
			    (void) destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
			    (void) destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
			    (void) destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);
			}
			if (burn_floor_paper(mtmp->mx, mtmp->my, see_it, FALSE) &&
				!see_it && distu(mtmp->mx, mtmp->my) <= 3*3)
			    You("VERB_SMELL Rauch."); /* EN You("smell smoke."); */
			if (is_ice(mtmp->mx,mtmp->my))
			    melt_ice(mtmp->mx,mtmp->my);
			if (see_it) seetrap(trap);
			break;

		case PIT:
		case SPIKED_PIT:
			fallverb = "falls"; /* EN fallverb = "falls"; */ // TODO DE
			if (is_flyer(mptr) || is_floater(mptr) ||
				(mtmp->wormno && count_wsegs(mtmp) > 5) ||
				is_clinger(mptr)) {
			    if (!inescapable) break;	/* avoids trap */
			    fallverb = "is dragged";	/* sokoban pit */ /* EN fallverb = "is dragged";	*/ // TODO DE
			}
			if (!passes_walls(mptr))
			    mtmp->mtrapped = 1;
			if (in_sight) {
			    pline("%s %s into %s pit!", /* EN pline("%s %s into %s pit!", */ // TODO DE
				  Monnam(mtmp), fallverb,
				  a_your[trap->madeby_u]);
			    if (mptr == &mons[PM_PIT_VIPER] || mptr == &mons[PM_PIT_FIEND])
				pline("How pitiful.  Isn't that the pits?"); /* EN pline("How pitiful.  Isn't that the pits?"); */ // TODO DE
			    seetrap(trap);
			}
			mselftouch(mtmp, "Falling, ", FALSE); /* EN mselftouch(mtmp, "Falling, ", FALSE); */ // TODO DE
			if (mtmp->mhp <= 0 ||
				thitm(0, mtmp, (struct obj *)0,
				      rnd((tt == PIT) ? 6 : 10), FALSE))
			    trapkilled = TRUE;
			break;
		case HOLE:
		case TRAPDOOR:
			if (!Can_fall_thru(&u.uz)) {
			 impossible("mintrap: %ss cannot exist on this level.",
				    defsyms[trap_to_defsym(tt)].explanation);
			    break;	/* don't activate it after all */
			}
			if (is_flyer(mptr) || is_floater(mptr) ||
				mptr == &mons[PM_WUMPUS] ||
				(mtmp->wormno && count_wsegs(mtmp) > 5) ||
				mptr->msize >= MZ_HUGE) {
			    if (inescapable) {	/* sokoban hole */
				if (in_sight) {
				    pline("%s seems to be yanked down!", /* EN pline("%s seems to be yanked down!", */ // TODO DE
					  Monnam(mtmp));
				    /* suppress message in mlevel_tele_trap() */
				    in_sight = FALSE;
				    seetrap(trap);
				}
			    } else
				break;
			}
			/* Fall through */
		case LEVEL_TELEP:
		case MAGIC_PORTAL:
			{
			    int mlev_res;
			    mlev_res = mlevel_tele_trap(mtmp, trap,
							inescapable, in_sight);
			    if (mlev_res) return(mlev_res);
			}
			break;

		case TELEP_TRAP:
			mtele_trap(mtmp, trap, in_sight);
			break;

		case WEB:
			/* Monster in a web. */
			if (webmaker(mptr)) break;
			if (amorphous(mptr) || is_whirly(mptr) || unsolid(mptr)){
			    if(acidic(mptr) ||
			       mptr == &mons[PM_GELATINOUS_CUBE] ||
			       mptr == &mons[PM_FIRE_ELEMENTAL]) {
				if (in_sight)
				    pline("SUBJECT %s %s OBJECT %s NOUN_SPIDER_WEB!", /* EN pline("%s %s %s spider web!", */
					  Monnam(mtmp),
					  (mptr == &mons[PM_FIRE_ELEMENTAL]) ?
					    "VERB_VERBRENNEN" : "dissolves", /* EN "burns" : "dissolves", */ // TODO DE
					  a_your[trap->madeby_u]);
				deltrap(trap);
				newsym(mtmp->mx, mtmp->my);
				break;
			    }
			    if (in_sight) {
				pline("%s flows through %s NOUN_SPIDER_WEB.", /* EN pline("%s flows through %s spider web.", */ // TODO DE
				      Monnam(mtmp),
				      a_your[trap->madeby_u]);
				seetrap(trap);
			    }
			    break;
			}
			tear_web = FALSE;
			switch (monsndx(mptr)) {
			    case PM_OWLBEAR: /* Eric Backus */
			    case PM_BUGBEAR:
				if (!in_sight) {
				    You_hear("the roaring of a confused bear!"); /* EN You_hear("the roaring of a confused bear!"); */ // TODO DE
				    mtmp->mtrapped = 1;
				    break;
				}
				/* fall though */
			    default:
				if (mptr->mlet == S_GIANT ||
				    (mptr->mlet == S_DRAGON &&
					extra_nasty(mptr)) || /* excl. babies */
				    (mtmp->wormno && count_wsegs(mtmp) > 5)) {
				    tear_web = TRUE;
				} else if (in_sight) {
				    pline("%s is caught in %s NOUN_SPIDER_WEB.", /* EN pline("%s is caught in %s spider web.", */ // TODO DE
					  Monnam(mtmp),
					  a_your[trap->madeby_u]);
				    seetrap(trap);
				}
				mtmp->mtrapped = tear_web ? 0 : 1;
				break;
			    /* this list is fairly arbitrary; it deliberately
			       excludes wumpus & giant/ettin zombies/mummies */
			    case PM_TITANOTHERE:
			    case PM_BALUCHITHERIUM:
			    case PM_PURPLE_WORM:
			    case PM_JABBERWOCK:
			    case PM_IRON_GOLEM:
			    case PM_BALROG:
			    case PM_KRAKEN:
			    case PM_MASTODON:
				tear_web = TRUE;
				break;
			}
			if (tear_web) {
			    if (in_sight)
				pline("%s tears through %s NOUN_SPIDER_WEB!", /* EN pline("%s tears through %s spider web!", */ // TODO DE
				      Monnam(mtmp), a_your[trap->madeby_u]);
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			}
			break;

		case STATUE_TRAP:
			break;

		case MAGIC_TRAP:
			/* A magic trap.  Monsters usually immune. */
			if (!rn2(21)) goto mfiretrap;
			break;
		case ANTI_MAGIC:
			break;

		case LANDMINE:
			if(rn2(3))
				break; /* monsters usually don't set it off */
			if(is_flyer(mptr)) {
				boolean already_seen = trap->tseen;
				if (in_sight && !already_seen) {
	pline("A trigger appears in a pile of soil below %s.", mon_nam(mtmp)); /* EN pline("A trigger appears in a pile of soil below %s.", mon_nam(mtmp)); */ // TODO DE
					seetrap(trap);
				}
				if (rn2(3)) break;
				if (in_sight) {
					newsym(mtmp->mx, mtmp->my);
					pline_The("air currents set %s off!", /* EN pline_The("air currents set %s off!", */ // TODO DE
					  already_seen ? "a land mine" : "it"); /* EN already_seen ? "a land mine" : "it"); */ // TODO DE
				}
			} else if(in_sight) {
			    newsym(mtmp->mx, mtmp->my);
			    pline("KAABLAMM!!!  %s triggers %s land mine!", /* EN pline("KAABLAMM!!!  %s triggers %s land mine!", */ // TODO DE
				Monnam(mtmp), a_your[trap->madeby_u]);
			}
			if (!in_sight)
				pline("Kaablamm!  You hear an explosion in the distance!"); /* EN pline("Kaablamm!  You hear an explosion in the distance!"); */ // TODO DE
			blow_up_landmine(trap);
			if (thitm(0, mtmp, (struct obj *)0, rnd(16), FALSE))
				trapkilled = TRUE;
			else {
				/* monsters recursively fall into new pit */
				if (mintrap(mtmp) == 2) trapkilled=TRUE;
			}
			/* a boulder may fill the new pit, crushing monster */
			fill_pit(trap->tx, trap->ty);
			if (mtmp->mhp <= 0) trapkilled = TRUE;
			if (unconscious()) {
				multi = -1;
				nomovemsg="SUBJECT ARTIKEL_BESTIMMTER NOUN_EXPLOSION VERB_WECKEN OBJECT PRONOMEN_PERSONAL auf!"; /* EN nomovemsg="The explosion awakens you!"; */
			}
			break;

		case POLY_TRAP:
		    if (resists_magm(mtmp)) {
			shieldeff(mtmp->mx, mtmp->my);
		    } else if (!resist(mtmp, WAND_CLASS, 0, NOTELL)) {
			(void) newcham(mtmp, (struct permonst *)0,
				       FALSE, FALSE);
			if (in_sight) seetrap(trap);
		    }
		    break;

		case ROLLING_BOULDER_TRAP:
		    if (!is_flyer(mptr)) {
			int style = ROLL | (in_sight ? 0 : LAUNCH_UNSEEN);

		        newsym(mtmp->mx,mtmp->my);
			if (in_sight)
			    pline("Klick! SUBJECT %s VERB_AUSLOESEN OBJECT %s SATZKLAMMER.", Monnam(mtmp), /* EN pline("Click! %s triggers %s.", Monnam(mtmp), */
				  trap->tseen ?
				  "ARTIKEL_UNBESTIMMTER NOUN_ROLLING_BOULDER_TRAP" : /* EN "a rolling boulder trap" : */
				  something);
			if (launch_obj(BOULDER, trap->launch.x, trap->launch.y,
				trap->launch2.x, trap->launch2.y, style)) {
			    if (in_sight) trap->tseen = TRUE;
			    if (mtmp->mhp <= 0) trapkilled = TRUE;
			} else {
			    deltrap(trap);
			    newsym(mtmp->mx,mtmp->my);
			}
		    }
		    break;

		default:
			impossible("Some monster encountered a strange trap of type %d.", tt);
	    }
	}
	if(trapkilled) return 2;
	return mtmp->mtrapped;
}

#endif /* OVL1 */
#ifdef OVLB

/* Combine cockatrice checks into single functions to avoid repeating code. */
void
instapetrify(str)
const char *str;
{
	if (Stone_resistance) return;
	if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
	    return;
	You("VERB_VERSTEINERN ..."); /* EN You("turn to stone..."); */
	killer_format = KILLED_BY;
	killer = str;
	done(STONING);
}

void
minstapetrify(mon,byplayer)
struct monst *mon;
boolean byplayer;
{
	if (resists_ston(mon)) return;
	if (poly_when_stoned(mon->data)) {
		mon_to_stone(mon);
		return;
	}

	/* give a "<mon> is slowing down" message and also remove
	   intrinsic speed (comparable to similar effect on the hero) */
	mon_adjust_speed(mon, -3, (struct obj *)0);

	if (cansee(mon->mx, mon->my))
		pline("SUBJECT %s VERB_VERSTEINERN.", Monnam(mon)); /* EN pline("%s turns to stone.", Monnam(mon)); */
	if (byplayer) {
		stoned = TRUE;
		xkilled(mon,0);
	} else monstone(mon);
}

void
selftouch(arg)
const char *arg;
{
	char kbuf[BUFSZ];

	if(uwep && uwep->otyp == CORPSE && touch_petrifies(&mons[uwep->corpsenm])
			&& !Stone_resistance) {
		pline("%s touch the %s corpse.", arg, /* EN pline("%s touch the %s corpse.", arg, */ // TODO DE
		        mons[uwep->corpsenm].mname);
		Sprintf(kbuf, "%s corpse", an(mons[uwep->corpsenm].mname)); /* EN Sprintf(kbuf, "%s corpse", an(mons[uwep->corpsenm].mname)); */ // TODO DE
		instapetrify(kbuf);
	}
	/* Or your secondary weapon, if wielded */
	if(u.twoweap && uswapwep && uswapwep->otyp == CORPSE &&
			touch_petrifies(&mons[uswapwep->corpsenm]) && !Stone_resistance){
		pline("%s touch the %s corpse.", arg, /* EN pline("%s touch the %s corpse.", arg, */ // TODO DE
		        mons[uswapwep->corpsenm].mname);
		Sprintf(kbuf, "%s corpse", an(mons[uswapwep->corpsenm].mname)); /* EN Sprintf(kbuf, "%s corpse", an(mons[uswapwep->corpsenm].mname)); */ // TODO DE
		instapetrify(kbuf);
	}
}

void
mselftouch(mon,arg,byplayer)
struct monst *mon;
const char *arg;
boolean byplayer;
{
	struct obj *mwep = MON_WEP(mon);

	if (mwep && mwep->otyp == CORPSE && touch_petrifies(&mons[mwep->corpsenm])) {
		if (cansee(mon->mx, mon->my)) {
			pline("%s%s touches the %s corpse.", /* EN pline("%s%s touches the %s corpse.", */ // TODO DE
			    arg ? arg : "", arg ? mon_nam(mon) : Monnam(mon),
			    mons[mwep->corpsenm].mname);
		}
		minstapetrify(mon, byplayer);
	}
}

void
float_up()
{
	if(u.utrap) {
		if(u.utraptype == TT_PIT) {
			u.utrap = 0;
			You("VERB_SCHWEBEN hoch, raus aus der Grube!"); /* EN You("float up, out of the pit!"); */
			vision_full_recalc = 1;	/* vision limits change */
			fill_pit(u.ux, u.uy);
		} else if (u.utraptype == TT_INFLOOR) {
			Your("body pulls upward, but your %s are still stuck.", /* EN Your("body pulls upward, but your %s are still stuck.", */ // TODO DE
			     makeplural(body_part(LEG)));
		} else {
			You("VERB_SCHWEBEN hoch, jedoch your %s is still stuck.", /* EN You("float up, only your %s is still stuck.", */ // TODO DE
				body_part(LEG));
		}
	}
	else if(Is_waterlevel(&u.uz))
		pline("It feels as though you've lost some weight."); /* EN pline("It feels as though you've lost some weight."); */ // TODO DE
	else if(u.uinwater)
		spoteffects(TRUE);
	else if(u.uswallow)
		You(is_animal(u.ustuck->data) ?
			"float away from the %s."  : /* EN "float away from the %s."  : */ // TODO DE
			"spiral up into %s.", /* EN "spiral up into %s.", */ // TODO DE
		    is_animal(u.ustuck->data) ?
			surface(u.ux, u.uy) :
			mon_nam(u.ustuck));
	else if (Hallucination)
		pline("Up, up, and awaaaay!  You're walking on air!"); /* EN pline("Up, up, and awaaaay!  You're walking on air!"); */ // TODO DE
	else if(Is_airlevel(&u.uz))
		You("gain control over your movements."); /* EN You("gain control over your movements."); */ // TODO DE
	else
		You("start to float in the air!"); /* EN You("start to float in the air!"); */ // TODO DE
#ifdef STEED
	if (u.usteed && !is_floater(u.usteed->data) &&
						!is_flyer(u.usteed->data)) {
	    if (Lev_at_will)
	    	pline("%s magically floats up!", Monnam(u.usteed)); /* EN pline("%s magically floats up!", Monnam(u.usteed)); */ // TODO DE
	    else {
	    	You("cannot stay on %s.", mon_nam(u.usteed)); /* EN You("cannot stay on %s.", mon_nam(u.usteed)); */ // TODO DE
	    	dismount_steed(DISMOUNT_GENERIC);
	    }
	}
#endif
	return;
}

void
fill_pit(x, y)
int x, y;
{
	struct obj *otmp;
	struct trap *t;

	if ((t = t_at(x, y)) &&
	    ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT)) &&
	    (otmp = sobj_at(BOULDER, x, y))) {
		obj_extract_self(otmp);
		(void) flooreffects(otmp, x, y, "settle"); /* EN (void) flooreffects(otmp, x, y, "settle"); */ // TODO DE
	}
}

int
float_down(hmask, emask)
long hmask, emask;     /* might cancel timeout */
{
	register struct trap *trap = (struct trap *)0;
	d_level current_dungeon_level;
	boolean no_msg = FALSE;

	HLevitation &= ~hmask;
	ELevitation &= ~emask;
	if(Levitation) return(0); /* maybe another ring/potion/boots */
	if(u.uswallow) {
	    You("float down, but you are still %s.", /* EN You("float down, but you are still %s.", */ // TODO DE
		is_animal(u.ustuck->data) ? "swallowed" : "engulfed"); /* EN is_animal(u.ustuck->data) ? "swallowed" : "engulfed"); */ // TODO DE
	    return(1);
	}

	if (Punished && !carried(uball) &&
	    (is_pool(uball->ox, uball->oy) ||
	     ((trap = t_at(uball->ox, uball->oy)) &&
	      ((trap->ttyp == PIT) || (trap->ttyp == SPIKED_PIT) ||
	       (trap->ttyp == TRAPDOOR) || (trap->ttyp == HOLE))))) {
			u.ux0 = u.ux;
			u.uy0 = u.uy;
			u.ux = uball->ox;
			u.uy = uball->oy;
			movobj(uchain, uball->ox, uball->oy);
			newsym(u.ux0, u.uy0);
			vision_full_recalc = 1;	/* in case the hero moved. */
	}
	/* check for falling into pool - added by GAN 10/20/86 */
	if(!Flying) {
		/* kludge alert:
		 * drown() and lava_effects() print various messages almost
		 * every time they're called which conflict with the "fall
		 * into" message below.  Thus, we want to avoid printing
		 * confusing, duplicate or out-of-order messages.
		 * Use knowledge of the two routines as a hack -- this
		 * should really be handled differently -dlc
		 */
		if(is_pool(u.ux,u.uy) && !Wwalking && !Swimming && !u.uinwater)
			no_msg = drown();

		if(is_lava(u.ux,u.uy)) {
			(void) lava_effects();
			no_msg = TRUE;
		}
	}
	if (!trap) {
	    trap = t_at(u.ux,u.uy);
	    if(Is_airlevel(&u.uz))
		You("begin to tumble in place."); /* EN You("begin to tumble in place."); */ // TODO DE
	    else if (Is_waterlevel(&u.uz) && !no_msg)
		Du_fuehlst_dich("schwerer."); /* EN You_feel("heavier."); */
	    /* u.uinwater msgs already in spoteffects()/drown() */
	    else if (!u.uinwater && !no_msg) {
#ifdef STEED
		if (!(emask & W_SADDLE))
#endif
		{
		    boolean sokoban_trap = (In_sokoban(&u.uz) && trap);
		    if (Hallucination)
			pline("Bummer!  You've %s.", /* EN pline("Bummer!  You've %s.", */ // TODO DE
			      is_pool(u.ux,u.uy) ?
			      "splashed down" : sokoban_trap ? "crashed" : /* EN "splashed down" : sokoban_trap ? "crashed" : */ // TODO DE
			      "hit the ground"); /* EN "hit the ground"); */ // TODO DE
		    else {
			if (!sokoban_trap)
			    You("float gently to the %s.", /* EN You("float gently to the %s.", */ // TODO DE
				surface(u.ux, u.uy));
			else {
			    /* Justification elsewhere for Sokoban traps
			     * is based on air currents. This is
			     * consistent with that.
			     * The unexpected additional force of the
			     * air currents once leviation
			     * ceases knocks you off your feet.
			     */
			    You("fall over."); /* EN You("fall over."); */ // TODO DE
			    losehp(rnd(2), "dangerous winds", KILLED_BY); /* EN losehp(rnd(2), "dangerous winds", KILLED_BY); */ // TODO DE
#ifdef STEED
			    if (u.usteed) dismount_steed(DISMOUNT_FELL);
#endif
			    selftouch("As you fall, you"); /* EN selftouch("As you fall, you"); */ // TODO DE
			}
		    }
		}
	    }
	}

	/* can't rely on u.uz0 for detecting trap door-induced level change;
	   it gets changed to reflect the new level before we can check it */
	assign_level(&current_dungeon_level, &u.uz);

	if(trap)
		switch(trap->ttyp) {
		case STATUE_TRAP:
			break;
		case HOLE:
		case TRAPDOOR:
			if(!Can_fall_thru(&u.uz) || u.ustuck)
				break;
			/* fall into next case */
		default:
			if (!u.utrap) /* not already in the trap */
				dotrap(trap, 0);
	}

	if (!Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz) && !u.uswallow &&
		/* falling through trap door calls goto_level,
		   and goto_level does its own pickup() call */
		on_level(&u.uz, &current_dungeon_level))
	    (void) pickup(1);
	return 1;
}

STATIC_OVL void
dofiretrap(box)
struct obj *box;	/* null for floor trap */
{
	boolean see_it = !Blind;
	int num, alt;

/* Bug: for box case, the equivalent of burn_floor_paper() ought
 * to be done upon its contents.
 */

	if ((box && !carried(box)) ? is_pool(box->ox, box->oy) : Underwater) {
	    pline("A cascade of steamy bubbles erupts from %s!", /* EN pline("A cascade of steamy bubbles erupts from %s!", */ // TODO DE
		    the(box ? xname(box) : surface(u.ux,u.uy)));
	    if (Fire_resistance) You("VERB_SEIN unversehrt."); /* EN if (Fire_resistance) You("are uninjured."); */
	    else losehp(rnd(3), "boiling water", KILLED_BY); /* EN else losehp(rnd(3), "boiling water", KILLED_BY); */ // TODO DE
	    return;
	}
	pline("A %s %s from %s!", tower_of_flame, /* EN pline("A %s %s from %s!", tower_of_flame, */ // TODO DE
	      box ? "bursts" : "erupts", /* EN box ? "bursts" : "erupts", */ // TODO DE
	      the(box ? xname(box) : surface(u.ux,u.uy)));
	if (Fire_resistance) {
	    shieldeff(u.ux, u.uy);
	    num = rn2(2);
	} else if (Upolyd) {
	    num = d(2,4);
	    switch (u.umonnum) {
	    case PM_PAPER_GOLEM:   alt = u.mhmax; break;
	    case PM_STRAW_GOLEM:   alt = u.mhmax / 2; break;
	    case PM_WOOD_GOLEM:    alt = u.mhmax / 4; break;
	    case PM_LEATHER_GOLEM: alt = u.mhmax / 8; break;
	    default: alt = 0; break;
	    }
	    if (alt > num) num = alt;
	    if (u.mhmax > mons[u.umonnum].mlevel)
		u.mhmax -= rn2(min(u.mhmax,num + 1)), flags.botl = 1;
	} else {
	    num = d(2,4);
	    if (u.uhpmax > u.ulevel)
		u.uhpmax -= rn2(min(u.uhpmax,num + 1)), flags.botl = 1;
	}
	if (!num)
	    You("VERB_SEIN unversehrt."); /* EN You("are uninjured."); */
	else
	    losehp(num, tower_of_flame, KILLED_BY_AN);
	burn_away_slime();

	if (burnarmor(&youmonst) || rn2(3)) {
	    destroy_item(SCROLL_CLASS, AD_FIRE);
	    destroy_item(SPBOOK_CLASS, AD_FIRE);
	    destroy_item(POTION_CLASS, AD_FIRE);
	}
	if (!box && burn_floor_paper(u.ux, u.uy, see_it, TRUE) && !see_it)
	    You("VERB_RIECHEN verbranntes Papier."); /* EN You("smell paper burning."); */
	if (is_ice(u.ux, u.uy))
	    melt_ice(u.ux, u.uy);
}

STATIC_OVL void
domagictrap()
{
	register int fate = rnd(20);

	/* What happened to the poor sucker? */

	if (fate < 10) {
	  /* Most of the time, it creates some monsters. */
	  register int cnt = rnd(4);

	  if (!resists_blnd(&youmonst)) {
		You("VERB_WERDEN kurz von einem Lichtblitz geblendet!"); /* EN You("are momentarily blinded by a flash of light!"); */
		make_blinded((long)rn1(5,10),FALSE);
		if (!Blind) Your(vision_clears);
	  } else if (!Blind) {
		You("VERB_SEHEN einen Lichtblitz!"); /* EN You("see a flash of light!"); */
	  }  else
		You_hear("ein ohrenbetäubendes Getöse!"); /* EN You_hear("a deafening roar!"); */
	  while(cnt--)
		(void) makemon((struct permonst *) 0, u.ux, u.uy, NO_MM_FLAGS);
	}
	else
	  switch (fate) {

	     case 10:
	     case 11:
		      /* sometimes nothing happens */
			break;
	     case 12: /* a flash of fire */
			dofiretrap((struct obj *)0);
			break;

	     /* odd feelings */
	     case 13:	pline("A shiver runs up and down your %s!", /* EN case 13:	pline("A shiver runs up and down your %s!", */ // TODO DE
			      body_part(SPINE)); /* EN body_part(SPINE)); */ // TODO DE
			break;
	     case 14:	You_hear(Hallucination ?
				"the moon howling at you." : /* EN "the moon howling at you." : */ // TODO DE
				"distant howling."); /* EN "distant howling."); */ // TODO DE
			break;
	     case 15:	if (on_level(&u.uz, &qstart_level))
			    You_feel("%slike the prodigal son.", /* EN You_feel("%slike the prodigal son.", */ // TODO DE
			      (flags.female || (Upolyd && is_neuter(youmonst.data))) ?
				     "oddly " : ""); /* EN "oddly " : ""); */ // TODO DE
			else
			    You("VERB_HABEN plötzlich Heimweh nach OBJECT KASUS_GENITIV %s.", /* EN You("suddenly yearn for %s.", */
				Hallucination ? "Cleveland" : /* EN Hallucination ? "Cleveland" : */ // TODO DE
			    (In_quest(&u.uz) || at_dgn_entrance("The Quest")) ?  /* EN (In_quest(&u.uz) || at_dgn_entrance("The Quest")) ? */ // TODO DE
						"PRONOMEN_POSSESSIV ADJEKTIV_NAHE NOUN_HEIMAT" : /* EN "your nearby homeland" : */
						"PRONOMEN_POSSESSIV ADJEKTIV_WEITENTFERNT NOUN_HEIMAT"); /* EN "your distant homeland"); */
			break;
	     case 16:   Your("pack shakes violently!"); /* EN case 16:   Your("pack shakes violently!"); */ // TODO DE
			break;
	     case 17:	You(Hallucination ?
				"VERB_SMELL Hamburger." : /* EN "smell hamburgers." : */
				"VERB_SMELL verkohltes Fleisch."); /* EN "smell charred flesh."); */
			break;
	     case 18:	Du_fuehlst_dich("müde."); /* EN case 18:	You_feel("tired."); */
			break;

	     /* very occasionally something nice happens. */

	     case 19:
		    /* tame nearby monsters */
		   {   register int i,j;
		       register struct monst *mtmp;

		       (void) adjattrib(A_CHA,1,FALSE);
		       for(i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
			   if(!isok(u.ux+i, u.uy+j)) continue;
			   mtmp = m_at(u.ux+i, u.uy+j);
			   if(mtmp)
			       (void) tamedog(mtmp, (struct obj *)0);
		       }
		       break;
		   }

	     case 20:
		    /* uncurse stuff */
		   {	struct obj pseudo;
			long save_conf = HConfusion;

			pseudo = zeroobj;   /* neither cursed nor blessed */
			pseudo.otyp = SCR_REMOVE_CURSE;
			HConfusion = 0L;
			(void) seffects(&pseudo);
			HConfusion = save_conf;
			break;
		   }
	     default: break;
	  }
}

/*
 * Scrolls, spellbooks, potions, and flammable items
 * may get affected by the fire.
 *
 * Return number of objects destroyed. --ALI
 */
int
fire_damage(chain, force, here, x, y)
struct obj *chain;
boolean force, here;
xchar x, y;
{
    int chance;
    struct obj *obj, *otmp, *nobj, *ncobj;
    int retval = 0;
    int in_sight = !Blind && couldsee(x, y);	/* Don't care if it's lit */
    int dindx;

    for (obj = chain; obj; obj = nobj) {
	nobj = here ? obj->nexthere : obj->nobj;

	/* object might light in a controlled manner */
	if (catch_lit(obj))
	    continue;

	if (Is_container(obj)) {
	    switch (obj->otyp) {
	    case ICE_BOX:
		continue;		/* Immune */
		/*NOTREACHED*/
		break;
	    case CHEST:
		chance = 40;
		break;
	    case LARGE_BOX:
		chance = 30;
		break;
	    default:
		chance = 20;
		break;
	    }
	    if (!force && (Luck + 5) > rn2(chance))
		continue;
	    /* Container is burnt up - dump contents out */
	    if (in_sight) pline("SUBJECT %s VERB_FANGEN Feuer und VERB_VERBRENNEN.", Yname2(obj)); /* EN if (in_sight) pline("%s catches fire and burns.", Yname2(obj)); */
	    if (Has_contents(obj)) {
		if (in_sight) pline("Der Inhalt KASUS_GENITIV %s fällt raus.", the(xname(obj))); /* EN if (in_sight) pline("Its contents fall out."); */
		for (otmp = obj->cobj; otmp; otmp = ncobj) {
		    ncobj = otmp->nobj;
		    obj_extract_self(otmp);
		    if (!flooreffects(otmp, x, y, ""))
			place_object(otmp, x, y);
		}
	    }
	    delobj(obj);
	    retval++;
	} else if (!force && (Luck + 5) > rn2(20)) {
	    /*  chance per item of sustaining damage:
	     *	max luck (full moon):	 5%
	     *	max luck (elsewhen):	10%
	     *	avg luck (Luck==0):	75%
	     *	awful luck (Luck<-4):  100%
	     */
	    continue;
	} else if (obj->oclass == SCROLL_CLASS || obj->oclass == SPBOOK_CLASS) {
	    if (obj->otyp == SCR_FIRE || obj->otyp == SPE_FIREBALL)
		continue;
	    if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
		if (in_sight) pline("Smoke rises from %s.", the(xname(obj))); /* EN if (in_sight) pline("Smoke rises from %s.", the(xname(obj))); */ // TODO DE
		continue;
	    }
	    dindx = (obj->oclass == SCROLL_CLASS) ? 2 : 3;
	    if (in_sight)
		pline("%s %s.", Yname2(obj), (obj->quan > 1) ?
		      destroy_strings[dindx*3 + 1] : destroy_strings[dindx*3]);
	    delobj(obj);
	    retval++;
	} else if (obj->oclass == POTION_CLASS) {
	    dindx = 1;
	    if (in_sight)
		pline("%s %s.", Yname2(obj), (obj->quan > 1) ?
		      destroy_strings[dindx*3 + 1] : destroy_strings[dindx*3]);
	    delobj(obj);
	    retval++;
	} else if (is_flammable(obj) && obj->oeroded < MAX_ERODE &&
		   !(obj->oerodeproof || (obj->blessed && !rnl(4)))) {
	    if (in_sight) {
		pline("SUBJECT %s %s%s.", Yname2(obj), otense(obj, "burn"), /* EN pline("%s %s%s.", Yname2(obj), otense(obj, "burn"), */ // TODO DE
		      obj->oeroded+1 == MAX_ERODE ? " completely" : /* EN obj->oeroded+1 == MAX_ERODE ? " completely" : */ // TODO DE
		      obj->oeroded ? " further" : ""); /* EN obj->oeroded ? " further" : ""); */ // TODO DE
	    }
	    obj->oeroded++;
	}
    }

    if (retval && !in_sight)
	You("VERB_SMELL Rauch."); /* EN You("smell smoke."); */
    return retval;
}

void
water_damage(obj, force, here)
register struct obj *obj;
register boolean force, here;
{
	struct obj *otmp;

	/* Scrolls, spellbooks, potions, weapons and
	   pieces of armor may get affected by the water */
	for (; obj; obj = otmp) {
		otmp = here ? obj->nexthere : obj->nobj;

		(void) snuff_lit(obj);

		if(obj->otyp == CAN_OF_GREASE && obj->spe > 0) {
			continue;
		} else if(obj->greased) {
			if (force || !rn2(2)) obj->greased = 0;
		} else if(Is_container(obj) && !Is_box(obj) &&
			(obj->otyp != OILSKIN_SACK || (obj->cursed && !rn2(3)))) {
			water_damage(obj->cobj, force, FALSE);
		} else if (!force && (Luck + 5) > rn2(20)) {
			/*  chance per item of sustaining damage:
			 *	max luck (full moon):	 5%
			 *	max luck (elsewhen):	10%
			 *	avg luck (Luck==0):	75%
			 *	awful luck (Luck<-4):  100%
			 */
			continue;
		} else if (obj->oclass == SCROLL_CLASS) {
#ifdef MAIL
		    if (obj->otyp != SCR_MAIL)
#endif
		    {
			obj->otyp = SCR_BLANK_PAPER;
			obj->spe = 0;
		    }
		} else if (obj->oclass == SPBOOK_CLASS) {
			if (obj->otyp == SPE_BOOK_OF_THE_DEAD)
				pline("Steam rises from %s.", the(xname(obj))); /* EN pline("Steam rises from %s.", the(xname(obj))); */ // TODO DE
			else obj->otyp = SPE_BLANK_PAPER;
		} else if (obj->oclass == POTION_CLASS) {
			if (obj->otyp == POT_ACID) {
				/* damage player/monster? */
				pline("SUBJECT ARTIKEL_UNBESTIMMTER NOUN_POTION VERB_EXPLODIEREN!"); /* EN pline("A potion explodes!"); */
				delobj(obj);
				continue;
			} else if (obj->odiluted) {
				obj->otyp = POT_WATER;
				obj->blessed = obj->cursed = 0;
				obj->odiluted = 0;
			} else if (obj->otyp != POT_WATER)
				obj->odiluted++;
		} else if (is_rustprone(obj) && obj->oeroded < MAX_ERODE &&
			  !(obj->oerodeproof || (obj->blessed && !rnl(4)))) {
			/* all metal stuff and armor except (body armor
			   protected by oilskin cloak) */
			if(obj->oclass != ARMOR_CLASS || obj != uarm ||
			   !uarmc || uarmc->otyp != OILSKIN_CLOAK ||
			   (uarmc->cursed && !rn2(3)))
				obj->oeroded++;
		}
	}
}

/*
 * This function is potentially expensive - rolling
 * inventory list multiple times.  Luckily it's seldom needed.
 * Returns TRUE if disrobing made player unencumbered enough to
 * crawl out of the current predicament.
 */
STATIC_OVL boolean
emergency_disrobe(lostsome)
boolean *lostsome;
{
	int invc = inv_cnt();

	while (near_capacity() > (Punished ? UNENCUMBERED : SLT_ENCUMBER)) {
	    register struct obj *obj, *otmp = (struct obj *)0;
	    register int i;

	    /* Pick a random object */
	    if (invc > 0) {
		i = rn2(invc);
		for (obj = invent; obj; obj = obj->nobj) {
		    /*
		     * Undroppables are: body armor, boots, gloves,
		     * amulets, and rings because of the time and effort
		     * in removing them + loadstone and other cursed stuff
		     * for obvious reasons.
		     */
		    if (!((obj->otyp == LOADSTONE && obj->cursed) ||
			  obj == uamul || obj == uleft || obj == uright ||
			  obj == ublindf || obj == uarm || obj == uarmc ||
			  obj == uarmg || obj == uarmf ||
#ifdef TOURIST
			  obj == uarmu ||
#endif
			  (obj->cursed && (obj == uarmh || obj == uarms)) ||
			  welded(obj)))
			otmp = obj;
		    /* reached the mark and found some stuff to drop? */
		    if (--i < 0 && otmp) break;

		    /* else continue */
		}
	    }
#ifndef GOLDOBJ
	    if (!otmp) {
		/* Nothing available left to drop; try gold */
		if (u.ugold) {
		    pline("Aus schierer Verzweiflung VERB_LASSEN SUBJECT PRONOMEN_PERSONAL OBJECT NOUN_PURSE fallen."); /* EN pline("In desperation, you drop your purse."); */
		    /* Hack: gold is not in the inventory, so make a gold object
		     * and put it at the head of the inventory list.
		     */
		    obj = mkgoldobj(u.ugold);    /* removes from u.ugold */
		    obj->in_use = TRUE;
		    u.ugold = obj->quan;         /* put the gold back */
		    assigninvlet(obj);           /* might end up as NOINVSYM */
		    obj->nobj = invent;
		    invent = obj;
		    *lostsome = TRUE;
		    dropx(obj);
		    continue;                    /* Try again */
		}
		/* We can't even drop gold! */
		return (FALSE);
	    }
#else
	    if (!otmp) return (FALSE); /* nothing to drop! */	
#endif
	    if (otmp->owornmask) remove_worn_item(otmp, FALSE);
	    *lostsome = TRUE;
	    dropx(otmp);
	    invc--;
	}
	return(TRUE);
}

/*
 *  return(TRUE) == player relocated
 */
boolean
drown()
{
	boolean inpool_ok = FALSE, crawl_ok;
	int i, x, y;

	/* happily wading in the same contiguous pool */
	if (u.uinwater && is_pool(u.ux-u.dx,u.uy-u.dy) &&
	    (Swimming || Amphibious)) {
		/* water effects on objects every now and then */
		if (!rn2(5)) inpool_ok = TRUE;
		else return(FALSE);
	}

	if (!u.uinwater) {
	    You("%s ins Wasser%c", /* EN You("%s into the water%c", */
		Is_waterlevel(&u.uz) ? "VERB_TAUCHEN" : "VERB_FALLEN", /* EN Is_waterlevel(&u.uz) ? "plunge" : "fall", */
		Amphibious || Swimming ? '.' : '!');
	    if (!Swimming && !Is_waterlevel(&u.uz))
		    You("VERB_SINKEN wie %s.", /* EN You("sink like %s.", */
			Hallucination ? "die Titanic" : "ein Stein"); /* EN Hallucination ? "the Titanic" : "a rock"); */
	}

	water_damage(invent, FALSE, FALSE);

	if (u.umonnum == PM_GREMLIN && rn2(3))
	    (void)split_mon(&youmonst, (struct monst *)0);
	else if (u.umonnum == PM_IRON_GOLEM) {
	    You("VERB_ROSTEN!"); /* EN You("rust!"); */
	    i = d(2,6);
	    if (u.mhmax > i) u.mhmax -= i;
	    losehp(i, "rusting away", KILLED_BY); /* EN losehp(i, "rusting away", KILLED_BY); */ // TODO DE
	}
	if (inpool_ok) return(FALSE);

	if ((i = number_leashed()) > 0) {
		pline_The("leash%s slip%s loose.", /* EN pline_The("leash%s slip%s loose.", */ // TODO DE
			(i > 1) ? "es" : "",
			(i > 1) ? "" : "s");
		unleash_all();
	}

	if (Amphibious || Swimming) {
		if (Amphibious) {
			if (flags.verbose)
				pline("Aber SUBJECT_IM_SATZ PRONOMEN_PERSONAL VERB_ERTRINKEN nicht."); /* EN pline("But you aren't drowning."); */
			if (!Is_waterlevel(&u.uz)) {
				if (Hallucination)
					Your("keel hits the bottom."); /* EN Your("keel hits the bottom."); */ // TODO DE
				else
					You("VERB_LAUFEN auf Grund."); /* EN You("touch bottom."); */
			}
		}
		if (Punished) {
			unplacebc();
			placebc();
		}
		vision_recalc(2);	/* unsee old position */
		u.uinwater = 1;
		under_water(1);
		vision_full_recalc = 1;
		return(FALSE);
	}
	if ((Teleportation || can_teleport(youmonst.data)) &&
		    !u.usleep && (Teleport_control || rn2(3) < Luck+2)) {
		You("VERB_VERSUCHEN einen Teleportationsspruch.");	/* utcsri!carroll */ /* EN You("attempt a teleport spell.");	*/
		(void) dotele();
		if(!is_pool(u.ux,u.uy))
			return(TRUE);
	}
#ifdef STEED
	if (u.usteed) {
		dismount_steed(DISMOUNT_GENERIC);
		if(!is_pool(u.ux,u.uy))
			return(TRUE);
	}
#endif
	crawl_ok = FALSE;
	x = y = 0;		/* lint suppression */
	/* if sleeping, wake up now so that we don't crawl out of water
	   while still asleep; we can't do that the same way that waking
	   due to combat is handled; note unmul() clears u.usleep */
	if (u.usleep) unmul("Plötzlich VERB_AUFWACHEN SUBJECT_IM_SATZ SATZKLAMMER!"); /* EN if (u.usleep) unmul("Suddenly you wake up!"); */
	/* can't crawl if unable to move (crawl_ok flag stays false) */
	if (multi < 0 || (Upolyd && !youmonst.data->mmove)) goto crawl;
	/* look around for a place to crawl to */
	for (i = 0; i < 100; i++) {
		x = rn1(3,u.ux - 1);
		y = rn1(3,u.uy - 1);
		if (goodpos(x, y, &youmonst, 0)) {
			crawl_ok = TRUE;
			goto crawl;
		}
	}
	/* one more scan */
	for (x = u.ux - 1; x <= u.ux + 1; x++)
		for (y = u.uy - 1; y <= u.uy + 1; y++)
			if (goodpos(x, y, &youmonst, 0)) {
				crawl_ok = TRUE;
				goto crawl;
			}
 crawl:
	if (crawl_ok) {
		boolean lost = FALSE;
		/* time to do some strip-tease... */
		boolean succ = Is_waterlevel(&u.uz) ? TRUE :
				emergency_disrobe(&lost);

		You("VERB_VERSUCHEN aus dem Wasser zu klettern."); /* EN You("try to crawl out of the water."); */
		if (lost)
			You("dump some of your gear to lose weight..."); /* EN You("dump some of your gear to lose weight..."); */ // TODO DE
		if (succ) {
			pline("Verdammt!  War das knapp."); /* EN pline("Pheew!  That was close."); */
			teleds(x,y,TRUE);
			return(TRUE);
		}
		/* still too much weight */
		pline("Umsonst."); /* EN pline("But in vain."); */
	}
	u.uinwater = 1;
	You("VERB_ERTRINKEN."); /* EN You("drown."); */ 
	killer_format = KILLED_BY_AN;
	killer = (levl[u.ux][u.uy].typ == POOL || Is_medusa_level(&u.uz)) ?
	    "pool of water" : "moat"; /* EN "pool of water" : "moat"; */ // TODO DE
	done(DROWNING);
	/* oops, we're still alive.  better get out of the water. */
	while (!safe_teleds(TRUE)) {
		pline("SUBJECT PRONOMEN_PERSONAL VERB_SEIN immer noch am Ertrinken."); /* EN pline("You're still drowning."); */
		done(DROWNING);
	}
	if (u.uinwater) {
	    u.uinwater = 0;
	    You("find yourself back %s.", Is_waterlevel(&u.uz) ?  /* EN You("find yourself back %s.", Is_waterlevel(&u.uz) ? */ // TODO DE
		"in an air bubble" : "on land"); /* EN "in an air bubble" : "on land"); */ // TODO DE
	}
	return(TRUE);
}

void
drain_en(n)
register int n;
{
	if (!u.uenmax) return;
	You_feel("your magical energy drain away!"); /* EN You_feel("your magical energy drain away!"); */ // TODO DE
	u.uen -= n;
	if(u.uen < 0)  {
		u.uenmax += u.uen;
		if(u.uenmax < 0) u.uenmax = 0;
		u.uen = 0;
	}
	flags.botl = 1;
}

int
dountrap()	/* disarm a trap */
{
	if (near_capacity() >= HVY_ENCUMBER) {
	    pline("You're too strained to do that."); /* EN pline("You're too strained to do that."); */ // TODO DE
	    return 0;
	}
	if ((nohands(youmonst.data) && !webmaker(youmonst.data)) || !youmonst.data->mmove) {
	    pline("And just how do you expect to do that?"); /* EN pline("And just how do you expect to do that?"); */ // TODO DE
	    return 0;
	} else if (u.ustuck && sticks(youmonst.data)) {
	    pline("You'll have to let go of %s first.", mon_nam(u.ustuck)); /* EN pline("You'll have to let go of %s first.", mon_nam(u.ustuck)); */ // TODO DE
	    return 0;
	}
	if (u.ustuck || (welded(uwep) && bimanual(uwep))) {
	    Your("%s seem to be too busy for that.", /* EN Your("%s seem to be too busy for that.", */ // TODO DE
		 makeplural(body_part(HAND)));
	    return 0;
	}
	return untrap(FALSE);
}
#endif /* OVLB */
#ifdef OVL2

/* Probability of disabling a trap.  Helge Hafting */
STATIC_OVL int
untrap_prob(ttmp)
struct trap *ttmp;
{
	int chance = 3;

	/* Only spiders know how to deal with webs reliably */
	if (ttmp->ttyp == WEB && !webmaker(youmonst.data))
	 	chance = 30;
	if (Confusion || Hallucination) chance++;
	if (Blind) chance++;
	if (Stunned) chance += 2;
	if (Fumbling) chance *= 2;
	/* Your own traps are better known than others. */
	if (ttmp && ttmp->madeby_u) chance--;
	if (Role_if(PM_ROGUE)) {
	    if (rn2(2 * MAXULEV) < u.ulevel) chance--;
	    if (u.uhave.questart && chance > 1) chance--;
	} else if (Role_if(PM_RANGER) && chance > 1) chance--;
	return rn2(chance);
}

/* Replace trap with object(s).  Helge Hafting */
STATIC_OVL void
cnv_trap_obj(otyp, cnt, ttmp)
int otyp;
int cnt;
struct trap *ttmp;
{
	struct obj *otmp = mksobj(otyp, TRUE, FALSE);
	otmp->quan=cnt;
	otmp->owt = weight(otmp);
	/* Only dart traps are capable of being poisonous */
	if (otyp != DART)
	    otmp->opoisoned = 0;
	place_object(otmp, ttmp->tx, ttmp->ty);
	/* Sell your own traps only... */
	if (ttmp->madeby_u) sellobj(otmp, ttmp->tx, ttmp->ty);
	stackobj(otmp);
	newsym(ttmp->tx, ttmp->ty);
	deltrap(ttmp);
}

/* while attempting to disarm an adjacent trap, we've fallen into it */
STATIC_OVL void
move_into_trap(ttmp)
struct trap *ttmp;
{
	int bc;
	xchar x = ttmp->tx, y = ttmp->ty, bx, by, cx, cy;
	boolean unused;

	/* we know there's no monster in the way, and we're not trapped */
	if (!Punished || drag_ball(x, y, &bc, &bx, &by, &cx, &cy, &unused,
		TRUE)) {
	    u.ux0 = u.ux,  u.uy0 = u.uy;
	    u.ux = x,  u.uy = y;
	    u.umoved = TRUE;
	    newsym(u.ux0, u.uy0);
	    vision_recalc(1);
	    check_leash(u.ux0, u.uy0);
	    if (Punished) move_bc(0, bc, bx, by, cx, cy);
	    spoteffects(FALSE);	/* dotrap() */
	    exercise(A_WIS, FALSE);
	}
}

/* 0: doesn't even try
 * 1: tries and fails
 * 2: succeeds
 */
STATIC_OVL int
try_disarm(ttmp, force_failure)
struct trap *ttmp;
boolean force_failure;
{
	struct monst *mtmp = m_at(ttmp->tx,ttmp->ty);
	int ttype = ttmp->ttyp;
	boolean under_u = (!u.dx && !u.dy);
	boolean holdingtrap = (ttype == BEAR_TRAP || ttype == WEB);
	
	/* Test for monster first, monsters are displayed instead of trap. */
	if (mtmp && (!mtmp->mtrapped || !holdingtrap)) {
		pline("SUBJECT %s VERB_STEHEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL im Weg.", Monnam(mtmp)); /* EN pline("%s is in the way.", Monnam(mtmp)); */
		return 0;
	}
	/* We might be forced to move onto the trap's location. */
	if (sobj_at(BOULDER, ttmp->tx, ttmp->ty)
				&& !Passes_walls && !under_u) {
		pline("SUBJECT NOUN_IT VERB_STEHEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL NEUES_OBJECT OBJECT KASUS_NOMINATIV ARTIKEL_UNBESTIMMTER NOUN_BOULDER im Weg."); /* EN There("is a boulder in your way."); */
		return 0;
	}
	/* duplicate tight-space checks from test_move */
	if (u.dx && u.dy &&
	    bad_rock(youmonst.data,u.ux,ttmp->ty) &&
	    bad_rock(youmonst.data,ttmp->tx,u.uy)) {
	    if ((invent && (inv_weight() + weight_cap() > 600)) ||
		bigmonst(youmonst.data)) {
		/* don't allow untrap if they can't get thru to it */
		You("are unable to reach the %s!", /* EN You("are unable to reach the %s!", */ // TODO DE
		    defsyms[trap_to_defsym(ttype)].explanation);
		return 0;
	    }
	}
	/* untrappable traps are located on the ground. */
	if (!can_reach_floor()) {
#ifdef STEED
		if (u.usteed && P_SKILL(P_RIDING) < P_BASIC)
			You("aren't skilled enough to reach from %s.", /* EN You("aren't skilled enough to reach from %s.", */ // TODO DE
				mon_nam(u.usteed));
		else
#endif
		You("are unable to reach the %s!", /* EN You("are unable to reach the %s!", */ // TODO DE
			defsyms[trap_to_defsym(ttype)].explanation);
		return 0;
	}

	/* Will our hero succeed? */
	if (force_failure || untrap_prob(ttmp)) {
		if (rnl(5)) {
		    pline("Huch ..."); /* EN pline("Whoops..."); */ // TODO DE
		    if (mtmp) {		/* must be a trap that holds monsters */
			if (ttype == BEAR_TRAP) {
			    if (mtmp->mtame) abuse_dog(mtmp);
			    if ((mtmp->mhp -= rnd(4)) <= 0) killed(mtmp);
			} else if (ttype == WEB) {
			    if (!webmaker(youmonst.data)) {
				struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB);
				if (ttmp2) {
				    pline_The("webbing sticks to you. You're caught too!"); /* EN pline_The("webbing sticks to you. You're caught too!"); */ // TODO DE
				    dotrap(ttmp2, NOWEBMSG);
#ifdef STEED
				    if (u.usteed && u.utrap) {
					/* you, not steed, are trapped */
					dismount_steed(DISMOUNT_FELL);
				    }
#endif
				}
			    } else
				pline("%s remains entangled.", Monnam(mtmp)); /* EN pline("%s remains entangled.", Monnam(mtmp)); */ // TODO DE
			}
		    } else if (under_u) {
			dotrap(ttmp, 0);
		    } else {
			move_into_trap(ttmp);
		    }
		} else {
		    pline("%s %s is difficult to %s.", /* EN pline("%s %s is difficult to %s.", */ // TODO DE
			  ttmp->madeby_u ? "Your" : under_u ? "This" : "That", /* EN ttmp->madeby_u ? "Your" : under_u ? "This" : "That", */ // TODO DE
			  defsyms[trap_to_defsym(ttype)].explanation,
			  (ttype == WEB) ? "remove" : "disarm"); /* EN (ttype == WEB) ? "remove" : "disarm"); */ // TODO DE
		}
		return 1;
	}
	return 2;
}

STATIC_OVL void
reward_untrap(ttmp, mtmp)
struct trap *ttmp;
struct monst *mtmp;
{
	if (!ttmp->madeby_u) {
	    if (rnl(10) < 8 && !mtmp->mpeaceful &&
		    !mtmp->msleeping && !mtmp->mfrozen &&
		    !mindless(mtmp->data) &&
		    mtmp->data->mlet != S_HUMAN) {
		mtmp->mpeaceful = 1;
		set_malign(mtmp);	/* reset alignment */
		pline("SUBJECT %s VERB_SEIN OBJECT KASUS_DATIV PRONOMEN_PERSONAL dankbar.", Monnam(mtmp)); /* EN pline("%s is grateful.", Monnam(mtmp)); */
	    }
	    /* Helping someone out of a trap is a nice thing to do,
	     * A lawful may be rewarded, but not too often.  */
	    if (!rn2(3) && !rnl(8) && u.ualign.type == A_LAWFUL) {
		adjalign(1);
		You("VERB_SPUEREN, dass PRONOMEN_PERSONAL genau das Richtige getan VERB_HABEN."); /* EN You_feel("that you did the right thing."); */
	    }
	}
}

STATIC_OVL int
disarm_holdingtrap(ttmp) /* Helge Hafting */
struct trap *ttmp;
{
	struct monst *mtmp;
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;

	/* ok, disarm it. */

	/* untrap the monster, if any.
	   There's no need for a cockatrice test, only the trap is touched */
	if ((mtmp = m_at(ttmp->tx,ttmp->ty)) != 0) {
		mtmp->mtrapped = 0;
		You("remove %s %s from %s.", the_your[ttmp->madeby_u], /* EN You("remove %s %s from %s.", the_your[ttmp->madeby_u], */ // TODO DE
			(ttmp->ttyp == BEAR_TRAP) ? "NOUN_BEAR_TRAP" : "NOUN_NETZ", /* EN (ttmp->ttyp == BEAR_TRAP) ? "bear trap" : "webbing", */
			mon_nam(mtmp));
		reward_untrap(ttmp, mtmp);
	} else {
		if (ttmp->ttyp == BEAR_TRAP) {
			You("VERB_DISARM OBJECT %s NOUN_BEARTRAP.", the_your[ttmp->madeby_u]); /* EN You("disarm %s bear trap.", the_your[ttmp->madeby_u]); */
			cnv_trap_obj(BEARTRAP, 1, ttmp);
		} else /* if (ttmp->ttyp == WEB) */ {
			You("succeed in removing %s web.", the_your[ttmp->madeby_u]); /* EN You("succeed in removing %s web.", the_your[ttmp->madeby_u]); */ // TODO DE
			deltrap(ttmp);
		}
	}
	newsym(u.ux + u.dx, u.uy + u.dy);
	return 1;
}

STATIC_OVL int
disarm_landmine(ttmp) /* Helge Hafting */
struct trap *ttmp;
{
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("VERB_DISARM OBJECT %s NOUN_LAND_MINE.", the_your[ttmp->madeby_u]); /* EN You("disarm %s land mine.", the_your[ttmp->madeby_u]); */
	cnv_trap_obj(LAND_MINE, 1, ttmp);
	return 1;
}

/* getobj will filter down to cans of grease and known potions of oil */
static NEARDATA const char oil[] = { ALL_CLASSES, TOOL_CLASS, POTION_CLASS, 0 };

/* it may not make much sense to use grease on floor boards, but so what? */
STATIC_OVL int
disarm_squeaky_board(ttmp)
struct trap *ttmp;
{
	struct obj *obj;
	boolean bad_tool;
	int fails;

	obj = getobj(oil, "untrap with"); /* EN obj = getobj(oil, "untrap with"); */ // TODO DE
	if (!obj) return 0;

	bad_tool = (obj->cursed ||
			((obj->otyp != POT_OIL || obj->lamplit) &&
			 (obj->otyp != CAN_OF_GREASE || !obj->spe)));

	fails = try_disarm(ttmp, bad_tool);
	if (fails < 2) return fails;

	/* successfully used oil or grease to fix squeaky board */
	if (obj->otyp == CAN_OF_GREASE) {
	    consume_obj_charge(obj, TRUE);
	} else {
	    useup(obj);	/* oil */
	    makeknown(POT_OIL);
	}
	You("repair the squeaky board.");	/* no madeby_u */ /* EN You("repair the squeaky board.");	*/ // TODO DE
	deltrap(ttmp);
	newsym(u.ux + u.dx, u.uy + u.dy);
	more_experienced(1, 5);
	newexplevel();
	return 1;
}

/* removes traps that shoot arrows, darts, etc. */
STATIC_OVL int
disarm_shooting_trap(ttmp, otyp)
struct trap *ttmp;
int otyp;
{
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
	You("VERB_DISARM OBJECT %s NOUN_TRAP.", the_your[ttmp->madeby_u]); /* EN You("disarm %s trap.", the_your[ttmp->madeby_u]); */
	cnv_trap_obj(otyp, 50-rnl(50), ttmp);
	return 1;
}

/* Is the weight too heavy?
 * Formula as in near_capacity() & check_capacity() */
STATIC_OVL int
try_lift(mtmp, ttmp, wt, stuff)
struct monst *mtmp;
struct trap *ttmp;
int wt;
boolean stuff;
{
	int wc = weight_cap();

	if (((wt * 2) / wc) >= HVY_ENCUMBER) {
	    pline("%s is %s for you to lift.", Monnam(mtmp), /* EN pline("%s is %s for you to lift.", Monnam(mtmp), */ // TODO DE
			stuff ? "carrying too much" : "too heavy"); /* EN stuff ? "carrying too much" : "too heavy"); */ // TODO DE
	    if (!ttmp->madeby_u && !mtmp->mpeaceful && mtmp->mcanmove &&
		    !mindless(mtmp->data) &&
		    mtmp->data->mlet != S_HUMAN && rnl(10) < 3) {
		mtmp->mpeaceful = 1;
		set_malign(mtmp);		/* reset alignment */
		pline("%s thinks it was nice of you to try.", Monnam(mtmp)); /* EN pline("%s thinks it was nice of you to try.", Monnam(mtmp)); */ // TODO DE
	    }
	    return 0;
	}
	return 1;
}

/* Help trapped monster (out of a (spiked) pit) */
STATIC_OVL int
help_monster_out(mtmp, ttmp)
struct monst *mtmp;
struct trap *ttmp;
{
	int wt;
	struct obj *otmp;
	boolean uprob;

	/*
	 * This works when levitating too -- consistent with the ability
	 * to hit monsters while levitating.
	 *
	 * Should perhaps check that our hero has arms/hands at the
	 * moment.  Helping can also be done by engulfing...
	 *
	 * Test the monster first - monsters are displayed before traps.
	 */
	if (!mtmp->mtrapped) {
		pline("%s isn't trapped.", Monnam(mtmp)); /* EN pline("%s isn't trapped.", Monnam(mtmp)); */ // TODO DE
		return 0;
	}
	/* Do you have the necessary capacity to lift anything? */
	if (check_capacity((char *)0)) return 1;

	/* Will our hero succeed? */
	if ((uprob = untrap_prob(ttmp)) && !mtmp->msleeping && mtmp->mcanmove) {
		You("try to reach out your %s, but %s backs away skeptically.", /* EN You("try to reach out your %s, but %s backs away skeptically.", */ // TODO DE
			makeplural(body_part(ARM)),
			mon_nam(mtmp));
		return 1;
	}


	/* is it a cockatrice?... */
	if (touch_petrifies(mtmp->data) && !uarmg && !Stone_resistance) {
		You("grab the trapped %s using your bare %s.", /* EN You("grab the trapped %s using your bare %s.", */ // TODO DE
				mtmp->data->mname, makeplural(body_part(HAND)));

		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			display_nhwindow(WIN_MESSAGE, FALSE);
		else {
			char kbuf[BUFSZ];

			Sprintf(kbuf, "trying to help %s out of a pit", /* EN Sprintf(kbuf, "trying to help %s out of a pit", */ // TODO DE
					an(mtmp->data->mname));
			instapetrify(kbuf);
			return 1;
		}
	}
	/* need to do cockatrice check first if sleeping or paralyzed */
	if (uprob) {
	    You("try to grab %s, but cannot get a firm grasp.", /* EN You("try to grab %s, but cannot get a firm grasp.", */ // TODO DE
		mon_nam(mtmp));
	    if (mtmp->msleeping) {
		mtmp->msleeping = 0;
		pline("SUBJECT %s VERB_WACHEN auf.", Monnam(mtmp)); /* EN pline("%s awakens.", Monnam(mtmp)); */
	    }
	    return 1;
	}

	You("reach out your %s and grab %s.", /* EN You("reach out your %s and grab %s.", */ // TODO DE
	    makeplural(body_part(ARM)), mon_nam(mtmp));

	if (mtmp->msleeping) {
	    mtmp->msleeping = 0;
	    pline("SUBJECT %s VERB_WACHEN auf.", Monnam(mtmp)); /* EN pline("%s awakens.", Monnam(mtmp)); */
	} else if (mtmp->mfrozen && !rn2(mtmp->mfrozen)) {
	    /* After such manhandling, perhaps the effect wears off */
	    mtmp->mcanmove = 1;
	    mtmp->mfrozen = 0;
	    pline("%s stirs.", Monnam(mtmp)); /* EN pline("%s stirs.", Monnam(mtmp)); */ // TODO DE
	}

	/* is the monster too heavy? */
	wt = inv_weight() + mtmp->data->cwt;
	if (!try_lift(mtmp, ttmp, wt, FALSE)) return 1;

	/* is the monster with inventory too heavy? */
	for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		wt += otmp->owt;
	if (!try_lift(mtmp, ttmp, wt, TRUE)) return 1;

	You("VERB_ZIEHEN OBJECT %s aus der Grube heraus.", mon_nam(mtmp)); /* EN You("pull %s out of the pit.", mon_nam(mtmp)); */
	mtmp->mtrapped = 0;
	fill_pit(mtmp->mx, mtmp->my);
	reward_untrap(ttmp, mtmp);
	return 1;
}

int
untrap(force)
boolean force;
{
	register struct obj *otmp;
	register boolean confused = (Confusion > 0 || Hallucination > 0);
	register int x,y;
	int ch;
	struct trap *ttmp;
	struct monst *mtmp;
	boolean trap_skipped = FALSE;
	boolean box_here = FALSE;
	boolean deal_with_floor_trap = FALSE;
	char the_trap[BUFSZ], qbuf[QBUFSZ];
	int containercnt = 0;

	if(!getdir((char *)0)) return(0);
	x = u.ux + u.dx;
	y = u.uy + u.dy;

	for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere) {
		if(Is_box(otmp) && !u.dx && !u.dy) {
			box_here = TRUE;
			containercnt++;
			if (containercnt > 1) break;
		}
	}

	if ((ttmp = t_at(x,y)) && ttmp->tseen) {
		deal_with_floor_trap = TRUE;
		Strcpy(the_trap, the(defsyms[trap_to_defsym(ttmp->ttyp)].explanation));
		if (box_here) {
			if (ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT) {
			    You_cant("do much about %s%s.", /* EN You_cant("do much about %s%s.", */ // TODO DE
					the_trap, u.utrap ?
					" that you're stuck in" : /* EN " that you're stuck in" : */ // TODO DE
					" while standing on the edge of it"); /* EN " while standing on the edge of it"); */ // TODO DE
			    trap_skipped = TRUE;
			    deal_with_floor_trap = FALSE;
			} else {
			    Sprintf(qbuf, "There %s and %s here. %s %s?", /* EN Sprintf(qbuf, "There %s and %s here. %s %s?", */ // TODO DE
				(containercnt == 1) ? "is a container" : "are containers", /* EN (containercnt == 1) ? "is a container" : "are containers", */ // TODO DE
				an(defsyms[trap_to_defsym(ttmp->ttyp)].explanation),
				ttmp->ttyp == WEB ? "Remove" : "Disarm", the_trap); /* EN ttmp->ttyp == WEB ? "Remove" : "Disarm", the_trap); */ // TODO DE
			    switch (ynq(qbuf)) {
				case 'q': return(0);
				case 'n': trap_skipped = TRUE;
					  deal_with_floor_trap = FALSE;
					  break;
			    }
			}
		}
		if (deal_with_floor_trap) {
		    if (u.utrap) {
			You("cannot deal with %s while trapped%s!", the_trap, /* EN You("cannot deal with %s while trapped%s!", the_trap, */ // TODO DE
				(x == u.ux && y == u.uy) ? " in it" : ""); /* EN (x == u.ux && y == u.uy) ? " in it" : ""); */ // TODO DE
			return 1;
		    }
		    switch(ttmp->ttyp) {
			case BEAR_TRAP:
			case WEB:
				return disarm_holdingtrap(ttmp);
			case LANDMINE:
				return disarm_landmine(ttmp);
			case SQKY_BOARD:
				return disarm_squeaky_board(ttmp);
			case DART_TRAP:
				return disarm_shooting_trap(ttmp, DART);
			case ARROW_TRAP:
				return disarm_shooting_trap(ttmp, ARROW);
			case PIT:
			case SPIKED_PIT:
				if (!u.dx && !u.dy) {
				    You("VERB_STEHEN schon am Grubenrand."); /* EN You("are already on the edge of the pit."); */
				    return 0;
				}
				if (!(mtmp = m_at(x,y))) {
				    pline("Try filling the pit instead."); /* EN pline("Try filling the pit instead."); */ // TODO DE
				    return 0;
				}
				return help_monster_out(mtmp, ttmp);
			default:
				You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this"); /* EN You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this"); */ // TODO DE
				return 0;
		    }
		}
	} /* end if */

	if(!u.dx && !u.dy) {
	    for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		if(Is_box(otmp)) {
		    Sprintf(qbuf, "Hier liegt %s. Auf Fallen überprüfen?", /* EN Sprintf(qbuf, "There is %s here. Check it for traps?", */
			safe_qbuf("", sizeof("Hier liegt  . Auf Fallen überprüfen?"), /* EN safe_qbuf("", sizeof("There is  here. Check it for traps?"), */
				doname(otmp), an(simple_typename(otmp->otyp)), "ein Behälter")); /* EN doname(otmp), an(simple_typename(otmp->otyp)), "a box")); */
		    switch (ynq(qbuf)) {
			case 'q': return(0);
			case 'n': continue;
		    }
#ifdef STEED
		    if (u.usteed && P_SKILL(P_RIDING) < P_BASIC) {
			You("aren't skilled enough to reach from %s.", /* EN You("aren't skilled enough to reach from %s.", */ // TODO DE
				mon_nam(u.usteed));
			return(0);
		    }
#endif
		    if((otmp->otrapped && (force || (!confused
				&& rn2(MAXULEV + 1 - u.ulevel) < 10)))
		       || (!force && confused && !rn2(3))) {
			You("find a trap on %s!", the(xname(otmp))); /* EN You("find a trap on %s!", the(xname(otmp))); */ // TODO DE
			if (!confused) exercise(A_WIS, TRUE);

			switch (ynq("Entschärfen?")) { /* EN switch (ynq("Disarm it?")) { */
			    case 'q': return(1);
			    case 'n': trap_skipped = TRUE;  continue;
			}

			if(otmp->otrapped) {
			    exercise(A_DEX, TRUE);
			    ch = ACURR(A_DEX) + u.ulevel;
			    if (Role_if(PM_ROGUE)) ch *= 2;
			    if(!force && (confused || Fumbling ||
				rnd(75+level_difficulty()/2) > ch)) {
				(void) chest_trap(otmp, FINGER, TRUE);
			    } else {
				You("VERB_DISARM sie!"); /* EN You("disarm it!"); */
				otmp->otrapped = 0;
			    }
			} else pline("That %s was not trapped.", xname(otmp)); /* EN } else pline("That %s was not trapped.", xname(otmp)); */ // TODO DE
			return(1);
		    } else {
			You("find no traps on %s.", the(xname(otmp))); /* EN You("find no traps on %s.", the(xname(otmp))); */ // TODO DE
			return(1);
		    }
		}

	    You(trap_skipped ? "find no other traps here." /* EN You(trap_skipped ? "find no other traps here." */ // TODO DE
			     : "know of no traps here."); /* EN : "know of no traps here."); */ // TODO DE
	    return(0);
	}

	if ((mtmp = m_at(x,y))				&&
		mtmp->m_ap_type == M_AP_FURNITURE	&&
		(mtmp->mappearance == S_hcdoor ||
			mtmp->mappearance == S_vcdoor)	&&
		!Protection_from_shape_changers)	 {

	    stumble_onto_mimic(mtmp);
	    return(1);
	}

	if (!IS_DOOR(levl[x][y].typ)) {
	    if ((ttmp = t_at(x,y)) && ttmp->tseen)
		You("cannot disable that trap."); /* EN You("cannot disable that trap."); */ // TODO DE
	    else
		You("know of no traps there."); /* EN You("know of no traps there."); */ // TODO DE
	    return(0);
	}

	switch (levl[x][y].doormask) {
	    case D_NODOOR:
		You("%s no door there.", Blind ? "feel" : "see"); /* EN You("%s no door there.", Blind ? "feel" : "see"); */ // TODO DE
		return(0);
	    case D_ISOPEN:
		pline("This door is safely open."); /* EN pline("This door is safely open."); */ // TODO DE
		return(0);
	    case D_BROKEN:
		pline("This door is broken."); /* EN pline("This door is broken."); */ // TODO DE
		return(0);
	}

	if ((levl[x][y].doormask & D_TRAPPED
	     && (force ||
		 (!confused && rn2(MAXULEV - u.ulevel + 11) < 10)))
	    || (!force && confused && !rn2(3))) {
		You("VERB_ENTDECKEN eine Falle an der Türe!"); /* EN You("find a trap on the door!"); */
		exercise(A_WIS, TRUE);
		if (ynq("Entschärfen?") != 'y') return(1); /* EN if (ynq("Disarm it?") != 'y') return(1); */
		if (levl[x][y].doormask & D_TRAPPED) {
		    ch = 15 + (Role_if(PM_ROGUE) ? u.ulevel*3 : u.ulevel);
		    exercise(A_DEX, TRUE);
		    if(!force && (confused || Fumbling ||
				     rnd(75+level_difficulty()/2) > ch)) {
			You("VERB_AUSLOESEN sie SATZKLAMMER!"); /* EN You("set it off!"); */
			b_trapped("NOUN_OBJ_DOOR", FINGER); /* EN b_trapped("door", FINGER); */
			levl[x][y].doormask = D_NODOOR;
			unblock_point(x, y);
			newsym(x, y);
			/* (probably ought to charge for this damage...) */
			if (*in_rooms(x, y, SHOPBASE)) add_damage(x, y, 0L);
		    } else {
			You("VERB_DISARM sie!"); /* EN You("disarm it!"); */
			levl[x][y].doormask &= ~D_TRAPPED;
		    }
		} else pline("This door was not trapped."); /* EN } else pline("This door was not trapped."); */ // TODO DE
		return(1);
	} else {
		You("find no traps on the door."); /* EN You("find no traps on the door."); */ // TODO DE
		return(1);
	}
}
#endif /* OVL2 */
#ifdef OVLB

/* only called when the player is doing something to the chest directly */
boolean
chest_trap(obj, bodypart, disarm)
register struct obj *obj;
register int bodypart;
boolean disarm;
{
	register struct obj *otmp = obj, *otmp2;
	char	buf[80];
	const char *msg;
	coord cc;

	if (get_obj_location(obj, &cc.x, &cc.y, 0))	/* might be carried */
	    obj->ox = cc.x,  obj->oy = cc.y;

	otmp->otrapped = 0;	/* trap is one-shot; clear flag first in case
				   chest kills you and ends up in bones file */
	You(disarm ? "set it off!" : "trigger a trap!"); /* EN You(disarm ? "set it off!" : "trigger a trap!"); */ // TODO DE
	display_nhwindow(WIN_MESSAGE, FALSE);
	if (Luck > -13 && rn2(13+Luck) > 7) {	/* saved by luck */
	    /* trap went off, but good luck prevents damage */
	    switch (rn2(13)) {
		case 12:
		case 11:  msg = "VERB_SEIN SUBJECT_IM_SATZ explosive charge is a dud";  break; /* EN case 11:  msg = "explosive charge is a dud";  break; */ // TODO DE
		case 10:
		case  9:  msg = "electric charge is grounded";  break; /* EN case  9:  msg = "electric charge is grounded";  break; */ // TODO DE
		case  8:
		case  7:  msg = "flame fizzles out";  break; /* EN case  7:  msg = "flame fizzles out";  break; */ // TODO DE
		case  6:
		case  5:
		case  4:  msg = "poisoned needle misses";  break; /* EN case  4:  msg = "poisoned needle misses";  break; */ // TODO DE
		case  3:
		case  2:
		case  1:
		case  0:  msg = "gas cloud blows away";  break; /* EN case  0:  msg = "gas cloud blows away";  break; */ // TODO DE
		default:  impossible("chest disarm bug");  msg = (char *)0;
			  break;
	    }
	    if (msg) pline("Aber zum Glück %s!", msg); /* EN if (msg) pline("But luckily the %s!", msg); */
	} else {
	    switch(rn2(20) ? ((Luck >= 13) ? 0 : rn2(13-Luck)) : rn2(26)) {
		case 25:
		case 24:
		case 23:
		case 22:
		case 21: {
			  struct monst *shkp = 0;
			  long loss = 0L;
			  boolean costly, insider;
			  register xchar ox = obj->ox, oy = obj->oy;

			  /* the obj location need not be that of player */
			  costly = (costly_spot(ox, oy) &&
				   (shkp = shop_keeper(*in_rooms(ox, oy,
				    SHOPBASE))) != (struct monst *)0);
			  insider = (*u.ushops && inside_shop(u.ux, u.uy) &&
				    *in_rooms(ox, oy, SHOPBASE) == *u.ushops);

			  pline("SUBJECT %s!", Tobjnam(obj, "VERB_EXPLODIEREN")); /* EN pline("%s!", Tobjnam(obj, "explode")); */
			  Sprintf(buf, "ADJEKTIV_EXPLODIEREND %s", xname(obj)); /* EN Sprintf(buf, "exploding %s", xname(obj)); */

			  if(costly)
			      loss += stolen_value(obj, ox, oy,
						(boolean)shkp->mpeaceful, TRUE);
			  delete_contents(obj);
			  /* we're about to delete all things at this location,
			   * which could include the ball & chain.
			   * If we attempt to call unpunish() in the
			   * for-loop below we can end up with otmp2
			   * being invalid once the chain is gone.
			   * Deal with ball & chain right now instead.
			   */
			  if (Punished && !carried(uball) &&
				((uchain->ox == u.ux && uchain->oy == u.uy) ||
				 (uball->ox == u.ux && uball->oy == u.uy)))
				unpunish();

			  for(otmp = level.objects[u.ux][u.uy];
							otmp; otmp = otmp2) {
			      otmp2 = otmp->nexthere;
			      if(costly)
				  loss += stolen_value(otmp, otmp->ox,
					  otmp->oy, (boolean)shkp->mpeaceful,
					  TRUE);
			      delobj(otmp);
			  }
			  wake_nearby();
			  losehp(d(6,6), buf, KILLED_BY_AN);
			  exercise(A_STR, FALSE);
			  if(costly && loss) {
			      if(insider)
			      You("owe %ld %s for objects destroyed.", /* EN You("owe %ld %s for objects destroyed.", */ // TODO DE
							loss, currency(loss));
			      else {
				  You("caused %ld %s worth of damage!", /* EN You("caused %ld %s worth of damage!", */ // TODO DE
							loss, currency(loss));
				  make_angry_shk(shkp, ox, oy);
			      }
			  }
			  return TRUE;
			}
		case 20:
		case 19:
		case 18:
		case 17:
			pline("A cloud of noxious gas billows from %s.", /* EN pline("A cloud of noxious gas billows from %s.", */ // TODO DE
							the(xname(obj)));
			poisoned("gas cloud", A_STR, "cloud of poison gas",15); /* EN poisoned("gas cloud", A_STR, "cloud of poison gas",15); */ // TODO DE
			exercise(A_CON, FALSE);
			break;
		case 16:
		case 15:
		case 14:
		case 13:
			You_feel("a needle prick your %s.",body_part(bodypart)); /* EN You_feel("a needle prick your %s.",body_part(bodypart)); */ // TODO DE
			poisoned("needle", A_CON, "poisoned needle",10); /* EN poisoned("needle", A_CON, "poisoned needle",10); */ // TODO DE
			exercise(A_CON, FALSE);
			break;
		case 12:
		case 11:
		case 10:
		case 9:
			dofiretrap(obj);
			break;
		case 8:
		case 7:
		case 6: {
			int dmg;

			You("are jolted by a surge of electricity!"); /* EN You("are jolted by a surge of electricity!"); */ // TODO DE
			if(Shock_resistance)  {
			    shieldeff(u.ux, u.uy);
			    You("don't seem to be affected."); /* EN You("don't seem to be affected."); */ // TODO DE
			    dmg = 0;
			} else
			    dmg = d(4, 4);
			destroy_item(RING_CLASS, AD_ELEC);
			destroy_item(WAND_CLASS, AD_ELEC);
			if (dmg) losehp(dmg, "electric shock", KILLED_BY_AN); /* EN if (dmg) losehp(dmg, "electric shock", KILLED_BY_AN); */ // TODO DE
			break;
		      }
		case 5:
		case 4:
		case 3:
			if (!Free_action) {                        
			pline("Suddenly you are frozen in place!"); /* EN pline("Suddenly you are frozen in place!"); */ // TODO DE
			nomul(-d(5, 6));
			exercise(A_DEX, FALSE);
			nomovemsg = You_can_move_again;
			} else You("momentarily stiffen."); /* EN } else You("momentarily stiffen."); */ // TODO DE
			break;
		case 2:
		case 1:
		case 0:
			pline("A cloud of %s gas billows from %s.", /* EN pline("A cloud of %s gas billows from %s.", */ // TODO DE
				Blind ? blindgas[rn2(SIZE(blindgas))] :
				rndcolor(), the(xname(obj)));
			if(!Stunned) {
			    if (Hallucination)
				pline("What a groovy feeling!"); /* EN pline("What a groovy feeling!"); */ // TODO DE
			    else if (Blind)
				You("%s and get dizzy...", /* EN You("%s and get dizzy...", */ // TODO DE
				    stagger(youmonst.data, "VERB_STAGGER")); /* EN stagger(youmonst.data, "stagger")); */
			    else
				You("%s and your vision blurs...", /* EN You("%s and your vision blurs...", */ // TODO DE
				    stagger(youmonst.data, "VERB_STAGGER")); /* EN stagger(youmonst.data, "stagger")); */
			}
			make_stunned(HStun + rn1(7, 16),FALSE);
			make_hallucinated(HHallucination + rn1(5, 16),FALSE,0L);
			break;
		default: impossible("bad chest trap");
			break;
	    }
	    bot();			/* to get immediate botl re-display */
	}

	return FALSE;
}

#endif /* OVLB */
#ifdef OVL0

struct trap *
t_at(x,y)
register int x, y;
{
	register struct trap *trap = ftrap;
	while(trap) {
		if(trap->tx == x && trap->ty == y) return(trap);
		trap = trap->ntrap;
	}
	return((struct trap *)0);
}

#endif /* OVL0 */
#ifdef OVLB

void
deltrap(trap)
register struct trap *trap;
{
	register struct trap *ttmp;

	if(trap == ftrap)
		ftrap = ftrap->ntrap;
	else {
		for(ttmp = ftrap; ttmp->ntrap != trap; ttmp = ttmp->ntrap) ;
		ttmp->ntrap = trap->ntrap;
	}
	dealloc_trap(trap);
}

boolean
delfloortrap(ttmp)
register struct trap *ttmp;
{
	/* Destroy a trap that emanates from the floor. */
	/* some of these are arbitrary -dlc */
	if (ttmp && ((ttmp->ttyp == SQKY_BOARD) ||
		     (ttmp->ttyp == BEAR_TRAP) ||
		     (ttmp->ttyp == LANDMINE) ||
		     (ttmp->ttyp == FIRE_TRAP) ||
		     (ttmp->ttyp == PIT) ||
		     (ttmp->ttyp == SPIKED_PIT) ||
		     (ttmp->ttyp == HOLE) ||
		     (ttmp->ttyp == TRAPDOOR) ||
		     (ttmp->ttyp == TELEP_TRAP) ||
		     (ttmp->ttyp == LEVEL_TELEP) ||
		     (ttmp->ttyp == WEB) ||
		     (ttmp->ttyp == MAGIC_TRAP) ||
		     (ttmp->ttyp == ANTI_MAGIC))) {
	    register struct monst *mtmp;

	    if (ttmp->tx == u.ux && ttmp->ty == u.uy) {
		u.utrap = 0;
		u.utraptype = 0;
	    } else if ((mtmp = m_at(ttmp->tx, ttmp->ty)) != 0) {
		mtmp->mtrapped = 0;
	    }
	    deltrap(ttmp);
	    return TRUE;
	} else
	    return FALSE;
}

/* used for doors (also tins).  can be used for anything else that opens. */
void
b_trapped(item, bodypart)
register const char *item;
register int bodypart;
{
	register int lvl = level_difficulty();
	int dmg = rnd(5 + (lvl < 5 ? lvl : 2+lvl/2));

	pline("KABUUUM!!  %s was booby-trapped!", The(item)); /* EN pline("KABOOM!!  %s was booby-trapped!", The(item)); */ // TODO DE
	wake_nearby();
	losehp(dmg, "explosion", KILLED_BY_AN); /* EN losehp(dmg, "explosion", KILLED_BY_AN); */ // TODO DE
	exercise(A_STR, FALSE);
	if (bodypart) exercise(A_CON, FALSE);
	make_stunned(HStun + dmg, TRUE);
}

/* Monster is hit by trap. */
/* Note: doesn't work if both obj and d_override are null */
STATIC_OVL boolean
thitm(tlev, mon, obj, d_override, nocorpse)
int tlev;
struct monst *mon;
struct obj *obj;
int d_override;
boolean nocorpse;
{
	int strike;
	boolean trapkilled = FALSE;

	if (d_override) strike = 1;
	else if (obj) strike = (find_mac(mon) + tlev + obj->spe <= rnd(20));
	else strike = (find_mac(mon) + tlev <= rnd(20));

	/* Actually more accurate than thitu, which doesn't take
	 * obj->spe into account.
	 */
	if(!strike) {
		if (obj && cansee(mon->mx, mon->my))
		    pline("%s is almost hit by %s!", Monnam(mon), doname(obj)); /* EN pline("%s is almost hit by %s!", Monnam(mon), doname(obj)); */ // TODO DE
	} else {
		int dam = 1;

		if (obj && cansee(mon->mx, mon->my))
			pline("%s is hit by %s!", Monnam(mon), doname(obj)); /* EN pline("%s is hit by %s!", Monnam(mon), doname(obj)); */ // TODO DE
		if (d_override) dam = d_override;
		else if (obj) {
			dam = dmgval(obj, mon);
			if (dam < 1) dam = 1;
		}
		if ((mon->mhp -= dam) <= 0) {
			int xx = mon->mx;
			int yy = mon->my;

			monkilled(mon, "", nocorpse ? -AD_RBRE : AD_PHYS);
			if (mon->mhp <= 0) {
				newsym(xx, yy);
				trapkilled = TRUE;
			}
		}
	}
	if (obj && (!strike || d_override)) {
		place_object(obj, mon->mx, mon->my);
		stackobj(obj);
	} else if (obj) dealloc_obj(obj);

	return trapkilled;
}

boolean
unconscious()
{
	return((boolean)(multi < 0 && (!nomovemsg ||
		u.usleep ||
		!strncmp(nomovemsg,"You regain con", 14) || /* EN !strncmp(nomovemsg,"You regain con", 14) || */ // TODO DE
		!strncmp(nomovemsg,"You are consci", 14)))); /* EN !strncmp(nomovemsg,"You are consci", 14)))); */ // TODO DE
}

static const char lava_killer[] = "ADJEKTIV_FLUESSIG NOUN_LAVA"; /* EN static const char lava_killer[] = "molten lava"; */ // TODO DE

boolean
lava_effects()
{
    register struct obj *obj, *obj2;
    int dmg;
    boolean usurvive;

    burn_away_slime();
    if (likes_lava(youmonst.data)) return FALSE;

    if (!Fire_resistance) {
	if(Wwalking) {
	    dmg = d(6,6);
	    pline_The("NOUN_LAVA VERB_VERBRENNEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL!"); /* EN pline_The("lava here burns you!"); */
	    if(dmg < u.uhp) {
		losehp(dmg, lava_killer, KILLED_BY);
		goto burn_stuff;
	    }
	} else
	    You("VERB_FALLEN in die Lava!"); /* EN You("fall into the lava!"); */

	usurvive = Lifesaved || discover;
#ifdef WIZARD
	if (wizard) usurvive = TRUE;
#endif
	for(obj = invent; obj; obj = obj2) {
	    obj2 = obj->nobj;
	    if(is_organic(obj) && !obj->oerodeproof) {
		if(obj->owornmask) {
		    if (usurvive)
			Your("%s into flame!", aobjnam(obj, "burst")); /* EN Your("%s into flame!", aobjnam(obj, "burst")); */ // TODO DE

		    if(obj == uarm) (void) Armor_gone();
		    else if(obj == uarmc) (void) Cloak_off();
		    else if(obj == uarmh) (void) Helmet_off();
		    else if(obj == uarms) (void) Shield_off();
		    else if(obj == uarmg) (void) Gloves_off();
		    else if(obj == uarmf) (void) Boots_off();
#ifdef TOURIST
		    else if(obj == uarmu) setnotworn(obj);
#endif
		    else if(obj == uleft) Ring_gone(obj);
		    else if(obj == uright) Ring_gone(obj);
		    else if(obj == ublindf) Blindf_off(obj);
		    else if(obj == uamul) Amulet_off();
		    else if(obj == uwep) uwepgone();
		    else if (obj == uquiver) uqwepgone();
		    else if (obj == uswapwep) uswapwepgone();
		}
		useupall(obj);
	    }
	}

	/* s/he died... */
	u.uhp = -1;
	killer_format = KILLED_BY;
	killer = lava_killer;
	You("burn to a crisp..."); /* EN You("burn to a crisp..."); */ // TODO DE
	done(BURNING);
	while (!safe_teleds(TRUE)) {
		pline("SUBJECT PRONOMEN_PERSONAL VERB_BRENNEN noch."); /* EN pline("You're still burning."); */
		done(BURNING);
	}
	You("VERB_SEIN wieder OBJECT KASUS_DATIV auf ADJEKTIV_FEST %s.", surface(u.ux, u.uy)); /* EN You("find yourself back on solid %s.", surface(u.ux, u.uy)); */
	return(TRUE);
    }

    if (!Wwalking) {
	u.utrap = rn1(4, 4) + (rn1(4, 12) << 8);
	u.utraptype = TT_LAVA;
	You("VERB_VERSINKEN in der Lava, aber es brennt nur ein bisschen!"); /* EN You("sink into the lava, but it only burns slightly!"); */
	if (u.uhp > 1)
	    losehp(1, lava_killer, KILLED_BY);
    }
    /* just want to burn boots, not all armor; destroy_item doesn't work on
       armor anyway */
burn_stuff:
    if(uarmf && !uarmf->oerodeproof && is_organic(uarmf)) {
	/* save uarmf value because Boots_off() sets uarmf to null */
	obj = uarmf;
	Your("%s bursts into flame!", xname(obj)); /* EN Your("%s bursts into flame!", xname(obj)); */ // TODO DE
	(void) Boots_off();
	useup(obj);
    }
    destroy_item(SCROLL_CLASS, AD_FIRE);
    destroy_item(SPBOOK_CLASS, AD_FIRE);
    destroy_item(POTION_CLASS, AD_FIRE);
    return(FALSE);
}

#endif /* OVLB */

/*trap.c*/
