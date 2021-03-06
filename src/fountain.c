/*	SCCS Id: @(#)fountain.c	3.4	2003/03/23	*/
/*	Copyright Scott R. Turner, srt@ucla, 10/27/86 */
/* NetHack may be freely redistributed.  See license for details. */

/* Code for drinking from fountains. */

#include "hack.h"

#ifdef GERMAN
# include "german.h"
#endif

STATIC_DCL void NDECL(dowatersnakes);
STATIC_DCL void NDECL(dowaterdemon);
STATIC_DCL void NDECL(dowaternymph);
STATIC_PTR void FDECL(gush, (int,int,genericptr_t));
STATIC_DCL void NDECL(dofindgem);

void
floating_above(what)
const char *what;
{
    You("VERB_SCHWEBEN hoch �ber OBJECT KASUS_DATIV ARTIKEL_BESTIMMTER %s.", what); /* EN You("are floating high above the %s.", what); */
}

STATIC_OVL void
dowatersnakes() /* Fountain of snakes! */
{
    register int num = rn1(5,2);
    struct monst *mtmp;

    if (!(mvitals[PM_WATER_MOCCASIN].mvflags & G_GONE)) {
	if (!Blind)
	    pline("Ein endloser Strom von KASUS_DATIV %s quillt hervor!", /* EN pline("An endless stream of %s pours forth!", */
		  Hallucination ? makeplural(rndmonnam()) : "NOUN_SNAKEs"); /* EN Hallucination ? makeplural(rndmonnam()) : "snakes"); */
	else
			You_hear("%s zischen!", something); /* EN You_hear("%s hissing!", something); */
	while(num-- > 0)
	    if((mtmp = makemon(&mons[PM_WATER_MOCCASIN],
			u.ux, u.uy, NO_MM_FLAGS)) && t_at(mtmp->mx, mtmp->my))
		(void) mintrap(mtmp);
    } else
	pline_The("NOUN_FOUNTAIN sprudelt einen Augenblick lang gewaltig und beruhigt sich wieder."); /* EN pline_The("fountain bubbles furiously for a moment, then calms."); */
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
		Du_spuerst("die Pr�senz des B�sen."); /* EN You_feel("the presence of evil."); */

	/* Give those on low levels a (slightly) better chance of survival */
	    if (rnd(100) > (80 + level_difficulty())) {
		pline("Aus Dankbarkeit f�r KASUS_AKKUSATIV %s NOUN_BEFREIUNG NEUER_SATZ SUBJECT_IM_SATZ VERB_GEWAEHREN %s OBJECT KASUS_DATIV PRONOMEN_PERSONAL einen Wunsch!", /* EN pline("Grateful for %s release, %s grants you a wish!", */
		      mhis(mtmp), mhe(mtmp));
		makewish();
		mongone(mtmp);
	    } else if (t_at(mtmp->mx, mtmp->my))
		(void) mintrap(mtmp);
	}
    } else
	pline_The("NOUN_FOUNTAIN blubbert heftig einen Augenblick lang und beruhigt sich wieder."); /* EN pline_The("fountain bubbles furiously for a moment, then calms."); */
}

STATIC_OVL void
dowaternymph() /* Water Nymph */
{
	register struct monst *mtmp;

	if(!(mvitals[PM_WATER_NYMPH].mvflags & G_GONE) &&
	   (mtmp = makemon(&mons[PM_WATER_NYMPH],u.ux,u.uy, NO_MM_FLAGS))) {
		if (!Blind)
		   You("VERB_ANLOCKEN OBJECT %s SATZKLAMMER!", a_monnam(mtmp)); /* EN You("attract %s!", a_monnam(mtmp)); */
		else
		   You_hear("eine verf�hrerische Stimme."); /* EN You_hear("a seductive voice."); */
		mtmp->msleeping = 0;
		if (t_at(mtmp->mx, mtmp->my))
		    (void) mintrap(mtmp);
	} else
		if (!Blind)
		   pline("Eine gro�e Luftblase steigt zur Oberfl�che auf und zerplatzt."); /* EN pline("A large bubble rises to the surface and pops."); */
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
		pline("SUBJECT PRONOMEN_PERSONAL VERB_WERDEN �ber und �ber mit Wasser bespritzt."); /* EN pline("Water sprays all over you."); */
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
	    pline_The("NOUN_FOUNTAIN schwappt �ber!"); /* EN pline("Water gushes forth from the overflowing fountain!"); */

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
	if (!Blind) You("VERB_ENTDECKEN OBJECT ARTIKEL_UNBESTIMMTER NOUN_GEM im glitzernden Wasser!"); /* EN if (!Blind) You("spot a gem in the sparkling waters!"); */
	else You_feel("hier OBJECT ARTIKEL_UNBESTIMMTER NOUN_GEM!"); /* EN else You_feel("a gem here!"); */
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
				verbalize("Hey, MODIFIER_VERB_IMPERATIV VERB_LASSEN OBJECT ARTIKEL_BESTIMMTER NOUN_FOUNTAIN in Ruhe!"); /* EN verbalize("Hey, stop using that fountain!"); */
				break;
			    }
			}
			/* You can see or hear this effect */
			if(!mtmp) pline("Der Strom schrumpft zu einem Rinnsal."); /* EN if(!mtmp) pline_The("flow reduces to a trickle."); */
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

		pline("Wau!  %s gro�artig!", this_makes_you_feel); /* EN pline("Wow!  This makes you feel great!"); */
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
		pline("Eine Dampffont�ne entweicht KASUS_DATIV ARTIKEL_BESTIMMTER NOUN_FOUNTAIN ..."); /* EN pline("A wisp of vapor escapes the fountain..."); */
		exercise(A_WIS, TRUE);
		levl[u.ux][u.uy].blessedftn = 0;
		return;
	}

	if (fate < 10) {
		pline("Der k�hle Schluck erfrischt KASUS_AKKUSATIV PRONOMEN_PERSONAL."); /* EN pline_The("cool draught refreshes you."); */
		u.uhunger += rnd(10); /* don't choke on water */
		newuhs(FALSE);
		if(mgkftn) return;
	} else {
	    switch (fate) {

		case 19: /* Self-knowledge */

			Du_fuehlst_dich("voller Selbsterkenntnis ..."); /* EN You_feel("self-knowledgeable..."); */
			display_nhwindow(WIN_MESSAGE, FALSE);
			enlightenment(0);
			exercise(A_WIS, TRUE);
			pline("Das Gef�hl schwindet."); /* EN pline_The("feeling subsides."); */
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
			      "Vielleicht ist es Abwasser von der nahegelegenen %s-Plantage.", /* EN "Perhaps it is runoff from the nearby %s farm.", */
				 fruitname(FALSE));
			   losehp(rnd(4),"durch einen Schluck ungek�hlten Fruchtsaftes", /* EN losehp(rnd(4),"unrefrigerated sip of juice", */
				KILLED_WITHOUT_PREPOSITION); /* EN KILLED_BY_AN); */
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
				Du_fuehlst_dich("durchsichtig."); /* EN You("feel transparent."); */
			    } else {
			    	Du_fuehlst_dich("�usserst selbst-bewusst."); /* EN You("feel very self-conscious."); */
			    	pline("Doch das geht vorbei."); /* EN pline("Then it passes."); */
			    }
			} else {
			   You("VERB_SEHEN die Umrisse eines heimlichen Verfolgers."); /* EN You("see an image of someone stalking you."); */
			   pline("Aber es verschwindet wieder."); /* EN pline("But it disappears."); */
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

			pline("Von diesem Wasser VERB_BEKOMMEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL �blen Mundgeruch!"); /* EN pline("This water gives you bad breath!"); */
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
			pline("Ein eiskalter Nebel steigt vom Wasser auf und umh�llt das Schwert."); /* EN pline("A freezing mist rises from the water and envelopes the sword."); */
			pline_The("NOUN_FOUNTAIN VERB_VERSCHWINDEN!"); /* EN pline_The("fountain disappears!"); */
			curse(obj);
			if (obj->spe > -6 && !rn2(3)) obj->spe--;
			obj->oerodeproof = FALSE;
			exercise(A_WIS, FALSE);
		} else {
			/* The lady of the lake acts! - Eric Backus */
			/* Be *REAL* nice */
        pline("Aus den tr�ben Tiefen langt eine Hand empor, das Schwert zu segnen."); /* EN pline("From the murky depths, a hand reaches up to bless the sword."); */
			pline("Als sich die Hand zur�ckzieht, verschwindet ARTIKEL_BESTIMMTER NOUN_FOUNTAIN!"); /* EN pline("As the hand retreats, the fountain disappears!"); */
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
	} else if (get_wet(obj) && !rn2(2))
		return;

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
				pline("Das Wasser glitzert einen kurzen Moment lang."); /* EN pline_The("water glows for a moment."); */
			    uncurse(obj);
			} else {
			    Dich("�berkommt ein Gef�hl des Verlustes."); /* EN pline("A feeling of loss comes over you."); */
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
			pline("Ein seltsames Kribbeln l�uft KASUS_DATIV PRONOMEN_PERSONAL KASUS_DATIV PRONOMEN_POSSESSIV %s entlang.", /* EN pline("A strange tingling runs up your %s.", */
							body_part(ARM));
			break;
		case 27: /* Strange feeling */
			Dir("VERB_SEIN pl�tzlich k�hl."); /* EN You_feel("a sudden chill."); */
			break;
		case 28: /* Strange feeling */
			Dich("VERB_UEBERMANNEN die Lust nach einem Bad."); /* EN pline("An urge to take a bath overwhelms you."); */
#ifndef GOLDOBJ
			if (u.ugold > 10) {
			    u.ugold -= somegold() / 10;
			    You("VERB_HABEN etwas OBJECT KASUS_DATIV von PRONOMEN_POSSESSIV NOUN_GOLD im Brunnen verloren!"); /* EN You("lost some of your gold in the fountain!"); */
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
			        You("VERB_HABEN etwas OBJECT KASUS_DATIV von PRONOMEN_POSSESSIV NOUN_GELD im Brunnen verloren!"); /* EN You("lost some of your money in the fountain!"); */
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
		pline("Tief OBJECT KASUS_DATIV unter PRONOMEN_PERSONAL VERB_SEHEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL M�nzen im Wasser glitzern."); /* EN pline("Far below you, you see coins glistening in the water."); */
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
	pline("Die Rohre bersten!  Wasser spritzt heraus!"); /* EN pline_The("pipes break!  Water spurts out!"); */
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
		case 2: You("VERB_NEHMEN einen Schluck siedend hei�en Wassers."); /* EN case 2: You("take a sip of scalding hot water."); */
			if (Fire_resistance)
				pline("Es schmeckt ganz gut."); /* EN pline("It seems quite tasty."); */
			else losehp(rnd(6), "einem Schluck kochendhei�en Wassers", KILLED_BY); /* EN else losehp(rnd(6), "sipping boiling water", KILLED_BY); */
			break;
		case 3: if (mvitals[PM_SEWER_RAT].mvflags & G_GONE)
				pline_The("NOUN_SINK VERB_SEIN ziemlich dreckig."); /* EN pline_The("sink seems quite dirty."); */
			else {
				mtmp = makemon(&mons[PM_SEWER_RAT],
						u.ux, u.uy, NO_MM_FLAGS);
				if (mtmp) pline("Igitt!  Da ist %s in der Sp�le!", /* EN if (mtmp) pline("Eek!  There's %s in the sink!", */
					(Blind || !canspotmon(mtmp)) ?
					"ein Viech" : /* EN "something squirmy" : */
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
			pline("Eine %s Fl�ssigkeit flie�t aus dem Wasserhahn.", /* EN pline("Some %s liquid flows from the faucet.", */
			      Blind ? "merkw�rdige" : /* EN Blind ? "odd" : */
			      hcolor(OBJ_DESCR(objects[otmp->otyp])));
			otmp->dknown = !(Blind || Hallucination);
			otmp->quan++; /* Avoid panic upon useup() */
			otmp->fromsink = 1; /* kludge for docall() */
			(void) dopotion(otmp);
			obfree(otmp, (struct obj *)0);
			break;
		case 5: if (!(levl[u.ux][u.uy].looted & S_LRING)) {
			    You("VERB_FINDEN einen Ring in OBJECT KASUS_DATIV ARTIKEL_BESTIMMTER NOUN_SINK!"); /* EN You("find a ring in the sink!"); */
			    (void) mkobj_at(RING_CLASS, u.ux, u.uy, TRUE);
			    levl[u.ux][u.uy].looted |= S_LRING;
			    exercise(A_WIS, TRUE);
			    newsym(u.ux,u.uy);
			} else pline("Es kommt etwas Dreckwasser aus dem Abflussrohr hoch."); /* EN } else pline("Some dirty water backs up in the drain."); */
			break;
		case 6: breaksink(u.ux,u.uy);
			break;
		case 7: pline("Das Wasser bewegt sich, als bes��e es einen eigenen Willen!"); /* EN case 7: pline_The("water moves as though of its own will!"); */
			if ((mvitals[PM_WATER_ELEMENTAL].mvflags & G_GONE)
			    || !makemon(&mons[PM_WATER_ELEMENTAL],
					u.ux, u.uy, NO_MM_FLAGS))
				pline("Aber es beruhigt sich wieder."); /* EN pline("But it quiets down."); */
			break;
		case 8: pline("Wrg, dieses Wasser schmeckt ekelhaft."); /* EN case 8: pline("Yuk, this water tastes awful."); */
			more_experienced(1,0);
			newexplevel();
			break;
		case 9: pline("Gaggg... das schmeckt wie Abwasser!  SUBJECT PRONOMEN_PERSONAL VERB_UEBERGEBEN OBJECT PRONOMEN_PERSONAL."); /* EN case 9: pline("Gaggg... this tastes like sewage!  You vomit."); */
			morehungry(rn1(30-ACURR(A_CON), 11));
			vomit();
			break;
		case 10: pline("Das Wasser enth�lt Giftm�ll!"); /* EN case 10: pline("This water contains toxic wastes!"); */
			if (!Unchanging) {
				You("VERB_DURCHLEBEN eine absonderliche Metamorphose!"); /* EN You("undergo a freakish metamorphosis!"); */
				polyself(FALSE);
			}
			break;
		/* more odd messages --JJB */
		case 11: You_hear("es in den Rohren scheppern ..."); /* EN case 11: You_hear("clanking from the pipes..."); */
			break;
		case 12: You_hear("Liedfetzen aus der Kanalisation ..."); /* EN case 12: You_hear("snatches of song from among the sewers..."); */
			break;
		case 19: if (Hallucination) {
		   pline("Aus dem dunklen Rohr reckt eine Hand empor ... --huch--"); /* EN pline("From the murky drain, a hand reaches up... --oops--"); */
				break;
			}
		default: You("VERB_NEHMEN einen Schluck %s Wassers.", /* EN default: You("take a sip of %s water.", */
			rn2(3) ? (rn2(2) ? "kalten" : "warmen") : "hei�en"); /* EN rn2(3) ? (rn2(2) ? "cold" : "warm") : "hot"); */
	}
}
#endif /* SINKS */

/*fountain.c*/
