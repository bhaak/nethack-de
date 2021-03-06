/*	SCCS Id: @(#)polyself.c	3.4	2003/01/08	*/
/*	Copyright (C) 1987, 1988, 1989 by Ken Arromdee */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Polymorph self routine.
 *
 * Note:  the light source handling code assumes that both youmonst.m_id
 * and youmonst.mx will always remain 0 when it handles the case of the
 * player polymorphed into a light-emitting monster.
 */

#include "hack.h"

#ifdef GERMAN
# include "german.h"
#endif

#ifdef OVLB
STATIC_DCL void FDECL(polyman, (const char *,const char *));
STATIC_DCL void NDECL(break_armor);
STATIC_DCL void FDECL(drop_weapon,(int));
STATIC_DCL void NDECL(uunstick);
STATIC_DCL int FDECL(armor_to_dragon,(int));
STATIC_DCL void NDECL(newman);

/* update the youmonst.data structure pointer */
void
set_uasmon()
{
	set_mon_data(&youmonst, &mons[u.umonnum], 0);
}

/* make a (new) human out of the player */
STATIC_OVL void
polyman(fmt, arg)
const char *fmt, *arg;
{
	boolean sticky = sticks(youmonst.data) && u.ustuck && !u.uswallow,
		was_mimicking = (youmonst.m_ap_type == M_AP_OBJECT);
	boolean could_pass_walls = Passes_walls;
	boolean was_blind = !!Blind;

	if (Upolyd) {
		u.acurr = u.macurr;	/* restore old attribs */
		u.amax = u.mamax;
		u.umonnum = u.umonster;
		flags.female = u.mfemale;
	}
	set_uasmon();

	u.mh = u.mhmax = 0;
	u.mtimedone = 0;
	skinback(FALSE);
	u.uundetected = 0;

	if (sticky) uunstick();
	find_ac();
	if (was_mimicking) {
	    if (multi < 0) unmul("");
	    youmonst.m_ap_type = M_AP_NOTHING;
	}

	newsym(u.ux,u.uy);

	You(fmt, arg);
	/* check whether player foolishly genocided self while poly'd */
	if ((mvitals[urole.malenum].mvflags & G_GENOD) ||
			(urole.femalenum != NON_PM &&
			(mvitals[urole.femalenum].mvflags & G_GENOD)) ||
			(mvitals[urace.malenum].mvflags & G_GENOD) ||
			(urace.femalenum != NON_PM &&
			(mvitals[urace.femalenum].mvflags & G_GENOD))) {
	    /* intervening activity might have clobbered genocide info */
	    killer = delayed_killer;
	    if (!killer || !strstri(killer, "genocid")) {
		killer_format = KILLED_BY;
		killer = "NOUN_SELF_GENOCIDE"; /* EN killer = "self-genocide"; */
	    }
	    done(GENOCIDED);
	}

	if (u.twoweap && !could_twoweap(youmonst.data))
	    untwoweapon();

	if (u.utraptype == TT_PIT) {
	    if (could_pass_walls) {	/* player forms cannot pass walls */
		u.utrap = rn1(6,2);
	    }
	}
	if (was_blind && !Blind) {	/* reverting from eyeless */
	    Blinded = 1L;
	    make_blinded(0L, TRUE);	/* remove blindness */
	}

	if(!Levitation && !u.ustuck &&
	   (is_pool(u.ux,u.uy) || is_lava(u.ux,u.uy)))
		spoteffects(TRUE);

	see_monsters();
}

void
change_sex()
{
	/* setting u.umonster for caveman/cavewoman or priest/priestess
	   swap unintentionally makes `Upolyd' appear to be true */
	boolean already_polyd = (boolean) Upolyd;

	/* Some monsters are always of one sex and their sex can't be changed */
	/* succubi/incubi can change, but are handled below */
	/* !already_polyd check necessary because is_male() and is_female()
           are true if the player is a priest/priestess */
	if (!already_polyd || (!is_male(youmonst.data) && !is_female(youmonst.data) && !is_neuter(youmonst.data)))
	    flags.female = !flags.female;
	if (already_polyd)	/* poly'd: also change saved sex */
	    u.mfemale = !u.mfemale;
	max_rank_sz();		/* [this appears to be superfluous] */
	if ((already_polyd ? u.mfemale : flags.female) && urole.name.f)
	    Strcpy(pl_character, urole.name.f);
	else
	    Strcpy(pl_character, urole.name.m);
	u.umonster = ((already_polyd ? u.mfemale : flags.female) && urole.femalenum != NON_PM) ?
			urole.femalenum : urole.malenum;
	if (!already_polyd) {
	    u.umonnum = u.umonster;
	} else if (u.umonnum == PM_SUCCUBUS || u.umonnum == PM_INCUBUS) {
	    flags.female = !flags.female;
	    /* change monster type to match new sex */
	    u.umonnum = (u.umonnum == PM_SUCCUBUS) ? PM_INCUBUS : PM_SUCCUBUS;
	    set_uasmon();
	}
}

STATIC_OVL void
newman()
{
	int tmp, oldlvl;

	tmp = u.uhpmax;
	oldlvl = u.ulevel;
	u.ulevel = u.ulevel + rn1(5, -2);
	if (u.ulevel > 127 || u.ulevel < 1) { /* level went below 0? */
	    u.ulevel = oldlvl; /* restore old level in case they lifesave */
	    goto dead;
	}
	if (u.ulevel > MAXULEV) u.ulevel = MAXULEV;
	/* If your level goes down, your peak level goes down by
	   the same amount so that you can't simply use blessed
	   full healing to undo the decrease.  But if your level
	   goes up, your peak level does *not* undergo the same
	   adjustment; you might end up losing out on the chance
	   to regain some levels previously lost to other causes. */
	if (u.ulevel < oldlvl) u.ulevelmax -= (oldlvl - u.ulevel);
	if (u.ulevelmax < u.ulevel) u.ulevelmax = u.ulevel;

	if (!rn2(10)) change_sex();

	adjabil(oldlvl, (int)u.ulevel);
	reset_rndmonst(NON_PM);	/* new monster generation criteria */

	/* random experience points for the new experience level */
	u.uexp = rndexp(FALSE);

	/* u.uhpmax * u.ulevel / oldlvl: proportionate hit points to new level
	 * -10 and +10: don't apply proportionate HP to 10 of a starting
	 *   character's hit points (since a starting character's hit points
	 *   are not on the same scale with hit points obtained through level
	 *   gain)
	 * 9 - rn2(19): random change of -9 to +9 hit points
	 */
#ifndef LINT
	u.uhpmax = ((u.uhpmax - 10) * (long)u.ulevel / oldlvl + 10) +
		(9 - rn2(19));
#endif

#ifdef LINT
	u.uhp = u.uhp + tmp;
#else
	u.uhp = u.uhp * (long)u.uhpmax/tmp;
#endif

	tmp = u.uenmax;
#ifndef LINT
	u.uenmax = u.uenmax * (long)u.ulevel / oldlvl + 9 - rn2(19);
#endif
	if (u.uenmax < 0) u.uenmax = 0;
#ifndef LINT
	u.uen = (tmp ? u.uen * (long)u.uenmax / tmp : u.uenmax);
#endif

	redist_attr();
	u.uhunger = rn1(500,500);
	if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
	Stoned = 0;
	delayed_killer = 0;
	if (u.uhp <= 0 || u.uhpmax <= 0) {
		if (Polymorph_control) {
		    if (u.uhp <= 0) u.uhp = 1;
		    if (u.uhpmax <= 0) u.uhpmax = 1;
		} else {
dead: /* we come directly here if their experience level went to 0 or less */
		    Your("ADJEKTIV_NEU NOUN_GESTALT VERB_SCHEINEN nicht ausreichend lebensf�hig zu sein."); /* EN Your("new form doesn't seem healthy enough to survive."); */
		    killer_format = KILLED_BY_AN;
		    killer="ADJEKTIV_GESCHEITERT NOUN_TRANSFORMATION"; /* EN killer="unsuccessful polymorph"; */
		    done(DIED);
		    newuhs(FALSE);
		    return; /* lifesaved */
		}
	}
	newuhs(FALSE);
	polyman("VERB_FUEHLEN OBJECT PRONOMEN_PERSONAL wie NEUES_OBJECT OBJECT KASUS_NOMINATIV ARTIKEL_UNBESTIMMTER ADJEKTIV_NEU %s!", /* EN polyman("feel like a new %s!", */
		(flags.female && urace.individual.f) ? urace.individual.f :
		(urace.individual.m) ? urace.individual.m : urace.noun);
	if (Slimed) {
		Your("NOUN_KOERPER VERB_TRANSFORMIEREN sich, aber NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL VERB_HABEN immer noch Schleim auf OBJECT PRONOMEN_PERSONAL."); /* EN Your("body transforms, but there is still slime on you."); */
		Slimed = 10L;
	}
	flags.botl = 1;
	see_monsters();
	(void) encumber_msg();
}

void
polyself(forcecontrol)
boolean forcecontrol;     
{
	char buf[BUFSZ];
	int old_light, new_light;
	int mntmp = NON_PM;
	int tries=0;
	boolean draconian = (uarm &&
				uarm->otyp >= GRAY_DRAGON_SCALE_MAIL &&
				uarm->otyp <= YELLOW_DRAGON_SCALES);
	boolean iswere = (u.ulycn >= LOW_PM || is_were(youmonst.data));
	boolean isvamp = (youmonst.data->mlet == S_VAMPIRE || u.umonnum == PM_VAMPIRE_BAT);
	boolean was_floating = (Levitation || Flying);

        if(!Polymorph_control && !forcecontrol && !draconian && !iswere && !isvamp) {
	    if (rn2(20) > ACURR(A_CON)) {
		You(shudder_for_moment);
		losehp(rnd(30), "durch Kreislaufversagen", KILLED_WITHOUT_PREPOSITION); /* EN losehp(rnd(30), "system shock", KILLED_BY_AN); */
		exercise(A_CON, FALSE);
		return;
	    }
	}
	old_light = Upolyd ? emits_light(youmonst.data) : 0;

	if (Polymorph_control || forcecontrol) {
		do {
			getlin("SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_WAEHLEN OBJECT PRONOMEN_POSSESSIV NOUN_DASEINSFORM. [NEUER_SATZ MODIFIER_VERB_IMPERATIV VERB_EINGEBEN den Namen SATZKLAMMER]", /* EN getlin("Become what kind of monster? [type the name]", */
				buf);
			mntmp = name_to_mon(buf);
			if (mntmp < LOW_PM)
				pline("Ich habe noch nie von solchen Wesen geh�rt."); /* EN pline("I've never heard of such monsters."); */
			/* Note:  humans are illegal as monsters, but an
			 * illegal monster forces newman(), which is what we
			 * want if they specified a human.... */
			else if (!polyok(&mons[mntmp]) && !your_race(&mons[mntmp]))
				You("VERB_KOENNEN OBJECT PRONOMEN_PERSONAL nicht in sowas transformieren."); /* EN You("cannot polymorph into that."); */
			else break;
		} while(++tries < 5);
		if (tries==5) pline(thats_enough_tries);
		/* allow skin merging, even when polymorph is controlled */
		if (draconian &&
		    (mntmp == armor_to_dragon(uarm->otyp) || tries == 5))
		    goto do_merge;
	} else if (draconian || iswere || isvamp) {
		/* special changes that don't require polyok() */
		if (draconian) {
		    do_merge:
			mntmp = armor_to_dragon(uarm->otyp);
			if (!(mvitals[mntmp].mvflags & G_GENOD)) {
				/* allow G_EXTINCT */
				You("VERB_VERSCHMELZEN OBJECT KASUS_DATIV mit PRONOMEN_POSSESSIV NOUN_SCHUPPENRUESTUNG."); /* EN You("merge with your scaly armor."); */
				uskin = uarm;
				uarm = (struct obj *)0;
				/* save/restore hack */
				uskin->owornmask |= I_SPECIAL;
			}
		} else if (iswere) {
			if (is_were(youmonst.data))
				mntmp = PM_HUMAN; /* Illegal; force newman() */
			else
				mntmp = u.ulycn;
		} else {
			if (youmonst.data->mlet == S_VAMPIRE)
				mntmp = PM_VAMPIRE_BAT;
			else
				mntmp = PM_VAMPIRE;
		}
		/* if polymon fails, "you feel" message has been given
		   so don't follow up with another polymon or newman */
		if (mntmp == PM_HUMAN) newman();	/* werecritter */
		else (void) polymon(mntmp);
		goto made_change;    /* maybe not, but this is right anyway */
	}

	if (mntmp < LOW_PM) {
		tries = 0;
		do {
			/* randomly pick an "ordinary" monster */
			mntmp = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
		} while((!polyok(&mons[mntmp]) || is_placeholder(&mons[mntmp]))
				&& tries++ < 200);
	}

	/* The below polyok() fails either if everything is genocided, or if
	 * we deliberately chose something illegal to force newman().
	 */
	if (!polyok(&mons[mntmp]) || !rn2(5) || your_race(&mons[mntmp]))
		newman();
	else if(!polymon(mntmp)) return;

	if (!uarmg) selftouch("Nicht mehr l�nger versteinerungsresistent SUBJECT_IM_SATZ VERB_TOUCH PRONOMEN_PERSONAL"); /* EN if (!uarmg) selftouch("No longer petrify-resistant, you"); */

 made_change:
	new_light = Upolyd ? emits_light(youmonst.data) : 0;
	if (old_light != new_light) {
	    if (old_light)
		del_light_source(LS_MONSTER, (genericptr_t)&youmonst);
	    if (new_light == 1) ++new_light;  /* otherwise it's undetectable */
	    if (new_light)
		new_light_source(u.ux, u.uy, new_light,
				 LS_MONSTER, (genericptr_t)&youmonst);
	}
	if (is_pool(u.ux,u.uy) && was_floating && !(Levitation || Flying) &&
		!breathless(youmonst.data) && !amphibious(youmonst.data) &&
		!Swimming) drown();
}

/* (try to) make a mntmp monster out of the player */
int
polymon(mntmp)	/* returns 1 if polymorph successful */
int	mntmp;
{
	boolean sticky = sticks(youmonst.data) && u.ustuck && !u.uswallow,
		was_blind = !!Blind, dochange = FALSE;
	boolean could_pass_walls = Passes_walls;
	int mlvl;

	if (mvitals[mntmp].mvflags & G_GENOD) {	/* allow G_EXTINCT */
		Du_fuehlst_dich("ziemlich %s-ig.",mons[mntmp].mname); /* EN You_feel("rather %s-ish.",mons[mntmp].mname); */ // TODO DE
		exercise(A_WIS, TRUE);
		return(0);
	}

	/* KMH, conduct */
	u.uconduct.polyselfs++;

	if (!Upolyd) {
		/* Human to monster; save human stats */
		u.macurr = u.acurr;
		u.mamax = u.amax;
		u.mfemale = flags.female;
	} else {
		/* Monster to monster; restore human stats, to be
		 * immediately changed to provide stats for the new monster
		 */
		u.acurr = u.macurr;
		u.amax = u.mamax;
		flags.female = u.mfemale;
	}

	if (youmonst.m_ap_type) {
	    /* stop mimicking immediately */
	    if (multi < 0) unmul("");
	} else if (mons[mntmp].mlet != S_MIMIC) {
	    /* as in polyman() */
	    youmonst.m_ap_type = M_AP_NOTHING;
	}
	if (is_male(&mons[mntmp])) {
		if(flags.female) dochange = TRUE;
	} else if (is_female(&mons[mntmp])) {
		if(!flags.female) dochange = TRUE;
	} else if (!is_neuter(&mons[mntmp]) && mntmp != u.ulycn) {
		if(!rn2(10)) dochange = TRUE;
	}
	if (dochange) {
		flags.female = !flags.female;
		You("%s %s%s!",
		    (u.umonnum != mntmp) ? "VERB_VERWANDELN OBJECT PRONOMEN_PERSONAL NEUES_OBJECT OBJECT in ARTIKEL_UNBESTIMMTER" : "VERB_FUEHLEN OBJECT PRONOMEN_PERSONAL wie NEUES_OBJECT OBJECT KASUS_NOMINATIV ARTIKEL_UNBESTIMMTER ADJEKTIV_NEU", /* EN (u.umonnum != mntmp) ? "turn into a" : "feel like a new", */
		    (is_male(&mons[mntmp]) || is_female(&mons[mntmp])) ? "" :
			flags.female ? "ADJEKTIV_WEIBLICH " : "ADJEKTIV_MAENNLICH ", /* EN flags.female ? "female " : "male ", */
		    mons[mntmp].mname);
	} else {
		if (u.umonnum != mntmp)
			You("VERB_VERWANDELN OBJECT PRONOMEN_PERSONAL NEUES_OBJECT OBJECT in %s!", an(mons[mntmp].mname)); /* EN You("turn into %s!", an(mons[mntmp].mname)); */
		else
			Du_fuehlst_dich("wie OBJECT KASUS_NOMINATIV ARTIKEL_UNBESTIMMTER ADJEKTIV_NEU %s!", mons[mntmp].mname); /* EN You_feel("like a new %s!", mons[mntmp].mname); */
	}
	if (Stoned && poly_when_stoned(&mons[mntmp])) {
		/* poly_when_stoned already checked stone golem genocide */
		You("VERB_VERSTEINERN!"); /* EN You("turn to stone!"); */
		mntmp = PM_STONE_GOLEM;
		Stoned = 0;
		delayed_killer = 0;
	}

	u.mtimedone = rn1(500, 500);
	u.umonnum = mntmp;
	set_uasmon();

	/* New stats for monster, to last only as long as polymorphed.
	 * Currently only strength gets changed.
	 */
	if(strongmonst(&mons[mntmp])) ABASE(A_STR) = AMAX(A_STR) = STR18(100);

	if (Stone_resistance && Stoned) { /* parnes@eniac.seas.upenn.edu */
		Stoned = 0;
		delayed_killer = 0;
		You("VERB_SCHEINEN nicht mehr zu versteinern."); /* EN You("no longer seem to be petrifying."); */
	}
	if (Sick_resistance && Sick) {
		make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		Dir_ist("nicht mehr �bel."); /* EN You("no longer feel sick."); */
	}
	if (Slimed) {
	    if (flaming(youmonst.data)) {
		pline("Der Schleim vertrocknet und bl�ttert ab!"); /* EN pline_The("slime burns away!"); */
		Slimed = 0L;
		flags.botl = 1;
	    } else if (mntmp == PM_GREEN_SLIME) {
		/* do it silently */
		Slimed = 0L;
		flags.botl = 1;
	    }
	}
	if (nohands(youmonst.data)) Glib = 0;

	/*
	mlvl = adj_lev(&mons[mntmp]);
	 * We can't do the above, since there's no such thing as an
	 * "experience level of you as a monster" for a polymorphed character.
	 */
	mlvl = (int)mons[mntmp].mlevel;
	if (youmonst.data->mlet == S_DRAGON && mntmp >= PM_GRAY_DRAGON) {
		u.mhmax = In_endgame(&u.uz) ? (8*mlvl) : (4*mlvl + d(mlvl,4));
	} else if (is_golem(youmonst.data)) {
		u.mhmax = golemhp(mntmp);
	} else {
		if (!mlvl) u.mhmax = rnd(4);
		else u.mhmax = d(mlvl, 8);
		if (is_home_elemental(&mons[mntmp])) u.mhmax *= 3;
	}
	u.mh = u.mhmax;

	if (u.ulevel < mlvl) {
	/* Low level characters can't become high level monsters for long */
#ifdef DUMB
		/* DRS/NS 2.2.6 messes up -- Peter Kendell */
		int mtd = u.mtimedone, ulv = u.ulevel;

		u.mtimedone = mtd * ulv / mlvl;
#else
		u.mtimedone = u.mtimedone * u.ulevel / mlvl;
#endif
	}

	if (uskin && mntmp != armor_to_dragon(uskin->otyp))
		skinback(FALSE);
	break_armor();
	drop_weapon(1);
	if (hides_under(youmonst.data))
		u.uundetected = OBJ_AT(u.ux, u.uy);
	else if (youmonst.data->mlet == S_EEL)
		u.uundetected = is_pool(u.ux, u.uy);
	else
		u.uundetected = 0;

	if (u.utraptype == TT_PIT) {
	    if (could_pass_walls && !Passes_walls) {
		u.utrap = rn1(6,2);
	    } else if (!could_pass_walls && Passes_walls) {
		u.utrap = 0;
	    }
	}
	if (was_blind && !Blind) {	/* previous form was eyeless */
	    Blinded = 1L;
	    make_blinded(0L, TRUE);	/* remove blindness */
	}
	newsym(u.ux,u.uy);		/* Change symbol */

	if (!sticky && !u.uswallow && u.ustuck && sticks(youmonst.data)) u.ustuck = 0;
	else if (sticky && !sticks(youmonst.data)) uunstick();
#ifdef STEED
	if (u.usteed) {
	    if (touch_petrifies(u.usteed->data) &&
	    		!Stone_resistance && rnl(3)) {
	    	char buf[BUFSZ];

	    	pline("Nicht mehr l�nger versteinerungsresistent SUBJECT_IM_SATZ VERB_TOUCH PRONOMEN_PERSONAL OBJECT %s.", /* EN pline("No longer petrifying-resistant, you touch %s.", */
	    			mon_nam(u.usteed));
	    	Sprintf(buf, "beim Reiten KASUS_GENITIV %s", an(u.usteed->data->mname)); /* EN Sprintf(buf, "riding %s", an(u.usteed->data->mname)); */
	    	instapetrify(buf);
 	    }
	    if (!can_ride(u.usteed)) dismount_steed(DISMOUNT_POLY);
	}
#endif

	if (flags.verbose) {
	    static const char use_thec[] = "SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_BENUTZEN den Befehl #%s um %s."; /* EN static const char use_thec[] = "Use the command #%s to %s."; */
	    static const char monsterc[] = "monster";
	    if (can_breathe(youmonst.data))
		pline(use_thec,monsterc,"OBJECT PRONOMEN_POSSESSIV Odem-NOUN_WEAPON zu benutzen"); /* EN pline(use_thec,monsterc,"use your breath weapon"); */
	    if (attacktype(youmonst.data, AT_SPIT))
		pline(use_thec,monsterc,"Gift zu speien"); /* EN pline(use_thec,monsterc,"spit venom"); */
	    if (youmonst.data->mlet == S_NYMPH)
		pline(use_thec,monsterc,"eine Eisenkugel zu entfernen"); /* EN pline(use_thec,monsterc,"remove an iron ball"); */
	    if (attacktype(youmonst.data, AT_GAZE))
		pline(use_thec,monsterc,"Monstern anzustarren"); /* EN pline(use_thec,monsterc,"gaze at monsters"); */
	    if (is_hider(youmonst.data))
		pline(use_thec,monsterc,"OBJECT PRONOMEN_PERSONAL zu verstecken"); /* EN pline(use_thec,monsterc,"hide"); */
	    if (is_were(youmonst.data))
		pline(use_thec,monsterc,"Hilfe zu rufen"); /* EN pline(use_thec,monsterc,"summon help"); */
	    if (webmaker(youmonst.data))
		pline(use_thec,monsterc,"ein Netz zu spinnen"); /* EN pline(use_thec,monsterc,"spin a web"); */
	    if (u.umonnum == PM_GREMLIN)
		pline(use_thec,monsterc,"sich im Springbrunnen zu vermehren"); /* EN pline(use_thec,monsterc,"multiply in a fountain"); */
	    if (is_unicorn(youmonst.data))
		pline(use_thec,monsterc,"OBJECT PRONOMEN_POSSESSIV NOUN_HORN zu benutzen"); /* EN pline(use_thec,monsterc,"use your horn"); */
	    if (is_mind_flayer(youmonst.data))
		pline(use_thec,monsterc,"eine Gedankenwelle auszusenden"); /* EN pline(use_thec,monsterc,"emit a mental blast"); */
	    if (youmonst.data->msound == MS_SHRIEK) /* worthless, actually */
		pline(use_thec,monsterc,"zu MODIFIER_VERB_INFINITIV VERB_SHRIEK"); /* EN pline(use_thec,monsterc,"shriek"); */
	    if (lays_eggs(youmonst.data) && flags.female)
		pline(use_thec,"sitzen","ein Ei zu legen"); /* EN pline(use_thec,"sit","lay an egg"); */
	}
	/* you now know what an egg of your type looks like */
	if (lays_eggs(youmonst.data)) {
	    learn_egg_type(u.umonnum);
	    /* make queen bees recognize killer bee eggs */
	    learn_egg_type(egg_type_from_parent(u.umonnum, TRUE));
	}
	find_ac();
	if((!Levitation && !u.ustuck && !Flying &&
	    (is_pool(u.ux,u.uy) || is_lava(u.ux,u.uy))) ||
	   (Underwater && !Swimming))
	    spoteffects(TRUE);
	if (Passes_walls && u.utrap && u.utraptype == TT_INFLOOR) {
	    u.utrap = 0;
	    pline("Der Stein h�lt KASUS_AKKUSATIV PRONOMEN_PERSONAL nicht l�nger gefangen."); /* EN pline_The("rock seems to no longer trap you."); */
	} else if (likes_lava(youmonst.data) && u.utrap && u.utraptype == TT_LAVA) {
	    u.utrap = 0;
			pline("Die Lava f�hlt sich jetzt angenehm an."); /* EN pline_The("lava now feels soothing."); */
	}
	if (amorphous(youmonst.data) || is_whirly(youmonst.data) || unsolid(youmonst.data)) {
	    if (Punished) {
		You("VERB_SCHLUEPFEN aus der Eisenkette."); /* EN You("slip out of the iron chain."); */
		unpunish();
	    }
	}
	if (u.utrap && (u.utraptype == TT_WEB || u.utraptype == TT_BEARTRAP) &&
		(amorphous(youmonst.data) || is_whirly(youmonst.data) || unsolid(youmonst.data) ||
		  (youmonst.data->msize <= MZ_SMALL && u.utraptype == TT_BEARTRAP))) {
	    You("VERB_STECKEN nicht l�nger OBJECT KASUS_DATIV in ARTIKEL_BESTIMMTER %s fest.", /* EN You("are no longer stuck in the %s.", */
		    u.utraptype == TT_WEB ? "NOUN_WEB" : "NOUN_BEAR_TRAP"); /* EN u.utraptype == TT_WEB ? "web" : "bear trap"); */
	    /* probably should burn webs too if PM_FIRE_ELEMENTAL */
	    u.utrap = 0;
	}
	if (webmaker(youmonst.data) && u.utrap && u.utraptype == TT_WEB) {
	    You("VERB_ORIENTIEREN OBJECT PRONOMEN_PERSONAL auf dem Netz."); /* EN You("orient yourself on the web."); */
	    u.utrap = 0;
	}
	flags.botl = 1;
	vision_full_recalc = 1;
	see_monsters();
	exercise(A_CON, FALSE);
	exercise(A_WIS, TRUE);
	(void) encumber_msg();
	return(1);
}

STATIC_OVL void
break_armor()
{
    register struct obj *otmp;

    if (breakarm(youmonst.data)) {
	if ((otmp = uarm) != 0) {
		if (donning(otmp)) cancel_don();
		You("VERB_PLATZEN OBJECT KASUS_DATIV aus PRONOMEN_POSSESSIV NOUN_ARMOR!"); /* EN You("break out of your armor!"); */
		exercise(A_STR, FALSE);
		(void) Armor_gone();
		useup(otmp);
	}
	if ((otmp = uarmc) != 0) {
	    if(otmp->oartifact) {
		Your("%s VERB_FALLEN runter!", cloak_simple_name(otmp)); /* EN Your("%s falls off!", cloak_simple_name(otmp)); */
		(void) Cloak_off();
		dropx(otmp);
	    } else {
		You("VERB_ZERREISSEN OBJECT PRONOMEN_POSSESSIV %s!", cloak_simple_name(otmp)); /* EN Your("%s tears apart!", cloak_simple_name(otmp)); */
		(void) Cloak_off();
		useup(otmp);
	    }
	}
#ifdef TOURIST
	if (uarmu) {
		Your("NOUN_SHIRT VERB_WERDEN in Fetzen gerissen!"); /* EN Your("shirt rips to shreds!"); */
		useup(uarmu);
	}
#endif
    } else if (sliparm(youmonst.data)) {
	if (((otmp = uarm) != 0) && (racial_exception(&youmonst, otmp) < 1)) {
		if (donning(otmp)) cancel_don();
		Your("NOUN_ARMOR VERB_FALLEN OBJECT KASUS_DATIV von PRONOMEN_PERSONAL ab!"); /* EN Your("armor falls around you!"); */
		(void) Armor_gone();
		dropx(otmp);
	}
	if ((otmp = uarmc) != 0) {
		if (is_whirly(youmonst.data))
			Your("%s falls, unsupported!", cloak_simple_name(otmp)); /* EN Your("%s falls, unsupported!", cloak_simple_name(otmp)); */ // TODO DE
		else You("VERB_SCHRUMPFEN OBJECT KASUS_DATIV aus PRONOMEN_POSSESSIV %s!", cloak_simple_name(otmp)); /* EN else You("shrink out of your %s!", cloak_simple_name(otmp)); */
		(void) Cloak_off();
		dropx(otmp);
	}
#ifdef TOURIST
	if ((otmp = uarmu) != 0) {
		if (is_whirly(youmonst.data))
			You("VERB_STROEMEN OBJECT durch PRONOMEN_POSSESSIV NOUN_SHIRT!"); /* EN You("seep right through your shirt!"); */
		else You("VERB_WERDEN zu klein OBJECT f�r PRONOMEN_POSSESSIV NOUN_SHIRT!"); /* EN else You("become much too small for your shirt!"); */
		setworn((struct obj *)0, otmp->owornmask & W_ARMU);
		dropx(otmp);
	}
#endif
    }
    if (has_horns(youmonst.data)) {
	if ((otmp = uarmh) != 0) {
	    if (is_flimsy(otmp) && !donning(otmp)) {
		char hornbuf[BUFSZ], yourbuf[BUFSZ];

		/* Future possiblities: This could damage/destroy helmet */
		Sprintf(hornbuf, "NOUN_HORN%s", plur(num_horns(youmonst.data))); /* EN Sprintf(hornbuf, "horn%s", plur(num_horns(youmonst.data))); */
		Your("%s %s sich OBJECT durch %s %s.", hornbuf, vtense(hornbuf, "VERB_BOHREN"), /* EN Your("%s %s through %s %s.", hornbuf, vtense(hornbuf, "pierce"), */
		     shk_your(yourbuf, otmp), xname(otmp));
	    } else {
		if (donning(otmp)) cancel_don();
		Your("NOUN_HELMET VERB_FALLEN %s!", zu_Boden(u.ux, u.uy)); /* EN Your("helmet falls to the %s!", surface(u.ux, u.uy)); */
		(void) Helmet_off();
		dropx(otmp);
	    }
	}
    }
    if (nohands(youmonst.data) || verysmall(youmonst.data)) {
	if ((otmp = uarmg) != 0) {
	    if (donning(otmp)) cancel_don();
	    /* Drop weapon along with gloves */
	    You("VERB_DROP OBJECT PRONOMEN_POSSESSIV NOUN_GLOVESs%s SATZKLAMMER!", uwep ? " und NEUES_OBJECT OBJECT NOUN_WEAPON" : ""); /* EN You("drop your gloves%s!", uwep ? " and weapon" : ""); */
	    drop_weapon(0);
	    (void) Gloves_off();
	    dropx(otmp);
	}
	if ((otmp = uarms) != 0) {
	    You("VERB_KOENNEN OBJECT PRONOMEN_POSSESSIV NOUN_SHIELD nicht mehr halten!"); /* EN You("can no longer hold your shield!"); */
	    (void) Shield_off();
	    dropx(otmp);
	}
	if ((otmp = uarmh) != 0) {
	    if (donning(otmp)) cancel_don();
	    Your("NOUN_HELMET VERB_FALLEN %s!", zu_Boden(u.ux, u.uy)); /* EN Your("helmet falls to the %s!", surface(u.ux, u.uy)); */
	    (void) Helmet_off();
	    dropx(otmp);
	}
    }
    if (nohands(youmonst.data) || verysmall(youmonst.data) ||
		slithy(youmonst.data) || youmonst.data->mlet == S_CENTAUR) {
	if ((otmp = uarmf) != 0) {
	    if (donning(otmp)) cancel_don();
	    if (is_whirly(youmonst.data))
		Your("NOUN_BOOTSs VERB_FALLEN runter!"); /* EN Your("boots fall away!"); */
	    else Your("NOUN_BOOTSs %s OBJECT KASUS_DATIV von PRONOMEN_POSSESSIV NOUN_FOOTs!", /* EN else Your("boots %s off your feet!", */
			verysmall(youmonst.data) ? "VERB_RUTSCHEN" : "VERB_PLATZEN"); /* EN verysmall(youmonst.data) ? "slide" : "are pushed"); */
	    (void) Boots_off();
	    dropx(otmp);
	}
    }
}

STATIC_OVL void
drop_weapon(alone)
int alone;
{
    struct obj *otmp;
    struct obj *otmp2;

    if ((otmp = uwep) != 0) {
	/* !alone check below is currently superfluous but in the
	 * future it might not be so if there are monsters which cannot
	 * wear gloves but can wield weapons
	 */
	if (!alone || cantwield(youmonst.data)) {
	    struct obj *wep = uwep;

	    if (alone) You("VERB_MERKEN, NEUER_SATZ dass PRONOMEN_PERSONAL OBJECT PRONOMEN_POSSESSIV NOUN_WEAPON%s fallenlassen VERB_MUESSEN!", /* EN if (alone) You("find you must drop your weapon%s!", */
			   	u.twoweap ? "s" : "");
	    otmp2 = u.twoweap ? uswapwep : 0;
	    uwepgone();
	    if (!wep->cursed || wep->otyp != LOADSTONE)
		dropx(otmp);
	    if (otmp2 != 0) {
		uswapwepgone();
		if (!otmp2->cursed || otmp2->otyp != LOADSTONE)
		    dropx(otmp2);
	    }
	    untwoweapon();
	} else if (!could_twoweap(youmonst.data)) {
	    untwoweapon();
	}
    }
}

void
rehumanize()
{
	/* You can't revert back while unchanging */
	if (Unchanging && (u.mh < 1)) {
		killer_format = NO_KILLER_PREFIX;
		killer = "in Kreaturgestalt feststeckend get�tet"; /* EN killer = "killed while stuck in creature form"; */
		done(DIED);
	}

	if (emits_light(youmonst.data))
	    del_light_source(LS_MONSTER, (genericptr_t)&youmonst);
	polyman("VERB_HAVE wieder OBJECT %s NOUN_GESTALT!", urace.adj); /* EN polyman("return to %s form!", urace.adj); */ /* 'Zwergenform, Menschengestalt besser? */

	if (u.uhp < 1) {
	    char kbuf[256];

	    Sprintf(kbuf, "R�ckkehr zu lebensunf�higer %s NOUN_GESTALT", urace.adj); /* EN Sprintf(kbuf, "reverting to unhealthy %s form", urace.adj); */
	    killer_format = KILLED_BY;
	    killer = kbuf;
	    done(DIED);
	}
	if (!uarmg) selftouch("Nicht mehr l�nger versteinerungsresistent SUBJECT_IM_SATZ VERB_TOUCH PRONOMEN_PERSONAL"); /* EN if (!uarmg) selftouch("No longer petrify-resistant, you"); */
	nomul(0);

	flags.botl = 1;
	vision_full_recalc = 1;
	(void) encumber_msg();
}

int
dobreathe()
{
	struct attack *mattk;

	if (Strangled) {
	    You_cant("atmen.  Tut mir leid."); /* EN You_cant("breathe.  Sorry."); */
	    return(0);
	}
	if (u.uen < 15) {
	    You("VERB_HAVE nicht genug Energie zum Atmen!"); /* EN You("don't have enough energy to breathe!"); */
	    return(0);
	}
	u.uen -= 15;
	flags.botl = 1;

	if (!getdir((char *)0)) return(0);

	mattk = attacktype_fordmg(youmonst.data, AT_BREA, AD_ANY);
	if (!mattk)
	    impossible("bad breath attack?");	/* mouthwash needed... */
	else
	    buzz((int) (20 + mattk->adtyp-1), (int)mattk->damn,
		u.ux, u.uy, u.dx, u.dy);
	return(1);
}

int
dospit()
{
	struct obj *otmp;

	if (!getdir((char *)0)) return(0);
	otmp = mksobj(u.umonnum==PM_COBRA ? BLINDING_VENOM : ACID_VENOM,
			TRUE, FALSE);
	otmp->spe = 1; /* to indicate it's yours */
	throwit(otmp, 0L, FALSE);
	return(1);
}

int
doremove()
{
	if (!Punished) {
		You("VERB_SEIN an nichts angekettet!"); /* EN You("are not chained to anything!"); */
		return(0);
	}
	unpunish();
	return(1);
}

int
dospinweb()
{
	register struct trap *ttmp = t_at(u.ux,u.uy);

	if (Levitation || Is_airlevel(&u.uz)
	    || Underwater || Is_waterlevel(&u.uz)) {
		You("VERB_MUESSEN am Boden sein um ein Netz zu spinnen."); /* EN You("must be on the ground to spin a web."); */
		return(0);
	}
	if (u.uswallow) {
		You("VERB_ABSONDERN Spinnenseide OBJECT KASUS_DATIV in %s SATZKLAMMER.", mon_nam(u.ustuck)); /* EN You("release web fluid inside %s.", mon_nam(u.ustuck)); */
		if (is_animal(u.ustuck->data)) {
			expels(u.ustuck, u.ustuck->data, TRUE);
			return(0);
		}
		if (is_whirly(u.ustuck->data)) {
			int i;

			for (i = 0; i < NATTK; i++)
				if (u.ustuck->data->mattk[i].aatyp == AT_ENGL)
					break;
			if (i == NATTK)
			       impossible("Swallower has no engulfing attack?");
			else {
				char sweep[40]; /* EN char sweep[30]; */

				sweep[0] = '\0';
				switch(u.ustuck->data->mattk[i].adtyp) {
					case AD_FIRE:
						Strcpy(sweep, "VERB_ENTZUENDEN sich und "); /* EN Strcpy(sweep, "ignites and "); */
						break;
					case AD_ELEC:
						Strcpy(sweep, "VERB_VERBRUTZELN und "); /* EN Strcpy(sweep, "fries and "); */
						break;
					case AD_COLD:
						Strcpy(sweep,
						      "VERB_GEFRIEREN, VERB_ZERBRECHEN und "); /* EN "freezes, shatters and "); */
						break;
				}
				pline_The("NOUN_WEB %sVERB_WERDEN fortgerissen!", sweep); /* EN pline_The("web %sis swept away!", sweep); */
			}
			return(0);
		}		     /* default: a nasty jelly-like creature */
		pline_The("NOUN_WEB VERB_AUFLOESEN OBJECT KASUS_DATIV _in_ %s SATZKLAMMER.", mon_nam(u.ustuck)); /* EN pline_The("web dissolves into %s.", mon_nam(u.ustuck)); */
		return(0);
	}
	if (u.utrap) {
		You("VERB_KOENNEN kein Netz spinnen, wenn NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL in einer Falle festsitzt."); /* EN You("cannot spin webs while stuck in a trap."); */
		return(0);
	}
	exercise(A_DEX, TRUE);
	if (ttmp) switch (ttmp->ttyp) {
		case PIT:
		case SPIKED_PIT: You("VERB_SPINNEN ein Netz, dass OBJECT ARTIKEL_BESTIMMTER NOUN_PIT zudeckt."); /* EN case SPIKED_PIT: You("spin a web, covering up the pit."); */
			deltrap(ttmp);
			bury_objs(u.ux, u.uy);
			newsym(u.ux, u.uy);
			return(1);
		case SQKY_BOARD: pline("Das Quietschen der Diele verstummt."); /* EN case SQKY_BOARD: pline_The("squeaky board is muffled."); */
			deltrap(ttmp);
			newsym(u.ux, u.uy);
			return(1);
		case TELEP_TRAP:
		case LEVEL_TELEP:
		case MAGIC_PORTAL:
			Your("NOUN_NETZ VERB_VERSCHWINDEN!"); /* EN Your("webbing vanishes!"); */
			return(0);
		case WEB: You("VERB_VERSTAERKEN das Netz."); /* EN case WEB: You("make the web thicker."); */
			return(1);
		case HOLE:
		case TRAPDOOR:
			You("VERB_SPINNEN ein Netz OBJECT KASUS_DATIV �ber ARTIKEL_BESTIMMTER %s.", /* EN You("web over the %s.", */
			    (ttmp->ttyp == TRAPDOOR) ? "NOUN_TRAP_DOOR" : "NOUN_LOCH"); /* EN (ttmp->ttyp == TRAPDOOR) ? "trap door" : "hole"); */
			deltrap(ttmp);
			newsym(u.ux, u.uy);
			return 1;
		case ROLLING_BOULDER_TRAP:
			You("VERB_SPINNEN OBJECT ARTIKEL_UNBESTIMMTER NOUN_WEB und VERB_BLOCKIEREN den Ausl�semechanismus."); /* EN You("spin a web, jamming the trigger."); */
			deltrap(ttmp);
			newsym(u.ux, u.uy);
			return(1);
		case ARROW_TRAP:
		case DART_TRAP:
		case BEAR_TRAP:
		case ROCKTRAP:
		case FIRE_TRAP:
		case LANDMINE:
		case SLP_GAS_TRAP:
		case RUST_TRAP:
		case MAGIC_TRAP:
		case ANTI_MAGIC:
		case POLY_TRAP:
			You("VERB_HAVE eine Falle ausgel�st!"); /* EN You("have triggered a trap!"); */
			dotrap(ttmp, 0);
			return(1);
		default:
			impossible("Webbing over trap type %d?", ttmp->ttyp);
			return(0);
		}
	else if (On_stairs(u.ux, u.uy)) {
	    /* cop out: don't let them hide the stairs */
	    Your("NOUN_WEB VERB_KOENNEN den Zugang OBJECT KASUS_DATIV zu ARTIKEL_BESTIMMTER %s nicht behindern.", /* EN Your("web fails to impede access to the %s.", */
		 (levl[u.ux][u.uy].typ == STAIRS) ? "NOUN_STAIRS" : "NOUN_LADDER"); /* EN (levl[u.ux][u.uy].typ == STAIRS) ? "stairs" : "ladder"); */
	    return(1);
		 
	}
	ttmp = maketrap(u.ux, u.uy, WEB);
	if (ttmp) {
		ttmp->tseen = 1;
		ttmp->madeby_u = 1;
	}
	newsym(u.ux, u.uy);
	return(1);
}

int
dosummon()
{
	int placeholder;
	if (u.uen < 10) {
	    Dir("fehlt die Energie um einen Hilferuf auszusenden!"); /* EN You("lack the energy to send forth a call for help!"); */
	    return(0);
	}
	u.uen -= 10;
	flags.botl = 1;

	You("VERB_RUFEN OBJECT PRONOMEN_POSSESSIV NOUN_BRUDERs zu Hilfe!"); /* EN You("call upon your brethren for help!"); */
	exercise(A_WIS, TRUE);
	if (!were_summon(youmonst.data, TRUE, &placeholder, (char *)0))
		pline("Aber es kommt niemand."); /* EN pline("But none arrive."); */
	return(1);
}

int
dogaze()
{
	register struct monst *mtmp;
	int looked = 0;
	char qbuf[QBUFSZ];
	int i;
	uchar adtyp = 0;

	for (i = 0; i < NATTK; i++) {
	    if(youmonst.data->mattk[i].aatyp == AT_GAZE) {
		adtyp = youmonst.data->mattk[i].adtyp;
		break;
	    }
	}
	if (adtyp != AD_CONF && adtyp != AD_FIRE) {
	    impossible("gaze attack %d?", adtyp);
	    return 0;
	}


	if (Blind) {
	    You("VERB_SEHEN hier nichts, NEUER_SATZ das PRONOMEN_PERSONAL anstarren MODIFIER_KONJUNKTIV_II VERB_KOENNEN."); /* EN You_cant("see anything to gaze at."); */
	    return 0;
	}
	if (u.uen < 15) {
	    Dir("fehlt die Energie KASUS_AKKUSATIV um PRONOMEN_POSSESSIV ADJEKTIV_SPEZIELL NOUN_GAZE zu benutzen!"); /* EN You("lack the energy to use your special gaze!"); */
	    return(0);
	}
	u.uen -= 15;
	flags.botl = 1;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my)) {
		looked++;
		if (Invis && !perceives(mtmp->data))
		    pline("SUBJECT %s VERB_SCHEINEN OBJECT PRONOMEN_POSSESSIV NOUN_GAZE nicht zu bemerken.", Monnam(mtmp)); /* EN pline("%s seems not to notice your gaze.", Monnam(mtmp)); */
		else if (mtmp->minvis && !See_invisible)
		    You_cant("sehen NEUER_SATZ wo SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT %s anstarren VERB_SOLLEN.", Monnam(mtmp)); /* EN You_cant("see where to gaze at %s.", Monnam(mtmp)); */
		else if (mtmp->m_ap_type == M_AP_FURNITURE
			|| mtmp->m_ap_type == M_AP_OBJECT) {
		    looked--;
		    continue;
		} else if (flags.safe_dog && !Confusion && !Hallucination
		  && mtmp->mtame) {
		    You("VERB_VERZICHTEN darauf, OBJECT %s anzustarren.", y_monnam(mtmp)); /* EN You("avoid gazing at %s.", y_monnam(mtmp)); */
		} else {
		    if (flags.confirm && mtmp->mpeaceful && !Confusion
							&& !Hallucination) {
			Sprintf(qbuf, "Wirklich KASUS_AKKUSATIV %s %s?", /* EN Sprintf(qbuf, "Really %s %s?", */ 
			    mon_nam(mtmp), (adtyp == AD_CONF) ? "verwirren" : "angreifen" /* EN (adtyp == AD_CONF) ? "confuse" : "attack", */
			    ); /* EN mon_nam(mtmp)); */
			if (yn(qbuf) != 'y') continue;
			setmangry(mtmp);
		    }
		    if (!mtmp->mcanmove || mtmp->mstun || mtmp->msleeping ||
				    !mtmp->mcansee || !haseyes(mtmp->data)) {
			looked--;
			continue;
		    }
		    /* No reflection check for consistency with when a monster
		     * gazes at *you*--only medusa gaze gets reflected then.
		     */
		    if (adtyp == AD_CONF) {
			if (!mtmp->mconf)
			    Your("NOUN_GAZE VERB_VERWIRREN OBJECT %s!", mon_nam(mtmp)); /* EN Your("gaze confuses %s!", mon_nam(mtmp)); */
			else
			    pline("SUBJECT %s VERB_WERDEN immer verwirrter.", /* EN pline("%s is getting more and more confused.", */
							Monnam(mtmp));
			mtmp->mconf = 1;
		    } else if (adtyp == AD_FIRE) {
			int dmg = d(2,6);
			You("VERB_ANGREIFEN OBJECT %s mit einem feurigen Blick SATZKLAMMER!", mon_nam(mtmp)); /* EN You("attack %s with a fiery gaze!", mon_nam(mtmp)); */
			if (resists_fire(mtmp)) {
			    pline("Das Feuer verbrennt KASUS_AKKUSATIV %s nicht!", mon_nam(mtmp)); /* EN pline_The("fire doesn't burn %s!", mon_nam(mtmp)); */
			    dmg = 0;
			}
			if((int) u.ulevel > rn2(20))
			    (void) destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
			if((int) u.ulevel > rn2(20))
			    (void) destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);
			if((int) u.ulevel > rn2(25))
			    (void) destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
			if (dmg && !DEADMONSTER(mtmp)) mtmp->mhp -= dmg;
			if (mtmp->mhp <= 0) killed(mtmp);
		    }
		    /* For consistency with passive() in uhitm.c, this only
		     * affects you if the monster is still alive.
		     */
		    if (!DEADMONSTER(mtmp) &&
			  (mtmp->data==&mons[PM_FLOATING_EYE]) && !mtmp->mcan) {
			if (!Free_action) {
			    You("VERB_ERSTARREN OBJECT KASUS_DATIV durch %s!", /* EN You("are frozen by %s gaze!", */
					     genitivattribut_zu_wort(mon_nam(mtmp), "NOUN_GAZE")); /* EN s_suffix(mon_nam(mtmp))); */
			    nomul((u.ulevel > 6 || rn2(4)) ?
				    -d((int)mtmp->m_lev+1,
					    (int)mtmp->data->mattk[0].damd)
				    : -200);
			    return 1;
			} else
			    You("VERB_VERSTEIFEN OBJECT PRONOMEN_PERSONAL kurz NEUES_OBJECT OBJECT KASUS_DATIV unter %s.", /* EN You("stiffen momentarily under %s gaze.", */
				    genitivattribut_zu_wort(mon_nam(mtmp),"NOUN_GAZE")); /* EN s_suffix(mon_nam(mtmp))); */
		    }
		    /* Technically this one shouldn't affect you at all because
		     * the Medusa gaze is an active monster attack that only
		     * works on the monster's turn, but for it to *not* have an
		     * effect would be too weird.
		     */
		    if (!DEADMONSTER(mtmp) &&
			    (mtmp->data == &mons[PM_MEDUSA]) && !mtmp->mcan) {
			pline(
			 "SATZBEGINN KASUS_AKKUSATIV ARTIKEL_BESTIMMTER ADJEKTIV_WACH %s anzustarren ist wirklich keine gute Idee.", /* EN "Gazing at the awake %s is not a very good idea.", */
			    l_monnam(mtmp));
			/* as if gazing at a sleeping anything is fruitful... */
			You("VERB_VERSTEINERN ...");
			killer_format = NO_KILLER_PREFIX; /* EN killer_format = KILLED_BY; */
			killer = "erwiderte absichtlich Medusas Blick"; /* EN killer = "deliberately meeting Medusa's gaze"; */
			done(STONING);
		    }
		}
	    }
	}
	if (!looked) You("VERB_GAZE ein bisschen in der Gegend rum."); /* EN if (!looked) You("gaze at no place in particular."); */
	return 1;
}

int
dohide()
{
	boolean ismimic = youmonst.data->mlet == S_MIMIC;

	if (u.uundetected || (ismimic && youmonst.m_ap_type != M_AP_NOTHING)) {
		You("VERB_VERSTECKEN OBJECT PRONOMEN_PERSONAL bereits."); /* EN You("are already hiding."); */
		return(0);
	}
	if (ismimic) {
		/* should bring up a dialog "what would you like to imitate?" */
		youmonst.m_ap_type = M_AP_OBJECT;
		youmonst.mappearance = STRANGE_OBJECT;
	} else
		u.uundetected = 1;
	newsym(u.ux,u.uy);
	return(1);
}

int
domindblast()
{
	struct monst *mtmp, *nmon;

	if (u.uen < 10) {
	    You("VERB_KONZENTRIEREN OBJECT PRONOMEN_PERSONAL, NEUER_SATZ aber KASUS_DATIV PRONOMEN_PERSONAL fehlt die Energie, die Konzentration aufrechtzuerhalten."); /* EN You("concentrate but lack the energy to maintain doing so."); */
	    return(0);
	}
	u.uen -= 10;
	flags.botl = 1;

	You("VERB_KONZENTRIEREN OBJECT PRONOMEN_PERSONAL."); /* EN You("concentrate."); */
	You("VERB_VERSTROEMEN eine Woge psionischer Energie."); /* EN pline("A wave of psychic energy pours out."); */
	for(mtmp=fmon; mtmp; mtmp = nmon) {
		int u_sen;

		nmon = mtmp->nmon;
		if (DEADMONSTER(mtmp))
			continue;
		if (distu(mtmp->mx, mtmp->my) > BOLT_LIM * BOLT_LIM)
			continue;
		if(mtmp->mpeaceful)
			continue;
		u_sen = telepathic(mtmp->data) && !mtmp->mcansee;
		if (u_sen || (telepathic(mtmp->data) && rn2(2)) || !rn2(10)) {
			You("VERB_ANPEILEN OBJECT %s SATZKLAMMER.", genitivattribut_zu_wort(mon_nam(mtmp), /* EN You("lock in on %s %s.", s_suffix(mon_nam(mtmp)), */
				u_sen ? "NOUN_TELEPATHIE" : /* EN u_sen ? "telepathy" : */
				telepathic(mtmp->data) ? "ADJEKTIV_LATENT NOUN_TELEPATHIE" : /* EN telepathic(mtmp->data) ? "latent telepathy" : */
				"NOUN_VERSTAND")); /* EN "mind"); */
			mtmp->mhp -= rnd(15);
			if (mtmp->mhp <= 0)
				killed(mtmp);
		}
	}
	return 1;
}

STATIC_OVL void
uunstick()
{
	You("VERB_HALTEN OBJECT %s nicht mehr NEUES_OBJECT OBJECT KASUS_DATIV in PRONOMEN_POSSESSIV NOUN_KLAUEs.", Monnam(u.ustuck)); /* EN pline("%s is no longer in your clutches.", Monnam(u.ustuck)); */
	u.ustuck = 0;
}

void
skinback(silently)
boolean silently;
{
	if (uskin) {
		if (!silently) Your("NOUN_HAUT wandelt sich in ihre urspr�ngliche Form um."); /* EN if (!silently) Your("skin returns to its original form."); */
		uarm = uskin;
		uskin = (struct obj *)0;
		/* undo save/restore hack */
		uarm->owornmask &= ~I_SPECIAL;
	}
}

#endif /* OVLB */
#ifdef OVL1

const char *
mbodypart(mon, part)
struct monst *mon;
int part;
{
	static NEARDATA const char
		/* DE HANDED wird nicht gebraucht */
	*humanoid_parts[] = { "NOUN_ARM", "NOUN_EYE", "NOUN_FACE", "NOUN_FINGER", /* EN *humanoid_parts[] = { "arm", "eye", "face", "finger", */
		"NOUN_FINGERTIP", "NOUN_FOOT", "NOUN_HAND", "handed", "NOUN_HEAD", "NOUN_LEG", /* EN "fingertip", "foot", "hand", "handed", "head", "leg", */
		"schwindlig", "NOUN_HALS", "NOUN_SPINE", "NOUN_ZEHE", "NOUN_HAARs", /* EN "light headed", "neck", "spine", "toe", "hair", */
		"NOUN_BLUT", "NOUN_LUNGE", "NOUN_NASE", "NOUN_STOMACH"}, /* EN "blood", "lung", "nose", "stomach"}, */
	*jelly_parts[] = { "NOUN_PSEUDOPODIUM", "Dunkel-NOUN_FLECK", "Vorder-NOUN_SEITE", /* EN *jelly_parts[] = { "pseudopod", "dark spot", "front", */
		"Pseudopodium-NOUN_EXTREMITAET", "Pseudopodiumextremit�t-NOUN_SPITZE", /* EN "pseudopod extension", "pseudopod extremity", */
		"Pseudopodiums-NOUN_SPITZE", "NOUN_AXOPODIUM", "grasped", "NOUN_HIRNAREAL", /* EN "pseudopod root", "grasp", "grasped", "cerebral area", */
		"ADJEKTIV_UNTER NOUN_PSEUDOPODIUM", "seimig", "NOUN_MITTE", "NOUN_OBERFLAECHE", /* EN "lower pseudopod", "viscous", "middle", "surface", */
		"Pseudopodiums-NOUN_SPITZE", "NOUN_FLAGELLEs", "NOUN_SAFTs", /* EN "pseudopod extremity", "ripples", "juices", */
		"NOUN_MITOCHONDRIUM", "NOUN_FUEHLERs", "NOUN_ZYTOPLASMA" }, /* EN "surface", "sensor", "stomach" }, */
	*animal_parts[] = { "Vorder-NOUN_LAUF", "NOUN_EYE", "NOUN_FACE", "Vorder-NOUN_PFOTE", "NOUN_KLAUE", /* EN *animal_parts[] = { "forelimb", "eye", "face", "foreclaw", "claw tip", */
		"Hinter-NOUN_PFOTE", "Vorder-NOUN_CLAW", "clawed", "NOUN_HEAD", "Hinter-NOUN_LAUF", /* EN "rear claw", "foreclaw", "clawed", "head", "rear limb", */
		"schwindlig", "NOUN_HALS", "NOUN_SPINE", "Hinter-NOUN_KLAUE", /* EN "light headed", "neck", "spine", "rear claw tip", */
		"NOUN_FELL", "NOUN_BLUT", "NOUN_LUNGE", "NOUN_NASE", "NOUN_STOMACH" }, /* EN "fur", "blood", "lung", "nose", "stomach" }, */
	*bird_parts[] = { "NOUN_FLUEGEL", "NOUN_EYE", "NOUN_FACE", "NOUN_FLUEGEL", "NOUN_FLUEGELSPITZE", /* EN *bird_parts[] = { "wing", "eye", "face", "wing", "wing tip", */
		"NOUN_FOOT", "NOUN_FLUEGEL", "winged", "NOUN_HEAD", "NOUN_LEG", /* EN "foot", "wing", "winged", "head", "leg", */
		"schwindlig", "NOUN_HALS", "NOUN_SPINE", "NOUN_ZEHE", /* EN "light headed", "neck", "spine", "toe", */
		"NOUN_FEATHERs", "NOUN_BLUT", "NOUN_LUNGE", "NOUN_SCHNABEL", "NOUN_STOMACH" }, /* EN "feathers", "blood", "lung", "bill", "stomach" }, */
	*horse_parts[] = { "Vorder-NOUN_LAUF", "NOUN_EYE", "NOUN_FACE", "Vorder-NOUN_HUF", "Huf-NOUN_SPITZE", /* EN *horse_parts[] = { "foreleg", "eye", "face", "forehoof", "hoof tip", */
		"Hinter-NOUN_HUF", "Vorder-NOUN_KLAUE", "hooved", "NOUN_HEAD", "Hinter-NOUN_LAUF", /* EN "rear hoof", "foreclaw", "hooved", "head", "rear leg", */
		"schwindlig", "NOUN_HALS", "NOUN_SPINE", "Hinterhuf-NOUN_SPITZE", /* EN "light headed", "neck", "backbone", "rear hoof tip", */
		"NOUN_MAEHNE", "NOUN_BLUT", "NOUN_LUNGE", "NOUN_NASE", "NOUN_STOMACH"}, /* EN "mane", "blood", "lung", "nose", "stomach"}, */
	*sphere_parts[] = { "NOUN_FORTSATZ", "NOUN_SEHNERV", "NOUN_KOERPER", "NOUN_TENTAKEL", /* EN *sphere_parts[] = { "appendage", "optic nerve", "body", "tentacle", */
		"Tentakel-NOUN_SPITZE", "ADJEKTIV_UNTER NOUN_FORTSATZ", "NOUN_TENTAKEL", "tentacled", /* EN "tentacle tip", "lower appendage", "tentacle", "tentacled", */
		"NOUN_KOERPER", "ADJEKTIV_UNTER NOUN_TENTAKEL", "rotierend", "NOUN_AEQUATOR", "NOUN_KOERPER", /* EN "body", "lower tentacle", "rotational", "equator", "body", */
		"ADJEKTIV_UNTER Tentakel-NOUN_SPITZE", "NOUN_ZILIEs", "Lebens-NOUN_ENERGIE", "Netz-NOUN_HAUT", /* EN "lower tentacle tip", "cilia", "life force", "retina", */
		"NOUN_NERVUS_OLFACTORIUS", "Glas-NOUN_KOERPER" }, /* EN "olfactory nerve", "interior" }, */
	*fungus_parts[] = { "NOUN_MYZELIUM", "NOUN_PHOTOREZEPTOR", "Vorder-NOUN_SEITE", "NOUN_HYPHE", /* EN *fungus_parts[] = { "mycelium", "visual area", "front", "hypha", */
		"NOUN_HYPHE", "NOUN_RHIZOMORPHE", "NOUN_MYZELIUM", "stranded", "Ober-NOUN_SEITE", /* EN "hypha", "root", "strand", "stranded", "cap area", */
		"NOUN_RHIZOM", "sporuliert", "NOUN_STIEL", "NOUN_RHIZOM", "Rhizom-NOUN_SPITZE", /* EN "rhizome", "sporulated", "stalk", "root", "rhizome tip", */
		"NOUN_SPOREs", "NOUN_SAFTs", "NOUN_LAMELLE", "NOUN_LAMELLE", "NOUN_INNERE" }, /* EN "spores", "juices", "gill", "gill", "interior" }, */
	*vortex_parts[] = { "NOUN_BEREICH", "NOUN_EYE", "NOUN_FRONT", "Neben-NOUN_STROEMUNG", /* EN *vortex_parts[] = { "region", "eye", "front", "minor current", */
		"Neben-NOUN_STROEMUNG", "Senk-NOUN_STROEMUNG", "NOUN_STRUDEL", "swirled", /* EN "minor current", "lower current", "swirl", "swirled", */
		"Zentral-NOUN_BEREICH", "Senk-NOUN_STROEMUNG", "turbulent", "NOUN_AUGE", /* EN "central core", "lower current", "addled", "center", */
		"NOUN_STROEMUNGs", "NOUN_RAND", "NOUN_STROEMUNGs", "Lebens-NOUN_ENERGIE", /* EN "currents", "edge", "currents", "life force", */
		"NOUN_ZENTRUM", "Rand-NOUN_KANTE", "NOUN_INNERE" }, /* EN "center", "leading edge", "interior" }, */
	*snake_parts[] = { "Stummel-NOUN_EXTREMITAET", "NOUN_EYE", "NOUN_FACE", "ADJEKTIV_GROSS NOUN_SCHUPPE", /* EN *snake_parts[] = { "vestigial limb", "eye", "face", "large scale", */
		"NOUN_SPITZE einer gro�en Schuppe", "NOUN_HINTERTEIL", "K�rper-NOUN_SCHUPPE", "scale gapped", /* EN "large scale tip", "rear region", "scale gap", "scale gapped", */
		"NOUN_HEAD", "NOUN_HINTERTEIL", "schwindlig", "NOUN_HALS", "NOUN_LAENGE", /* EN "head", "rear region", "light headed", "neck", "length", */
		"Schwanz-NOUN_SCHUPPE", "NOUN_SCHUPPEs", "NOUN_BLUT", "NOUN_LUNGE", "ADJEKTIV_GESPALTEN NOUN_ZUNGE", "NOUN_STOMACH" }, /* EN "rear scale", "scales", "blood", "lung", "forked tongue", "stomach" }, */
	*fish_parts[] = { "NOUN_FLOSSE", "NOUN_EYE", "NOUN_PRAEMAXILLARE", "Bauch-NOUN_FLOSSE", /* EN *fish_parts[] = { "fin", "eye", "premaxillary", "pelvic axillary", */
		"Bauchflossen-NOUN_SPITZE", "After-NOUN_FLOSSE", "Brust-NOUN_FLOSSE", "finned", "NOUN_HEAD", "NOUN_SCHWANZSTIEL", /* EN "pelvic fin", "anal fin", "pectoral fin", "finned", "head", "peduncle", */
		"ausgedrillt", "NOUN_KIEMEs", "Dorsal-NOUN_FLOSSE", "Schwanz-NOUN_FLOSSE", /* EN "played out", "gills", "dorsal fin", "caudal fin", */
		"NOUN_SCHUPPEs", "NOUN_BLUT", "NOUN_KIEME", "NOUN_NASENHOEHLE", "NOUN_STOMACH" }; /* EN "scales", "blood", "gill", "nostril", "stomach" }; */
	/* claw attacks are overloaded in mons[]; most humanoids with
	   such attacks should still reference hands rather than claws */
	static const char not_claws[] = {
		S_HUMAN, S_MUMMY, S_ZOMBIE, S_ANGEL,
		S_NYMPH, S_LEPRECHAUN, S_QUANTMECH, S_VAMPIRE,
		S_ORC, S_GIANT,		/* quest nemeses */
		'\0'		/* string terminator; assert( S_xxx != 0 ); */
	};
	struct permonst *mptr = mon->data;

	if (part == HAND || part == HANDED) {	/* some special cases */
	    if (mptr->mlet == S_DOG || mptr->mlet == S_FELINE ||
		    mptr->mlet == S_YETI)
		return part == HAND ? "NOUN_PFOTE" : "pawed"; /* EN return part == HAND ? "paw" : "pawed"; */
	    if (humanoid(mptr) && attacktype(mptr, AT_CLAW) &&
		    !index(not_claws, mptr->mlet) &&
		    mptr != &mons[PM_STONE_GOLEM] &&
		    mptr != &mons[PM_INCUBUS] && mptr != &mons[PM_SUCCUBUS])
		return part == HAND ? "NOUN_KLAUE" : "clawed"; /* EN return part == HAND ? "claw" : "clawed"; */
	}
	if ((mptr == &mons[PM_MUMAK] || mptr == &mons[PM_MASTODON]) &&
		part == NOSE)
	    return "NOUN_RUESSEL"; /* EN return "trunk"; */
	if (mptr == &mons[PM_SHARK] && part == HAIR)
	    return "NOUN_HAUT";	/* sharks don't have scales */ /* EN return "skin";	*/
	if (mptr == &mons[PM_JELLYFISH] && (part == ARM || part == FINGER ||
	    part == HAND || part == FOOT || part == TOE))
	    return "NOUN_TENTAKEL"; /* EN return "tentacle"; */
	if (mptr == &mons[PM_FLOATING_EYE] && part == EYE)
	    return "NOUN_HORNHAUT"; /* EN return "cornea"; */
	if (humanoid(mptr) &&
		(part == ARM || part == FINGER || part == FINGERTIP ||
		    part == HAND || part == HANDED))
	    return humanoid_parts[part];
	if (mptr == &mons[PM_RAVEN])
	    return bird_parts[part];
	if (mptr->mlet == S_CENTAUR || mptr->mlet == S_UNICORN ||
		(mptr == &mons[PM_ROTHE] && part != HAIR))
	    return horse_parts[part];
	if (mptr->mlet == S_LIGHT) {
		if (part == HANDED) return "rayed";
		else if (part == ARM || part == FINGER ||
				part == FINGERTIP || part == HAND) return "NOUN_RAY"; /* EN part == FINGERTIP || part == HAND) return "ray"; */
		else return "NOUN_BEAM"; /* EN else return "beam"; */
	}
	if (mptr->mlet == S_EEL && mptr != &mons[PM_JELLYFISH])
	    return fish_parts[part];
	if (slithy(mptr) || (mptr->mlet == S_DRAGON && part == HAIR))
	    return snake_parts[part];
	if (mptr->mlet == S_EYE)
	    return sphere_parts[part];
	if (mptr->mlet == S_JELLY || mptr->mlet == S_PUDDING ||
		mptr->mlet == S_BLOB || mptr == &mons[PM_JELLYFISH])
	    return jelly_parts[part];
	if (mptr->mlet == S_VORTEX || mptr->mlet == S_ELEMENTAL)
	    return vortex_parts[part];
	if (mptr->mlet == S_FUNGUS)
	    return fungus_parts[part];
	if (humanoid(mptr))
	    return humanoid_parts[part];
	return animal_parts[part];
}

const char *
body_part(part)
int part;
{
	return mbodypart(&youmonst, part);
}

#endif /* OVL1 */
#ifdef OVL0

int
poly_gender()
{
/* Returns gender of polymorphed player; 0/1=same meaning as flags.female,
 * 2=none.
 */
	if (is_neuter(youmonst.data) || !humanoid(youmonst.data)) return 2;
	return flags.female;
}

#endif /* OVL0 */
#ifdef OVLB

void
ugolemeffects(damtype, dam)
int damtype, dam;
{
	int heal = 0;
	/* We won't bother with "slow"/"haste" since players do not
	 * have a monster-specific slow/haste so there is no way to
	 * restore the old velocity once they are back to human.
	 */
	if (u.umonnum != PM_FLESH_GOLEM && u.umonnum != PM_IRON_GOLEM)
		return;
	switch (damtype) {
		case AD_ELEC: if (u.umonnum == PM_FLESH_GOLEM)
				heal = dam / 6; /* Approx 1 per die */
			break;
		case AD_FIRE: if (u.umonnum == PM_IRON_GOLEM)
				heal = dam;
			break;
	}
	if (heal && (u.mh < u.mhmax)) {
		u.mh += heal;
		if (u.mh > u.mhmax) u.mh = u.mhmax;
		flags.botl = 1;
		pline("Seltsamerweise VERB_SICH_FUEHLEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT PRONOMEN_PERSONAL besser als vorher."); /* EN pline("Strangely, you feel better than before."); */
		exercise(A_STR, TRUE);
	}
}

STATIC_OVL int
armor_to_dragon(atyp)
int atyp;
{
	switch(atyp) {
	    case GRAY_DRAGON_SCALE_MAIL:
	    case GRAY_DRAGON_SCALES:
		return PM_GRAY_DRAGON;
	    case SILVER_DRAGON_SCALE_MAIL:
	    case SILVER_DRAGON_SCALES:
		return PM_SILVER_DRAGON;
#if 0	/* DEFERRED */
	    case SHIMMERING_DRAGON_SCALE_MAIL:
	    case SHIMMERING_DRAGON_SCALES:
		return PM_SHIMMERING_DRAGON;
#endif
	    case RED_DRAGON_SCALE_MAIL:
	    case RED_DRAGON_SCALES:
		return PM_RED_DRAGON;
	    case ORANGE_DRAGON_SCALE_MAIL:
	    case ORANGE_DRAGON_SCALES:
		return PM_ORANGE_DRAGON;
	    case WHITE_DRAGON_SCALE_MAIL:
	    case WHITE_DRAGON_SCALES:
		return PM_WHITE_DRAGON;
	    case BLACK_DRAGON_SCALE_MAIL:
	    case BLACK_DRAGON_SCALES:
		return PM_BLACK_DRAGON;
	    case BLUE_DRAGON_SCALE_MAIL:
	    case BLUE_DRAGON_SCALES:
		return PM_BLUE_DRAGON;
	    case GREEN_DRAGON_SCALE_MAIL:
	    case GREEN_DRAGON_SCALES:
		return PM_GREEN_DRAGON;
	    case YELLOW_DRAGON_SCALE_MAIL:
	    case YELLOW_DRAGON_SCALES:
		return PM_YELLOW_DRAGON;
	    default:
		return -1;
	}
}

#endif /* OVLB */

/*polyself.c*/
