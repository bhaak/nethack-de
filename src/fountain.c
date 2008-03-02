/*	SCCS Id: @(#)fountain.c	3.4	2003/03/23	*/
/*	Copyright Scott R. Turner, srt@ucla, 10/27/86 */
/* NetHack may be freely redistributed.  See license for details. */

/* Code for drinking from fountains. */

#include "hack.h"

STATIC_DCL void NDECL(dowatersnakes);
STATIC_DCL void NDECL(dowaterdemon);
STATIC_DCL void NDECL(dowaternymph);
STATIC_PTR void FDECL(gush, (int,int,genericptr_t));
STATIC_DCL void NDECL(dofindgem);

void
floating_above(what)
const char *what;
{
    You("VERB_SCHWEBEN hoch über OBJECT ARTIKEL_BESTIMMTER %s.", what); /* EN You("are floating high above the %s.", what); */
}

STATIC_OVL void
dowatersnakes() /* Fountain of snakes! */
{
    register int num = rn1(5,2);
    struct monst *mtmp;

    if (!(mvitals[PM_WATER_MOCCASIN].mvflags & G_GONE)) {
	if (!Blind)
	    pline("An endless stream of %s pours forth!", /* EN pline("An endless stream of %s pours forth!", */ // TODO DE
		  Hallucination ? makeplural(rndmonnam()) : "NOUN_SNAKEs"); /* EN Hallucination ? makeplural(rndmonnam()) : "snakes"); */
	else
			You_hear("%s zischen!", something); /* EN You_hear("%s hissing!", something); */
	while(num-- > 0)
	    if((mtmp = makemon(&mons[PM_WATER_MOCCASIN],
			u.ux, u.uy, NO_MM_FLAGS)) && t_at(mtmp->mx, mtmp->my))
		(void) mintrap(mtmp);
    } else
	pline_The("fountain bubbles furiously for a moment, then calms."); /* EN pline_The("fountain bubbles furiously for a moment, then calms."); */ // TODO DE
}

STATIC_OVL
void
dowaterdemon() /* Water demon */
{
    register struct monst *mtmp;

    if(!(mvitals[PM_WATER_DEMON].mvflags & G_GONE)) {
	if((mtmp = makemon(&mons[PM_WATER_DEMON],u.ux,u.uy, NO_MM_FLAGS))) {
	    if (!Blind)
		You("VERB_ENTFESSELN OBJECT %s!", a_monnam(mtmp)); /* EN You("unleash %s!", a_monnam(mtmp)); */
	    else
		Du_spuerst("die Präsenz des Bösen."); /* EN You_feel("the presence of evil."); */

	/* Give those on low levels a (slightly) better chance of survival */
	    if (rnd(100) > (80 + level_difficulty())) {
		pline("Aus Dankbarkeit für KASUS_AKKUSATIV %s NOUN_BEFREIUNG NEUER_SATZ SUBJECT_IM_SATZ VERB_GEWAEHREN %s OBJECT KASUS_DATIV PRONOMEN_PERSONAL einen Wunsch!", /* EN pline("Grateful for %s release, %s grants you a wish!", */
		      mhis(mtmp), mhe(mtmp));
		makewish();
		mongone(mtmp);
	    } else if (t_at(mtmp->mx, mtmp->my))
		(void) mintrap(mtmp);
	}
    } else
	pline_The("fountain bubbles furiously for a moment, then calms."); /* EN pline_The("fountain bubbles furiously for a moment, then calms."); */ // TODO DE
}

STATIC_OVL void
dowaternymph() /* Water Nymph */
{
	register struct monst *mtmp;

	if(!(mvitals[PM_WATER_NYMPH].mvflags & G_GONE) &&
	   (mtmp = makemon(&mons[PM_WATER_NYMPH],u.ux,u.uy, NO_MM_FLAGS))) {
		if (!Blind)
		   You("attract %s!", a_monnam(mtmp)); /* EN You("attract %s!", a_monnam(mtmp)); */ // TODO DE
		else
		   You_hear("eine verführerische Stimme."); /* EN You_hear("a seductive voice."); */
		mtmp->msleeping = 0;
		if (t_at(mtmp->mx, mtmp->my))
		    (void) mintrap(mtmp);
	} else
		if (!Blind)
		   pline("Eine große Luftblase steigt zur Oberfläche auf und zerplatzt."); /* EN pline("A large bubble rises to the surface and pops."); */
		else
		   You_hear("einen lauten Knall."); /* EN You_hear("a loud pop."); */
}

void
dogushforth(drinking) /* Gushing forth along LOS from (u.ux, u.uy) */
int drinking;
{
	int madepool = 0;

	do_clear_area(u.ux, u.uy, 7, gush, (genericptr_t)&madepool);
	if (!madepool) {
	    if (drinking)
		Your("NOUN_DURST VERB_SEIN gestillt."); /* EN Your("thirst is quenched."); */
	    else
		pline("Water sprays all over you."); /* EN pline("Water sprays all over you."); */ // TODO DE
	}
}

STATIC_PTR void
gush(x, y, poolcnt)
int x, y;
genericptr_t poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;

	if (((x+y)%2) || (x == u.ux && y == u.uy) ||
	    (rn2(1 + distmin(u.ux, u.uy, x, y)))  ||
	    (levl[x][y].typ != ROOM) ||
	    (sobj_at(BOULDER, x, y)) || nexttodoor(x, y))
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	if (!((*(int *)poolcnt)++))
	    pline("Water gushes forth from the overflowing fountain!"); /* EN pline("Water gushes forth from the overflowing fountain!"); */ // TODO DE

	/* Put a pool at x, y */
	levl[x][y].typ = POOL;
	/* No kelp! */
	del_engr_at(x, y);
	water_damage(level.objects[x][y], FALSE, TRUE);

	if ((mtmp = m_at(x, y)) != 0)
		(void) minliquid(mtmp);
	else
		newsym(x,y);
}

STATIC_OVL void
dofindgem() /* Find a gem in the sparkling waters. */
{
	if (!Blind) You("spot a gem in the sparkling waters!"); /* EN if (!Blind) You("spot a gem in the sparkling waters!"); */ // TODO DE
	else You_feel("a gem here!"); /* EN else You_feel("a gem here!"); */ // TODO DE
	(void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL, LUCKSTONE-1),
			 u.ux, u.uy, FALSE, FALSE);
	SET_FOUNTAIN_LOOTED(u.ux,u.uy);
	newsym(u.ux, u.uy);
	exercise(A_WIS, TRUE);			/* a discovery! */
}

void
dryup(x, y, isyou)
xchar x, y;
boolean isyou;
{
	if (IS_FOUNTAIN(levl[x][y].typ) &&
	    (!rn2(3) || FOUNTAIN_IS_WARNED(x,y))) {
		if(isyou && in_town(x, y) && !FOUNTAIN_IS_WARNED(x,y)) {
			struct monst *mtmp;
			SET_FOUNTAIN_WARNED(x,y);
			/* Warn about future fountain use. */
			for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
			    if (DEADMONSTER(mtmp)) continue;
			    if ((mtmp->data == &mons[PM_WATCHMAN] ||
				mtmp->data == &mons[PM_WATCH_CAPTAIN]) &&
			       couldsee(mtmp->mx, mtmp->my) &&
			       mtmp->mpeaceful) {
				pline("SUBJECT %s VERB_BRUELLEN:", Amonnam(mtmp)); /* EN pline("%s yells:", Amonnam(mtmp)); */
				verbalize("Hey, stop using that fountain!"); /* EN verbalize("Hey, stop using that fountain!"); */ // TODO DE
				break;
			    }
			}
			/* You can see or hear this effect */
			if(!mtmp) pline_The("flow reduces to a trickle."); /* EN if(!mtmp) pline_The("flow reduces to a trickle."); */ // TODO DE
			return;
		}
#ifdef WIZARD
		if (isyou && wizard) {
			if (yn("NOUN_FOUNTAIN austrocken?") == 'n') /* EN if (yn("Dry up fountain?") == 'n') */
				return;
		}
#endif
		/* replace the fountain with ordinary floor */
		levl[x][y].typ = ROOM;
		levl[x][y].looted = 0;
		levl[x][y].blessedftn = 0;
		if (cansee(x,y)) pline_The("NOUN_FOUNTAIN versiegt!"); /* EN if (cansee(x,y)) pline_The("fountain dries up!"); */
		/* The location is seen if the hero/monster is invisible */
		/* or felt if the hero is blind.			 */
		newsym(x, y);
		level.flags.nfountains--;
		if(isyou && in_town(x, y))
		    (void) angry_guards(FALSE);
	}
}

void
drinkfountain()
{
	/* What happens when you drink from a fountain? */
	register boolean mgkftn = (levl[u.ux][u.uy].blessedftn == 1);
	register int fate = rnd(30);

	if (Levitation) {
		floating_above("NOUN_FOUNTAIN"); /* EN floating_above("fountain"); */
		return;
	}

	if (mgkftn && u.uluck >= 0 && fate >= 10) {
		int i, ii, littleluck = (u.uluck < 4);

		pline("Wau!  SUBJECT PRONOMEN_PERSONAL VERB_FUEHLEN OBJECT PRONOMEN_PERSONAL grossartig!"); /* EN pline("Wow!  This makes you feel great!"); */
		/* blessed restore ability */
		for (ii = 0; ii < A_MAX; ii++)
		    if (ABASE(ii) < AMAX(ii)) {
			ABASE(ii) = AMAX(ii);
			flags.botl = 1;
		    }
		/* gain ability, blessed if "natural" luck is high */
		i = rn2(A_MAX);		/* start at a random attribute */
		for (ii = 0; ii < A_MAX; ii++) {
		    if (adjattrib(i, 1, littleluck ? -1 : 0) && littleluck)
			break;
		    if (++i >= A_MAX) i = 0;
		}
		display_nhwindow(WIN_MESSAGE, FALSE);
		pline("A wisp of vapor escapes the fountain..."); /* EN pline("A wisp of vapor escapes the fountain..."); */ // TODO DE
		exercise(A_WIS, TRUE);
		levl[u.ux][u.uy].blessedftn = 0;
		return;
	}

	if (fate < 10) {
		pline_The("cool draught refreshes you."); /* EN pline_The("cool draught refreshes you."); */ // TODO DE
		u.uhunger += rnd(10); /* don't choke on water */
		newuhs(FALSE);
		if(mgkftn) return;
	} else {
	    switch (fate) {

		case 19: /* Self-knowledge */

			You_feel("self-knowledgeable..."); /* EN You_feel("self-knowledgeable..."); */ // TODO DE
			display_nhwindow(WIN_MESSAGE, FALSE);
			enlightenment(0);
			exercise(A_WIS, TRUE);
			pline_The("feeling subsides."); /* EN pline_The("feeling subsides."); */ // TODO DE
			break;

		case 20: /* Foul water */

			pline_The("NOUN_WATER VERB_SEIN eklig!  NEUER_SATZ SUBJECT PRONOMEN_PERSONAL VERB_WUERGEN und VERB_UEBERGEBEN OBJECT PRONOMEN_PERSONAL."); /* EN pline_The("water is foul!  You gag and vomit."); */
			morehungry(rn1(20, 11));
			vomit();
			break;

		case 21: /* Poisonous */

			pline_The("NOUN_WATER ist kontaminiert!"); /* EN pline_The("water is contaminated!"); */
			if (Poison_resistance) {
			   pline(
			      "Perhaps it is runoff from the nearby %s farm.", /* EN "Perhaps it is runoff from the nearby %s farm.", */ // TODO DE
				 fruitname(FALSE));
			   losehp(rnd(4),"unrefrigerated sip of juice", /* EN losehp(rnd(4),"unrefrigerated sip of juice", */ // TODO DE
				KILLED_BY_AN);
			   break;
			}
			losestr(rn1(4,3));
			losehp(rnd(10),"ADJEKTIV_KONTAMINIERT NOUN_WATER", KILLED_BY); /* EN losehp(rnd(10),"contaminated water", KILLED_BY); */
			exercise(A_CON, FALSE);
			break;

		case 22: /* Fountain of snakes! */

			dowatersnakes();
			break;

		case 23: /* Water demon */
			dowaterdemon();
			break;

		case 24: /* Curse an item */ {
			register struct obj *obj;

			pline("Dieses Wasser ist schlecht!"); /* EN pline("This water's no good!"); */
			morehungry(rn1(20, 11));
			exercise(A_CON, FALSE);
			for(obj = invent; obj ; obj = obj->nobj)
				if (!rn2(5))	curse(obj);
			break;
			}

		case 25: /* See invisible */

			if (Blind) {
			    if (Invisible) {
				Du_fuehlst_dich(" durchsichtig."); /* EN You("feel transparent."); */
			    } else {
			    	Du_fuehlst_dich("äusserst selbst-bewusst."); /* EN You("feel very self-conscious."); */
			    	pline("Then it passes."); /* EN pline("Then it passes."); */ // TODO DE
			    }
			} else {
			   You("see an image of someone stalking you."); /* EN You("see an image of someone stalking you."); */ // TODO DE
			   pline("But it disappears."); /* EN pline("But it disappears."); */ // TODO DE
			}
			HSee_invisible |= FROMOUTSIDE;
			newsym(u.ux,u.uy);
			exercise(A_WIS, TRUE);
			break;

		case 26: /* See Monsters */

			(void) monster_detect((struct obj *)0, 0);
			exercise(A_WIS, TRUE);
			break;

		case 27: /* Find a gem in the sparkling waters. */

			if (!FOUNTAIN_IS_LOOTED(u.ux,u.uy)) {
				dofindgem();
				break;
			}

		case 28: /* Water Nymph */

			dowaternymph();
			break;

		case 29: /* Scare */ {
			register struct monst *mtmp;

			pline("Von diesem Wasser VERB_BEKOMMEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL üblen Mundgeruch!"); /* EN pline("This water gives you bad breath!"); */
			for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
			    if(!DEADMONSTER(mtmp))
				monflee(mtmp, 0, FALSE, FALSE);
			}
			break;

		case 30: /* Gushing forth in this room */

			dogushforth(TRUE);
			break;

		default:

			pline("Das Wasser ist lauwarm und geschmacklos."); /* EN pline("This tepid water is tasteless."); */
			break;
	    }
	}
	dryup(u.ux, u.uy, TRUE);
}

void
dipfountain(obj)
register struct obj *obj;
{
	if (Levitation) {
		floating_above("NOUN_FOUNTAIN"); /* EN floating_above("fountain"); */
		return;
	}

	/* Don't grant Excalibur when there's more than one object.  */
	/* (quantity could be > 1 if merged daggers got polymorphed) */
	if (obj->otyp == LONG_SWORD && obj->quan == 1L
	    && u.ulevel >= 5 && !rn2(6)
	    && !obj->oartifact
	    && !exist_artifact(LONG_SWORD, artiname(ART_EXCALIBUR))) {

		if (u.ualign.type != A_LAWFUL) {
			/* Ha!  Trying to cheat her. */
			pline("A freezing mist rises from the water and envelopes the sword."); /* EN pline("A freezing mist rises from the water and envelopes the sword."); */ // TODO DE
			pline_The("NOUN_FOUNTAIN VERB_VERSCHWINDEN!"); /* EN pline_The("fountain disappears!"); */
			curse(obj);
			if (obj->spe > -6 && !rn2(3)) obj->spe--;
			obj->oerodeproof = FALSE;
			exercise(A_WIS, FALSE);
		} else {
			/* The lady of the lake acts! - Eric Backus */
			/* Be *REAL* nice */
	  pline("From the murky depths, a hand reaches up to bless the sword."); /* EN pline("From the murky depths, a hand reaches up to bless the sword."); */ // TODO DE
			pline("As the hand retreats, the fountain disappears!"); /* EN pline("As the hand retreats, the fountain disappears!"); */ // TODO DE
			obj = oname(obj, artiname(ART_EXCALIBUR));
			discover_artifact(ART_EXCALIBUR);
			bless(obj);
			obj->oeroded = obj->oeroded2 = 0;
			obj->oerodeproof = TRUE;
			exercise(A_WIS, TRUE);
		}
		update_inventory();
		levl[u.ux][u.uy].typ = ROOM;
		levl[u.ux][u.uy].looted = 0;
		newsym(u.ux, u.uy);
		level.flags.nfountains--;
		if(in_town(u.ux, u.uy))
		    (void) angry_guards(FALSE);
		return;
	} else (void) get_wet(obj);

	/* Acid and water don't mix */
	if (obj->otyp == POT_ACID) {
	    useup(obj);
	    return;
	}

	switch (rnd(30)) {
		case 16: /* Curse the item */
			curse(obj);
			break;
		case 17:
		case 18:
		case 19:
		case 20: /* Uncurse the item */
			if(obj->cursed) {
			    if (!Blind)
				pline_The("water glows for a moment."); /* EN pline_The("water glows for a moment."); */ // TODO DE
			    uncurse(obj);
			} else {
			    Dich("überkommt ein Gefühl des Verlustes."); /* EN pline("A feeling of loss comes over you."); */
			}
			break;
		case 21: /* Water Demon */
			dowaterdemon();
			break;
		case 22: /* Water Nymph */
			dowaternymph();
			break;
		case 23: /* an Endless Stream of Snakes */
			dowatersnakes();
			break;
		case 24: /* Find a gem */
			if (!FOUNTAIN_IS_LOOTED(u.ux,u.uy)) {
				dofindgem();
				break;
			}
		case 25: /* Water gushes forth */
			dogushforth(FALSE);
			break;
		case 26: /* Strange feeling */
			pline("A strange tingling runs up your %s.", /* EN pline("A strange tingling runs up your %s.", */ // TODO DE
							body_part(ARM));
			break;
		case 27: /* Strange feeling */
			You_feel("a sudden chill."); /* EN You_feel("a sudden chill."); */ // TODO DE
			break;
		case 28: /* Strange feeling */
			pline("An urge to take a bath overwhelms you."); /* EN pline("An urge to take a bath overwhelms you."); */ // TODO DE
#ifndef GOLDOBJ
			if (u.ugold > 10) {
			    u.ugold -= somegold() / 10;
			    You("lost some of your gold in the fountain!"); /* EN You("lost some of your gold in the fountain!"); */ // TODO DE
			    CLEAR_FOUNTAIN_LOOTED(u.ux,u.uy);
			    exercise(A_WIS, FALSE);
			}
#else
			{
			    long money = money_cnt(invent);
			    struct obj *otmp;
                            if (money > 10) {
				/* Amount to loose.  Might get rounded up as fountains don't pay change... */
			        money = somegold(money) / 10; 
			        for (otmp = invent; otmp && money > 0; otmp = otmp->nobj) if (otmp->oclass == COIN_CLASS) {
				    int denomination = objects[otmp->otyp].oc_cost;
				    long coin_loss = (money + denomination - 1) / denomination;
                                    coin_loss = min(coin_loss, otmp->quan);
				    otmp->quan -= coin_loss;
				    money -= coin_loss * denomination;				  
				    if (!otmp->quan) delobj(otmp);
				}
			        You("lost some of your money in the fountain!"); /* EN You("lost some of your money in the fountain!"); */ // TODO DE
				CLEAR_FOUNTAIN_LOOTED(u.ux,u.uy);
			        exercise(A_WIS, FALSE);
                            }
			}
#endif
			break;
		case 29: /* You see coins */

		/* We make fountains have more coins the closer you are to the
		 * surface.  After all, there will have been more people going
		 * by.	Just like a shopping mall!  Chris Woodbury  */

		    if (FOUNTAIN_IS_LOOTED(u.ux,u.uy)) break;
		    SET_FOUNTAIN_LOOTED(u.ux,u.uy);
		    (void) mkgold((long)
			(rnd((dunlevs_in_dungeon(&u.uz)-dunlev(&u.uz)+1)*2)+5),
			u.ux, u.uy);
		    if (!Blind)
		pline("Far below you, you see coins glistening in the water."); /* EN pline("Far below you, you see coins glistening in the water."); */ // TODO DE
		    exercise(A_WIS, TRUE);
		    newsym(u.ux,u.uy);
		    break;
	}
	update_inventory();
	dryup(u.ux, u.uy, TRUE);
}

#ifdef SINKS
void
breaksink(x,y)
int x, y;
{
    if(cansee(x,y) || (x == u.ux && y == u.uy))
	pline_The("pipes break!  Water spurts out!"); /* EN pline_The("pipes break!  Water spurts out!"); */ // TODO DE
    level.flags.nsinks--;
    levl[x][y].doormask = 0;
    levl[x][y].typ = FOUNTAIN;
    level.flags.nfountains++;
    newsym(x,y);
}

void
drinksink()
{
	struct obj *otmp;
	struct monst *mtmp;

	if (Levitation) {
		floating_above("NOUN_SINK"); /* EN floating_above("sink"); */
		return;
	}
	switch(rn2(20)) {
		case 0: You("VERB_NEHMEN einen Schluck sehr kalten Wassers."); /* EN case 0: You("take a sip of very cold water."); */
			break;
		case 1: You("VERB_NEHMEN einen Schluck sehr warmen Wassers."); /* EN case 1: You("take a sip of very warm water."); */
			break;
		case 2: You("VERB_NEHMEN einen Schluck siedend heißen Wassers."); /* EN case 2: You("take a sip of scalding hot water."); */
			if (Fire_resistance)
				pline("It seems quite tasty."); /* EN pline("It seems quite tasty."); */ // TODO DE
			else losehp(rnd(6), "sipping boiling water", KILLED_BY); /* EN else losehp(rnd(6), "sipping boiling water", KILLED_BY); */ // TODO DE
			break;
		case 3: if (mvitals[PM_SEWER_RAT].mvflags & G_GONE)
				pline_The("NOUN_SINK VERB_SEIN ziemlich dreckig."); /* EN pline_The("sink seems quite dirty."); */
			else {
				mtmp = makemon(&mons[PM_SEWER_RAT],
						u.ux, u.uy, NO_MM_FLAGS);
				if (mtmp) pline("Eek!  There's %s in the sink!", /* EN if (mtmp) pline("Eek!  There's %s in the sink!", */ // TODO DE
					(Blind || !canspotmon(mtmp)) ?
					"something squirmy" : /* EN "something squirmy" : */ // TODO DE
					a_monnam(mtmp));
			}
			break;
		case 4: do {
				otmp = mkobj(POTION_CLASS,FALSE);
				if (otmp->otyp == POT_WATER) {
					obfree(otmp, (struct obj *)0);
					otmp = (struct obj *) 0;
				}
			} while(!otmp);
			otmp->cursed = otmp->blessed = 0;
			pline("Some %s liquid flows from the faucet.", /* EN pline("Some %s liquid flows from the faucet.", */ // TODO DE
			      Blind ? "odd" : /* EN Blind ? "odd" : */ // TODO DE
			      hcolor(OBJ_DESCR(objects[otmp->otyp])));
			otmp->dknown = !(Blind || Hallucination);
			otmp->quan++; /* Avoid panic upon useup() */
			otmp->corpsenm = 1; /* kludge for docall() */
			(void) dopotion(otmp);
			obfree(otmp, (struct obj *)0);
			break;
		case 5: if (!(levl[u.ux][u.uy].looted & S_LRING)) {
			    You("VERB_FINDEN einen Ring in OBJECT KASUS_DATIV ARTIKEL_BESTIMMTER NOUN_SINK!"); /* EN You("find a ring in the sink!"); */
			    (void) mkobj_at(RING_CLASS, u.ux, u.uy, TRUE);
			    levl[u.ux][u.uy].looted |= S_LRING;
			    exercise(A_WIS, TRUE);
			    newsym(u.ux,u.uy);
			} else pline("Some dirty water backs up in the drain."); /* EN } else pline("Some dirty water backs up in the drain."); */ // TODO DE
			break;
		case 6: breaksink(u.ux,u.uy);
			break;
		case 7: pline_The("water moves as though of its own will!"); /* EN case 7: pline_The("water moves as though of its own will!"); */ // TODO DE
			if ((mvitals[PM_WATER_ELEMENTAL].mvflags & G_GONE)
			    || !makemon(&mons[PM_WATER_ELEMENTAL],
					u.ux, u.uy, NO_MM_FLAGS))
				pline("But it quiets down."); /* EN pline("But it quiets down."); */ // TODO DE
			break;
		case 8: pline("Yuk, this water tastes awful."); /* EN case 8: pline("Yuk, this water tastes awful."); */ // TODO DE
			more_experienced(1,0);
			newexplevel();
			break;
		case 9: pline("Gaggg... das schmeckt wie Abwasser!  SUBJECT PRONOMEN_PERSONAL VERB_UEBERGEBEN OBJECT PRONOMEN_PERSONAL."); /* EN case 9: pline("Gaggg... this tastes like sewage!  You vomit."); */
			morehungry(rn1(30-ACURR(A_CON), 11));
			vomit();
			break;
		case 10: pline("Das Wasser enthält Giftmüll!"); /* EN case 10: pline("This water contains toxic wastes!"); */
			if (!Unchanging) {
				You("VERB_DURCHLEBEN eine absonderliche Metamorphose!"); /* EN You("undergo a freakish metamorphosis!"); */
				polyself(FALSE);
			}
			break;
		/* more odd messages --JJB */
		case 11: You_hear("clanking from the pipes..."); /* EN case 11: You_hear("clanking from the pipes..."); */ // TODO DE
			break;
		case 12: You_hear("snatches of song from among the sewers..."); /* EN case 12: You_hear("snatches of song from among the sewers..."); */ // TODO DE
			break;
		case 19: if (Hallucination) {
		   pline("From the murky drain, a hand reaches up... --upps--"); /* EN pline("From the murky drain, a hand reaches up... --oops--"); */ // TODO DE
				break;
			}
		default: You("VERB_NEHMEN einen Schluck %s Wassers.", /* EN default: You("take a sip of %s water.", */
			rn2(3) ? (rn2(2) ? "kalten" : "warmen") : "heißen"); /* EN rn2(3) ? (rn2(2) ? "cold" : "warm") : "hot"); */
	}
}
#endif /* SINKS */

/*fountain.c*/
