/*	SCCS Id: @(#)potion.c	3.4	2002/10/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef OVLB
boolean notonhead = FALSE;

static NEARDATA int nothing, unkn;
static NEARDATA const char beverages[] = { POTION_CLASS, 0 };

STATIC_DCL long FDECL(itimeout, (long));
STATIC_DCL long FDECL(itimeout_incr, (long,int));
STATIC_DCL void NDECL(ghost_from_bottle);
STATIC_DCL short FDECL(mixtype, (struct obj *,struct obj *));

/* force `val' to be within valid range for intrinsic timeout value */
STATIC_OVL long
itimeout(val)
long val;
{
    if (val >= TIMEOUT) val = TIMEOUT;
    else if (val < 1) val = 0;

    return val;
}

/* increment `old' by `incr' and force result to be valid intrinsic timeout */
STATIC_OVL long
itimeout_incr(old, incr)
long old;
int incr;
{
    return itimeout((old & TIMEOUT) + (long)incr);
}

/* set the timeout field of intrinsic `which' */
void
set_itimeout(which, val)
long *which, val;
{
    *which &= ~TIMEOUT;
    *which |= itimeout(val);
}

/* increment the timeout field of intrinsic `which' */
void
incr_itimeout(which, incr)
long *which;
int incr;
{
    set_itimeout(which, itimeout_incr(*which, incr));
}

void
make_confused(xtime,talk)
long xtime;
boolean talk;
{
	long old = HConfusion;

	if (!xtime && old) {
		if (talk)
		    You_feel("less %s now.", /* EN You_feel("less %s now.", */ // TODO DE
			Hallucination ? "trippy" : "confused"); /* EN Hallucination ? "trippy" : "confused"); */ // TODO DE
	}
	if ((xtime && !old) || (!xtime && old)) flags.botl = TRUE;

	set_itimeout(&HConfusion, xtime);
}

void
make_stunned(xtime,talk)
long xtime;
boolean talk;
{
	long old = HStun;

	if (!xtime && old) {
		if (talk)
		    You_feel("%s now.", /* EN You_feel("%s now.", */ // TODO DE
			Hallucination ? "less wobbly" : "a bit steadier"); /* EN Hallucination ? "less wobbly" : "a bit steadier"); */ // TODO DE
	}
	if (xtime && !old) {
		if (talk) {
#ifdef STEED
			if (u.usteed)
				You("wobble in the saddle."); /* EN You("wobble in the saddle."); */ // TODO DE
			else
#endif
			You("%s...", stagger(youmonst.data, "stagger")); /* EN You("%s...", stagger(youmonst.data, "stagger")); */ // TODO DE
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HStun, xtime);
}

void
make_sick(xtime, cause, talk, type)
long xtime;
const char *cause;	/* sickness cause */
boolean talk;
int type;
{
	long old = Sick;

	if (xtime > 0L) {
	    if (Sick_resistance) return;
	    if (!old) {
		/* newly sick */
		You_feel("deathly sick."); /* EN You_feel("deathly sick."); */ // TODO DE
	    } else {
		/* already sick */
		if (talk) You_feel("%s worse.", /* EN if (talk) You_feel("%s worse.", */ // TODO DE
			      xtime <= Sick/2L ? "much" : "even"); /* EN xtime <= Sick/2L ? "much" : "even"); */ // TODO DE
	    }
	    set_itimeout(&Sick, xtime);
	    u.usick_type |= type;
	    flags.botl = TRUE;
	} else if (old && (type & u.usick_type)) {
	    /* was sick, now not */
	    u.usick_type &= ~type;
	    if (u.usick_type) { /* only partly cured */
		if (talk) You_feel("somewhat better."); /* EN if (talk) You_feel("somewhat better."); */ // TODO DE
		set_itimeout(&Sick, Sick * 2); /* approximation */
	    } else {
		if (talk) pline("What a relief!"); /* EN if (talk) pline("What a relief!"); */ // TODO DE
		Sick = 0L;		/* set_itimeout(&Sick, 0L) */
	    }
	    flags.botl = TRUE;
	}

	if (Sick) {
	    exercise(A_CON, FALSE);
	    if (cause) {
		(void) strncpy(u.usick_cause, cause, sizeof(u.usick_cause));
		u.usick_cause[sizeof(u.usick_cause)-1] = 0;
		}
	    else
		u.usick_cause[0] = 0;
	} else
	    u.usick_cause[0] = 0;
}

void
make_vomiting(xtime, talk)
long xtime;
boolean talk;
{
	long old = Vomiting;

	if(!xtime && old)
	    if(talk) You_feel("much less nauseated now."); /* EN if(talk) You_feel("much less nauseated now."); */ // TODO DE

	set_itimeout(&Vomiting, xtime);
}

static const char vismsg[] = "vision seems to %s for a moment but is %s now."; /* EN static const char vismsg[] = "vision seems to %s for a moment but is %s now."; */ // TODO DE
static const char eyemsg[] = "%s momentarily %s."; /* EN static const char eyemsg[] = "%s momentarily %s."; */ // TODO DE

void
make_blinded(xtime, talk)
long xtime;
boolean talk;
{
	long old = Blinded;
	boolean u_could_see, can_see_now;
	int eyecnt;
	char buf[BUFSZ];

	/* we need to probe ahead in case the Eyes of the Overworld
	   are or will be overriding blindness */
	u_could_see = !Blind;
	Blinded = xtime ? 1L : 0L;
	can_see_now = !Blind;
	Blinded = old;		/* restore */

	if (u.usleep) talk = FALSE;

	if (can_see_now && !u_could_see) {	/* regaining sight */
	    if (talk) {
		if (Hallucination)
		    pline("Far out!  Everything is all cosmic again!"); /* EN pline("Far out!  Everything is all cosmic again!"); */ // TODO DE
		else
		    You("VERB_CAN wieder sehen."); /* EN You("can see again."); */
	    }
	} else if (old && !xtime) {
	    /* clearing temporary blindness without toggling blindness */
	    if (talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blindfolded) {
		    Strcpy(buf, body_part(EYE));
		    eyecnt = eyecount(youmonst.data);
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "itches" : "itch"); /* EN (eyecnt == 1) ? "itches" : "itch"); */ // TODO DE
		} else {	/* Eyes of the Overworld */
		    Your(vismsg, "brighten", /* EN Your(vismsg, "brighten", */ // TODO DE
			 Hallucination ? "sadder" : "normal"); /* EN Hallucination ? "sadder" : "normal"); */ // TODO DE
		}
	    }
	}

	if (u_could_see && !can_see_now) {	/* losing sight */
	    if (talk) {
		if (Hallucination)
		    pline("Oh, bummer!  Everything is dark!  Help!"); /* EN pline("Oh, bummer!  Everything is dark!  Help!"); */ // TODO DE
		else
		    pline("A cloud of darkness falls upon you."); /* EN pline("A cloud of darkness falls upon you."); */ // TODO DE
	    }
	    /* Before the hero goes blind, set the ball&chain variables. */
	    if (Punished) set_bc(0);
	} else if (!old && xtime) {
	    /* setting temporary blindness without toggling blindness */
	    if (talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blindfolded) {
		    Strcpy(buf, body_part(EYE));
		    eyecnt = eyecount(youmonst.data);
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "twitches" : "twitch"); /* EN (eyecnt == 1) ? "twitches" : "twitch"); */ // TODO DE
		} else {	/* Eyes of the Overworld */
		    Your(vismsg, "dim", /* EN Your(vismsg, "dim", */ // TODO DE
			 Hallucination ? "happier" : "normal"); /* EN Hallucination ? "happier" : "normal"); */ // TODO DE
		}
	    }
	}

	set_itimeout(&Blinded, xtime);

	if (u_could_see ^ can_see_now) {  /* one or the other but not both */
	    flags.botl = 1;
	    vision_full_recalc = 1;	/* blindness just got toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	}
}

void
make_hallucinated(xtime, talk, mask)
long xtime;	/* nonzero if this is an attempt to turn on hallucination */
boolean talk;
long mask;	/* nonzero if resistance status should change by mask */
{
	long old = HHallucination;
	boolean changed = 0;
	const char *message, *verb;

	message = (!xtime) ? "Everything %s SO boring now." : /* EN message = (!xtime) ? "Everything %s SO boring now." : */ // TODO DE
			     "Oh wow!  Everything %s so cosmic!"; /* EN "Oh wow!  Everything %s so cosmic!"; */ // TODO DE
	verb = (!Blind) ? "looks" : "feels"; /* EN verb = (!Blind) ? "looks" : "feels"; */ // TODO DE

	if (mask) {
	    if (HHallucination) changed = TRUE;

	    if (!xtime) EHalluc_resistance |= mask;
	    else EHalluc_resistance &= ~mask;
	} else {
	    if (!EHalluc_resistance && (!!HHallucination != !!xtime))
		changed = TRUE;
	    set_itimeout(&HHallucination, xtime);

	    /* clearing temporary hallucination without toggling vision */
	    if (!changed && !HHallucination && old && talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blind) {
		    char buf[BUFSZ];
		    int eyecnt = eyecount(youmonst.data);

		    Strcpy(buf, body_part(EYE));
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "itches" : "itch"); /* EN (eyecnt == 1) ? "itches" : "itch"); */ // TODO DE
		} else {	/* Grayswandir */
		    Your(vismsg, "flatten", "normal"); /* EN Your(vismsg, "flatten", "normal"); */ // TODO DE
		}
	    }
	}

	if (changed) {
	    if (u.uswallow) {
		swallowed(0);	/* redraw swallow display */
	    } else {
		/* The see_* routines should be called *before* the pline. */
		see_monsters();
		see_objects();
		see_traps();
	    }

	    /* for perm_inv and anything similar
	    (eg. Qt windowport's equipped items display) */
	    update_inventory();

	    flags.botl = 1;
	    if (talk) pline(message, verb);
	}
}

STATIC_OVL void
ghost_from_bottle()
{
	struct monst *mtmp = makemon(&mons[PM_GHOST], u.ux, u.uy, NO_MM_FLAGS);

	if (!mtmp) {
		pline("This bottle turns out to be empty."); /* EN pline("This bottle turns out to be empty."); */ // TODO DE
		return;
	}
	if (Blind) {
		pline("As you open the bottle, %s emerges.", something); /* EN pline("As you open the bottle, %s emerges.", something); */ // TODO DE
		return;
	}
	pline("As you open the bottle, an enormous %s emerges!", /* EN pline("As you open the bottle, an enormous %s emerges!", */ // TODO DE
		Hallucination ? rndmonnam() : (const char *)"ghost"); /* EN Hallucination ? rndmonnam() : (const char *)"ghost"); */ // TODO DE
	if(flags.verbose)
	    You("are frightened to death, and unable to move."); /* EN You("are frightened to death, and unable to move."); */ // TODO DE
	nomul(-3);
	nomovemsg = "You regain your composure."; /* EN nomovemsg = "You regain your composure."; */ // TODO DE
}

/* "Quaffing is like drinking, except you spill more."  -- Terry Pratchett
 */
int
dodrink()
{
	register struct obj *otmp;
	const char *potion_descr;

	if (Strangled) {
		pline("If you can't breathe air, how can you drink liquid?"); /* EN pline("If you can't breathe air, how can you drink liquid?"); */ // TODO DE
		return 0;
	}
	/* Is there a fountain to drink from here? */
	if (IS_FOUNTAIN(levl[u.ux][u.uy].typ) && !Levitation) {
		if(yn("Drink from the fountain?") == 'y') { /* EN if(yn("Drink from the fountain?") == 'y') { */ // TODO DE
			drinkfountain();
			return 1;
		}
	}
#ifdef SINKS
	/* Or a kitchen sink? */
	if (IS_SINK(levl[u.ux][u.uy].typ)) {
		if (yn("Drink from the sink?") == 'y') { /* EN if (yn("Drink from the sink?") == 'y') { */ // TODO DE
			drinksink();
			return 1;
		}
	}
#endif

	/* Or are you surrounded by water? */
	if (Underwater) {
		if (yn("Drink the water around you?") == 'y') { /* EN if (yn("Drink the water around you?") == 'y') { */ // TODO DE
// TODO DE Tiere paaren sich darin
		    pline("Do you know what lives in this water!"); /* EN pline("Do you know what lives in this water!");  */ // TODO DE
			return 1;
		}
	}

	otmp = getobj(beverages, "drink"); /* EN otmp = getobj(beverages, "drink"); */ // TODO DE
	if(!otmp) return(0);
	otmp->in_use = TRUE;		/* you've opened the stopper */

#define POTION_OCCUPANT_CHANCE(n) (13 + 2*(n))	/* also in muse.c */

	potion_descr = OBJ_DESCR(objects[otmp->otyp]);
	if (potion_descr) {
	    if (!strcmp(potion_descr, "milky") && /* EN if (!strcmp(potion_descr, "milky") && */ // TODO DE
		    flags.ghost_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.ghost_count))) {
		ghost_from_bottle();
		useup(otmp);
		return(1);
	    } else if (!strcmp(potion_descr, "smoky") && /* EN } else if (!strcmp(potion_descr, "smoky") && */ // TODO DE
		    flags.djinni_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.djinni_count))) {
		djinni_from_bottle(otmp);
		useup(otmp);
		return(1);
	    }
	}
	return dopotion(otmp);
}

int
dopotion(otmp)
register struct obj *otmp;
{
	int retval;

	otmp->in_use = TRUE;
	nothing = unkn = 0;
	if((retval = peffects(otmp)) >= 0) return(retval);

	if(nothing) {
	    unkn++;
	    You("have a %s feeling for a moment, then it passes.", /* EN You("have a %s feeling for a moment, then it passes.", */ // TODO DE
		  Hallucination ? "normal" : "peculiar"); /* EN Hallucination ? "normal" : "peculiar"); */ // TODO DE
	}
	if(otmp->dknown && !objects[otmp->otyp].oc_name_known) {
		if(!unkn) {
			makeknown(otmp->otyp);
			more_experienced(0,10);
		} else if(!objects[otmp->otyp].oc_uname)
			docall(otmp);
	}
	useup(otmp);
	return(1);
}

int
peffects(otmp)
	register struct obj	*otmp;
{
	register int i, ii, lim;

	switch(otmp->otyp){
	case POT_RESTORE_ABILITY:
	case SPE_RESTORE_ABILITY:
		unkn++;
		if(otmp->cursed) {
		    pline("Ulch!  This makes you feel mediocre!"); /* EN pline("Ulch!  This makes you feel mediocre!"); */ // TODO DE
		    break;
		} else {
		    pline("Wow!  This makes you feel %s!", /* EN pline("Wow!  This makes you feel %s!", */ // TODO DE
			  (otmp->blessed) ?
				(unfixable_trouble_count(FALSE) ? "better" : "great") /* EN (unfixable_trouble_count(FALSE) ? "better" : "great") */ // TODO DE
			  : "good"); /* EN : "good"); */ // TODO DE
		    i = rn2(A_MAX);		/* start at a random point */
		    for (ii = 0; ii < A_MAX; ii++) {
			lim = AMAX(i);
			if (i == A_STR && u.uhs >= 3) --lim;	/* WEAK */
			if (ABASE(i) < lim) {
			    ABASE(i) = lim;
			    flags.botl = 1;
			    /* only first found if not blessed */
			    if (!otmp->blessed) break;
			}
			if(++i >= A_MAX) i = 0;
		    }
		}
		break;
	case POT_HALLUCINATION:
		if (Hallucination || Halluc_resistance) nothing++;
		make_hallucinated(itimeout_incr(HHallucination,
					   rn1(200, 600 - 300 * bcsign(otmp))),
				  TRUE, 0L);
		break;
	case POT_WATER:
		if(!otmp->blessed && !otmp->cursed) {
		    pline("This tastes like water."); /* EN pline("This tastes like water."); */ // TODO DE
		    u.uhunger += rnd(10);
		    newuhs(FALSE);
		    break;
		}
		unkn++;
		if(is_undead(youmonst.data) || is_demon(youmonst.data) ||
				u.ualign.type == A_CHAOTIC) {
		    if(otmp->blessed) {
			pline("This burns like acid!"); /* EN pline("This burns like acid!"); */ // TODO DE
			exercise(A_CON, FALSE);
			if (u.ulycn >= LOW_PM) {
			    Your("affinity to %s disappears!", /* EN Your("affinity to %s disappears!", */ // TODO DE
				 makeplural(mons[u.ulycn].mname));
			    if (youmonst.data == &mons[u.ulycn])
				you_unwere(FALSE);
			    u.ulycn = NON_PM;	/* cure lycanthropy */
			}
			losehp(d(2,6), "potion of holy water", KILLED_BY_AN); /* EN losehp(d(2,6), "potion of holy water", KILLED_BY_AN); */ // TODO DE
		    } else if(otmp->cursed) {
			You_feel("quite proud of yourself."); /* EN You_feel("quite proud of yourself."); */ // TODO DE
			healup(d(2,6),0,0,0);
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, TRUE);
		    }
		} else {
		    if(otmp->blessed) {
			You_feel("full of awe."); /* EN You_feel("full of awe."); */ // TODO DE
			make_sick(0L, (char *) 0, TRUE, SICK_ALL);
			exercise(A_WIS, TRUE);
			exercise(A_CON, TRUE);
			if (u.ulycn >= LOW_PM)
			    you_unwere(TRUE);	/* "Purified" */
			/* make_confused(0L,TRUE); */
		    } else {
			if(u.ualign.type == A_LAWFUL) {
			    pline("This burns like acid!"); /* EN pline("This burns like acid!"); */ // TODO DE
			    losehp(d(2,6), "potion of unholy water", /* EN losehp(d(2,6), "potion of unholy water", */ // TODO DE
				KILLED_BY_AN);
			} else
			    You_feel("full of dread."); /* EN You_feel("full of dread."); */ // TODO DE
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, FALSE);
		    }
		}
		break;
	case POT_BOOZE:
		unkn++;
		pline("Ooph!  This tastes like %s%s!", /* EN pline("Ooph!  This tastes like %s%s!", */ // TODO DE
		      otmp->odiluted ? "watered down " : "", /* EN otmp->odiluted ? "watered down " : "", */ // TODO DE
		      Hallucination ? "dandelion wine" : "liquid fire"); /* EN Hallucination ? "dandelion wine" : "liquid fire"); */ // TODO DE
		if (!otmp->blessed)
		    make_confused(itimeout_incr(HConfusion, d(3,8)), FALSE);
		/* the whiskey makes us feel better */
		if (!otmp->odiluted) healup(1, 0, FALSE, FALSE);
		u.uhunger += 10 * (2 + bcsign(otmp));
		newuhs(FALSE);
		exercise(A_WIS, FALSE);
		if(otmp->cursed) {
			You("pass out."); /* EN You("pass out."); */ // TODO DE
			multi = -rnd(15);
			nomovemsg = "You awake with a headache."; /* EN nomovemsg = "You awake with a headache."; */ // TODO DE
		}
		break;
	case POT_ENLIGHTENMENT:
		if(otmp->cursed) {
			unkn++;
			You("have an uneasy feeling..."); /* EN You("have an uneasy feeling..."); */ // TODO DE
			exercise(A_WIS, FALSE);
		} else {
			if (otmp->blessed) {
				(void) adjattrib(A_INT, 1, FALSE);
				(void) adjattrib(A_WIS, 1, FALSE);
			}
			You_feel("self-knowledgeable..."); /* EN You_feel("self-knowledgeable..."); */ // TODO DE
			display_nhwindow(WIN_MESSAGE, FALSE);
			enlightenment(0);
			pline_The("feeling subsides."); /* EN pline_The("feeling subsides."); */ // TODO DE
			exercise(A_WIS, TRUE);
		}
		break;
	case SPE_INVISIBILITY:
		/* spell cannot penetrate mummy wrapping */
		if (BInvis && uarmc->otyp == MUMMY_WRAPPING) {
			You_feel("rather itchy under your %s.", xname(uarmc)); /* EN You_feel("rather itchy under your %s.", xname(uarmc)); */ // TODO DE
			break;
		}
		/* FALLTHRU */
	case POT_INVISIBILITY:
		if (Invis || Blind || BInvis) {
		    nothing++;
		} else {
		    self_invis_message();
		}
		if (otmp->blessed) HInvis |= FROMOUTSIDE;
		else incr_itimeout(&HInvis, rn1(15,31));
		newsym(u.ux,u.uy);	/* update position */
		if(otmp->cursed) {
		    pline("For some reason, you feel your presence is known."); /* EN pline("For some reason, you feel your presence is known."); */ // TODO DE
		    aggravate();
		}
		break;
	case POT_SEE_INVISIBLE:
		/* tastes like fruit juice in Rogue */
	case POT_FRUIT_JUICE:
	    {
		int msg = Invisible && !Blind;

		unkn++;
		if (otmp->cursed)
		    pline("Yecch!  This tastes %s.", /* EN pline("Yecch!  This tastes %s.", */ // TODO DE
			  Hallucination ? "overripe" : "rotten"); /* EN Hallucination ? "overripe" : "rotten"); */ // TODO DE
		else
		    pline(Hallucination ?
		      "This tastes like 10%% real %s%s all-natural beverage." : /* EN "This tastes like 10%% real %s%s all-natural beverage." : */ // TODO DE
				"This tastes like %s%s.", /* EN "This tastes like %s%s.", */ // TODO DE
			  otmp->odiluted ? "reconstituted " : "", /* EN otmp->odiluted ? "reconstituted " : "", */ // TODO DE
			  fruitname(TRUE));
		if (otmp->otyp == POT_FRUIT_JUICE) {
		    u.uhunger += (otmp->odiluted ? 5 : 10) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		    break;
		}
		if (!otmp->cursed) {
			/* Tell them they can see again immediately, which
			 * will help them identify the potion...
			 */
			make_blinded(0L,TRUE);
		}
		if (otmp->blessed)
			HSee_invisible |= FROMOUTSIDE;
		else
			incr_itimeout(&HSee_invisible, rn1(100,750));
		set_mimic_blocking(); /* do special mimic handling */
		see_monsters();	/* see invisible monsters */
		newsym(u.ux,u.uy); /* see yourself! */
		if (msg && !Blind) { /* Blind possible if polymorphed */
		    You("can see through yourself, but you are visible!"); /* EN You("can see through yourself, but you are visible!"); */ // TODO DE
		    unkn--;
		}
		break;
	    }
	case POT_PARALYSIS:
		if (Free_action)
		    You("stiffen momentarily."); /* EN You("stiffen momentarily."); */ // TODO DE
		else {
		    if (Levitation || Is_airlevel(&u.uz)||Is_waterlevel(&u.uz))
			You("are motionlessly suspended."); /* EN You("are motionlessly suspended."); */ // TODO DE
#ifdef STEED
		    else if (u.usteed)
			You("are frozen in place!"); /* EN You("are frozen in place!"); */ // TODO DE
#endif
		    else
			Your("%s are frozen to the %s!", /* EN Your("%s are frozen to the %s!", */ // TODO DE
			     makeplural(body_part(FOOT)), surface(u.ux, u.uy));
		    nomul(-(rn1(10, 25 - 12*bcsign(otmp))));
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		}
		break;
	case POT_SLEEPING:
		if(Sleep_resistance || Free_action)
		    You("yawn."); /* EN You("yawn."); */ // TODO DE
		else {
		    You("suddenly fall asleep!"); /* EN You("suddenly fall asleep!"); */ // TODO DE
		    fall_asleep(-rn1(10, 25 - 12*bcsign(otmp)), TRUE);
		}
		break;
	case POT_MONSTER_DETECTION:
	case SPE_DETECT_MONSTERS:
		if (otmp->blessed) {
		    int x, y;

		    if (Detect_monsters) nothing++;
		    unkn++;
		    /* after a while, repeated uses become less effective */
		    if (HDetect_monsters >= 300L)
			i = 1;
		    else
			i = rn1(40,21);
		    incr_itimeout(&HDetect_monsters, i);
		    for (x = 1; x < COLNO; x++) {
			for (y = 0; y < ROWNO; y++) {
			    if (levl[x][y].glyph == GLYPH_INVISIBLE) {
				unmap_object(x, y);
				newsym(x,y);
			    }
			    if (MON_AT(x,y)) unkn = 0;
			}
		    }
		    see_monsters();
		    if (unkn) You_feel("lonely.");
		    break;
		}
		if (monster_detect(otmp, 0))
			return(1);		/* nothing detected */
		exercise(A_WIS, TRUE);
		break;
	case POT_OBJECT_DETECTION:
	case SPE_DETECT_TREASURE:
		if (object_detect(otmp, 0))
			return(1);		/* nothing detected */
		exercise(A_WIS, TRUE);
		break;
	case POT_SICKNESS:
		pline("Yecch!  This stuff tastes like poison."); /* EN pline("Yecch!  This stuff tastes like poison."); */ // TODO DE
		if (otmp->blessed) {
		    pline("(But in fact it was mildly stale %s.)", /* EN pline("(But in fact it was mildly stale %s.)", */ // TODO DE
			  fruitname(TRUE));
		    if (!Role_if(PM_HEALER)) {
			if (otmp->corpsenm)
			    losehp(1,
				   "mildly contaminated tap water", KILLED_BY); /* EN "mildly contaminated tap water", KILLED_BY); */ // TODO DE
			else
			    losehp(1,
				   "mildly contaminated potion", KILLED_BY_AN); /* EN "mildly contaminated potion", KILLED_BY_AN); */ // TODO DE
		    }
		} else {
		    if(Poison_resistance)
			pline(
			  "(But in fact it was biologically contaminated %s.)", /* EN "(But in fact it was biologically contaminated %s.)", */ // TODO DE
			      fruitname(TRUE));
		    if (Role_if(PM_HEALER))
			pline("Fortunately, you have been immunized."); /* EN pline("Fortunately, you have been immunized."); */ // TODO DE
		    else {
			int typ = rn2(A_MAX);

			if (!Fixed_abil) {
			    poisontell(typ);
			    (void) adjattrib(typ,
			    		Poison_resistance ? -1 : -rn1(4,3),
			    		TRUE);
			}
			if(!Poison_resistance) {
			    if (otmp->corpsenm)
				losehp(rnd(10)+5*!!(otmp->cursed),
				       "contaminated tap water", KILLED_BY); /* EN "contaminated tap water", KILLED_BY); */ // TODO DE
			    else
				losehp(rnd(10)+5*!!(otmp->cursed),
				       "contaminated potion", KILLED_BY_AN); /* EN "contaminated potion", KILLED_BY_AN); */ // TODO DE
			}
			exercise(A_CON, FALSE);
		    }
		}
		if(Hallucination) {
			You("are shocked back to your senses!"); /* EN You("are shocked back to your senses!"); */ // TODO DE
			make_hallucinated(0L,FALSE,0L);
		}
		break;
	case POT_CONFUSION:
		if(!Confusion)
		    if (Hallucination) {
			pline("What a trippy feeling!"); /* EN pline("What a trippy feeling!"); */ // TODO DE
			unkn++;
		    } else
			pline("Huh, What?  Where am I?"); /* EN pline("Huh, What?  Where am I?"); */ // TODO DE
		else	nothing++;
		make_confused(itimeout_incr(HConfusion,
					    rn1(7, 16 - 8 * bcsign(otmp))),
			      FALSE);
		break;
	case POT_GAIN_ABILITY:
		if(otmp->cursed) {
		    pline("Ulch!  That potion tasted foul!"); /* EN pline("Ulch!  That potion tasted foul!"); */ // TODO DE
		    unkn++;
		} else if (Fixed_abil) {
		    nothing++;
		} else {      /* If blessed, increase all; if not, try up to */
		    int itmp; /* 6 times to find one which can be increased. */
		    i = -1;		/* increment to 0 */
		    for (ii = A_MAX; ii > 0; ii--) {
			i = (otmp->blessed ? i + 1 : rn2(A_MAX));
			/* only give "your X is already as high as it can get"
			   message on last attempt (except blessed potions) */
			itmp = (otmp->blessed || ii == 1) ? 0 : -1;
			if (adjattrib(i, 1, itmp) && !otmp->blessed)
			    break;
		    }
		}
		break;
	case POT_SPEED:
		if(Wounded_legs && !otmp->cursed
#ifdef STEED
		   && !u.usteed	/* heal_legs() would heal steeds legs */
#endif
						) {
			heal_legs();
			unkn++;
			break;
		} /* and fall through */
	case SPE_HASTE_SELF:
		if(!Very_fast) /* wwf@doe.carleton.ca */
			You("are suddenly moving %sfaster.", /* EN You("are suddenly moving %sfaster.", */ // TODO DE
				Fast ? "" : "much "); /* EN Fast ? "" : "much "); */ // TODO DE
		else {
			Your("%s get new energy.", /* EN Your("%s get new energy.", */ // TODO DE
				makeplural(body_part(LEG)));
			unkn++;
		}
		exercise(A_DEX, TRUE);
		incr_itimeout(&HFast, rn1(10, 100 + 60 * bcsign(otmp)));
		break;
	case POT_BLINDNESS:
		if(Blind) nothing++;
		make_blinded(itimeout_incr(Blinded,
					   rn1(200, 250 - 125 * bcsign(otmp))),
			     (boolean)!Blind);
		break;
	case POT_GAIN_LEVEL:
		if (otmp->cursed) {
			unkn++;
			/* they went up a level */
			if((ledger_no(&u.uz) == 1 && u.uhave.amulet) ||
				Can_rise_up(u.ux, u.uy, &u.uz)) {
			    const char *riseup ="rise up, through the %s!"; /* EN const char *riseup ="rise up, through the %s!"; */ // TODO DE
			    if(ledger_no(&u.uz) == 1) {
			        You(riseup, ceiling(u.ux,u.uy));
				goto_level(&earth_level, FALSE, FALSE, FALSE);
			    } else {
			        register int newlev = depth(&u.uz)-1;
				d_level newlevel;

				get_level(&newlevel, newlev);
				if(on_level(&newlevel, &u.uz)) {
				    pline("It tasted bad."); /* EN pline("It tasted bad."); */ // TODO DE
				    break;
				} else You(riseup, ceiling(u.ux,u.uy));
				goto_level(&newlevel, FALSE, FALSE, FALSE);
			    }
			}
			else You("have an uneasy feeling."); /* EN else You("have an uneasy feeling."); */ // TODO DE
			break;
		}
		pluslvl(FALSE);
		if (otmp->blessed)
			/* blessed potions place you at a random spot in the
			 * middle of the new level instead of the low point
			 */
			u.uexp = rndexp(TRUE);
		break;
	case POT_HEALING:
		You_feel("better."); /* EN You_feel("better."); */ // TODO DE
		healup(d(6 + 2 * bcsign(otmp), 4),
		       !otmp->cursed ? 1 : 0, !!otmp->blessed, !otmp->cursed);
		exercise(A_CON, TRUE);
		break;
	case POT_EXTRA_HEALING:
		You_feel("much better."); /* EN You_feel("much better."); */ // TODO DE
		healup(d(6 + 2 * bcsign(otmp), 8),
		       otmp->blessed ? 5 : !otmp->cursed ? 2 : 0,
		       !otmp->cursed, TRUE);
		make_hallucinated(0L,TRUE,0L);
		exercise(A_CON, TRUE);
		exercise(A_STR, TRUE);
		break;
	case POT_FULL_HEALING:
		You_feel("completely healed."); /* EN You_feel("completely healed."); */ // TODO DE
		healup(400, 4+4*bcsign(otmp), !otmp->cursed, TRUE);
		/* Restore one lost level if blessed */
		if (otmp->blessed && u.ulevel < u.ulevelmax) {
		    /* when multiple levels have been lost, drinking
		       multiple potions will only get half of them back */
		    u.ulevelmax -= 1;
		    pluslvl(FALSE);
		}
		make_hallucinated(0L,TRUE,0L);
		exercise(A_STR, TRUE);
		exercise(A_CON, TRUE);
		break;
	case POT_LEVITATION:
	case SPE_LEVITATION:
		if (otmp->cursed) HLevitation &= ~I_SPECIAL;
		if(!Levitation) {
			/* kludge to ensure proper operation of float_up() */
			HLevitation = 1;
			float_up();
			/* reverse kludge */
			HLevitation = 0;
			if (otmp->cursed && !Is_waterlevel(&u.uz)) {
	if((u.ux != xupstair || u.uy != yupstair)
	   && (u.ux != sstairs.sx || u.uy != sstairs.sy || !sstairs.up)
	   && (!xupladder || u.ux != xupladder || u.uy != yupladder)
	) {
					You("hit your %s on the %s.", /* EN You("hit your %s on the %s.", */ // TODO DE
						body_part(HEAD),
						ceiling(u.ux,u.uy));
					losehp(uarmh ? 1 : rnd(10),
						"colliding with the ceiling", /* EN "colliding with the ceiling", */ // TODO DE
						KILLED_BY);
				} else (void) doup();
			}
		} else
			nothing++;
		if (otmp->blessed) {
		    incr_itimeout(&HLevitation, rn1(50,250));
		    HLevitation |= I_SPECIAL;
		} else incr_itimeout(&HLevitation, rn1(140,10));
		spoteffects(FALSE);	/* for sinks */
		break;
	case POT_GAIN_ENERGY:			/* M. Stephenson */
		{	register int num;
			if(otmp->cursed)
			    You_feel("lackluster."); /* EN You_feel("lackluster."); */ // TODO DE
			else
			    pline("Magical energies course through your body."); /* EN pline("Magical energies course through your body."); */ // TODO DE
			num = rnd(5) + 5 * otmp->blessed + 1;
			u.uenmax += (otmp->cursed) ? -num : num;
			u.uen += (otmp->cursed) ? -num : num;
			if(u.uenmax <= 0) u.uenmax = 0;
			if(u.uen <= 0) u.uen = 0;
			flags.botl = 1;
			exercise(A_WIS, TRUE);
		}
		break;
	case POT_OIL:				/* P. Winner */
		{
			boolean good_for_you = FALSE;

			if (otmp->lamplit) {
			    if (likes_fire(youmonst.data)) {
				pline("Ahh, a refreshing drink."); /* EN pline("Ahh, a refreshing drink."); */ // TODO DE
				good_for_you = TRUE;
			    } else {
				You("burn your %s.", body_part(FACE)); /* EN You("burn your %s.", body_part(FACE)); */ // TODO DE
				losehp(d(Fire_resistance ? 1 : 3, 4),
				       "burning potion of oil", KILLED_BY_AN); /* EN "burning potion of oil", KILLED_BY_AN); */ // TODO DE
			    }
			} else if(otmp->cursed)
			    pline("This tastes like castor oil."); /* EN pline("This tastes like castor oil."); */ // TODO DE
			else
			    pline("That was smooth!"); /* EN pline("That was smooth!"); */ // TODO DE
			exercise(A_WIS, good_for_you);
		}
		break;
	case POT_ACID:
		if (Acid_resistance)
			/* Not necessarily a creature who _likes_ acid */
			pline("This tastes %s.", Hallucination ? "tangy" : "sour"); /* EN pline("This tastes %s.", Hallucination ? "tangy" : "sour"); */ // TODO DE
		else {
			pline("This burns%s!", otmp->blessed ? " a little" : /* EN pline("This burns%s!", otmp->blessed ? " a little" : */ // TODO DE
					otmp->cursed ? " a lot" : " like acid"); /* EN otmp->cursed ? " a lot" : " like acid"); */ // TODO DE
			losehp(d(otmp->cursed ? 2 : 1, otmp->blessed ? 4 : 8),
					"potion of acid", KILLED_BY_AN); /* EN "potion of acid", KILLED_BY_AN); */ // TODO DE
			exercise(A_CON, FALSE);
		}
		if (Stoned) fix_petrification();
		unkn++; /* holy/unholy water can burn like acid too */
		break;
	case POT_POLYMORPH:
		You_feel("a little %s.", Hallucination ? "normal" : "strange"); /* EN You_feel("a little %s.", Hallucination ? "normal" : "strange"); */ // TODO DE
		if (!Unchanging) polyself(FALSE);
		break;
	default:
		impossible("What a funny potion! (%u)", otmp->otyp);
		return(0);
	}
	return(-1);
}

void
healup(nhp, nxtra, curesick, cureblind)
	int nhp, nxtra;
	register boolean curesick, cureblind;
{
	if (nhp) {
		if (Upolyd) {
			u.mh += nhp;
			if (u.mh > u.mhmax) u.mh = (u.mhmax += nxtra);
		} else {
			u.uhp += nhp;
			if(u.uhp > u.uhpmax) u.uhp = (u.uhpmax += nxtra);
		}
	}
	if(cureblind)	make_blinded(0L,TRUE);
	if(curesick)	make_sick(0L, (char *) 0, TRUE, SICK_ALL);
	flags.botl = 1;
	return;
}

void
strange_feeling(obj,txt)
register struct obj *obj;
register const char *txt;
{
	if (flags.beginner || !txt)
		You("have a %s feeling for a moment, then it passes.", /* EN You("have a %s feeling for a moment, then it passes.", */ // TODO DE
		Hallucination ? "normal" : "strange"); /* EN Hallucination ? "normal" : "strange"); */ // TODO DE
	else
		pline(txt);

	if(!obj)	/* e.g., crystal ball finds no traps */
		return;

	if(obj->dknown && !objects[obj->otyp].oc_name_known &&
						!objects[obj->otyp].oc_uname)
		docall(obj);
	useup(obj);
}

const char *bottlenames[] = {
	"bottle", "phial", "flagon", "carafe", "flask", "jar", "vial" /* EN "bottle", "phial", "flagon", "carafe", "flask", "jar", "vial" */ // TODO DE
};


const char *
bottlename()
{
	return bottlenames[rn2(SIZE(bottlenames))];
}

void
potionhit(mon, obj, your_fault)
register struct monst *mon;
register struct obj *obj;
boolean your_fault;
{
	register const char *botlnam = bottlename();
	boolean isyou = (mon == &youmonst);
	int distance;

	if(isyou) {
		distance = 0;
		pline_The("%s crashes on your %s and breaks into shards.", /* EN pline_The("%s crashes on your %s and breaks into shards.", */ // TODO DE
			botlnam, body_part(HEAD));
		losehp(rnd(2), "thrown potion", KILLED_BY_AN); /* EN losehp(rnd(2), "thrown potion", KILLED_BY_AN); */ // TODO DE
	} else {
		distance = distu(mon->mx,mon->my);
		if (!cansee(mon->mx,mon->my)) pline("Crash!"); /* EN if (!cansee(mon->mx,mon->my)) pline("Crash!"); */ // TODO DE
		else {
		    char *mnam = mon_nam(mon);
		    char buf[BUFSZ];

		    if(has_head(mon->data)) {
			Sprintf(buf, "%s %s",
				s_suffix(mnam),
				(notonhead ? "body" : "head")); /* EN (notonhead ? "body" : "head")); */ // TODO DE
		    } else {
			Strcpy(buf, mnam);
		    }
		    pline_The("%s crashes on %s and breaks into shards.", /* EN pline_The("%s crashes on %s and breaks into shards.", */ // TODO DE
			   botlnam, buf);
		}
		if(rn2(5) && mon->mhp > 1)
			mon->mhp--;
	}

	/* oil doesn't instantly evaporate */
	if (obj->otyp != POT_OIL && cansee(mon->mx,mon->my))
		pline("%s.", Tobjnam(obj, "evaporate")); /* EN pline("%s.", Tobjnam(obj, "evaporate")); */ // TODO DE

    if (isyou) {
	switch (obj->otyp) {
	case POT_OIL:
		if (obj->lamplit)
		    splatter_burning_oil(u.ux, u.uy);
		break;
	case POT_POLYMORPH:
		You_feel("a little %s.", Hallucination ? "normal" : "strange"); /* EN You_feel("a little %s.", Hallucination ? "normal" : "strange"); */ // TODO DE
		if (!Unchanging && !Antimagic) polyself(FALSE);
		break;
	case POT_ACID:
		if (!Acid_resistance) {
		    pline("This burns%s!", obj->blessed ? " a little" : /* EN pline("This burns%s!", obj->blessed ? " a little" : */ // TODO DE
				    obj->cursed ? " a lot" : ""); /* EN obj->cursed ? " a lot" : ""); */ // TODO DE
		    losehp(d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8),
				    "potion of acid", KILLED_BY_AN); /* EN "potion of acid", KILLED_BY_AN); */ // TODO DE
		}
		break;
	}
    } else {
	boolean angermon = TRUE;

	if (!your_fault) angermon = FALSE;
	switch (obj->otyp) {
	case POT_HEALING:
	case POT_EXTRA_HEALING:
	case POT_FULL_HEALING:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		/*FALLTHRU*/
	case POT_RESTORE_ABILITY:
	case POT_GAIN_ABILITY:
 do_healing:
		angermon = FALSE;
		if(mon->mhp < mon->mhpmax) {
		    mon->mhp = mon->mhpmax;
		    if (canseemon(mon))
			pline("%s looks sound and hale again.", Monnam(mon)); /* EN pline("%s looks sound and hale again.", Monnam(mon)); */ // TODO DE
		}
		break;
	case POT_SICKNESS:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_healing;
		if (dmgtype(mon->data, AD_DISE) ||
			   dmgtype(mon->data, AD_PEST) || /* won't happen, see prior goto */
			   resists_poison(mon)) {
		    if (canseemon(mon))
			pline("%s looks unharmed.", Monnam(mon)); /* EN pline("%s looks unharmed.", Monnam(mon)); */ // TODO DE
		    break;
		}
 do_illness:
		if((mon->mhpmax > 3) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhpmax /= 2;
		if((mon->mhp > 2) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhp /= 2;
		if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
		if (canseemon(mon))
		    pline("%s looks rather ill.", Monnam(mon)); /* EN pline("%s looks rather ill.", Monnam(mon)); */ // TODO DE
		break;
	case POT_CONFUSION:
	case POT_BOOZE:
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  mon->mconf = TRUE;
		break;
	case POT_INVISIBILITY:
		angermon = FALSE;
		mon_set_minvis(mon);
		break;
	case POT_SLEEPING:
		/* wakeup() doesn't rouse victims of temporary sleep */
		if (sleep_monst(mon, rnd(12), POTION_CLASS)) {
		    pline("%s falls asleep.", Monnam(mon)); /* EN pline("%s falls asleep.", Monnam(mon)); */ // TODO DE
		    slept_monst(mon);
		}
		break;
	case POT_PARALYSIS:
		if (mon->mcanmove) {
			mon->mcanmove = 0;
			/* really should be rnd(5) for consistency with players
			 * breathing potions, but...
			 */
			mon->mfrozen = rnd(25);
		}
		break;
	case POT_SPEED:
		angermon = FALSE;
		mon_adjust_speed(mon, 1, obj);
		break;
	case POT_BLINDNESS:
		if(haseyes(mon->data)) {
		    register int btmp = 64 + rn2(32) +
			rn2(32) * !resist(mon, POTION_CLASS, 0, NOTELL);
		    btmp += mon->mblinded;
		    mon->mblinded = min(btmp,127);
		    mon->mcansee = 0;
		}
		break;
	case POT_WATER:
		if (is_undead(mon->data) || is_demon(mon->data) ||
			is_were(mon->data)) {
		    if (obj->blessed) {
			pline("%s %s in pain!", Monnam(mon), /* EN pline("%s %s in pain!", Monnam(mon), */ // TODO DE
						is_silent(mon->data) ? "writhes" : "shrieks"); /* EN is_silent(mon->data) ? "writhes" : "shrieks"); */ // TODO DE
			mon->mhp -= d(2,6);
			/* should only be by you */
			if (mon->mhp < 1) killed(mon);
			else if (is_were(mon->data) && !is_human(mon->data))
			    new_were(mon);	/* revert to human */
		    } else if (obj->cursed) {
			angermon = FALSE;
			if (canseemon(mon))
			    pline("%s looks healthier.", Monnam(mon)); /* EN pline("%s looks healthier.", Monnam(mon)); */ // TODO DE
			mon->mhp += d(2,6);
			if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			if (is_were(mon->data) && is_human(mon->data) &&
				!Protection_from_shape_changers)
			    new_were(mon);	/* transform into beast */
		    }
		} else if(mon->data == &mons[PM_GREMLIN]) {
		    angermon = FALSE;
		    (void)split_mon(mon, (struct monst *)0);
		} else if(mon->data == &mons[PM_IRON_GOLEM]) {
		    if (canseemon(mon))
			pline("%s rusts.", Monnam(mon)); /* EN pline("%s rusts.", Monnam(mon)); */ // TODO DE
		    mon->mhp -= d(1,6);
		    /* should only be by you */
		    if (mon->mhp < 1) killed(mon);
		}
		break;
	case POT_OIL:
		if (obj->lamplit)
			splatter_burning_oil(mon->mx, mon->my);
		break;
	case POT_ACID:
		if (!resists_acid(mon) && !resist(mon, POTION_CLASS, 0, NOTELL)) {
		    pline("%s %s in pain!", Monnam(mon), /* EN pline("%s %s in pain!", Monnam(mon), */ // TODO DE
			  is_silent(mon->data) ? "writhes" : "shrieks"); /* EN is_silent(mon->data) ? "writhes" : "shrieks"); */ // TODO DE
		    mon->mhp -= d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8);
		    if (mon->mhp < 1) {
			if (your_fault)
			    killed(mon);
			else
			    monkilled(mon, "", AD_ACID);
		    }
		}
		break;
	case POT_POLYMORPH:
		(void) bhitm(mon, obj);
		break;
/*
	case POT_GAIN_LEVEL:
	case POT_LEVITATION:
	case POT_FRUIT_JUICE:
	case POT_MONSTER_DETECTION:
	case POT_OBJECT_DETECTION:
		break;
*/
	}
	if (angermon)
	    wakeup(mon);
	else
	    mon->msleeping = 0;
    }

	/* Note: potionbreathe() does its own docall() */
	if ((distance==0 || ((distance < 3) && rn2(5))) &&
	    (!breathless(youmonst.data) || haseyes(youmonst.data)))
		potionbreathe(obj);
	else if (obj->dknown && !objects[obj->otyp].oc_name_known &&
		   !objects[obj->otyp].oc_uname && cansee(mon->mx,mon->my))
		docall(obj);
	if(*u.ushops && obj->unpaid) {
	        register struct monst *shkp =
			shop_keeper(*in_rooms(u.ux, u.uy, SHOPBASE));

		if(!shkp)
		    obj->unpaid = 0;
		else {
		    (void)stolen_value(obj, u.ux, u.uy,
				 (boolean)shkp->mpeaceful, FALSE);
		    subfrombill(obj, shkp);
		}
	}
	obfree(obj, (struct obj *)0);
}

/* vapors are inhaled or get in your eyes */
void
potionbreathe(obj)
register struct obj *obj;
{
	register int i, ii, isdone, kn = 0;

	switch(obj->otyp) {
	case POT_RESTORE_ABILITY:
	case POT_GAIN_ABILITY:
		if(obj->cursed) {
		    if (!breathless(youmonst.data))
			pline("Ulch!  That potion smells terrible!"); /* EN pline("Ulch!  That potion smells terrible!"); */ // TODO DE
		    else if (haseyes(youmonst.data)) {
			int numeyes = eyecount(youmonst.data);
			Your("%s sting%s!", /* EN Your("%s sting%s!", */ // TODO DE
			     (numeyes == 1) ? body_part(EYE) : makeplural(body_part(EYE)),
			     (numeyes == 1) ? "s" : "");
		    }
		    break;
		} else {
		    i = rn2(A_MAX);		/* start at a random point */
		    for(isdone = ii = 0; !isdone && ii < A_MAX; ii++) {
			if(ABASE(i) < AMAX(i)) {
			    ABASE(i)++;
			    /* only first found if not blessed */
			    isdone = !(obj->blessed);
			    flags.botl = 1;
			}
			if(++i >= A_MAX) i = 0;
		    }
		}
		break;
	case POT_FULL_HEALING:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		/*FALL THROUGH*/
	case POT_EXTRA_HEALING:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		/*FALL THROUGH*/
	case POT_HEALING:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		exercise(A_CON, TRUE);
		break;
	case POT_SICKNESS:
		if (!Role_if(PM_HEALER)) {
			if (Upolyd) {
			    if (u.mh <= 5) u.mh = 1; else u.mh -= 5;
			} else {
			    if (u.uhp <= 5) u.uhp = 1; else u.uhp -= 5;
			}
			flags.botl = 1;
			exercise(A_CON, FALSE);
		}
		break;
	case POT_HALLUCINATION:
		You("have a momentary vision."); /* EN You("have a momentary vision."); */ // TODO DE
		break;
	case POT_CONFUSION:
	case POT_BOOZE:
		if(!Confusion)
			You_feel("somewhat dizzy."); /* EN You_feel("somewhat dizzy."); */ // TODO DE
		make_confused(itimeout_incr(HConfusion, rnd(5)), FALSE);
		break;
	case POT_INVISIBILITY:
		if (!Blind && !Invis) {
		    kn++;
		    pline("For an instant you %s!", /* EN pline("For an instant you %s!", */ // TODO DE
			See_invisible ? "could see right through yourself" /* EN See_invisible ? "could see right through yourself" */ // TODO DE
			: "couldn't see yourself"); /* EN : "couldn't see yourself"); */ // TODO DE
		}
		break;
	case POT_PARALYSIS:
		kn++;
		if (!Free_action) {
		    pline("%s seems to be holding you.", Something); /* EN pline("%s seems to be holding you.", Something); */ // TODO DE
		    nomul(-rnd(5));
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("stiffen momentarily."); /* EN } else You("stiffen momentarily."); */ // TODO DE
		break;
	case POT_SLEEPING:
		kn++;
		if (!Free_action && !Sleep_resistance) {
		    You_feel("rather tired."); /* EN You_feel("rather tired."); */ // TODO DE
		    nomul(-rnd(5));
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("yawn."); /* EN } else You("yawn."); */ // TODO DE
		break;
	case POT_SPEED:
		if (!Fast) Your("knees seem more flexible now."); /* EN if (!Fast) Your("knees seem more flexible now."); */ // TODO DE
		incr_itimeout(&HFast, rnd(5));
		exercise(A_DEX, TRUE);
		break;
	case POT_BLINDNESS:
		if (!Blind && !u.usleep) {
		    kn++;
		    pline("It suddenly gets dark."); /* EN pline("It suddenly gets dark."); */ // TODO DE
		}
		make_blinded(itimeout_incr(Blinded, rnd(5)), FALSE);
		if (!Blind && !u.usleep) Your(vision_clears);
		break;
	case POT_WATER:
		if(u.umonnum == PM_GREMLIN) {
		    (void)split_mon(&youmonst, (struct monst *)0);
		} else if (u.ulycn >= LOW_PM) {
		    /* vapor from [un]holy water will trigger
		       transformation but won't cure lycanthropy */
		    if (obj->blessed && youmonst.data == &mons[u.ulycn])
			you_unwere(FALSE);
		    else if (obj->cursed && !Upolyd)
			you_were();
		}
		break;
	case POT_ACID:
	case POT_POLYMORPH:
		exercise(A_CON, FALSE);
		break;
/*
	case POT_GAIN_LEVEL:
	case POT_LEVITATION:
	case POT_FRUIT_JUICE:
	case POT_MONSTER_DETECTION:
	case POT_OBJECT_DETECTION:
	case POT_OIL:
		break;
*/
	}
	/* note: no obfree() */
	if (obj->dknown) {
	    if (kn)
		makeknown(obj->otyp);
	    else if (!objects[obj->otyp].oc_name_known &&
						!objects[obj->otyp].oc_uname)
		docall(obj);
	}
}

STATIC_OVL short
mixtype(o1, o2)
register struct obj *o1, *o2;
/* returns the potion type when o1 is dipped in o2 */
{
	/* cut down on the number of cases below */
	if (o1->oclass == POTION_CLASS &&
	    (o2->otyp == POT_GAIN_LEVEL ||
	     o2->otyp == POT_GAIN_ENERGY ||
	     o2->otyp == POT_HEALING ||
	     o2->otyp == POT_EXTRA_HEALING ||
	     o2->otyp == POT_FULL_HEALING ||
	     o2->otyp == POT_ENLIGHTENMENT ||
	     o2->otyp == POT_FRUIT_JUICE)) {
		struct obj *swp;

		swp = o1; o1 = o2; o2 = swp;
	}

	switch (o1->otyp) {
		case POT_HEALING:
			switch (o2->otyp) {
			    case POT_SPEED:
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_EXTRA_HEALING;
			}
		case POT_EXTRA_HEALING:
			switch (o2->otyp) {
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_FULL_HEALING;
			}
		case POT_FULL_HEALING:
			switch (o2->otyp) {
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_GAIN_ABILITY;
			}
		case UNICORN_HORN:
			switch (o2->otyp) {
			    case POT_SICKNESS:
				return POT_FRUIT_JUICE;
			    case POT_HALLUCINATION:
			    case POT_BLINDNESS:
			    case POT_CONFUSION:
				return POT_WATER;
			}
			break;
		case AMETHYST:		/* "a-methyst" == "not intoxicated" */
			if (o2->otyp == POT_BOOZE)
			    return POT_FRUIT_JUICE;
			break;
		case POT_GAIN_LEVEL:
		case POT_GAIN_ENERGY:
			switch (o2->otyp) {
			    case POT_CONFUSION:
				return (rn2(3) ? POT_BOOZE : POT_ENLIGHTENMENT);
			    case POT_HEALING:
				return POT_EXTRA_HEALING;
			    case POT_EXTRA_HEALING:
				return POT_FULL_HEALING;
			    case POT_FULL_HEALING:
				return POT_GAIN_ABILITY;
			    case POT_FRUIT_JUICE:
				return POT_SEE_INVISIBLE;
			    case POT_BOOZE:
				return POT_HALLUCINATION;
			}
			break;
		case POT_FRUIT_JUICE:
			switch (o2->otyp) {
			    case POT_SICKNESS:
				return POT_SICKNESS;
			    case POT_SPEED:
				return POT_BOOZE;
			    case POT_GAIN_LEVEL:
			    case POT_GAIN_ENERGY:
				return POT_SEE_INVISIBLE;
			}
			break;
		case POT_ENLIGHTENMENT:
			switch (o2->otyp) {
			    case POT_LEVITATION:
				if (rn2(3)) return POT_GAIN_LEVEL;
				break;
			    case POT_FRUIT_JUICE:
				return POT_BOOZE;
			    case POT_BOOZE:
				return POT_CONFUSION;
			}
			break;
	}

	return 0;
}


boolean
get_wet(obj)
register struct obj *obj;
/* returns TRUE if something happened (potion should be used up) */
{
	char Your_buf[BUFSZ];

	if (snuff_lit(obj)) return(TRUE);

	if (obj->greased) {
		grease_protect(obj,(char *)0,&youmonst);
		return(FALSE);
	}
	(void) Shk_Your(Your_buf, obj);
	/* (Rusting shop goods ought to be charged for.) */
	switch (obj->oclass) {
	    case WEAPON_CLASS:
		if (!obj->oerodeproof && is_rustprone(obj) &&
		    (obj->oeroded < MAX_ERODE) && !rn2(2)) {
			pline("%s %s some%s.", /* EN pline("%s %s some%s.", */ // TODO DE
			      Your_buf, aobjnam(obj, "rust"), /* EN Your_buf, aobjnam(obj, "rust"), */ // TODO DE
			      obj->oeroded ? " more" : "what"); /* EN obj->oeroded ? " more" : "what"); */ // TODO DE
			obj->oeroded++;
			update_inventory();
			return TRUE;
		} else break;
	    case POTION_CLASS:
		if (obj->otyp == POT_WATER) return FALSE;
		/* KMH -- Water into acid causes an explosion */
		if (obj->otyp == POT_ACID) {
			pline("It boils vigorously!"); /* EN pline("It boils vigorously!"); */ // TODO DE
			You("are caught in the explosion!"); /* EN You("are caught in the explosion!"); */ // TODO DE
			losehp(rnd(10), "elementary chemistry", KILLED_BY); /* EN losehp(rnd(10), "elementary chemistry", KILLED_BY); */ // TODO DE
			makeknown(obj->otyp);
			update_inventory();
			return (TRUE);
		}
		pline("%s %s%s.", Your_buf, aobjnam(obj,"dilute"), /* EN pline("%s %s%s.", Your_buf, aobjnam(obj,"dilute"), */ // TODO DE
		      obj->odiluted ? " further" : ""); /* EN obj->odiluted ? " further" : ""); */ // TODO DE
		if(obj->unpaid && costly_spot(u.ux, u.uy)) {
		    You("dilute it, you pay for it."); /* EN You("dilute it, you pay for it."); */ // TODO DE
		    bill_dummy_object(obj);
		}
		if (obj->odiluted) {
			obj->odiluted = 0;
#ifdef UNIXPC
			obj->blessed = FALSE;
			obj->cursed = FALSE;
#else
			obj->blessed = obj->cursed = FALSE;
#endif
			obj->otyp = POT_WATER;
		} else obj->odiluted++;
		update_inventory();
		return TRUE;
	    case SCROLL_CLASS:
		if (obj->otyp != SCR_BLANK_PAPER
#ifdef MAIL
		    && obj->otyp != SCR_MAIL
#endif
		    ) {
			if (!Blind) {
				boolean oq1 = obj->quan == 1L;
				pline_The("scroll%s %s.", /* EN pline_The("scroll%s %s.", */ // TODO DE
						oq1 ? "" : "s", otense(obj, "fade")); /* EN oq1 ? "" : "s", otense(obj, "fade")); */ // TODO DE
			}
			if(obj->unpaid && costly_spot(u.ux, u.uy)) {
			    You("erase it, you pay for it."); /* EN You("erase it, you pay for it."); */ // TODO DE
			    bill_dummy_object(obj);
			}
			obj->otyp = SCR_BLANK_PAPER;
			obj->spe = 0;
			update_inventory();
			return TRUE;
		} else break;
	    case SPBOOK_CLASS:
		if (obj->otyp != SPE_BLANK_PAPER) {

			if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
	pline("%s suddenly heats up; steam rises and it remains dry.", /* EN pline("%s suddenly heats up; steam rises and it remains dry.", */ // TODO DE
				The(xname(obj)));
			} else {
			    if (!Blind) {
				    boolean oq1 = obj->quan == 1L;
				    pline_The("spellbook%s %s.", /* EN pline_The("spellbook%s %s.", */ // TODO DE
					oq1 ? "" : "s", otense(obj, "fade")); /* EN oq1 ? "" : "s", otense(obj, "fade")); */ // TODO DE
			    }
			    if(obj->unpaid && costly_spot(u.ux, u.uy)) {
			        You("erase it, you pay for it."); /* EN You("erase it, you pay for it."); */ // TODO DE
			        bill_dummy_object(obj);
			    }
			    obj->otyp = SPE_BLANK_PAPER;
			    update_inventory();
			}
			return TRUE;
		}
	}
	pline("%s %s wet.", Your_buf, aobjnam(obj,"get")); /* EN pline("%s %s wet.", Your_buf, aobjnam(obj,"get")); */ // TODO DE
	return FALSE;
}

int
dodip()
{
	register struct obj *potion, *obj;
	struct obj *singlepotion;
	const char *tmp;
	uchar here;
	char allowall[2];
	short mixture;
	char qbuf[QBUFSZ], Your_buf[BUFSZ];

	allowall[0] = ALL_CLASSES; allowall[1] = '\0';
	if(!(obj = getobj(allowall, "dip"))) /* EN if(!(obj = getobj(allowall, "dip"))) */ // TODO DE
		return(0);

	here = levl[u.ux][u.uy].typ;
	/* Is there a fountain to dip into here? */
	if (IS_FOUNTAIN(here)) {
		if(yn("Dip it into the fountain?") == 'y') { /* EN if(yn("Dip it into the fountain?") == 'y') { */ // TODO DE
			dipfountain(obj);
			return(1);
		}
	} else if (is_pool(u.ux,u.uy)) {
		tmp = (here == POOL) ? "pool" : "moat"; /* EN tmp = (here == POOL) ? "pool" : "moat"; */ // TODO DE
		Sprintf(qbuf, "Dip it into the %s?", tmp); /* EN Sprintf(qbuf, "Dip it into the %s?", tmp); */ // TODO DE
		if (yn(qbuf) == 'y') {
		    if (Levitation) {
			floating_above(tmp);
#ifdef STEED
		    } else if (u.usteed && !is_swimmer(u.usteed->data) &&
			    P_SKILL(P_RIDING) < P_BASIC) {
			rider_cant_reach(); /* not skilled enough to reach */
#endif
		    } else {
			(void) get_wet(obj);
			if (obj->otyp == POT_ACID) useup(obj);
		    }
		    return 1;
		}
	}

	if(!(potion = getobj(beverages, "dip into"))) /* EN if(!(potion = getobj(beverages, "dip into"))) */ // TODO DE
		return(0);
	if (potion == obj && potion->quan == 1L) {
		pline("That is a potion bottle, not a Klein bottle!"); /* EN pline("That is a potion bottle, not a Klein bottle!"); */ // TODO DE
		return 0;
	}
	potion->in_use = TRUE;		/* assume it will be used up */
	if(potion->otyp == POT_WATER) {
		boolean useeit = !Blind;
		if (useeit) (void) Shk_Your(Your_buf, obj);
		if (potion->blessed) {
			if (obj->cursed) {
				if (useeit)
				    pline("%s %s %s.",
					  Your_buf,
					  aobjnam(obj, "softly glow"), /* EN aobjnam(obj, "softly glow"), */ // TODO DE
					  hcolor(NH_AMBER));
				uncurse(obj);
				obj->bknown=1;
	poof:
				if(!(objects[potion->otyp].oc_name_known) &&
				   !(objects[potion->otyp].oc_uname))
					docall(potion);
				useup(potion);
				return(1);
			} else if(!obj->blessed) {
				if (useeit) {
				    tmp = hcolor(NH_LIGHT_BLUE);
				    pline("%s %s with a%s %s aura.", /* EN pline("%s %s with a%s %s aura.", */ // TODO DE
					  Your_buf,
					  aobjnam(obj, "softly glow"), /* EN aobjnam(obj, "softly glow"), */ // TODO DE
					  index(vowels, *tmp) ? "n" : "", tmp);
				}
				bless(obj);
				obj->bknown=1;
				goto poof;
			}
		} else if (potion->cursed) {
			if (obj->blessed) {
				if (useeit)
				    pline("%s %s %s.",
					  Your_buf,
					  aobjnam(obj, "glow"), /* EN aobjnam(obj, "glow"), */ // TODO DE
					  hcolor((const char *)"brown")); /* EN hcolor((const char *)"brown")); */ // TODO DE
				unbless(obj);
				obj->bknown=1;
				goto poof;
			} else if(!obj->cursed) {
				if (useeit) {
				    tmp = hcolor(NH_BLACK);
				    pline("%s %s with a%s %s aura.", /* EN pline("%s %s with a%s %s aura.", */ // TODO DE
					  Your_buf,
					  aobjnam(obj, "glow"), /* EN aobjnam(obj, "glow"), */ // TODO DE
					  index(vowels, *tmp) ? "n" : "", tmp);
				}
				curse(obj);
				obj->bknown=1;
				goto poof;
			}
		} else
			if (get_wet(obj))
			    goto poof;
	} else if (obj->otyp == POT_POLYMORPH ||
		potion->otyp == POT_POLYMORPH) {
	    /* some objects can't be polymorphed */
	    if (obj->otyp == potion->otyp ||	/* both POT_POLY */
		    obj->otyp == WAN_POLYMORPH ||
		    obj->otyp == SPE_POLYMORPH ||
		    obj == uball || obj == uskin ||
		    obj_resists(obj->otyp == POT_POLYMORPH ?
				potion : obj, 5, 95)) {
		pline(nothing_happens);
	    } else {
	    	boolean was_wep = FALSE, was_swapwep = FALSE, was_quiver = FALSE;
		short save_otyp = obj->otyp;
		/* KMH, conduct */
		u.uconduct.polypiles++;

		if (obj == uwep) was_wep = TRUE;
		else if (obj == uswapwep) was_swapwep = TRUE;
		else if (obj == uquiver) was_quiver = TRUE;

		obj = poly_obj(obj, STRANGE_OBJECT);

		if (was_wep) setuwep(obj);
		else if (was_swapwep) setuswapwep(obj);
		else if (was_quiver) setuqwep(obj);

		if (obj->otyp != save_otyp) {
			makeknown(POT_POLYMORPH);
			useup(potion);
			prinv((char *)0, obj, 0L);
			return 1;
		} else {
			pline("Nothing seems to happen."); /* EN pline("Nothing seems to happen."); */ // TODO DE
			goto poof;
		}
	    }
	    potion->in_use = FALSE;	/* didn't go poof */
	    return(1);
	} else if(obj->oclass == POTION_CLASS && obj->otyp != potion->otyp) {
		/* Mixing potions is dangerous... */
		pline_The("potions mix..."); /* EN pline_The("potions mix..."); */ // TODO DE
		/* KMH, balance patch -- acid is particularly unstable */
		if (obj->cursed || obj->otyp == POT_ACID || !rn2(10)) {
			pline("BOOM!  They explode!"); /* EN pline("BOOM!  They explode!"); */ // TODO DE
			exercise(A_STR, FALSE);
			if (!breathless(youmonst.data) || haseyes(youmonst.data))
				potionbreathe(obj);
			useup(obj);
			useup(potion);
			losehp(rnd(10), "alchemic blast", KILLED_BY_AN); /* EN losehp(rnd(10), "alchemic blast", KILLED_BY_AN); */ // TODO DE
			return(1);
		}

		obj->blessed = obj->cursed = obj->bknown = 0;
		if (Blind || Hallucination) obj->dknown = 0;

		if ((mixture = mixtype(obj, potion)) != 0) {
			obj->otyp = mixture;
		} else {
		    switch (obj->odiluted ? 1 : rnd(8)) {
			case 1:
				obj->otyp = POT_WATER;
				break;
			case 2:
			case 3:
				obj->otyp = POT_SICKNESS;
				break;
			case 4:
				{
				  struct obj *otmp;
				  otmp = mkobj(POTION_CLASS,FALSE);
				  obj->otyp = otmp->otyp;
				  obfree(otmp, (struct obj *)0);
				}
				break;
			default:
				if (!Blind)
			  pline_The("mixture glows brightly and evaporates."); /* EN pline_The("mixture glows brightly and evaporates."); */ // TODO DE
				useup(obj);
				useup(potion);
				return(1);
		    }
		}

		obj->odiluted = (obj->otyp != POT_WATER);

		if (obj->otyp == POT_WATER && !Hallucination) {
			pline_The("mixture bubbles%s.", /* EN pline_The("mixture bubbles%s.", */ // TODO DE
				Blind ? "" : ", then clears"); /* EN Blind ? "" : ", then clears"); */ // TODO DE
		} else if (!Blind) {
			pline_The("mixture looks %s.", /* EN pline_The("mixture looks %s.", */ // TODO DE
				hcolor(OBJ_DESCR(objects[obj->otyp])));
		}

		useup(potion);
		return(1);
	}

#ifdef INVISIBLE_OBJECTS
	if (potion->otyp == POT_INVISIBILITY && !obj->oinvis) {
		obj->oinvis = TRUE;
		if (!Blind) {
		    if (!See_invisible) pline("Where did %s go?", /* EN if (!See_invisible) pline("Where did %s go?", */ // TODO DE
		    		the(xname(obj)));
		    else You("notice a little haziness around %s.", /* EN else You("notice a little haziness around %s.", */ // TODO DE
		    		the(xname(obj)));
		}
		goto poof;
	} else if (potion->otyp == POT_SEE_INVISIBLE && obj->oinvis) {
		obj->oinvis = FALSE;
		if (!Blind) {
		    if (!See_invisible) pline("So that's where %s went!", /* EN if (!See_invisible) pline("So that's where %s went!", */ // TODO DE
		    		the(xname(obj)));
		    else pline_The("haziness around %s disappears.", /* EN else pline_The("haziness around %s disappears.", */ // TODO DE
		    		the(xname(obj)));
		}
		goto poof;
	}
#endif

	if(is_poisonable(obj)) {
	    if(potion->otyp == POT_SICKNESS && !obj->opoisoned) {
		char buf[BUFSZ];
		if (potion->quan > 1L)
		    Sprintf(buf, "One of %s", the(xname(potion))); /* EN Sprintf(buf, "One of %s", the(xname(potion))); */ // TODO DE
		else
		    Strcpy(buf, The(xname(potion)));
		pline("%s forms a coating on %s.", /* EN pline("%s forms a coating on %s.", */ // TODO DE
		      buf, the(xname(obj)));
		obj->opoisoned = TRUE;
		goto poof;
	    } else if(obj->opoisoned &&
		      (potion->otyp == POT_HEALING ||
		       potion->otyp == POT_EXTRA_HEALING ||
		       potion->otyp == POT_FULL_HEALING)) {
		pline("A coating wears off %s.", the(xname(obj))); /* EN pline("A coating wears off %s.", the(xname(obj))); */ // TODO DE
		obj->opoisoned = 0;
		goto poof;
	    }
	}

	if (potion->otyp == POT_OIL) {
	    boolean wisx = FALSE;
	    if (potion->lamplit) {	/* burning */
		int omat = objects[obj->otyp].oc_material;
		/* the code here should be merged with fire_damage */
		if (catch_lit(obj)) {
		    /* catch_lit does all the work if true */
		} else if (obj->oerodeproof || obj_resists(obj, 5, 95) ||
			   !is_flammable(obj) || obj->oclass == FOOD_CLASS) {
		    pline("%s %s to burn for a moment.", /* EN pline("%s %s to burn for a moment.", */ // TODO DE
			  Yname2(obj), otense(obj, "seem")); /* EN Yname2(obj), otense(obj, "seem")); */ // TODO DE
		} else {
		    if ((omat == PLASTIC || omat == PAPER) && !obj->oartifact)
			obj->oeroded = MAX_ERODE;
		    pline_The("burning oil %s %s.", /* EN pline_The("burning oil %s %s.", */ // TODO DE
			    obj->oeroded == MAX_ERODE ? "destroys" : "damages", /* EN obj->oeroded == MAX_ERODE ? "destroys" : "damages", */ // TODO DE
			    yname(obj));
		    if (obj->oeroded == MAX_ERODE) {
			obj_extract_self(obj);
			obfree(obj, (struct obj *)0);
			obj = (struct obj *) 0;
		    } else {
			/* we know it's carried */
			if (obj->unpaid) {
			    /* create a dummy duplicate to put on bill */
			    verbalize("You burnt it, you bought it!"); /* EN verbalize("You burnt it, you bought it!"); */ // TODO DE
			    bill_dummy_object(obj);
			}
			obj->oeroded++;
		    }
		}
	    } else if (potion->cursed) {
		pline_The("potion spills and covers your %s with oil.", /* EN pline_The("potion spills and covers your %s with oil.", */ // TODO DE
			  makeplural(body_part(FINGER)));
		incr_itimeout(&Glib, d(2,10));
	    } else if (obj->oclass != WEAPON_CLASS && !is_weptool(obj)) {
		/* the following cases apply only to weapons */
		goto more_dips;
	    /* Oil removes rust and corrosion, but doesn't unburn.
	     * Arrows, etc are classed as metallic due to arrowhead
	     * material, but dipping in oil shouldn't repair them.
	     */
	    } else if ((!is_rustprone(obj) && !is_corrodeable(obj)) ||
			is_ammo(obj) || (!obj->oeroded && !obj->oeroded2)) {
		/* uses up potion, doesn't set obj->greased */
		pline("%s %s with an oily sheen.", /* EN pline("%s %s with an oily sheen.", */ // TODO DE
		      Yname2(obj), otense(obj, "gleam")); /* EN Yname2(obj), otense(obj, "gleam")); */ // TODO DE
	    } else {
		pline("%s %s less %s.", /* EN pline("%s %s less %s.", */ // TODO DE
		      Yname2(obj), otense(obj, "are"), /* EN Yname2(obj), otense(obj, "are"), */ // TODO DE
		      (obj->oeroded && obj->oeroded2) ? "corroded and rusty" : /* EN (obj->oeroded && obj->oeroded2) ? "corroded and rusty" : */ // TODO DE
			obj->oeroded ? "rusty" : "corroded"); /* EN obj->oeroded ? "rusty" : "corroded"); */ // TODO DE
		if (obj->oeroded > 0) obj->oeroded--;
		if (obj->oeroded2 > 0) obj->oeroded2--;
		wisx = TRUE;
	    }
	    exercise(A_WIS, wisx);
	    makeknown(potion->otyp);
	    useup(potion);
	    return 1;
	}
    more_dips:

	/* Allow filling of MAGIC_LAMPs to prevent identification by player */
	if ((obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP) &&
	   (potion->otyp == POT_OIL)) {
	    /* Turn off engine before fueling, turn off fuel too :-)  */
	    if (obj->lamplit || potion->lamplit) {
		useup(potion);
		explode(u.ux, u.uy, 11, d(6,6), 0, EXPL_FIERY);
		exercise(A_WIS, FALSE);
		return 1;
	    }
	    /* Adding oil to an empty magic lamp renders it into an oil lamp */
	    if ((obj->otyp == MAGIC_LAMP) && obj->spe == 0) {
		obj->otyp = OIL_LAMP;
		obj->age = 0;
	    }
	    if (obj->age > 1000L) {
		pline("%s %s full.", Yname2(obj), otense(obj, "are")); /* EN pline("%s %s full.", Yname2(obj), otense(obj, "are")); */ // TODO DE
		potion->in_use = FALSE;	/* didn't go poof */
	    } else {
		You("fill %s with oil.", yname(obj)); /* EN You("fill %s with oil.", yname(obj)); */ // TODO DE
		check_unpaid(potion);	/* Yendorian Fuel Tax */
		obj->age += 2*potion->age;	/* burns more efficiently */
		if (obj->age > 1500L) obj->age = 1500L;
		useup(potion);
		exercise(A_WIS, TRUE);
	    }
	    makeknown(POT_OIL);
	    obj->spe = 1;
	    update_inventory();
	    return 1;
	}

	potion->in_use = FALSE;		/* didn't go poof */
	if ((obj->otyp == UNICORN_HORN || obj->otyp == AMETHYST) &&
	    (mixture = mixtype(obj, potion)) != 0) {
		char oldbuf[BUFSZ], newbuf[BUFSZ];
		short old_otyp = potion->otyp;
		boolean old_dknown = FALSE;
		boolean more_than_one = potion->quan > 1;

		oldbuf[0] = '\0';
		if (potion->dknown) {
		    old_dknown = TRUE;
		    Sprintf(oldbuf, "%s ",
			    hcolor(OBJ_DESCR(objects[potion->otyp])));
		}
		/* with multiple merged potions, split off one and
		   just clear it */
		if (potion->quan > 1L) {
		    singlepotion = splitobj(potion, 1L);
		} else singlepotion = potion;
		
		if(singlepotion->unpaid && costly_spot(u.ux, u.uy)) {
		    You("use it, you pay for it."); /* EN You("use it, you pay for it."); */ // TODO DE
		    bill_dummy_object(singlepotion);
		}
		singlepotion->otyp = mixture;
		singlepotion->blessed = 0;
		if (mixture == POT_WATER)
		    singlepotion->cursed = singlepotion->odiluted = 0;
		else
		    singlepotion->cursed = obj->cursed;  /* odiluted left as-is */
		singlepotion->bknown = FALSE;
		if (Blind) {
		    singlepotion->dknown = FALSE;
		} else {
		    singlepotion->dknown = !Hallucination;
		    if (mixture == POT_WATER && singlepotion->dknown)
			Sprintf(newbuf, "clears"); /* EN Sprintf(newbuf, "clears"); */ // TODO DE
		    else
			Sprintf(newbuf, "turns %s", /* EN Sprintf(newbuf, "turns %s", */ // TODO DE
				hcolor(OBJ_DESCR(objects[mixture])));
		    pline_The("%spotion%s %s.", oldbuf, /* EN pline_The("%spotion%s %s.", oldbuf, */ // TODO DE
			      more_than_one ? " that you dipped into" : "", /* EN more_than_one ? " that you dipped into" : "", */ // TODO DE
			      newbuf);
		    if(!objects[old_otyp].oc_uname &&
			!objects[old_otyp].oc_name_known && old_dknown) {
			struct obj fakeobj;
			fakeobj = zeroobj;
			fakeobj.dknown = 1;
			fakeobj.otyp = old_otyp;
			fakeobj.oclass = POTION_CLASS;
			docall(&fakeobj);
		    }
		}
		obj_extract_self(singlepotion);
		singlepotion = hold_another_object(singlepotion,
					"You juggle and drop %s!", /* EN "You juggle and drop %s!", */ // TODO DE
					doname(singlepotion), (const char *)0);
		update_inventory();
		return(1);
	}

	pline("Interesting..."); /* EN pline("Interesting..."); */ // TODO DE
	return(1);
}


void
djinni_from_bottle(obj)
register struct obj *obj;
{
	struct monst *mtmp;
	int chance;

	if(!(mtmp = makemon(&mons[PM_DJINNI], u.ux, u.uy, NO_MM_FLAGS))){
		pline("It turns out to be empty."); /* EN pline("It turns out to be empty."); */ // TODO DE
		return;
	}

	if (!Blind) {
		pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp)); /* EN pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp)); */ // TODO DE
		pline("%s speaks.", Monnam(mtmp)); /* EN pline("%s speaks.", Monnam(mtmp)); */ // TODO DE
	} else {
		You("smell acrid fumes."); /* EN You("smell acrid fumes."); */ // TODO DE
		pline("%s speaks.", Something); /* EN pline("%s speaks.", Something); */ // TODO DE
	}

	chance = rn2(5);
	if (obj->blessed) chance = (chance == 4) ? rnd(4) : 0;
	else if (obj->cursed) chance = (chance == 0) ? rn2(4) : 4;
	/* 0,1,2,3,4:  b=80%,5,5,5,5; nc=20%,20,20,20,20; c=5%,5,5,5,80 */

	switch (chance) {
	case 0 : verbalize("I am in your debt.  I will grant one wish!"); /* EN case 0 : verbalize("I am in your debt.  I will grant one wish!"); */ // TODO DE
		makewish();
		mongone(mtmp);
		break;
	case 1 : verbalize("Thank you for freeing me!"); /* EN case 1 : verbalize("Thank you for freeing me!"); */ // TODO DE
		(void) tamedog(mtmp, (struct obj *)0);
		break;
	case 2 : verbalize("You freed me!"); /* EN case 2 : verbalize("You freed me!"); */ // TODO DE
		mtmp->mpeaceful = TRUE;
		set_malign(mtmp);
		break;
	case 3 : verbalize("It is about time!"); /* EN case 3 : verbalize("It is about time!"); */ // TODO DE
		pline("%s vanishes.", Monnam(mtmp)); /* EN pline("%s vanishes.", Monnam(mtmp)); */ // TODO DE
		mongone(mtmp);
		break;
	default: verbalize("You disturbed me, fool!"); /* EN default: verbalize("You disturbed me, fool!"); */ // TODO DE
		break;
	}
}

/* clone a gremlin or mold (2nd arg non-null implies heat as the trigger);
   hit points are cut in half (odd HP stays with original) */
struct monst *
split_mon(mon, mtmp)
struct monst *mon,	/* monster being split */
	     *mtmp;	/* optional attacker whose heat triggered it */
{
	struct monst *mtmp2;
	char reason[BUFSZ];

	reason[0] = '\0';
	if (mtmp) Sprintf(reason, " from %s heat", /* EN if (mtmp) Sprintf(reason, " from %s heat", */ // TODO DE
			  (mtmp == &youmonst) ? (const char *)"your" : /* EN (mtmp == &youmonst) ? (const char *)"your" : */ // TODO DE
			      (const char *)s_suffix(mon_nam(mtmp)));

	if (mon == &youmonst) {
	    mtmp2 = cloneu();
	    if (mtmp2) {
		mtmp2->mhpmax = u.mhmax / 2;
		u.mhmax -= mtmp2->mhpmax;
		flags.botl = 1;
		You("multiply%s!", reason); /* EN You("multiply%s!", reason); */ // TODO DE
	    }
	} else {
	    mtmp2 = clone_mon(mon);
	    if (mtmp2) {
		mtmp2->mhpmax = mon->mhpmax / 2;
		mon->mhpmax -= mtmp2->mhpmax;
		if (canspotmon(mon))
		    pline("%s multiplies%s!", Monnam(mon), reason); /* EN pline("%s multiplies%s!", Monnam(mon), reason); */ // TODO DE
	    }
	}
	return mtmp2;
}

#endif /* OVLB */

/*potion.c*/
