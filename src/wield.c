/*	SCCS Id: @(#)wield.c	3.4	2003/01/29	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/* KMH -- Differences between the three weapon slots.
 *
 * The main weapon (uwep):
 * 1.  Is filled by the (w)ield command.
 * 2.  Can be filled with any type of item.
 * 3.  May be carried in one or both hands.
 * 4.  Is used as the melee weapon and as the launcher for
 *     ammunition.
 * 5.  Only conveys intrinsics when it is a weapon, weapon-tool,
 *     or artifact.
 * 6.  Certain cursed items will weld to the hand and cannot be
 *     unwielded or dropped.  See erodeable_wep() and will_weld()
 *     below for the list of which items apply.
 *
 * The secondary weapon (uswapwep):
 * 1.  Is filled by the e(x)change command, which swaps this slot
 *     with the main weapon.  If the "pushweapon" option is set,
 *     the (w)ield command will also store the old weapon in the
 *     secondary slot.
 * 2.  Can be field with anything that will fit in the main weapon
 *     slot; that is, any type of item.
 * 3.  Is usually NOT considered to be carried in the hands.
 *     That would force too many checks among the main weapon,
 *     second weapon, shield, gloves, and rings; and it would
 *     further be complicated by bimanual weapons.  A special
 *     exception is made for two-weapon combat.
 * 4.  Is used as the second weapon for two-weapon combat, and as
 *     a convenience to swap with the main weapon.
 * 5.  Never conveys intrinsics.
 * 6.  Cursed items never weld (see #3 for reasons), but they also
 *     prevent two-weapon combat.
 *
 * The quiver (uquiver):
 * 1.  Is filled by the (Q)uiver command.
 * 2.  Can be filled with any type of item.
 * 3.  Is considered to be carried in a special part of the pack.
 * 4.  Is used as the item to throw with the (f)ire command.
 *     This is a convenience over the normal (t)hrow command.
 * 5.  Never conveys intrinsics.
 * 6.  Cursed items never weld; their effect is handled by the normal
 *     throwing code.
 *
 * No item may be in more than one of these slots.
 */


STATIC_DCL int FDECL(ready_weapon, (struct obj *));

/* used by will_weld() */
/* probably should be renamed */
#define erodeable_wep(optr)	((optr)->oclass == WEAPON_CLASS \
				|| is_weptool(optr) \
				|| (optr)->otyp == HEAVY_IRON_BALL \
				|| (optr)->otyp == IRON_CHAIN)

/* used by welded(), and also while wielding */
#define will_weld(optr)		((optr)->cursed \
				&& (erodeable_wep(optr) \
				   || (optr)->otyp == TIN_OPENER))


/*** Functions that place a given item in a slot ***/
/* Proper usage includes:
 * 1.  Initializing the slot during character generation or a
 *     restore.
 * 2.  Setting the slot due to a player's actions.
 * 3.  If one of the objects in the slot are split off, these
 *     functions can be used to put the remainder back in the slot.
 * 4.  Putting an item that was thrown and returned back into the slot.
 * 5.  Emptying the slot, by passing a null object.  NEVER pass
 *     zeroobj!
 *
 * If the item is being moved from another slot, it is the caller's
 * responsibility to handle that.  It's also the caller's responsibility
 * to print the appropriate messages.
 */
void
setuwep(obj)
register struct obj *obj;
{
	struct obj *olduwep = uwep;

	if (obj == uwep) return; /* necessary to not set unweapon */
	/* This message isn't printed in the caller because it happens
	 * *whenever* Sunsword is unwielded, from whatever cause.
	 */
	setworn(obj, W_WEP);
	if (uwep == obj && artifact_light(olduwep) && olduwep->lamplit) {
	    end_burn(olduwep, FALSE);
	    if (!Blind) pline("%s glowing.", Tobjnam(olduwep, "stop")); /* EN if (!Blind) pline("%s glowing.", Tobjnam(olduwep, "stop")); */ // TODO DE
	}
	/* Note: Explicitly wielding a pick-axe will not give a "bashing"
	 * message.  Wielding one via 'a'pplying it will.
	 * 3.2.2:  Wielding arbitrary objects will give bashing message too.
	 */
	if (obj) {
		unweapon = (obj->oclass == WEAPON_CLASS) ?
				is_launcher(obj) || is_ammo(obj) ||
				is_missile(obj) || (is_pole(obj)
#ifdef STEED
				&& !u.usteed
#endif
				) : !is_weptool(obj);
	} else
		unweapon = TRUE;	/* for "bare hands" message */
	update_inventory();
}

STATIC_OVL int
ready_weapon(wep)
struct obj *wep;
{
	/* Separated function so swapping works easily */
	int res = 0;

	if (!wep) {
	    /* No weapon */
	    if (uwep) {
		You("VERB_SEIN jetzt unbewaffnet."); /* EN You("are empty %s.", body_part(HANDED)); */
		setuwep((struct obj *) 0);
		res++;
	    } else
		You("VERB_SEIN schon unbewaffnet."); /* EN You("are already empty %s.", body_part(HANDED)); */
	} else if (!uarmg && !Stone_resistance && wep->otyp == CORPSE
				&& touch_petrifies(&mons[wep->corpsenm])) {
	    /* Prevent wielding cockatrice when not wearing gloves --KAA */
	    char kbuf[BUFSZ];

	    You("wield the %s corpse in your bare %s.", /* EN You("wield the %s corpse in your bare %s.", */ // TODO DE
		mons[wep->corpsenm].mname, makeplural(body_part(HAND)));
	    Sprintf(kbuf, "%s corpse", an(mons[wep->corpsenm].mname)); /* EN Sprintf(kbuf, "%s corpse", an(mons[wep->corpsenm].mname)); */ // TODO DE
	    instapetrify(kbuf);
	} else if (uarms && bimanual(wep))
	    You("VERB_KOENNEN OBJECT PRONOMEN_KEIN ADJEKTIV_ZWEIHAENDISCH %s f�hren und gleichzeitig einen Schild tragen.", /* EN You("cannot wield a two-handed %s while wearing a shield.", */
		is_sword(wep) ? "NOUN_SWORD" : /* EN is_sword(wep) ? "sword" : */
		    wep->otyp == BATTLE_AXE ? "NOUN_AXE" : "NOUN_WEAPON"); /* EN wep->otyp == BATTLE_AXE ? "axe" : "weapon"); */
	else if (wep->oartifact && !touch_artifact(wep, &youmonst)) {
	    res++;	/* takes a turn even though it doesn't get wielded */
	} else {
	    /* Weapon WILL be wielded after this point */
	    res++;
	    if (will_weld(wep)) {
		const char *tmp = xname(wep), *thestr = "The "; /* EN const char *tmp = xname(wep), *thestr = "The "; */ // TODO DE
		if (strncmp(tmp, thestr, 4) && !strncmp(The(tmp),thestr,4))
		    tmp = thestr;
		else tmp = "";
		pline("%s%s %s to your %s!", tmp, aobjnam(wep, "weld"), /* EN pline("%s%s %s to your %s!", tmp, aobjnam(wep, "weld"), */ // TODO DE
			(wep->quan == 1L) ? "itself" : "themselves", /* a3 */ /* EN (wep->quan == 1L) ? "itself" : "themselves", */ // TODO DE
			bimanual(wep) ?
				(const char *)makeplural(body_part(HAND))
				: body_part(HAND));
		wep->bknown = TRUE;
	    } else {
		/* The message must be printed before setuwep (since
		 * you might die and be revived from changing weapons),
		 * and the message must be before the death message and
		 * Lifesaved rewielding.  Yet we want the message to
		 * say "weapon in hand", thus this kludge.
		 */
		long dummy = wep->owornmask;
		wep->owornmask |= W_WEP;
		prinv((char *)0, wep, 0L);
		wep->owornmask = dummy;
	    }
	    setuwep(wep);

	    /* KMH -- Talking artifacts are finally implemented */
	    arti_speak(wep);

	    if (artifact_light(wep) && !wep->lamplit) {
		begin_burn(wep, FALSE);
		if (!Blind)
		    pline("SUBJECT %s hell zu erstrahlen!", Tobjnam(wep, "VERB_BEGINNEN")); /* EN pline("%s to glow brilliantly!", Tobjnam(wep, "begin")); */
	    }

#if 0
	    /* we'll get back to this someday, but it's not balanced yet */
	    if (Race_if(PM_ELF) && !wep->oartifact &&
			    objects[wep->otyp].oc_material == IRON) {
		/* Elves are averse to wielding cold iron */
		You("have an uneasy feeling about wielding cold iron.");
		change_luck(-1);
	    }
#endif

	    if (wep->unpaid) {
		struct monst *this_shkp;

		if ((this_shkp = shop_keeper(inside_shop(u.ux, u.uy))) !=
		    (struct monst *)0) {
		    pline("SUBJECT %s sagt \"MODIFIER_VERB_IMPERATIV VERB_SEIN bitte vorsichtig OBJECT KASUS_DATIV mit PRONOMEN_MEIN %s!\"", /* EN pline("%s says \"You be careful with my %s!\"", */
			  shkname(this_shkp),
			  xname(wep));
		}
	    }
	}
	return(res);
}

void
setuqwep(obj)
register struct obj *obj;
{
	setworn(obj, W_QUIVER);
	update_inventory();
}

void
setuswapwep(obj)
register struct obj *obj;
{
	setworn(obj, W_SWAPWEP);
	update_inventory();
}


/*** Commands to change particular slot(s) ***/

static NEARDATA const char wield_objs[] =
	{ ALL_CLASSES, ALLOW_NONE, WEAPON_CLASS, TOOL_CLASS, 0 };
static NEARDATA const char ready_objs[] =
	{ ALL_CLASSES, ALLOW_NONE, WEAPON_CLASS, 0 };
static NEARDATA const char bullets[] =	/* (note: different from dothrow.c) */
	{ ALL_CLASSES, ALLOW_NONE, GEM_CLASS, WEAPON_CLASS, 0 };

int
dowield()
{
	register struct obj *wep, *oldwep;
	int result;

	/* May we attempt this? */
	multi = 0;
	if (cantwield(youmonst.data)) {
		pline("Don't be ridiculous!"); /* EN pline("Don't be ridiculous!"); */ // TODO DE
		return(0);
	}

	/* Prompt for a new weapon */
	if (!(wep = getobj(wield_objs, "wield"))) /* EN if (!(wep = getobj(wield_objs, "wield"))) */ // TODO DE
		/* Cancelled */
		return (0);
	else if (wep == uwep) {
	    You("are already wielding that!"); /* EN You("are already wielding that!"); */ // TODO DE
	    if (is_weptool(wep)) unweapon = FALSE;	/* [see setuwep()] */
		return (0);
	} else if (welded(uwep)) {
		weldmsg(uwep);
		/* previously interrupted armor removal mustn't be resumed */
		reset_remarm();
		return (0);
	}

	/* Handle no object, or object in other slot */
	if (wep == &zeroobj)
		wep = (struct obj *) 0;
	else if (wep == uswapwep)
		return (doswapweapon());
	else if (wep == uquiver)
		setuqwep((struct obj *) 0);
	else if (wep->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL
#ifdef STEED
			| W_SADDLE
#endif
			)) {
		You("cannot wield that!"); /* EN You("cannot wield that!"); */ // TODO DE
		return (0);
	}

	/* Set your new primary weapon */
	oldwep = uwep;
	result = ready_weapon(wep);
	if (flags.pushweapon && oldwep && uwep != oldwep)
		setuswapwep(oldwep);
	untwoweapon();

	return (result);
}

int
doswapweapon()
{
	register struct obj *oldwep, *oldswap;
	int result = 0;


	/* May we attempt this? */
	multi = 0;
	if (cantwield(youmonst.data)) {
		pline("Don't be ridiculous!"); /* EN pline("Don't be ridiculous!"); */ // TODO DE
		return(0);
	}
	if (welded(uwep)) {
		weldmsg(uwep);
		return (0);
	}

	/* Unwield your current secondary weapon */
	oldwep = uwep;
	oldswap = uswapwep;
	setuswapwep((struct obj *) 0);

	/* Set your new primary weapon */
	result = ready_weapon(oldswap);

	/* Set your new secondary weapon */
	if (uwep == oldwep)
		/* Wield failed for some reason */
		setuswapwep(oldswap);
	else {
		setuswapwep(oldwep);
		if (uswapwep)
			prinv((char *)0, uswapwep, 0L);
		else
			You("VERB_HABEN keine Zweitwaffe parat."); /* EN You("have no secondary weapon readied."); */
	}

	if (u.twoweap && !can_twoweapon())
		untwoweapon();

	return (result);
}

int
dowieldquiver()
{
	register struct obj *newquiver;
	const char *quivee_types = (uslinging() ||
		  (uswapwep && objects[uswapwep->otyp].oc_skill == P_SLING)) ?
				  bullets : ready_objs;

	/* Since the quiver isn't in your hands, don't check cantwield(), */
	/* will_weld(), touch_petrifies(), etc. */
	multi = 0;

	/* Because 'Q' used to be quit... */
	if (flags.suppress_alert < FEATURE_NOTICE_VER(3,3,0))
		pline("Note: Please use #quit if you wish to exit the game."); /* EN pline("Note: Please use #quit if you wish to exit the game."); */ // TODO DE

	/* Prompt for a new quiver */
	if (!(newquiver = getobj(quivee_types, "ready"))) /* EN if (!(newquiver = getobj(quivee_types, "ready"))) */ // TODO DE
		/* Cancelled */
		return (0);

	/* Handle no object, or object in other slot */
	/* Any type is okay, since we give no intrinsics anyways */
	if (newquiver == &zeroobj) {
		/* Explicitly nothing */
		if (uquiver) {
			You("now have no ammunition readied."); /* EN You("now have no ammunition readied."); */ // TODO DE
			setuqwep(newquiver = (struct obj *) 0);
		} else {
			You("already have no ammunition readied!"); /* EN You("already have no ammunition readied!"); */ // TODO DE
			return(0);
		}
	} else if (newquiver == uquiver) {
		pline("That ammunition is already readied!"); /* EN pline("That ammunition is already readied!"); */ // TODO DE
		return(0);
	} else if (newquiver == uwep) {
		/* Prevent accidentally readying the main weapon */
		pline("%s already being used as a weapon!", /* EN pline("%s already being used as a weapon!", */ // TODO DE
		      !is_plural(uwep) ? "That is" : "They are"); /* EN !is_plural(uwep) ? "That is" : "They are"); */ // TODO DE
		return(0);
	} else if (newquiver->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL
#ifdef STEED
			| W_SADDLE
#endif
			)) {
		You("cannot ready that!"); /* EN You("cannot ready that!"); */ // TODO DE
		return (0);
	} else {
		long dummy;


		/* Check if it's the secondary weapon */
		if (newquiver == uswapwep) {
			setuswapwep((struct obj *) 0);
			untwoweapon();
		}

		/* Okay to put in quiver; print it */
		dummy = newquiver->owornmask;
		newquiver->owornmask |= W_QUIVER;
		prinv((char *)0, newquiver, 0L);
		newquiver->owornmask = dummy;
	}

	/* Finally, place it in the quiver */
	setuqwep(newquiver);
	/* Take no time since this is a convenience slot */
	return (0);
}

/* used for #rub and for applying pick-axe, whip, grappling hook, or polearm */
/* (moved from apply.c) */
boolean
wield_tool(obj, verb)
struct obj *obj;
const char *verb;	/* "rub",&c */
{
    const char *what;
    boolean more_than_1;

    if (obj == uwep) return TRUE;   /* nothing to do if already wielding it */

    if (!verb) verb = "wield"; /* EN if (!verb) verb = "wield"; */ // TODO DE
    what = xname(obj);
    more_than_1 = (obj->quan > 1L ||
		   strstri(what, "pair of ") != 0 || /* EN strstri(what, "pair of ") != 0 || */ // TODO DE
		   strstri(what, "s of ") != 0); /* EN strstri(what, "s of ") != 0); */ // TODO DE

    if (obj->owornmask & (W_ARMOR|W_RING|W_AMUL|W_TOOL)) {
	char yourbuf[BUFSZ];

	You_cant("%s %s %s while wearing %s.", /* EN You_cant("%s %s %s while wearing %s.", */ // TODO DE
		 verb, shk_your(yourbuf, obj), what,
		 more_than_1 ? "them" : "it"); /* EN more_than_1 ? "them" : "it"); */ // TODO DE
	return FALSE;
    }
    if (welded(uwep)) {
	if (flags.verbose) {
	    const char *hand = body_part(HAND);

	    if (bimanual(uwep)) hand = makeplural(hand);
	    if (strstri(what, "pair of ") != 0) more_than_1 = FALSE; /* EN if (strstri(what, "pair of ") != 0) more_than_1 = FALSE; */ // TODO DE
	    pline(
	     "Since your weapon is welded to your %s, you cannot %s %s %s.", /* EN "Since your weapon is welded to your %s, you cannot %s %s %s.", */ // TODO DE
		  hand, verb, more_than_1 ? "those" : "that", xname(obj)); /* EN hand, verb, more_than_1 ? "those" : "that", xname(obj)); */ // TODO DE
	} else {
	    You("VERB_KOENNEN das nicht tun."); /* EN You_cant("do that."); */
	}
	return FALSE;
    }
    if (cantwield(youmonst.data)) {
	You_cant("hold %s strongly enough.", more_than_1 ? "them" : "it"); /* EN You_cant("hold %s strongly enough.", more_than_1 ? "them" : "it"); */ // TODO DE
	return FALSE;
    }
    /* check shield */
    if (uarms && bimanual(obj)) {
	You("cannot %s a two-handed %s while wearing a shield.", /* EN You("cannot %s a two-handed %s while wearing a shield.", */ // TODO DE
	    verb, (obj->oclass == WEAPON_CLASS) ? "weapon" : "tool"); /* EN verb, (obj->oclass == WEAPON_CLASS) ? "weapon" : "tool"); */ // TODO DE
	return FALSE;
    }
    if (uquiver == obj) setuqwep((struct obj *)0);
    if (uswapwep == obj) {
	(void) doswapweapon();
	/* doswapweapon might fail */
	if (uswapwep == obj) return FALSE;
    } else {
	You("now wield %s.", doname(obj)); /* EN You("now wield %s.", doname(obj)); */ // TODO DE
	setuwep(obj);
    }
    if (uwep != obj) return FALSE;	/* rewielded old object after dying */
    /* applying weapon or tool that gets wielded ends two-weapon combat */
    if (u.twoweap)
	untwoweapon();
    if (obj->oclass != WEAPON_CLASS)
	unweapon = TRUE;
    return TRUE;
}

int
can_twoweapon()
{
	struct obj *otmp;

#define NOT_WEAPON(obj) (!is_weptool(obj) && obj->oclass != WEAPON_CLASS)
	if (!could_twoweap(youmonst.data)) {
		if (Upolyd)
		    You_cant("use two weapons in your current form."); /* EN You_cant("use two weapons in your current form."); */ // TODO DE
		else
		    pline("%s aren't able to use two weapons at once.", /* EN pline("%s aren't able to use two weapons at once.", */ // TODO DE
			  makeplural((flags.female && urole.name.f) ?
				     urole.name.f : urole.name.m));
	} else if (!uwep || !uswapwep)
		Your("%s%s%s empty.", uwep ? "left " : uswapwep ? "right " : "", /* EN Your("%s%s%s empty.", uwep ? "left " : uswapwep ? "right " : "", */ // TODO DE
			body_part(HAND), (!uwep && !uswapwep) ? "s are" : " is"); /* EN body_part(HAND), (!uwep && !uswapwep) ? "s are" : " is"); */ // TODO DE
	else if (NOT_WEAPON(uwep) || NOT_WEAPON(uswapwep)) {
		otmp = NOT_WEAPON(uwep) ? uwep : uswapwep;
		pline("%s %s.", Yname2(otmp), /* EN pline("%s %s.", Yname2(otmp), */ // TODO DE
		    is_plural(otmp) ? "aren't weapons" : "isn't a weapon"); /* EN is_plural(otmp) ? "aren't weapons" : "isn't a weapon"); */ // TODO DE
	} else if (bimanual(uwep) || bimanual(uswapwep)) {
		otmp = bimanual(uwep) ? uwep : uswapwep;
		pline("%s isn't one-handed.", Yname2(otmp)); /* EN pline("%s isn't one-handed.", Yname2(otmp)); */ // TODO DE
	} else if (uarms)
		You_cant("use two weapons while wearing a shield."); /* EN You_cant("use two weapons while wearing a shield."); */ // TODO DE
	else if (uswapwep->oartifact)
		pline("%s %s being held second to another weapon!", /* EN pline("%s %s being held second to another weapon!", */ // TODO DE
			Yname2(uswapwep), otense(uswapwep, "resist")); /* EN Yname2(uswapwep), otense(uswapwep, "resist")); */ // TODO DE
	else if (!uarmg && !Stone_resistance && (uswapwep->otyp == CORPSE &&
		    touch_petrifies(&mons[uswapwep->corpsenm]))) {
		char kbuf[BUFSZ];

		You("wield the %s corpse with your bare %s.", /* EN You("wield the %s corpse with your bare %s.", */ // TODO DE
		    mons[uswapwep->corpsenm].mname, body_part(HAND));
		Sprintf(kbuf, "%s corpse", an(mons[uswapwep->corpsenm].mname)); /* EN Sprintf(kbuf, "%s corpse", an(mons[uswapwep->corpsenm].mname)); */ // TODO DE
		instapetrify(kbuf);
	} else if (Glib || uswapwep->cursed) {
		if (!Glib)
			uswapwep->bknown = TRUE;
		drop_uswapwep();
	} else
		return (TRUE);
	return (FALSE);
}

void
drop_uswapwep()
{
	char str[BUFSZ];
	struct obj *obj = uswapwep;

	/* Avoid trashing makeplural's static buffer */
	Strcpy(str, makeplural(body_part(HAND)));
	Your("%s from your %s!",  aobjnam(obj, "slip"), str); /* EN Your("%s from your %s!",  aobjnam(obj, "slip"), str); */ // TODO DE
	dropx(obj);
}

int
dotwoweapon()
{
	/* You can always toggle it off */
	if (u.twoweap) {
		You("VERB_WECHSELN zu OBJECT KASUS_DATIV PRONOMEN_POSSESSIV NOUN_ERSTWAFFE."); /* EN You("switch to your primary weapon."); */
		u.twoweap = 0;
		update_inventory();
		return (0);
	}

	/* May we use two weapons? */
	if (can_twoweapon()) {
		/* Success! */
		You("VERB_KAEMPFEN ab jetzt beidh�ndig."); /* EN You("begin two-weapon combat."); */
		u.twoweap = 1;
		update_inventory();
		return (rnd(20) > ACURR(A_DEX));
	}
	return (0);
}

/*** Functions to empty a given slot ***/
/* These should be used only when the item can't be put back in
 * the slot by life saving.  Proper usage includes:
 * 1.  The item has been eaten, stolen, burned away, or rotted away.
 * 2.  Making an item disappear for a bones pile.
 */
void
uwepgone()
{
	if (uwep) {
		if (artifact_light(uwep) && uwep->lamplit) {
		    end_burn(uwep, FALSE);
		    if (!Blind) pline("%s glowing.", Tobjnam(uwep, "stop")); /* EN if (!Blind) pline("%s glowing.", Tobjnam(uwep, "stop")); */ // TODO DE
		}
		setworn((struct obj *)0, W_WEP);
		unweapon = TRUE;
		update_inventory();
	}
}

void
uswapwepgone()
{
	if (uswapwep) {
		setworn((struct obj *)0, W_SWAPWEP);
		update_inventory();
	}
}

void
uqwepgone()
{
	if (uquiver) {
		setworn((struct obj *)0, W_QUIVER);
		update_inventory();
	}
}

void
untwoweapon()
{
	if (u.twoweap) {
		You("VERB_KOENNEN nicht mehr l�nger zwei Waffen gleichzeitig benutzen."); /* EN You("can no longer use two weapons at once."); */
		u.twoweap = FALSE;
		update_inventory();
	}
	return;
}

/* Maybe rust object, or corrode it if acid damage is called for */
void
erode_obj(target, acid_dmg, fade_scrolls)
struct obj *target;		/* object (e.g. weapon or armor) to erode */
boolean acid_dmg;
boolean fade_scrolls;
{
	int erosion;
	struct monst *victim;
	boolean vismon;
	boolean visobj;

	if (!target)
	    return;
	victim = carried(target) ? &youmonst :
	    mcarried(target) ? target->ocarry : (struct monst *)0;
	vismon = victim && (victim != &youmonst) && canseemon(victim);
	visobj = !victim && cansee(bhitpos.x, bhitpos.y); /* assume thrown */

	erosion = acid_dmg ? target->oeroded2 : target->oeroded;

	if (target->greased) {
	    grease_protect(target,(char *)0,victim);
	} else if (target->oclass == SCROLL_CLASS) {
	    if(fade_scrolls && target->otyp != SCR_BLANK_PAPER
#ifdef MAIL
	    && target->otyp != SCR_MAIL
#endif
					)
	    {
		if (!Blind) {
		    if (victim == &youmonst)
			Your("%s.", aobjnam(target, "VERB_FADE")); /* EN Your("%s.", aobjnam(target, "fade")); */
		    else if (vismon)
			pline("%s's %s.", Monnam(victim), /* EN pline("%s's %s.", Monnam(victim), */ // TODO DE
			      aobjnam(target, "VERB_FADE")); /* EN aobjnam(target, "fade")); */
		    else if (visobj)
			pline_The("%s.", aobjnam(target, "VERB_FADE")); /* EN pline_The("%s.", aobjnam(target, "fade")); */
		}
		target->otyp = SCR_BLANK_PAPER;
		target->spe = 0;
	    }
	} else if (target->oerodeproof ||
		(acid_dmg ? !is_corrodeable(target) : !is_rustprone(target))) {
	    if (flags.verbose || !(target->oerodeproof && target->rknown)) {
		if (victim == &youmonst)
		    Your("%s not affected.", aobjnam(target, "are")); /* EN Your("%s not affected.", aobjnam(target, "are")); */ // TODO DE
		else if (vismon)
		    pline("%s's %s not affected.", Monnam(victim), /* EN pline("%s's %s not affected.", Monnam(victim), */ // TODO DE
			aobjnam(target, "are")); /* EN aobjnam(target, "are")); */ // TODO DE
		/* no message if not carried */
	    }
	    if (target->oerodeproof) target->rknown = TRUE;
	} else if (erosion < MAX_ERODE) {
	    if (victim == &youmonst)
		Your("%s%s!", aobjnam(target, acid_dmg ? "VERB_CORRODE" : "VERB_ROSTEN"), /* EN Your("%s%s!", aobjnam(target, acid_dmg ? "corrode" : "rust"), */
		    erosion+1 == MAX_ERODE ? " v�llig" : /* EN erosion+1 == MAX_ERODE ? " completely" : */
		    erosion ? " weiter" : ""); /* EN erosion ? " further" : ""); */
	    else if (vismon)
		pline("%s's %s%s!", Monnam(victim), /* EN pline("%s's %s%s!", Monnam(victim), */ // TODO DE
		    aobjnam(target, acid_dmg ? "VERB_CORRODE" : "VERB_ROSTEN"), /* EN aobjnam(target, acid_dmg ? "corrode" : "rust"), */
		    erosion+1 == MAX_ERODE ? " v�llig" : /* EN erosion+1 == MAX_ERODE ? " completely" : */ // TODO DE
		    erosion ? " weiter" : ""); /* EN erosion ? " further" : ""); */
	    else if (visobj)
		pline_The("%s%s!",
		    aobjnam(target, acid_dmg ? "VERB_CORRODE" : "VERB_ROSTEN"), /* EN aobjnam(target, acid_dmg ? "corrode" : "rust"), */
		    erosion+1 == MAX_ERODE ? " v�llig" : /* EN erosion+1 == MAX_ERODE ? " completely" : */
		    erosion ? " weiter" : ""); /* EN erosion ? " further" : ""); */
	    if (acid_dmg)
		target->oeroded2++;
	    else
		target->oeroded++;
	} else {
	    if (flags.verbose) {
		if (victim == &youmonst)
		    Your("%s v�llig %s.", /* EN Your("%s completely %s.", */ // TODO DE
			aobjnam(target, Blind ? "feel" : "look"), /* EN aobjnam(target, Blind ? "feel" : "look"), */ // TODO DE
			acid_dmg ? "ADJEKTIV_CORRODED" : "ADJEKTIV_ROSTIG"); /* EN acid_dmg ? "corroded" : "rusty"); */
		else if (vismon)
		    pline("%s's %s v�llig %s.", Monnam(victim), /* EN pline("%s's %s completely %s.", Monnam(victim), */ // TODO DE
			aobjnam(target, "look"), /* EN aobjnam(target, "look"), */ // TODO DE
			acid_dmg ? "ADJEKTIV_CORRODED" : "ADJEKTIV_ROSTIG"); /* EN acid_dmg ? "corroded" : "rusty"); */
		else if (visobj)
		    pline_The("%s v�llig %s.", /* EN pline_The("%s completely %s.", */ // TODO DE
			aobjnam(target, "look"), /* EN aobjnam(target, "look"), */ // TODO DE
			acid_dmg ? "ADJEKTIV_CORRODED" : "ADJEKTIV_ROSTIG"); /* EN acid_dmg ? "corroded" : "rusty"); */
	    }
	}
}

int
chwepon(otmp, amount)
register struct obj *otmp;
register int amount;
{
	const char *color = hcolor((amount < 0) ? NH_BLACK : NH_BLUE);
	const char *xtime;
	int otyp = STRANGE_OBJECT;

	if(!uwep || (uwep->oclass != WEAPON_CLASS && !is_weptool(uwep))) {
		char buf[BUFSZ];

		Sprintf(buf, "Your %s %s.", makeplural(body_part(HAND)), /* EN Sprintf(buf, "Your %s %s.", makeplural(body_part(HAND)), */ // TODO DE
			(amount >= 0) ? "twitch" : "itch"); /* EN (amount >= 0) ? "twitch" : "itch"); */ // TODO DE
		strange_feeling(otmp, buf);
		exercise(A_DEX, (boolean) (amount >= 0));
		return(0);
	}

	if (otmp && otmp->oclass == SCROLL_CLASS) otyp = otmp->otyp;

	if(uwep->otyp == WORM_TOOTH && amount >= 0) {
		uwep->otyp = CRYSKNIFE;
		uwep->oerodeproof = 0;
		Your("NOUN_WEAPON VERB_SCHEINEN jetzt sch�rfer zu sein."); /* EN Your("weapon seems sharper now."); */
		uwep->cursed = 0;
		if (otyp != STRANGE_OBJECT) makeknown(otyp);
		return(1);
	}

	if(uwep->otyp == CRYSKNIFE && amount < 0) {
		uwep->otyp = WORM_TOOTH;
		uwep->oerodeproof = 0;
		Your("NOUN_WEAPON VERB_SCHEINEN jetzt stumpfer zu sein."); /* EN Your("weapon seems duller now."); */
		if (otyp != STRANGE_OBJECT && otmp->bknown) makeknown(otyp);
		return(1);
	}

	if (amount < 0 && uwep->oartifact && restrict_name(uwep, ONAME(uwep))) {
	    if (!Blind)
		Your("%s %s.", aobjnam(uwep, "faintly glow"), color); /* EN Your("%s %s.", aobjnam(uwep, "faintly glow"), color); */ // TODO DE
	    return(1);
	}
	/* there is a (soft) upper and lower limit to uwep->spe */
	if(((uwep->spe > 5 && amount >= 0) || (uwep->spe < -5 && amount < 0))
								&& rn2(3)) {
	    if (!Blind)
	    Your("%s %s for a while and then %s.", /* EN Your("%s %s for a while and then %s.", */ // TODO DE
		 aobjnam(uwep, "violently glow"), color, /* EN aobjnam(uwep, "violently glow"), color, */ // TODO DE
		 otense(uwep, "evaporate")); /* EN otense(uwep, "evaporate")); */ // TODO DE
	    else
		Your("%s.", aobjnam(uwep, "evaporate")); /* EN Your("%s.", aobjnam(uwep, "evaporate")); */ // TODO DE

	    useupall(uwep);	/* let all of them disappear */
	    return(1);
	}
	if (!Blind) {
	    xtime = (amount*amount == 1) ? "moment" : "while"; /* EN xtime = (amount*amount == 1) ? "moment" : "while"; */ // TODO DE
	    Your("%s %s for a %s.", /* EN Your("%s %s for a %s.", */ // TODO DE
		 aobjnam(uwep, amount == 0 ? "violently glow" : "glow"), /* EN aobjnam(uwep, amount == 0 ? "violently glow" : "glow"), */ // TODO DE
		 color, xtime);
	    if (otyp != STRANGE_OBJECT && uwep->known &&
		    (amount > 0 || (amount < 0 && otmp->bknown)))
		makeknown(otyp);
	}
	uwep->spe += amount;
	if(amount > 0) uwep->cursed = 0;

	/*
	 * Enchantment, which normally improves a weapon, has an
	 * addition adverse reaction on Magicbane whose effects are
	 * spe dependent.  Give an obscure clue here.
	 */
	if (uwep->oartifact == ART_MAGICBANE && uwep->spe >= 0) {
		Your("ADJEKTIV_RECHT %s %s!", /* EN Your("right %s %sches!", */
			body_part(HAND),
			(((amount > 1) && (uwep->spe > 1)) ? "VERB_ZUCKEN" : "VERB_JUCKEN")); /* EN (((amount > 1) && (uwep->spe > 1)) ? "flin" : "it")); */
	}

	/* an elven magic clue, cookie@keebler */
	/* elven weapons vibrate warningly when enchanted beyond a limit */
	if ((uwep->spe > 5)
		&& (is_elven_weapon(uwep) || uwep->oartifact || !rn2(7)))
	    Your("%s.", /* EN Your("%s unexpectedly.", */
		aobjnam(uwep, "VERB_VIBRIEREN urpl�tzlich")); /* EN aobjnam(uwep, "suddenly vibrate")); */

	return(1);
}

int
welded(obj)
register struct obj *obj;
{
	if (obj && obj == uwep && will_weld(obj)) {
		obj->bknown = TRUE;
		return 1;
	}
	return 0;
}

void
weldmsg(obj)
register struct obj *obj;
{
	long savewornmask;

	savewornmask = obj->owornmask;
	Your("%s %s welded to your %s!", /* EN Your("%s %s welded to your %s!", */ // TODO DE
		xname(obj), otense(obj, "are"), /* EN xname(obj), otense(obj, "are"), */ // TODO DE
		bimanual(obj) ? (const char *)makeplural(body_part(HAND))
				: body_part(HAND));
	obj->owornmask = savewornmask;
}

/*wield.c*/
