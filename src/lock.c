/*	SCCS Id: @(#)lock.c	3.4	2000/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_PTR int NDECL(picklock);
STATIC_PTR int NDECL(forcelock);

/* at most one of `door' and `box' should be non-null at any given time */
STATIC_VAR NEARDATA struct xlock_s {
	struct rm  *door;
	struct obj *box;
	int picktyp, chance, usedtime;
} xlock;

#ifdef OVLB

STATIC_DCL const char *NDECL(lock_action);
STATIC_DCL boolean FDECL(obstructed,(int,int));
STATIC_DCL void FDECL(chest_shatter_msg, (struct obj *));

boolean
picking_lock(x, y)
	int *x, *y;
{
	if (occupation == picklock) {
	    *x = u.ux + u.dx;
	    *y = u.uy + u.dy;
	    return TRUE;
	} else {
	    *x = *y = 0;
	    return FALSE;
	}
}

boolean
picking_at(x, y)
int x, y;
{
	return (boolean)(occupation == picklock && xlock.door == &levl[x][y]);
}

/* produce an occupation string appropriate for the current activity */
STATIC_OVL const char *
lock_action()
{
	static const char *unlock_actions[] = {
		/* [0] */	"die Türe aufzuschließen", /* EN "unlocking the door" */
		/* [1] */	"die Truhe aufzuschließen",/* EN "unlocking the chest" */
		/* [2] */	"die Kiste aufzuschließen", /* EN "unlocking the box" */
		/* [3] */	"das Schloss zu knacken" /* "picking the lock" */
	};
	static const char *lock_actions[] = {
		/* [0] */	"die Türe zu verschließen", /* EN "locking the door" */
		/* [1] */	"die Truhe zu verschließen",/* EN "locking the chest" */
		/* [2] */	"die Kiste zu verschließen", /* EN "locking the box" */
		/* [3] */	"unmoegliche Aktion"
	};

	/* if the target is currently unlocked, we're trying to lock it now */
	if (xlock.door && !(xlock.door->doormask & D_LOCKED))
		return lock_actions[0];	/* "locking the door" */
	else if (xlock.box && !xlock.box->olocked)
		return xlock.box->otyp == CHEST ? lock_actions[1] : lock_actions[2];
	/* otherwise we're trying to unlock it */
	else if (xlock.picktyp == LOCK_PICK)
		return unlock_actions[3];	/* "picking the lock" */
#ifdef TOURIST
	else if (xlock.picktyp == CREDIT_CARD)
		return unlock_actions[3];	/* same as lock_pick */
#endif
	else if (xlock.door)
		return unlock_actions[0];	/* "unlocking the door" */
	else
		return xlock.box->otyp == CHEST ? unlock_actions[1] : unlock_actions[2];
}

STATIC_PTR
int
picklock()	/* try to open/close a lock */
{

	if (xlock.box) {
	    if((xlock.box->ox != u.ux) || (xlock.box->oy != u.uy)) {
		return((xlock.usedtime = 0));		/* you or it moved */
	    }
	} else {		/* door */
	    if(xlock.door != &(levl[u.ux+u.dx][u.uy+u.dy])) {
		return((xlock.usedtime = 0));		/* you moved */
	    }
	    switch (xlock.door->doormask) {
		case D_NODOOR:
		    pline("Dieser Eingang hat keine Türe."); /* EN pline("This doorway has no door."); */
		    return((xlock.usedtime = 0));
		case D_ISOPEN:
		    You("VERB_KOENNEN keine offenen Türen abschließen."); /* EN You("cannot lock an open door."); */
		    return((xlock.usedtime = 0));
		case D_BROKEN:
		    pline("Diese Türe ist kaputt."); /* EN pline("This door is broken."); */
		    return((xlock.usedtime = 0));
	    }
	}

	if (xlock.usedtime++ >= 50 || nohands(youmonst.data)) {
	    You("VERB_GEBEN den Versuch auf, %s.", lock_action()); /* EN You("give up your attempt at %s.", lock_action()); */
	    exercise(A_DEX, TRUE);	/* even if you don't succeed */
	    return((xlock.usedtime = 0));
	}

	if(rn2(100) >= xlock.chance) return(1);		/* still busy */

	Dir("VERB_GELINGEN es, %s.", lock_action()); /* EN You("succeed in %s.", lock_action()); */
	if (xlock.door) {
	    if(xlock.door->doormask & D_TRAPPED) {
		    b_trapped("NOUN_OBJ_DOOR", FINGER); /* EN b_trapped("door", FINGER); */
		    xlock.door->doormask = D_NODOOR;
		    unblock_point(u.ux+u.dx, u.uy+u.dy);
		    if (*in_rooms(u.ux+u.dx, u.uy+u.dy, SHOPBASE))
			add_damage(u.ux+u.dx, u.uy+u.dy, 0L);
		    newsym(u.ux+u.dx, u.uy+u.dy);
	    } else if (xlock.door->doormask & D_LOCKED)
		xlock.door->doormask = D_CLOSED;
	    else xlock.door->doormask = D_LOCKED;
	} else {
	    xlock.box->olocked = !xlock.box->olocked;
	    if(xlock.box->otrapped)	
		(void) chest_trap(xlock.box, FINGER, FALSE);
	}
	exercise(A_DEX, TRUE);
	return((xlock.usedtime = 0));
}

STATIC_PTR
int
forcelock()	/* try to force a locked chest */
{

	register struct obj *otmp;

	if((xlock.box->ox != u.ux) || (xlock.box->oy != u.uy))
		return((xlock.usedtime = 0));		/* you or it moved */

	if (xlock.usedtime++ >= 50 || !uwep || nohands(youmonst.data)) {
	    You("VERB_GEBEN den Versuch auf, das Schloss aufzubrechen."); /* EN You("give up your attempt to force the lock."); */
	    if(xlock.usedtime >= 50)		/* you made the effort */
	      exercise((xlock.picktyp) ? A_DEX : A_STR, TRUE);
	    return((xlock.usedtime = 0));
	}

	if(xlock.picktyp) {	/* blade */

	    if(rn2(1000-(int)uwep->spe) > (992-greatest_erosion(uwep)*10) &&
	       !uwep->cursed && !obj_resists(uwep, 0, 99)) {
		/* for a +0 weapon, probability that it survives an unsuccessful
		 * attempt to force the lock is (.992)^50 = .67
		 */
		pline("SUBJECT %s %s VERB_ZERBRECHEN!", /* EN pline("%s %s broke!", */
		      (uwep->quan > 1L) ? "One of your" : "PRONOMEN_POSSESSIV", xname(uwep)); /* EN (uwep->quan > 1L) ? "One of your" : "Your", xname(uwep)); */ // TODO DE
		/*pline("%sour %s broke!",
			(uwep->quan > 1L) ? "One of y" : "Y", xname(uwep));*/
		useup(uwep);
		You("VERB_GEBEN den Versuch auf, das Schloss aufzubrechen."); /* EN You("give up your attempt to force the lock."); */
		exercise(A_DEX, TRUE);
		return((xlock.usedtime = 0));
	    }
	} else			/* blunt */
	    wake_nearby();	/* due to hammering on the container */

	if(rn2(100) >= xlock.chance) return(1);		/* still busy */

	Dir("VERB_GELINGEN es, das Schloss aufzubrechen."); /* EN You("succeed in forcing the lock."); */
	xlock.box->olocked = 0;
	xlock.box->obroken = 1;
	if(!xlock.picktyp && !rn2(3)) {
	    struct monst *shkp;
	    boolean costly;
	    long loss = 0L;

	    costly = (*u.ushops && costly_spot(u.ux, u.uy));
	    shkp = costly ? shop_keeper(*u.ushops) : 0;

	    You("VERB_HABEN sogar OBJECT %s vollkommen zerstört.", /* EN pline("In fact, you've totally destroyed %s.", */
		  the(xname(xlock.box)));

	    /* Put the contents on ground at the hero's feet. */
	    while ((otmp = xlock.box->cobj) != 0) {
		obj_extract_self(otmp);
		if(!rn2(3) || otmp->oclass == POTION_CLASS) {
		    chest_shatter_msg(otmp);
		    if (costly)
		        loss += stolen_value(otmp, u.ux, u.uy,
					     (boolean)shkp->mpeaceful, TRUE);
		    if (otmp->quan == 1L) {
			obfree(otmp, (struct obj *) 0);
			continue;
		    }
		    useup(otmp);
		}
		if (xlock.box->otyp == ICE_BOX && otmp->otyp == CORPSE) {
		    otmp->age = monstermoves - otmp->age; /* actual age */
		    start_corpse_timeout(otmp);
		}
		place_object(otmp, u.ux, u.uy);
		stackobj(otmp);
	    }

	    if (costly)
		loss += stolen_value(xlock.box, u.ux, u.uy,
					     (boolean)shkp->mpeaceful, TRUE);
	    if(loss) You("owe %ld %s for objects destroyed.", loss, currency(loss)); /* EN if(loss) You("owe %ld %s for objects destroyed.", loss, currency(loss)); */ // TODO DE
	    delobj(xlock.box);
	}
	exercise((xlock.picktyp) ? A_DEX : A_STR, TRUE);
	return((xlock.usedtime = 0));
}

#endif /* OVLB */
#ifdef OVL0

void
reset_pick()
{
	xlock.usedtime = xlock.chance = xlock.picktyp = 0;
	xlock.door = 0;
	xlock.box = 0;
}

#endif /* OVL0 */
#ifdef OVLB

int
pick_lock(pick) /* pick a lock with a given object */
	register struct	obj	*pick;
{
	int picktyp, c, ch;
	coord cc;
	struct rm	*door;
	struct obj	*otmp;
	char qbuf[QBUFSZ];

	picktyp = pick->otyp;

	/* check whether we're resuming an interrupted previous attempt */
	if (xlock.usedtime && picktyp == xlock.picktyp) {
	    static char no_longer[] = "Bedauerlicherweise VERB_KOENNEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT ARTIKEL_BESTIMMTER %s nicht mehr %s."; /* EN static char no_longer[] = "Unfortunately, you can no longer %s %s."; */

	    if (nohands(youmonst.data)) {
		const char *what = (picktyp == LOCK_PICK) ? "NOUN_LOCK_PICK" : "NOUN_KEY"; /* EN const char *what = (picktyp == LOCK_PICK) ? "pick" : "key"; */
#ifdef TOURIST
		if (picktyp == CREDIT_CARD) what = "NOUN_CREDIT_CARD"; /* EN if (picktyp == CREDIT_CARD) what = "card"; */
#endif
		pline(no_longer, what, "halten"); /* EN pline(no_longer, "hold the", what); */
		reset_pick();
		return 0;
	    } else if (xlock.box && !can_reach_floor()) {
		pline(no_longer, "NOUN_LOCK", "erreichen"); /* EN pline(no_longer, "reach the", "lock"); */
		reset_pick();
		return 0;
	    } else {
		const char *action = lock_action();
		You("VERB_MACHEN OBJECT KASUS_DATIV mit PRONOMEN_POSSESSIV NOUN_VERSUCH, %s, weiter.", action); /* EN You("resume your attempt at %s.", action); */
		set_occupation(picklock, action, 0);
		return(1);
	    }
	}

	if(nohands(youmonst.data)) {
		You("VERB_KOENNEN OBJECT PRONOMEN_KEIN %s halten - NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL VERB_HAVE keine Hände!", doname(pick)); /* EN You_cant("hold %s -- you have no hands!", doname(pick)); */
		return(0);
	}

	if((picktyp != LOCK_PICK &&
#ifdef TOURIST
	    picktyp != CREDIT_CARD &&
#endif
	    picktyp != SKELETON_KEY)) {
		impossible("picking lock with object %d?", picktyp);
		return(0);
	}
	ch = 0;		/* lint suppression */

	if(!get_adjacent_loc((char *)0, "Invalid location!", u.ux, u.uy, &cc)) return 0; /* EN if(!get_adjacent_loc((char *)0, "Invalid location!", u.ux, u.uy, &cc)) return 0; */ // TODO DE
	if (cc.x == u.ux && cc.y == u.uy) {	/* pick lock on a container */
	    const char *verb;
	    boolean it;
	    int count;

	    if (u.dz < 0) {
		pline("%s oben gibt es keine Schlösser.", /* EN There("isn't any sort of lock up %s.", */
		      Levitation ? "Hier" : "Dort"); /* EN Levitation ? "here" : "there"); */
		return 0;
	    } else if (is_lava(u.ux, u.uy)) {
		pline("Das würde vermutlich KASUS_AKKUSATIV PRONOMEN_POSSESSIV %s schmelzen.", /* EN pline("Doing that would probably melt your %s.", */
		      xname(pick));
		return 0;
	    } else if (is_pool(u.ux, u.uy) && !Underwater) {
		pline("Wasser hat keine Schlösser."); /* EN pline_The("water has no lock."); */
		return 0;
	    }

	    count = 0;
	    c = 'n';			/* in case there are no boxes here */
	    for(otmp = level.objects[cc.x][cc.y]; otmp; otmp = otmp->nexthere)
		if (Is_box(otmp)) {
		    ++count;
		    if (!can_reach_floor()) {
			You_cant("reach %s from up here.", the(xname(otmp))); /* EN You_cant("reach %s from up here.", the(xname(otmp))); */ // TODO DE
			return 0;
		    }
		    it = 0;
		    if (otmp->obroken) verb = "reparieren"; /* EN if (otmp->obroken) verb = "fix"; */
		    else if (!otmp->olocked) verb = "verschließen", it = 1; /* EN else if (!otmp->olocked) verb = "lock", it = 1; */
		    else if (picktyp != LOCK_PICK) verb = "aufschließen", it = 1; /* EN else if (picktyp != LOCK_PICK) verb = "unlock", it = 1; */
		    else verb = "knacken"; /* EN else verb = "pick"; */
		    Sprintf(qbuf, "Hier ist %s, SUBJECT_IM_SATZ MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL %s %s?", /* EN Sprintf(qbuf, "There is %s here, %s %s?", */
		    	    safe_qbuf("", sizeof("Hier ist  , SUBJECT_IM_SATZ MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL ihr Schloss %s?"), /* EN safe_qbuf("", sizeof("There is  here, unlock its lock?"), */
			    	doname(otmp), an(simple_typename(otmp->otyp)), "eine Kiste"), /* EN doname(otmp), an(simple_typename(otmp->otyp)), "a box"), */
			    it ? "sie" : "ihr Schloss", verb); /* EN verb, it ? "it" : "its lock"); */

		    c = ynq(qbuf);
		    if(c == 'q') return(0);
		    if(c == 'n') continue;

		    if (otmp->obroken) {
			You("VERB_KOENNEN das kaputte Schloss nicht OBJECT KASUS_DATIV mit %s reparieren.", doname(pick)); /* EN You_cant("fix its broken lock with %s.", doname(pick)); */
			return 0;
		    }
#ifdef TOURIST
		    else if (picktyp == CREDIT_CARD && !otmp->olocked) {
			/* credit cards are only good for unlocking */
			pline("Das geht nicht KASUS_DATIV mit %s.", doname(pick)); /* EN You_cant("do that with %s.", doname(pick)); */
			return 0;
		    }
#endif
		    switch(picktyp) {
#ifdef TOURIST
			case CREDIT_CARD:
			    ch = ACURR(A_DEX) + 20*Role_if(PM_ROGUE);
			    break;
#endif
			case LOCK_PICK:
			    ch = 4*ACURR(A_DEX) + 25*Role_if(PM_ROGUE);
			    break;
			case SKELETON_KEY:
			    ch = 75 + ACURR(A_DEX);
			    break;
			default:	ch = 0;
		    }
		    if(otmp->cursed) ch /= 2;

		    xlock.picktyp = picktyp;
		    xlock.box = otmp;
		    xlock.door = 0;
		    break;
		}
	    if (c != 'y') {
		if (!count)
		    There("doesn't seem to be any sort of lock here."); /* EN There("doesn't seem to be any sort of lock here."); */ // TODO DE
		return(0);		/* decided against all boxes */
	    }
	} else {			/* pick the lock in a door */
	    struct monst *mtmp;

	    if (u.utrap && u.utraptype == TT_PIT) {
		You_cant("reach over the edge of the pit."); /* EN You_cant("reach over the edge of the pit."); */ // TODO DE
		return(0);
	    }

	    door = &levl[cc.x][cc.y];
	    if ((mtmp = m_at(cc.x, cc.y)) && canseemon(mtmp)
			&& mtmp->m_ap_type != M_AP_FURNITURE
			&& mtmp->m_ap_type != M_AP_OBJECT) {
#ifdef TOURIST
		if (picktyp == CREDIT_CARD &&
		    (mtmp->isshk || mtmp->data == &mons[PM_ORACLE]))
		    verbalize("Keine Schecks, kein Kredit, keine Probleme."); /* EN verbalize("No checks, no credit, no problem."); */
		else
#endif
		    pline("Ich glaube nicht, dass %s das goutieren würde.", mon_nam(mtmp)); /* EN pline("I don't think %s would appreciate that.", mon_nam(mtmp)); */
		return(0);
	    }
	    if(!IS_DOOR(door->typ)) {
		if (is_drawbridge_wall(cc.x,cc.y) >= 0)
		    You("%s kein Schloss an der Zugbrücke.", /* EN You("%s no lock on the drawbridge.", */
				Blind ? "VERB_SPUEREN" : "VERB_SEE"); /* EN Blind ? "feel" : "see"); */
		else
		    You("%s hier OBJECT PRONOMEN_KEIN NOUN_OBJ_DOOR.", /* EN You("%s no door there.", */
				Blind ? "VERB_SPUEREN" : "VERB_SEE"); /* EN Blind ? "feel" : "see"); */
		return(0);
	    }
	    switch (door->doormask) {
		case D_NODOOR:
		    pline("Dieser Eingang hat keine Türe."); /* EN pline("This doorway has no door."); */
		    return(0);
		case D_ISOPEN:
		    You("VERB_KOENNEN keine offenen Türen verschließen."); /* EN You("cannot lock an open door."); */
		    return(0);
		case D_BROKEN:
		    pline("Diese Türe ist kaputt."); /* EN pline("This door is broken."); */
		    return(0);
		default:
#ifdef TOURIST
		    /* credit cards are only good for unlocking */
		    if(picktyp == CREDIT_CARD && !(door->doormask & D_LOCKED)) {
			You("VERB_KOENNEN keine Türe mit einer Kreditkarte abschließen."); /* EN You_cant("lock a door with a credit card."); */
			return(0);
		    }
#endif

		    Sprintf(qbuf,"%sschließen?", /* EN Sprintf(qbuf,"%sock it?", */
			(door->doormask & D_LOCKED) ? "Auf" : "Ver" ); /* EN (door->doormask & D_LOCKED) ? "Unl" : "L" ); */
		    c = yn(qbuf);
		    if(c == 'n') return(0);

		    switch(picktyp) {
#ifdef TOURIST
			case CREDIT_CARD:
			    ch = 2*ACURR(A_DEX) + 20*Role_if(PM_ROGUE);
			    break;
#endif
			case LOCK_PICK:
			    ch = 3*ACURR(A_DEX) + 30*Role_if(PM_ROGUE);
			    break;
			case SKELETON_KEY:
			    ch = 70 + ACURR(A_DEX);
			    break;
			default:    ch = 0;
		    }
		    xlock.door = door;
		    xlock.box = 0;
	    }
	}
	flags.move = 0;
	xlock.chance = ch;
	xlock.picktyp = picktyp;
	xlock.usedtime = 0;
	set_occupation(picklock, lock_action(), 0);
	return(1);
}

int
doforce()		/* try to force a chest with your weapon */
{
	register struct obj *otmp;
	register int c, picktyp;
	char qbuf[QBUFSZ];

	if(!uwep ||	/* proper type test */
	   (uwep->oclass != WEAPON_CLASS && !is_weptool(uwep) &&
	    uwep->oclass != ROCK_CLASS) ||
	   (objects[uwep->otyp].oc_skill < P_DAGGER) ||
	   (objects[uwep->otyp].oc_skill > P_LANCE) ||
	   uwep->otyp == FLAIL || uwep->otyp == AKLYS
#ifdef KOPS
	   || uwep->otyp == RUBBER_HOSE
#endif
	  ) {
	    pline("Ohne eine %sWaffe SUBJECT_IM_SATZ VERB_KOENNEN PRONOMEN_PERSONAL nichts aufbrechen.", /* EN You_cant("force anything without a %sweapon.", */
		  (uwep) ? "geeignete " : ""); /* EN (uwep) ? "proper " : ""); */
	    return(0);
	}

	picktyp = is_blade(uwep);
	if(xlock.usedtime && xlock.box && picktyp == xlock.picktyp) {
	    You("VERB_MACHEN OBJECT KASUS_DATIV mit PRONOMEN_POSSESSIV NOUN_VERSUCH, das Schloss aufzubrechen, weiter."); /* EN You("resume your attempt to force the lock."); */
	    set_occupation(forcelock, "das Schloss aufzubrechen", 0); /* EN set_occupation(forcelock, "forcing the lock", 0); */
	    return(1);
	}

	/* A lock is made only for the honest man, the thief will break it. */
	xlock.box = (struct obj *)0;
	for(otmp = level.objects[u.ux][u.uy]; otmp; otmp = otmp->nexthere)
	    if(Is_box(otmp)) {
		if (otmp->obroken || !otmp->olocked) {
		    There("is %s here, but its lock is already %s.", /* EN There("is %s here, but its lock is already %s.", */ // TODO DE
			  doname(otmp), otmp->obroken ? "broken" : "unlocked"); /* EN doname(otmp), otmp->obroken ? "broken" : "unlocked"); */ // TODO DE
		    continue;
		}
		Sprintf(qbuf,"There is %s here, force its lock?", /* EN Sprintf(qbuf,"There is %s here, force its lock?", */ // TODO DE
			safe_qbuf("", sizeof("There is  here, force its lock?"), /* EN safe_qbuf("", sizeof("There is  here, force its lock?"), */ // TODO DE
				doname(otmp), an(simple_typename(otmp->otyp)),
				"eine Kiste")); /* EN "a box")); */

		c = ynq(qbuf);
		if(c == 'q') return(0);
		if(c == 'n') continue;

		if(picktyp)
		    You("VERB_ZWAENGEN OBJECT PRONOMEN_POSSESSIV %s in einen Spalt und VERB_BEGINNEN zu stemmen.", xname(uwep)); /* EN You("force your %s into a crack and pry.", xname(uwep)); */
		else
		    You("VERB_BEGINNEN OBJECT KASUS_DATIV mit PRONOMEN_POSSESSIV %s auf die Monster einzuprügeln.", xname(uwep)); /* EN You("start bashing it with your %s.", xname(uwep)); */
		xlock.box = otmp;
		xlock.chance = objects[uwep->otyp].oc_wldam * 2;
		xlock.picktyp = picktyp;
		xlock.usedtime = 0;
		break;
	    }

	if(xlock.box)	set_occupation(forcelock, "das Schloss aufzubrechen", 0); /* EN if(xlock.box)	set_occupation(forcelock, "forcing the lock", 0); */
	else		You("decide not to force the issue."); /* EN else		You("decide not to force the issue."); */
	return(1);
}

int
doopen()		/* try to open a door */
{
	coord cc;
	register struct rm *door;
	struct monst *mtmp;

	if (nohands(youmonst.data)) {
	    You("VERB_KOENNEN nichts öffnen - PRONOMEN_PERSONAL VERB_HAVE keine Hände!"); /* EN You_cant("open anything -- you have no hands!"); */
	    return 0;
	}

	if (u.utrap && u.utraptype == TT_PIT) {
	    You_cant("reach over the edge of the pit."); /* EN You_cant("reach over the edge of the pit."); */
	    return 0;
	}

	if(!get_adjacent_loc((char *)0, (char *)0, u.ux, u.uy, &cc)) return(0);

	if((cc.x == u.ux) && (cc.y == u.uy)) return(0);

	if ((mtmp = m_at(cc.x,cc.y))			&&
		mtmp->m_ap_type == M_AP_FURNITURE	&&
		(mtmp->mappearance == S_hcdoor ||
			mtmp->mappearance == S_vcdoor)	&&
		!Protection_from_shape_changers)	 {

	    stumble_onto_mimic(mtmp);
	    return(1);
	}

	door = &levl[cc.x][cc.y];

	if(!IS_DOOR(door->typ)) {
		if (is_db_wall(cc.x,cc.y)) {
		    pline("Es gibt keinen offensichtlichen Weg die Zugbrücke zu öffnen."); /* EN There("is no obvious way to open the drawbridge."); */
		    return(0);
		}
		You("%s hier OBJECT PRONOMEN_KEIN NOUN_OBJ_DOOR.", /* EN You("%s no door there.", */
				Blind ? "VERB_SPUEREN" : "VERB_SEHEN"); /* EN Blind ? "feel" : "see"); */
		return(0);
	}

	if (!(door->doormask & D_CLOSED)) {
	    const char *mesg;

	    switch (door->doormask) {
	    case D_BROKEN: mesg = " Türe ist kaputt"; break; /* EN case D_BROKEN: mesg = " is broken"; break; */
	    case D_NODOOR: mesg = "r Eingang hat keine Türe"; break; /* EN case D_NODOOR: mesg = "way has no door"; break; */
	    case D_ISOPEN: mesg = " Türe ist bereits offen"; break; /* EN case D_ISOPEN: mesg = " is already open"; break; */
	    default:	   mesg = " Türe ist abgeschlossen"; break; /* EN default:       mesg = " is locked"; break; */
	    }
	    pline("Diese%s.", mesg); /* EN pline("This door%s.", mesg); */
	    if (Blind) feel_location(cc.x,cc.y);
	    return(0);
	}

	if(verysmall(youmonst.data)) {
	    pline("SUBJECT PRONOMEN_PERSONAL VERB_SEIN zu klein um die schwere Türe zu öffnen."); /* EN pline("You're too small to pull the door open."); */
	    return(0);
	}

	/* door is known to be CLOSED */
	if (rnl(20) < (ACURRSTR+ACURR(A_DEX)+ACURR(A_CON))/3) {
	    pline_The("NOUN_OBJ_DOOR VERB_OPEN SATZKLAMMER."); /* EN pline_The("door opens."); */
	    if(door->doormask & D_TRAPPED) {
		b_trapped("NOUN_OBJ_DOOR", FINGER); /* EN b_trapped("door", FINGER); */
		door->doormask = D_NODOOR;
		if (*in_rooms(cc.x, cc.y, SHOPBASE)) add_damage(cc.x, cc.y, 0L);
	    } else
		door->doormask = D_ISOPEN;
	    if (Blind)
		feel_location(cc.x,cc.y);	/* the hero knows she opened it  */
	    else
		newsym(cc.x,cc.y);
	    unblock_point(cc.x,cc.y);		/* vision: new see through there */
	} else {
	    exercise(A_STR, TRUE);
	    pline_The("NOUN_OBJ_DOOR klemmt!"); /* EN pline_The("door resists!"); */
	}

	return(1);
}

STATIC_OVL
boolean
obstructed(x,y)
register int x, y;
{
	register struct monst *mtmp = m_at(x, y);

	if(mtmp && mtmp->m_ap_type != M_AP_FURNITURE) {
		if (mtmp->m_ap_type == M_AP_OBJECT) goto objhere;
		pline("SUBJECT %s VERB_STEHEN OBJECT KASUS_DATIV im Weg!", !canspotmon(mtmp) ?  /* EN pline("%s stands in the way!", !canspotmon(mtmp) ? */
			"ARTIKEL_UNBESTIMMTER NOUN_CREATURE" : Monnam(mtmp)); /* EN "Some creature" : Monnam(mtmp)); */
		if (!canspotmon(mtmp))
		    map_invisible(mtmp->mx, mtmp->my);
		return(TRUE);
	}
	if (OBJ_AT(x, y)) {
objhere:	pline("SUBJECT %s VERB_STEHEN OBJECT KASUS_DATIV im Weg.", Something); /* EN objhere:	pline("%s's in the way.", Something); */
		return(TRUE);
	}
	return(FALSE);
}

int
doclose()		/* try to close a door */
{
	register int x, y;
	register struct rm *door;
	struct monst *mtmp;

	if (nohands(youmonst.data)) {
	    You("VERB_KOENNEN nichts schließen - PRONOMEN_PERSONAL VERB_HAVE keine Hände!"); /* EN You_cant("close anything -- you have no hands!"); */
	    return 0;
	}

	if (u.utrap && u.utraptype == TT_PIT) {
	    You_cant("reach over the edge of the pit."); /* EN You_cant("reach over the edge of the pit."); */ // TODO DE
	    return 0;
	}

	if(!getdir((char *)0)) return(0);

	x = u.ux + u.dx;
	y = u.uy + u.dy;
	if((x == u.ux) && (y == u.uy)) {
		You("VERB_STEHEN im Weg!"); /* EN You("are in the way!"); */
		return(1);
	}

	if ((mtmp = m_at(x,y))				&&
		mtmp->m_ap_type == M_AP_FURNITURE	&&
		(mtmp->mappearance == S_hcdoor ||
			mtmp->mappearance == S_vcdoor)	&&
		!Protection_from_shape_changers)	 {

	    stumble_onto_mimic(mtmp);
	    return(1);
	}

	door = &levl[x][y];

	if(!IS_DOOR(door->typ)) {
		if (door->typ == DRAWBRIDGE_DOWN)
		    pline("Es gibt keinen offensichtlichen Weg die Zugbrücke zu schließen."); /* EN There("is no obvious way to close the drawbridge."); */
		else
		    You("%s hier OBJECT PRONOMEN_KEIN NOUN_OBJ_DOOR.", /* EN You("%s no door there.", */
				Blind ? "VERB_SPUEREN" : "VERB_SEE"); /* EN Blind ? "feel" : "see"); */
		return(0);
	}

	if(door->doormask == D_NODOOR) {
	    pline("Dieser Eingang hat keine Türe."); /* EN pline("This doorway has no door."); */
	    return(0);
	}

	if(obstructed(x, y)) return(0);

	if(door->doormask == D_BROKEN) {
	    pline("Diese Türe ist kaputt."); /* EN pline("This door is broken."); */
	    return(0);
	}

	if(door->doormask & (D_CLOSED | D_LOCKED)) {
	    pline("Diese Türe ist schon geschlossen."); /* EN pline("This door is already closed."); */
	    return(0);
	}

	if(door->doormask == D_ISOPEN) {
	    if(verysmall(youmonst.data)
#ifdef STEED
		&& !u.usteed
#endif
		) {
		 pline("SUBJECT PRONOMEN_PERSONAL VERB_SEIN zu klein um die Türe zuzuziehen."); /* EN pline("You're too small to push the door closed."); */
		 return(0);
	    }
	    if (
#ifdef STEED
		 u.usteed ||
#endif
		rn2(25) < (ACURRSTR+ACURR(A_DEX)+ACURR(A_CON))/3) {
		pline_The("NOUN_OBJ_DOOR VERB_SCHLIESSEN sich."); /* EN pline_The("door closes."); */
		door->doormask = D_CLOSED;
		if (Blind)
		    feel_location(x,y);	/* the hero knows she closed it */
		else
		    newsym(x,y);
		block_point(x,y);	/* vision:  no longer see there */
	    }
	    else {
	        exercise(A_STR, TRUE);
	        pline_The("NOUN_OBJ_DOOR rührt sich nicht!"); /* EN pline_The("door resists!"); */
	    }
	}

	return(1);
}

boolean			/* box obj was hit with spell effect otmp */
boxlock(obj, otmp)	/* returns true if something happened */
register struct obj *obj, *otmp;	/* obj *is* a box */
{
	register boolean res = 0;

	switch(otmp->otyp) {
	case WAN_LOCKING:
	case SPE_WIZARD_LOCK:
	    if (!obj->olocked) {	/* lock it; fix if broken */
		pline("Klack!"); /* EN pline("Klunk!"); */
		obj->olocked = 1;
		obj->obroken = 0;
		res = 1;
	    } /* else already closed and locked */
	    break;
	case WAN_OPENING:
	case SPE_KNOCK:
	    if (obj->olocked) {		/* unlock; couldn't be broken */
		pline("Klick!"); /* EN pline("Klick!"); */
		obj->olocked = 0;
		res = 1;
	    } else			/* silently fix if broken */
		obj->obroken = 0;
	    break;
	case WAN_POLYMORPH:
	case SPE_POLYMORPH:
	    /* maybe start unlocking chest, get interrupted, then zap it;
	       we must avoid any attempt to resume unlocking it */
	    if (xlock.box == obj)
		reset_pick();
	    break;
	}
	return res;
}

boolean			/* Door/secret door was hit with spell effect otmp */
doorlock(otmp,x,y)	/* returns true if something happened */
struct obj *otmp;
int x, y;
{
	register struct rm *door = &levl[x][y];
	boolean res = TRUE;
	int loudness = 0;
	const char *msg = (const char *)0;
	const char *dustcloud = "Eine Staubwolke"; /* EN const char *dustcloud = "A cloud of dust"; */
	const char *quickly_dissipates = "verflüchtigt sich schnell wieder"; /* EN const char *quickly_dissipates = "quickly dissipates"; */
	
	if (door->typ == SDOOR) {
	    switch (otmp->otyp) {
	    case WAN_OPENING:
	    case SPE_KNOCK:
	    case WAN_STRIKING:
	    case SPE_FORCE_BOLT:
		door->typ = DOOR;
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
		newsym(x,y);
		if (cansee(x,y)) pline("Eine Türe erscheint in der Wand!"); /* EN if (cansee(x,y)) pline("A door appears in the wall!"); */
		if (otmp->otyp == WAN_OPENING || otmp->otyp == SPE_KNOCK)
		    return TRUE;
		break;		/* striking: continue door handling below */
	    case WAN_LOCKING:
	    case SPE_WIZARD_LOCK:
	    default:
		return FALSE;
	    }
	}

	switch(otmp->otyp) {
	case WAN_LOCKING:
	case SPE_WIZARD_LOCK:
#ifdef REINCARNATION
	    if (Is_rogue_level(&u.uz)) {
	    	boolean vis = cansee(x,y);
		/* Can't have real locking in Rogue, so just hide doorway */
		if (vis) pline("%s quillt im älteren, primitiven Durchgang empor.", /* EN if (vis) pline("%s springs up in the older, more primitive doorway.", */
			dustcloud);
		else
			You_hear("ein Rauschen."); /* EN You_hear("a swoosh."); */
		if (obstructed(x,y)) {
			if (vis) pline("Die Wolke %s.",quickly_dissipates); /* EN if (vis) pline_The("cloud %s.",quickly_dissipates); */
			return FALSE;
		}
		block_point(x, y);
		door->typ = SDOOR;
		if (vis) pline("Der Durchgang verschwindet!"); /* EN if (vis) pline_The("doorway vanishes!"); */
		newsym(x,y);
		return TRUE;
	    }
#endif
	    if (obstructed(x,y)) return FALSE;
	    /* Don't allow doors to close over traps.  This is for pits */
	    /* & trap doors, but is it ever OK for anything else? */
	    if (t_at(x,y)) {
		/* maketrap() clears doormask, so it should be NODOOR */
		pline(
		"%s quillt im Durchgang empor, aber %s.", /* EN "%s springs up in the doorway, but %s.", */
		dustcloud, quickly_dissipates);
		return FALSE;
	    }

	    switch (door->doormask & ~D_TRAPPED) {
	    case D_CLOSED:
		msg = "Die Türe verriegelt sich!"; /* EN msg = "The door locks!"; */
		break;
	    case D_ISOPEN:
		msg = "Die Türe schlägt zu und verriegelt sich!"; /* EN msg = "The door swings shut, and locks!"; */
		break;
	    case D_BROKEN:
		msg = "Die kaputte Türe setzt sich wieder zusammen und verriegelt sich!"; /* EN msg = "The broken door reassembles and locks!"; */
		break;
	    case D_NODOOR:
		msg =
		"Eine Staubwolke quillt empor und verdichtet sich zu einer Türe!"; /* EN "A cloud of dust springs up and assembles itself into a door!"; */
		break;
	    default:
		res = FALSE;
		break;
	    }
	    block_point(x, y);
	    door->doormask = D_LOCKED | (door->doormask & D_TRAPPED);
	    newsym(x,y);
	    break;
	case WAN_OPENING:
	case SPE_KNOCK:
	    if (door->doormask & D_LOCKED) {
		msg = "Die Türe entriegelt sich!"; /* EN msg = "The door unlocks!"; */
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
	    } else res = FALSE;
	    break;
	case WAN_STRIKING:
	case SPE_FORCE_BOLT:
	    if (door->doormask & (D_LOCKED | D_CLOSED)) {
		if (door->doormask & D_TRAPPED) {
		    if (MON_AT(x, y))
			(void) mb_trapped(m_at(x,y));
		    else if (flags.verbose) {
			if (cansee(x,y))
			    pline("KABUUUM!!  SUBJECT PRONOMEN_PERSONAL VERB_SEHEN eine Türe explodieren."); /* EN pline("KABOOM!!  You see a door explode."); */
			else if (flags.soundok)
			    You_hear("eine entfernte Explosion."); /* EN You_hear("a distant explosion."); */
		    }
		    door->doormask = D_NODOOR;
		    unblock_point(x,y);
		    newsym(x,y);
		    loudness = 40;
		    break;
		}
		door->doormask = D_BROKEN;
		if (flags.verbose) {
		    if (cansee(x,y))
			pline("Die Türe kracht splitternd auf!"); /* EN pline_The("door crashes open!"); */
		    else if (flags.soundok)
			You_hear("ein krachendes Geräusch."); /* EN You_hear("a crashing sound."); */
		}
		unblock_point(x,y);
		newsym(x,y);
		/* force vision recalc before printing more messages */
		if (vision_full_recalc) vision_recalc(0);
		loudness = 20;
	    } else res = FALSE;
	    break;
	default: impossible("magic (%d) attempted on door.", otmp->otyp);
	    break;
	}
	if (msg && cansee(x,y)) pline(msg);
	if (loudness > 0) {
	    /* door was destroyed */
	    wake_nearto(x, y, loudness);
	    if (*in_rooms(x, y, SHOPBASE)) add_damage(x, y, 0L);
	}

	if (res && picking_at(x, y)) {
	    /* maybe unseen monster zaps door you're unlocking */
	    stop_occupation();
	    reset_pick();
	}
	return res;
}

STATIC_OVL void
chest_shatter_msg(otmp)
struct obj *otmp;
{
	const char *disposition;
	const char *thing;
	long save_Blinded;

	if (otmp->oclass == POTION_CLASS) {
		You("%s OBJECT %s zerbrechen!", Blind ? "VERB_HEAR" : "VERB_SEE", an(bottlename())); /* EN You("%s %s shatter!", Blind ? "hear" : "see", an(bottlename())); */
		if (!breathless(youmonst.data) || haseyes(youmonst.data))
			potionbreathe(otmp);
		return;
	}
	/* We have functions for distant and singular names, but not one */
	/* which does _both_... */
	save_Blinded = Blinded;
	Blinded = 1;
	thing = singular(otmp, xname);
	Blinded = save_Blinded;
	switch (objects[otmp->otyp].oc_material) {
	case PAPER:	disposition = "wird zerrissen"; /* EN case PAPER:	disposition = "is torn to shreds"; */
		break;
	case WAX:	disposition = "wird zerdrückt"; /* EN case WAX:	disposition = "is crushed"; */
		break;
	case VEGGY:	disposition = "wird zermatscht"; /* EN case VEGGY:	disposition = "is pulped"; */
		break;
	case FLESH:	disposition = "wird zerquetscht"; /* EN case FLESH:	disposition = "is mashed"; */
		break;
	case GLASS:	disposition = "zerbricht"; /* EN case GLASS:	disposition = "shatters"; */
		break;
	case WOOD:	disposition = "zersplittert"; /* EN case WOOD:	disposition = "splinters to fragments"; */
		break;
	default:	disposition = "wird zerstört"; /* EN default:	disposition = "is destroyed"; */
		break;
	}
	pline("SUBJECT %s %s!", An(thing), disposition); /* EN pline("%s %s!", An(thing), disposition); */
}

#endif /* OVLB */

/*lock.c*/
