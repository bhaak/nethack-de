/*	SCCS Id: @(#)potion.c	3.4	2002/10/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef GERMAN
# include "german.h"
#endif

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
		    Du_fuehlst_dich("jetzt weniger %s.", /* EN You_feel("less %s now.", */
			Hallucination ? "trippig" : "verwirrt"); /* EN Hallucination ? "trippy" : "confused"); */
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
		    Du_fuehlst_dich("jetzt %s.", /* EN You_feel("%s now.", */
			Hallucination ? "weniger wacklig" : "nicht mehr so benommen"); /* EN Hallucination ? "less wobbly" : "a bit steadier"); */
	}
	if (xtime && !old) {
		if (talk) {
#ifdef STEED
			if (u.usteed)
				You("VERB_SCHWANKEN im Sattel."); /* EN You("wobble in the saddle."); */
			else
#endif
			You("%s ...", stagger(youmonst.data, "VERB_STAGGER")); /* EN You("%s...", stagger(youmonst.data, "stagger")); */
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
		Du_fuehlst_dich("todkrank."); /* EN You_feel("deathly sick."); */
	    } else {
		/* already sick */
		if (talk) Du_fuehlst_dich("%s schlechter.", /* EN if (talk) You_feel("%s worse.", */
			      xtime <= Sick/2L ? "viel" : "noch"); /* EN xtime <= Sick/2L ? "much" : "even"); */
	    }
	    set_itimeout(&Sick, xtime);
	    u.usick_type |= type;
	    flags.botl = TRUE;
	} else if (old && (type & u.usick_type)) {
	    /* was sick, now not */
	    u.usick_type &= ~type;
	    if (u.usick_type) { /* only partly cured */
		if (talk) Du_fuehlst_dich("ein wenig besser."); /* EN if (talk) You_feel("somewhat better."); */
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
	    if(talk) Dir_ist("jetzt viel weniger ¸bel."); /* EN if(talk) You_feel("much less nauseated now."); */

	set_itimeout(&Vomiting, xtime);
}

static const char vismsg[] = "NOUN_SEHKRAFT VERB_SCHEINEN sich kurz zu %s, aber sie ist jetzt wieder %s."; /* EN static const char vismsg[] = "vision seems to %s for a moment but is %s now."; */
static const char eyemsg[] = "%s %s kurz."; /* EN static const char eyemsg[] = "%s momentarily %s."; */

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
		    pline("Voll geil!  Alles ist wieder kosmisch!"); /* EN pline("Far out!  Everything is all cosmic again!"); */
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
			 (eyecnt == 1) ? "VERB_JUCKEN" : "VERB_JUCKEN"); /* EN (eyecnt == 1) ? "itches" : "itch"); */
		} else {	/* Eyes of the Overworld */
		    Your(vismsg, "bessern", /* EN Your(vismsg, "brighten", */
			 Hallucination ? "schlimmer" : "normal"); /* EN Hallucination ? "sadder" : "normal"); */
		}
	    }
	}

	if (u_could_see && !can_see_now) {	/* losing sight */
	    if (talk) {
		if (Hallucination)
		    pline("Oh, Mist!  Alles ist dunkel!  Hilfe!"); /* EN pline("Oh, bummer!  Everything is dark!  Help!"); */
		else
		    pline("SUBJECT NOUN_DUNKELHEIT VERB_BRECHEN ¸ber OBJECT PRONOMEN_PERSONAL herein."); /* EN pline("A cloud of darkness falls upon you."); */
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
			 (eyecnt == 1) ? "VERB_ZUCKEN" : "VERB_ZUCKEN"); /* EN (eyecnt == 1) ? "twitches" : "twitch"); */
		} else {	/* Eyes of the Overworld */
		    Your(vismsg, "tr¸ben", /* EN Your(vismsg, "dim", */
			 Hallucination ? "zufriedenstellend" : "normal"); /* EN Hallucination ? "happier" : "normal"); */
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

boolean
make_hallucinated(xtime, talk, mask)
long xtime;	/* nonzero if this is an attempt to turn on hallucination */
boolean talk;
long mask;	/* nonzero if resistance status should change by mask */
{
	long old = HHallucination;
	boolean changed = 0;
	const char *message, *verb, *verbzusatz;

	message = (!xtime) ? "Jetzt %s alles SO langweilig %s." : /* EN message = (!xtime) ? "Everything %s SO boring now." : */
			     "Oh wow!  Alles %s so kosmisch %s!"; /* EN "Oh wow!  Everything %s so cosmic!"; */
	verb = (!Blind) ? "sieht" : "f¸hlt sich"; /* EN verb = (!Blind) ? "looks" : "feels"; */
	verbzusatz = (!Blind) ? "aus" : "an"; /* EN */

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
			 (eyecnt == 1) ? "VERB_JUCKEN" : "VERB_JUCKEN"); /* EN (eyecnt == 1) ? "itches" : "itch"); */
		} else {	/* Grayswandir */
		    Your(vismsg, "reduzieren", "normal"); /* EN Your(vismsg, "flatten", "normal"); */
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
	    if (talk) pline(message, verb, verbzusatz); /* EN if (talk) pline(message, verb); */
	}
	return changed;
}

STATIC_OVL void
ghost_from_bottle()
{
	struct monst *mtmp = makemon(&mons[PM_GHOST], u.ux, u.uy, NO_MM_FLAGS);

	if (!mtmp) {
		pline("Diese Flasche stellt sich als leer heraus."); /* EN pline("This bottle turns out to be empty."); */
		return;
	}
	if (Blind) {
		pline("Als PRONOMEN_PERSONAL die Flasche VERB_OEFFNEN, NEUER_SATZ SUBJECT_IM_SATZ VERB_ENTWEICHEN %s.", something); /* EN pline("As you open the bottle, %s emerges.", something); */
		return;
	}
	pline("Als PRONOMEN_PERSONAL die Flasche VERB_OEFFNEN, NEUER_SATZ SUBJECT_IM_SATZ VERB_ENTWEICHEN ARTIKEL_UNBESTIMMTER ADJEKTIV_RIESIG %s!", /* EN pline("As you open the bottle, an enormous %s emerges!", */
		Hallucination ? rndmonnam() : (const char *)"NOUN_GHOST"); /* EN Hallucination ? rndmonnam() : (const char *)"ghost"); */
	if(flags.verbose)
	    You("VERB_SEIN zu Tode erschrocken und starr vor Angst."); /* EN You("are frightened to death, and unable to move."); */
	nomul(-3);
	nomovemsg = "SUBJECT PRONOMEN_PERSONAL VERB_GEWINNEN OBJECT PRONOMEN_POSSESSIV NOUN_FASSUNG zur¸ck."; /* EN nomovemsg = "You regain your composure."; */
}

/* "Quaffing is like drinking, except you spill more."  -- Terry Pratchett
 */
int
dodrink()
{
	register struct obj *otmp;
	const char *potion_descr;

	if (Strangled) {
		pline("Wenn SUBJECT_IM_SATZ PRONOMEN_PERSONAL nicht atmen VERB_KOENNEN, wie VERB_WOLLEN PRONOMEN_PERSONAL dann trinken?"); /* EN pline("If you can't breathe air, how can you drink liquid?"); */
		return 0;
	}
	/* Is there a fountain to drink from here? */
	if (IS_FOUNTAIN(levl[u.ux][u.uy].typ) && !Levitation) {
		if(yn("Vom Springbrunnen trinken?") == 'y') { /* EN if(yn("Drink from the fountain?") == 'y') { */
			drinkfountain();
			return 1;
		}
	}
#ifdef SINKS
	/* Or a kitchen sink? */
	if (IS_SINK(levl[u.ux][u.uy].typ)) {
		if (yn("Von der Sp¸le trinken?") == 'y') { /* EN if (yn("Drink from the sink?") == 'y') { */
			drinksink();
			return 1;
		}
	}
#endif

	/* Or are you surrounded by water? */
	if (Underwater) {
		if (yn("Das Wasser um KASUS_AKKUSATIV PRONOMEN_PERSONAL herum trinken?") == 'y') { /* EN if (yn("Drink the water around you?") == 'y') { */
			if (Role_if(PM_ARCHEOLOGIST)) {
		    pline("Nein, danke.  Fische lieben sich darin!"); /* Indiana-Jones-Referenz */
			} else {
		    pline("SATZBEGINN VERB_KOENNEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL KASUS_DATIV PRONOMEN_PERSONAL ¸berhaupt vorstellen, was in dem Wasser lebt?"); /* EN pline("Do you know what lives in this water!");  */
			}
			return 1;
		}
	}

	otmp = getobj(beverages, "VERB_TRINKEN"); /* EN otmp = getobj(beverages, "drink"); */
	if(!otmp) return(0);
	otmp->in_use = TRUE;		/* you've opened the stopper */

#define POTION_OCCUPANT_CHANCE(n) (13 + 2*(n))	/* also in muse.c */

	potion_descr = OBJ_DESCR(objects[otmp->otyp]);
	if (potion_descr) {
	    if (!strcmp(potion_descr, "ADJEKTIV_POT_MILKY") && /* EN if (!strcmp(potion_descr, "milky") && */
		    flags.ghost_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.ghost_count))) {
		ghost_from_bottle();
		useup(otmp);
		return(1);
	    } else if (!strcmp(potion_descr, "ADJEKTIV_POT_SMOKY") && /* EN } else if (!strcmp(potion_descr, "smoky") && */
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
	    pline("Ein %s Gef¸hl ¸berkommt KASUS_AKKUSATIV PRONOMEN_PERSONAL plˆtzlich, doch es vergeht wieder.", /* EN You("have a %s feeling for a moment, then it passes.", */
		  Hallucination ? "normales" : "sonderbares"); /* EN Hallucination ? "normal" : "peculiar"); */
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
		    pline("÷rks!  %s mittelm‰ﬂig!", this_makes_you_feel); /* EN pline("Ulch!  This makes you feel mediocre!"); */
		    break;
		} else {
		    pline("Wau!  %s %s!", this_makes_you_feel, /* EN pline("Wow!  This makes you feel %s!", */
			  (otmp->blessed) ?
				(unfixable_trouble_count(FALSE) ? "besser" : "groﬂartig") /* EN (unfixable_trouble_count(FALSE) ? "better" : "great") */
			  : "gut"); /* EN : "good"); */
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
		(void) make_hallucinated(itimeout_incr(HHallucination,
					   rn1(200, 600 - 300 * bcsign(otmp))),
				  TRUE, 0L);
		break;
	case POT_WATER:
		if(!otmp->blessed && !otmp->cursed) {
		    pline("Das schmeckt nach Wasser."); /* EN pline("This tastes like water."); */
		    u.uhunger += rnd(10);
		    newuhs(FALSE);
		    break;
		}
		unkn++;
		if(is_undead(youmonst.data) || is_demon(youmonst.data) ||
				u.ualign.type == A_CHAOTIC) {
		    if(otmp->blessed) {
			pline("Das brennt wie S‰ure!"); /* EN pline("This burns like acid!"); */
			exercise(A_CON, FALSE);
			if (u.ulycn >= LOW_PM) {
			    Your("NOUN_VERBUNDENHEIT mit OBJECT KASUS_DATIV %s VERB_VERSCHWINDEN!", /* EN Your("affinity to %s disappears!", */
				 makeplural(mons[u.ulycn].mname));
			    if (youmonst.data == &mons[u.ulycn])
				you_unwere(FALSE);
			    u.ulycn = NON_PM;	/* cure lycanthropy */
			}
			losehp(d(2,6), "NOUN_POTION PARTIKEL_OF NOUN_POT_HOLY_WATER", KILLED_BY_AN); /* EN losehp(d(2,6), "potion of holy water", KILLED_BY_AN); */
		    } else if(otmp->cursed) {
			Du_fuehlst_dich("ziemlich stolz auf KASUS_AKKUSATIV PRONOMEN_PERSONAL selbst."); /* EN You_feel("quite proud of yourself."); */
			healup(d(2,6),0,0,0);
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, TRUE);
		    }
		} else {
		    if(otmp->blessed) {
			Du_fuehlst_dich("voller Ehrfurcht."); /* EN You_feel("full of awe."); */
			make_sick(0L, (char *) 0, TRUE, SICK_ALL);
			exercise(A_WIS, TRUE);
			exercise(A_CON, TRUE);
			if (u.ulycn >= LOW_PM)
			    you_unwere(TRUE);	/* "Purified" */
			/* make_confused(0L,TRUE); */
		    } else {
			if(u.ualign.type == A_LAWFUL) {
			    pline("Das brennt wie S‰ure!"); /* EN pline("This burns like acid!"); */
			    losehp(d(2,6), "NOUN_POTION PARTIKEL_OF NOUN_POT_UNHOLY_WATER", /* EN losehp(d(2,6), "potion of unholy water", */
				KILLED_BY_AN);
			} else
			    Du_fuehlst_dich("voller Furcht."); /* EN You_feel("full of dread."); */
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, FALSE);
		    }
		}
		break;
	case POT_BOOZE:
		unkn++;
		pline("Uff!  Das schmeckt wie %s%s!", /* EN pline("Ooph!  This tastes like %s%s!", */
		      otmp->odiluted ? "verw‰ssertes " : "", /* EN otmp->odiluted ? "watered down " : "", */
		      Hallucination ? "Lˆwenzahnwein" : "Feuerwasser"); /* EN Hallucination ? "dandelion wine" : "liquid fire"); */
		if (!otmp->blessed)
		    make_confused(itimeout_incr(HConfusion, d(3,8)), FALSE);
		/* the whiskey makes us feel better */
		if (!otmp->odiluted) healup(1, 0, FALSE, FALSE);
		u.uhunger += 10 * (2 + bcsign(otmp));
		newuhs(FALSE);
		exercise(A_WIS, FALSE);
		if(otmp->cursed) {
			You("VERB_WERDEN ohnm‰chtig."); /* EN You("pass out."); */
			multi = -rnd(15);
			nomovemsg = "SUBJECT PRONOMEN_PERSONAL VERB_AUFWACHEN mit Kopfschmerzen SATZKLAMMER."; /* EN nomovemsg = "You awake with a headache."; */
		}
		break;
	case POT_ENLIGHTENMENT:
		if(otmp->cursed) {
			unkn++;
			You("VERB_HAVE eine bˆse Vorahnung ..."); /* EN You("have an uneasy feeling..."); */
			exercise(A_WIS, FALSE);
		} else {
			if (otmp->blessed) {
				(void) adjattrib(A_INT, 1, FALSE);
				(void) adjattrib(A_WIS, 1, FALSE);
			}
			Du_fuehlst_dich("voller Selbsterkenntnis ..."); /* EN You_feel("self-knowledgeable..."); */
			display_nhwindow(WIN_MESSAGE, FALSE);
			enlightenment(0);
			pline("Das Gef¸hl l‰sst nach."); /* EN pline_The("feeling subsides."); */
			exercise(A_WIS, TRUE);
		}
		break;
	case SPE_INVISIBILITY:
		/* spell cannot penetrate mummy wrapping */
		if (BInvis && uarmc->otyp == MUMMY_WRAPPING) {
			pline("SUBJECT NOUN_IT VERB_JUCKEN OBJECT PRONOMEN_PERSONAL stark unter OBJECT KASUS_DATIV PRONOMEN_POSSESSIV %s.", xname(uarmc)); /* EN You_feel("rather itchy under your %s.", xname(uarmc)); */
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
		    pline("Aus irgendeinem Grund VERB_SPUEREN SUBJECT_IM_SATZ PRONOMEN_PERSONAL, dass NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_POSSESSIV NOUN_ANWESENHEIT bekannt VERB_SEIN."); /* EN pline("For some reason, you feel your presence is known."); */
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
		    pline("Iii!  Das schmeckt %s.", /* EN pline("Yecch!  This tastes %s.", */
			  Hallucination ? "¸berreif" : "vergammelt"); /* EN Hallucination ? "overripe" : "rotten"); */
		else
		    pline(Hallucination ?
		      "This tastes like 10%% real %s%s all-natural beverage." : /* EN "This tastes like 10%% real %s%s all-natural beverage." : */ // TODO DE
				"Das schmeckt wie %s%s.", /* EN "This tastes like %s%s.", */
			  otmp->odiluted ? "ADJEKTIV_WIEDERAUFBEREITET " : "", /* EN otmp->odiluted ? "reconstituted " : "", */
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
		    You("VERB_KOENNEN durch OBJECT PRONOMEN_PERSONAL durchsehen, aber NEUER_SATZ SUBJECT PRONOMEN_PERSONAL VERB_SEIN sichtbar!"); /* EN You("can see through yourself, but you are visible!"); */
		    unkn--;
		}
		break;
	    }
	case POT_PARALYSIS:
		if (Free_action)
		    You("VERB_VERSTEIFEN OBJECT PRONOMEN_PERSONAL f¸r einen Moment."); /* EN You("stiffen momentarily."); */
		else {
		    if (Levitation || Is_airlevel(&u.uz)||Is_waterlevel(&u.uz))
			You("VERB_HAENGEN bewegungslos in der Luft."); /* EN You("are motionlessly suspended."); */
#ifdef STEED
		    else if (u.usteed)
			You("VERB_ERSTARREN auf der Stelle!"); /* EN You("are frozen in place!"); */
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
		    You("VERB_GAEHNEN."); /* EN You("yawn."); */
		else {
		    You("VERB_EINSCHLAFEN plˆtzlich SATZKLAMMER!"); /* EN You("suddenly fall asleep!"); */
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
		    if (unkn) Du_fuehlst_dich("einsam."); /* EN if (unkn) You_feel("lonely."); */
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
		pline("Wuah!  Das Zeug schmeckt giftig."); /* EN pline("Yecch!  This stuff tastes like poison."); */
		if (otmp->blessed) {
		    pline("(Aber eigentlich war es nur leicht SUBJECT_IM_SATZ ADJEKTIV_ABGESTANDEN %s.)", /* EN pline("(But in fact it was mildly stale %s.)", */
			  fruitname(TRUE));
		    if (!Role_if(PM_HEALER)) {
			/* NB: blessed otmp->fromsink is not possible */
			losehp(1, "leicht ADJEKTIV_KONTAMINIERT NOUN_POTION", KILLED_BY_AN); /* EN losehp(1, "mildly contaminated potion", KILLED_BY_AN); */
		    }
		} else {
		    if(Poison_resistance)
			pline(
			  "(Aber eigentlich war es nur biologisch SUBJECT_IM_SATZ ADJEKTIV_KONTAMINIERT %s.)", /* EN "(But in fact it was biologically contaminated %s.)", */
			      fruitname(TRUE));
		    if (Role_if(PM_HEALER))
			pline("Zum Gl¸ck VERB_SEIN PRONOMEN_PERSONAL dagegen geimpft."); /* EN pline("Fortunately, you have been immunized."); */
		    else {
			int typ = rn2(A_MAX);

			if (!Fixed_abil) {
			    poisontell(typ);
			    (void) adjattrib(typ,
			    		Poison_resistance ? -1 : -rn1(4,3),
			    		TRUE);
			}
			if(!Poison_resistance) {
			    if (otmp->fromsink)
				losehp(rnd(10)+5*!!(otmp->cursed),
				       "ADJEKTIV_KONTAMINIERT NOUN_LEITUNGSWASSER", KILLED_BY); /* EN "contaminated tap water", KILLED_BY); */
			    else
				losehp(rnd(10)+5*!!(otmp->cursed),
				       "ADJEKTIV_KONTAMINIERT NOUN_POTION", KILLED_BY_AN); /* EN "contaminated potion", KILLED_BY_AN); */
			}
			exercise(A_CON, FALSE);
		    }
		}
		if(Hallucination) {
			You("are shocked back to your senses!"); /* EN You("are shocked back to your senses!"); */ // TODO DE
			(void) make_hallucinated(0L,FALSE,0L);
		}
		break;
	case POT_CONFUSION:
		if(!Confusion)
		    if (Hallucination) {
			pline("Was f¸r ein trippiges Gef¸hl!"); /* EN pline("What a trippy feeling!"); */
			unkn++;
		    } else
			pline("He, was?  Wo bin ich?"); /* EN pline("Huh, What?  Where am I?"); */
		else	nothing++;
		make_confused(itimeout_incr(HConfusion,
					    rn1(7, 16 - 8 * bcsign(otmp))),
			      FALSE);
		break;
	case POT_GAIN_ABILITY:
		if(otmp->cursed) {
		    pline("÷rks!  Dieser Trank schmeckt faulig!"); /* EN pline("Ulch!  That potion tasted foul!"); */
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
			You("VERB_BEWEGEN OBJECT PRONOMEN_PERSONAL plˆtzlich %sschneller.", /* EN You("are suddenly moving %sfaster.", */
				Fast ? "" : "viel "); /* EN Fast ? "" : "much "); */
		else {
			Your("%s tanken neue Energie.", /* EN Your("%s get new energy.", */
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
			    const char *riseup ="VERB_EMPORSTEIGEN SATZKLAMMER, OBJECT durch ARTIKEL_BESTIMMTER %s hindurch!"; /* EN const char *riseup ="rise up, through the %s!"; */
			    if(ledger_no(&u.uz) == 1) {
			        You(riseup, ceiling(u.ux,u.uy));
				goto_level(&earth_level, FALSE, FALSE, FALSE);
			    } else {
			        register int newlev = depth(&u.uz)-1;
				d_level newlevel;

				get_level(&newlevel, newlev);
				if(on_level(&newlevel, &u.uz)) {
				    pline("Schmeckt schlecht."); /* EN pline("It tasted bad."); */
				    break;
				} else You(riseup, ceiling(u.ux,u.uy));
				goto_level(&newlevel, FALSE, FALSE, FALSE);
			    }
			}
			else You("VERB_HAVE eine dunkle Vorahnung."); /* EN else You("have an uneasy feeling."); */
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
		Du_fuehlst_dich("besser."); /* EN You_feel("better."); */
		healup(d(6 + 2 * bcsign(otmp), 4),
		       !otmp->cursed ? 1 : 0, !!otmp->blessed, !otmp->cursed);
		exercise(A_CON, TRUE);
		break;
	case POT_EXTRA_HEALING:
		Du_fuehlst_dich("viel besser."); /* EN You_feel("much better."); */
		healup(d(6 + 2 * bcsign(otmp), 8),
		       otmp->blessed ? 5 : !otmp->cursed ? 2 : 0,
		       !otmp->cursed, TRUE);
		(void) make_hallucinated(0L,TRUE,0L);
		exercise(A_CON, TRUE);
		exercise(A_STR, TRUE);
		break;
	case POT_FULL_HEALING:
		Du_fuehlst_dich("vollst‰ndig geheilt."); /* EN You_feel("completely healed."); */
		healup(400, 4+4*bcsign(otmp), !otmp->cursed, TRUE);
		/* Restore one lost level if blessed */
		if (otmp->blessed && u.ulevel < u.ulevelmax) {
		    /* when multiple levels have been lost, drinking
		       multiple potions will only get half of them back */
		    u.ulevelmax -= 1;
		    pluslvl(FALSE);
		}
		(void) make_hallucinated(0L,TRUE,0L);
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
					You("VERB_STOSSEN OBJECT PRONOMEN_POSSESSIV %s NEUES_OBJECT OBJECT KASUS_DATIV _an_ ARTIKEL_BESTIMMTER %s.", /* EN You("hit your %s on the %s.", */
						body_part(HEAD),
						ceiling(u.ux,u.uy));
					losehp(uarmh ? 1 : rnd(10),
						"NOUN_KOLLISION mit der Decke", /* EN "colliding with the ceiling", */
						KILLED_BY_AN); /* EN KILLED_BY); */
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
			    Du_fuehlst_dich("drˆge."); /* EN You_feel("lackluster."); */
			else
			    pline("Magische Energie strˆmt KASUS_AKKUSATIV durch PRONOMEN_POSSESSIV NOUN_KOERPER."); /* EN pline("Magical energies course through your body."); */
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
				pline("Ah, ein erfrischender Trunk."); /* EN pline("Ahh, a refreshing drink."); */
				good_for_you = TRUE;
			    } else {
				You("VERB_BRENNEN OBJECT PRONOMEN_POSSESSIV %s.", body_part(FACE)); /* EN You("burn your %s.", body_part(FACE)); */
				losehp(d(Fire_resistance ? 1 : 3, 4),
				       "ADJEKTIV_BRENNEND NOUN_POTION PARTIKEL_OF NOUN_POT_OIL", KILLED_BY_AN); /* EN "burning potion of oil", KILLED_BY_AN); */
			    }
			} else if(otmp->cursed)
			    pline("Das schmeckt wie Rizinusˆl."); /* EN pline("This tastes like castor oil."); */
			else
			    pline("Das ging runter wie ÷l!"); /* EN pline("That was smooth!"); */
			exercise(A_WIS, good_for_you);
		}
		break;
	case POT_ACID:
		if (Acid_resistance)
			/* Not necessarily a creature who _likes_ acid */
			pline("Das schmeckt %s.", Hallucination ? "w¸rzig" : "sauer"); /* EN pline("This tastes %s.", Hallucination ? "tangy" : "sour"); */
		else {
			pline("Das brennt%s!", otmp->blessed ? " ein wenig" : /* EN pline("This burns%s!", otmp->blessed ? " a little" : */
					otmp->cursed ? " stark" : " wie S‰ure"); /* EN otmp->cursed ? " a lot" : " like acid"); */
			losehp(d(otmp->cursed ? 2 : 1, otmp->blessed ? 4 : 8),
					"NOUN_POTION PARTIKEL_OF NOUN_POT_ACID", KILLED_BY_AN); /* EN "potion of acid", KILLED_BY_AN); */
			exercise(A_CON, FALSE);
		}
		if (Stoned) fix_petrification();
		unkn++; /* holy/unholy water can burn like acid too */
		break;
	case POT_POLYMORPH:
		Du_fuehlst_dich("etwas %s.", Hallucination ? "normal" : "seltsam"); /* EN You_feel("a little %s.", Hallucination ? "normal" : "strange"); */
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
		pline("Ein %s Gef¸hl ¸berkommt KASUS_AKKUSATIV PRONOMEN_PERSONAL plˆtzlich, doch es vergeht wieder.", /* EN You("have a %s feeling for a moment, then it passes.", */
		Hallucination ? "normales" : "seltsames"); /* EN Hallucination ? "normal" : "strange"); */
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
	"NOUN_POTION", "NOUN_FLAKON", "NOUN_KRUG", "NOUN_KARAFFE", "NOUN_KOLBEN", "NOUN_FLAESCHCHEN", "NOUN_PHIOLE" /* EN "bottle", "phial", "flagon", "carafe", "flask", "jar", "vial" */
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
		pline_The("%s VERB_KRACHEN OBJECT auf PRONOMEN_POSSESSIV %s und zerplatzt in viele kleine Glasscherben.", /* EN pline_The("%s crashes on your %s and breaks into shards.", */
			botlnam, body_part(HEAD));
		losehp(rnd(2), "ADJEKTIV_GEWORFEN NOUN_POTION", KILLED_BY_AN); /* EN losehp(rnd(2), "thrown potion", KILLED_BY_AN); */
	} else {
		distance = distu(mon->mx,mon->my);
		if (!cansee(mon->mx,mon->my)) pline("Klirr!"); /* EN if (!cansee(mon->mx,mon->my)) pline("Crash!"); */
		else {
		    char *mnam = mon_nam(mon);
		    char buf[BUFSZ];

		    if(has_head(mon->data)) {
			Sprintf(buf, "%s", /* EN Sprintf(buf, "%s %s", */
				genitivattribut_zu_wort((mnam), /* EN s_suffix(mnam), */
				(notonhead ? "NOUN_BODY" : "NOUN_HEAD"))); /* EN (notonhead ? "body" : "head")); */
		    } else {
			Strcpy(buf, mnam);
		    }
		    pline_The("%s VERB_KRACHEN OBJECT auf %s und zerplatzt in viele kleine Glasscherben.", /* EN pline_The("%s crashes on %s and breaks into shards.", */
			   botlnam, buf);
		}
		if(rn2(5) && mon->mhp > 1)
			mon->mhp--;
	}

	/* oil doesn't instantly evaporate */
	if (obj->otyp != POT_OIL && cansee(mon->mx,mon->my))
		pline("SUBJECT %s sich.", Tobjnam(obj, "VERB_VERFLUECHTIGEN")); /* EN pline("%s.", Tobjnam(obj, "evaporate")); */

    if (isyou) {
	switch (obj->otyp) {
	case POT_OIL:
		if (obj->lamplit)
		    splatter_burning_oil(u.ux, u.uy);
		break;
	case POT_POLYMORPH:
		Du_fuehlst_dich("etwas %s.", Hallucination ? "normal" : "seltsam"); /* EN You_feel("a little %s.", Hallucination ? "normal" : "strange"); */
		if (!Unchanging && !Antimagic) polyself(FALSE);
		break;
	case POT_ACID:
		if (!Acid_resistance) {
		    pline("Das brennt%s!", obj->blessed ? " ein wenig" : /* EN pline("This burns%s!", obj->blessed ? " a little" : */
				    obj->cursed ? " stark" : ""); /* EN obj->cursed ? " a lot" : ""); */
		    losehp(d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8),
				    "NOUN_POTION PARTIKEL_OF NOUN_POT_ACID", KILLED_BY_AN); /* EN "potion of acid", KILLED_BY_AN); */
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
			pline("SUBJECT %s VERB_AUSSEHEN wieder gesund und munter SATZKLAMMER.", Monnam(mon)); /* EN pline("%s looks sound and hale again.", Monnam(mon)); */
		}
		break;
	case POT_SICKNESS:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_healing;
		if (dmgtype(mon->data, AD_DISE) ||
			   dmgtype(mon->data, AD_PEST) || /* won't happen, see prior goto */
			   resists_poison(mon)) {
		    if (canseemon(mon))
			pline("SUBJECT %s VERB_AUSSEHEN unverletzt SATZKLAMMER.", Monnam(mon)); /* EN pline("%s looks unharmed.", Monnam(mon)); */
		    break;
		}
 do_illness:
		if((mon->mhpmax > 3) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhpmax /= 2;
		if((mon->mhp > 2) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhp /= 2;
		if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
		if (canseemon(mon))
		    pline("SUBJECT %s VERB_AUSSEHEN ziemlich krank SATZKLAMMER.", Monnam(mon)); /* EN pline("%s looks rather ill.", Monnam(mon)); */
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
		    pline("SUBJECT %s VERB_EINSCHLAFEN SATZKLAMMER.", Monnam(mon)); /* EN pline("%s falls asleep.", Monnam(mon)); */
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
			pline("SUBJECT %s %s vor Schmerzen!", Monnam(mon), /* EN pline("%s %s in pain!", Monnam(mon), */
						is_silent(mon->data) ? "VERB_WINDEN" : "VERB_SCHREIEN"); /* EN is_silent(mon->data) ? "writhes" : "shrieks"); */
			mon->mhp -= d(2,6);
			/* should only be by you */
			if (mon->mhp < 1) killed(mon);
			else if (is_were(mon->data) && !is_human(mon->data))
			    new_were(mon);	/* revert to human */
		    } else if (obj->cursed) {
			angermon = FALSE;
			if (canseemon(mon))
			    pline("SUBJECT %s VERB_AUSSEHEN ges¸nder SATZKLAMMER.", Monnam(mon)); /* EN pline("%s looks healthier.", Monnam(mon)); */
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
			pline("SUBJECT %s VERB_ROSTEN.", Monnam(mon)); /* EN pline("%s rusts.", Monnam(mon)); */
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
		    pline("SUBJECT %s %s vor Schmerzen!", Monnam(mon), /* EN pline("%s %s in pain!", Monnam(mon), */
			  is_silent(mon->data) ? "VERB_WINDEN" : "VERB_SCHREIEN"); /* EN is_silent(mon->data) ? "writhes" : "shrieks"); */
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
			pline("÷rks!  Dieser Trank riecht schrecklich!"); /* EN pline("Ulch!  That potion smells terrible!"); */
		    else if (haseyes(youmonst.data)) {
			int numeyes = eyecount(youmonst.data);
			Your("%s VERB_BRENNEN%s!", /* EN Your("%s sting%s!", */
			     (numeyes == 1) ? body_part(EYE) : makeplural(body_part(EYE)),
			     (numeyes == 1) ? "" : ""); /* EN (numeyes == 1) ? "s" : ""); */
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
		You("VERB_HABEN eine kurze Vision."); /* EN You("have a momentary vision."); */
		break;
	case POT_CONFUSION:
	case POT_BOOZE:
		if(!Confusion)
			Dir_ist("etwas schwindlig."); /* EN You_feel("somewhat dizzy."); */
		make_confused(itimeout_incr(HConfusion, rnd(5)), FALSE);
		break;
	case POT_INVISIBILITY:
		if (!Blind && !Invis) {
		    kn++;
		    pline("Eine Sekunde lang SUBJECT_IM_SATZ MODIFIER_VERB_PRAETERITUM VERB_KOENNEN PRONOMEN_PERSONAL %s!", /* EN pline("For an instant you %s!", */
			See_invisible ? "einfach durch OBJECT PRONOMEN_PERSONAL durchsehen" /* EN See_invisible ? "could see right through yourself" */
			: "OBJECT PRONOMEN_PERSONAL nicht sehen"); /* EN : "couldn't see yourself"); */
		}
		break;
	case POT_PARALYSIS:
		kn++;
		if (!Free_action) {
		    pline("SUBJECT %s VERB_SCHEINEN OBJECT PRONOMEN_PERSONAL festzuhalten.", Something); /* EN pline("%s seems to be holding you.", Something); */
		    nomul(-rnd(5));
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("VERB_VERSTEIFEN OBJECT PRONOMEN_PERSONAL f¸r einen Moment."); /* EN } else You("stiffen momentarily."); */
		break;
	case POT_SLEEPING:
		kn++;
		if (!Free_action && !Sleep_resistance) {
		    Du_fuehlst_dich("ziemlich m¸de."); /* EN You_feel("rather tired."); */
		    nomul(-rnd(5));
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		} else You("VERB_GAEHNEN."); /* EN } else You("yawn."); */
		break;
	case POT_SPEED:
		if (!Fast) Your("NOUN_KNIEs VERB_SCHEINEN jetzt gelenkiger."); /* EN if (!Fast) Your("knees seem more flexible now."); */
		incr_itimeout(&HFast, rnd(5));
		exercise(A_DEX, TRUE);
		break;
	case POT_BLINDNESS:
		if (!Blind && !u.usleep) {
		    kn++;
		    pline("Es wird plˆtzlich dunkel."); /* EN pline("It suddenly gets dark."); */
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
	    case POTION_CLASS:
		if (obj->otyp == POT_WATER) return FALSE;
		/* KMH -- Water into acid causes an explosion */
		if (obj->otyp == POT_ACID) {
			pline("Es brodelt heftig!"); /* EN pline("It boils vigorously!"); */
			You("VERB_WERDEN von einer Explosion erfasst!"); /* EN You("are caught in the explosion!"); */
			losehp(rnd(10), "Grundschul-NOUN_CHEMIE", KILLED_BY); /* EN losehp(rnd(10), "elementary chemistry", KILLED_BY); */
			makeknown(obj->otyp);
			update_inventory();
			return (TRUE);
		}
		pline("SUBJECT %s %s%s sich.", Your_buf, aobjnam(obj,"VERB_VERDUENNEN"), /* EN pline("%s %s%s.", Your_buf, aobjnam(obj,"dilute"), */ 
		      obj->odiluted ? " noch st‰rker" : ""); /* EN obj->odiluted ? " further" : ""); */
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
				pline_The("NOUN_SCROLL%s %s.", /* EN pline_The("scroll%s %s.", */
						oq1 ? "" : "s", otense(obj, "VERB_VERBLASSEN")); /* EN oq1 ? "" : "s", otense(obj, "fade")); */
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
	pline("SUBJECT %s erhitzt sich plˆtzlich! Dampf steigt auf und es bleibt trocken.", /* EN pline("%s suddenly heats up; steam rises and it remains dry.", */
				The(xname(obj)));
			} else {
			    if (!Blind) {
				    boolean oq1 = obj->quan == 1L;
				    pline_The("NOUN_SPELLBOOK%s %s.", /* EN pline_The("spellbook%s %s.", */
					oq1 ? "" : "s", otense(obj, "VERB_VERBLASSEN")); /* EN oq1 ? "" : "s", otense(obj, "fade")); */
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
		break;
	    case WEAPON_CLASS:
	    /* Just "fall through" to generic rustprone check for now. */
	    /* fall through */
	    default:
		if (!obj->oerodeproof && is_rustprone(obj) &&
		    (obj->oeroded < MAX_ERODE) && !rn2(2)) {
			pline("SUBJECT %s %s %s.", /* EN pline("%s %s some%s.", */
			      Your_buf, aobjnam(obj, "VERB_ROSTEN"), /* EN Your_buf, aobjnam(obj, "rust"), */
			      obj->oeroded ? "noch mehr" : "ein wenig"); /* EN obj->oeroded ? " more" : "what"); */
			obj->oeroded++;
			update_inventory();
			return TRUE;
		} else break;
	}
	pline("SUBJECT %s %s nass.", Your_buf, aobjnam(obj,"VERB_WERDEN")); /* EN pline("%s %s wet.", Your_buf, aobjnam(obj,"get")); */
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
	if(!(obj = getobj(allowall, "VERB_DIP"))) /* EN if(!(obj = getobj(allowall, "dip"))) */ // TODO DE
		return(0);

	here = levl[u.ux][u.uy].typ;
	/* Is there a fountain to dip into here? */
	if (IS_FOUNTAIN(here)) {
		if(yn("In OBJECT KASUS_AKKUSATIV ARTIKEL_BESTIMMTER NOUN_FOUNTAIN MODIFIER_VERB_INFINITIV VERB_DIP?") == 'y') { /* EN if(yn("Dip it into the fountain?") == 'y') { */ // TODO DE
			dipfountain(obj);
			return(1);
		}
	} else if (is_pool(u.ux,u.uy)) {
		tmp = waterbody_name(u.ux,u.uy);
		Sprintf(qbuf, "In OBJECT KASUS_AKKUSATIV ARTIKEL_BESTIMMTER %s MODIFIER_VERB_INFINITIV VERB_DIP?", tmp); /* EN Sprintf(qbuf, "Dip it into the %s?", tmp); */ // TODO DE
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
		pline("Das ist eine gewˆhnliche Flasche, keine Kleinsche Flasche!"); /* EN pline("That is a potion bottle, not a Klein bottle!"); */
		return 0;
	}
	potion->in_use = TRUE;		/* assume it will be used up */
	if(potion->otyp == POT_WATER) {
		boolean useeit = !Blind;
		if (useeit) (void) Shk_Your(Your_buf, obj);
		if (potion->blessed) {
			if (obj->cursed) {
				if (useeit)
				    pline("SUBJECT %s %s %s.", /* EN pline("%s %s %s.", */
					  Your_buf,
					  aobjnam(obj, "VERB_LEUCHTEN schwach"), /* EN aobjnam(obj, "softly glow"), */
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
				    pline("SUBJECT %s %s OBJECT KASUS_DATIV mit ARTIKEL_UNBESTIMMTER %s NOUN_AURA.", /* EN pline("%s %s with a%s %s aura.", */
					  Your_buf,
					  aobjnam(obj, "VERB_LEUCHTEN schwach"), /* EN aobjnam(obj, "softly glow"), */
					  tmp); /* EN index(vowels, *tmp) ? "n" : "", tmp); */
				}
				bless(obj);
				obj->bknown=1;
				goto poof;
			}
		} else if (potion->cursed) {
			if (obj->blessed) {
				if (useeit)
				    pline("SUBJECT %s %s %s.", /* EN pline("%s %s %s.", */
					  Your_buf,
					  aobjnam(obj, "VERB_LEUCHTEN"), /* EN aobjnam(obj, "glow"), */
					  hcolor((const char *)"ADJEKTIV_FARBE_BRAUN")); /* EN hcolor((const char *)"brown")); */
				unbless(obj);
				obj->bknown=1;
				goto poof;
			} else if(!obj->cursed) {
				if (useeit) {
				    tmp = hcolor(NH_BLACK);
				    pline("SUBJECT %s %s OBJECT KASUS_DATIV mit ARTIKEL_UNBESTIMMTER %s NOUN_AURA.", /* EN pline("%s %s with a%s %s aura.", */
					  Your_buf,
					  aobjnam(obj, "VERB_LEUCHTEN"), /* EN aobjnam(obj, "glow"), */
					  tmp); /* EN index(vowels, *tmp) ? "n" : "", tmp); */
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
			prinv((char *)0, obj, 0L, (char *)0);
			return 1;
		} else {
			pline("Nichts scheint zu passieren."); /* EN pline("Nothing seems to happen."); */
			goto poof;
		}
	    }
	    potion->in_use = FALSE;	/* didn't go poof */
	    return(1);
	} else if(obj->oclass == POTION_CLASS && obj->otyp != potion->otyp) {
		/* Mixing potions is dangerous... */
		pline_The("NOUN_POTIONs VERB_VERMISCHEN sich ..."); /* EN pline_The("potions mix..."); */
		/* KMH, balance patch -- acid is particularly unstable */
		if (obj->cursed || obj->otyp == POT_ACID || !rn2(10)) {
			pline("BUMM!  Sie explodieren!"); /* EN pline("BOOM!  They explode!"); */
			exercise(A_STR, FALSE);
			if (!breathless(youmonst.data) || haseyes(youmonst.data))
				potionbreathe(obj);
			useup(obj);
			useup(potion);
			losehp(rnd(10), "ADJEKTIV_ALCHEMISTISCH NOUN_ENTLADUNG", KILLED_BY_AN); /* EN losehp(rnd(10), "alchemic blast", KILLED_BY_AN); */
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
			  pline("Das Gemisch leuchtet gleiﬂend hell auf und verdampft."); /* EN pline_The("mixture glows brightly and evaporates."); */
				useup(obj);
				useup(potion);
				return(1);
		    }
		}

		obj->odiluted = (obj->otyp != POT_WATER);

		if (obj->otyp == POT_WATER && !Hallucination) {
			pline("Das Gemisch blubbert%s.", /* EN pline_The("mixture bubbles%s.", */
				Blind ? "" : " und kl‰rt sich schlieﬂlich"); /* EN Blind ? "" : ", then clears"); */
		} else if (!Blind) {
			pline("Das Gemisch sieht %s aus.", /* EN pline_The("mixture looks %s.", */
				hcolor(OBJ_DESCR(objects[obj->otyp])));
		}

		useup(potion);
		return(1);
	}

#ifdef INVISIBLE_OBJECTS
	if (potion->otyp == POT_INVISIBILITY && !obj->oinvis) {
		obj->oinvis = TRUE;
		if (!Blind) {
		    if (!See_invisible) pline("Wohin ist %s verschwunden?", /* EN if (!See_invisible) pline("Where did %s go?", */
		    		the(xname(obj)));
		    else You("VERB_BEMERKEN eine leichte Unsch‰rfe OBJECT um %s.", /* EN else You("notice a little haziness around %s.", */
		    		the(xname(obj)));
		}
		goto poof;
	} else if (potion->otyp == POT_SEE_INVISIBLE && obj->oinvis) {
		obj->oinvis = FALSE;
		if (!Blind) {
		    if (!See_invisible) pline("Dahin ist also %s verschwunden!", /* EN if (!See_invisible) pline("So that's where %s went!", */
		    		the(xname(obj)));
		    else pline("Die Unsch‰rfe um KASUS_AKKUSATIV %s verschwindet.", /* EN else pline_The("haziness around %s disappears.", */
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
		pline("SUBJECT %s VERB_BILDEN eine Schicht OBJECT KASUS_DATIV auf %s.", /* EN pline("%s forms a coating on %s.", */
		      buf, the(xname(obj)));
		obj->opoisoned = TRUE;
		goto poof;
	    } else if(obj->opoisoned &&
		      (potion->otyp == POT_HEALING ||
		       potion->otyp == POT_EXTRA_HEALING ||
		       potion->otyp == POT_FULL_HEALING)) {
		pline("Die Schicht auf KASUS_DATIV %s verliert sich.", the(xname(obj))); /* EN pline("A coating wears off %s.", the(xname(obj))); */
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
		    pline("SUBJECT %s %s f¸r einen Moment zu brennen.", /* EN pline("%s %s to burn for a moment.", */
			  Yname2(obj), otense(obj, "VERB_SCHEINEN")); /* EN Yname2(obj), otense(obj, "seem")); */
		} else {
		    if ((omat == PLASTIC || omat == PAPER) && !obj->oartifact)
			obj->oeroded = MAX_ERODE;
		    pline("Das brennende ÷l %s KASUS_AKKUSATIV %s.", /* EN pline_The("burning oil %s %s.", */
			    obj->oeroded == MAX_ERODE ? "zerstˆrt" : "besch‰digt", /* EN obj->oeroded == MAX_ERODE ? "destroys" : "damages", */
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
		You("VERB_VERSCHUETTEN OBJECT ARTIKEL_BESTIMMTER NOUN_POTION und VERB_VERSCHMIEREN NEUES_OBJECT OBJECT PRONOMEN_POSSESSIV %s mit ÷l.", /* EN pline_The("potion spills and covers your %s with oil.", */
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
		pline("%s %s in einem ˆligen Glanz.", /* EN pline("%s %s with an oily sheen.", */
		      Yname2(obj), otense(obj, "VERB_SCHIMMERN")); /* EN Yname2(obj), otense(obj, "gleam")); */
	    } else {
		pline("SUBJECT %s %s weniger %s.", /* EN pline("%s %s less %s.", */
		      Yname2(obj), otense(obj, "VERB_SEIN"), /* EN Yname2(obj), otense(obj, "are"), */
		      (obj->oeroded && obj->oeroded2) ? "ADJEKTIV_CORRODED und ADJEKTIV_ROSTIG" : /* EN (obj->oeroded && obj->oeroded2) ? "corroded and rusty" : */
			obj->oeroded ? "ADJEKTIV_ROSTIG" : "ADJEKTIV_CORRODED"); /* EN obj->oeroded ? "rusty" : "corroded"); */
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
		pline("SUBJECT %s %s voll.", Yname2(obj), otense(obj, "VERB_SEIN")); /* EN pline("%s %s full.", Yname2(obj), otense(obj, "are")); */
		potion->in_use = FALSE;	/* didn't go poof */
	    } else {
		You("VERB_FUELLEN OBJECT %s mit ÷l.", yname(obj)); /* EN You("fill %s with oil.", yname(obj)); */
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
			Sprintf(newbuf, "VERB_KLAEREN sich"); /* EN Sprintf(newbuf, "clears"); */
		    else
			Sprintf(newbuf, "VERB_WERDEN %s", /* EN Sprintf(newbuf, "turns %s", */
				hcolor(OBJ_DESCR(objects[mixture])));
		    pline_The("%sNOUN_POTION%s %s.", oldbuf, /* EN pline_The("%spotion%s %s.", oldbuf, */
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
					"SUBJECT PRONOMEN_PERSONAL VERB_JONGLIEREN und VERB_DROP OBJECT %s SATZKLAMMER!", /* EN "You juggle and drop %s!", */
					doname(singlepotion), (const char *)0);
		update_inventory();
		return(1);
	}

	pline("Interessant ..."); /* EN pline("Interesting..."); */
	return(1);
}


void
djinni_from_bottle(obj)
register struct obj *obj;
{
	struct monst *mtmp;
	int chance;

	if(!(mtmp = makemon(&mons[PM_DJINNI], u.ux, u.uy, NO_MM_FLAGS))){
		pline("Es stellt sich als leer heraus."); /* EN pline("It turns out to be empty."); */
		return;
	}

	if (!Blind) {
		pline("Einer dichten Rauchwolke entsteigt %s!", a_monnam(mtmp)); /* EN pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp)); */
		pline("SUBJECT %s VERB_SPRECHEN.", Monnam(mtmp)); /* EN pline("%s speaks.", Monnam(mtmp)); */
	} else {
		You("VERB_SMELL beiﬂenden Qualm."); /* EN You("smell acrid fumes."); */
		pline("SUBJECT %s VERB_SPRECHEN.", "NOUN_JEMAND"); /* EN pline("%s speaks.", Something); */
	}

	chance = rn2(5);
	if (obj->blessed) chance = (chance == 4) ? rnd(4) : 0;
	else if (obj->cursed) chance = (chance == 0) ? rn2(4) : 4;
	/* 0,1,2,3,4:  b=80%,5,5,5,5; nc=20%,20,20,20,20; c=5%,5,5,5,80 */

	switch (chance) {
	case 0 : verbalize("Ich stehe tief in KASUS_DATIV PRONOMEN_POSSESSIV NOUN_SCHULD. NEUER_SATZ Ich gew‰hre KASUS_DATIV PRONOMEN_PERSONAL einen Wunsch!"); /* EN case 0 : verbalize("I am in your debt.  I will grant one wish!"); */
		makewish();
		mongone(mtmp);
		break;
	case 1 : verbalize("Ich danke KASUS_DATIV PRONOMEN_PERSONAL f¸r meine Befreiung!"); /* EN case 1 : verbalize("Thank you for freeing me!"); */
		(void) tamedog(mtmp, (struct obj *)0);
		break;
	case 2 : verbalize("SUBJECT PRONOMEN_PERSONAL VERB_HABEN mich befreit!"); /* EN case 2 : verbalize("You freed me!"); */
		mtmp->mpeaceful = TRUE;
		set_malign(mtmp);
		break;
	case 3 : verbalize("Wird auch langsam Zeit!"); /* EN case 3 : verbalize("It is about time!"); */
		pline("SUBJECT %s VERB_VERSCHWINDEN.", Monnam(mtmp)); /* EN pline("%s vanishes.", Monnam(mtmp)); */
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
	if (mtmp) Sprintf(reason, " NEUES_OBJECT OBJECT durch %s", /* EN if (mtmp) Sprintf(reason, " from %s heat", */
			  (mtmp == &youmonst) ? (const char *)"PRONOMEN_POSSESSIV NOUN_WAERME" : /* EN (mtmp == &youmonst) ? (const char *)"your" : */
			      (const char *)genitivattribut_zu_wort(mon_nam(mtmp),"NOUN_WAERME")); /* EN (const char *)s_suffix(mon_nam(mtmp))); */

	if (mon == &youmonst) {
	    mtmp2 = cloneu();
	    if (mtmp2) {
		mtmp2->mhpmax = u.mhmax / 2;
		u.mhmax -= mtmp2->mhpmax;
		flags.botl = 1;
		You("VERB_VERMEHREN OBJECT PRONOMEN_PERSONAL%s!", reason); /* EN You("multiply%s!", reason); */
	    }
	} else {
	    mtmp2 = clone_mon(mon, 0, 0);
	    if (mtmp2) {
		mtmp2->mhpmax = mon->mhpmax / 2;
		mon->mhpmax -= mtmp2->mhpmax;
		if (canspotmon(mon))
		    pline("SUBJECT %s VERB_VERMEHREN sich%s!", Monnam(mon), reason); /* EN pline("%s multiplies%s!", Monnam(mon), reason); */
	    }
	}
	return mtmp2;
}

#endif /* OVLB */

/*potion.c*/
