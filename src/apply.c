/*	SCCS Id: @(#)apply.c	3.4	2003/11/18	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h"

#ifdef GERMAN
# include "german.h"
#endif

#ifdef OVLB

static const char tools[] = { TOOL_CLASS, WEAPON_CLASS, WAND_CLASS, 0 };
static const char tools_too[] = { ALL_CLASSES, TOOL_CLASS, POTION_CLASS,
				  WEAPON_CLASS, WAND_CLASS, GEM_CLASS, 0 };

#ifdef TOURIST
STATIC_DCL int FDECL(use_camera, (struct obj *));
#endif
STATIC_DCL int FDECL(use_towel, (struct obj *));
STATIC_DCL boolean FDECL(its_dead, (int,int,int *));
STATIC_DCL int FDECL(use_stethoscope, (struct obj *));
STATIC_DCL void FDECL(use_whistle, (struct obj *));
STATIC_DCL void FDECL(use_magic_whistle, (struct obj *));
STATIC_DCL void FDECL(use_leash, (struct obj *));
STATIC_DCL int FDECL(use_mirror, (struct obj *));
STATIC_DCL void FDECL(use_bell, (struct obj **));
STATIC_DCL void FDECL(use_candelabrum, (struct obj *));
STATIC_DCL void FDECL(use_candle, (struct obj **));
STATIC_DCL void FDECL(use_lamp, (struct obj *));
STATIC_DCL void FDECL(light_cocktail, (struct obj *));
STATIC_DCL void FDECL(use_tinning_kit, (struct obj *));
STATIC_DCL void FDECL(use_figurine, (struct obj **));
STATIC_DCL void FDECL(use_grease, (struct obj *));
STATIC_DCL void FDECL(use_trap, (struct obj *));
STATIC_DCL void FDECL(use_stone, (struct obj *));
STATIC_PTR int NDECL(set_trap);		/* occupation callback */
STATIC_DCL int FDECL(use_whip, (struct obj *));
STATIC_DCL int FDECL(use_pole, (struct obj *));
STATIC_DCL int FDECL(use_cream_pie, (struct obj *));
STATIC_DCL int FDECL(use_grapple, (struct obj *));
STATIC_DCL int FDECL(do_break_wand, (struct obj *));
STATIC_DCL boolean FDECL(figurine_location_checks,
				(struct obj *, coord *, BOOLEAN_P));
STATIC_DCL boolean NDECL(uhave_graystone);
STATIC_DCL void FDECL(add_class, (char *, CHAR_P));

#ifdef	AMIGA
void FDECL( amii_speaker, ( struct obj *, char *, int ) );
#endif

static const char no_elbow_room[] = "don't have enough elbow-room to maneuver."; /* EN static const char no_elbow_room[] = "don't have enough elbow-room to maneuver."; */ // TODO DE

#ifdef TOURIST
STATIC_OVL int
use_camera(obj)
	struct obj *obj;
{
	register struct monst *mtmp;

	if(Underwater) {
		pline("Using your camera underwater would void the warranty."); /* EN pline("Using your camera underwater would void the warranty."); */ // TODO DE
		return(0);
	}
	if(!getdir((char *)0)) return(0);

	if (obj->spe <= 0) {
		pline(nothing_happens);
		return (1);
	}
	consume_obj_charge(obj, TRUE);

	if (obj->cursed && !rn2(2)) {
		(void) zapyourself(obj, TRUE);
	} else if (u.uswallow) {
		You("VERB_FOTOGRAFIEREN OBJECT %s.", genitivattribut_zu_wort(mon_nam(u.ustuck), /* EN You("take a picture of %s %s.", s_suffix(mon_nam(u.ustuck)), */
		    mbodypart(u.ustuck, STOMACH))); /* EN mbodypart(u.ustuck, STOMACH)); */
	} else if (u.dz) {
		You("VERB_FOTOGRAFIEREN OBJECT ARTIKEL_BESTIMMTER %s.", /* EN You("take a picture of the %s.", */
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
	} else if (!u.dx && !u.dy) {
		(void) zapyourself(obj, TRUE);
	} else if ((mtmp = bhit(u.dx, u.dy, COLNO, FLASHED_LIGHT,
				(int FDECL((*),(MONST_P,OBJ_P)))0,
				(int FDECL((*),(OBJ_P,OBJ_P)))0,
				obj)) != 0) {
		obj->ox = u.ux,  obj->oy = u.uy;
		(void) flash_hits_mon(mtmp, obj);
	}
	return 1;
}
#endif

STATIC_OVL int
use_towel(obj)
	struct obj *obj;
{
	if(!freehand()) {
		You("VERB_HAVE PRONOMEN_KEIN ADJEKTIV_FREI %s!", body_part(HAND)); /* EN You("have no free %s!", body_part(HAND)); */
		return 0;
	} else if (obj->owornmask) {
		You("cannot use it while you're wearing it!"); /* EN You("cannot use it while you're wearing it!"); */ // TODO DE
		return 0;
	} else if (obj->cursed) {
		long old;
		switch (rn2(3)) {
		case 2:
		    old = Glib;
		    Glib += rn1(10, 3);
		    Your("%s %s!", makeplural(body_part(HAND)),
			(old ? "VERB_SEIN dreckiger denn je" : "VERB_WERDEN ganz glibberig")); /* EN (old ? "are filthier than ever" : "get slimy")); */
		    return 1;
		case 1:
		    if (!ublindf) {
			old = u.ucreamed;
			u.ucreamed += rn1(10, 3);
			pline("Eklig! SUBJECT PRONOMEN_PERSONAL VERB_HABEN %s Schnodder auf OBJECT KASUS_DATIV PRONOMEN_POSSESSIV %s!",  /* EN pline("Yecch! Your %s %s gunk on it!", body_part(FACE), */
			      (old ? "noch mehr" : "jetzt"), body_part(FACE)); /* EN (old ? "has more" : "now has")); */
			make_blinded(Blinded + (long)u.ucreamed - old, TRUE);
		    } else {
			const char *what = (ublindf->otyp == LENSES) ?
					    "NOUN_LENSESs" : "NOUN_BLINDFOLD"; /* EN "lenses" : "blindfold"; */
			if (ublindf->cursed) {
			    You("push your %s %s.", what, /* EN You("push your %s %s.", what, */ // TODO DE
				rn2(2) ? "cock-eyed" : "crooked"); /* EN rn2(2) ? "cock-eyed" : "crooked"); */ // TODO DE
			} else {
			    struct obj *saved_ublindf = ublindf;
			    You("push your %s off.", what); /* EN You("push your %s off.", what); */ // TODO DE
			    Blindf_off(ublindf);
			    dropx(saved_ublindf);
			}
		    }
		    return 1;
		case 0:
		    break;
		}
	}

	if (Glib) {
		Glib = 0;
		You("VERB_SAEUBERN OBJECT PRONOMEN_POSSESSIV %s.", makeplural(body_part(HAND))); /* EN You("wipe off your %s.", makeplural(body_part(HAND))); */
		return 1;
	} else if(u.ucreamed) {
		Blinded -= u.ucreamed;
		u.ucreamed = 0;

		if (!Blinded) {
			pline("SUBJECT PRONOMEN_PERSONAL VERB_HABEN den Rotz abbekommen."); /* EN pline("You've got the glop off."); */
			Blinded = 1;
			make_blinded(0L,TRUE);
		} else {
			Your("%s VERB_FUEHLEN sich jetzt sauber an.", body_part(FACE)); /* EN Your("%s feels clean now.", body_part(FACE)); */
		}
		return 1;
	}

	Your("%s und SUBJECT_IM_SATZ PRONOMEN_POSSESSIV %s VERB_SEIN bereits sauber.", /* EN Your("%s and %s are already clean.", */
		body_part(FACE), makeplural(body_part(HAND)));

	return 0;
}

/* maybe give a stethoscope message based on floor objects */
STATIC_OVL boolean
its_dead(rx, ry, resp)
int rx, ry, *resp;
{
	struct obj *otmp;
	struct trap *ttmp;

	if (!can_reach_floor()) return FALSE;

	/* additional stethoscope messages from jyoung@apanix.apana.org.au */
	if (Hallucination && sobj_at(CORPSE, rx, ry)) {
	    /* (a corpse doesn't retain the monster's sex,
	       so we're forced to use generic pronoun here) */
	    You_hear("eine Stimme sagen: \"Er ist tot, Jim.\""); /* EN You_hear("a voice say, \"It's dead, Jim.\""); */
	    *resp = 1;
	    return TRUE;
	} else if (Role_if(PM_HEALER) && ((otmp = sobj_at(CORPSE, rx, ry)) != 0 ||
				    (otmp = sobj_at(STATUE, rx, ry)) != 0)) {
	    /* possibly should check uppermost {corpse,statue} in the pile
	       if both types are present, but it's not worth the effort */
	    if (vobj_at(rx, ry)->otyp == STATUE) otmp = vobj_at(rx, ry);
	    if (otmp->otyp == CORPSE) {
		You("VERB_STELLEN fest, dass NEUER_SATZ %s bedauernswerte Wesen tot ist.", /* EN You("determine that %s unfortunate being is dead.", */
		    (rx == u.ux && ry == u.uy) ? "dieses" : "jenes");
	    } else {
		ttmp = t_at(rx, ry);
		pline("SUBJECT %s VERB_SCHEINEN für eine Statue bei %s Gesundheit zu sein.", /* EN pline("%s appears to be in %s health for a statue.", */
		      The(mons[otmp->corpsenm].mname),
		      (ttmp && ttmp->ttyp == STATUE_TRAP) ?
			"außerordentlich guter" : "exzellenter"); /* EN "extraordinary" : "excellent"); */
	    }
	    return TRUE;
	}
	return FALSE;
}

static const char hollow_str[] = "a hollow sound.  This must be a secret %s!"; /* EN static const char hollow_str[] = "a hollow sound.  This must be a secret %s!"; */ // TODO DE

/* Strictly speaking it makes no sense for usage of a stethoscope to
   not take any time; however, unless it did, the stethoscope would be
   almost useless.  As a compromise, one use per turn is free, another
   uses up the turn; this makes curse status have a tangible effect. */
STATIC_OVL int
use_stethoscope(obj)
	register struct obj *obj;
{
	static long last_used_move = -1;
	static short last_used_movement = 0;
	struct monst *mtmp;
	struct rm *lev;
	int rx, ry, res;
	boolean interference = (u.uswallow && is_whirly(u.ustuck->data) &&
				!rn2(Role_if(PM_HEALER) ? 10 : 3));

	if (nohands(youmonst.data)) {	/* should also check for no ears and/or deaf */
		You("VERB_HAVE OBJECT PRONOMEN_KEIN NOUN_HANDs!");	/* not `body_part(HAND)' */ /* EN You("have no hands!");	*/
		return 0;
	} else if (!freehand()) {
		You("VERB_HAVE OBJECT PRONOMEN_KEIN %s frei.", body_part(HAND)); /* EN You("have no free %s.", body_part(HAND)); */
		return 0;
	}
	if (!getdir((char *)0)) return 0;

	res = (moves == last_used_move) &&
	      (youmonst.movement == last_used_movement);
	last_used_move = moves;
	last_used_movement = youmonst.movement;

#ifdef STEED
	if (u.usteed && u.dz > 0) {
		if (interference) {
			pline("SUBJECT %s VERB_BEHINDERN OBJECT PRONOMEN_PERSONAL.", Monnam(u.ustuck)); /* EN pline("%s interferes.", Monnam(u.ustuck)); */
			mstatusline(u.ustuck);
		} else
			mstatusline(u.usteed);
		return res;
	} else
#endif
	if (u.uswallow && (u.dx || u.dy || u.dz)) {
		mstatusline(u.ustuck);
		return res;
	} else if (u.uswallow && interference) {
		pline("SUBJECT %s VERB_BEHINDERN OBJECT PRONOMEN_PERSONAL.", Monnam(u.ustuck)); /* EN pline("%s interferes.", Monnam(u.ustuck)); */
		mstatusline(u.ustuck);
		return res;
	} else if (u.dz) {
		if (Underwater)
		    You_hear("leises Geplätscher."); /* EN You_hear("faint splashing."); */
		else if (u.dz < 0 || !can_reach_floor())
		    pline("SUBJECT ARTIKEL_BESTIMMTER %s ist nicht erreichbar.", /* EN You_cant("reach the %s.", */
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
		else if (its_dead(u.ux, u.uy, &res))
		    ;	/* message already given */
		else if (Is_stronghold(&u.uz))
		    You_hear("prasselnden Höllenfeuer."); /* EN You_hear("the crackling of hellfire."); */
		else
		    pline_The("%s VERB_SCHEINEN OBJECT KASUS_DATIV gesund genug.", surface(u.ux,u.uy)); /* EN pline_The("%s seems healthy enough.", surface(u.ux,u.uy)); */
		return res;
	} else if (obj->cursed && !rn2(2)) {
		You_hear("OBJECT PRONOMEN_POSSESSIV NOUN_HERZSCHLAG."); /* EN You_hear("your heart beat."); */
		return res;
	}
	if (Stunned || (Confusion && !rn2(5))) confdir();
	if (!u.dx && !u.dy) {
		ustatusline();
		return res;
	}
	rx = u.ux + u.dx; ry = u.uy + u.dy;
	if (!isok(rx,ry)) {
		You_hear("ein schwaches Tippgeräusch."); /* EN You_hear("a faint typing noise."); */
		return 0;
	}
	if ((mtmp = m_at(rx,ry)) != 0) {
		mstatusline(mtmp);
		if (mtmp->mundetected) {
			mtmp->mundetected = 0;
			if (cansee(rx,ry)) newsym(mtmp->mx,mtmp->my);
		}
		if (!canspotmon(mtmp))
			map_invisible(rx,ry);
		return res;
	}
	if (glyph_is_invisible(levl[rx][ry].glyph)) {
		unmap_object(rx, ry);
		newsym(rx, ry);
		pline_The("ADJEKTIV_UNSICHTBAR NOUN_MONSTER muss sich bewegt haben."); /* EN pline_The("invisible monster must have moved."); */
	}
	lev = &levl[rx][ry];
	switch(lev->typ) {
	case SDOOR:
		You_hear(hollow_str, "NOUN_DOOR"); /* EN You_hear(hollow_str, "door"); */
		cvt_sdoor_to_door(lev);		/* ->typ = DOOR */
		if (Blind) feel_location(rx,ry);
		else newsym(rx,ry);
		return res;
	case SCORR:
		You_hear(hollow_str, "NOUN_PASSAGE"); /* EN You_hear(hollow_str, "passage"); */
		lev->typ = CORR;
		unblock_point(rx,ry);
		if (Blind) feel_location(rx,ry);
		else newsym(rx,ry);
		return res;
	}

	if (!its_dead(rx, ry, &res))
	    You("VERB_HOEREN nichts Besonderes.");	/* not You_hear()  */ /* EN You("hear nothing special.");	*/
	return res;
}

static const char whistle_str[] = "VERB_ERZEUGEN einen %s Pfeifton."; /* EN static const char whistle_str[] = "produce a %s whistling sound."; */

STATIC_OVL void
use_whistle(obj)
struct obj *obj;
{
	You(whistle_str, obj->cursed ? "schrillen" : "hohen"); /* EN You(whistle_str, obj->cursed ? "shrill" : "high"); */
	wake_nearby();
}

STATIC_OVL void
use_magic_whistle(obj)
struct obj *obj;
{
	register struct monst *mtmp, *nextmon;

	if(obj->cursed && !rn2(2)) {
		You("produce a high-pitched humming noise."); /* EN You("produce a high-pitched humming noise."); */ // TODO DE
		wake_nearby();
	} else {
		int pet_cnt = 0;
		You(whistle_str, Hallucination ? "normalen" : "seltsamen"); /* EN You(whistle_str, Hallucination ? "normal" : "strange"); */
		for(mtmp = fmon; mtmp; mtmp = nextmon) {
		    nextmon = mtmp->nmon; /* trap might kill mon */
		    if (DEADMONSTER(mtmp)) continue;
		    if (mtmp->mtame) {
			if (mtmp->mtrapped) {
			    /* no longer in previous trap (affects mintrap) */
			    mtmp->mtrapped = 0;
			    fill_pit(mtmp->mx, mtmp->my);
			}
			mnexto(mtmp);
			if (canspotmon(mtmp)) ++pet_cnt;
			if (mintrap(mtmp) == 2) change_luck(-1);
		    }
		}
		if (pet_cnt > 0) makeknown(obj->otyp);
	}
}

boolean
um_dist(x,y,n)
register xchar x, y, n;
{
	return((boolean)(abs(u.ux - x) > n  || abs(u.uy - y) > n));
}

int
number_leashed()
{
	register int i = 0;
	register struct obj *obj;

	for(obj = invent; obj; obj = obj->nobj)
		if(obj->otyp == LEASH && obj->leashmon != 0) i++;
	return(i);
}

void
o_unleash(otmp)		/* otmp is about to be destroyed or stolen */
register struct obj *otmp;
{
	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		if(mtmp->m_id == (unsigned)otmp->leashmon)
			mtmp->mleashed = 0;
	otmp->leashmon = 0;
}

void
m_unleash(mtmp, feedback)	/* mtmp is about to die, or become untame */
register struct monst *mtmp;
boolean feedback;
{
	register struct obj *otmp;

	if (feedback) {
	    if (canseemon(mtmp))
		pline("%s pulls free of %s leash!", Monnam(mtmp), mhis(mtmp)); /* EN pline("%s pulls free of %s leash!", Monnam(mtmp), mhis(mtmp)); */ // TODO DE
	    else
		Your("leash falls slack."); /* EN Your("leash falls slack."); */ // TODO DE
	}
	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == LEASH &&
				otmp->leashmon == (int)mtmp->m_id)
			otmp->leashmon = 0;
	mtmp->mleashed = 0;
}

void
unleash_all()		/* player is about to die (for bones) */
{
	register struct obj *otmp;
	register struct monst *mtmp;

	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == LEASH) otmp->leashmon = 0;
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		mtmp->mleashed = 0;
}

#define MAXLEASHED	2

/* ARGSUSED */
STATIC_OVL void
use_leash(obj)
struct obj *obj;
{
	coord cc;
	register struct monst *mtmp;
	int spotmon;

	if(!obj->leashmon && number_leashed() >= MAXLEASHED) {
		You("cannot leash any more pets."); /* EN You("cannot leash any more pets."); */ // TODO DE
		return;
	}

	if(!get_adjacent_loc((char *)0, (char *)0, u.ux, u.uy, &cc)) return;

	if((cc.x == u.ux) && (cc.y == u.uy)) {
#ifdef STEED
		if (u.usteed && u.dz > 0) {
		    mtmp = u.usteed;
		    spotmon = 1;
		    goto got_target;
		}
#endif
		pline("Fesselspielchen?  Sehr witzig ..."); /* EN pline("Leash yourself?  Very funny..."); */
		return;
	}

	if(!(mtmp = m_at(cc.x, cc.y))) {
		There("is no creature there."); /* EN There("is no creature there."); */ // TODO DE
		return;
	}

	spotmon = canspotmon(mtmp);
#ifdef STEED
 got_target:
#endif

	if(!mtmp->mtame) {
	    if(!spotmon)
		There("is no creature there."); /* EN There("is no creature there."); */ // TODO DE
	    else
		pline("SUBJECT %s %s!", Monnam(mtmp), (!obj->leashmon) ?  /* EN pline("%s %s leashed!", Monnam(mtmp), (!obj->leashmon) ? */
				"VERB_KOENNEN nicht angeleint werden" : "VERB_SEIN nicht angeleint"); /* EN "cannot be" : "is not"); */
	    return;
	}
	if(!obj->leashmon) {
		if(mtmp->mleashed) {
			pline("SUBJECT PRONOMEN_DIESER %s VERB_SEIN bereits angeleint.", /* EN pline("This %s is already leashed.", */
			      spotmon ? l_monnam(mtmp) : "NOUN_MONSTER"); /* EN spotmon ? l_monnam(mtmp) : "monster"); */
			return;
		}
		You("VERB_NEHMEN OBJECT %s%s an die Leine.", /* EN You("slip the leash around %s%s.", */
		    spotmon ? "PRONOMEN_POSSESSIV " : "", l_monnam(mtmp));
		mtmp->mleashed = 1;
		obj->leashmon = (int)mtmp->m_id;
		mtmp->msleeping = 0;
		return;
	}
	if(obj->leashmon != (int)mtmp->m_id) {
		pline("This leash is not attached to that creature."); /* EN pline("This leash is not attached to that creature."); */ // TODO DE
		return;
	} else {
		if(obj->cursed) {
			You("VERB_BEKOMMEN die Leine nicht ab!"); /* EN pline_The("leash would not come off!"); */
			obj->bknown = TRUE;
			return;
		}
		mtmp->mleashed = 0;
		obj->leashmon = 0;
		You("VERB_NEHMEN OBJECT KASUS_DATIV %s%s die Leine ab.", /* EN You("remove the leash from %s%s.", */
		    spotmon ? "PRONOMEN_POSSESSIV " : "", l_monnam(mtmp)); /* EN spotmon ? "your " : "", l_monnam(mtmp)); */
	}
	return;
}

struct obj *
get_mleash(mtmp)	/* assuming mtmp->mleashed has been checked */
register struct monst *mtmp;
{
	register struct obj *otmp;

	otmp = invent;
	while(otmp) {
		if(otmp->otyp == LEASH && otmp->leashmon == (int)mtmp->m_id)
			return(otmp);
		otmp = otmp->nobj;
	}
	return((struct obj *)0);
}

#endif /* OVLB */
#ifdef OVL1

boolean
next_to_u()
{
	register struct monst *mtmp;
	register struct obj *otmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		if(mtmp->mleashed) {
			if (distu(mtmp->mx,mtmp->my) > 2) mnexto(mtmp);
			if (distu(mtmp->mx,mtmp->my) > 2) {
			    for(otmp = invent; otmp; otmp = otmp->nobj)
				if(otmp->otyp == LEASH &&
					otmp->leashmon == (int)mtmp->m_id) {
				    if(otmp->cursed) return(FALSE);
				    You_feel("%s leash go slack.", /* EN You_feel("%s leash go slack.", */ // TODO DE
					(number_leashed() > 1) ? "a" : "the");
				    mtmp->mleashed = 0;
				    otmp->leashmon = 0;
				}
			}
		}
	}
#ifdef STEED
	/* no pack mules for the Amulet */
	if (u.usteed && mon_has_amulet(u.usteed)) return FALSE;
#endif
	return(TRUE);
}

#endif /* OVL1 */
#ifdef OVL0

void
check_leash(x, y)
register xchar x, y;
{
	register struct obj *otmp;
	register struct monst *mtmp;

	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if (otmp->otyp != LEASH || otmp->leashmon == 0) continue;
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		if ((int)mtmp->m_id == otmp->leashmon) break; 
	    }
	    if (!mtmp) {
		impossible("leash in use isn't attached to anything?");
		otmp->leashmon = 0;
		continue;
	    }
	    if (dist2(u.ux,u.uy,mtmp->mx,mtmp->my) >
		    dist2(x,y,mtmp->mx,mtmp->my)) {
		if (!um_dist(mtmp->mx, mtmp->my, 3)) {
		    ;	/* still close enough */
		} else if (otmp->cursed && !breathless(mtmp->data)) {
		    if (um_dist(mtmp->mx, mtmp->my, 5) ||
			    (mtmp->mhp -= rnd(2)) <= 0) {
			long save_pacifism = u.uconduct.killer;

			Your("leash chokes %s to death!", mon_nam(mtmp)); /* EN Your("leash chokes %s to death!", mon_nam(mtmp)); */ // TODO DE
			/* hero might not have intended to kill pet, but
			   that's the result of his actions; gain experience,
			   lose pacifism, take alignment and luck hit, make
			   corpse less likely to remain tame after revival */
			xkilled(mtmp, 0);	/* no "you kill it" message */
			/* life-saving doesn't ordinarily reset this */
			if (mtmp->mhp > 0) u.uconduct.killer = save_pacifism;
		    } else {
			pline("%s chokes on the leash!", Monnam(mtmp)); /* EN pline("%s chokes on the leash!", Monnam(mtmp)); */ // TODO DE
			/* tameness eventually drops to 1 here (never 0) */
			if (mtmp->mtame && rn2(mtmp->mtame)) mtmp->mtame--;
		    }
		} else {
		    if (um_dist(mtmp->mx, mtmp->my, 5)) {
			pline("SUBJECT %s snaps loose!", genitivattribut_zu_wort(Monnam(mtmp), "NOUN_LEASH")); /* EN pline("%s leash snaps loose!", s_suffix(Monnam(mtmp))); */ // TODO DE
			m_unleash(mtmp, FALSE);
		    } else {
			You("VERB_ZIEHEN an der Leine."); /* EN You("pull on the leash."); */
			if (mtmp->data->msound != MS_SILENT)
			    switch (rn2(3)) {
			    case 0:  growl(mtmp);   break;
			    case 1:  yelp(mtmp);    break;
			    default: whimper(mtmp); break;
			    }
		    }
		}
	    }
	}
}

#endif /* OVL0 */
#ifdef OVLB

#define WEAK	3	/* from eat.c */

static const char look_str[] = "VERB_AUSSEHEN %s SATZKLAMMER."; /* EN static const char look_str[] = "look %s."; */

STATIC_OVL int
use_mirror(obj)
struct obj *obj;
{
	register struct monst *mtmp;
	register char mlet;
	boolean vis;

	if(!getdir((char *)0)) return 0;
	if(obj->cursed && !rn2(2)) {
		if (!Blind)
			pline_The("mirror fogs up and doesn't reflect!"); /* EN pline_The("mirror fogs up and doesn't reflect!"); */ // TODO DE
		return 1;
	}
	if(!u.dx && !u.dy && !u.dz) {
		if(!Blind && !Invisible) {
		    if (u.umonnum == PM_FLOATING_EYE) {
			if (!Free_action) {
			pline(Hallucination ?
			      "Hey!  Der Spiegel starrt zurück!" : /* EN "Yow!  The mirror stares back!" : */
			      "Yikes!  You've frozen yourself!"); /* EN "Yikes!  You've frozen yourself!"); */ // TODO DE
			nomul(-rnd((MAXULEV+6) - u.ulevel));
			} else You("VERB_VERSTEIFEN OBJECT PRONOMEN_PERSONAL kurz NEUES_OBJECT OBJECT KASUS_DATIV unter PRONOMEN_POSSESSIV NOUN_GAZE."); /* EN } else You("stiffen momentarily under your gaze."); */
		    } else if (youmonst.data->mlet == S_VAMPIRE)
			You("VERB_HABEN kein Spiegelbild."); /* EN You("don't have a reflection."); */
		    else if (u.umonnum == PM_UMBER_HULK) {
			pline("Hm?  Das Spiegelbild sieht KASUS_DATIV PRONOMEN_PERSONAL gar nicht ähnlich!"); /* EN pline("Huh?  That doesn't look like you!"); */
			make_confused(HConfusion + d(3,4),FALSE);
		    } else if (Hallucination)
			You(look_str, hcolor((char *)0));
		    else if (Sick)
			You(look_str, "blass"); /* EN You(look_str, "peaked"); */
		    else if (u.uhs >= WEAK)
			You(look_str, "unterernährt"); /* EN You(look_str, "undernourished"); */
		    else You("VERB_AUSSEHEN so %s SATZKLAMMER wie eh und je.", /* EN else You("look as %s as ever.", */
				ACURR(A_CHA) > 14 ?
				(poly_gender()==1 ? "beautiful" : "handsome") : /* EN (poly_gender()==1 ? "beautiful" : "handsome") : */ // TODO DE
				"hässlich"); /* EN "ugly"); */
		} else {
			You("VERB_KOENNEN OBJECT PRONOMEN_POSSESSIV %s %s nicht sehen.", /* EN You_cant("see your %s %s.", */
				ACURR(A_CHA) > 14 ?
				(poly_gender()==1 ? "beautiful" : "handsome") : /* EN (poly_gender()==1 ? "beautiful" : "handsome") : */ // TODO DE
				"hässlich", /* EN "ugly", */ // TODO DE
				body_part(FACE));
		}
		return 1;
	}
	if(u.uswallow) {
		if (!Blind) You("reflect %s.", genitivattribut_zu_wort(mon_nam(u.ustuck), /* EN if (!Blind) You("reflect %s %s.", s_suffix(mon_nam(u.ustuck)), */ // TODO DE
		    mbodypart(u.ustuck, STOMACH))); /* EN mbodypart(u.ustuck, STOMACH)); */
		return 1;
	}
	if(Underwater) {
		You(Hallucination ?
		    "give the fish a chance to fix their makeup." : /* EN "give the fish a chance to fix their makeup." : */ // TODO DE
		    "reflect the murky water."); /* EN "reflect the murky water."); */ // TODO DE
		return 1;
	}
	if(u.dz) {
		if (!Blind)
		    You("reflect the %s.", /* EN You("reflect the %s.", */ // TODO DE
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
		return 1;
	}
	mtmp = bhit(u.dx, u.dy, COLNO, INVIS_BEAM,
		    (int FDECL((*),(MONST_P,OBJ_P)))0,
		    (int FDECL((*),(OBJ_P,OBJ_P)))0,
		    obj);
	if (!mtmp || !haseyes(mtmp->data))
		return 1;

	vis = canseemon(mtmp);
	mlet = mtmp->data->mlet;
	if (mtmp->msleeping) {
		if (vis)
		    pline ("SUBJECT %s VERB_SEIN zu müde um OBJECT PRONOMEN_POSSESSIV NOUN_MIRROR anzuschauen.", /* EN pline ("%s is too tired to look at your mirror.", */
			    Monnam(mtmp));
	} else if (!mtmp->mcansee) {
	    if (vis)
		pline("SUBJECT %s VERB_KOENNEN im Moment gar nichts sehen.", Monnam(mtmp)); /* EN pline("%s can't see anything right now.", Monnam(mtmp)); */
	/* some monsters do special things */
	} else if (mlet == S_VAMPIRE || mlet == S_GHOST) {
	    if (vis)
		pline ("SUBJECT %s VERB_HABEN kein Spiegelbild.", Monnam(mtmp)); /* EN pline ("%s doesn't have a reflection.", Monnam(mtmp)); */
	} else if(!mtmp->mcan && !mtmp->minvis &&
					mtmp->data == &mons[PM_MEDUSA]) {
		if (mon_reflects(mtmp, "The gaze is reflected away by %s!")) /* EN if (mon_reflects(mtmp, "The gaze is reflected away by %s %s!")) */ // TODO DE
			return 1;
		if (vis)
			pline("SUBJECT %s VERB_VERSTEINERN!", Monnam(mtmp)); /* EN pline("%s is turned to stone!", Monnam(mtmp)); */
		stoned = TRUE;
		killed(mtmp);
	} else if(!mtmp->mcan && !mtmp->minvis &&
					mtmp->data == &mons[PM_FLOATING_EYE]) {
		int tmp = d((int)mtmp->m_lev, (int)mtmp->data->mattk[0].damd);
		if (!rn2(4)) tmp = 120;
		if (vis)
			pline("%s is frozen by its reflection.", Monnam(mtmp)); /* EN pline("%s is frozen by its reflection.", Monnam(mtmp)); */ // TODO DE
		else You_hear("%s stop moving.",something); /* EN else You_hear("%s stop moving.",something); */ // TODO DE
		mtmp->mcanmove = 0;
		if ( (int) mtmp->mfrozen + tmp > 127)
			mtmp->mfrozen = 127;
		else mtmp->mfrozen += tmp;
	} else if(!mtmp->mcan && !mtmp->minvis &&
					mtmp->data == &mons[PM_UMBER_HULK]) {
		if (vis)
			pline ("SUBJECT %s VERB_VERWIRREN sich selbst!", Monnam(mtmp)); /* EN pline ("%s confuses itself!", Monnam(mtmp)); */
		mtmp->mconf = 1;
	} else if(!mtmp->mcan && !mtmp->minvis && (mlet == S_NYMPH
				     || mtmp->data==&mons[PM_SUCCUBUS])) {
		if (vis) {
		    pline ("SUBJECT %s VERB_BEWUNDERN sich OBJECT KASUS_DATIV in PRONOMEN_POSSESSIV NOUN_MIRROR.", Monnam(mtmp)); /* EN pline ("%s admires herself in your mirror.", Monnam(mtmp)); */
		    pline ("Sie nimmt ihn!"); /* EN pline ("She takes it!"); */
		} else pline ("SUBJECT NOUN_JEMAND VERB_STEHLEN OBJECT PRONOMEN_POSSESSIV NOUN_MIRROR!"); /* EN } else pline ("It steals your mirror!"); */
		setnotworn(obj); /* in case mirror was wielded */
		freeinv(obj);
		(void) mpickobj(mtmp,obj);
		if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
	} else if (!is_unicorn(mtmp->data) && !humanoid(mtmp->data) &&
			(!mtmp->minvis || perceives(mtmp->data)) && rn2(5)) {
		if (vis)
		    pline("SUBJECT %s VERB_SEIN OBJECT KASUS_DATIV von PRONOMEN_POSSESSIV_SUBJECT NOUN_REFLECTION verängstigt.", Monnam(mtmp)); /* EN pline("%s is frightened by its reflection.", Monnam(mtmp)); */
		monflee(mtmp, d(2,4), FALSE, FALSE);
	} else if (!Blind) {
		if (mtmp->minvis && !See_invisible)
		    ;
		else if ((mtmp->minvis && !perceives(mtmp->data))
			 || !haseyes(mtmp->data))
		    pline("SUBJECT %s VERB_SCHEINEN OBJECT PRONOMEN_POSSESSIV_SUBJECT NOUN_REFLECTION nicht zu bemerken.", /* EN pline("%s doesn't seem to notice its reflection.", */
			Monnam(mtmp));
		else
		    pline("SUBJECT %s VERB_IGNORIEREN OBJECT PRONOMEN_POSSESSIV_SUBJECT NOUN_REFLECTION.", /* EN pline("%s ignores %s reflection.", */
			  Monnam(mtmp)); /* EN Monnam(mtmp), mhis(mtmp)); */
	}
	return 1;
}

STATIC_OVL void
use_bell(optr)
struct obj **optr;
{
	register struct obj *obj = *optr;
	struct monst *mtmp;
	boolean wakem = FALSE, learno = FALSE,
		ordinary = (obj->otyp != BELL_OF_OPENING || !obj->spe),
		invoking = (obj->otyp == BELL_OF_OPENING &&
			 invocation_pos(u.ux, u.uy) && !On_stairs(u.ux, u.uy));

	You("VERB_LAEUTEN OBJECT %s.", the(xname(obj))); /* EN You("ring %s.", the(xname(obj))); */

	if (Underwater || (u.uswallow && ordinary)) {
#ifdef	AMIGA
	    amii_speaker( obj, "AhDhGqEqDhEhAqDqFhGw", AMII_MUFFLED_VOLUME );
#endif
	    pline("But the sound is muffled."); /* EN pline("But the sound is muffled."); */ // TODO DE

	} else if (invoking && ordinary) {
	    /* needs to be recharged... */
	    pline("Aber SUBJECT_IM_SATZ PRONOMEN_PERSONAL VERB_HOEREN keinen Ton."); /* EN pline("But it makes no sound."); */
	    learno = TRUE;	/* help player figure out why */

	} else if (ordinary) {
#ifdef	AMIGA
	    amii_speaker( obj, "ahdhgqeqdhehaqdqfhgw", AMII_MUFFLED_VOLUME );
#endif
	    if (obj->cursed && !rn2(4) &&
		    /* note: once any of them are gone, we stop all of them */
		    !(mvitals[PM_WOOD_NYMPH].mvflags & G_GONE) &&
		    !(mvitals[PM_WATER_NYMPH].mvflags & G_GONE) &&
		    !(mvitals[PM_MOUNTAIN_NYMPH].mvflags & G_GONE) &&
		    (mtmp = makemon(mkclass(S_NYMPH, 0),
					u.ux, u.uy, NO_MINVENT)) != 0) {
		You("VERB_RUFEN OBJECT %s herbei!", a_monnam(mtmp)); /* EN You("summon %s!", a_monnam(mtmp)); */
		if (!obj_resists(obj, 93, 100)) {
		    pline("SUBJECT %s zersprungen!", Tobjnam(obj, "VERB_SEIN")); /* EN pline("%s shattered!", Tobjnam(obj, "have")); */
		    useup(obj);
		    *optr = 0;
		} else switch (rn2(3)) {
			default:
				break;
			case 1:
				mon_adjust_speed(mtmp, 2, (struct obj *)0);
				break;
			case 2: /* no explanation; it just happens... */
				nomovemsg = "";
				nomul(-rnd(2));
				break;
		}
	    }
	    wakem = TRUE;

	} else {
	    /* charged Bell of Opening */
	    consume_obj_charge(obj, TRUE);

	    if (u.uswallow) {
		if (!obj->cursed)
		    (void) openit();
		else
		    pline(nothing_happens);

	    } else if (obj->cursed) {
		coord mm;

		mm.x = u.ux;
		mm.y = u.uy;
		mkundead(&mm, FALSE, NO_MINVENT);
		wakem = TRUE;

	    } else  if (invoking) {
		pline("SUBJECT %s an beunruhigenden schrillen Ton ...", /* EN pline("%s an unsettling shrill sound...", */
		      Tobjnam(obj, "issue")); /* EN Tobjnam(obj, "issue")); */ // TODO DE
#ifdef	AMIGA
		amii_speaker( obj, "aefeaefeaefeaefeaefe", AMII_LOUDER_VOLUME );
#endif
		obj->age = moves;
		learno = TRUE;
		wakem = TRUE;

	    } else if (obj->blessed) {
		int res = 0;

#ifdef	AMIGA
		amii_speaker( obj, "ahahahDhEhCw", AMII_SOFT_VOLUME );
#endif
		if (uchain) {
		    unpunish();
		    res = 1;
		}
		res += openit();
		switch (res) {
		  case 0:  pline(nothing_happens); break;
		  case 1:  pline("SUBJECT %s VERB_OEFFNEN sich ...", Something); /* EN case 1:  pline("%s opens...", Something); */
			   learno = TRUE; break;
		  default: pline("Die Dinge öffnen sich KASUS_DATIV um PRONOMEN_PERSONAL ..."); /* EN default: pline("Things open around you..."); */
			   learno = TRUE; break;
		}

	    } else {  /* uncursed */
#ifdef	AMIGA
		amii_speaker( obj, "AeFeaeFeAefegw", AMII_OKAY_VOLUME );
#endif
		if (findit() != 0) learno = TRUE;
		else pline(nothing_happens);
	    }

	}	/* charged BofO */

	if (learno) {
	    makeknown(BELL_OF_OPENING);
	    obj->known = 1;
	}
	if (wakem) wake_nearby();
}

STATIC_OVL void
use_candelabrum(obj)
register struct obj *obj;
{
	const char *s = (obj->spe != 1) ? "NOUN_CANDLEs" : "NOUN_CANDLE"; /* EN const char *s = (obj->spe != 1) ? "candles" : "candle"; */

	if(Underwater) {
		You("VERB_KOENNEN kein Feuer unter Wasser machen."); /* EN You("cannot make fire under water."); */
		return;
	}
	if(obj->lamplit) {
		You("VERB_AUSLOESCHEN OBJECT ARTIKEL_BESTIMMTER %s SATZKLAMMER.", s); /* EN You("snuff the %s.", s); */
		end_burn(obj, TRUE);
		return;
	}
	if(obj->spe <= 0) {
		pline("SUBJECT PRONOMEN_DIESER %s VERB_HAVE OBJECT PRONOMEN_KEIN %s.", xname(obj), s); /* EN pline("This %s has no %s.", xname(obj), s); */
		return;
	}
	if(u.uswallow || obj->cursed) {
		if (!Blind)
		    pline_The("%s %s einen Moment lang, then %s.", /* EN pline_The("%s %s for a moment, then %s.", */ // TODO DE
			      s, vtense(s, "VERB_FLACKERN"), vtense(s, "die")); /* EN s, vtense(s, "flicker"), vtense(s, "die")); */ // TODO DE
		return;
	}
	if(obj->spe < 7) {
		pline("Es %s nur OBJECT %d %s in %s.", /* EN There("%s only %d %s in %s.", */ // TODO DE
		      vtense(s, "VERB_STECKEN"), obj->spe, s, the(xname(obj))); /* EN vtense(s, "are"), obj->spe, s, the(xname(obj))); */
		if (!Blind)
		    pline("%s lit.  %s dimly.", /* EN pline("%s lit.  %s dimly.", */ // TODO DE
			  obj->spe == 1 ? "It is" : "They are", /* EN obj->spe == 1 ? "It is" : "They are", */ // TODO DE
			  Tobjnam(obj, "shine")); /* EN Tobjnam(obj, "shine")); */ // TODO DE
	} else {
		pline("%s's %s burn%s", The(xname(obj)), s, /* EN pline("%s's %s burn%s", The(xname(obj)), s, */ // TODO DE
			(Blind ? "." : " brightly!")); /* EN (Blind ? "." : " brightly!")); */ // TODO DE
	}
	if (!invocation_pos(u.ux, u.uy)) {
		pline_The("%s %s being rapidly consumed!", s, vtense(s, "are")); /* EN pline_The("%s %s being rapidly consumed!", s, vtense(s, "are")); */ // TODO DE
		obj->age /= 2;
	} else {
		if(obj->spe == 7) {
		    if (Blind)
		      pline("%s a strange warmth!", Tobjnam(obj, "radiate")); /* EN pline("%s a strange warmth!", Tobjnam(obj, "radiate")); */ // TODO DE
		    else
		      pline("%s with a strange light!", Tobjnam(obj, "glow")); /* EN pline("%s with a strange light!", Tobjnam(obj, "glow")); */ // TODO DE
		}
		obj->known = 1;
	}
	begin_burn(obj, FALSE);
}

STATIC_OVL void
use_candle(optr)
struct obj **optr;
{
	register struct obj *obj = *optr;
	register struct obj *otmp;
	const char *s = (obj->quan != 1) ? "NOUN_CANDLEs" : "NOUN_CANDLE"; /* EN const char *s = (obj->quan != 1) ? "candles" : "candle"; */
	char qbuf[QBUFSZ];

	if(u.uswallow) {
		You(no_elbow_room);
		return;
	}
	if(Underwater) {
		pline("Sorry, fire and water don't mix."); /* EN pline("Sorry, fire and water don't mix."); */ // TODO DE
		return;
	}

	otmp = carrying(CANDELABRUM_OF_INVOCATION);
	if(!otmp || otmp->spe == 7) {
		use_lamp(obj);
		return;
	}

	Sprintf(qbuf, "Attach %s", the(xname(obj))); /* EN Sprintf(qbuf, "Attach %s", the(xname(obj))); */ // TODO DE
	Sprintf(eos(qbuf), " to %s?", /* EN Sprintf(eos(qbuf), " to %s?", */ // TODO DE
		safe_qbuf(qbuf, sizeof(" to ?"), the(xname(otmp)), /* EN safe_qbuf(qbuf, sizeof(" to ?"), the(xname(otmp)), */ // TODO DE
			the(simple_typename(otmp->otyp)), "it")); /* EN the(simple_typename(otmp->otyp)), "it")); */ // TODO DE
	if(yn(qbuf) == 'n') {
		if (!obj->lamplit)
		    You("try to light %s...", the(xname(obj))); /* EN You("try to light %s...", the(xname(obj))); */ // TODO DE
		use_lamp(obj);
		return;
	} else {
		if ((long)otmp->spe + obj->quan > 7L)
		    obj = splitobj(obj, 7L - (long)otmp->spe);
		else *optr = 0;
		You("attach %ld%s %s to %s.", /* EN You("attach %ld%s %s to %s.", */ // TODO DE
		    obj->quan, !otmp->spe ? "" : " more", /* EN obj->quan, !otmp->spe ? "" : " more", */ // TODO DE
		    s, the(xname(otmp)));
		if (!otmp->spe || otmp->age > obj->age)
		    otmp->age = obj->age;
		otmp->spe += (int)obj->quan;
		if (otmp->lamplit && !obj->lamplit)
		    pline_The("new %s magically %s!", s, vtense(s, "ignite")); /* EN pline_The("new %s magically %s!", s, vtense(s, "ignite")); */ // TODO DE
		else if (!otmp->lamplit && obj->lamplit)
		    pline("%s out.", (obj->quan > 1L) ? "They go" : "It goes"); /* EN pline("%s out.", (obj->quan > 1L) ? "They go" : "It goes"); */ // TODO DE
		if (obj->unpaid)
		    verbalize("You %s %s, you bought %s!", /* EN verbalize("You %s %s, you bought %s!", */ // TODO DE
			      otmp->lamplit ? "burn" : "use", /* EN otmp->lamplit ? "burn" : "use", */ // TODO DE
			      (obj->quan > 1L) ? "them" : "it",
			      (obj->quan > 1L) ? "them" : "it");
		if (obj->quan < 7L && otmp->spe == 7)
		    pline("%s now has seven%s candles attached.", /* EN pline("%s now has seven%s candles attached.", */ // TODO DE
			  The(xname(otmp)), otmp->lamplit ? " lit" : ""); /* EN The(xname(otmp)), otmp->lamplit ? " lit" : ""); */ // TODO DE
		/* candelabrum's light range might increase */
		if (otmp->lamplit) obj_merge_light_sources(otmp, otmp);
		/* candles are no longer a separate light source */
		if (obj->lamplit) end_burn(obj, TRUE);
		/* candles are now gone */
		useupall(obj);
	}
}

boolean
snuff_candle(otmp)  /* call in drop, throw, and put in box, etc. */
register struct obj *otmp;
{
	register boolean candle = Is_candle(otmp);

	if ((candle || otmp->otyp == CANDELABRUM_OF_INVOCATION) &&
		otmp->lamplit) {
	    char buf[BUFSZ];
	    xchar x, y;
	    register boolean many = candle ? otmp->quan > 1L : otmp->spe > 1;

	    (void) get_obj_location(otmp, &x, &y, 0);
	    if (otmp->where == OBJ_MINVENT ? cansee(x,y) : !Blind)
		pline("%s %scandle%s flame%s extinguished.", /* EN pline("%s %scandle%s flame%s extinguished.", */ // TODO DE
		      Shk_Your(buf, otmp),
		      (candle ? "" : "candelabrum's "), /* EN (candle ? "" : "candelabrum's "), */ // TODO DE
		      (many ? "s'" : "'s"), (many ? "s are" : " is")); /* EN (many ? "s'" : "'s"), (many ? "s are" : " is")); */ // TODO DE
	   end_burn(otmp, TRUE);
	   return(TRUE);
	}
	return(FALSE);
}

/* called when lit lamp is hit by water or put into a container or
   you've been swallowed by a monster; obj might be in transit while
   being thrown or dropped so don't assume that its location is valid */
boolean
snuff_lit(obj)
struct obj *obj;
{
	xchar x, y;

	if (obj->lamplit) {
	    if (obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
		    obj->otyp == BRASS_LANTERN || obj->otyp == POT_OIL) {
		(void) get_obj_location(obj, &x, &y, 0);
		if (obj->where == OBJ_MINVENT ? cansee(x,y) : !Blind)
		    pline("SUBJECT %s %s SATZKLAMMER!", Yname2(obj), otense(obj, "VERB_AUSGEHEN")); /* EN pline("%s %s out!", Yname2(obj), otense(obj, "go")); */
		end_burn(obj, TRUE);
		return TRUE;
	    }
	    if (snuff_candle(obj)) return TRUE;
	}
	return FALSE;
}

/* Called when potentially lightable object is affected by fire_damage().
   Return TRUE if object was lit and FALSE otherwise --ALI */
boolean
catch_lit(obj)
struct obj *obj;
{
	xchar x, y;

	if (!obj->lamplit && (obj->otyp == MAGIC_LAMP || ignitable(obj))) {
	    if ((obj->otyp == MAGIC_LAMP ||
		 obj->otyp == CANDELABRUM_OF_INVOCATION) &&
		obj->spe == 0)
		return FALSE;
	    else if (obj->otyp != MAGIC_LAMP && obj->age == 0)
		return FALSE;
	    if (!get_obj_location(obj, &x, &y, 0))
		return FALSE;
	    if (obj->otyp == CANDELABRUM_OF_INVOCATION && obj->cursed)
		return FALSE;
	    if ((obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
		 obj->otyp == BRASS_LANTERN) && obj->cursed && !rn2(2))
		return FALSE;
	    if (obj->where == OBJ_MINVENT ? cansee(x,y) : !Blind)
		pline("SUBJECT %s %s Feuer!", Yname2(obj), otense(obj, "VERB_FANGEN")); /* EN pline("%s %s light!", Yname2(obj), otense(obj, "catch")); */
	    if (obj->otyp == POT_OIL) makeknown(obj->otyp);
	    if (obj->unpaid && costly_spot(u.ux, u.uy) && (obj->where == OBJ_INVENT)) {
	        /* if it catches while you have it, then it's your tough luck */
		check_unpaid(obj);
	        verbalize("That's in addition to the cost of %s %s, of course.", /* EN verbalize("That's in addition to the cost of %s %s, of course.", */ // TODO DE
				Yname2(obj), obj->quan == 1 ? "itself" : "themselves"); /* EN Yname2(obj), obj->quan == 1 ? "itself" : "themselves"); */ // TODO DE
		bill_dummy_object(obj);
	    }
	    begin_burn(obj, FALSE);
	    return TRUE;
	}
	return FALSE;
}

STATIC_OVL void
use_lamp(obj)
struct obj *obj;
{
	char buf[BUFSZ];

	if(Underwater) {
		pline("Das ist keine Tauchlampe."); /* EN pline("This is not a diving lamp."); */
		return;
	}
	if(obj->lamplit) {
		if(obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
				obj->otyp == BRASS_LANTERN)
		    pline("SUBJECT %s NOUN_LAMP VERB_SEIN jetzt aus.", Shk_Your(buf, obj)); /* EN pline("%s lamp is now off.", Shk_Your(buf, obj)); */
		else
		    You("VERB_AUSLOESCHEN OBJECT %s SATZKLAMMER.", yname(obj)); /* EN You("snuff out %s.", yname(obj)); */
		end_burn(obj, TRUE);
		return;
	}
	/* magic lamps with an spe == 0 (wished for) cannot be lit */
	if ((!Is_candle(obj) && obj->age == 0)
			|| (obj->otyp == MAGIC_LAMP && obj->spe == 0)) {
		if (obj->otyp == BRASS_LANTERN)
			Deinem("NOUN_LANTERN ist das Licht ausgegangen."); /* EN Your("lamp has run out of power."); */
		else pline("SUBJECT PRONOMEN_DIESER %s VERB_HAVE kein Öl.", xname(obj)); /* EN else pline("This %s has no oil.", xname(obj)); */
		return;
	}
	if (obj->cursed && !rn2(2)) {
		pline("SUBJECT %s for a moment, dann %s.", /* EN pline("%s for a moment, then %s.", */ // TODO DE
		      Tobjnam(obj, "VERB_FLACKERN"), otense(obj, "die")); /* EN Tobjnam(obj, "flicker"), otense(obj, "die")); */ // TODO DE
	} else {
		if(obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
				obj->otyp == BRASS_LANTERN) {
		    check_unpaid(obj);
		    pline("SUBJECT %s NOUN_LAMP VERB_BRENNEN jetzt.", Shk_Your(buf, obj)); /* EN pline("%s lamp is now on.", Shk_Your(buf, obj)); */
		} else {	/* candle(s) */
		    pline("SUBJECT %s %s%s", /* EN pline("%s flame%s %s%s", */
			Yname2(obj), /* EN s_suffix(Yname2(obj)), */
			otense(obj, "VERB_BRENNEN"), /* EN plur(obj->quan), otense(obj, "burn"), */
			Blind ? "." : " hell!"); /* EN Blind ? "." : " brightly!"); */
		    if (obj->unpaid && costly_spot(u.ux, u.uy) &&
			  obj->age == 20L * (long)objects[obj->otyp].oc_cost) {
			const char *ithem = obj->quan > 1L ? "them" : "it"; /* EN const char *ithem = obj->quan > 1L ? "them" : "it"; */ // TODO DE
			verbalize("You burn %s, you bought %s!", ithem, ithem); /* EN verbalize("You burn %s, you bought %s!", ithem, ithem); */ // TODO DE
			bill_dummy_object(obj);
		    }
		}
		begin_burn(obj, FALSE);
	}
}

STATIC_OVL void
light_cocktail(obj)
	struct obj *obj;	/* obj is a potion of oil */
{
	char buf[BUFSZ];

	if (u.uswallow) {
	    You(no_elbow_room);
	    return;
	}

	if (obj->lamplit) {
	    You("VERB_LOESCHEN den entzündeten Trank."); /* EN You("snuff the lit potion."); */
	    end_burn(obj, TRUE);
	    /*
	     * Free & add to re-merge potion.  This will average the
	     * age of the potions.  Not exactly the best solution,
	     * but its easy.
	     */
	    freeinv(obj);
	    (void) addinv(obj);
	    return;
	} else if (Underwater) {
	    pline("Hier gibt es nicht genug Sauerstoff um ein Feuer aufrechtzuerhalten."); /* EN There("is not enough oxygen to sustain a fire."); */
	    return;
	}

	You("VERB_ENTZUENDEN OBJECT %s NOUN_POTION.%s", shk_your(buf, obj), /* EN You("light %s potion.%s", shk_your(buf, obj), */
	    Blind ? "" : "  Er gibt nur ein schummriges Licht ab."); /* EN Blind ? "" : "  It gives off a dim light."); */
	if (obj->unpaid && costly_spot(u.ux, u.uy)) {
	    /* Normally, we shouldn't both partially and fully charge
	     * for an item, but (Yendorian Fuel) Taxes are inevitable...
	     */
	    check_unpaid(obj);
	    verbalize("Das ist natürlich zusätzlich zu den Kosten des Trankes."); /* EN verbalize("That's in addition to the cost of the potion, of course."); */
	    bill_dummy_object(obj);
	}
	makeknown(obj->otyp);

	if (obj->quan > 1L) {
	    obj = splitobj(obj, 1L);
	    begin_burn(obj, FALSE);	/* burn before free to get position */
	    obj_extract_self(obj);	/* free from inv */

	    /* shouldn't merge */
	    obj = hold_another_object(obj, "SUBJECT VERB_DROP %s SATZKLAMMER!", /* EN obj = hold_another_object(obj, "You drop %s!", */
				      doname(obj), (const char *)0);
	} else
	    begin_burn(obj, FALSE);
}

static NEARDATA const char cuddly[] = { TOOL_CLASS, GEM_CLASS, 0 };

int
dorub()
{
	struct obj *obj = getobj(cuddly, "rub"); /* EN struct obj *obj = getobj(cuddly, "rub"); */ // TODO DE

	if (obj && obj->oclass == GEM_CLASS) {
	    if (is_graystone(obj)) {
		use_stone(obj);
		return 1;
	    } else {
		pline("Tut mir Leid, ich weiß nicht, wie das zu benutzen ist."); /* EN pline("Sorry, I don't know how to use that."); */
		return 0;
	    }
	}

	if (!obj || !wield_tool(obj, "rub")) return 0; /* EN if (!obj || !wield_tool(obj, "rub")) return 0; */ // TODO DE

	/* now uwep is obj */
	if (uwep->otyp == MAGIC_LAMP) {
	    if (uwep->spe > 0 && !rn2(3)) {
		check_unpaid_usage(uwep, TRUE);		/* unusual item use */
		djinni_from_bottle(uwep);
		makeknown(MAGIC_LAMP);
		uwep->otyp = OIL_LAMP;
		uwep->spe = 0; /* for safety */
		uwep->age = rn1(500,1000);
		if (uwep->lamplit) begin_burn(uwep, TRUE);
		update_inventory();
	    } else if (rn2(2) && !Blind)
		You("VERB_SEHEN eine Rauchwölkchen."); /* EN You("see a puff of smoke."); */
	    else pline(nothing_happens);
	} else if (obj->otyp == BRASS_LANTERN) {
	    /* message from Adventure */
	    pline("Die elektrische Lampe zu reiben ist nicht besonders befriedigend."); /* EN pline("Rubbing the electric lamp is not particularly rewarding."); */
	    pline("Jedenfalls, nichts Aufregendes passiert."); /* EN pline("Anyway, nothing exciting happens."); */
	} else pline(nothing_happens);
	return 1;
}

int
dojump()
{
	/* Physical jump */
	return jump(0);
}

int
jump(magic)
int magic; /* 0=Physical, otherwise skill level */
{
	coord cc;

	if (!magic && (nolimbs(youmonst.data) || slithy(youmonst.data))) {
		/* normally (nolimbs || slithy) implies !Jumping,
		   but that isn't necessarily the case for knights */
		You_cant("springen. NEUER_SATZ PRONOMEN_PERSONAL VERB_HAVE keine Beine!"); /* EN You_cant("jump; you have no legs!"); */
		return 0;
	} else if (!magic && !Jumping) {
		You_cant("sehr weit springen."); /* EN You_cant("jump very far."); */
		return 0;
	} else if (u.uswallow) {
		if (magic) {
			You("VERB_HUEPFEN ein bisschen herum."); /* EN You("bounce around a little."); */
			return 1;
		}
		You("VERB_MACHEN wohl Witze?!"); /* EN pline("You've got to be kidding!"); */
		return 0;
	} else if (u.uinwater) {
		if (magic) {
			You("VERB_STRAMPELN ein bisschen herum."); /* EN You("swish around a little."); */
			return 1;
		}
		pline("Schwimmen ist angesagt, nicht springen!"); /* EN pline("This calls for swimming, not jumping!"); */
		return 0;
	} else if (u.ustuck) {
		if (u.ustuck->mtame && !Conflict && !u.ustuck->mconf) {
		    You("VERB_REISSEN OBJECT PRONOMEN_PERSONAL NEUES_OBJECT OBJECT von %s los.", mon_nam(u.ustuck)); /* EN You("pull free from %s.", mon_nam(u.ustuck)); */
		    u.ustuck = 0;
		    return 1;
		}
		if (magic) {
			You("VERB_WINDEN OBJECT PRONOMEN_PERSONAL ein wenig NEUES_OBJECT OBJECT KASUS_DATIV in %s!", genitivattribut_zu_wort(mon_nam(u.ustuck),"NOUN_GRIFF")); /* EN You("writhe a little in the grasp of %s!", mon_nam(u.ustuck)); */
			return 1;
		}
		You("VERB_KOENNEN OBJECT KASUS_DATIV %s nicht entfliehen!", mon_nam(u.ustuck)); /* EN You("cannot escape from %s!", mon_nam(u.ustuck)); */
		return 0;
	} else if (Levitation || Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)) {
		if (magic) {
			You("VERB_HAMPELN etwas herum."); /* EN You("flail around a little."); */
			return 1;
		}
		You("VERB_ZAPPELN nicht genug Bodenhaftung zum Springen."); /* EN You("don't have enough traction to jump."); */
		return 0;
	} else if (!magic && near_capacity() > UNENCUMBERED) {
		You("VERB_TRAGEN zuviel zum Springen!"); /* EN You("are carrying too much to jump!"); */
		return 0;
	} else if (!magic && (u.uhunger <= 100 || ACURR(A_STR) < 6)) {
		Dir("fehlt die Kraft zum Springen!"); /* EN You("lack the strength to jump!"); */
		return 0;
	} else if (Wounded_legs) {
		long wl = (Wounded_legs & BOTH_SIDES);
		const char *bp = body_part(LEG);

		if (wl == BOTH_SIDES) bp = makeplural(bp);
#ifdef STEED
		if (u.usteed)
		    pline("SUBJECT %s nicht in der Verfassung zu springen.", Monnam(u.usteed)); /* EN pline("%s is in no shape for jumping.", Monnam(u.usteed)); */
		else
#endif
		Your("%s%s %s nicht zum Springen zu gebrauchen.", /* EN Your("%s%s %s in no shape for jumping.", */
		     (wl == LEFT_SIDE) ? "ADJEKTIV_LINK " : /* EN (wl == LEFT_SIDE) ? "left " : */
			(wl == RIGHT_SIDE) ? "ADJEKTIV_RECHT " : "", /* EN (wl == RIGHT_SIDE) ? "right " : "", */
		     bp, (wl == BOTH_SIDES) ? "VERB_SEIN" : "VERB_SEIN"); /* EN bp, (wl == BOTH_SIDES) ? "are" : "is"); */
		return 0;
	}
#ifdef STEED
	else if (u.usteed && u.utrap) {
		pline("SUBJECT %s VERB_STECKEN in einer Falle fest.", Monnam(u.usteed)); /* EN pline("%s is stuck in a trap.", Monnam(u.usteed)); */
		return (0);
	}
#endif

	pline("SUBJECT Wohin VERB_WOLLEN PRONOMEN_PERSONAL springen?"); /* EN pline("Where do you want to jump?"); */
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "die gewünschte Stelle") < 0) /* EN if (getpos(&cc, TRUE, "the desired position") < 0) */ // TODO DE
		return 0;	/* user pressed ESC */
	if (!magic && !(HJumping & ~INTRINSIC) && !EJumping &&
			distu(cc.x, cc.y) != 5) {
		/* The Knight jumping restriction still applies when riding a
		 * horse.  After all, what shape is the knight piece in chess?
		 */
		pline("Illegaler Zug!"); /* EN pline("Illegal move!"); */
		return 0;
	} else if (distu(cc.x, cc.y) > (magic ? 6+magic*3 : 9)) {
		pline("Zu weit!"); /* EN pline("Too far!"); */
		return 0;
	} else if (!cansee(cc.x, cc.y)) {
		You("VERB_SEHEN nicht, wo NEUER_SATZ PRONOMEN_PERSONAL landen MODIFIER_KONJUNKTIV_II VERB_WERDEN!"); /* EN You("cannot see where to land!"); */
		return 0;
	} else if (!isok(cc.x, cc.y)) {
		You("VERB_KOENNEN nicht dorthin springen!"); /* EN You("cannot jump there!"); */
		return 0;
	} else {
	    coord uc;
	    int range, temp;

	    if(u.utrap)
		switch(u.utraptype) {
		case TT_BEARTRAP: {
		    register long side = rn2(3) ? LEFT_SIDE : RIGHT_SIDE;
		    You("rip yourself free of the bear trap!  Aua!"); /* EN You("rip yourself free of the bear trap!  Ouch!"); */ // TODO DE
		    losehp(rnd(10), "jumping out of a bear trap", KILLED_BY); /* EN losehp(rnd(10), "jumping out of a bear trap", KILLED_BY); */ // TODO DE
		    set_wounded_legs(side, rn1(1000,500));
		    break;
		  }
		case TT_PIT:
		    You("VERB_SPRINGEN aus der Grube!"); /* EN You("leap from the pit!"); */
		    break;
		case TT_WEB:
		    You("VERB_ZERREISSEN das Netz, NEUER_SATZ MODIFIER_NEBENSATZ als SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT PRONOMEN_PERSONAL VERB_LOSREISSEN!"); /* EN You("tear the web apart as you pull yourself free!"); */
		    deltrap(t_at(u.ux,u.uy));
		    break;
		case TT_LAVA:
		    You("VERB_ZIEHEN OBJECT PRONOMEN_PERSONAL über die Lava hoch!"); /* EN You("pull yourself above the lava!"); */
		    u.utrap = 0;
		    return 1;
		case TT_INFLOOR:
		    You("VERB_HABEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL OBJECT PRONOMEN_POSSESSIV %s, NEUER_SATZ aber SUBJECT_IM_SATZ PRONOMEN_PERSONAL VERB_STECKEN immer noch im Boden fest.", /* EN You("strain your %s, but you're still stuck in the floor.", */
			makeplural(body_part(LEG)));
		    set_wounded_legs(LEFT_SIDE, rn1(10, 11));
		    set_wounded_legs(RIGHT_SIDE, rn1(10, 11));
		    return 1;
		}

	    /*
	     * Check the path from uc to cc, calling hurtle_step at each
	     * location.  The final position actually reached will be
	     * in cc.
	     */
	    uc.x = u.ux;
	    uc.y = u.uy;
	    /* calculate max(abs(dx), abs(dy)) as the range */
	    range = cc.x - uc.x;
	    if (range < 0) range = -range;
	    temp = cc.y - uc.y;
	    if (temp < 0) temp = -temp;
	    if (range < temp)
		range = temp;
	    (void) walk_path(&uc, &cc, hurtle_step, (genericptr_t)&range);

	    /* A little Sokoban guilt... */
	    if (In_sokoban(&u.uz))
		change_luck(-1);

	    teleds(cc.x, cc.y, TRUE);
	    nomul(-1);
	    nomovemsg = "";
	    morehungry(rnd(25));
	    return 1;
	}
}

boolean
tinnable(corpse)
struct obj *corpse;
{
	if (corpse->oeaten) return 0;
	if (!mons[corpse->corpsenm].cnutrit) return 0;
	return 1;
}

STATIC_OVL void
use_tinning_kit(obj)
register struct obj *obj;
{
	register struct obj *corpse, *can;

	/* This takes only 1 move.  If this is to be changed to take many
	 * moves, we've got to deal with decaying corpses...
	 */
	if (obj->spe <= 0) {
		You("VERB_SCHEINEN keine Dosen mehr zu haben."); /* EN You("seem to be out of tins."); */
		return;
	}
	if (!(corpse = floorfood("eindosen", 2))) return; /* EN if (!(corpse = floorfood("tin", 2))) return; */
	if (corpse->oeaten) {
		You("VERB_KOENNEN nicht etwas eindosen, was schon angefressen wurde."); /* EN You("cannot tin %s which is partly eaten.",something); */
		return;
	}
	if (touch_petrifies(&mons[corpse->corpsenm])
		&& !Stone_resistance && !uarmg) {
	    char kbuf[BUFSZ];

	    if (poly_when_stoned(youmonst.data))
		You("VERB_EINDOSEN ohne Handschuhe zu tragen OBJECT %s SATZKLAMMER.", /* EN You("tin %s without wearing gloves.", */
			an(mons[corpse->corpsenm].mname));
	    else {
		pline("SATZBEGINN KASUS_AKKUSATIV %s eindosen ohne Handschuhe zu tragen ist ein verhängnisvoller Fehler ...", /* EN pline("Tinning %s without wearing gloves is a fatal mistake...", */
			an(mons[corpse->corpsenm].mname));
		Sprintf(kbuf, "versuchte ohne Handschuhe KASUS_AKKUSATIV %s einzudosen", /* EN Sprintf(kbuf, "trying to tin %s without gloves", */ // TODO DE
			an(mons[corpse->corpsenm].mname));
	    }
	    instapetrify(kbuf);
	}
	if (is_rider(&mons[corpse->corpsenm])) {
		(void) revive_corpse(corpse);
		verbalize("SUBJECT PRONOMEN_PERSONAL VERB_HABEN da was falsch verstanden ...  Der Krieg erhält nur sich selbst ..."); /* EN verbalize("Yes...  But War does not preserve its enemies..."); */
		return;
	}
	if (mons[corpse->corpsenm].cnutrit == 0) {
		pline("Das ist zu dürftig um eingemacht zu werden."); /* EN pline("That's too insubstantial to tin."); */
		return;
	}
	consume_obj_charge(obj, TRUE);

	if ((can = mksobj(TIN, FALSE, FALSE)) != 0) {
	    static const char you_buy_it[] = "Eingedost, eingekauft!"; /* EN static const char you_buy_it[] = "You tin it, you bought it!"; */

	    can->corpsenm = corpse->corpsenm;
	    can->cursed = obj->cursed;
	    can->blessed = obj->blessed;
	    can->owt = weight(can);
	    can->known = 1;
	    can->spe = -1;  /* Mark tinned tins. No spinach allowed... */
	    if (carried(corpse)) {
		if (corpse->unpaid)
		    verbalize(you_buy_it);
		useup(corpse);
	    } else {
		if (costly_spot(corpse->ox, corpse->oy) && !corpse->no_charge)
		    verbalize(you_buy_it);
		useupf(corpse, 1L);
	    }
	    can = hold_another_object(can, "You make, but cannot pick up, %s.", /* EN can = hold_another_object(can, "You make, but cannot pick up, %s.", */ // TODO DE
				      doname(can), (const char *)0);
	} else impossible("Tinning failed.");
}

void
use_unicorn_horn(obj)
struct obj *obj;
{
#define PROP_COUNT 6		/* number of properties we're dealing with */
#define ATTR_COUNT (A_MAX*3)	/* number of attribute points we might fix */
	int idx, val, val_limit,
	    trouble_count, unfixable_trbl, did_prop, did_attr;
	int trouble_list[PROP_COUNT + ATTR_COUNT];

	if (obj && obj->cursed) {
	    long lcount = (long) rnd(100);

	    switch (rn2(6)) {
	    case 0: make_sick(Sick ? Sick/3L + 1L : (long)rn1(ACURR(A_CON),20),
			xname(obj), TRUE, SICK_NONVOMITABLE);
		    break;
	    case 1: make_blinded(Blinded + lcount, TRUE);
		    break;
	    case 2: if (!Confusion)
			Du_fuehlst_dich("plötzlich %s.", /* EN You("suddenly feel %s.", */
			    Hallucination ? "trippig" : "verwirrt"); /* EN Hallucination ? "trippy" : "confused"); */
		    make_confused(HConfusion + lcount, TRUE);
		    break;
	    case 3: make_stunned(HStun + lcount, TRUE);
		    break;
	    case 4: (void) adjattrib(rn2(A_MAX), -1, FALSE);
		    break;
	    case 5: (void) make_hallucinated(HHallucination + lcount, TRUE, 0L);
		    break;
	    }
	    return;
	}

/*
 * Entries in the trouble list use a very simple encoding scheme.
 */
#define prop2trbl(X)	((X) + A_MAX)
#define attr2trbl(Y)	(Y)
#define prop_trouble(X) trouble_list[trouble_count++] = prop2trbl(X)
#define attr_trouble(Y) trouble_list[trouble_count++] = attr2trbl(Y)

	trouble_count = unfixable_trbl = did_prop = did_attr = 0;

	/* collect property troubles */
	if (Sick) prop_trouble(SICK);
	if (Blinded > (long)u.ucreamed) prop_trouble(BLINDED);
	if (HHallucination) prop_trouble(HALLUC);
	if (Vomiting) prop_trouble(VOMITING);
	if (HConfusion) prop_trouble(CONFUSION);
	if (HStun) prop_trouble(STUNNED);

	unfixable_trbl = unfixable_trouble_count(TRUE);

	/* collect attribute troubles */
	for (idx = 0; idx < A_MAX; idx++) {
	    val_limit = AMAX(idx);
	    /* don't recover strength lost from hunger */
	    if (idx == A_STR && u.uhs >= WEAK) val_limit--;
	    /* don't recover more than 3 points worth of any attribute */
	    if (val_limit > ABASE(idx) + 3) val_limit = ABASE(idx) + 3;

	    for (val = ABASE(idx); val < val_limit; val++)
		attr_trouble(idx);
	    /* keep track of unfixed trouble, for message adjustment below */
	    unfixable_trbl += (AMAX(idx) - val_limit);
	}

	if (trouble_count == 0) {
	    pline(nothing_happens);
	    return;
	} else if (trouble_count > 1) {		/* shuffle */
	    int i, j, k;

	    for (i = trouble_count - 1; i > 0; i--)
		if ((j = rn2(i + 1)) != i) {
		    k = trouble_list[j];
		    trouble_list[j] = trouble_list[i];
		    trouble_list[i] = k;
		}
	}

	/*
	 *		Chances for number of troubles to be fixed
	 *		 0	1      2      3      4	    5	   6	  7
	 *   blessed:  22.7%  22.7%  19.5%  15.4%  10.7%   5.7%   2.6%	 0.8%
	 *  uncursed:  35.4%  35.4%  22.9%   6.3%    0	    0	   0	  0
	 */
	val_limit = rn2( d(2, (obj && obj->blessed) ? 4 : 2) );
	if (val_limit > trouble_count) val_limit = trouble_count;

	/* fix [some of] the troubles */
	for (val = 0; val < val_limit; val++) {
	    idx = trouble_list[val];

	    switch (idx) {
	    case prop2trbl(SICK):
		make_sick(0L, (char *) 0, TRUE, SICK_ALL);
		did_prop++;
		break;
	    case prop2trbl(BLINDED):
		make_blinded((long)u.ucreamed, TRUE);
		did_prop++;
		break;
	    case prop2trbl(HALLUC):
		(void) make_hallucinated(0L, TRUE, 0L);
		did_prop++;
		break;
	    case prop2trbl(VOMITING):
		make_vomiting(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(CONFUSION):
		make_confused(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(STUNNED):
		make_stunned(0L, TRUE);
		did_prop++;
		break;
	    default:
		if (idx >= 0 && idx < A_MAX) {
		    ABASE(idx) += 1;
		    did_attr++;
		} else
		    panic("use_unicorn_horn: bad trouble? (%d)", idx);
		break;
	    }
	}

	if (did_attr)
	    pline("%s %s!", this_makes_you_feel, /* EN pline("This makes you feel %s!", */
		  (did_prop + did_attr) == (trouble_count + unfixable_trbl) ?
		  "großartig" : "besser"); /* EN "great" : "better"); */
	else if (!did_prop)
	    pline("Es scheint nichts zu passieren."); /* EN pline("Nothing seems to happen."); */

	flags.botl = (did_attr || did_prop);
#undef PROP_COUNT
#undef ATTR_COUNT
#undef prop2trbl
#undef attr2trbl
#undef prop_trouble
#undef attr_trouble
}

/*
 * Timer callback routine: turn figurine into monster
 */
void
fig_transform(arg, timeout)
genericptr_t arg;
long timeout;
{
	struct obj *figurine = (struct obj *)arg;
	struct monst *mtmp;
	coord cc;
	boolean cansee_spot, silent, okay_spot;
	boolean redraw = FALSE;
	char monnambuf[BUFSZ], carriedby[BUFSZ];

	if (!figurine) {
#ifdef DEBUG
	    pline("null figurine in fig_transform()");
#endif
	    return;
	}
	silent = (timeout != monstermoves); /* happened while away */
	okay_spot = get_obj_location(figurine, &cc.x, &cc.y, 0);
	if (figurine->where == OBJ_INVENT ||
	    figurine->where == OBJ_MINVENT)
		okay_spot = enexto(&cc, cc.x, cc.y,
				   &mons[figurine->corpsenm]);
	if (!okay_spot ||
	    !figurine_location_checks(figurine,&cc, TRUE)) {
		/* reset the timer to try again later */
		(void) start_timer((long)rnd(5000), TIMER_OBJECT,
				FIG_TRANSFORM, (genericptr_t)figurine);
		return;
	}

	cansee_spot = cansee(cc.x, cc.y);
	mtmp = make_familiar(figurine, cc.x, cc.y, TRUE);
	if (mtmp) {
	    Sprintf(monnambuf, "%s",an(m_monnam(mtmp)));
	    switch (figurine->where) {
		case OBJ_INVENT:
		    if (Blind)
			You("VERB_SPUEREN, NEUER_SATZ wie SUBJECT_IM_SATZ %s OBJECT KASUS_DATIV aus PRONOMEN_POSSESSIV NOUN_PACK %s!", something, /* EN You_feel("%s %s from your pack!", something, */
			    locomotion(mtmp->data,"VERB_FALLEN")); /* EN locomotion(mtmp->data,"drop")); */
		    else
			You("VERB_SEHEN OBJECT %s OBJECT KASUS_DATIV aus PRONOMEN_POSSESSIV NOUN_PACK %s!", /* EN You("see %s %s out of your pack!", */
			    monnambuf,
			    locomotion(mtmp->data,"VERB_FALLEN")); /* EN locomotion(mtmp->data,"drop")); */
		    break;

		case OBJ_FLOOR:
		    if (cansee_spot && !silent) {
			You("VERB_SEHEN, NEUER_SATZ wie sich SUBJECT_IM_SATZ ARTIKEL_UNBESTIMMTER NOUN_FIGURINE plötzlich OBJECT in ARTIKEL_UNBESTIMMTER %s VERB_TRANSFORMIEREN!", /* EN You("suddenly see a figurine transform into %s!", */
				monnambuf);
			redraw = TRUE;	/* update figurine's map location */
		    }
		    break;

		case OBJ_MINVENT:
		    if (cansee_spot && !silent) {
			struct monst *mon;
			mon = figurine->ocarry;
			/* figurine carring monster might be invisible */
			if (canseemon(figurine->ocarry)) {
			    Sprintf(carriedby, "%s", /* EN Sprintf(carriedby, "%s pack", */
				     genitivattribut_zu_wort(a_monnam(mon), "NOUN_PACK")); /* EN s_suffix(a_monnam(mon))); */
			}
			else if (is_pool(mon->mx, mon->my))
			    Strcpy(carriedby, "kristallklarem Wasser"); /* EN Strcpy(carriedby, "empty water"); */
			else
			    Strcpy(carriedby, "der Luft"); /* EN Strcpy(carriedby, "thin air"); */
			You("VERB_SEHEN OBJECT %s aus %s %s!", monnambuf, /* EN You("see %s %s out of %s!", monnambuf, */
			    carriedby, locomotion(mtmp->data, "VERB_KOMMEN")); /* EN locomotion(mtmp->data, "drop"), carriedby); */
		    }
		    break;
#if 0
		case OBJ_MIGRATING:
		    break;
#endif

		default:
		    impossible("figurine came to life where? (%d)",
				(int)figurine->where);
		break;
	    }
	}
	/* free figurine now */
	obj_extract_self(figurine);
	obfree(figurine, (struct obj *)0);
	if (redraw) newsym(cc.x, cc.y);
}

STATIC_OVL boolean
figurine_location_checks(obj, cc, quietly)
struct obj *obj;
coord *cc;
boolean quietly;
{
	xchar x,y;

	if (carried(obj) && u.uswallow) {
		if (!quietly)
			pline("Hier drinnen gibt es zu wenig Platz."); /* EN You("don't have enough room in here."); */
		return FALSE;
	}
	x = cc->x; y = cc->y;
	if (!isok(x,y)) {
		if (!quietly)
			pline("Dorthin VERB_KOENNEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT ARTIKEL_BESTIMMTER NOUN_FIGURINE stellen."); /* EN You("cannot put the figurine there."); */
		return FALSE;
	}
	if (IS_ROCK(levl[x][y].typ) &&
	    !(passes_walls(&mons[obj->corpsenm]) && may_passwall(x,y))) {
		if (!quietly)
		    You("VERB_KOENNEN OBJECT PRONOMEN_KEIN NOUN_FIGURINE %s stellen!", /* EN You("cannot place a figurine in %s!", */
			IS_TREE(levl[x][y].typ) ? "auf einen Baum stellen" : "in Felsgestein"); /* EN IS_TREE(levl[x][y].typ) ? "a tree" : "solid rock"); */
		return FALSE;
	}
	if (sobj_at(BOULDER,x,y) && !passes_walls(&mons[obj->corpsenm])
			&& !throws_rocks(&mons[obj->corpsenm])) {
		if (!quietly)
			You("VERB_KOENNEN OBJECT ARTIKEL_BESTIMMTER NOUN_FIGURINE auf den Felsbrocken stellen."); /* EN You("cannot fit the figurine on the boulder."); */
		return FALSE;
	}
	return TRUE;
}

STATIC_OVL void
use_figurine(optr)
struct obj **optr;
{
	register struct obj *obj = *optr;
	xchar x, y;
	coord cc;

	if (u.uswallow) {
		/* can't activate a figurine while swallowed */
		if (!figurine_location_checks(obj, (coord *)0, FALSE))
			return;
	}
	if(!getdir((char *)0)) {
		flags.move = multi = 0;
		return;
	}
	x = u.ux + u.dx; y = u.uy + u.dy;
	cc.x = x; cc.y = y;
	/* Passing FALSE arg here will result in messages displayed */
	if (!figurine_location_checks(obj, &cc, FALSE)) return;
	You("%s und sie transformiert.", /* EN You("%s and it transforms.", */
	    (u.dx||u.dy) ? "VERB_STELLEN die Statuette OBJECT neben PRONOMEN_PERSONAL" : /* EN (u.dx||u.dy) ? "set the figurine beside you" : */
	    (Is_airlevel(&u.uz) || Is_waterlevel(&u.uz) ||
	     is_pool(cc.x, cc.y)) ?
		"VERB_LASSEN die Statuette los" : /* EN "release the figurine" : */
	    (u.dz < 0 ?
		"VERB_WERFEN die Statuette in die Luft" : /* EN "toss the figurine into the air" : */
		"VERB_STELLEN die Statuette auf den Boden")); /* EN "set the figurine on the ground")); */
	(void) make_familiar(obj, cc.x, cc.y, FALSE);
	(void) stop_timer(FIG_TRANSFORM, (genericptr_t)obj);
	useup(obj);
	*optr = 0;
}

static NEARDATA const char lubricables[] = { ALL_CLASSES, ALLOW_NONE, 0 };
static NEARDATA const char need_to_remove_outer_armor[] =
			"VERB_MUESSEN OBJECT PRONOMEN_POSSESSIV %s ausziehen um OBJECT PRONOMEN_POSSESSIV %s einzufetten."; /* EN "need to remove your %s to grease your %s."; */

STATIC_OVL void
use_grease(obj)
struct obj *obj;
{
	struct obj *otmp;
	char buf[BUFSZ];

	if (Glib) {
	    pline("%s from your %s.", Tobjnam(obj, "slip"), /* EN pline("%s from your %s.", Tobjnam(obj, "slip"), */ // TODO DE
		  makeplural(body_part(FINGER)));
	    dropx(obj);
	    return;
	}

	if (obj->spe > 0) {
		if ((obj->cursed || Fumbling) && !rn2(2)) {
			consume_obj_charge(obj, TRUE);

			pline("%s from your %s.", Tobjnam(obj, "slip"), /* EN pline("%s from your %s.", Tobjnam(obj, "slip"), */ // TODO DE
			      makeplural(body_part(FINGER)));
			dropx(obj);
			return;
		}
		otmp = getobj(lubricables, "fetten"); /* EN otmp = getobj(lubricables, "grease"); */
		if (!otmp) return;
		if ((otmp->owornmask & WORN_ARMOR) && uarmc) {
			Strcpy(buf, xname(uarmc));
			You(need_to_remove_outer_armor, buf, xname(otmp));
			return;
		}
#ifdef TOURIST
		if ((otmp->owornmask & WORN_SHIRT) && (uarmc || uarm)) {
			Strcpy(buf, uarmc ? xname(uarmc) : "");
			if (uarmc && uarm) Strcat(buf, " und "); /* EN if (uarmc && uarm) Strcat(buf, " and "); */
			Strcat(buf, uarm ? xname(uarm) : "");
			You(need_to_remove_outer_armor, buf, xname(otmp));
			return;
		}
#endif
		consume_obj_charge(obj, TRUE);

		if (otmp != &zeroobj) {
			You("VERB_UEBERZIEHEN OBJECT %s mit einer dicken Schicht Fett.", /* EN You("cover %s with a thick layer of grease.", */
			    yname(otmp));
			otmp->greased = 1;
			if (obj->cursed && !nohands(youmonst.data)) {
			    incr_itimeout(&Glib, rnd(15));
			    pline("Some of the grease gets all over your %s.", /* EN pline("Some of the grease gets all over your %s.", */ // TODO DE
				makeplural(body_part(HAND)));
			}
		} else {
			Glib += rnd(15);
			You("coat your %s with grease.", /* EN You("coat your %s with grease.", */ // TODO DE
			    makeplural(body_part(FINGER)));
		}
	} else {
	    if (obj->known)
		pline("SUBJECT %s leer.", Tobjnam(obj, "VERB_SEIN")); /* EN pline("%s empty.", Tobjnam(obj, "are")); */
	    else
		pline("SUBJECT %s leer zu sein.", Tobjnam(obj, "VERB_SCHEINEN")); /* EN pline("%s to be empty.", Tobjnam(obj, "seem")); */
	}
	update_inventory();
}

static struct trapinfo {
	struct obj *tobj;
	xchar tx, ty;
	int time_needed;
	boolean force_bungle;
} trapinfo;

void
reset_trapset()
{
	trapinfo.tobj = 0;
	trapinfo.force_bungle = 0;
}

/* touchstones - by Ken Arnold */
STATIC_OVL void
use_stone(tstone)
struct obj *tstone;
{
    struct obj *obj;
    boolean do_scratch;
    const char *streak_color, *choices;
    char stonebuf[QBUFSZ];
    static const char scritch[] = "\"kratz, kratz\""; /* EN static const char scritch[] = "\"scritch, scritch\""; */
    static const char allowall[3] = { COIN_CLASS, ALL_CLASSES, 0 };
    static const char justgems[3] = { ALLOW_NONE, GEM_CLASS, 0 };
#ifndef GOLDOBJ
    struct obj goldobj;
#endif

    /* in case it was acquired while blinded */
    if (!Blind) tstone->dknown = 1;
    /* when the touchstone is fully known, don't bother listing extra
       junk as likely candidates for rubbing */
    choices = (tstone->otyp == TOUCHSTONE && tstone->dknown &&
		objects[TOUCHSTONE].oc_name_known) ? justgems : allowall;
    Sprintf(stonebuf, "rub on the stone%s", plur(tstone->quan)); /* EN Sprintf(stonebuf, "rub on the stone%s", plur(tstone->quan)); */ // TODO DE
    if ((obj = getobj(choices, stonebuf)) == 0)
	return;
#ifndef GOLDOBJ
    if (obj->oclass == COIN_CLASS) {
	u.ugold += obj->quan;	/* keep botl up to date */
	goldobj = *obj;
	dealloc_obj(obj);
	obj = &goldobj;
    }
#endif

    if (obj == tstone && obj->quan == 1) {
	You_cant("rub %s on itself.", the(xname(obj))); /* EN You_cant("rub %s on itself.", the(xname(obj))); */ // TODO DE
	return;
    }

    if (tstone->otyp == TOUCHSTONE && tstone->cursed &&
	    obj->oclass == GEM_CLASS && !is_graystone(obj) &&
	    !obj_resists(obj, 80, 100)) {
	if (Blind)
	    Du_spuerst("etwas zersplittern."); /* EN pline("You feel something shatter."); */
	else if (Hallucination)
	    pline("Mann eh, schau dir die hübschen Scherben an."); /* EN pline("Oh, wow, look at the pretty shards."); */
	else
	    pline("A sharp crack shatters %s%s.", /* EN pline("A sharp crack shatters %s%s.", */ // TODO DE
		  (obj->quan > 1) ? "one of " : "", the(xname(obj))); /* EN (obj->quan > 1) ? "one of " : "", the(xname(obj))); */ // TODO DE
#ifndef GOLDOBJ
     /* assert(obj != &goldobj); */
#endif
	useup(obj);
	return;
    }

    if (Blind) {
	pline(scritch);
	return;
    } else if (Hallucination) {
	pline("Verschärft, Mann: Fraktale!"); /* EN pline("Oh wow, man: Fractals!"); */
	return;
    }

    do_scratch = FALSE;
    streak_color = 0;

    switch (obj->oclass) {
    case GEM_CLASS:	/* these have class-specific handling below */
    case RING_CLASS:
	if (tstone->otyp != TOUCHSTONE) {
	    do_scratch = TRUE;
	} else if (obj->oclass == GEM_CLASS && (tstone->blessed ||
		(!tstone->cursed &&
		    (Role_if(PM_ARCHEOLOGIST) || Race_if(PM_GNOME))))) {
	    makeknown(TOUCHSTONE);
	    makeknown(obj->otyp);
	    prinv((char *)0, obj, 0L);
	    return;
	} else {
	    /* either a ring or the touchstone was not effective */
	    if (objects[obj->otyp].oc_material == GLASS) {
		do_scratch = TRUE;
		break;
	    }
	}
	streak_color = c_obj_colors[objects[obj->otyp].oc_color];
	break;		/* gem or ring */

    default:
	switch (objects[obj->otyp].oc_material) {
	case CLOTH:
	    pline("SUBJECT %s jetzt ein bisschen besser poliert aus.", Tobjnam(tstone, "VERB_AUSSEHEN")); /* EN pline("%s a little more polished now.", Tobjnam(tstone, "look")); */
	    return;
	case LIQUID:
	    if (!obj->known)		/* note: not "whetstone" */
		pline("Achtung! Steter Tropfen höhlt den Stein!"); /* EN You("must think this is a wetstone, do you?"); */ /* Wortspiel */
	    else
		pline("SUBJECT %s jetzt ein wenig nasser.", Tobjnam(tstone, "VERB_SEIN")); /* EN pline("%s a little wetter now.", Tobjnam(tstone, "are")); */
	    return;
	case WAX:
	    streak_color = "ADJEKTIV_WAECHSERN"; /* EN streak_color = "waxy"; */
	    break;		/* okay even if not touchstone */
	case WOOD:
	    streak_color = "ADJEKTIV_HOELZERN"; /* EN streak_color = "wooden"; */
	    break;		/* okay even if not touchstone */
	case GOLD:
	    do_scratch = TRUE;	/* scratching and streaks */
	    streak_color = "ADJEKTIV_FARBE_GOLDEN"; /* EN streak_color = "golden"; */
	    break;
	case SILVER:
	    do_scratch = TRUE;	/* scratching and streaks */
	    streak_color = "ADJEKTIV_FARBE_SILVER"; /* EN streak_color = "silvery"; */
	    break;
	default:
	    /* Objects passing the is_flimsy() test will not
	       scratch a stone.  They will leave streaks on
	       non-touchstones and touchstones alike. */
	    if (is_flimsy(obj))
		streak_color = c_obj_colors[objects[obj->otyp].oc_color];
	    else
		do_scratch = (tstone->otyp != TOUCHSTONE);
	    break;
	}
	break;		/* default oclass */
    }

    Sprintf(stonebuf, "NOUN_GEM_ROCK%s", plur(tstone->quan)); /* EN Sprintf(stonebuf, "stone%s", plur(tstone->quan)); */
    if (do_scratch)
	pline("SUBJECT %s VERB_HINTERLASSEN OBJECT %s%sNOUN_KRATZSPURs NEUES_OBJECT OBJECT KASUS_DATIV auf ARTIKEL_BESTIMMTER %s.", the(xname(obj)), /* EN pline("You make %s%sscratch marks on the %s.", */
	      streak_color ? streak_color : (const char *)"",
	      streak_color ? " " : "", stonebuf);
    else if (streak_color)
	You("VERB_SEHEN OBJECT %s NOUN_STRICHs NEUES_OBJECT OBJECT KASUS_DATIV auf ARTIKEL_BESTIMMTER %s.", streak_color, stonebuf); /* EN pline("You see %s streaks on the %s.", streak_color, stonebuf); */
    else
	pline(scritch);
    return;
}

/* Place a landmine/bear trap.  Helge Hafting */
STATIC_OVL void
use_trap(otmp)
struct obj *otmp;
{
	int ttyp, tmp;
	const char *what = (char *)0;
	char buf[BUFSZ];
	const char *occutext = "setting the trap"; /* EN const char *occutext = "setting the trap"; */ // TODO DE

	if (nohands(youmonst.data))
	    what = "ohne Hände"; /* EN what = "without hands"; */
	else if (Stunned)
	    what = "benommen"; /* EN what = "while stunned"; */
	else if (u.uswallow)
	    what = is_animal(u.ustuck->data) ? "while swallowed" : /* EN what = is_animal(u.ustuck->data) ? "while swallowed" : */ // TODO DE
			"while engulfed"; /* EN "while engulfed"; */ // TODO DE
	else if (Underwater)
	    what = "unter Wasser"; /* EN what = "underwater"; */
	else if (Levitation)
	    what = "schwebend"; /* EN what = "while levitating"; */
	else if (is_pool(u.ux, u.uy))
	    what = "im Wasser"; /* EN what = "in water"; */
	else if (is_lava(u.ux, u.uy))
	    what = "in der Lava"; /* EN what = "in lava"; */
	else if (On_stairs(u.ux, u.uy))
	    what = (u.ux == xdnladder || u.ux == xupladder) ?
			"auf einer Leiter" : "auf einer Treppe"; /* EN "on the ladder" : "on the stairs"; */
	else if (IS_FURNITURE(levl[u.ux][u.uy].typ) ||
		IS_ROCK(levl[u.ux][u.uy].typ) ||
		closed_door(u.ux, u.uy) || t_at(u.ux, u.uy))
	    what = "hier"; /* EN what = "here"; */
	if (what) {
	    You("VERB_KOENNEN %s keine Falle aufstellen!",what); /* EN You_cant("set a trap %s!",what); */
	    reset_trapset();
	    return;
	}
	ttyp = (otmp->otyp == LAND_MINE) ? LANDMINE : BEAR_TRAP;
	if (otmp == trapinfo.tobj &&
		u.ux == trapinfo.tx && u.uy == trapinfo.ty) {
	    You("resume setting %s %s.", /* EN You("resume setting %s %s.", */ // TODO DE
		shk_your(buf, otmp),
		defsyms[trap_to_defsym(what_trap(ttyp))].explanation);
	    set_occupation(set_trap, occutext, 0);
	    return;
	}
	trapinfo.tobj = otmp;
	trapinfo.tx = u.ux,  trapinfo.ty = u.uy;
	tmp = ACURR(A_DEX);
	trapinfo.time_needed = (tmp > 17) ? 2 : (tmp > 12) ? 3 :
				(tmp > 7) ? 4 : 5;
	if (Blind) trapinfo.time_needed *= 2;
	tmp = ACURR(A_STR);
	if (ttyp == BEAR_TRAP && tmp < 18)
	    trapinfo.time_needed += (tmp > 12) ? 1 : (tmp > 7) ? 2 : 4;
	/*[fumbling and/or confusion and/or cursed object check(s)
	   should be incorporated here instead of in set_trap]*/
#ifdef STEED
	if (u.usteed && P_SKILL(P_RIDING) < P_BASIC) {
	    boolean chance;

	    if (Fumbling || otmp->cursed) chance = (rnl(10) > 3);
	    else  chance = (rnl(10) > 5);
	    You("aren't very skilled at reaching from %s.", /* EN You("aren't very skilled at reaching from %s.", */ // TODO DE
		mon_nam(u.usteed));
	    Sprintf(buf, "Continue your attempt to set %s?", /* EN Sprintf(buf, "Continue your attempt to set %s?", */ // TODO DE
		the(defsyms[trap_to_defsym(what_trap(ttyp))].explanation));
	    if(yn(buf) == 'y') {
		if (chance) {
			switch(ttyp) {
			    case LANDMINE:	/* set it off */
			    	trapinfo.time_needed = 0;
			    	trapinfo.force_bungle = TRUE;
				break;
			    case BEAR_TRAP:	/* drop it without arming it */
				reset_trapset();
				You("VERB_DROP OBJECT %s SATZKLAMMER!", /* EN You("drop %s!", */
			  the(defsyms[trap_to_defsym(what_trap(ttyp))].explanation));
				dropx(otmp);
				return;
			}
		}
	    } else {
	    	reset_trapset();
		return;
	    }
	}
#endif
	You("begin setting %s %s.", /* EN You("begin setting %s %s.", */ // TODO DE
	    shk_your(buf, otmp),
	    defsyms[trap_to_defsym(what_trap(ttyp))].explanation);
	set_occupation(set_trap, occutext, 0);
	return;
}

STATIC_PTR
int
set_trap()
{
	struct obj *otmp = trapinfo.tobj;
	struct trap *ttmp;
	int ttyp;

	if (!otmp || !carried(otmp) ||
		u.ux != trapinfo.tx || u.uy != trapinfo.ty) {
	    /* ?? */
	    reset_trapset();
	    return 0;
	}

	if (--trapinfo.time_needed > 0) return 1;	/* still busy */

	ttyp = (otmp->otyp == LAND_MINE) ? LANDMINE : BEAR_TRAP;
	ttmp = maketrap(u.ux, u.uy, ttyp);
	if (ttmp) {
	    ttmp->tseen = 1;
	    ttmp->madeby_u = 1;
	    newsym(u.ux, u.uy); /* if our hero happens to be invisible */
	    if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
		add_damage(u.ux, u.uy, 0L);		/* schedule removal */
	    }
	    if (!trapinfo.force_bungle)
		You("finish arming %s.", /* EN You("finish arming %s.", */ // TODO DE
			the(defsyms[trap_to_defsym(what_trap(ttyp))].explanation));
	    if (((otmp->cursed || Fumbling) && (rnl(10) > 5)) || trapinfo.force_bungle)
		dotrap(ttmp,
			(unsigned)(trapinfo.force_bungle ? FORCEBUNGLE : 0));
	} else {
	    /* this shouldn't happen */
	    Your("trap setting attempt fails."); /* EN Your("trap setting attempt fails."); */ // TODO DE
	}
	useup(otmp);
	reset_trapset();
	return 0;
}

STATIC_OVL int
use_whip(obj)
struct obj *obj;
{
    char buf[BUFSZ];
    struct monst *mtmp;
    struct obj *otmp;
    int rx, ry, proficient, res = 0;
    const char *msg_slipsfree = "The bullwhip slips free."; /* EN const char *msg_slipsfree = "The bullwhip slips free."; */ // TODO DE
    const char *msg_snap = "Snap!"; /* EN const char *msg_snap = "Snap!"; */ // TODO DE

    if (obj != uwep) {
	if (!wield_tool(obj, "lash")) return 0; /* EN if (!wield_tool(obj, "lash")) return 0; */ // TODO DE
	else res = 1;
    }
    if (!getdir((char *)0)) return res;

    if (Stunned || (Confusion && !rn2(5))) confdir();
    rx = u.ux + u.dx;
    ry = u.uy + u.dy;
    mtmp = m_at(rx, ry);

    /* fake some proficiency checks */
    proficient = 0;
    if (Role_if(PM_ARCHEOLOGIST)) ++proficient;
    if (ACURR(A_DEX) < 6) proficient--;
    else if (ACURR(A_DEX) >= 14) proficient += (ACURR(A_DEX) - 14);
    if (Fumbling) --proficient;
    if (proficient > 3) proficient = 3;
    if (proficient < 0) proficient = 0;

    if (u.uswallow && attack(u.ustuck)) {
	There("is not enough room to flick your bullwhip."); /* EN There("is not enough room to flick your bullwhip."); */ // TODO DE

    } else if (Underwater) {
	There("is too much resistance to flick your bullwhip."); /* EN There("is too much resistance to flick your bullwhip."); */ // TODO DE

    } else if (u.dz < 0) {
	You("flick a bug off of the %s.",ceiling(u.ux,u.uy)); /* EN You("flick a bug off of the %s.",ceiling(u.ux,u.uy)); */ // TODO DE

    } else if ((!u.dx && !u.dy) || (u.dz > 0)) {
	int dam;

#ifdef STEED
	/* Sometimes you hit your steed by mistake */
	if (u.usteed && !rn2(proficient + 2)) {
	    You("whip %s!", mon_nam(u.usteed)); /* EN You("whip %s!", mon_nam(u.usteed)); */ // TODO DE
	    kick_steed();
	    return 1;
	}
#endif
	if (Levitation
#ifdef STEED
			|| u.usteed
#endif
		) {
	    /* Have a shot at snaring something on the floor */
	    otmp = level.objects[u.ux][u.uy];
	    if (otmp && otmp->otyp == CORPSE && otmp->corpsenm == PM_HORSE) {
		pline("SUBJECT PRONOMEN_PERSONAL MODIFIER_KONJUNKTIV_II VERB_KOENNEN genausogut darauf herumreiten."); /* EN pline("Why beat a dead horse?"); */
		return 1;
	    }
	    if (otmp && proficient) {
		You("wrap your bullwhip around %s on the %s.", /* EN You("wrap your bullwhip around %s on the %s.", */ // TODO DE
		    an(singular(otmp, xname)), surface(u.ux, u.uy));
		if (rnl(6) || pickup_object(otmp, 1L, TRUE) < 1)
		    pline(msg_slipsfree);
		return 1;
	    }
	}
	dam = rnd(2) + dbon() + obj->spe;
	if (dam <= 0) dam = 1;
	You("hit your %s with your bullwhip.", body_part(FOOT)); /* EN You("hit your %s with your bullwhip.", body_part(FOOT)); */ // TODO DE
	Sprintf(buf, "killed %sself with %s bullwhip", uhim(), uhis()); /* EN Sprintf(buf, "killed %sself with %s bullwhip", uhim(), uhis()); */ // TODO DE
	losehp(dam, buf, NO_KILLER_PREFIX);
	flags.botl = 1;
	return 1;

    } else if ((Fumbling || Glib) && !rn2(5)) {
	pline_The("bullwhip slips out of your %s.", body_part(HAND)); /* EN pline_The("bullwhip slips out of your %s.", body_part(HAND)); */ // TODO DE
	dropx(obj);

    } else if (u.utrap && u.utraptype == TT_PIT) {
	/*
	 *     Assumptions:
	 *
	 *	if you're in a pit
	 *		- you are attempting to get out of the pit
	 *		- or, if you are applying it towards a small
	 *		  monster then it is assumed that you are
	 *		  trying to hit it.
	 *	else if the monster is wielding a weapon
	 *		- you are attempting to disarm a monster
	 *	else
	 *		- you are attempting to hit the monster
	 *
	 *	if you're confused (and thus off the mark)
	 *		- you only end up hitting.
	 *
	 */
	const char *wrapped_what = (char *)0;

	if (mtmp) {
	    if (bigmonst(mtmp->data)) {
		wrapped_what = strcpy(buf, mon_nam(mtmp));
	    } else if (proficient) {
		if (attack(mtmp)) return 1;
		else pline(msg_snap);
	    }
	}
	if (!wrapped_what) {
	    if (IS_FURNITURE(levl[rx][ry].typ))
		wrapped_what = something;
	    else if (sobj_at(BOULDER, rx, ry))
		wrapped_what = "ARTIKEL_UNBESTIMMTER NOUN_BOULDER"; /* EN wrapped_what = "a boulder"; */
	}
	if (wrapped_what) {
	    coord cc;

	    cc.x = rx; cc.y = ry;
	    You("wrap your bullwhip around %s.", wrapped_what); /* EN You("wrap your bullwhip around %s.", wrapped_what); */ // TODO DE
	    if (proficient && rn2(proficient + 2)) {
		if (!mtmp || enexto(&cc, rx, ry, youmonst.data)) {
		    You("yank yourself out of the pit!"); /* EN You("yank yourself out of the pit!"); */ // TODO DE
		    teleds(cc.x, cc.y, TRUE);
		    u.utrap = 0;
		    vision_full_recalc = 1;
		}
	    } else {
		pline(msg_slipsfree);
	    }
	    if (mtmp) wakeup(mtmp);
	} else pline(msg_snap);

    } else if (mtmp) {
	if (!canspotmon(mtmp) &&
		!glyph_is_invisible(levl[rx][ry].glyph)) {
	   pline("Ein Monster ist dort, das SUBJECT_IM_SATZ PRONOMEN_PERSONAL nicht sehen VERB_KOENNEN."); /* EN pline("A monster is there that you couldn't see."); */
	   map_invisible(rx, ry);
	}
	otmp = MON_WEP(mtmp);	/* can be null */
	if (otmp) {
	    char onambuf[BUFSZ];
	    const char *mon_hand;
	    boolean gotit = proficient && (!Fumbling || !rn2(10));

	    Strcpy(onambuf, cxname(otmp));
	    if (gotit) {
		mon_hand = mbodypart(mtmp, HAND);
		if (bimanual(otmp)) mon_hand = makeplural(mon_hand);
	    } else
		mon_hand = 0;	/* lint suppression */

	    You("wrap your bullwhip around %s.", /* EN You("wrap your bullwhip around %s %s.", */ // TODO DE
		genitivattribut_zu_wort(mon_nam(mtmp), onambuf)); /* EN s_suffix(mon_nam(mtmp)), onambuf); */
	    if (gotit && otmp->cursed) {
		pline("%s welded to %s %s%c", /* EN pline("%s welded to %s %s%c", */ // TODO DE
		      (otmp->quan == 1L) ? "It is" : "They are", /* EN (otmp->quan == 1L) ? "It is" : "They are", */ // TODO DE
		      mhis(mtmp), mon_hand,
		      !otmp->bknown ? '!' : '.');
		otmp->bknown = 1;
		gotit = FALSE;	/* can't pull it free */
	    }
	    if (gotit) {
		obj_extract_self(otmp);
		possibly_unwield(mtmp, FALSE);
		setmnotwielded(mtmp,otmp);

		switch (rn2(proficient + 1)) {
		case 2:
		    /* to floor near you */
		    You("yank %s to the %s!", genitivattribut_zu_wort(mon_nam(mtmp), onambuf), /* EN You("yank %s %s to the %s!", s_suffix(mon_nam(mtmp)), */ // TODO DE
			surface(u.ux, u.uy)); /* EN onambuf, surface(u.ux, u.uy)); */
		    place_object(otmp, u.ux, u.uy);
		    stackobj(otmp);
		    break;
		case 3:
		    /* right to you */
#if 0
		    if (!rn2(25)) {
			/* proficient with whip, but maybe not
			   so proficient at catching weapons */
			int hitu, hitvalu;

			hitvalu = 8 + otmp->spe;
			hitu = thitu(hitvalu,
				     dmgval(otmp, &youmonst),
				     otmp, (char *)0);
			if (hitu) {
			    pline_The("%s hits you as you try to snatch it!",
				the(onambuf));
			}
			place_object(otmp, u.ux, u.uy);
			stackobj(otmp);
			break;
		    }
#endif /* 0 */
		    /* right into your inventory */
		    You("VERB_SCHNAPPEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL NEUES_OBJECT OBJECT %s!", genitivattribut_zu_wort(mon_nam(mtmp), onambuf)); /* EN You("snatch %s %s!", s_suffix(mon_nam(mtmp)), onambuf); */
		    if (otmp->otyp == CORPSE &&
			    touch_petrifies(&mons[otmp->corpsenm]) &&
			    !uarmg && !Stone_resistance &&
			    !(poly_when_stoned(youmonst.data) &&
				polymon(PM_STONE_GOLEM))) {
			char kbuf[BUFSZ];

			Sprintf(kbuf, "%s corpse", /* EN Sprintf(kbuf, "%s corpse", */ // TODO DE
				an(mons[otmp->corpsenm].mname));
			pline("Snatching %s is a fatal mistake.", kbuf); /* EN pline("Snatching %s is a fatal mistake.", kbuf); */ // TODO DE
			instapetrify(kbuf);
		    }
		    otmp = hold_another_object(otmp, "SUBJECT VERB_DROP %s SATZKLAMMER!", /* EN otmp = hold_another_object(otmp, "You drop %s!", */
					       doname(otmp), (const char *)0);
		    break;
		default:
		    /* to floor beneath mon */
		    You("yank %s from %s!", the(onambuf), /* EN You("yank %s from %s %s!", the(onambuf), */ // TODO DE
			genitivattribut_zu_wort(mon_nam(mtmp), mon_hand)); /* EN s_suffix(mon_nam(mtmp)), mon_hand); */
		    obj_no_longer_held(otmp);
		    place_object(otmp, mtmp->mx, mtmp->my);
		    stackobj(otmp);
		    break;
		}
	    } else {
		pline(msg_slipsfree);
	    }
	    wakeup(mtmp);
	} else {
	    if (mtmp->m_ap_type &&
		!Protection_from_shape_changers && !sensemon(mtmp))
		stumble_onto_mimic(mtmp);
	    else You("flick your bullwhip towards %s.", mon_nam(mtmp)); /* EN else You("flick your bullwhip towards %s.", mon_nam(mtmp)); */ // TODO DE
	    if (proficient) {
		if (attack(mtmp)) return 1;
		else pline(msg_snap);
	    }
	}

    } else if (Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)) {
	    /* it must be air -- water checked above */
	    You("snap your whip through thin air."); /* EN You("snap your whip through thin air."); */ // TODO DE

    } else {
	pline(msg_snap);

    }
    return 1;
}


static const char
	not_enough_room[] = "There's not enough room here to use that.", /* EN not_enough_room[] = "There's not enough room here to use that.", */ // TODO DE
	where_to_hit[] = "Where do you want to hit?", /* EN where_to_hit[] = "Where do you want to hit?", */ // TODO DE
	cant_see_spot[] = "won't hit anything if you can't see that spot.", /* EN cant_see_spot[] = "won't hit anything if you can't see that spot.", */ // TODO DE
	cant_reach[] = "can't reach that spot from here."; /* EN cant_reach[] = "can't reach that spot from here."; */ // TODO DE

/* Distance attacks by pole-weapons */
STATIC_OVL int
use_pole (obj)
	struct obj *obj;
{
	int res = 0, typ, max_range = 4, min_range = 4;
	coord cc;
	struct monst *mtmp;


	/* Are you allowed to use the pole? */
	if (u.uswallow) {
	    pline(not_enough_room);
	    return (0);
	}
	if (obj != uwep) {
	    if (!wield_tool(obj, "swing")) return(0); /* EN if (!wield_tool(obj, "swing")) return(0); */ // TODO DE
	    else res = 1;
	}
     /* assert(obj == uwep); */

	/* Prompt for a location */
	pline(where_to_hit);
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the spot to hit") < 0) /* EN if (getpos(&cc, TRUE, "the spot to hit") < 0) */ // TODO DE
	    return 0;	/* user pressed ESC */

	/* Calculate range */
	typ = uwep_skill_type();
	if (typ == P_NONE || P_SKILL(typ) <= P_BASIC) max_range = 4;
	else if (P_SKILL(typ) == P_SKILLED) max_range = 5;
	else max_range = 8;
	if (distu(cc.x, cc.y) > max_range) {
	    pline("Zu weit weg!"); /* EN pline("Too far!"); */
	    return (res);
	} else if (distu(cc.x, cc.y) < min_range) {
	    pline("Zu nah!"); /* EN pline("Too close!"); */
	    return (res);
	} else if (!cansee(cc.x, cc.y) &&
		   ((mtmp = m_at(cc.x, cc.y)) == (struct monst *)0 ||
		    !canseemon(mtmp))) {
	    You(cant_see_spot);
	    return (res);
	} else if (!couldsee(cc.x, cc.y)) { /* Eyes of the Overworld */
	    You(cant_reach);
	    return res;
	}

	/* Attack the monster there */
	if ((mtmp = m_at(cc.x, cc.y)) != (struct monst *)0) {
	    int oldhp = mtmp->mhp;

	    bhitpos = cc;
	    check_caitiff(mtmp);
	    (void) thitmonst(mtmp, uwep);
	    /* check the monster's HP because thitmonst() doesn't return
	     * an indication of whether it hit.  Not perfect (what if it's a
	     * non-silver weapon on a shade?)
	     */
	    if (mtmp->mhp < oldhp)
		u.uconduct.weaphit++;
	} else
	    /* Now you know that nothing is there... */
	    pline(nothing_happens);
	return (1);
}

STATIC_OVL int
use_cream_pie(obj)
struct obj *obj;
{
	boolean wasblind = Blind;
	boolean wascreamed = u.ucreamed;
	boolean several = FALSE;

	if (obj->quan > 1L) {
		several = TRUE;
		obj = splitobj(obj, 1L);
	}
	if (Hallucination)
		You("VERB_MACHEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL eine Gesichtspackung."); /* EN You("give yourself a facial."); */
	else
		pline("You immerse your %s in %s%s.", body_part(FACE), /* EN pline("You immerse your %s in %s%s.", body_part(FACE), */ // TODO DE
			several ? "one of " : "", /* EN several ? "one of " : "", */ // TODO DE
			several ? makeplural(the(xname(obj))) : the(xname(obj)));
	if(can_blnd((struct monst*)0, &youmonst, AT_WEAP, obj)) {
		int blindinc = rnd(25);
		u.ucreamed += blindinc;
		make_blinded(Blinded + (long)blindinc, FALSE);
		if (!Blind || (Blind && wasblind))
			You("VERB_HABEN %sklebrige Sahne OBJECT KASUS_DATIV in PRONOMEN_POSSESSIV %s.", /* EN pline("There's %ssticky goop all over your %s.", */
				wascreamed ? "mehr " : "", /* EN wascreamed ? "more " : "", */
				body_part(FACE));
		else /* Blind  && !wasblind */
			pline("Durch all die klebrige Sahne KASUS_DATIV in PRONOMEN_POSSESSIV %s SUBJECT_IM_SATZ VERB_KOENNEN PRONOMEN_PERSONAL nichts sehen.", /* EN You_cant("see through all the sticky goop on your %s.", */
				body_part(FACE));
	}
	if (obj->unpaid) {
		verbalize("Gebraucht, gekauft!"); /* EN verbalize("You used it, you bought it!"); */
		bill_dummy_object(obj);
	}
	obj_extract_self(obj);
	delobj(obj);
	return(0);
}

STATIC_OVL int
use_grapple (obj)
	struct obj *obj;
{
	int res = 0, typ, max_range = 4, tohit;
	coord cc;
	struct monst *mtmp;
	struct obj *otmp;

	/* Are you allowed to use the hook? */
	if (u.uswallow) {
	    pline(not_enough_room);
	    return (0);
	}
	if (obj != uwep) {
	    if (!wield_tool(obj, "cast")) return(0); /* EN if (!wield_tool(obj, "cast")) return(0); */ // TODO DE
	    else res = 1;
	}
     /* assert(obj == uwep); */

	/* Prompt for a location */
	pline(where_to_hit);
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the spot to hit") < 0) /* EN if (getpos(&cc, TRUE, "the spot to hit") < 0) */ // TODO DE
	    return 0;	/* user pressed ESC */

	/* Calculate range */
	typ = uwep_skill_type();
	if (typ == P_NONE || P_SKILL(typ) <= P_BASIC) max_range = 4;
	else if (P_SKILL(typ) == P_SKILLED) max_range = 5;
	else max_range = 8;
	if (distu(cc.x, cc.y) > max_range) {
	    pline("Zu weit!"); /* EN pline("Too far!"); */ // TODO DE
	    return (res);
	} else if (!cansee(cc.x, cc.y)) {
	    You(cant_see_spot);
	    return (res);
	} else if (!couldsee(cc.x, cc.y)) { /* Eyes of the Overworld */
	    You(cant_reach);
	    return res;
	}

	/* What do you want to hit? */
	tohit = rn2(5);
	if (typ != P_NONE && P_SKILL(typ) >= P_SKILLED) {
	    winid tmpwin = create_nhwindow(NHW_MENU);
	    anything any;
	    char buf[BUFSZ];
	    menu_item *selected;

	    any.a_void = 0;	/* set all bits to zero */
	    any.a_int = 1;	/* use index+1 (cant use 0) as identifier */
	    start_menu(tmpwin);
	    any.a_int++;
	    Sprintf(buf, "an object on the %s", surface(cc.x, cc.y)); /* EN Sprintf(buf, "an object on the %s", surface(cc.x, cc.y)); */ // TODO DE
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 buf, MENU_UNSELECTED);
	    any.a_int++;
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			"a monster", MENU_UNSELECTED); /* EN "a monster", MENU_UNSELECTED); */ // TODO DE
	    any.a_int++;
	    Sprintf(buf, "the %s", surface(cc.x, cc.y));
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 buf, MENU_UNSELECTED);
	    end_menu(tmpwin, "Auf was zielen?"); /* EN end_menu(tmpwin, "Aim for what?"); */
	    tohit = rn2(4);
	    if (select_menu(tmpwin, PICK_ONE, &selected) > 0 &&
			rn2(P_SKILL(typ) > P_SKILLED ? 20 : 2))
		tohit = selected[0].item.a_int - 1;
	    free((genericptr_t)selected);
	    destroy_nhwindow(tmpwin);
	}

	/* What did you hit? */
	switch (tohit) {
	case 0:	/* Trap */
	    /* FIXME -- untrap needs to deal with non-adjacent traps */
	    break;
	case 1:	/* Object */
	    if ((otmp = level.objects[cc.x][cc.y]) != 0) {
		You("snag an object from the %s!", surface(cc.x, cc.y)); /* EN You("snag an object from the %s!", surface(cc.x, cc.y)); */ // TODO DE
		(void) pickup_object(otmp, 1L, FALSE);
		/* If pickup fails, leave it alone */
		newsym(cc.x, cc.y);
		return (1);
	    }
	    break;
	case 2:	/* Monster */
	    if ((mtmp = m_at(cc.x, cc.y)) == (struct monst *)0) break;
	    if (verysmall(mtmp->data) && !rn2(4) &&
			enexto(&cc, u.ux, u.uy, (struct permonst *)0)) {
		You("pull in %s!", mon_nam(mtmp)); /* EN You("pull in %s!", mon_nam(mtmp)); */ // TODO DE
		mtmp->mundetected = 0;
		rloc_to(mtmp, cc.x, cc.y);
		return (1);
	    } else if ((!bigmonst(mtmp->data) && !strongmonst(mtmp->data)) ||
		       rn2(4)) {
		(void) thitmonst(mtmp, uwep);
		return (1);
	    }
	    /* FALL THROUGH */
	case 3:	/* Surface */
	    if (IS_AIR(levl[cc.x][cc.y].typ) || is_pool(cc.x, cc.y))
		pline_The("hook slices through the %s.", surface(cc.x, cc.y)); /* EN pline_The("hook slices through the %s.", surface(cc.x, cc.y)); */ // TODO DE
	    else {
		You("are yanked toward the %s!", surface(cc.x, cc.y)); /* EN You("are yanked toward the %s!", surface(cc.x, cc.y)); */ // TODO DE
		hurtle(sgn(cc.x-u.ux), sgn(cc.y-u.uy), 1, FALSE);
		spoteffects(TRUE);
	    }
	    return (1);
	default:	/* Yourself (oops!) */
	    if (P_SKILL(typ) <= P_BASIC) {
		You("hook yourself!"); /* EN You("hook yourself!"); */ // TODO DE
		losehp(rn1(10,10), "a grappling hook", KILLED_BY); /* EN losehp(rn1(10,10), "a grappling hook", KILLED_BY); */ // TODO DE
		return (1);
	    }
	    break;
	}
	pline(nothing_happens);
	return (1);
}


#define BY_OBJECT	((struct monst *)0)

/* return 1 if the wand is broken, hence some time elapsed */
STATIC_OVL int
do_break_wand(obj)
    struct obj *obj;
{
    static const char nothing_else_happens[] = "Aber es passiert nichts weiter ..."; /* EN static const char nothing_else_happens[] = "But nothing else happens..."; */
    register int i, x, y;
    register struct monst *mon;
    int dmg, damage;
    boolean affects_objects;
    boolean shop_damage = FALSE;
    int expltype = EXPL_MAGICAL;
    char confirm[QBUFSZ], the_wand[BUFSZ], buf[BUFSZ];

    Strcpy(the_wand, yname(obj));
    Sprintf(confirm, "SUBJECT VERB_SEIN PRONOMEN_PERSONAL Are you really sure you want to break %s?", /* EN Sprintf(confirm, "Are you really sure you want to break %s?", */ // TODO DE
	safe_qbuf("", sizeof("Are you really sure you want to break ?"), /* EN safe_qbuf("", sizeof("Are you really sure you want to break ?"), */ // TODO DE
				the_wand, ysimple_name(obj), "ARTIKEL_BESTIMMTER NOUN_WAND")); /* EN the_wand, ysimple_name(obj), "the wand")); */
    if (yn(confirm) == 'n' ) return 0;

    if (nohands(youmonst.data)) {
	You("VERB_KOENNEN OBJECT %s ohne Hände nicht zerbrechen!", the_wand); /* EN You_cant("break %s without hands!", the_wand); */
	return 0;
    } else if (ACURR(A_STR) < 10) {
	Dir("VERB_FEHLEN die Stärke um OBJECT %s zu zerbrechem!", the_wand); /* EN You("don't have the strength to break %s!", the_wand); */
	return 0;
    }
    You("VERB_HALTEN OBJECT %s hoch über NEUES_OBJECT OBJECT PRONOMEN_POSSESSIV %s und VERB_BRECHEN ihn entzwei!", /* EN pline("Raising %s high above your %s, you break it in two!", */
	  the_wand, body_part(HEAD));

    /* [ALI] Do this first so that wand is removed from bill. Otherwise,
     * the freeinv() below also hides it from setpaid() which causes problems.
     */
    if (obj->unpaid) {
	check_unpaid(obj);		/* Extra charge for use */
	bill_dummy_object(obj);
    }

    current_wand = obj;		/* destroy_item might reset this */
    freeinv(obj);		/* hide it from destroy_item instead... */
    setnotworn(obj);		/* so we need to do this ourselves */

    if (obj->spe <= 0) {
	pline(nothing_else_happens);
	goto discard_broken_wand;
    }
    obj->ox = u.ux;
    obj->oy = u.uy;
    dmg = obj->spe * 4;
    affects_objects = FALSE;

    switch (obj->otyp) {
    case WAN_WISHING:
    case WAN_NOTHING:
    case WAN_LOCKING:
    case WAN_PROBING:
    case WAN_ENLIGHTENMENT:
    case WAN_OPENING:
    case WAN_SECRET_DOOR_DETECTION:
	pline(nothing_else_happens);
	goto discard_broken_wand;
    case WAN_DEATH:
    case WAN_LIGHTNING:
	dmg *= 4;
	goto wanexpl;
    case WAN_FIRE:
	expltype = EXPL_FIERY;
    case WAN_COLD:
	if (expltype == EXPL_MAGICAL) expltype = EXPL_FROSTY;
	dmg *= 2;
    case WAN_MAGIC_MISSILE:
    wanexpl:
	explode(u.ux, u.uy,
		(obj->otyp - WAN_MAGIC_MISSILE), dmg, WAND_CLASS, expltype);
	makeknown(obj->otyp);	/* explode described the effect */
	goto discard_broken_wand;
    case WAN_STRIKING:
	/* we want this before the explosion instead of at the very end */
	pline("A wall of force smashes down around you!"); /* EN pline("A wall of force smashes down around you!"); */ // TODO DE
	dmg = d(1 + obj->spe,6);	/* normally 2d12 */
    case WAN_CANCELLATION:
    case WAN_POLYMORPH:
    case WAN_TELEPORTATION:
    case WAN_UNDEAD_TURNING:
	affects_objects = TRUE;
	break;
    default:
	break;
    }

    /* magical explosion and its visual effect occur before specific effects */
    explode(obj->ox, obj->oy, 0, rnd(dmg), WAND_CLASS, EXPL_MAGICAL);

    /* this makes it hit us last, so that we can see the action first */
    for (i = 0; i <= 8; i++) {
	bhitpos.x = x = obj->ox + xdir[i];
	bhitpos.y = y = obj->oy + ydir[i];
	if (!isok(x,y)) continue;

	if (obj->otyp == WAN_DIGGING) {
	    if(dig_check(BY_OBJECT, FALSE, x, y)) {
		if (IS_WALL(levl[x][y].typ) || IS_DOOR(levl[x][y].typ)) {
		    /* normally, pits and holes don't anger guards, but they
		     * do if it's a wall or door that's being dug */
		    watch_dig((struct monst *)0, x, y, TRUE);
		    if (*in_rooms(x,y,SHOPBASE)) shop_damage = TRUE;
		}		    
		digactualhole(x, y, BY_OBJECT,
			      (rn2(obj->spe) < 3 || !Can_dig_down(&u.uz)) ?
			       PIT : HOLE);
	    }
	    continue;
	} else if(obj->otyp == WAN_CREATE_MONSTER) {
	    /* u.ux,u.uy creates it near you--x,y might create it in rock */
	    (void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
	    continue;
	} else {
	    if (x == u.ux && y == u.uy) {
		/* teleport objects first to avoid race with tele control and
		   autopickup.  Other wand/object effects handled after
		   possible wand damage is assessed */
		if (obj->otyp == WAN_TELEPORTATION &&
		    affects_objects && level.objects[x][y]) {
		    (void) bhitpile(obj, bhito, x, y);
		    if (flags.botl) bot();		/* potion effects */
		}
		damage = zapyourself(obj, FALSE);
		if (damage) {
		    Sprintf(buf, "tötete sich selbst %sself by breaking a wand", uhim()); /* EN Sprintf(buf, "killed %sself by breaking a wand", uhim()); */ // TODO DE
		    losehp(damage, buf, NO_KILLER_PREFIX);
		}
		if (flags.botl) bot();		/* blindness */
	    } else if ((mon = m_at(x, y)) != 0) {
		(void) bhitm(mon, obj);
	     /* if (flags.botl) bot(); */
	    }
	    if (affects_objects && level.objects[x][y]) {
		(void) bhitpile(obj, bhito, x, y);
		if (flags.botl) bot();		/* potion effects */
	    }
	}
    }

    /* Note: if player fell thru, this call is a no-op.
       Damage is handled in digactualhole in that case */
    if (shop_damage) pay_for_damage("dig into", FALSE); /* EN if (shop_damage) pay_for_damage("dig into", FALSE); */ // TODO DE

    if (obj->otyp == WAN_LIGHT)
	litroom(TRUE, obj);	/* only needs to be done once */

 discard_broken_wand:
    obj = current_wand;		/* [see dozap() and destroy_item()] */
    current_wand = 0;
    if (obj)
	delobj(obj);
    nomul(0);
    return 1;
}

STATIC_OVL boolean
uhave_graystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(is_graystone(otmp))
			return TRUE;
	return FALSE;
}

STATIC_OVL void
add_class(cl, class)
char *cl;
char class;
{
	char tmp[2];
	tmp[0] = class;
	tmp[1] = '\0';
	Strcat(cl, tmp);
}

int
doapply()
{
	struct obj *obj;
	register int res = 1;
	char class_list[MAXOCLASSES+2];

	if(check_capacity((char *)0)) return (0);

	if (carrying(POT_OIL) || uhave_graystone())
		Strcpy(class_list, tools_too);
	else
		Strcpy(class_list, tools);
	if (carrying(CREAM_PIE) || carrying(EUCALYPTUS_LEAF))
		add_class(class_list, FOOD_CLASS);

	obj = getobj(class_list, "use or apply"); /* EN obj = getobj(class_list, "use or apply"); */ // TODO DE
	if(!obj) return 0;

	if (obj->oartifact && !touch_artifact(obj, &youmonst))
	    return 1;	/* evading your grasp costs a turn; just be
			   grateful that you don't drop it as well */

	if (obj->oclass == WAND_CLASS)
	    return do_break_wand(obj);

	switch(obj->otyp){
	case BLINDFOLD:
	case LENSES:
		if (obj == ublindf) {
		    if (!cursed(obj)) Blindf_off(obj);
		} else if (!ublindf)
		    Blindf_on(obj);
		else You("%s.", /* EN else You("are already %s.", */ // TODO DE
			ublindf->otyp == TOWEL ?     "VERB_SEIN bereits covered by a towel" : /* EN ublindf->otyp == TOWEL ?     "covered by a towel" : */ // TODO DE
			ublindf->otyp == BLINDFOLD ? "VERB_TRAGEN bereits OBJECT ARTIKEL_UNBESTIMMTER NOUN_BLINDFOLD" : /* EN ublindf->otyp == BLINDFOLD ? "wearing a blindfold" : */
						     "VERB_TRAGEN bereits OBJECT ARTIKEL_UNBESTIMMTER NOUN_LENSESs"); /* EN "wearing lenses"); */
		break;
	case CREAM_PIE:
		res = use_cream_pie(obj);
		break;
	case BULLWHIP:
		res = use_whip(obj);
		break;
	case GRAPPLING_HOOK:
		res = use_grapple(obj);
		break;
	case LARGE_BOX:
	case CHEST:
	case ICE_BOX:
	case SACK:
	case BAG_OF_HOLDING:
	case OILSKIN_SACK:
		res = use_container(obj, 1);
		break;
	case BAG_OF_TRICKS:
		bagotricks(obj);
		break;
	case CAN_OF_GREASE:
		use_grease(obj);
		break;
	case LOCK_PICK:
#ifdef TOURIST
	case CREDIT_CARD:
#endif
	case SKELETON_KEY:
		(void) pick_lock(obj);
		break;
	case PICK_AXE:
	case DWARVISH_MATTOCK:
		res = use_pick_axe(obj);
		break;
	case TINNING_KIT:
		use_tinning_kit(obj);
		break;
	case LEASH:
		use_leash(obj);
		break;
#ifdef STEED
	case SADDLE:
		res = use_saddle(obj);
		break;
#endif
	case MAGIC_WHISTLE:
		use_magic_whistle(obj);
		break;
	case TIN_WHISTLE:
		use_whistle(obj);
		break;
	case EUCALYPTUS_LEAF:
		/* MRKR: Every Australian knows that a gum leaf makes an */
		/*	 excellent whistle, especially if your pet is a  */
		/*	 tame kangaroo named Skippy.			 */
		if (obj->blessed) {
		    use_magic_whistle(obj);
		    /* sometimes the blessing will be worn off */
		    if (!rn2(49)) {
			if (!Blind) {
			    char buf[BUFSZ];

			    pline("SUBJECT %s %s %s.", Shk_Your(buf, obj), /* EN pline("%s %s %s.", Shk_Your(buf, obj), */
				  aobjnam(obj, "glow"), hcolor("brown")); /* EN aobjnam(obj, "glow"), hcolor("brown")); */ // TODO DE
			    obj->bknown = 1;
			}
			unbless(obj);
		    }
		} else {
		    use_whistle(obj);
		}
		break;
	case STETHOSCOPE:
		res = use_stethoscope(obj);
		break;
	case MIRROR:
		res = use_mirror(obj);
		break;
	case BELL:
	case BELL_OF_OPENING:
		use_bell(&obj);
		break;
	case CANDELABRUM_OF_INVOCATION:
		use_candelabrum(obj);
		break;
	case WAX_CANDLE:
	case TALLOW_CANDLE:
		use_candle(&obj);
		break;
	case OIL_LAMP:
	case MAGIC_LAMP:
	case BRASS_LANTERN:
		use_lamp(obj);
		break;
	case POT_OIL:
		light_cocktail(obj);
		break;
#ifdef TOURIST
	case EXPENSIVE_CAMERA:
		res = use_camera(obj);
		break;
#endif
	case TOWEL:
		res = use_towel(obj);
		break;
	case CRYSTAL_BALL:
		use_crystal_ball(obj);
		break;
	case MAGIC_MARKER:
		res = dowrite(obj);
		break;
	case TIN_OPENER:
		if(!carrying(TIN)) {
			You("VERB_HABEN OBJECT PRONOMEN_KEIN NOUN_TIN zum Öffnen."); /* EN You("have no tin to open."); */
			goto xit;
		}
		You("VERB_KOENNEN OBJECT PRONOMEN_KEIN NOUN_TIN öffnen ohne ihren Inhalt zu essen oder wegzuwerfen."); /* EN You("cannot open a tin without eating or discarding its contents."); */
		if(flags.verbose)
			pline("Um zu essen, MODIFIER_VERB_IMPERATIV VERB_BENUTZEN den Befehl 'e'."); /* EN pline("In order to eat, use the 'e' command."); */
		if(obj != uwep)
    pline("Die Dose ließe sich viel leichter öffnen, wenn SUBJECT_IM_SATZ PRONOMEN_PERSONAL den Dosenöffner benutzen MODIFIER_KONJUNKTIV_II VERB_WERDEN."); /* EN pline("Opening the tin will be much easier if you wield the tin opener."); */
		goto xit;

	case FIGURINE:
		use_figurine(&obj);
		break;
	case UNICORN_HORN:
		use_unicorn_horn(obj);
		break;
	case WOODEN_FLUTE:
	case MAGIC_FLUTE:
	case TOOLED_HORN:
	case FROST_HORN:
	case FIRE_HORN:
	case WOODEN_HARP:
	case MAGIC_HARP:
	case BUGLE:
	case LEATHER_DRUM:
	case DRUM_OF_EARTHQUAKE:
		res = do_play_instrument(obj);
		break;
	case HORN_OF_PLENTY:	/* not a musical instrument */
		if (obj->spe > 0) {
		    struct obj *otmp;
		    const char *what;

		    consume_obj_charge(obj, TRUE);
		    if (!rn2(13)) {
			otmp = mkobj(POTION_CLASS, FALSE);
			if (objects[otmp->otyp].oc_magic) do {
			    otmp->otyp = rnd_class(POT_BOOZE, POT_WATER);
			} while (otmp->otyp == POT_SICKNESS);
			what = "ARTIKEL_UNBESTIMMTER NOUN_POTION"; /* EN what = "A potion"; */
		    } else {
			otmp = mkobj(FOOD_CLASS, FALSE);
			if (otmp->otyp == FOOD_RATION && !rn2(7))
			    otmp->otyp = LUMP_OF_ROYAL_JELLY;
			what = "Etwas zu essen"; /* EN what = "Some food"; */
		    }
		    pline("SUBJECT %s kommt zum Vorschein.", what); /* EN pline("%s spills out.", what); */
		    otmp->blessed = obj->blessed;
		    otmp->cursed = obj->cursed;
		    otmp->owt = weight(otmp);
		    otmp = hold_another_object(otmp, u.uswallow ?
				       "NOUN_OOPS!  SUBJECT %s außer Reichweite!" : /* EN "Oops!  %s out of your reach!" : */
					(Is_airlevel(&u.uz) ||
					 Is_waterlevel(&u.uz) ||
					 levl[u.ux][u.uy].typ < IRONBARS ||
					 levl[u.ux][u.uy].typ >= ICE) ?
					       "NOUN_OOPS!  SUBJECT %s von OBJECT KASUS_DATIV PRONOMEN_PERSONAL weg!" : /* EN "Oops!  %s away from you!" : */
					       "NOUN_OOPS!  SUBJECT %s zu Boden!", /* EN "Oops!  %s to the floor!", */
					       The(aobjnam(otmp, "VERB_RUTSCHEN")), /* EN The(aobjnam(otmp, "slip")), */
					       (const char *)0);
		    makeknown(HORN_OF_PLENTY);
		} else
		    pline(nothing_happens);
		break;
	case LAND_MINE:
	case BEARTRAP:
		use_trap(obj);
		break;
	case FLINT:
	case LUCKSTONE:
	case LOADSTONE:
	case TOUCHSTONE:
		use_stone(obj);
		break;
	default:
		/* Pole-weapons can strike at a distance */
		if (is_pole(obj)) {
			res = use_pole(obj);
			break;
		} else if (is_pick(obj) || is_axe(obj)) {
			res = use_pick_axe(obj);
			break;
		}
		pline("Tut mir leid, ich weiss nicht, wie das zu benutzen ist."); /* EN pline("Sorry, I don't know how to use that."); */
	xit:
		nomul(0);
		return 0;
	}
	if (res && obj && obj->oartifact) arti_speak(obj);
	nomul(0);
	return res;
}

/* Keep track of unfixable troubles for purposes of messages saying you feel
 * great.
 */
int
unfixable_trouble_count(is_horn)
	boolean is_horn;
{
	int unfixable_trbl = 0;

	if (Stoned) unfixable_trbl++;
	if (Strangled) unfixable_trbl++;
	if (Wounded_legs
#ifdef STEED
		    && !u.usteed
#endif
				) unfixable_trbl++;
	if (Slimed) unfixable_trbl++;
	/* lycanthropy is not desirable, but it doesn't actually make you feel
	   bad */

	/* we'll assume that intrinsic stunning from being a bat/stalker
	   doesn't make you feel bad */
	if (!is_horn) {
	    if (Confusion) unfixable_trbl++;
	    if (Sick) unfixable_trbl++;
	    if (HHallucination) unfixable_trbl++;
	    if (Vomiting) unfixable_trbl++;
	    if (HStun) unfixable_trbl++;
	}
	return unfixable_trbl;
}

#endif /* OVLB */

/*apply.c*/
