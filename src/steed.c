/*	SCCS Id: @(#)steed.c	3.4	2003/01/10	*/
/* Copyright (c) Kevin Hugo, 1998-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


#ifdef STEED

/* Monsters that might be ridden */
static NEARDATA const char steeds[] = {
	S_QUADRUPED, S_UNICORN, S_ANGEL, S_CENTAUR, S_DRAGON, S_JABBERWOCK, '\0'
};

STATIC_DCL boolean FDECL(landing_spot, (coord *, int, int));

/* caller has decided that hero can't reach something while mounted */
void
rider_cant_reach()
{
     You("aren't skilled enough to reach from %s.", y_monnam(u.usteed)); /* EN You("aren't skilled enough to reach from %s.", y_monnam(u.usteed)); */ // TODO DE
}

/*** Putting the saddle on ***/

/* Can this monster wear a saddle? */
boolean
can_saddle(mtmp)
	struct monst *mtmp;
{
	struct permonst *ptr = mtmp->data;

	return (index(steeds, ptr->mlet) && (ptr->msize >= MZ_MEDIUM) &&
			(!humanoid(ptr) || ptr->mlet == S_CENTAUR) &&
			!amorphous(ptr) && !noncorporeal(ptr) &&
			!is_whirly(ptr) && !unsolid(ptr));
}


int
use_saddle(otmp)
	struct obj *otmp;
{
	struct monst *mtmp;
	struct permonst *ptr;
	int chance;
	const char *s;


	/* Can you use it? */
	if (nohands(youmonst.data)) {
		You("VERB_HAVE keine Hände!");	/* not `body_part(HAND)' */ /* EN You("have no hands!");	*/
		return 0;
	} else if (!freehand()) {
		You("VERB_HAVE OBJECT PRONOMEN_KEIN %s frei.", body_part(HAND)); /* EN You("have no free %s.", body_part(HAND)); */
		return 0;
	}

	/* Select an animal */
	if (u.uswallow || Underwater || !getdir((char *)0)) {
	    pline(Never_mind);
	    return 0;
	}
	if (!u.dx && !u.dy) {
	    pline("Reiterspielchen?  SUBJECT PRONOMEN_PERSONAL MODIFIER_KONJUNKTIV_II VERB_SOLLEN OBJECT PRONOMEN_PERSONAL was schämen ..."); /* EN pline("Saddle yourself?  Very funny..."); */
	    return 0;
	}
	if (!isok(u.ux+u.dx, u.uy+u.dy) ||
			!(mtmp = m_at(u.ux+u.dx, u.uy+u.dy)) ||
			!canspotmon(mtmp)) {
	    pline("Ich sehe da niemanden."); /* EN pline("I see nobody there."); */
	    return 1;
	}

	/* Is this a valid monster? */
	if (mtmp->misc_worn_check & W_SADDLE ||
			which_armor(mtmp, W_SADDLE)) {
	    pline("SUBJECT %s VERB_BRAUCHEN nicht noch einen.", Monnam(mtmp)); /* EN pline("%s doesn't need another one.", Monnam(mtmp)); */
	    return 1;
	}
	ptr = mtmp->data;
	if (touch_petrifies(ptr) && !uarmg && !Stone_resistance) {
	    char kbuf[BUFSZ];

	    You("VERB_TOUCH OBJECT %s.", mon_nam(mtmp)); /* EN You("touch %s.", mon_nam(mtmp)); */
 	    if (!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
		Sprintf(kbuf, "attempting to saddle %s", an(mtmp->data->mname)); /* EN Sprintf(kbuf, "attempting to saddle %s", an(mtmp->data->mname)); */ // TODO DE
		instapetrify(kbuf);
 	    }
	}
	if (ptr == &mons[PM_INCUBUS] || ptr == &mons[PM_SUCCUBUS]) {
	    pline("SUBJECT PRONOMEN_PERSONAL MODIFIER_KONJUNKTIV_II VERB_SOLLEN OBJECT PRONOMEN_PERSONAL was schämen!"); /* EN pline("Shame on you!"); */
	    exercise(A_WIS, FALSE);
	    return 1;
	}
	if (mtmp->isminion || mtmp->isshk || mtmp->ispriest ||
			mtmp->isgd || mtmp->iswiz) {
	    pline("Ich glaube, %s hätte was dagegen.", mon_nam(mtmp)); /* EN pline("I think %s would mind.", mon_nam(mtmp)); */
	    return 1;
	}
	if (!can_saddle(mtmp)) {
		You("VERB_KOENNEN so eine Kreatur nicht satteln."); /* EN You_cant("saddle such a creature."); */
		return 1;
	}

	/* Calculate your chance */
	chance = ACURR(A_DEX) + ACURR(A_CHA)/2 + 2*mtmp->mtame;
	chance += u.ulevel * (mtmp->mtame ? 20 : 5);
	if (!mtmp->mtame) chance -= 10*mtmp->m_lev;
	if (Role_if(PM_KNIGHT))
	    chance += 20;
	switch (P_SKILL(P_RIDING)) {
	case P_ISRESTRICTED:
	case P_UNSKILLED:
	default:
	    chance -= 20;	break;
	case P_BASIC:
	    break;
	case P_SKILLED:
	    chance += 15;	break;
	case P_EXPERT:
	    chance += 30;	break;
	}
	if (Confusion || Fumbling || Glib)
	    chance -= 20;
	else if (uarmg &&
		(s = OBJ_DESCR(objects[uarmg->otyp])) != (char *)0 &&
		!strncmp(s, "NOUN_RIDING_", 12)) /* EN !strncmp(s, "riding ", 7)) */
	    /* Bonus for wearing "riding" (but not fumbling) gloves */
	    chance += 10;
	else if (uarmf &&
		(s = OBJ_DESCR(objects[uarmf->otyp])) != (char *)0 &&
		!strncmp(s, "NOUN_RIDING_", 12)) /* EN !strncmp(s, "riding ", 7)) */
	    /* ... or for "riding boots" */
	    chance += 10;
	if (otmp->cursed)
	    chance -= 50;

	/* Make the attempt */
	if (rn2(100) < chance) {
	    You("put the saddle on %s.", mon_nam(mtmp)); /* EN You("put the saddle on %s.", mon_nam(mtmp)); */ // TODO DE
	    if (otmp->owornmask) remove_worn_item(otmp, FALSE);
	    freeinv(otmp);
	    /* mpickobj may free otmp it if merges, but we have already
	       checked for a saddle above, so no merger should happen */
	    (void) mpickobj(mtmp, otmp);
	    mtmp->misc_worn_check |= W_SADDLE;
	    otmp->owornmask = W_SADDLE;
	    otmp->leashmon = mtmp->m_id;
	    update_mon_intrinsics(mtmp, otmp, TRUE, FALSE);
	} else
	    pline("%s resists!", Monnam(mtmp)); /* EN pline("%s resists!", Monnam(mtmp)); */ // TODO DE
	return 1;
}


/*** Riding the monster ***/

/* Can we ride this monster?  Caller should also check can_saddle() */
boolean
can_ride(mtmp)
	struct monst *mtmp;
{
	return (mtmp->mtame && humanoid(youmonst.data) &&
			!verysmall(youmonst.data) && !bigmonst(youmonst.data) &&
			(!Underwater || is_swimmer(mtmp->data)));
}


int
doride()
{
	boolean forcemount = FALSE;

	if (u.usteed)
	    dismount_steed(DISMOUNT_BYCHOICE);
	else if (getdir((char *)0) && isok(u.ux+u.dx, u.uy+u.dy)) {
#ifdef WIZARD
	if (wizard && yn("Force the mount to succeed?") == 'y')
		forcemount = TRUE;
#endif
	    return (mount_steed(m_at(u.ux+u.dx, u.uy+u.dy), forcemount));
	} else
	    return 0;
	return 1;
}


/* Start riding, with the given monster */
boolean
mount_steed(mtmp, force)
	struct monst *mtmp;	/* The animal */
	boolean force;		/* Quietly force this animal */
{
	struct obj *otmp;
	char buf[BUFSZ];
	struct permonst *ptr;

	/* Sanity checks */
	if (u.usteed) {
	    You("VERB_REITEN bereits OBJECT KASUS_DATIV auf %s.", mon_nam(u.usteed)); /* EN You("are already riding %s.", mon_nam(u.usteed)); */
	    return (FALSE);
	}

	/* Is the player in the right form? */
	if (Hallucination && !force) {
	    pline("Maybe you should find a designated driver."); /* EN pline("Maybe you should find a designated driver."); */ // TODO DE
	    return (FALSE);
	}
	/* While riding Wounded_legs refers to the steed's,
	 * not the hero's legs.
	 * That opens up a potential abuse where the player
	 * can mount a steed, then dismount immediately to
	 * heal leg damage, because leg damage is always
	 * healed upon dismount (Wounded_legs context switch).
	 * By preventing a hero with Wounded_legs from
	 * mounting a steed, the potential for abuse is
	 * minimized, if not eliminated altogether.
	 */
	if (Wounded_legs) {
	    Your("%s VERB_SEIN nicht zum Reiten zu gebrauchen.", makeplural(body_part(LEG))); /* EN Your("%s are in no shape for riding.", makeplural(body_part(LEG))); */
#ifdef WIZARD
	    if (force && wizard && yn("SUBJECT PRONOMEN_POSSESSIV NOUN_LEGs heilen?") == 'y') /* EN if (force && wizard && yn("Heal your legs?") == 'y') */
		HWounded_legs = EWounded_legs = 0;
	    else
#endif
	    return (FALSE);
	}

	if (Upolyd && (!humanoid(youmonst.data) || verysmall(youmonst.data) ||
			bigmonst(youmonst.data) || slithy(youmonst.data))) {
	    You("VERB_PASSEN nicht auf einen Sattel."); /* EN You("won't fit on a saddle."); */
	    return (FALSE);
	}
	if(!force && (near_capacity() > SLT_ENCUMBER)) {
	    You_cant("do that while carrying so much stuff."); /* EN You_cant("do that while carrying so much stuff."); */ // TODO DE
	    return (FALSE);
	}

	/* Can the player reach and see the monster? */
	if (!mtmp || (!force && ((Blind && !Blind_telepat) ||
		mtmp->mundetected ||
		mtmp->m_ap_type == M_AP_FURNITURE ||
		mtmp->m_ap_type == M_AP_OBJECT))) {
	    pline("Ich sehe da niemanden."); /* EN pline("I see nobody there."); */
	    return (FALSE);
	}
	if (u.uswallow || u.ustuck || u.utrap || Punished ||
	    !test_move(u.ux, u.uy, mtmp->mx-u.ux, mtmp->my-u.uy, TEST_MOVE)) {
	    if (Punished || !(u.uswallow || u.ustuck || u.utrap))
		You("are unable to swing your %s over.", body_part(LEG)); /* EN You("are unable to swing your %s over.", body_part(LEG));  */ // TODO DE
	    else
		You("are stuck here for now."); /* EN You("are stuck here for now."); */ // TODO DE
	    return (FALSE);
	}

	/* Is this a valid monster? */
	otmp = which_armor(mtmp, W_SADDLE);
	if (!otmp) {
	    pline("SUBJECT %s VERB_SEIN nicht ADJEKTIV_SADDLED.", Monnam(mtmp)); /* EN  pline("%s is not saddled.", Monnam(mtmp)); */
	    return (FALSE);
	}
	ptr = mtmp->data;
	if (touch_petrifies(ptr) && !Stone_resistance) {
	    char kbuf[BUFSZ];

	    You("VERB_TOUCH OBJECT %s.", mon_nam(mtmp)); /* EN You("touch %s.", mon_nam(mtmp)); */
	    Sprintf(kbuf, "attempting to ride %s", an(mtmp->data->mname)); /* EN Sprintf(kbuf, "attempting to ride %s", an(mtmp->data->mname)); */ // TODO DE
	    instapetrify(kbuf);
	}
	if (!mtmp->mtame || mtmp->isminion) {
	    pline("Ich glaube, %s hätte was dagegen.", mon_nam(mtmp)); /* EN pline("I think %s would mind.", mon_nam(mtmp)); */
	    return (FALSE);
	}
	if (mtmp->mtrapped) {
	    struct trap *t = t_at(mtmp->mx, mtmp->my);

	    You_cant("VERB_MOUNT %s while %s's trapped in %s.", /* EN You_cant("mount %s while %s's trapped in %s.", */ // TODO DE
		     mon_nam(mtmp), mhe(mtmp),
		     an(defsyms[trap_to_defsym(t->ttyp)].explanation));
	    return (FALSE);
	}

	if (!force && !Role_if(PM_KNIGHT) && !(--mtmp->mtame)) {
	    /* no longer tame */
	    newsym(mtmp->mx, mtmp->my);
	    pline("%s resists%s!", Monnam(mtmp), /* EN pline("%s resists%s!", Monnam(mtmp), */ // TODO DE
		  mtmp->mleashed ? " and its leash comes off" : ""); /* EN mtmp->mleashed ? " and its leash comes off" : ""); */ // TODO DE
	    if (mtmp->mleashed) m_unleash(mtmp, FALSE);
	    return (FALSE);
	}
	if (!force && Underwater && !is_swimmer(ptr)) {
	    You("VERB_KOENNEN unter Wasser nicht diese Kreatur reiten."); /* EN You_cant("ride that creature while under water."); */
	    return (FALSE);
	}
	if (!can_saddle(mtmp) || !can_ride(mtmp)) {
	    You("VERB_KOENNEN diese Kreatur nicht reiten."); /* EN You_cant("ride such a creature."); */
	    return (0);
	}

	/* Is the player impaired? */
	if (!force && !is_floater(ptr) && !is_flyer(ptr) &&
			Levitation && !Lev_at_will) {
	    You("cannot reach %s.", mon_nam(mtmp)); /* EN You("cannot reach %s.", mon_nam(mtmp)); */ // TODO DE
	    return (FALSE);
	}
	if (!force && uarm && is_metallic(uarm) &&
			greatest_erosion(uarm)) {
	    Your("%s armor is too stiff to be able to mount %s.", /* EN Your("%s armor is too stiff to be able to mount %s.", */ // TODO DE
			uarm->oeroded ? "ADJEKTIV_ROSTIG" : "ADJEKTIV_CORRODED", /* EN uarm->oeroded ? "rusty" : "corroded", */
			mon_nam(mtmp));
	    return (FALSE);
	}
	if (!force && (Confusion || Fumbling || Glib || Wounded_legs ||
		otmp->cursed || (u.ulevel+mtmp->mtame < rnd(MAXULEV/2+5)))) {
	    if (Levitation) {
		pline("%s slips away from you.", Monnam(mtmp)); /* EN pline("%s slips away from you.", Monnam(mtmp)); */ // TODO DE
		return FALSE;
	    }
	    You("slip while trying to get on %s.", mon_nam(mtmp)); /* EN You("slip while trying to get on %s.", mon_nam(mtmp)); */ // TODO DE

	    Sprintf(buf, "slipped while mounting %s", /* EN Sprintf(buf, "slipped while mounting %s", */ // TODO DE
		    /* "a saddled mumak" or "a saddled pony called Dobbin" */
		    x_monnam(mtmp, ARTICLE_A, (char *)0,
			SUPPRESS_IT|SUPPRESS_INVISIBLE|SUPPRESS_HALLUCINATION,
			     TRUE));
	    losehp(rn1(5,10), buf, NO_KILLER_PREFIX);
	    return (FALSE);
	}

	/* Success */
	if (!force) {
	    if (Levitation && !is_floater(ptr) && !is_flyer(ptr))
	    	/* Must have Lev_at_will at this point */
	    	pline("%s magically floats up!", Monnam(mtmp)); /* EN pline("%s magically floats up!", Monnam(mtmp)); */ // TODO DE
	    You("VERB_MOUNT OBJECT %s.", mon_nam(mtmp)); /* EN You("mount %s.", mon_nam(mtmp)); */
	}
	/* setuwep handles polearms differently when you're mounted */
	if (uwep && is_pole(uwep)) unweapon = FALSE;
	u.usteed = mtmp;
	remove_monster(mtmp->mx, mtmp->my);
	teleds(mtmp->mx, mtmp->my, TRUE);
	return (TRUE);
}


/* You and your steed have moved */
void
exercise_steed()
{
	if (!u.usteed)
		return;

	/* It takes many turns of riding to exercise skill */
	if (u.urideturns++ >= 100) {
	    u.urideturns = 0;
	    use_skill(P_RIDING, 1);
	}
	return;
}


/* The player kicks or whips the steed */
void
kick_steed()
{
	char He[255]; /* EN char He[4]; */
	if (!u.usteed)
	    return;

	/* [ALI] Various effects of kicking sleeping/paralyzed steeds */
	if (u.usteed->msleeping || !u.usteed->mcanmove) {
	    /* We assume a message has just been output of the form
	     * "You kick <steed>."
	     */
	    Strcpy(He, mhe(u.usteed));
	    *He = highc(*He);
	    if ((u.usteed->mcanmove || u.usteed->mfrozen) && !rn2(2)) {
		if (u.usteed->mcanmove)
		    u.usteed->msleeping = 0;
		else if (u.usteed->mfrozen > 2)
		    u.usteed->mfrozen -= 2;
		else {
		    u.usteed->mfrozen = 0;
		    u.usteed->mcanmove = 1;
		}
		if (u.usteed->msleeping || !u.usteed->mcanmove)
		    pline("SUBJECT %s VERB_RUEHREN sich nicht.", He); /* EN pline("%s stirs.", He); */
		else
		    pline("SUBJECT %s VERB_AUFSCHRECKEN SATZKLAMMER!", He); /* EN pline("%s rouses %sself!", He, mhim(u.usteed)); */
	    } else
		pline("SUBJECT %s VERB_REAGIEREN nicht.", He); /* EN pline("%s does not respond.", He); */
	    return;
	}

	/* Make the steed less tame and check if it resists */
	if (u.usteed->mtame) u.usteed->mtame--;
	if (!u.usteed->mtame && u.usteed->mleashed) m_unleash(u.usteed, TRUE);
	if (!u.usteed->mtame || (u.ulevel+u.usteed->mtame < rnd(MAXULEV/2+5))) {
	    newsym(u.usteed->mx, u.usteed->my);
	    dismount_steed(DISMOUNT_THROWN);
	    return;
	}

	pline("SUBJECT %s VERB_GALOPPIEREN!", Monnam(u.usteed)); /* EN pline("%s gallops!", Monnam(u.usteed)); */
	u.ugallop += rn1(20, 30);
	return;
}

/*
 * Try to find a dismount point adjacent to the steed's location.
 * If all else fails, try enexto().  Use enexto() as a last resort because
 * enexto() chooses its point randomly, possibly even outside the
 * room's walls, which is not what we want.
 * Adapted from mail daemon code.
 */
STATIC_OVL boolean
landing_spot(spot, reason, forceit)
coord *spot;	/* landing position (we fill it in) */
int reason;
int forceit;
{
    int i = 0, x, y, distance, min_distance = -1;
    boolean found = FALSE;
    struct trap *t;

    /* avoid known traps (i == 0) and boulders, but allow them as a backup */
    if (reason != DISMOUNT_BYCHOICE || Stunned || Confusion || Fumbling) i = 1;
    for (; !found && i < 2; ++i) {
	for (x = u.ux-1; x <= u.ux+1; x++)
	    for (y = u.uy-1; y <= u.uy+1; y++) {
		if (!isok(x, y) || (x == u.ux && y == u.uy)) continue;

		if (ACCESSIBLE(levl[x][y].typ) &&
			    !MON_AT(x,y) && !closed_door(x,y)) {
		    distance = distu(x,y);
		    if (min_distance < 0 || distance < min_distance ||
			    (distance == min_distance && rn2(2))) {
			if (i > 0 || (((t = t_at(x, y)) == 0 || !t->tseen) &&
				      (!sobj_at(BOULDER, x, y) ||
				       throws_rocks(youmonst.data)))) {
			    spot->x = x;
			    spot->y = y;
			    min_distance = distance;
			    found = TRUE;
			}
		    }
		}
	    }
    }

    /* If we didn't find a good spot and forceit is on, try enexto(). */
    if (forceit && min_distance < 0 &&
		!enexto(spot, u.ux, u.uy, youmonst.data))
	return FALSE;

    return found;
}

/* Stop riding the current steed */
void
dismount_steed(reason)
	int reason;		/* Player was thrown off etc. */
{
	struct monst *mtmp;
	struct obj *otmp;
	coord cc;
	const char *verb = "VERB_FALLEN"; /* EN const char *verb = "fall"; */
#ifdef GERMAN
	const char *partizip = "";
#endif
	boolean repair_leg_damage = TRUE;
	unsigned save_utrap = u.utrap;
	boolean have_spot = landing_spot(&cc,reason,0);
	
	mtmp = u.usteed;		/* make a copy of steed pointer */
	/* Sanity check */
	if (!mtmp)		/* Just return silently */
	    return;

	/* Check the reason for dismounting */
	otmp = which_armor(mtmp, W_SADDLE);
	switch (reason) {
	    case DISMOUNT_THROWN:
		verb = "VERB_WERDEN"; /* EN verb = "are thrown"; */
#ifdef GERMAN
		partizip = " abgeworfen"; 
#endif
	    case DISMOUNT_FELL:
		You("%s OBJECT KASUS_DATIV _von_ %s%s!", verb, mon_nam(mtmp), partizip); /* EN You("%s off of %s!", verb, mon_nam(mtmp)); */
		if (!have_spot) have_spot = landing_spot(&cc,reason,1);
		losehp(rn1(10,10), "NOUN_REITUNFALL", KILLED_BY_AN); /* EN losehp(rn1(10,10), "riding accident", KILLED_BY_AN); */
		set_wounded_legs(BOTH_SIDES, (int)HWounded_legs + rn1(5,5));
		repair_leg_damage = FALSE;
		break;
	    case DISMOUNT_POLY:
		You("VERB_KOENNEN nicht mehr OBJECT KASUS_DATIV auf %s reiten.", mon_nam(u.usteed)); /* EN You("can no longer ride %s.", mon_nam(u.usteed)); */
		if (!have_spot) have_spot = landing_spot(&cc,reason,1);
		break;
	    case DISMOUNT_ENGULFED:
		/* caller displays message */
		break;
	    case DISMOUNT_BONES:
		/* hero has just died... */
		break;
	    case DISMOUNT_GENERIC:
		/* no messages, just make it so */
		break;
	    case DISMOUNT_BYCHOICE:
	    default:
		if (otmp && otmp->cursed) {
		    pline("Das geht nicht.  Der Sattel %s verflucht%s.", /* EN You("can't.  The saddle %s cursed.", */
			otmp->bknown ? "ist" : "scheint", otmp->bknown ? "" : " zu sein"); /* EN otmp->bknown ? "is" : "seems to be"); */
		    otmp->bknown = TRUE;
		    return;
		}
		if (!have_spot) {
		    pline("Das geht nicht. Dort gibt es nichts worauf SUBJECT_IM_SATZ PRONOMEN_PERSONAL stehen MODIFIER_KONJUNKTIV_II VERB_KOENNEN."); /* EN You("can't. There isn't anywhere for you to stand."); */
		    return;
		}
		if (!mtmp->mnamelth) {
			pline("SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_GEHEN durch die Höhle auf nem/ner %s ohne Namen.", /* EN pline("You've been through the dungeon on %s with no name.", */ // TODO DE  du gingst durch die Wueste aufm Pferd ohne Namen
				an(mtmp->data->mname));
			if (Hallucination)
				pline("Es war schön, nicht mehr im Regen zu sein."); /* EN pline("It felt good to get out of the rain."); */
		} else
			You("VERB_STEIGEN OBJECT KASUS_DATIV _von_ %s.", mon_nam(mtmp)); /* EN You("dismount %s.", mon_nam(mtmp)); */
	}
	/* While riding these refer to the steed's legs
	 * so after dismounting they refer to the player's
	 * legs once again.
	 */
	if (repair_leg_damage) HWounded_legs = EWounded_legs = 0;

	/* Release the steed and saddle */
	u.usteed = 0;
	u.ugallop = 0L;

	/* Set player and steed's position.  Try moving the player first
	   unless we're in the midst of creating a bones file. */
	if (reason == DISMOUNT_BONES) {
	    /* move the steed to an adjacent square */
	    if (enexto(&cc, u.ux, u.uy, mtmp->data))
		rloc_to(mtmp, cc.x, cc.y);
	    else	/* evidently no room nearby; move steed elsewhere */
		(void) rloc(mtmp, FALSE);
	    return;
	}
	if (!DEADMONSTER(mtmp)) {
	    place_monster(mtmp, u.ux, u.uy);
	    if (!u.uswallow && !u.ustuck && have_spot) {
		struct permonst *mdat = mtmp->data;

		/* The steed may drop into water/lava */
		if (!is_flyer(mdat) && !is_floater(mdat) && !is_clinger(mdat)) {
		    if (is_pool(u.ux, u.uy)) {
			if (!Underwater)
			    pline("SUBJECT %s VERB_FALLEN %s!", Monnam(mtmp), /* EN pline("%s falls into the %s!", Monnam(mtmp), */
							in_den_Boden(u.ux, u.uy)); /* EN surface(u.ux, u.uy)); */
			if (!is_swimmer(mdat) && !amphibious(mdat)) {
			    killed(mtmp);
			    adjalign(-1);
			}
		    } else if (is_lava(u.ux, u.uy)) {
			pline("SUBJECT %s VERB_VERSINKEN in der Lava!", Monnam(mtmp)); /* EN pline("%s is pulled into the lava!", Monnam(mtmp)); */
			if (!likes_lava(mdat)) {
			    killed(mtmp);
			    adjalign(-1);
			}
		    }
		}
	    /* Steed dismounting consists of two steps: being moved to another
	     * square, and descending to the floor.  We have functions to do
	     * each of these activities, but they're normally called
	     * individually and include an attempt to look at or pick up the
	     * objects on the floor:
	     * teleds() --> spoteffects() --> pickup()
	     * float_down() --> pickup()
	     * We use this kludge to make sure there is only one such attempt.
	     *
	     * Clearly this is not the best way to do it.  A full fix would
	     * involve having these functions not call pickup() at all, instead
	     * calling them first and calling pickup() afterwards.  But it
	     * would take a lot of work to keep this change from having any
	     * unforseen side effects (for instance, you would no longer be
	     * able to walk onto a square with a hole, and autopickup before
	     * falling into the hole).
	     */
		/* [ALI] No need to move the player if the steed died. */
		if (!DEADMONSTER(mtmp)) {
		    /* Keep steed here, move the player to cc;
		     * teleds() clears u.utrap
		     */
		    in_steed_dismounting = TRUE;
		    teleds(cc.x, cc.y, TRUE);
		    in_steed_dismounting = FALSE;

		    /* Put your steed in your trap */
		    if (save_utrap)
			(void) mintrap(mtmp);
		}
	    /* Couldn't... try placing the steed */
	    } else if (enexto(&cc, u.ux, u.uy, mtmp->data)) {
		/* Keep player here, move the steed to cc */
		rloc_to(mtmp, cc.x, cc.y);
		/* Player stays put */
	    /* Otherwise, kill the steed */
	    } else {
		killed(mtmp);
		adjalign(-1);
	    }
	}

	/* Return the player to the floor */
	if (reason != DISMOUNT_ENGULFED) {
	    in_steed_dismounting = TRUE;
	    (void) float_down(0L, W_SADDLE);
	    in_steed_dismounting = FALSE;
	    flags.botl = 1;
	    (void)encumber_msg();
	    vision_full_recalc = 1;
	} else
	    flags.botl = 1;
	/* polearms behave differently when not mounted */
	if (uwep && is_pole(uwep)) unweapon = TRUE;
	return;
}

void
place_monster(mon, x, y)
struct monst *mon;
int x, y;
{
    if (mon == u.usteed ||
	    /* special case is for convoluted vault guard handling */
	    (DEADMONSTER(mon) && !(mon->isgd && x == 0 && y == 0))) {
	impossible("placing %s onto map?",
		   (mon == u.usteed) ? "steed" : "defunct monster");
	return;
    }
    mon->mx = x, mon->my = y;
    level.monsters[x][y] = mon;
}

#endif /* STEED */

/*steed.c*/
