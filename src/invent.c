/*	SCCS Id: @(#)invent.c	3.4	2003/12/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef GERMAN
#include "german.h"
#endif

#define NOINVSYM	'#'
#define CONTAINED_SYM	'>'	/* designator for inside a container */

#ifdef OVL1
STATIC_DCL void NDECL(reorder_invent);
STATIC_DCL boolean FDECL(mergable,(struct obj *,struct obj *));
STATIC_DCL void FDECL(invdisp_nothing, (const char *,const char *));
STATIC_DCL boolean FDECL(worn_wield_only, (struct obj *));
STATIC_DCL boolean FDECL(only_here, (struct obj *));
#endif /* OVL1 */
STATIC_DCL void FDECL(compactify,(char *));
STATIC_DCL boolean FDECL(taking_off, (const char *));
STATIC_DCL boolean FDECL(putting_on, (const char *));
STATIC_PTR int FDECL(ckunpaid,(struct obj *));
STATIC_PTR int FDECL(ckvalidcat,(struct obj *));
static char FDECL(display_pickinv, (const char *,BOOLEAN_P, long *));
#ifdef OVLB
STATIC_DCL boolean FDECL(this_type_only, (struct obj *));
STATIC_DCL void NDECL(dounpaid);
STATIC_DCL struct obj *FDECL(find_unpaid,(struct obj *,struct obj **));
STATIC_DCL void FDECL(menu_identify, (int));
STATIC_DCL boolean FDECL(tool_in_use, (struct obj *));
#endif /* OVLB */
STATIC_DCL char FDECL(obj_to_let,(struct obj *));

#ifdef OVLB

static int lastinvnr = 51;	/* 0 ... 51 (never saved&restored) */

#ifdef WIZARD
/* wizards can wish for venom, which will become an invisible inventory
 * item without this.  putting it in inv_order would mean venom would
 * suddenly become a choice for all the inventory-class commands, which
 * would probably cause mass confusion.  the test for inventory venom
 * is only WIZARD and not wizard because the wizard can leave venom lying
 * around on a bones level for normal players to find.
 */
static char venom_inv[] = { VENOM_CLASS, 0 };	/* (constant) */
#endif

void
assigninvlet(otmp)
register struct obj *otmp;
{
	boolean inuse[52];
	register int i;
	register struct obj *obj;

#ifdef GOLDOBJ
        /* There is only one of these in inventory... */        
        if (otmp->oclass == COIN_CLASS) {
	    otmp->invlet = GOLD_SYM;
	    return;
	}
#endif

	for(i = 0; i < 52; i++) inuse[i] = FALSE;
	for(obj = invent; obj; obj = obj->nobj) if(obj != otmp) {
		i = obj->invlet;
		if('a' <= i && i <= 'z') inuse[i - 'a'] = TRUE; else
		if('A' <= i && i <= 'Z') inuse[i - 'A' + 26] = TRUE;
		if(i == otmp->invlet) otmp->invlet = 0;
	}
	if((i = otmp->invlet) &&
	    (('a' <= i && i <= 'z') || ('A' <= i && i <= 'Z')))
		return;
	for(i = lastinvnr+1; i != lastinvnr; i++) {
		if(i == 52) { i = -1; continue; }
		if(!inuse[i]) break;
	}
	otmp->invlet = (inuse[i] ? NOINVSYM :
			(i < 26) ? ('a'+i) : ('A'+i-26));
	lastinvnr = i;
}

#endif /* OVLB */
#ifdef OVL1

/* note: assumes ASCII; toggling a bit puts lowercase in front of uppercase */
#define inv_rank(o) ((o)->invlet ^ 040)

/* sort the inventory; used by addinv() and doorganize() */
STATIC_OVL void
reorder_invent()
{
	struct obj *otmp, *prev, *next;
	boolean need_more_sorting;

	do {
	    /*
	     * We expect at most one item to be out of order, so this
	     * isn't nearly as inefficient as it may first appear.
	     */
	    need_more_sorting = FALSE;
	    for (otmp = invent, prev = 0; otmp; ) {
		next = otmp->nobj;
		if (next && inv_rank(next) < inv_rank(otmp)) {
		    need_more_sorting = TRUE;
		    if (prev) prev->nobj = next;
		    else      invent = next;
		    otmp->nobj = next->nobj;
		    next->nobj = otmp;
		    prev = next;
		} else {
		    prev = otmp;
		    otmp = next;
		}
	    }
	} while (need_more_sorting);
}

#undef inv_rank

/* scan a list of objects to see whether another object will merge with
   one of them; used in pickup.c when all 52 inventory slots are in use,
   to figure out whether another object could still be picked up */
struct obj *
merge_choice(objlist, obj)
struct obj *objlist, *obj;
{
	struct monst *shkp;
	int save_nocharge;

	if (obj->otyp == SCR_SCARE_MONSTER)	/* punt on these */
	    return (struct obj *)0;
	/* if this is an item on the shop floor, the attributes it will
	   have when carried are different from what they are now; prevent
	   that from eliciting an incorrect result from mergable() */
	save_nocharge = obj->no_charge;
	if (objlist == invent && obj->where == OBJ_FLOOR &&
		(shkp = shop_keeper(inside_shop(obj->ox, obj->oy))) != 0) {
	    if (obj->no_charge) obj->no_charge = 0;
	    /* A billable object won't have its `unpaid' bit set, so would
	       erroneously seem to be a candidate to merge with a similar
	       ordinary object.  That's no good, because once it's really
	       picked up, it won't merge after all.  It might merge with
	       another unpaid object, but we can't check that here (depends
	       too much upon shk's bill) and if it doesn't merge it would
	       end up in the '#' overflow inventory slot, so reject it now. */
	    else if (inhishop(shkp)) return (struct obj *)0;
	}
	while (objlist) {
	    if (mergable(objlist, obj)) break;
	    objlist = objlist->nobj;
	}
	obj->no_charge = save_nocharge;
	return objlist;
}

/* merge obj with otmp and delete obj if types agree */
int
merged(potmp, pobj)
struct obj **potmp, **pobj;
{
	register struct obj *otmp = *potmp, *obj = *pobj;

	if(mergable(otmp, obj)) {
		/* Approximate age: we do it this way because if we were to
		 * do it "accurately" (merge only when ages are identical)
		 * we'd wind up never merging any corpses.
		 * otmp->age = otmp->age*(1-proportion) + obj->age*proportion;
		 *
		 * Don't do the age manipulation if lit.  We would need
		 * to stop the burn on both items, then merge the age,
		 * then restart the burn.
		 */
		if (!obj->lamplit)
		    otmp->age = ((otmp->age*otmp->quan) + (obj->age*obj->quan))
			    / (otmp->quan + obj->quan);

		otmp->quan += obj->quan;
#ifdef GOLDOBJ
                /* temporary special case for gold objects!!!! */
#endif
		if (otmp->oclass == COIN_CLASS) otmp->owt = weight(otmp);
		else otmp->owt += obj->owt;
		if(!otmp->onamelth && obj->onamelth)
			otmp = *potmp = oname(otmp, ONAME(obj));
		obj_extract_self(obj);

		/* really should merge the timeouts */
		if (obj->lamplit) obj_merge_light_sources(obj, otmp);
		if (obj->timed) obj_stop_timers(obj);	/* follows lights */

		/* fixup for `#adjust' merging wielded darts, daggers, &c */
		if (obj->owornmask && carried(otmp)) {
		    long wmask = otmp->owornmask | obj->owornmask;

		    /* Both the items might be worn in competing slots;
		       merger preference (regardless of which is which):
			 primary weapon + alternate weapon -> primary weapon;
			 primary weapon + quiver -> primary weapon;
			 alternate weapon + quiver -> alternate weapon.
		       (Prior to 3.3.0, it was not possible for the two
		       stacks to be worn in different slots and `obj'
		       didn't need to be unworn when merging.) */
		    if (wmask & W_WEP) wmask = W_WEP;
		    else if (wmask & W_SWAPWEP) wmask = W_SWAPWEP;
		    else if (wmask & W_QUIVER) wmask = W_QUIVER;
		    else {
			impossible("merging strangely worn items (%lx)", wmask);
			wmask = otmp->owornmask;
		    }
		    if ((otmp->owornmask & ~wmask) != 0L) setnotworn(otmp);
		    setworn(otmp, wmask);
		    setnotworn(obj);
		}
#if 0
		/* (this should not be necessary, since items
		    already in a monster's inventory don't ever get
		    merged into other objects [only vice versa]) */
		else if (obj->owornmask && mcarried(otmp)) {
		    if (obj == MON_WEP(otmp->ocarry)) {
			MON_WEP(otmp->ocarry) = otmp;
			otmp->owornmask = W_WEP;
		    }
		}
#endif /*0*/

		obfree(obj,otmp);	/* free(obj), bill->otmp */
		return(1);
	}
	return 0;
}

/*
Adjust hero intrinsics as if this object was being added to the hero's
inventory.  Called _before_ the object has been added to the hero's
inventory.

This is called when adding objects to the hero's inventory normally (via
addinv) or when an object in the hero's inventory has been polymorphed
in-place.

It may be valid to merge this code with with addinv_core2().
*/
void
addinv_core1(obj)
struct obj *obj;
{
	if (obj->oclass == COIN_CLASS) {
#ifndef GOLDOBJ
		u.ugold += obj->quan;
#else
		flags.botl = 1;
#endif
	} else if (obj->otyp == AMULET_OF_YENDOR) {
		if (u.uhave.amulet) impossible("already have amulet?");
		u.uhave.amulet = 1;
#ifdef RECORD_ACHIEVE
                achieve.get_amulet = 1;
#ifdef LIVELOGFILE
				livelog_achieve_update();
#endif
#endif
	} else if (obj->otyp == CANDELABRUM_OF_INVOCATION) {
		if (u.uhave.menorah) impossible("already have candelabrum?");
		u.uhave.menorah = 1;
#ifdef RECORD_ACHIEVE
                achieve.get_candelabrum = 1;
#ifdef LIVELOGFILE
				livelog_achieve_update();
#endif
#endif
	} else if (obj->otyp == BELL_OF_OPENING) {
		if (u.uhave.bell) impossible("already have silver bell?");
		u.uhave.bell = 1;
#ifdef RECORD_ACHIEVE
                achieve.get_bell = 1;
#ifdef LIVELOGFILE
				livelog_achieve_update();
#endif
#endif
	} else if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
		if (u.uhave.book) impossible("already have the book?");
		u.uhave.book = 1;
#ifdef RECORD_ACHIEVE
                achieve.get_book = 1;
#ifdef LIVELOGFILE
				livelog_achieve_update();
#endif
#endif
	} else if (obj->oartifact) {
		if (is_quest_artifact(obj)) {
		    if (u.uhave.questart)
			impossible("already have quest artifact?");
		    u.uhave.questart = 1;
		    artitouch();
		}
		set_artifact_intrinsic(obj, 1, W_ART);
	}

#ifdef RECORD_ACHIEVE
        if(obj->otyp == LUCKSTONE && obj->record_achieve_special) {
                achieve.get_luckstone = 1;
                obj->record_achieve_special = 0;
#ifdef LIVELOGFILE
				livelog_achieve_update();
#endif
        } else if((obj->otyp == AMULET_OF_REFLECTION ||
                   obj->otyp == BAG_OF_HOLDING) &&
                  obj->record_achieve_special) {
                achieve.finish_sokoban = 1;
                obj->record_achieve_special = 0;
#ifdef LIVELOGFILE
				livelog_achieve_update();
#endif
        }
#endif /* RECORD_ACHIEVE */

}

/*
Adjust hero intrinsics as if this object was being added to the hero's
inventory.  Called _after_ the object has been added to the hero's
inventory.

This is called when adding objects to the hero's inventory normally (via
addinv) or when an object in the hero's inventory has been polymorphed
in-place.
*/
void
addinv_core2(obj)
struct obj *obj;
{
	if (confers_luck(obj)) {
		/* new luckstone must be in inventory by this point
		 * for correct calculation */
		set_moreluck();
	}
}

/*
Add obj to the hero's inventory.  Make sure the object is "free".
Adjust hero attributes as necessary.
*/
struct obj *
addinv(obj)
struct obj *obj;
{
	struct obj *otmp, *prev;

	if (obj->where != OBJ_FREE)
	    panic("addinv: obj not free");
	obj->no_charge = 0;	/* not meaningful for invent */

	addinv_core1(obj);
#ifndef GOLDOBJ
	/* if handed gold, we're done */
	if (obj->oclass == COIN_CLASS)
	    return obj;
#endif

	/* merge if possible; find end of chain in the process */
	for (prev = 0, otmp = invent; otmp; prev = otmp, otmp = otmp->nobj)
	    if (merged(&otmp, &obj)) {
		obj = otmp;
		goto added;
	    }
	/* didn't merge, so insert into chain */
	if (flags.invlet_constant || !prev) {
	    if (flags.invlet_constant) assigninvlet(obj);
	    obj->nobj = invent;		/* insert at beginning */
	    invent = obj;
	    if (flags.invlet_constant) reorder_invent();
	} else {
	    prev->nobj = obj;		/* insert at end */
	    obj->nobj = 0;
	}
	obj->where = OBJ_INVENT;

added:
	addinv_core2(obj);
	carry_obj_effects(obj);		/* carrying affects the obj */
	update_inventory();
	return(obj);
}

/*
 * Some objects are affected by being carried.
 * Make those adjustments here. Called _after_ the object
 * has been added to the hero's or monster's inventory,
 * and after hero's intrinsics have been updated.
 */
void
carry_obj_effects(obj)
struct obj *obj;
{
	/* Cursed figurines can spontaneously transform
	   when carried. */
	if (obj->otyp == FIGURINE) {
		if (obj->cursed
	    	    && obj->corpsenm != NON_PM
	    	    && !dead_species(obj->corpsenm,TRUE)) {
			attach_fig_transform_timeout(obj);
		    }
	}
}

#endif /* OVL1 */
#ifdef OVLB

/* Add an item to the inventory unless we're fumbling or it refuses to be
 * held (via touch_artifact), and give a message.
 * If there aren't any free inventory slots, we'll drop it instead.
 * If both success and failure messages are NULL, then we're just doing the
 * fumbling/slot-limit checking for a silent grab.  In any case,
 * touch_artifact will print its own messages if they are warranted.
 */
struct obj *
hold_another_object(obj, drop_fmt, drop_arg, hold_msg)
struct obj *obj;
const char *drop_fmt, *drop_arg, *hold_msg;
{
	char buf[BUFSZ];

	if (!Blind) obj->dknown = 1;	/* maximize mergibility */
	if (obj->oartifact) {
	    /* place_object may change these */
	    boolean crysknife = (obj->otyp == CRYSKNIFE);
	    int oerode = obj->oerodeproof;
	    boolean wasUpolyd = Upolyd;

	    /* in case touching this object turns out to be fatal */
	    place_object(obj, u.ux, u.uy);

	    if (!touch_artifact(obj, &youmonst)) {
		obj_extract_self(obj);	/* remove it from the floor */
		dropy(obj);		/* now put it back again :-) */
		return obj;
	    } else if (wasUpolyd && !Upolyd) {
		/* loose your grip if you revert your form */
		if (drop_fmt) pline(drop_fmt, drop_arg);
		obj_extract_self(obj);
		dropy(obj);
		return obj;
	    }
	    obj_extract_self(obj);
	    if (crysknife) {
		obj->otyp = CRYSKNIFE;
		obj->oerodeproof = oerode;
	    }
	}
	if (Fumbling) {
	    if (drop_fmt) pline(drop_fmt, drop_arg);
	    dropy(obj);
	} else {
	    long oquan = obj->quan;
	    int prev_encumbr = near_capacity();	/* before addinv() */

	    /* encumbrance only matters if it would now become worse
	       than max( current_value, stressed ) */
	    if (prev_encumbr < MOD_ENCUMBER) prev_encumbr = MOD_ENCUMBER;
	    /* addinv() may redraw the entire inventory, overwriting
	       drop_arg when it comes from something like doname() */
	    if (drop_arg) drop_arg = strcpy(buf, drop_arg);

	    obj = addinv(obj);
	    if (inv_cnt() > 52
		    || ((obj->otyp != LOADSTONE || !obj->cursed)
			&& near_capacity() > prev_encumbr)) {
		if (drop_fmt) pline(drop_fmt, drop_arg);
		/* undo any merge which took place */
		if (obj->quan > oquan) obj = splitobj(obj, oquan);
		dropx(obj);
	    } else {
		if (flags.autoquiver && !uquiver && !obj->owornmask &&
			(is_missile(obj) ||
			    ammo_and_launcher(obj, uwep) ||
			    ammo_and_launcher(obj, uswapwep)))
		    setuqwep(obj);
		if (hold_msg || drop_fmt) prinv(hold_msg, obj, oquan);
	    }
	}
	return obj;
}

/* useup() all of an item regardless of its quantity */
void
useupall(obj)
struct obj *obj;
{
	setnotworn(obj);
	freeinv(obj);
	obfree(obj, (struct obj *)0);	/* deletes contents also */
}

void
useup(obj)
register struct obj *obj;
{
	/*  Note:  This works correctly for containers because they */
	/*	   (containers) don't merge.			    */
	if (obj->quan > 1L) {
		obj->in_use = FALSE;	/* no longer in use */
		obj->quan--;
		obj->owt = weight(obj);
		update_inventory();
	} else {
		useupall(obj);
	}
}

/* use one charge from an item and possibly incur shop debt for it */
void
consume_obj_charge(obj, maybe_unpaid)
struct obj *obj;
boolean maybe_unpaid;	/* false if caller handles shop billing */
{
	if (maybe_unpaid) check_unpaid(obj);
	obj->spe -= 1;
	if (obj->known) update_inventory();
}

#endif /* OVLB */
#ifdef OVL3

/*
Adjust hero's attributes as if this object was being removed from the
hero's inventory.  This should only be called from freeinv() and
where we are polymorphing an object already in the hero's inventory.

Should think of a better name...
*/
void
freeinv_core(obj)
struct obj *obj;
{
	if (obj->oclass == COIN_CLASS) {
#ifndef GOLDOBJ
		u.ugold -= obj->quan;
		obj->in_use = FALSE;
#endif
		flags.botl = 1;
		return;
	} else if (obj->otyp == AMULET_OF_YENDOR) {
		if (!u.uhave.amulet) impossible("don't have amulet?");
		u.uhave.amulet = 0;
	} else if (obj->otyp == CANDELABRUM_OF_INVOCATION) {
		if (!u.uhave.menorah) impossible("don't have candelabrum?");
		u.uhave.menorah = 0;
	} else if (obj->otyp == BELL_OF_OPENING) {
		if (!u.uhave.bell) impossible("don't have silver bell?");
		u.uhave.bell = 0;
	} else if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
		if (!u.uhave.book) impossible("don't have the book?");
		u.uhave.book = 0;
	} else if (obj->oartifact) {
		if (is_quest_artifact(obj)) {
		    if (!u.uhave.questart)
			impossible("don't have quest artifact?");
		    u.uhave.questart = 0;
		}
		set_artifact_intrinsic(obj, 0, W_ART);
	}

	if (obj->otyp == LOADSTONE) {
		curse(obj);
	} else if (confers_luck(obj)) {
		set_moreluck();
		flags.botl = 1;
	} else if (obj->otyp == FIGURINE && obj->timed) {
		(void) stop_timer(FIG_TRANSFORM, (genericptr_t) obj);
	}
}

/* remove an object from the hero's inventory */
void
freeinv(obj)
register struct obj *obj;
{
	extract_nobj(obj, &invent);
	freeinv_core(obj);
	update_inventory();
}

void
delallobj(x, y)
int x, y;
{
	struct obj *otmp, *otmp2;

	for (otmp = level.objects[x][y]; otmp; otmp = otmp2) {
		if (otmp == uball)
			unpunish();
		/* after unpunish(), or might get deallocated chain */
		otmp2 = otmp->nexthere;
		if (otmp == uchain)
			continue;
		delobj(otmp);
	}
}

#endif /* OVL3 */
#ifdef OVL2

/* destroy object in fobj chain (if unpaid, it remains on the bill) */
void
delobj(obj)
register struct obj *obj;
{
	boolean update_map;

	if (obj->otyp == AMULET_OF_YENDOR ||
			obj->otyp == CANDELABRUM_OF_INVOCATION ||
			obj->otyp == BELL_OF_OPENING ||
			obj->otyp == SPE_BOOK_OF_THE_DEAD) {
		/* player might be doing something stupid, but we
		 * can't guarantee that.  assume special artifacts
		 * are indestructible via drawbridges, and exploding
		 * chests, and golem creation, and ...
		 */
		return;
	}
	update_map = (obj->where == OBJ_FLOOR);
	obj_extract_self(obj);
	if (update_map) newsym(obj->ox, obj->oy);
	obfree(obj, (struct obj *) 0);	/* frees contents also */
}

#endif /* OVL2 */
#ifdef OVL0

struct obj *
sobj_at(n,x,y)
register int n, x, y;
{
	register struct obj *otmp;

	for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		if(otmp->otyp == n)
		    return(otmp);
	return((struct obj *)0);
}

#endif /* OVL0 */
#ifdef OVLB

struct obj *
carrying(type)
register int type;
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == type)
			return(otmp);
	return((struct obj *) 0);
}

const char *
currency(amount)
long amount;
{
	if (amount == 1L) return "Zorkmid"; /* EN if (amount == 1L) return "zorkmid"; */
	else return "Zorkmid"; /* EN else return "zorkmids"; */
}

boolean
have_lizard()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_LIZARD)
			return(TRUE);
	return(FALSE);
}

struct obj *
o_on(id, objchn)
unsigned int id;
register struct obj *objchn;
{
	struct obj *temp;

	while(objchn) {
		if(objchn->o_id == id) return(objchn);
		if (Has_contents(objchn) && (temp = o_on(id,objchn->cobj)))
			return temp;
		objchn = objchn->nobj;
	}
	return((struct obj *) 0);
}

boolean
obj_here(obj, x, y)
register struct obj *obj;
int x, y;
{
	register struct obj *otmp;

	for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		if(obj == otmp) return(TRUE);
	return(FALSE);
}

#endif /* OVLB */
#ifdef OVL2

struct obj *
g_at(x,y)
register int x, y;
{
	register struct obj *obj = level.objects[x][y];
	while(obj) {
	    if (obj->oclass == COIN_CLASS) return obj;
	    obj = obj->nexthere;
	}
	return((struct obj *)0);
}

#endif /* OVL2 */
#ifdef OVLB
#ifndef GOLDOBJ
/* Make a gold object from the hero's gold. */
struct obj *
mkgoldobj(q)
register long q;
{
	register struct obj *otmp;

	otmp = mksobj(GOLD_PIECE, FALSE, FALSE);
	u.ugold -= q;
	otmp->quan = q;
	otmp->owt = weight(otmp);
	flags.botl = 1;
	return(otmp);
}
#endif
#endif /* OVLB */
#ifdef OVL1

STATIC_OVL void
compactify(buf)
register char *buf;
/* compact a string of inventory letters by dashing runs of letters */
{
	register int i1 = 1, i2 = 1;
	register char ilet, ilet1, ilet2;

	ilet2 = buf[0];
	ilet1 = buf[1];
	buf[++i2] = buf[++i1];
	ilet = buf[i1];
	while(ilet) {
		if(ilet == ilet1+1) {
			if(ilet1 == ilet2+1)
				buf[i2 - 1] = ilet1 = '-';
			else if(ilet2 == '-') {
				buf[i2 - 1] = ++ilet1;
				buf[i2] = buf[++i1];
				ilet = buf[i1];
				continue;
			}
		}
		ilet2 = ilet1;
		ilet1 = ilet;
		buf[++i2] = buf[++i1];
		ilet = buf[i1];
	}
}

/* match the prompt for either 'T' or 'R' command */
STATIC_OVL boolean
taking_off(action)
const char *action;
{
    return !strcmp(action, "take off") || !strcmp(action, "remove"); /* EN return !strcmp(action, "take off") || !strcmp(action, "remove"); */ // TODO DE
}

/* match the prompt for either 'W' or 'P' command */
STATIC_OVL boolean
putting_on(action)
const char *action;
{
    return !strcmp(action, "VERB_TRAGEN") || !strcmp(action, "put on"); /* EN return !strcmp(action, "wear") || !strcmp(action, "put on"); */ // TODO DE
}

/*
 * getobj returns:
 *	struct obj *xxx:	object to do something with.
 *	(struct obj *) 0	error return: no object.
 *	&zeroobj		explicitly no object (as in w-).
#ifdef GOLDOBJ
!!!! test if gold can be used in unusual ways (eaten etc.)
!!!! may be able to remove "usegold"
#endif
 */
struct obj *
getobj(let,word)
register const char *let,*word;
{
	register struct obj *otmp;
	register char ilet;
	char buf[BUFSZ], qbuf[QBUFSZ];
	char lets[BUFSZ], altlets[BUFSZ], *ap;
	register int foo = 0;
	register char *bp = buf;
	xchar allowcnt = 0;	/* 0, 1 or 2 */
#ifndef GOLDOBJ
	boolean allowgold = FALSE;	/* can't use gold because they don't have any */
#endif
	boolean usegold = FALSE;	/* can't use gold because its illegal */
	boolean allowall = FALSE;
	boolean allownone = FALSE;
	boolean useboulder = FALSE;
	xchar foox = 0;
	long cnt;
	boolean prezero = FALSE;
	long dummymask;
#ifdef GERMAN
	char fragewort[BUFSZ];

	if ((!strcmp(word, "schreiben")) || (!strcmp(word, "entsch�rfen"))) {
		Strcpy(fragewort, "Womit");
	} else {
		Strcpy(fragewort, "Was");
	}
#endif

	if(*let == ALLOW_COUNT) let++, allowcnt = 1;
#ifndef GOLDOBJ
	if(*let == COIN_CLASS) let++,
		usegold = TRUE, allowgold = (u.ugold ? TRUE : FALSE);
#else
	if(*let == COIN_CLASS) let++, usegold = TRUE;
#endif

	/* Equivalent of an "ugly check" for gold */
	if (usegold && !strcmp(word, "VERB_ESSEN") && /* EN if (usegold && !strcmp(word, "eat") && */
	    (!metallivorous(youmonst.data)
	     || youmonst.data == &mons[PM_RUST_MONSTER]))
#ifndef GOLDOBJ
		usegold = allowgold = FALSE;
#else
		usegold = FALSE;
#endif

	if(*let == ALL_CLASSES) let++, allowall = TRUE;
	if(*let == ALLOW_NONE) let++, allownone = TRUE;
	/* "ugly check" for reading fortune cookies, part 1 */
	/* The normal 'ugly check' keeps the object on the inventory list.
	 * We don't want to do that for shirts/cookies, so the check for
	 * them is handled a bit differently (and also requires that we set
	 * allowall in the caller)
	 */
	if(allowall && !strcmp(word, "VERB_READ")) allowall = FALSE; /* EN if(allowall && !strcmp(word, "read")) allowall = FALSE; */

	/* another ugly check: show boulders (not statues) */
	if(*let == WEAPON_CLASS &&
	   !strcmp(word, "VERB_WERFEN") && throws_rocks(youmonst.data)) /* EN !strcmp(word, "throw") && throws_rocks(youmonst.data)) */
	    useboulder = TRUE;

	if(allownone) *bp++ = '-';
#ifndef GOLDOBJ
	if(allowgold) *bp++ = def_oc_syms[COIN_CLASS];
#endif
	if(bp > buf && bp[-1] == '-') *bp++ = ' ';
	ap = altlets;

	ilet = 'a';
	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if (!flags.invlet_constant)
#ifdef GOLDOBJ
		if (otmp->invlet != GOLD_SYM) /* don't reassign this */
#endif
		otmp->invlet = ilet;	/* reassign() */
	    if (!*let || index(let, otmp->oclass)
#ifdef GOLDOBJ
		|| (usegold && otmp->invlet == GOLD_SYM)
#endif
		|| (useboulder && otmp->otyp == BOULDER)
		) {
		register int otyp = otmp->otyp;
		bp[foo++] = otmp->invlet;

		/* ugly check: remove inappropriate things */
		if ((taking_off(word) &&
		    (!(otmp->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL))
		     || (otmp==uarm && uarmc)
#ifdef TOURIST
		     || (otmp==uarmu && (uarm || uarmc))
#endif
		    ))
		|| (putting_on(word) &&
		     (otmp->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL)))
							/* already worn */
#if 0	/* 3.4.1 -- include currently wielded weapon among the choices */
		|| (!strcmp(word, "wield") && /* EN || (!strcmp(word, "wield") && */ // TODO DE
		    (otmp->owornmask & W_WEP))
#endif
		|| (!strcmp(word, "ready") && /* EN || (!strcmp(word, "ready") && */ // TODO DE
		    (otmp == uwep || (otmp == uswapwep && u.twoweap)))
		    ) {
			foo--;
			foox++;
		}

		/* Second ugly check; unlike the first it won't trigger an
		 * "else" in "you don't have anything else to ___".
		 */
		else if ((putting_on(word) &&
		    ((otmp->oclass == FOOD_CLASS && otmp->otyp != MEAT_RING) ||
		    (otmp->oclass == TOOL_CLASS &&
		     otyp != BLINDFOLD && otyp != TOWEL && otyp != LENSES)))
		|| (!strcmp(word, "wield") && /* EN || (!strcmp(word, "wield") && */ // TODO DE
		    (otmp->oclass == TOOL_CLASS && !is_weptool(otmp)))
		|| (!strcmp(word, "VERB_ESSEN") && !is_edible(otmp)) /* EN || (!strcmp(word, "eat") && !is_edible(otmp)) */
		|| (!strcmp(word, "VERB_SACRIFICE") && /* EN || (!strcmp(word, "sacrifice") && */
		    (otyp != CORPSE &&
		     otyp != AMULET_OF_YENDOR && otyp != FAKE_AMULET_OF_YENDOR))
		|| (!strcmp(word, "schreiben") && /* EN || (!strcmp(word, "write with") && */
		    (otmp->oclass == TOOL_CLASS &&
		     otyp != MAGIC_MARKER && otyp != TOWEL))
		|| (!strcmp(word, "eindosen") && /* EN || (!strcmp(word, "tin") && */
		    (otyp != CORPSE || !tinnable(otmp)))
		|| (!strcmp(word, "reiben") && /* EN || (!strcmp(word, "rub") && */
		    ((otmp->oclass == TOOL_CLASS &&
		      otyp != OIL_LAMP && otyp != MAGIC_LAMP &&
		      otyp != BRASS_LANTERN) ||
		     (otmp->oclass == GEM_CLASS && !is_graystone(otmp))))
		|| (!strncmp(word, "rub on the stone", 16) && /* EN || (!strncmp(word, "rub on the stone", 16) && */ // TODO DE
		    *let == GEM_CLASS &&	/* using known touchstone */
		    otmp->dknown && objects[otyp].oc_name_known)
		|| ((!strcmp(word, "use or apply") || /* EN || ((!strcmp(word, "use or apply") || */ // TODO DE
			!strcmp(word, "entsch�rfen")) && /* EN !strcmp(word, "untrap with")) && */
		     /* Picks, axes, pole-weapons, bullwhips */
		    ((otmp->oclass == WEAPON_CLASS && !is_pick(otmp) &&
		      !is_axe(otmp) && !is_pole(otmp) && otyp != BULLWHIP) ||
		     (otmp->oclass == POTION_CLASS &&
		     /* only applicable potion is oil, and it will only
			be offered as a choice when already discovered */
		     (otyp != POT_OIL || !otmp->dknown ||
		      !objects[POT_OIL].oc_name_known)) ||
		     (otmp->oclass == FOOD_CLASS &&
		      otyp != CREAM_PIE && otyp != EUCALYPTUS_LEAF) ||
		     (otmp->oclass == GEM_CLASS && !is_graystone(otmp))))
		|| (!strcmp(word, "invoke") && /* EN || (!strcmp(word, "invoke") && */ // TODO DE
		    (!otmp->oartifact && !objects[otyp].oc_unique &&
		     (otyp != FAKE_AMULET_OF_YENDOR || otmp->known) &&
		     otyp != CRYSTAL_BALL &&	/* #invoke synonym for apply */
		   /* note: presenting the possibility of invoking non-artifact
		      mirrors and/or lamps is a simply a cruel deception... */
		     otyp != MIRROR && otyp != MAGIC_LAMP &&
		     (otyp != OIL_LAMP ||	/* don't list known oil lamp */
		      (otmp->dknown && objects[OIL_LAMP].oc_name_known))))
		|| (!strcmp(word, "entsch�rfen") && /* EN || (!strcmp(word, "untrap with") && */
		    (otmp->oclass == TOOL_CLASS && otyp != CAN_OF_GREASE))
		|| (!strcmp(word, "charge") && !is_chargeable(otmp)) /* EN || (!strcmp(word, "charge") && !is_chargeable(otmp)) */ // TODO DE
		    )
			foo--;
		/* ugly check for unworn armor that can't be worn */
		else if (putting_on(word) && *let == ARMOR_CLASS &&
			 !canwearobj(otmp, &dummymask, FALSE)) {
			foo--;
			allowall = TRUE;
			*ap++ = otmp->invlet;
		}
	    } else {

		/* "ugly check" for reading fortune cookies, part 2 */
		if ((!strcmp(word, "VERB_READ") && /* EN if ((!strcmp(word, "read") && */
		    (otmp->otyp == FORTUNE_COOKIE
#ifdef TOURIST
			|| otmp->otyp == T_SHIRT
#endif
		    )))
			allowall = TRUE;
	    }

	    if(ilet == 'z') ilet = 'A'; else ilet++;
	}
	bp[foo] = 0;
	if(foo == 0 && bp > buf && bp[-1] == ' ') *--bp = 0;
	Strcpy(lets, bp);	/* necessary since we destroy buf */
	if(foo > 5)			/* compactify string */
		compactify(bp);
	*ap = '\0';

#ifndef GOLDOBJ
	if(!foo && !allowall && !allowgold && !allownone) {
#else
	if(!foo && !allowall && !allownone) {
#endif
		You("VERB_HABEN nichts %szu MODIFIER_VERB_INFINITIV %s.", /* EN You("don't have anything %sto %s.", */
			foox ? "anderes " : "", word); /* EN foox ? "else " : "", word); */
		return((struct obj *)0);
	}
	for(;;) {
		cnt = 0;
		if (allowcnt == 2) allowcnt = 1;  /* abort previous count */
		if(!buf[0]) {
			Sprintf(qbuf, "%s MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL MODIFIER_VERB_INFINITIV %s? [*]", fragewort, word); /* EN Sprintf(qbuf, "What do you want to %s? [*]", word); */
		} else {
			Sprintf(qbuf, "%s MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL MODIFIER_VERB_INFINITIV %s? [%s oder ?*]", fragewort, /* EN Sprintf(qbuf, "What do you want to %s? [%s or ?*]", */
				word, buf);
		}
#ifdef REDO
		if (in_doagain)
		    ilet = readchar();
		else
#endif
		    ilet = yn_function(qbuf, (char *)0, '\0');
		if(ilet == '0') prezero = TRUE;
		while(digit(ilet) && allowcnt) {
#ifdef REDO
			if (ilet != '?' && ilet != '*')	savech(ilet);
#endif
			cnt = 10*cnt + (ilet - '0');
			allowcnt = 2;	/* signal presence of cnt */
			ilet = readchar();
		}
		if(digit(ilet)) {
			pline("No count allowed with this command."); /* EN pline("No count allowed with this command."); */ // TODO DE
			continue;
		}
		if(index(quitchars,ilet)) {
		    if(flags.verbose)
			pline(Never_mind);
		    return((struct obj *)0);
		}
		if(ilet == '-') {
			return(allownone ? &zeroobj : (struct obj *) 0);
		}
		if(ilet == def_oc_syms[COIN_CLASS]) {
			if (!usegold) {
			    if (!strncmp(word, "rub on ", 7)) { /* DE egal, beide S�tze funktionieren im Deutschen */
				/* the dangers of building sentences... */
				You("VERB_KOENNEN kein Gold MODIFIER_VERB_INFINITIV %s.", word); /* EN You("cannot rub gold%s.", word + 3); */
			    } else {
				You("VERB_KOENNEN kein Gold MODIFIER_VERB_INFINITIV %s.", word); /* EN You("cannot %s gold.", word); */
			    }
			    return(struct obj *)0;
#ifndef GOLDOBJ
			} else if (!allowgold) {
				You("VERB_HABEN kein Gold OBJECT KASUS_DATIV bei PRONOMEN_PERSONAL."); /* EN You("are not carrying any gold."); */
				return(struct obj *)0;
#endif
			} 
			if(cnt == 0 && prezero) return((struct obj *)0);
			/* Historic note: early Nethack had a bug which was
			 * first reported for Larn, where trying to drop 2^32-n
			 * gold pieces was allowed, and did interesting things
			 * to your money supply.  The LRS is the tax bureau
			 * from Larn.
			 */
			if(cnt < 0) {
	pline("Die LRS w�rde sehr gerne erfahren, dass SUBJECT_IM_SATZ PRONOMEN_PERSONAL soviel Gold VERB_BESITZEN."); /* EN pline_The("LRS would be very interested to know you have that much."); */
				return(struct obj *)0;
			}

#ifndef GOLDOBJ
			if(!(allowcnt == 2 && cnt < u.ugold))
				cnt = u.ugold;
			return(mkgoldobj(cnt));
#endif
		}
		if(ilet == '?' || ilet == '*') {
		    char *allowed_choices = (ilet == '?') ? lets : (char *)0;
		    long ctmp = 0;

		    if (ilet == '?' && !*lets && *altlets)
			allowed_choices = altlets;
		    ilet = display_pickinv(allowed_choices, TRUE,
					   allowcnt ? &ctmp : (long *)0);
		    if(!ilet) continue;
		    if (allowcnt && ctmp >= 0) {
			cnt = ctmp;
			if (!cnt) prezero = TRUE;
			allowcnt = 2;
		    }
		    if(ilet == '\033') {
			if(flags.verbose)
			    pline(Never_mind);
			return((struct obj *)0);
		    }
		    /* they typed a letter (not a space) at the prompt */
		}
		if(allowcnt == 2 && !strcmp(word,"VERB_WERFEN")) { /* EN if(allowcnt == 2 && !strcmp(word,"throw")) { */
		    /* permit counts for throwing gold, but don't accept
		     * counts for other things since the throw code will
		     * split off a single item anyway */
#ifdef GOLDOBJ
		    if (ilet != def_oc_syms[COIN_CLASS])
#endif
			allowcnt = 1;
		    if(cnt == 0 && prezero) return((struct obj *)0);
		    if(cnt > 1) {
			You("VERB_KOENNEN davon nur eins aufs Mal werfen."); /* EN You("can only throw one item at a time."); */
			continue;
		    }
		}
#ifdef GOLDOBJ
		flags.botl = 1; /* May have changed the amount of money */
#endif
#ifdef REDO
		savech(ilet);
#endif
		for (otmp = invent; otmp; otmp = otmp->nobj)
			if (otmp->invlet == ilet) break;
		if(!otmp) {
			You("VERB_HABEN dieses Objekt nicht."); /* EN You("don't have that object."); */
#ifdef REDO
			if (in_doagain) return((struct obj *) 0);
#endif
			continue;
		} else if (cnt < 0 || otmp->quan < cnt) {
			pline("Soviel VERB_HABEN PRONOMEN_PERSONAL nicht!  NEUER_SATZ SUBJECT PRONOMEN_PERSONAL VERB_HABEN nur %ld.", /* EN You("don't have that many!  You have only %ld.", */
			    otmp->quan);
#ifdef REDO
			if (in_doagain) return((struct obj *) 0);
#endif
			continue;
		}
		break;
	}
	if(!allowall && let && !index(let,otmp->oclass)
#ifdef GOLDOBJ
	   && !(usegold && otmp->oclass == COIN_CLASS)
#endif
	   ) {
		silly_thing(word, otmp);
		return((struct obj *)0);
	}
	if(allowcnt == 2) {	/* cnt given */
	    if(cnt == 0) return (struct obj *)0;
	    if(cnt != otmp->quan) {
		/* don't split a stack of cursed loadstones */
		if (otmp->otyp == LOADSTONE && otmp->cursed)
		    /* kludge for canletgo()'s can't-drop-this message */
		    otmp->corpsenm = (int) cnt;
		else
		otmp = splitobj(otmp, cnt);
	    }
	}
	return(otmp);
}

void
silly_thing(word, otmp)
const char *word;
struct obj *otmp;
{
	const char *s1, *s2, *s3, *what;
	int ocls = otmp->oclass, otyp = otmp->otyp;

	s1 = s2 = s3 = 0;
	/* check for attempted use of accessory commands ('P','R') on armor
	   and for corresponding armor commands ('W','T') on accessories */
	if (ocls == ARMOR_CLASS) {
	    if (!strcmp(word, "put on")) /* EN if (!strcmp(word, "put on")) */ // TODO DE
		s1 = "W", s2 = "zu tragen", s3 = ""; /* EN s1 = "W", s2 = "wear", s3 = ""; */
	    else if (!strcmp(word, "remove")) /* EN else if (!strcmp(word, "remove")) */ // TODO DE
		s1 = "T", s2 = "take", s3 = " off"; /* EN s1 = "T", s2 = "take", s3 = " off"; */ // TODO DE
	} else if ((ocls == RING_CLASS || otyp == MEAT_RING) ||
		ocls == AMULET_CLASS ||
		(otyp == BLINDFOLD || otyp == TOWEL || otyp == LENSES)) {
	    if (!strcmp(word, "VERB_TRAGEN")) /* EN if (!strcmp(word, "wear")) */
		s1 = "P", s2 = "anzuziehen", s3 = ""; /* EN s1 = "P", s2 = "put", s3 = " on"; */ // TODO DE
	    else if (!strcmp(word, "take off")) /* EN else if (!strcmp(word, "take off")) */ // TODO DE
		s1 = "R", s2 = "remove", s3 = ""; /* EN s1 = "R", s2 = "remove", s3 = ""; */ // TODO DE
	}
	if (s1) {
	    what = "das"; /* EN what = "that"; */
	    /* quantity for armor and accessory objects is always 1,
	       but some things should be referred to as plural */
	    if (otyp == LENSES || is_gloves(otmp) || is_boots(otmp))
		what = "die"; /* EN what = "those"; */
			pline("SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_BENUTZEN den '%s'-Befehl um %s %s%s.", s1, what, s2, s3); /* EN pline("Use the '%s' command to %s %s%s.", s1, s2, what, s3); */
	} else {
	    pline(silly_thing_to, word);
	}
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_PTR int
ckvalidcat(otmp)
register struct obj *otmp;
{
	/* use allow_category() from pickup.c */
	return((int)allow_category(otmp));
}

STATIC_PTR int
ckunpaid(otmp)
register struct obj *otmp;
{
	return((int)(otmp->unpaid));
}

boolean
wearing_armor()
{
	return((boolean)(uarm || uarmc || uarmf || uarmg || uarmh || uarms
#ifdef TOURIST
		|| uarmu
#endif
		));
}

boolean
is_worn(otmp)
register struct obj *otmp;
{
    return((boolean)(!!(otmp->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL |
#ifdef STEED
			W_SADDLE |
#endif
			W_WEP | W_SWAPWEP | W_QUIVER))));
}

static NEARDATA const char removeables[] =
	{ ARMOR_CLASS, WEAPON_CLASS, RING_CLASS, AMULET_CLASS, TOOL_CLASS, 0 };

/* interactive version of getobj - used for Drop, Identify and */
/* Takeoff (A). Return the number of times fn was called successfully */
/* If combo is TRUE, we just use this to get a category list */
int
ggetobj(word, fn, mx, combo, resultflags)
const char *word;
int FDECL((*fn),(OBJ_P)), mx;
boolean combo;		/* combination menu flag */
unsigned *resultflags;
{
	int FDECL((*ckfn),(OBJ_P)) = (int FDECL((*),(OBJ_P))) 0;
	boolean FDECL((*filter),(OBJ_P)) = (boolean FDECL((*),(OBJ_P))) 0;
	boolean takeoff, ident, allflag, m_seen;
	int itemcount;
#ifndef GOLDOBJ
	int oletct, iletct, allowgold, unpaid, oc_of_sym;
#else
	int oletct, iletct, unpaid, oc_of_sym;
#endif
	char sym, *ip, olets[MAXOCLASSES+5], ilets[MAXOCLASSES+5];
	char extra_removeables[3+1];	/* uwep,uswapwep,uquiver */
	char buf[BUFSZ], qbuf[QBUFSZ];

	if (resultflags) *resultflags = 0;
#ifndef GOLDOBJ
	allowgold = (u.ugold && !strcmp(word, "VERB_DROP")) ? 1 : 0; /* EN allowgold = (u.ugold && !strcmp(word, "drop")) ? 1 : 0; */
#endif
	takeoff = ident = allflag = m_seen = FALSE;
#ifndef GOLDOBJ
	if(!invent && !allowgold){
#else
	if(!invent){
#endif
		You("VERB_HABEN nichts zu MODIFIER_VERB_INFINITIV %s.", word); /* EN You("have nothing to %s.", word); */
		return(0);
	}
	add_valid_menu_class(0);	/* reset */
	if (taking_off(word)) {
	    takeoff = TRUE;
	    filter = is_worn;
	} else if (!strcmp(word, "VERB_IDENTIFY")) { /* EN } else if (!strcmp(word, "identify")) { */
	    ident = TRUE;
	    filter = not_fully_identified;
	}

	iletct = collect_obj_classes(ilets, invent,
				     	FALSE,
#ifndef GOLDOBJ
					(allowgold != 0),
#endif
					filter, &itemcount);
	unpaid = count_unpaid(invent);

	if (ident && !iletct) {
	    return -1;		/* no further identifications */
	} else if (!takeoff && (unpaid || invent)) {
	    ilets[iletct++] = ' ';
	    if (unpaid) ilets[iletct++] = 'u';
	    if (count_buc(invent, BUC_BLESSED))  ilets[iletct++] = 'B';
	    if (count_buc(invent, BUC_UNCURSED)) ilets[iletct++] = 'U';
	    if (count_buc(invent, BUC_CURSED))   ilets[iletct++] = 'C';
	    if (count_buc(invent, BUC_UNKNOWN))  ilets[iletct++] = 'X';
	    if (invent) ilets[iletct++] = 'a';
	} else if (takeoff && invent) {
	    ilets[iletct++] = ' ';
	}
	ilets[iletct++] = 'i';
	if (!combo)
	    ilets[iletct++] = 'm';	/* allow menu presentation on request */
	ilets[iletct] = '\0';

	for (;;) {
	    Sprintf(qbuf,"What kinds of thing do you want to %s? [%s]", /* EN Sprintf(qbuf,"What kinds of thing do you want to %s? [%s]", */ // TODO DE
		    word, ilets);
	    getlin(qbuf, buf);
	    if (buf[0] == '\033') return(0);
	    if (index(buf, 'i')) {
		if (display_inventory((char *)0, TRUE) == '\033') return 0;
	    } else
		break;
	}

	extra_removeables[0] = '\0';
	if (takeoff) {
	    /* arbitrary types of items can be placed in the weapon slots
	       [any duplicate entries in extra_removeables[] won't matter] */
	    if (uwep) (void)strkitten(extra_removeables, uwep->oclass);
	    if (uswapwep) (void)strkitten(extra_removeables, uswapwep->oclass);
	    if (uquiver) (void)strkitten(extra_removeables, uquiver->oclass);
	}

	ip = buf;
	olets[oletct = 0] = '\0';
	while ((sym = *ip++) != '\0') {
	    if (sym == ' ') continue;
	    oc_of_sym = def_char_to_objclass(sym);
	    if (takeoff && oc_of_sym != MAXOCLASSES) {
		if (index(extra_removeables, oc_of_sym)) {
		    ;	/* skip rest of takeoff checks */
		} else if (!index(removeables, oc_of_sym)) {
		    pline("Nicht zutreffend."); /* EN pline("Not applicable."); */
		    return 0;
		} else if (oc_of_sym == ARMOR_CLASS && !wearing_armor()) {
		    You("VERB_TRAGEN keine R�stung."); /* EN You("are not wearing any armor."); */
		    return 0;
		} else if (oc_of_sym == WEAPON_CLASS &&
			!uwep && !uswapwep && !uquiver) {
		    You("are not wielding anything."); /* EN You("are not wielding anything."); */ // TODO DE
		    return 0;
		} else if (oc_of_sym == RING_CLASS && !uright && !uleft) {
		    You("VERB_TRAGEN keine Ringe."); /* EN You("are not wearing rings."); */
		    return 0;
		} else if (oc_of_sym == AMULET_CLASS && !uamul) {
		    You("VERB_TRAGEN kein Amulett."); /* EN You("are not wearing an amulet."); */
		    return 0;
		} else if (oc_of_sym == TOOL_CLASS && !ublindf) {
		    You("VERB_TRAGEN OBJECT PRONOMEN_KEIN NOUN_BLINDFOLD."); /* EN You("are not wearing a blindfold."); */
		    return 0;
		}
	    }

	    if (oc_of_sym == COIN_CLASS && !combo) {
#ifndef GOLDOBJ
		if (allowgold == 1)
		    (*fn)(mkgoldobj(u.ugold));
		else if (!u.ugold)
		    You("VERB_HAVE kein Gold."); /* EN You("have no gold."); */
		allowgold = 2;
#else
		flags.botl = 1;
#endif
	    } else if (sym == 'a') {
		allflag = TRUE;
	    } else if (sym == 'A') {
		/* same as the default */ ;
	    } else if (sym == 'u') {
		add_valid_menu_class('u');
		ckfn = ckunpaid;
	    } else if (sym == 'B') {
	    	add_valid_menu_class('B');
	    	ckfn = ckvalidcat;
	    } else if (sym == 'U') {
	    	add_valid_menu_class('U');
	    	ckfn = ckvalidcat;
	    } else if (sym == 'C') {
	    	add_valid_menu_class('C');
		ckfn = ckvalidcat;
	    } else if (sym == 'X') {
	    	add_valid_menu_class('X');
		ckfn = ckvalidcat;
	    } else if (sym == 'm') {
		m_seen = TRUE;
	    } else if (oc_of_sym == MAXOCLASSES) {
		You("VERB_HABEN keine %cs.", sym); /* EN You("don't have any %c's.", sym); */
	    } else if (oc_of_sym != VENOM_CLASS) {	/* suppress venom */
		if (!index(olets, oc_of_sym)) {
		    add_valid_menu_class(oc_of_sym);
		    olets[oletct++] = oc_of_sym;
		    olets[oletct] = 0;
		}
	    }
	}

	if (m_seen)
	    return (allflag || (!oletct && ckfn != ckunpaid)) ? -2 : -3;
	else if (flags.menu_style != MENU_TRADITIONAL && combo && !allflag)
	    return 0;
#ifndef GOLDOBJ
	else if (allowgold == 2 && !oletct)
	    return 1;	/* you dropped gold (or at least tried to) */
	else {
#else
	else /*!!!! if (allowgold == 2 && !oletct)
	    !!!! return 1;	 you dropped gold (or at least tried to) 
            !!!! test gold dropping
	else*/ {
#endif
	    int cnt = askchain(&invent, olets, allflag, fn, ckfn, mx, word); 
	    /*
	     * askchain() has already finished the job in this case
	     * so set a special flag to convey that back to the caller
	     * so that it won't continue processing.
	     * Fix for bug C331-1 reported by Irina Rempt-Drijfhout. 
	     */
	    if (combo && allflag && resultflags)
		*resultflags |= ALL_FINISHED; 
	    return cnt;
	}
}

/*
 * Walk through the chain starting at objchn and ask for all objects
 * with olet in olets (if nonNULL) and satisfying ckfn (if nonnull)
 * whether the action in question (i.e., fn) has to be performed.
 * If allflag then no questions are asked. Max gives the max nr of
 * objects to be treated. Return the number of objects treated.
 */
int
askchain(objchn, olets, allflag, fn, ckfn, mx, word)
struct obj **objchn;
register int allflag, mx;
register const char *olets, *word;	/* olets is an Obj Class char array */
register int FDECL((*fn),(OBJ_P)), FDECL((*ckfn),(OBJ_P));
{
	struct obj *otmp, *otmp2, *otmpo;
	register char sym, ilet;
	register int cnt = 0, dud = 0, tmp;
	boolean takeoff, nodot, ident, ininv;
	char qbuf[QBUFSZ];

	takeoff = taking_off(word);
	ident = !strcmp(word, "VERB_IDENTIFY"); /* EN ident = !strcmp(word, "identify"); */
	nodot = (!strcmp(word, "nodot") || !strcmp(word, "VERB_DROP") || /* EN nodot = (!strcmp(word, "nodot") || !strcmp(word, "drop") || */
		 ident || takeoff);
	ininv = (*objchn == invent);
	/* Changed so the askchain is interrogated in the order specified.
	 * For example, if a person specifies =/ then first all rings will be
	 * asked about followed by all wands -dgk
	 */
nextclass:
	ilet = 'a'-1;
	if (*objchn && (*objchn)->oclass == COIN_CLASS)
		ilet--;		/* extra iteration */
	for (otmp = *objchn; otmp; otmp = otmp2) {
		if(ilet == 'z') ilet = 'A'; else ilet++;
		otmp2 = otmp->nobj;
		if (olets && *olets && otmp->oclass != *olets) continue;
		if (takeoff && !is_worn(otmp)) continue;
		if (ident && !not_fully_identified(otmp)) continue;
		if (ckfn && !(*ckfn)(otmp)) continue;
		if (!allflag) {
			Strcpy(qbuf, !ininv ? doname(otmp) :
				xprname(otmp, (char *)0, ilet, !nodot, 0L, 0L));
			Strcat(qbuf, "?");
			sym = (takeoff || ident || otmp->quan < 2L) ?
				nyaq(qbuf) : nyNaq(qbuf);
		}
		else	sym = 'y';

		otmpo = otmp;
		if (sym == '#') {
		 /* Number was entered; split the object unless it corresponds
		    to 'none' or 'all'.  2 special cases: cursed loadstones and
		    welded weapons (eg, multiple daggers) will remain as merged
		    unit; done to avoid splitting an object that won't be
		    droppable (even if we're picking up rather than dropping).
		  */
		    if (!yn_number)
			sym = 'n';
		    else {
			sym = 'y';
			if (yn_number < otmp->quan && !welded(otmp) &&
			    (!otmp->cursed || otmp->otyp != LOADSTONE)) {
			    otmp = splitobj(otmp, yn_number);
			}
		    }
		}
		switch(sym){
		case 'a':
			allflag = 1;
		case 'y':
			tmp = (*fn)(otmp);
			if(tmp < 0) {
			    if (otmp != otmpo) {
				/* split occurred, merge again */
				(void) merged(&otmpo, &otmp);
			    }
			    goto ret;
			}
			cnt += tmp;
			if(--mx == 0) goto ret;
		case 'n':
			if(nodot) dud++;
		default:
			break;
		case 'q':
			/* special case for seffects() */
			if (ident) cnt = -1;
			goto ret;
		}
	}
	if (olets && *olets && *++olets)
		goto nextclass;
	if(!takeoff && (dud || cnt)) pline("Das war's."); /* EN if(!takeoff && (dud || cnt)) pline("That was all."); */
	else if(!dud && !cnt) pline("Keine zutreffenden Objekte."); /* EN else if(!dud && !cnt) pline("No applicable objects."); */
ret:
	return(cnt);
}


/*
 *	Object identification routines:
 */

/* make an object actually be identified; no display updating */
void
fully_identify_obj(otmp)
struct obj *otmp;
{
    makeknown(otmp->otyp);
    if (otmp->oartifact) discover_artifact((xchar)otmp->oartifact);
    otmp->known = otmp->dknown = otmp->bknown = otmp->rknown = 1;
    if (otmp->otyp == EGG && otmp->corpsenm != NON_PM)
	learn_egg_type(otmp->corpsenm);
}

/* ggetobj callback routine; identify an object and give immediate feedback */
int
identify(otmp)
struct obj *otmp;
{
    fully_identify_obj(otmp);
    prinv((char *)0, otmp, 0L);
    return 1;
}

/* menu of unidentified objects; select and identify up to id_limit of them */
STATIC_OVL void
menu_identify(id_limit)
int id_limit;
{
    menu_item *pick_list;
    int n, i, first = 1;
    char buf[BUFSZ];
    /* assumptions:  id_limit > 0 and at least one unID'd item is present */

    while (id_limit) {
	Sprintf(buf, "SUBJECT Was MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL %s identifizieren?", /* EN Sprintf(buf, "What would you like to identify %s?", */
		first ? "zuerst" : "als n�chstes"); /* EN first ? "first" : "next"); */
	n = query_objlist(buf, invent, SIGNAL_NOMENU|USE_INVLET|INVORDER_SORT,
		&pick_list, PICK_ANY, not_fully_identified);

	if (n > 0) {
	    if (n > id_limit) n = id_limit;
	    for (i = 0; i < n; i++, id_limit--)
		(void) identify(pick_list[i].item.a_obj);
	    free((genericptr_t) pick_list);
	    mark_synch(); /* Before we loop to pop open another menu */
	} else {
	    if (n < 0) pline("Das war's."); /* EN if (n < 0) pline("That was all."); */
	    id_limit = 0; /* Stop now */
	}
	first = 0;
    }
}

/* dialog with user to identify a given number of items; 0 means all */
void
identify_pack(id_limit)
int id_limit;
{
    struct obj *obj, *the_obj;
    int n, unid_cnt;

    unid_cnt = 0;
    the_obj = 0;		/* if unid_cnt ends up 1, this will be it */
    for (obj = invent; obj; obj = obj->nobj)
	if (not_fully_identified(obj)) ++unid_cnt, the_obj = obj;

    if (!unid_cnt) {
	You("VERB_HABEN schon OBJECT PRONOMEN_POSSESSIV ADJEKTIV_GESAMT NOUN_HABE identifiziert."); /* EN You("have already identified all of your possessions."); */
    } else if (!id_limit) {
	/* identify everything */
	if (unid_cnt == 1) {
	    (void) identify(the_obj);
	} else {

	    /* TODO:  use fully_identify_obj and cornline/menu/whatever here */
	    for (obj = invent; obj; obj = obj->nobj)
		if (not_fully_identified(obj)) (void) identify(obj);

	}
    } else {
	/* identify up to `id_limit' items */
	n = 0;
	if (flags.menu_style == MENU_TRADITIONAL)
	    do {
		n = ggetobj("VERB_IDENTIFY", identify, id_limit, FALSE, (unsigned *)0); /* EN n = ggetobj("identify", identify, id_limit, FALSE, (unsigned *)0); */
		if (n < 0) break; /* quit or no eligible items */
	    } while ((id_limit -= n) > 0);
	if (n == 0 || n < -1)
	    menu_identify(id_limit);
    }
    update_inventory();
}

#endif /* OVLB */
#ifdef OVL2

STATIC_OVL char
obj_to_let(obj)	/* should of course only be called for things in invent */
register struct obj *obj;
{
#ifndef GOLDOBJ
	if (obj->oclass == COIN_CLASS)
		return GOLD_SYM;
#endif
	if (!flags.invlet_constant) {
		obj->invlet = NOINVSYM;
		reassign();
	}
	return obj->invlet;
}

/*
 * Print the indicated quantity of the given object.  If quan == 0L then use
 * the current quantity.
 */
void
prinv(prefix, obj, quan)
const char *prefix;
register struct obj *obj;
long quan;
{
	if (!prefix) prefix = "";
	pline("%s%s%s",
	      prefix, *prefix ? " " : "",
	      xprname(obj, (char *)0, obj_to_let(obj), TRUE, 0L, quan));
}

#endif /* OVL2 */
#ifdef OVL1

char *
xprname(obj, txt, let, dot, cost, quan)
struct obj *obj;
const char *txt;	/* text to print instead of obj */
char let;		/* inventory letter */
boolean dot;		/* append period; (dot && cost => Iu) */
long cost;		/* cost (for inventory of unpaid or expended items) */
long quan;		/* if non-0, print this quantity, not obj->quan */
{
#ifdef LINT	/* handle static char li[BUFSZ]; */
    char li[BUFSZ];
#else
    static char li[BUFSZ];
#endif
    boolean use_invlet = flags.invlet_constant && let != CONTAINED_SYM;
    long savequan = 0;

    if (quan && obj) {
	savequan = obj->quan;
	obj->quan = quan;
    }

    /*
     * If let is:
     *	*  Then obj == null and we are printing a total amount.
     *	>  Then the object is contained and doesn't have an inventory letter.
     */
    if (cost != 0 || let == '*') {
	/* if dot is true, we're doing Iu, otherwise Ix */
	Sprintf(li, "%c - %-45s %6ld %s",
		(dot && use_invlet ? obj->invlet : let),
		(txt ? german(txt) : german(doname(obj))), cost, currency(cost)); /* EN (txt ? txt : doname(obj)), cost, currency(cost)); */
#ifndef GOLDOBJ
    } else if (obj && obj->oclass == COIN_CLASS) {
	Sprintf(li, "%ld NOUN_GOLD_PIECE%s%s", obj->quan, plur(obj->quan), /* EN Sprintf(li, "%ld gold piece%s%s", obj->quan, plur(obj->quan), */
		(dot ? "." : ""));
#endif
    } else {
	/* ordinary inventory display or pickup message */
	Sprintf(li, "%c - %s%s",
		(use_invlet ? obj->invlet : let),
		(txt ? txt : german(doname(obj))), (dot ? "." : "")); /* EN (txt ? txt : doname(obj)), (dot ? "." : "")); */
    }
    if (savequan) obj->quan = savequan;

		return li;
}

#endif /* OVL1 */
#ifdef OVLB

/* the 'i' command */
int
ddoinv()
{
	(void) display_inventory((char *)0, FALSE);
	return 0;
}

/*
 * find_unpaid()
 *
 * Scan the given list of objects.  If last_found is NULL, return the first
 * unpaid object found.  If last_found is not NULL, then skip over unpaid
 * objects until last_found is reached, then set last_found to NULL so the
 * next unpaid object is returned.  This routine recursively follows
 * containers.
 */
STATIC_OVL struct obj *
find_unpaid(list, last_found)
    struct obj *list, **last_found;
{
    struct obj *obj;

    while (list) {
	if (list->unpaid) {
	    if (*last_found) {
		/* still looking for previous unpaid object */
		if (list == *last_found)
		    *last_found = (struct obj *) 0;
	    } else
		return (*last_found = list);
	}
	if (Has_contents(list)) {
	    if ((obj = find_unpaid(list->cobj, last_found)) != 0)
		return obj;
	}
	list = list->nobj;
    }
    return (struct obj *) 0;
}

/*
 * Internal function used by display_inventory and getobj that can display
 * inventory and return a count as well as a letter. If out_cnt is not null,
 * any count returned from the menu selection is placed here.
 */
static char
display_pickinv(lets, want_reply, out_cnt)
register const char *lets;
boolean want_reply;
long* out_cnt;
{
	struct obj *otmp;
	char ilet, ret;
	char *invlet = flags.inv_order;
	int n, classcount;
	winid win;				/* windows being used */
	static winid local_win = WIN_ERR;	/* window for partial menus */
	anything any;
	menu_item *selected;

	/* overriden by global flag */
	if (flags.perm_invent) {
	    win = (lets && *lets) ? local_win : WIN_INVEN;
	    /* create the first time used */
	    if (win == WIN_ERR)
		win = local_win = create_nhwindow(NHW_MENU);
	} else
	    win = WIN_INVEN;

	/*
	Exit early if no inventory -- but keep going if we are doing
	a permanent inventory update.  We need to keep going so the
	permanent inventory window updates itself to remove the last
	item(s) dropped.  One down side:  the addition of the exception
	for permanent inventory window updates _can_ pop the window
	up when it's not displayed -- even if it's empty -- because we
	don't know at this level if its up or not.  This may not be
	an issue if empty checks are done before hand and the call
	to here is short circuited away.
	*/
	if (!invent && !(flags.perm_invent && !lets && !want_reply)) {
#ifndef GOLDOBJ
	    pline("Not carrying anything%s.", u.ugold ? " ausgenommen Gold" : ""); /* EN pline("Not carrying anything%s.", u.ugold ? " except gold" : ""); */ // TODO DE
#else
	    pline("Not carrying anything."); /* EN pline("Not carrying anything."); */ // TODO DE
#endif
	    return 0;
	}

	/* oxymoron? temporarily assign permanent inventory letters */
	if (!flags.invlet_constant) reassign();

	if (lets && strlen(lets) == 1) {
	    /* when only one item of interest, use pline instead of menus;
	       we actually use a fake message-line menu in order to allow
	       the user to perform selection at the --More-- prompt for tty */
	    ret = '\0';
	    for (otmp = invent; otmp; otmp = otmp->nobj) {
		if (otmp->invlet == lets[0]) {
		    ret = message_menu(lets[0],
			  want_reply ? PICK_ONE : PICK_NONE,
			  xprname(otmp, (char *)0, lets[0], TRUE, 0L, 0L));
		    if (out_cnt) *out_cnt = -1L;	/* select all */
		    break;
		}
	    }
	    return ret;
	}

	start_menu(win);
nextclass:
	classcount = 0;
	any.a_void = 0;		/* set all bits to zero */
	for(otmp = invent; otmp; otmp = otmp->nobj) {
		ilet = otmp->invlet;
		if(!lets || !*lets || index(lets, ilet)) {
			if (!flags.sortpack || otmp->oclass == *invlet) {
			    if (flags.sortpack && !classcount) {
				any.a_void = 0;		/* zero */
				add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
				    let_to_name(*invlet, FALSE), MENU_UNSELECTED);
				classcount++;
			    }
			    any.a_char = ilet;
			    add_menu(win, obj_to_glyph(otmp),
					&any, ilet, 0, ATR_NONE, doname(otmp),
					MENU_UNSELECTED);
			}
		}
	}
	if (flags.sortpack) {
		if (*++invlet) goto nextclass;
#ifdef WIZARD
		if (--invlet != venom_inv) {
			invlet = venom_inv;
			goto nextclass;
		}
#endif
	}
	end_menu(win, (char *) 0);

	n = select_menu(win, want_reply ? PICK_ONE : PICK_NONE, &selected);
	if (n > 0) {
	    ret = selected[0].item.a_char;
	    if (out_cnt) *out_cnt = selected[0].count;
	    free((genericptr_t)selected);
	} else
	    ret = !n ? '\0' : '\033';	/* cancelled */

	return ret;
}

/*
 * If lets == NULL or "", list all objects in the inventory.  Otherwise,
 * list all objects with object classes that match the order in lets.
 *
 * Returns the letter identifier of a selected item, or 0 if nothing
 * was selected.
 */
char
display_inventory(lets, want_reply)
register const char *lets;
boolean want_reply;
{
	return display_pickinv(lets, want_reply, (long *)0);
}

/*
 * Returns the number of unpaid items within the given list.  This includes
 * contained objects.
 */
int
count_unpaid(list)
    struct obj *list;
{
    int count = 0;

    while (list) {
	if (list->unpaid) count++;
	if (Has_contents(list))
	    count += count_unpaid(list->cobj);
	list = list->nobj;
    }
    return count;
}

/*
 * Returns the number of items with b/u/c/unknown within the given list.  
 * This does NOT include contained objects.
 */
int
count_buc(list, type)
    struct obj *list;
    int type;
{
    int count = 0;

    while (list) {
	if (Role_if(PM_PRIEST)) list->bknown = TRUE;
	switch(type) {
	    case BUC_BLESSED:
		if (list->oclass != COIN_CLASS && list->bknown && list->blessed)
		    count++;
		break;
	    case BUC_CURSED:
		if (list->oclass != COIN_CLASS && list->bknown && list->cursed)
		    count++;
		break;
	    case BUC_UNCURSED:
		if (list->oclass != COIN_CLASS &&
			list->bknown && !list->blessed && !list->cursed)
		    count++;
		break;
	    case BUC_UNKNOWN:
		if (list->oclass != COIN_CLASS && !list->bknown)
		    count++;
		break;
	    default:
		impossible("need count of curse status %d?", type);
		return 0;
	}
	list = list->nobj;
    }
    return count;
}

STATIC_OVL void
dounpaid()
{
    winid win;
    struct obj *otmp, *marker;
    register char ilet;
    char *invlet = flags.inv_order;
    int classcount, count, num_so_far;
    int save_unpaid = 0;	/* lint init */
    long cost, totcost;

    count = count_unpaid(invent);

    if (count == 1) {
	marker = (struct obj *) 0;
	otmp = find_unpaid(invent, &marker);

	/* see if the unpaid item is in the top level inventory */
	for (marker = invent; marker; marker = marker->nobj)
	    if (marker == otmp) break;

	pline("%s", xprname(otmp, distant_name(otmp, doname),
			    marker ? otmp->invlet : CONTAINED_SYM,
			    TRUE, unpaid_cost(otmp), 0L));
	return;
    }

    win = create_nhwindow(NHW_MENU);
    cost = totcost = 0;
    num_so_far = 0;	/* count of # printed so far */
    if (!flags.invlet_constant) reassign();

    do {
	classcount = 0;
	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    ilet = otmp->invlet;
	    if (otmp->unpaid) {
		if (!flags.sortpack || otmp->oclass == *invlet) {
		    if (flags.sortpack && !classcount) {
			putstr(win, 0, let_to_name(*invlet, TRUE));
			classcount++;
		    }

		    totcost += cost = unpaid_cost(otmp);
		    /* suppress "(unpaid)" suffix */
		    save_unpaid = otmp->unpaid;
		    otmp->unpaid = 0;
		    putstr(win, 0, xprname(otmp, distant_name(otmp, doname),
					   ilet, TRUE, cost, 0L));
		    otmp->unpaid = save_unpaid;
		    num_so_far++;
		}
	    }
	}
    } while (flags.sortpack && (*++invlet));

    if (count > num_so_far) {
	/* something unpaid is contained */
	if (flags.sortpack)
	    putstr(win, 0, let_to_name(CONTAINED_SYM, TRUE));
	/*
	 * Search through the container objects in the inventory for
	 * unpaid items.  The top level inventory items have already
	 * been listed.
	 */
	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if (Has_contents(otmp)) {
		marker = (struct obj *) 0;	/* haven't found any */
		while (find_unpaid(otmp->cobj, &marker)) {
		    totcost += cost = unpaid_cost(marker);
		    save_unpaid = marker->unpaid;
		    marker->unpaid = 0;    /* suppress "(unpaid)" suffix */
		    putstr(win, 0,
			   xprname(marker, distant_name(marker, doname),
				   CONTAINED_SYM, TRUE, cost, 0L));
		    marker->unpaid = save_unpaid;
		}
	    }
	}
    }

    putstr(win, 0, "");
    putstr(win, 0, xprname((struct obj *)0, "Total:", '*', FALSE, totcost, 0L));
    display_nhwindow(win, FALSE);
    destroy_nhwindow(win);
}


/* query objlist callback: return TRUE if obj type matches "this_type" */
static int this_type;

STATIC_OVL boolean
this_type_only(obj)
    struct obj *obj;
{
    return (obj->oclass == this_type);
}

/* the 'I' command */
int
dotypeinv()
{
	char c = '\0';
	int n, i = 0;
	char *extra_types, types[BUFSZ];
	int class_count, oclass, unpaid_count, itemcount;
	boolean billx = *u.ushops && doinvbill(0);
	menu_item *pick_list;
	boolean traditional = TRUE;
	const char *prompt = "What type of object do you want an inventory of?"; /* EN const char *prompt = "What type of object do you want an inventory of?"; */ // TODO DE

#ifndef GOLDOBJ
	if (!invent && !u.ugold && !billx) {
#else
	if (!invent && !billx) {
#endif
	    You("aren't carrying anything."); /* EN You("aren't carrying anything."); */ // TODO DE
	    return 0;
	}
	unpaid_count = count_unpaid(invent);
	if (flags.menu_style != MENU_TRADITIONAL) {
	    if (flags.menu_style == MENU_FULL ||
				flags.menu_style == MENU_PARTIAL) {
		traditional = FALSE;
		i = UNPAID_TYPES;
		if (billx) i |= BILLED_TYPES;
		n = query_category(prompt, invent, i, &pick_list, PICK_ONE);
		if (!n) return 0;
		this_type = c = pick_list[0].item.a_int;
		free((genericptr_t) pick_list);
	    }
	}
	if (traditional) {
	    /* collect a list of classes of objects carried, for use as a prompt */
	    types[0] = 0;
	    class_count = collect_obj_classes(types, invent,
					      FALSE,
#ifndef GOLDOBJ
					      (u.ugold != 0),
#endif
					      (boolean FDECL((*),(OBJ_P))) 0, &itemcount);
	    if (unpaid_count) {
		Strcat(types, "u");
		class_count++;
	    }
	    if (billx) {
		Strcat(types, "x");
		class_count++;
	    }
	    /* add everything not already included; user won't see these */
	    extra_types = eos(types);
	    *extra_types++ = '\033';
	    if (!unpaid_count) *extra_types++ = 'u';
	    if (!billx) *extra_types++ = 'x';
	    *extra_types = '\0';	/* for index() */
	    for (i = 0; i < MAXOCLASSES; i++)
		if (!index(types, def_oc_syms[i])) {
		    *extra_types++ = def_oc_syms[i];
		    *extra_types = '\0';
		}

	    if(class_count > 1) {
		c = yn_function(prompt, types, '\0');
#ifdef REDO
		savech(c);
#endif
		if(c == '\0') {
			clear_nhwindow(WIN_MESSAGE);
			return 0;
		}
	    } else {
		/* only one thing to itemize */
		if (unpaid_count)
		    c = 'u';
		else if (billx)
		    c = 'x';
		else
		    c = types[0];
	    }
	}
	if (c == 'x') {
	    if (billx)
		(void) doinvbill(1);
	    else
		pline("Auf der Einkaufsliste sind keine aufgebrauchten Objekte."); /* EN pline("No used-up objects on your shopping bill."); */
	    return 0;
	}
	if (c == 'u') {
	    if (unpaid_count)
		dounpaid();
	    else
		You("VERB_HABEN keine unbezahlten Objekte."); /* EN You("are not carrying any unpaid objects."); */
	    return 0;
	}
	if (traditional) {
	    oclass = def_char_to_objclass(c); /* change to object class */
	    if (oclass == COIN_CLASS) {
		return doprgold();
	    } else if (index(types, c) > index(types, '\033')) {
		You("VERB_HABEN keine derartigen Gegenst�nde."); /* EN You("have no such objects."); */
		return 0;
	    }
	    this_type = oclass;
	}
	if (query_objlist((char *) 0, invent,
		    (flags.invlet_constant ? USE_INVLET : 0)|INVORDER_SORT,
		    &pick_list, PICK_NONE, this_type_only) > 0)
	    free((genericptr_t)pick_list);
	return 0;
}

/* return a string describing the dungeon feature at <x,y> if there
   is one worth mentioning at that location; otherwise null */
const char *
dfeature_at(x, y, buf)
int x, y;
char *buf;
{
	struct rm *lev = &levl[x][y];
	int ltyp = lev->typ, cmap = -1;
	const char *dfeature = 0;
	static char altbuf[BUFSZ];

	if (IS_DOOR(ltyp)) {
	    switch (lev->doormask) {
	    case D_NODOOR:	cmap = S_ndoor; break;	/* "doorway" */
	    case D_ISOPEN:	cmap = S_vodoor; break;	/* "open door" */
	    case D_BROKEN:	dfeature = "ADJEKTIV_KAPUTT NOUN_DOOR"; break; /* EN case D_BROKEN:	dfeature = "broken door"; break; */
	    default:	cmap = S_vcdoor; break;	/* "closed door" */
	    }
	    /* override door description for open drawbridge */
	    if (is_drawbridge_wall(x, y) >= 0)
		dfeature = "ADJEKTIV_GEOEFFNET NOUN_PORTCULLIS",  cmap = -1; /* EN dfeature = "open drawbridge portcullis",  cmap = -1; */
	} else if (IS_FOUNTAIN(ltyp))
	    cmap = S_fountain;				/* "fountain" */
	else if (IS_THRONE(ltyp))
	    cmap = S_throne;				/* "opulent throne" */
	else if (is_lava(x,y))
	    cmap = S_lava;				/* "molten lava" */
	else if (is_ice(x,y))
	    cmap = S_ice;				/* "ice" */
	else if (is_pool(x,y))
	    dfeature = "NOUN_POOL_OF_WATER"; /* EN dfeature = "pool of water"; */
#ifdef SINKS
	else if (IS_SINK(ltyp))
	    cmap = S_sink;				/* "sink" */
#endif
	else if (IS_ALTAR(ltyp)) {
	    Sprintf(altbuf, "NOUN_ALTAR %s %s (%s)", gott_weiblich(a_gname())?"der":"des", a_gname(), /* EN Sprintf(altbuf, "altar to %s (%s)", a_gname(), */
		    align_str(Amask2align(lev->altarmask & ~AM_SHRINE)));
	    dfeature = altbuf;
	} else if ((x == xupstair && y == yupstair) ||
		 (x == sstairs.sx && y == sstairs.sy && sstairs.up))
	    cmap = S_upstair;				/* "staircase up" */
	else if ((x == xdnstair && y == ydnstair) ||
		 (x == sstairs.sx && y == sstairs.sy && !sstairs.up))
	    cmap = S_dnstair;				/* "staircase down" */
	else if (x == xupladder && y == yupladder)
	    cmap = S_upladder;				/* "ladder up" */
	else if (x == xdnladder && y == ydnladder)
	    cmap = S_dnladder;				/* "ladder down" */
	else if (ltyp == DRAWBRIDGE_DOWN)
	    cmap = S_vodbridge;			/* "lowered drawbridge" */
	else if (ltyp == DBWALL)
	    cmap = S_vcdbridge;			/* "raised drawbridge" */
	else if (IS_GRAVE(ltyp))
	    cmap = S_grave;				/* "grave" */
	else if (ltyp == TREE)
	    cmap = S_tree;				/* "tree" */
	else if (ltyp == IRONBARS)
	    dfeature = "NOUN_IRON_BARs"; /* EN dfeature = "set of iron bars"; */

	if (cmap >= 0) dfeature = defsyms[cmap].explanation;
	if (dfeature) Strcpy(buf, dfeature);
	return dfeature;
}

/* look at what is here; if there are many objects (5 or more),
   don't show them unless obj_cnt is 0 */
int
look_here(obj_cnt, picked_some)
int obj_cnt;	/* obj_cnt > 0 implies that autopickup is in progess */
boolean picked_some;
{
	struct obj *otmp;
	struct trap *trap;
	const char *verb = Blind ? "VERB_SPUEREN" : "VERB_SEE"; /* EN const char *verb = Blind ? "feel" : "see"; */
	const char *dfeature = (char *)0;
	char fbuf[BUFSZ], fbuf2[BUFSZ];
	winid tmpwin;
	boolean skip_objects = (obj_cnt >= 5), felt_cockatrice = FALSE;

	if (u.uswallow && u.ustuck) {
	    struct monst *mtmp = u.ustuck;
	    Sprintf(fbuf, "Inhalt KASUS_GENITIV %s", /* EN Sprintf(fbuf, "Contents of %s %s", */
		genitivattribut_zu_wort(mon_nam(mtmp), mbodypart(mtmp, STOMACH))); /* EN s_suffix(mon_nam(mtmp)), mbodypart(mtmp, STOMACH)); */
	    /* Skip "Contents of " by using fbuf index 12 */
	    You("%s MODIFIER_VERB_INFINITIV %s, was hier KASUS_DATIV _in_ %s herumliegt.", /* EN You("%s to %s what is lying in %s.", */
		Blind ? "VERB_VERSUCHEN zu" : "VERB_SCHAUEN OBJECT PRONOMEN_PERSONAL um, um zu", verb, &fbuf[21]); /* EN Blind ? "try" : "look around", verb, &fbuf[12]); */
	    otmp = mtmp->minvent;
	    if (otmp) {
		for ( ; otmp; otmp = otmp->nobj) {
			/* If swallower is an animal, it should have become stone but... */
			if (otmp->otyp == CORPSE) feel_cockatrice(otmp, FALSE);
		}
		if (Blind) Strcpy(fbuf, "SUBJECT PRONOMEN_PERSONAL VERB_SPUEREN"); /* EN if (Blind) Strcpy(fbuf, "You feel"); */
		Strcat(fbuf,":");
	    	(void) display_minventory(mtmp, MINV_ALL, fbuf);
	    } else {
		You("%s hier keine Objekte.", verb); /* EN You("%s no objects here.", verb); */
	    }
	    return(!!Blind);
	}
	if (!skip_objects && (trap = t_at(u.ux,u.uy)) && trap->tseen)
		pline("Hier ist %s.", /* EN There("is %s here.", */
			an(defsyms[trap_to_defsym(trap->ttyp)].explanation));

	otmp = level.objects[u.ux][u.uy];
	dfeature = dfeature_at(u.ux, u.uy, fbuf2);
	if (dfeature && !strcmp(dfeature, "NOUN_POOL_OF_WATER") && Underwater) /* EN if (dfeature && !strcmp(dfeature, "pool of water") && Underwater) */
		dfeature = 0;

	if (Blind) {
		boolean drift = Is_airlevel(&u.uz) || Is_waterlevel(&u.uz);
		if (dfeature && !strncmp(dfeature, "NOUN_ALTAR ", 11)) { /* EN if (dfeature && !strncmp(dfeature, "altar ", 6)) { */
		    /* don't say "altar" twice, dfeature has more info */
		    You("VERB_VERSUCHEN zu ertasten, was hier liegt."); /* EN You("try to feel what is here."); */
		} else {
		    You("VERB_VERSUCHEN zu ertasten, was hier %s%s.", /* EN You("try to feel what is %s%s.", */
			drift ? "" : auf_dem_Boden(u.ux, u.uy), /* EN drift ? "floating here" : "lying here on the ", */
			drift ? "herumschwebt" : " liegt"); /* EN drift ? ""		: surface(u.ux, u.uy)); */
		}
		if (dfeature && !drift && !strcmp(dfeature, surface(u.ux,u.uy)))
			dfeature = 0;		/* ice already identifed */
		if (!can_reach_floor()) {
			pline("But you can't reach it!"); /* EN pline("But you can't reach it!"); */ // TODO DE
			return(0);
		}
	}

	if (dfeature)
		Sprintf(fbuf, "Hier ist %s.", an(dfeature)); /* EN Sprintf(fbuf, "There is %s here.", an(dfeature)); */

	if (!otmp || is_lava(u.ux,u.uy) || (is_pool(u.ux,u.uy) && !Underwater)) {
		if (dfeature) pline(fbuf);
		read_engr_at(u.ux, u.uy); /* Eric Backus */
		if (!skip_objects && (Blind || !dfeature))
		    You("%s hier keine Objekte.", verb); /* EN You("%s no objects here.", verb); */
		return(!!Blind);
	}
	/* we know there is something here */

	if (skip_objects) {
	    if (dfeature) pline(fbuf);
	    read_engr_at(u.ux, u.uy); /* Eric Backus */
	    pline("Hier liegen %s%s Gegenst�nde.", /* EN There("are %s%s objects here.", */
		  (obj_cnt <= 10) ? "einige" : "viele", /* EN (obj_cnt <= 10) ? "several" : "many", */
		  picked_some ? " weitere" : ""); /* EN picked_some ? " more" : ""); */
	} else if (!otmp->nexthere) {
	    /* only one object */
	    if (dfeature) pline(fbuf);
	    read_engr_at(u.ux, u.uy); /* Eric Backus */
#ifdef INVISIBLE_OBJECTS
	    if (otmp->oinvis && !See_invisible) verb = "VERB_FUEHLEN"; /* EN if (otmp->oinvis && !See_invisible) verb = "feel"; */
#endif
	    You("%s hier OBJECT %s.", verb, doname(otmp)); /* EN You("%s here %s.", verb, doname(otmp)); */
	    if (otmp->otyp == CORPSE) feel_cockatrice(otmp, FALSE);
	} else {
	    display_nhwindow(WIN_MESSAGE, FALSE);
	    tmpwin = create_nhwindow(NHW_MENU);
	    if(dfeature) {
		putstr(tmpwin, 0, fbuf);
		putstr(tmpwin, 0, "");
	    }
	    putstr(tmpwin, 0, Blind ? "Hier von KASUS_DATIV PRONOMEN_PERSONAL ertastete Dinge:" : /* EN putstr(tmpwin, 0, Blind ? "Things that you feel here:" : */
				      "Hier liegende Dinge:"); /* EN "Things that are here:"); */
	    for ( ; otmp; otmp = otmp->nexthere) {
		if (otmp->otyp == CORPSE && will_feel_cockatrice(otmp, FALSE)) {
			char buf[BUFSZ];
			felt_cockatrice = TRUE;
			Strcpy(buf, doname(otmp));
			Strcat(buf, " ..."); /* EN Strcat(buf, "..."); */
			putstr(tmpwin, 0, buf);
			break;
		}
		putstr(tmpwin, 0, doname(otmp));
	    }
	    display_nhwindow(tmpwin, TRUE);
	    destroy_nhwindow(tmpwin);
	    if (felt_cockatrice) feel_cockatrice(otmp, FALSE);
	    read_engr_at(u.ux, u.uy); /* Eric Backus */
	}
	return(!!Blind);
}

/* explicilty look at what is here, including all objects */
int
dolook()
{
	return look_here(0, FALSE);
}

boolean
will_feel_cockatrice(otmp, force_touch)
struct obj *otmp;
boolean force_touch;
{
	if ((Blind || force_touch) && !uarmg && !Stone_resistance &&
		(otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm])))
			return TRUE;
	return FALSE;
}

void
feel_cockatrice(otmp, force_touch)
struct obj *otmp;
boolean force_touch;
{
	char kbuf[BUFSZ];

	if (will_feel_cockatrice(otmp, force_touch)) {
	    if(poly_when_stoned(youmonst.data))
			You("touched the %s corpse with your bare %s.", /* EN You("touched the %s corpse with your bare %s.", */ // TODO DE
				mons[otmp->corpsenm].mname, makeplural(body_part(HAND)));
	    else
			pline("Touching the %s corpse is a fatal mistake...", /* EN pline("Touching the %s corpse is a fatal mistake...", */ // TODO DE
				mons[otmp->corpsenm].mname);
		Sprintf(kbuf, "%s corpse", an(mons[otmp->corpsenm].mname)); /* EN Sprintf(kbuf, "%s corpse", an(mons[otmp->corpsenm].mname)); */ // TODO DE
		instapetrify(kbuf);
	}
}

#endif /* OVLB */
#ifdef OVL1

void
stackobj(obj)
struct obj *obj;
{
	struct obj *otmp;

	for(otmp = level.objects[obj->ox][obj->oy]; otmp; otmp = otmp->nexthere)
		if(otmp != obj && merged(&obj,&otmp))
			break;
	return;
}

STATIC_OVL boolean
mergable(otmp, obj)	/* returns TRUE if obj  & otmp can be merged */
	register struct obj *otmp, *obj;
{
	if (obj->otyp != otmp->otyp) return FALSE;
#ifdef GOLDOBJ
	/* coins of the same kind will always merge */
	if (obj->oclass == COIN_CLASS) return TRUE;
#endif
	if (obj->unpaid != otmp->unpaid ||
	    obj->spe != otmp->spe || obj->dknown != otmp->dknown ||
	    (obj->bknown != otmp->bknown && !Role_if(PM_PRIEST)) ||
	    obj->cursed != otmp->cursed || obj->blessed != otmp->blessed ||
	    obj->no_charge != otmp->no_charge ||
	    obj->obroken != otmp->obroken ||
	    obj->otrapped != otmp->otrapped ||
	    obj->lamplit != otmp->lamplit ||
#ifdef INVISIBLE_OBJECTS
		obj->oinvis != otmp->oinvis ||
#endif
	    obj->greased != otmp->greased ||
	    obj->oeroded != otmp->oeroded ||
	    obj->oeroded2 != otmp->oeroded2 ||
	    obj->bypass != otmp->bypass)
	    return(FALSE);

	if ((obj->oclass==WEAPON_CLASS || obj->oclass==ARMOR_CLASS) &&
	    (obj->oerodeproof!=otmp->oerodeproof || obj->rknown!=otmp->rknown))
	    return FALSE;

	if (obj->oclass == FOOD_CLASS && (obj->oeaten != otmp->oeaten ||
					  obj->orotten != otmp->orotten))
	    return(FALSE);

	if (obj->otyp == CORPSE || obj->otyp == EGG || obj->otyp == TIN) {
		if (obj->corpsenm != otmp->corpsenm)
				return FALSE;
	}

	/* hatching eggs don't merge; ditto for revivable corpses */
	if ((obj->otyp == EGG && (obj->timed || otmp->timed)) ||
	    (obj->otyp == CORPSE && otmp->corpsenm >= LOW_PM &&
		is_reviver(&mons[otmp->corpsenm])))
	    return FALSE;

	/* allow candle merging only if their ages are close */
	/* see begin_burn() for a reference for the magic "25" */
	if (Is_candle(obj) && obj->age/25 != otmp->age/25)
	    return(FALSE);

	/* burning potions of oil never merge */
	if (obj->otyp == POT_OIL && obj->lamplit)
	    return FALSE;

	/* don't merge surcharged item with base-cost item */
	if (obj->unpaid && !same_price(obj, otmp))
	    return FALSE;

	/* if they have names, make sure they're the same */
	if ( (obj->onamelth != otmp->onamelth &&
		((obj->onamelth && otmp->onamelth) || obj->otyp == CORPSE)
	     ) ||
	    (obj->onamelth && otmp->onamelth &&
		    strncmp(ONAME(obj), ONAME(otmp), (int)obj->onamelth)))
		return FALSE;

	/* for the moment, any additional information is incompatible */
	if (obj->oxlth || otmp->oxlth) return FALSE;

	if(obj->oartifact != otmp->oartifact) return FALSE;

	if(obj->known == otmp->known ||
		!objects[otmp->otyp].oc_uses_known) {
		return((boolean)(objects[obj->otyp].oc_merge));
	} else return(FALSE);
}

int
doprgold()
{
	/* the messages used to refer to "carrying gold", but that didn't
	   take containers into account */
#ifndef GOLDOBJ
	if(!u.ugold)
	    Your("NOUN_WALLET ist leer."); /* EN Your("wallet is empty."); */
	else
	    Your("NOUN_WALLET VERB_CONTAIN OBJECT %ld NOUN_GOLD_PIECE%s.", u.ugold, plur(u.ugold)); /* EN Your("wallet contains %ld gold piece%s.", u.ugold, plur(u.ugold)); */
#else
        long umoney = money_cnt(invent);
	if(!umoney)
	    Your("NOUN_WALLET ist leer."); /* EN Your("wallet is empty."); */
	else
	    Your("NOUN_WALLET VERB_CONTAIN %ld %s.", umoney, currency(umoney)); /* EN Your("wallet contains %ld %s.", umoney, currency(umoney)); */
#endif
	shopper_financial_report();
	return 0;
}

#endif /* OVL1 */
#ifdef OVLB

int
doprwep()
{
    if (!uwep) {
	You("VERB_SEIN jetzt unbewaffnet."); /* EN You("are empty %s.", body_part(HANDED)); */
    } else {
	prinv((char *)0, uwep, 0L);
	if (u.twoweap) prinv((char *)0, uswapwep, 0L);
    }
    return 0;
}

int
doprarm()
{
	if(!wearing_armor())
		You("VERB_TRAGEN gar OBJECT PRONOMEN_KEIN NOUN_ARMOR."); /* EN You("are not wearing any armor."); */
	else {
#ifdef TOURIST
		char lets[8];
#else
		char lets[7];
#endif
		register int ct = 0;

#ifdef TOURIST
		if(uarmu) lets[ct++] = obj_to_let(uarmu);
#endif
		if(uarm) lets[ct++] = obj_to_let(uarm);
		if(uarmc) lets[ct++] = obj_to_let(uarmc);
		if(uarmh) lets[ct++] = obj_to_let(uarmh);
		if(uarms) lets[ct++] = obj_to_let(uarms);
		if(uarmg) lets[ct++] = obj_to_let(uarmg);
		if(uarmf) lets[ct++] = obj_to_let(uarmf);
		lets[ct] = 0;
		(void) display_inventory(lets, FALSE);
	}
	return 0;
}

int
doprring()
{
	if(!uleft && !uright)
		You("VERB_TRAGEN gar keine Ringe."); /* EN You("are not wearing any rings."); */
	else {
		char lets[3];
		register int ct = 0;

		if(uleft) lets[ct++] = obj_to_let(uleft);
		if(uright) lets[ct++] = obj_to_let(uright);
		lets[ct] = 0;
		(void) display_inventory(lets, FALSE);
	}
	return 0;
}

int
dopramulet()
{
	if (!uamul)
		You("VERB_TRAGEN gar kein Amulett."); /* EN You("are not wearing an amulet."); */
	else
		prinv((char *)0, uamul, 0L);
	return 0;
}

STATIC_OVL boolean
tool_in_use(obj)
struct obj *obj;
{
	if ((obj->owornmask & (W_TOOL
#ifdef STEED
			| W_SADDLE
#endif
			)) != 0L) return TRUE;
	if (obj->oclass != TOOL_CLASS) return FALSE;
	return (boolean)(obj == uwep || obj->lamplit ||
				(obj->otyp == LEASH && obj->leashmon));
}

int
doprtool()
{
	struct obj *otmp;
	int ct = 0;
	char lets[52+1];

	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (tool_in_use(otmp))
		lets[ct++] = obj_to_let(otmp);
	lets[ct] = '\0';
	if (!ct) You("VERB_BENUTZEN OBJECT gar PRONOMEN_KEIN NOUN_TOOLs."); /* EN if (!ct) You("are not using any tools."); */
	else (void) display_inventory(lets, FALSE);
	return 0;
}

/* '*' command; combines the ')' + '[' + '=' + '"' + '(' commands;
   show inventory of all currently wielded, worn, or used objects */
int
doprinuse()
{
	struct obj *otmp;
	int ct = 0;
	char lets[52+1];

	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (is_worn(otmp) || tool_in_use(otmp))
		lets[ct++] = obj_to_let(otmp);
	lets[ct] = '\0';
	if (!ct) You("are not wearing or wielding anything."); /* EN if (!ct) You("are not wearing or wielding anything."); */ // TODO DE
	else (void) display_inventory(lets, FALSE);
	return 0;
}

/*
 * uses up an object that's on the floor, charging for it as necessary
 */
void
useupf(obj, numused)
register struct obj *obj;
long numused;
{
	register struct obj *otmp;
	boolean at_u = (obj->ox == u.ux && obj->oy == u.uy);

	/* burn_floor_paper() keeps an object pointer that it tries to
	 * useupf() multiple times, so obj must survive if plural */
	if (obj->quan > numused)
		otmp = splitobj(obj, numused);
	else
		otmp = obj;
	if(costly_spot(otmp->ox, otmp->oy)) {
	    if(index(u.urooms, *in_rooms(otmp->ox, otmp->oy, 0)))
	        addtobill(otmp, FALSE, FALSE, FALSE);
	    else (void)stolen_value(otmp, otmp->ox, otmp->oy, FALSE, FALSE);
	}
	delobj(otmp);
	if (at_u && u.uundetected && hides_under(youmonst.data))
	    u.uundetected = OBJ_AT(u.ux, u.uy);
}

#endif /* OVLB */


#ifdef OVL1

/*
 * Conversion from a class to a string for printing.
 * This must match the object class order.
 */
STATIC_VAR NEARDATA const char *names[] = { 0,
	"Illegal objects", "NOUN_WEAPONs", "NOUN_ARMOR", "NOUN_RINGs", "NOUN_AMULETs",
	"NOUN_TOOLs", "NOUN_COMESTIBLE", "NOUN_POTIONs", "NOUN_SCROLLs", "NOUN_SPELLBOOKs",
	"NOUN_WANDs", "NOUN_COINs", "NOUN_GEMs", "NOUN_BOULDERs, NOUN_STATUEs", "Iron balls",
	"Chains", "Venoms"
};

static NEARDATA const char oth_symbols[] = {
	CONTAINED_SYM,
	'\0'
};

static NEARDATA const char *oth_names[] = {
	"Gegenst�nde in S�cken oder Kisten" /* EN "Bagged/Boxed items" */
};

static NEARDATA char *invbuf = (char *)0;
static NEARDATA unsigned invbufsiz = 0;

char *
let_to_name(let,unpaid)
char let;
boolean unpaid;
{
	const char *class_name;
	const char *pos;
	int oclass = (let >= 1 && let < MAXOCLASSES) ? let : 0;
	unsigned len;

	if (oclass)
	    class_name = names[oclass];
	else if ((pos = index(oth_symbols, let)) != 0)
	    class_name = oth_names[pos - oth_symbols];
	else
	    class_name = names[0];

	len = strlen(class_name) + (unpaid ? sizeof "satzbeginn_adjektiv_unbezahlt_" : sizeof ""); /* EN len = strlen(class_name) + (unpaid ? sizeof "unpaid_" : sizeof ""); */
	if (len > invbufsiz) {
	    if (invbuf) free((genericptr_t)invbuf);
	    invbufsiz = len + 10; /* add slop to reduce incremental realloc */
	    invbuf = (char *) alloc(invbufsiz);
	}
	if (unpaid)
	    Strcat(strcpy(invbuf, "SATZBEGINN ADJEKTIV_UNBEZAHLT "), class_name); /* EN Strcat(strcpy(invbuf, "Unpaid "), class_name); */
	else
	    Strcpy(invbuf, class_name);
	return invbuf;
}

void
free_invbuf()
{
	if (invbuf) free((genericptr_t)invbuf),  invbuf = (char *)0;
	invbufsiz = 0;
}

#endif /* OVL1 */
#ifdef OVLB

void
reassign()
{
	register int i;
	register struct obj *obj;

	for(obj = invent, i = 0; obj; obj = obj->nobj, i++)
		obj->invlet = (i < 26) ? ('a'+i) : ('A'+i-26);
	lastinvnr = i;
}

#endif /* OVLB */
#ifdef OVL1

int
doorganize()	/* inventory organizer by Del Lamb */
{
	struct obj *obj, *otmp;
	register int ix, cur;
	register char let;
	char alphabet[52+1], buf[52+1];
	char qbuf[QBUFSZ];
	char allowall[2];
	const char *adj_type;

	if (!flags.invlet_constant) reassign();
	/* get a pointer to the object the user wants to organize */
	allowall[0] = ALL_CLASSES; allowall[1] = '\0';
	if (!(obj = getobj(allowall,"adjust"))) return(0); /* EN if (!(obj = getobj(allowall,"adjust"))) return(0); */ // TODO DE

	/* initialize the list with all upper and lower case letters */
	for (let = 'a', ix = 0;  let <= 'z';) alphabet[ix++] = let++;
	for (let = 'A', ix = 26; let <= 'Z';) alphabet[ix++] = let++;
	alphabet[52] = 0;

	/* blank out all the letters currently in use in the inventory */
	/* except those that will be merged with the selected object   */
	for (otmp = invent; otmp; otmp = otmp->nobj)
		if (otmp != obj && !mergable(otmp,obj)) {
			if (otmp->invlet <= 'Z')
				alphabet[(otmp->invlet) - 'A' + 26] = ' ';
			else	alphabet[(otmp->invlet) - 'a']	    = ' ';
		}

	/* compact the list by removing all the blanks */
	for (ix = cur = 0; ix <= 52; ix++)
		if (alphabet[ix] != ' ') buf[cur++] = alphabet[ix];

	/* and by dashing runs of letters */
	if(cur > 5) compactify(buf);

	/* get new letter to use as inventory letter */
	for (;;) {
		Sprintf(qbuf, "Adjust letter to what [%s]?",buf); /* EN Sprintf(qbuf, "Adjust letter to what [%s]?",buf); */ // TODO DE
		let = yn_function(qbuf, (char *)0, '\0');
		if(index(quitchars,let)) {
			pline(Never_mind);
			return(0);
		}
		if (let == '@' || !letter(let))
			pline("Bitte MODIFIER_VERB_IMPERATIV VERB_WAEHLEN einen Inventar-Buchstaben."); /* EN pline("Select an inventory slot letter."); */
		else
			break;
	}

	/* change the inventory and print the resulting item */
	adj_type = "Verschoben:"; /* EN adj_type = "Moving:"; */

	/*
	 * don't use freeinv/addinv to avoid double-touching artifacts,
	 * dousing lamps, losing luck, cursing loadstone, etc.
	 */
	extract_nobj(obj, &invent);

	for (otmp = invent; otmp;)
		if (merged(&otmp,&obj)) {
			adj_type = "Zusammengelegt:"; /* EN adj_type = "Merging:"; */
			obj = otmp;
			otmp = otmp->nobj;
			extract_nobj(obj, &invent);
		} else {
			if (otmp->invlet == let) {
				adj_type = "Getauscht:"; /* EN adj_type = "Swapping:"; */
				otmp->invlet = obj->invlet;
			}
			otmp = otmp->nobj;
		}

	/* inline addinv (assuming flags.invlet_constant and !merged) */
	obj->invlet = let;
	obj->nobj = invent; /* insert at beginning */
	obj->where = OBJ_INVENT;
	invent = obj;
	reorder_invent();

	prinv(adj_type, obj, 0L);
	update_inventory();
	return(0);
}

/* common to display_minventory and display_cinventory */
STATIC_OVL void
invdisp_nothing(hdr, txt)
const char *hdr, *txt;
{
	winid win;
	anything any;
	menu_item *selected;

	any.a_void = 0;
	win = create_nhwindow(NHW_MENU);
	start_menu(win);
	add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings, hdr, MENU_UNSELECTED);
	add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, "", MENU_UNSELECTED);
	add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, txt, MENU_UNSELECTED);
	end_menu(win, (char *)0);
	if (select_menu(win, PICK_NONE, &selected) > 0)
	    free((genericptr_t)selected);
	destroy_nhwindow(win);
	return;
}

/* query_objlist callback: return things that could possibly be worn/wielded */
STATIC_OVL boolean
worn_wield_only(obj)
struct obj *obj;
{
    return (obj->oclass == WEAPON_CLASS
		|| obj->oclass == ARMOR_CLASS
		|| obj->oclass == AMULET_CLASS
		|| obj->oclass == RING_CLASS
		|| obj->oclass == TOOL_CLASS);
}

/*
 * Display a monster's inventory.
 * Returns a pointer to the object from the monster's inventory selected
 * or NULL if nothing was selected.
 *
 * By default, only worn and wielded items are displayed.  The caller
 * can pick one.  Modifier flags are:
 *
 *	MINV_NOLET	- nothing selectable
 *	MINV_ALL	- display all inventory
 */
struct obj *
display_minventory(mon, dflags, title)
register struct monst *mon;
int dflags;
char *title;
{
	struct obj *ret;
#ifndef GOLDOBJ
	struct obj m_gold;
#endif
	char tmp[QBUFSZ];
	int n;
	menu_item *selected = 0;
#ifndef GOLDOBJ
	int do_all = (dflags & MINV_ALL) != 0,
	    do_gold = (do_all && mon->mgold);
#else
	int do_all = (dflags & MINV_ALL) != 0;
#endif

	Sprintf(tmp,"SATZBEGINN %s:", genitivattribut_zu_wort(noit_Monnam(mon), /* EN Sprintf(tmp,"%s %s:", s_suffix(noit_Monnam(mon)), */
		do_all ? "NOUN_BESITZTUMs" : "NOUN_BEWAFFNUNG")); /* EN do_all ? "possessions" : "armament"); */

#ifndef GOLDOBJ
	if (do_all ? (mon->minvent || mon->mgold)
#else
	if (do_all ? (mon->minvent != 0)
#endif
		   : (mon->misc_worn_check || MON_WEP(mon))) {
	    /* Fool the 'weapon in hand' routine into
	     * displaying 'weapon in claw', etc. properly.
	     */
	    youmonst.data = mon->data;

#ifndef GOLDOBJ
	    if (do_gold) {
		/*
		 * Make temporary gold object and insert at the head of
		 * the mon's inventory.  We can get away with using a
		 * stack variable object because monsters don't carry
		 * gold in their inventory, so it won't merge.
		 */
		m_gold = zeroobj;
		m_gold.otyp = GOLD_PIECE;  m_gold.oclass = COIN_CLASS;
		m_gold.quan = mon->mgold;  m_gold.dknown = 1;
		m_gold.where = OBJ_FREE;
		/* we had better not merge and free this object... */
		if (add_to_minv(mon, &m_gold))
		    panic("display_minventory: static object freed.");
	    }

#endif
	    n = query_objlist(title ? title : tmp, mon->minvent, INVORDER_SORT, &selected,
			(dflags & MINV_NOLET) ? PICK_NONE : PICK_ONE,
			do_all ? allow_all : worn_wield_only);

#ifndef GOLDOBJ
	    if (do_gold) obj_extract_self(&m_gold);
#endif

	    set_uasmon();
	} else {
	    invdisp_nothing(title ? title : tmp, "(nichts)"); /* EN invdisp_nothing(title ? title : tmp, "(none)"); */
	    n = 0;
	}

	if (n > 0) {
	    ret = selected[0].item.a_obj;
	    free((genericptr_t)selected);
#ifndef GOLDOBJ
	    /*
	     * Unfortunately, we can't return a pointer to our temporary
	     * gold object.  We'll have to work out a scheme where this
	     * can happen.  Maybe even put gold in the inventory list...
	     */
	    if (ret == &m_gold) ret = (struct obj *) 0;
#endif
	} else
	    ret = (struct obj *) 0;
	return ret;
}

/*
 * Display the contents of a container in inventory style.
 * Currently, this is only used for statues, via wand of probing.
 */
struct obj *
display_cinventory(obj)
register struct obj *obj;
{
	struct obj *ret;
	char tmp[QBUFSZ];
	int n;
	menu_item *selected = 0;

	Sprintf(tmp,"Inhalt KASUS_GENITIV %s:", doname(obj)); /* EN Sprintf(tmp,"Contents of %s:", doname(obj)); */

	if (obj->cobj) {
	    n = query_objlist(tmp, obj->cobj, INVORDER_SORT, &selected,
			    PICK_NONE, allow_all);
	} else {
	    invdisp_nothing(tmp, "(leer)"); /* EN invdisp_nothing(tmp, "(empty)"); */
	    n = 0;
	}
	if (n > 0) {
	    ret = selected[0].item.a_obj;
	    free((genericptr_t)selected);
	} else
	    ret = (struct obj *) 0;
	return ret;
}

/* query objlist callback: return TRUE if obj is at given location */
static coord only;

STATIC_OVL boolean
only_here(obj)
    struct obj *obj;
{
    return (obj->ox == only.x && obj->oy == only.y);
}

/*
 * Display a list of buried items in inventory style.  Return a non-zero
 * value if there were items at that spot.
 *
 * Currently, this is only used with a wand of probing zapped downwards.
 */
int
display_binventory(x, y, as_if_seen)
int x, y;
boolean as_if_seen;
{
	struct obj *obj;
	menu_item *selected = 0;
	int n;

	/* count # of objects here */
	for (n = 0, obj = level.buriedobjlist; obj; obj = obj->nobj)
	    if (obj->ox == x && obj->oy == y) {
		if (as_if_seen) obj->dknown = 1;
		n++;
	    }

	if (n) {
	    only.x = x;
	    only.y = y;
	    if (query_objlist("Hier vergrabene Dinge:", /* EN if (query_objlist("Things that are buried here:", */
			      level.buriedobjlist, INVORDER_SORT,
			      &selected, PICK_NONE, only_here) > 0)
		free((genericptr_t)selected);
	    only.x = only.y = 0;
	}
	return n;
}

#endif /* OVL1 */

/*invent.c*/
