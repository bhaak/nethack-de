/*	SCCS Id: @(#)pickup.c	3.4	2003/07/27	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 *	Contains code for picking objects up, and container use.
 */

#include "hack.h"

#ifdef GERMAN
# include "german.h"
# include <ctype.h>
#endif

STATIC_DCL void FDECL(simple_look, (struct obj *,BOOLEAN_P));
#ifndef GOLDOBJ
STATIC_DCL boolean FDECL(query_classes, (char *,boolean *,boolean *,
		const char *,struct obj *,BOOLEAN_P,BOOLEAN_P,int *));
#else
STATIC_DCL boolean FDECL(query_classes, (char *,boolean *,boolean *,
		const char *,struct obj *,BOOLEAN_P,int *));
#endif
STATIC_DCL void FDECL(check_here, (BOOLEAN_P));
STATIC_DCL boolean FDECL(n_or_more, (struct obj *));
STATIC_DCL boolean FDECL(all_but_uchain, (struct obj *));
#if 0 /* not used */
STATIC_DCL boolean FDECL(allow_cat_no_uchain, (struct obj *));
#endif
STATIC_DCL int FDECL(autopick, (struct obj*, int, menu_item **));
STATIC_DCL int FDECL(count_categories, (struct obj *,int));
STATIC_DCL long FDECL(carry_count,
		      (struct obj *,struct obj *,long,BOOLEAN_P,int *,int *));
STATIC_DCL int FDECL(lift_object, (struct obj *,struct obj *,long *,BOOLEAN_P));
STATIC_DCL boolean FDECL(mbag_explodes, (struct obj *,int));
STATIC_PTR int FDECL(in_container,(struct obj *));
STATIC_PTR int FDECL(ck_bag,(struct obj *));
STATIC_PTR int FDECL(out_container,(struct obj *));
STATIC_DCL long FDECL(mbag_item_gone, (int,struct obj *));
STATIC_DCL void FDECL(observe_quantum_cat, (struct obj *));
STATIC_DCL int FDECL(menu_loot, (int, struct obj *, BOOLEAN_P));
STATIC_DCL int FDECL(in_or_out_menu, (const char *,struct obj *, BOOLEAN_P, BOOLEAN_P));
STATIC_DCL int FDECL(container_at, (int, int, BOOLEAN_P));
STATIC_DCL boolean FDECL(able_to_loot, (int, int));
STATIC_DCL boolean FDECL(mon_beside, (int, int));

/* define for query_objlist() and autopickup() */
#define FOLLOW(curr, flags) \
    (((flags) & BY_NEXTHERE) ? (curr)->nexthere : (curr)->nobj)

/*
 *  How much the weight of the given container will change when the given
 *  object is removed from it.  This calculation must match the one used
 *  by weight() in mkobj.c.
 */
#define DELTA_CWT(cont,obj)		\
    ((cont)->cursed ? (obj)->owt * 2 :	\
		      1 + ((obj)->owt / ((cont)->blessed ? 4 : 2)))
#define GOLD_WT(n)		(((n) + 50L) / 100L)
/* if you can figure this out, give yourself a hearty pat on the back... */
#define GOLD_CAPACITY(w,n)	(((w) * -100L) - ((n) + 50L) - 1L)

static const char moderateloadmsg[] = "SUBJECT PRONOMEN_PERSONAL VERB_HABEN etwas M�he, OBJECT"; /* EN static const char moderateloadmsg[] = "You have a little trouble lifting"; */
static const char nearloadmsg[] = "SUBJECT PRONOMEN_PERSONAL VERB_HABEN ziemliche M�he, OBJECT"; /* EN static const char nearloadmsg[] = "You have much trouble lifting"; */
static const char overloadmsg[] = "SUBJECT PRONOMEN_PERSONAL VERB_HABEN extreme Probleme, OBJECT"; /* EN static const char overloadmsg[] = "You have extreme difficulty lifting"; */

/* BUG: this lets you look at cockatrice corpses while blind without
   touching them */
/* much simpler version of the look-here code; used by query_classes() */
STATIC_OVL void
simple_look(otmp, here)
struct obj *otmp;	/* list of objects */
boolean here;		/* flag for type of obj list linkage */
{
	/* Neither of the first two cases is expected to happen, since
	 * we're only called after multiple classes of objects have been
	 * detected, hence multiple objects must be present.
	 */
	if (!otmp) {
	    impossible("simple_look(null)");
	} else if (!(here ? otmp->nexthere : otmp->nobj)) {
	    pline("SUBJECT %s", doname(otmp)); /* EN pline("%s", doname(otmp)); */
	} else {
	    winid tmpwin = create_nhwindow(NHW_MENU);
	    putstr(tmpwin, 0, "");
	    do {
		putstr(tmpwin, 0, doname(otmp));
		otmp = here ? otmp->nexthere : otmp->nobj;
	    } while (otmp);
	    display_nhwindow(tmpwin, TRUE);
	    destroy_nhwindow(tmpwin);
	}
}

#ifndef GOLDOBJ
int
collect_obj_classes(ilets, otmp, here, incl_gold, filter, itemcount)
char ilets[];
register struct obj *otmp;
boolean here, incl_gold;
boolean FDECL((*filter),(OBJ_P));
int *itemcount;
#else
int
collect_obj_classes(ilets, otmp, here, filter, itemcount)
char ilets[];
register struct obj *otmp;
boolean here;
boolean FDECL((*filter),(OBJ_P));
int *itemcount;
#endif
{
	register int iletct = 0;
	register char c;

	*itemcount = 0;
#ifndef GOLDOBJ
	if (incl_gold)
	    ilets[iletct++] = def_oc_syms[COIN_CLASS];
#endif
	ilets[iletct] = '\0'; /* terminate ilets so that index() will work */
	while (otmp) {
	    c = def_oc_syms[(int)otmp->oclass];
	    if (!index(ilets, c) && (!filter || (*filter)(otmp)))
		ilets[iletct++] = c,  ilets[iletct] = '\0';
	    *itemcount += 1;
	    otmp = here ? otmp->nexthere : otmp->nobj;
	}

	return iletct;
}

/*
 * Suppose some '?' and '!' objects are present, but '/' objects aren't:
 *	"a" picks all items without further prompting;
 *	"A" steps through all items, asking one by one;
 *	"?" steps through '?' items, asking, and ignores '!' ones;
 *	"/" becomes 'A', since no '/' present;
 *	"?a" or "a?" picks all '?' without further prompting;
 *	"/a" or "a/" becomes 'A' since there aren't any '/'
 *	    (bug fix:  3.1.0 thru 3.1.3 treated it as "a");
 *	"?/a" or "a?/" or "/a?",&c picks all '?' even though no '/'
 *	    (ie, treated as if it had just been "?a").
 */
#ifndef GOLDOBJ
STATIC_OVL boolean
query_classes(oclasses, one_at_a_time, everything, action, objs,
	      here, incl_gold, menu_on_demand)
char oclasses[];
boolean *one_at_a_time, *everything;
const char *action;
struct obj *objs;
boolean here, incl_gold;
int *menu_on_demand;
#else
STATIC_OVL boolean
query_classes(oclasses, one_at_a_time, everything, action, objs,
	      here, menu_on_demand)
char oclasses[];
boolean *one_at_a_time, *everything;
const char *action;
struct obj *objs;
boolean here;
int *menu_on_demand;
#endif
{
	char ilets[20], inbuf[BUFSZ];
	int iletct, oclassct;
	boolean not_everything;
	char qbuf[QBUFSZ];
	boolean m_seen;
	int itemcount;

	oclasses[oclassct = 0] = '\0';
	*one_at_a_time = *everything = m_seen = FALSE;
	iletct = collect_obj_classes(ilets, objs, here,
#ifndef GOLDOBJ
				     incl_gold,
#endif
				     (boolean FDECL((*),(OBJ_P))) 0, &itemcount);
	if (iletct == 0) {
		return FALSE;
	} else if (iletct == 1) {
		oclasses[0] = def_char_to_objclass(ilets[0]);
		oclasses[1] = '\0';
		if (itemcount && menu_on_demand) {
			ilets[iletct++] = 'm';
			*menu_on_demand = 0;
			ilets[iletct] = '\0';
		}
	} else  {	/* more than one choice available */
		const char *where = 0;
		register char sym, oc_of_sym, *p;
		/* additional choices */
		ilets[iletct++] = ' ';
		ilets[iletct++] = 'a';
		ilets[iletct++] = 'A';
		ilets[iletct++] = (objs == invent ? 'i' : ':');
		if (menu_on_demand) {
			ilets[iletct++] = 'm';
			*menu_on_demand = 0;
		}
		ilets[iletct] = '\0';
ask_again:
		oclasses[oclassct = 0] = '\0';
		*one_at_a_time = *everything = FALSE;
		not_everything = FALSE;
		Sprintf(qbuf,"Welche Sachen MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL MODIFIER_VERB_INFINITIV %s? [%s]", /* EN Sprintf(qbuf,"What kinds of thing do you want to %s? [%s]", */
			action, ilets);
		getlin(qbuf,inbuf);
		if (*inbuf == '\033') return FALSE;

		for (p = inbuf; (sym = *p++); ) {
		    /* new A function (selective all) added by GAN 01/09/87 */
		    if (sym == ' ') continue;
		    else if (sym == 'A') *one_at_a_time = TRUE;
		    else if (sym == 'a') *everything = TRUE;
		    else if (sym == ':') {
			simple_look(objs, here);  /* dumb if objs==invent */
			goto ask_again;
		    } else if (sym == 'i') {
			(void) display_inventory((char *)0, TRUE);
			goto ask_again;
		    } else if (sym == 'm') {
			m_seen = TRUE;
		    } else {
			oc_of_sym = def_char_to_objclass(sym);
			if (index(ilets,sym)) {
			    add_valid_menu_class(oc_of_sym);
			    oclasses[oclassct++] = oc_of_sym;
			    oclasses[oclassct] = '\0';
			} else {
			    if (!where)
				where = !strcmp(action,"aufheben")  ? "Hier" : /* EN where = !strcmp(action,"pick up")  ? "here" : */
					!strcmp(action,"herausnehmen") ?  /* EN !strcmp(action,"take out") ? */
							    "Da drinnen" : ""; /* EN "inside" : ""; */
			    if (*where)
				pline("SUBJECT %s VERB_SEIN keine %cs.", where, sym); /* EN There("are no %c's %s.", sym, where); */
			    else
				You("VERB_HABEN keine %cs.", sym); /* EN You("have no %c's.", sym); */
			    not_everything = TRUE;
			}
		    }
		}
		if (m_seen && menu_on_demand) {
			*menu_on_demand = (*everything || !oclassct) ? -2 : -3;
			return FALSE;
		}
		if (!oclassct && (!*everything || not_everything)) {
		    /* didn't pick anything,
		       or tried to pick something that's not present */
		    *one_at_a_time = TRUE;	/* force 'A' */
		    *everything = FALSE;	/* inhibit 'a' */
		}
	}
	return TRUE;
}

/* look at the objects at our location, unless there are too many of them */
STATIC_OVL void
check_here(picked_some)
boolean picked_some;
{
	register struct obj *obj;
	register int ct = 0;

	/* count the objects here */
	for (obj = level.objects[u.ux][u.uy]; obj; obj = obj->nexthere) {
	    if (obj != uchain)
		ct++;
	}

	/* If there are objects here, take a look. */
	if (ct) {
	    if (flags.run) nomul(0);
	    flush_screen(1);
	    (void) look_here(ct, picked_some);
	} else {
	    read_engr_at(u.ux,u.uy);
	}
}

/* Value set by query_objlist() for n_or_more(). */
static long val_for_n_or_more;

/* query_objlist callback: return TRUE if obj's count is >= reference value */
STATIC_OVL boolean
n_or_more(obj)
struct obj *obj;
{
    if (obj == uchain) return FALSE;
    return (obj->quan >= val_for_n_or_more);
}

/* List of valid menu classes for query_objlist() and allow_category callback */
static char valid_menu_classes[MAXOCLASSES + 2];

void
add_valid_menu_class(c)
int c;
{
	static int vmc_count = 0;

	if (c == 0)  /* reset */
	  vmc_count = 0;
	else
	  valid_menu_classes[vmc_count++] = (char)c;
	valid_menu_classes[vmc_count] = '\0';
}

/* query_objlist callback: return TRUE if not uchain */
STATIC_OVL boolean
all_but_uchain(obj)
struct obj *obj;
{
    return (obj != uchain);
}

/* query_objlist callback: return TRUE */
/*ARGSUSED*/
boolean
allow_all(obj)
struct obj *obj;
{
    return TRUE;
}

boolean
allow_category(obj)
struct obj *obj;
{
    if (Role_if(PM_PRIEST)) obj->bknown = TRUE;
    if (((index(valid_menu_classes,'u') != (char *)0) && obj->unpaid) ||
	(index(valid_menu_classes, obj->oclass) != (char *)0))
	return TRUE;
    else if (((index(valid_menu_classes,'U') != (char *)0) &&
	(obj->oclass != COIN_CLASS && obj->bknown && !obj->blessed && !obj->cursed)))
	return TRUE;
    else if (((index(valid_menu_classes,'B') != (char *)0) &&
	(obj->oclass != COIN_CLASS && obj->bknown && obj->blessed)))
	return TRUE;
    else if (((index(valid_menu_classes,'C') != (char *)0) &&
	(obj->oclass != COIN_CLASS && obj->bknown && obj->cursed)))
	return TRUE;
    else if (((index(valid_menu_classes,'X') != (char *)0) &&
	(obj->oclass != COIN_CLASS && !obj->bknown)))
	return TRUE;
    else
	return FALSE;
}

#if 0 /* not used */
/* query_objlist callback: return TRUE if valid category (class), no uchain */
STATIC_OVL boolean
allow_cat_no_uchain(obj)
struct obj *obj;
{
    if ((obj != uchain) &&
	(((index(valid_menu_classes,'u') != (char *)0) && obj->unpaid) ||
	(index(valid_menu_classes, obj->oclass) != (char *)0)))
	return TRUE;
    else
	return FALSE;
}
#endif

/* query_objlist callback: return TRUE if valid class and worn */
boolean
is_worn_by_type(otmp)
register struct obj *otmp;
{
	return((boolean)(!!(otmp->owornmask &
			(W_ARMOR | W_RING | W_AMUL | W_TOOL | W_WEP | W_SWAPWEP | W_QUIVER)))
	        && (index(valid_menu_classes, otmp->oclass) != (char *)0));
}

/*
 * Have the hero pick things from the ground
 * or a monster's inventory if swallowed.
 *
 * Arg what:
 *	>0  autopickup
 *	=0  interactive
 *	<0  pickup count of something
 *
 * Returns 1 if tried to pick something up, whether
 * or not it succeeded.
 */
int
pickup(what)
int what;		/* should be a long */
{
	int i, n, res, count, n_tried = 0, n_picked = 0;
	menu_item *pick_list = (menu_item *) 0;
	boolean autopickup = what > 0;
	struct obj *objchain;
	int traverse_how;

	if (what < 0)		/* pick N of something */
	    count = -what;
	else			/* pick anything */
	    count = 0;

	if (!u.uswallow) {
		struct trap *ttmp = t_at(u.ux, u.uy);
		/* no auto-pick if no-pick move, nothing there, or in a pool */
		if (autopickup && (flags.nopick || !OBJ_AT(u.ux, u.uy) ||
			(is_pool(u.ux, u.uy) && !Underwater) || is_lava(u.ux, u.uy))) {
			read_engr_at(u.ux, u.uy);
			return (0);
		}

		/* no pickup if levitating & not on air or water level */
		if (!can_reach_floor()) {
		    if ((multi && !flags.run) || (autopickup && !flags.pickup))
			read_engr_at(u.ux, u.uy);
		    return (0);
		}
		if (ttmp && ttmp->tseen) {
		    /* Allow pickup from holes and trap doors that you escaped
		     * from because that stuff is teetering on the edge just
		     * like you, but not pits, because there is an elevation
		     * discrepancy with stuff in pits.
		     */
		    if ((ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT) &&
			(!u.utrap || (u.utrap && u.utraptype != TT_PIT))) {
			read_engr_at(u.ux, u.uy);
			return(0);
		    }
		}
		/* multi && !flags.run means they are in the middle of some other
		 * action, or possibly paralyzed, sleeping, etc.... and they just
		 * teleported onto the object.  They shouldn't pick it up.
		 */
		if ((multi && !flags.run) || (autopickup && !flags.pickup)) {
		    check_here(FALSE);
		    return (0);
		}
		if (notake(youmonst.data)) {
		    if (!autopickup)
			You("VERB_SEIN k�rperlich au�erstande etwas aufzuheben."); /* EN You("are physically incapable of picking anything up."); */
		    else
			check_here(FALSE);
		    return (0);
		}

		/* if there's anything here, stop running */
		if (OBJ_AT(u.ux,u.uy) && flags.run && flags.run != 8 && !flags.nopick) nomul(0);
	}

	add_valid_menu_class(0);	/* reset */
	if (!u.uswallow) {
		objchain = level.objects[u.ux][u.uy];
		traverse_how = BY_NEXTHERE;
	} else {
		objchain = u.ustuck->minvent;
		traverse_how = 0;	/* nobj */
	}
	/*
	 * Start the actual pickup process.  This is split into two main
	 * sections, the newer menu and the older "traditional" methods.
	 * Automatic pickup has been split into its own menu-style routine
	 * to make things less confusing.
	 */
	if (autopickup) {
	    n = autopick(objchain, traverse_how, &pick_list);
	    goto menu_pickup;
	}

	if (flags.menu_style != MENU_TRADITIONAL || iflags.menu_requested) {

	    /* use menus exclusively */
	    if (count) {	/* looking for N of something */
		char buf[QBUFSZ];
		Sprintf(buf, "Wovon %d St�ck aufheben?", count); /* EN Sprintf(buf, "Pick %d of what?", count); */
		val_for_n_or_more = count;	/* set up callback selector */
		n = query_objlist(buf, objchain,
			    traverse_how|AUTOSELECT_SINGLE|INVORDER_SORT,
			    &pick_list, PICK_ONE, n_or_more);
		/* correct counts, if any given */
		for (i = 0; i < n; i++)
		    pick_list[i].count = count;
	    } else {
		n = query_objlist("Was aufheben?", objchain, /* EN n = query_objlist("Pick up what?", objchain, */
			traverse_how|AUTOSELECT_SINGLE|INVORDER_SORT|FEEL_COCKATRICE,
			&pick_list, PICK_ANY, all_but_uchain);
	    }
menu_pickup:
	    n_tried = n;
	    for (n_picked = i = 0 ; i < n; i++) {
		res = pickup_object(pick_list[i].item.a_obj,pick_list[i].count,
					FALSE);
		if (res < 0) break;	/* can't continue */
		n_picked += res;
	    }
	    if (pick_list) free((genericptr_t)pick_list);

	} else {
	    /* old style interface */
	    int ct = 0;
	    long lcount;
	    boolean all_of_a_type, selective;
	    char oclasses[MAXOCLASSES];
	    struct obj *obj, *obj2;

	    oclasses[0] = '\0';		/* types to consider (empty for all) */
	    all_of_a_type = TRUE;	/* take all of considered types */
	    selective = FALSE;		/* ask for each item */

	    /* check for more than one object */
	    for (obj = objchain;
		  obj; obj = (traverse_how == BY_NEXTHERE) ? obj->nexthere : obj->nobj)
		ct++;

	    if (ct == 1 && count) {
		/* if only one thing, then pick it */
		obj = objchain;
		lcount = min(obj->quan, (long)count);
		n_tried++;
		if (pickup_object(obj, lcount, FALSE) > 0)
		    n_picked++;	/* picked something */
		goto end_query;

	    } else if (ct >= 2) {
		int via_menu = 0;

		pline("Hier liegen %s Gegenst�nde.", /* EN There("are %s objects here.", */
		      (ct <= 10) ? "einige" : "viele"); /* EN (ct <= 10) ? "several" : "many"); */
		if (!query_classes(oclasses, &selective, &all_of_a_type,
					 "aufheben", objchain, /* EN "pick up", objchain, */
				   traverse_how == BY_NEXTHERE,
#ifndef GOLDOBJ
				   FALSE,
#endif
				   &via_menu)) {
		    if (!via_menu) return (0);
		    n = query_objlist("Was aufheben?", /* EN n = query_objlist("Pick up what?", */
				  objchain,
				  traverse_how|(selective ? 0 : INVORDER_SORT),
				  &pick_list, PICK_ANY,
				  via_menu == -2 ? allow_all : allow_category);
		    goto menu_pickup;
		}
	    }

	    for (obj = objchain; obj; obj = obj2) {
		if (traverse_how == BY_NEXTHERE)
			obj2 = obj->nexthere;	/* perhaps obj will be picked up */
		else
			obj2 = obj->nobj;
		lcount = -1L;

		if (!selective && oclasses[0] && !index(oclasses,obj->oclass))
		    continue;

		if (!all_of_a_type) {
		    char qbuf[BUFSZ];
		    Sprintf(qbuf, "SATZBEGINN KASUS_AKKUSATIV %s aufheben?", /* EN Sprintf(qbuf, "Pick up %s?", */
			safe_qbuf("", sizeof("SATZBEGINN KASUS_AKKUSATIV  aufheben?"), doname(obj), /* EN safe_qbuf("", sizeof("Pick up ?"), doname(obj), */
					an(simple_typename(obj->otyp)), "NOUN_ETWAS")); /* EN an(simple_typename(obj->otyp)), "something")); */
		    switch ((obj->quan < 2L) ? ynaq(qbuf) : ynNaq(qbuf)) {
		    case 'q': goto end_query;	/* out 2 levels */
		    case 'n': continue;
		    case 'a':
			all_of_a_type = TRUE;
			if (selective) {
			    selective = FALSE;
			    oclasses[0] = obj->oclass;
			    oclasses[1] = '\0';
			}
			break;
		    case '#':	/* count was entered */
			if (!yn_number) continue; /* 0 count => No */
			lcount = (long) yn_number;
			if (lcount > obj->quan) lcount = obj->quan;
			/* fall thru */
		    default:	/* 'y' */
			break;
		    }
		}
		if (lcount == -1L) lcount = obj->quan;

		n_tried++;
		if ((res = pickup_object(obj, lcount, FALSE)) < 0) break;
		n_picked += res;
	    }
end_query:
	    ;	/* semicolon needed by brain-damaged compilers */
	}

	if (!u.uswallow) {
		if (!OBJ_AT(u.ux,u.uy)) u.uundetected = 0;

		/* position may need updating (invisible hero) */
		if (n_picked) newsym(u.ux,u.uy);

		/* see whether there's anything else here, after auto-pickup is done */
		if (autopickup) check_here(n_picked > 0);
	}
	return (n_tried > 0);
}

#ifdef AUTOPICKUP_EXCEPTIONS
boolean
is_autopickup_exception(obj, grab)
struct obj *obj;
boolean grab;	 /* forced pickup, rather than forced leave behind? */
{
	/*
	 *  Does the text description of this match an exception?
	 */
	char *objdesc = makesingular(doname(obj));
	struct autopickup_exception *ape = (grab) ?
					iflags.autopickup_exceptions[AP_GRAB] :
					iflags.autopickup_exceptions[AP_LEAVE];
	while (ape) {
		if (pmatch(ape->pattern, objdesc)) return TRUE;
		ape = ape->next;
	}
	return FALSE;
}
#endif /* AUTOPICKUP_EXCEPTIONS */

/*
 * Pick from the given list using flags.pickup_types.  Return the number
 * of items picked (not counts).  Create an array that returns pointers
 * and counts of the items to be picked up.  If the number of items
 * picked is zero, the pickup list is left alone.  The caller of this
 * function must free the pickup list.
 */
STATIC_OVL int
autopick(olist, follow, pick_list)
struct obj *olist;	/* the object list */
int follow;		/* how to follow the object list */
menu_item **pick_list;	/* list of objects and counts to pick up */
{
	menu_item *pi;	/* pick item */
	struct obj *curr;
	int n;
	const char *otypes = flags.pickup_types;

	/* first count the number of eligible items */
	for (n = 0, curr = olist; curr; curr = FOLLOW(curr, follow))


#ifndef AUTOPICKUP_EXCEPTIONS
	    if (!*otypes || index(otypes, curr->oclass) ||
		(flags.pickup_thrown && curr->was_thrown))
#else
	    if ((!*otypes || index(otypes, curr->oclass) ||
		flags.pickup_thrown && curr->was_thrown ||
		 is_autopickup_exception(curr, TRUE)) &&
	    	 !is_autopickup_exception(curr, FALSE))
#endif
		n++;

	if (n) {
	    *pick_list = pi = (menu_item *) alloc(sizeof(menu_item) * n);
	    for (n = 0, curr = olist; curr; curr = FOLLOW(curr, follow))
#ifndef AUTOPICKUP_EXCEPTIONS
		if (!*otypes || index(otypes, curr->oclass) ||
			(flags.pickup_thrown && curr->was_thrown)) {
#else
	    if ((!*otypes || index(otypes, curr->oclass) ||
			flags.pickup_thrown && curr->was_thrown ||
		 is_autopickup_exception(curr, TRUE)) &&
	    	 !is_autopickup_exception(curr, FALSE)) {
#endif
		    pi[n].item.a_obj = curr;
		    pi[n].count = curr->quan;
		    n++;
		}
	}
	return n;
}


/*
 * Put up a menu using the given object list.  Only those objects on the
 * list that meet the approval of the allow function are displayed.  Return
 * a count of the number of items selected, as well as an allocated array of
 * menu_items, containing pointers to the objects selected and counts.  The
 * returned counts are guaranteed to be in bounds and non-zero.
 *
 * Query flags:
 *	BY_NEXTHERE	  - Follow object list via nexthere instead of nobj.
 *	AUTOSELECT_SINGLE - Don't ask if only 1 object qualifies - just
 *			    use it.
 *	USE_INVLET	  - Use object's invlet.
 *	INVORDER_SORT	  - Use hero's pack order.
 *	SIGNAL_NOMENU	  - Return -1 rather than 0 if nothing passes "allow".
 */
int
query_objlist(qstr, olist, qflags, pick_list, how, allow)
const char *qstr;		/* query string */
struct obj *olist;		/* the list to pick from */
int qflags;			/* options to control the query */
menu_item **pick_list;		/* return list of items picked */
int how;			/* type of query */
boolean FDECL((*allow), (OBJ_P));/* allow function */
{
	int n;
	winid win;
	struct obj *curr, *last;
	char *pack;
	anything any;
	boolean printed_type_name;

	*pick_list = (menu_item *) 0;
	if (!olist) return 0;

	/* count the number of items allowed */
	for (n = 0, last = 0, curr = olist; curr; curr = FOLLOW(curr, qflags))
	    if ((*allow)(curr)) {
		last = curr;
		n++;
	    }

	if (n == 0)	/* nothing to pick here */
	    return (qflags & SIGNAL_NOMENU) ? -1 : 0;

	if (n == 1 && (qflags & AUTOSELECT_SINGLE)) {
	    *pick_list = (menu_item *) alloc(sizeof(menu_item));
	    (*pick_list)->item.a_obj = last;
	    (*pick_list)->count = last->quan;
	    return 1;
	}

	win = create_nhwindow(NHW_MENU);
	start_menu(win);
	any.a_obj = (struct obj *) 0;

	/*
	 * Run through the list and add the objects to the menu.  If
	 * INVORDER_SORT is set, we'll run through the list once for
	 * each type so we can group them.  The allow function will only
	 * be called once per object in the list.
	 */
	pack = flags.inv_order;
	do {
	    printed_type_name = FALSE;
	    for (curr = olist; curr; curr = FOLLOW(curr, qflags)) {
		if ((qflags & FEEL_COCKATRICE) && curr->otyp == CORPSE &&
		     will_feel_cockatrice(curr, FALSE)) {
			destroy_nhwindow(win);	/* stop the menu and revert */
			(void) look_here(0, FALSE);
			return 0;
		}
		if ((!(qflags & INVORDER_SORT) || curr->oclass == *pack)
							&& (*allow)(curr)) {

		    /* if sorting, print type name (once only) */
		    if (qflags & INVORDER_SORT && !printed_type_name) {
			any.a_obj = (struct obj *) 0;
			add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
					let_to_name(*pack, FALSE), MENU_UNSELECTED);
			printed_type_name = TRUE;
		    }

		    any.a_obj = curr;
		    add_menu(win, obj_to_glyph(curr), &any,
			    qflags & USE_INVLET ? curr->invlet : 0,
			    def_oc_syms[(int)objects[curr->otyp].oc_class],
			    ATR_NONE, doname_with_price(curr), MENU_UNSELECTED);
		}
	    }
	    pack++;
	} while (qflags & INVORDER_SORT && *pack);

	end_menu(win, qstr);
	n = select_menu(win, how, pick_list);
	destroy_nhwindow(win);

	if (n > 0) {
	    menu_item *mi;
	    int i;

	    /* fix up counts:  -1 means no count used => pick all */
	    for (i = 0, mi = *pick_list; i < n; i++, mi++)
		if (mi->count == -1L || mi->count > mi->item.a_obj->quan)
		    mi->count = mi->item.a_obj->quan;
	} else if (n < 0) {
	    n = 0;	/* caller's don't expect -1 */
	}
	return n;
}

/*
 * allow menu-based category (class) selection (for Drop,take off etc.)
 *
 */
int
query_category(qstr, olist, qflags, pick_list, how)
const char *qstr;		/* query string */
struct obj *olist;		/* the list to pick from */
int qflags;			/* behaviour modification flags */
menu_item **pick_list;		/* return list of items picked */
int how;			/* type of query */
{
	int n;
	winid win;
	struct obj *curr;
	char *pack;
	anything any;
	boolean collected_type_name;
	char invlet;
	int ccount;
	boolean do_unpaid = FALSE;
	boolean do_blessed = FALSE, do_cursed = FALSE, do_uncursed = FALSE,
	    do_buc_unknown = FALSE;
	int num_buc_types = 0;

	*pick_list = (menu_item *) 0;
	if (!olist) return 0;
	if ((qflags & UNPAID_TYPES) && count_unpaid(olist)) do_unpaid = TRUE;
	if ((qflags & BUC_BLESSED) && count_buc(olist, BUC_BLESSED)) {
	    do_blessed = TRUE;
	    num_buc_types++;
	}
	if ((qflags & BUC_CURSED) && count_buc(olist, BUC_CURSED)) {
	    do_cursed = TRUE;
	    num_buc_types++;
	}
	if ((qflags & BUC_UNCURSED) && count_buc(olist, BUC_UNCURSED)) {
	    do_uncursed = TRUE;
	    num_buc_types++;
	}
	if ((qflags & BUC_UNKNOWN) && count_buc(olist, BUC_UNKNOWN)) {
	    do_buc_unknown = TRUE;
	    num_buc_types++;
	}

	ccount = count_categories(olist, qflags);
	/* no point in actually showing a menu for a single category */
	if (ccount == 1 && !do_unpaid && num_buc_types <= 1 && !(qflags & BILLED_TYPES)) {
	    for (curr = olist; curr; curr = FOLLOW(curr, qflags)) {
		if ((qflags & WORN_TYPES) &&
		    !(curr->owornmask & (W_ARMOR|W_RING|W_AMUL|W_TOOL|W_WEP|W_SWAPWEP|W_QUIVER)))
		    continue;
		break;
	    }
	    if (curr) {
		*pick_list = (menu_item *) alloc(sizeof(menu_item));
		(*pick_list)->item.a_int = curr->oclass;
		return 1;
	    } else {
#ifdef DEBUG
		impossible("query_category: no single object match");
#endif
	    }
	    return 0;
	}

	win = create_nhwindow(NHW_MENU);
	start_menu(win);
	pack = flags.inv_order;
	if ((qflags & ALL_TYPES) && (ccount > 1)) {
		invlet = 'a';
		any.a_void = 0;
		any.a_int = ALL_TYPES_SELECTED;
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
		       (qflags & WORN_TYPES) ? "Alle Arten getragener Objekte" : "Alle Arten", /* EN (qflags & WORN_TYPES) ? "All worn types" : "All types", */
			MENU_UNSELECTED);
		invlet = 'b';
	} else
		invlet = 'a';
	do {
	    collected_type_name = FALSE;
	    for (curr = olist; curr; curr = FOLLOW(curr, qflags)) {
		if (curr->oclass == *pack) {
		   if ((qflags & WORN_TYPES) &&
		   		!(curr->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL |
		    	W_WEP | W_SWAPWEP | W_QUIVER)))
			 continue;
		   if (!collected_type_name) {
			any.a_void = 0;
			any.a_int = curr->oclass;
			add_menu(win, NO_GLYPH, &any, invlet++,
				def_oc_syms[(int)objects[curr->otyp].oc_class],
				ATR_NONE, let_to_name(*pack, FALSE),
				MENU_UNSELECTED);
			collected_type_name = TRUE;
		   }
		}
	    }
	    pack++;
	    if (invlet >= 'u') {
		impossible("query_category: too many categories");
		return 0;
	    }
	} while (*pack);
	/* unpaid items if there are any */
	if (do_unpaid) {
		invlet = 'u';
		any.a_void = 0;
		any.a_int = 'u';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Unbezahlte Gegenst�nde", MENU_UNSELECTED); /* EN "Unpaid items", MENU_UNSELECTED); */
	}
	/* billed items: checked by caller, so always include if BILLED_TYPES */
	if (qflags & BILLED_TYPES) {
		invlet = 'x';
		any.a_void = 0;
		any.a_int = 'x';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			 "Unbezahlte, bereits aufgebrauchte Gegenst�nde", MENU_UNSELECTED); /* EN "Unpaid items already used up", MENU_UNSELECTED); */
	}
	if (qflags & CHOOSE_ALL) {
		invlet = 'A';
		any.a_void = 0;
		any.a_int = 'A';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			(qflags & WORN_TYPES) ?
			"Auto-Auswahl aller getragenen Gegenst�nde" : /* EN "Auto-select every item being worn" : */
			"Auto-Auswahl aller Gegenst�nde", MENU_UNSELECTED); /* EN "Auto-select every item", MENU_UNSELECTED); */
	}
	/* items with b/u/c/unknown if there are any */
	if (do_blessed) {
		invlet = 'B';
		any.a_void = 0;
		any.a_int = 'B';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Als geheiligt bekannte Gegenst�nde", MENU_UNSELECTED); /* EN "Items known to be Blessed", MENU_UNSELECTED); */
	}
	if (do_cursed) {
		invlet = 'C';
		any.a_void = 0;
		any.a_int = 'C';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Als verflucht bekannte Gegenst�nde", MENU_UNSELECTED); /* EN "Items known to be Cursed", MENU_UNSELECTED); */
	}
	if (do_uncursed) {
		invlet = 'U';
		any.a_void = 0;
		any.a_int = 'U';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Als nicht verflucht bekannte Gegenst�nde", MENU_UNSELECTED); /* EN "Items known to be Uncursed", MENU_UNSELECTED); */
	}
	if (do_buc_unknown) {
		invlet = 'X';
		any.a_void = 0;
		any.a_int = 'X';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Gegenst�nde mit unbekanntem Status", /* EN "Items of unknown B/C/U status", */
			MENU_UNSELECTED);
	}
	end_menu(win, qstr);
	n = select_menu(win, how, pick_list);
	destroy_nhwindow(win);
	if (n < 0)
	    n = 0;	/* caller's don't expect -1 */
	return n;
}

STATIC_OVL int
count_categories(olist, qflags)
struct obj *olist;
int qflags;
{
	char *pack;
	boolean counted_category;
	int ccount = 0;
	struct obj *curr;

	pack = flags.inv_order;
	do {
	    counted_category = FALSE;
	    for (curr = olist; curr; curr = FOLLOW(curr, qflags)) {
		if (curr->oclass == *pack) {
		   if ((qflags & WORN_TYPES) &&
		    	!(curr->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL |
		    	W_WEP | W_SWAPWEP | W_QUIVER)))
			 continue;
		   if (!counted_category) {
			ccount++;
			counted_category = TRUE;
		   }
		}
	    }
	    pack++;
	} while (*pack);
	return ccount;
}

/* could we carry `obj'? if not, could we carry some of it/them? */
STATIC_OVL long
carry_count(obj, container, count, telekinesis, wt_before, wt_after)
struct obj *obj, *container;	/* object to pick up, bag it's coming out of */
long count;
boolean telekinesis;
int *wt_before, *wt_after;
{
    boolean adjust_wt = container && carried(container),
	    is_gold = obj->oclass == COIN_CLASS;
    int wt, iw, ow, oow;
    long qq, savequan;
#ifdef GOLDOBJ
    long umoney = money_cnt(invent);
#endif
    unsigned saveowt;
    const char *verb, *prefx1, *prefx2, *suffx;
    char obj_nambuf[BUFSZ], where[BUFSZ], verb_tmp[BUFSZ]; /* EN char obj_nambuf[BUFSZ], where[BUFSZ]; */

    savequan = obj->quan;
    saveowt = obj->owt;

    iw = max_capacity();

    if (count != savequan) {
	obj->quan = count;
	obj->owt = (unsigned)weight(obj);
    }
    wt = iw + (int)obj->owt;
    if (adjust_wt)
	wt -= (container->otyp == BAG_OF_HOLDING) ?
		(int)DELTA_CWT(container, obj) : (int)obj->owt;
#ifndef GOLDOBJ
    if (is_gold)	/* merged gold might affect cumulative weight */
	wt -= (GOLD_WT(u.ugold) + GOLD_WT(count) - GOLD_WT(u.ugold + count));
#else
    /* This will go with silver+copper & new gold weight */
    if (is_gold)	/* merged gold might affect cumulative weight */
	wt -= (GOLD_WT(umoney) + GOLD_WT(count) - GOLD_WT(umoney + count));
#endif
    if (count != savequan) {
	obj->quan = savequan;
	obj->owt = saveowt;
    }
    *wt_before = iw;
    *wt_after  = wt;

    if (wt < 0)
	return count;

    /* see how many we can lift */
    if (is_gold) {
#ifndef GOLDOBJ
	iw -= (int)GOLD_WT(u.ugold);
	if (!adjust_wt) {
	    qq = GOLD_CAPACITY((long)iw, u.ugold);
	} else {
	    oow = 0;
	    qq = 50L - (u.ugold % 100L) - 1L;
#else
	iw -= (int)GOLD_WT(umoney);
	if (!adjust_wt) {
	    qq = GOLD_CAPACITY((long)iw, umoney);
	} else {
	    oow = 0;
	    qq = 50L - (umoney % 100L) - 1L;
#endif
	    if (qq < 0L) qq += 100L;
	    for ( ; qq <= count; qq += 100L) {
		obj->quan = qq;
		obj->owt = (unsigned)GOLD_WT(qq);
#ifndef GOLDOBJ
		ow = (int)GOLD_WT(u.ugold + qq);
#else
		ow = (int)GOLD_WT(umoney + qq);
#endif
		ow -= (container->otyp == BAG_OF_HOLDING) ?
			(int)DELTA_CWT(container, obj) : (int)obj->owt;
		if (iw + ow >= 0) break;
		oow = ow;
	    }
	    iw -= oow;
	    qq -= 100L;
	}
	if (qq < 0L) qq = 0L;
	else if (qq > count) qq = count;
#ifndef GOLDOBJ
	wt = iw + (int)GOLD_WT(u.ugold + qq);
#else
	wt = iw + (int)GOLD_WT(umoney + qq);
#endif
    } else if (count > 1 || count < obj->quan) {
	/*
	 * Ugh. Calc num to lift by changing the quan of of the
	 * object and calling weight.
	 *
	 * This works for containers only because containers
	 * don't merge.		-dean
	 */
	for (qq = 1L; qq <= count; qq++) {
	    obj->quan = qq;
	    obj->owt = (unsigned)(ow = weight(obj));
	    if (adjust_wt)
		ow -= (container->otyp == BAG_OF_HOLDING) ?
			(int)DELTA_CWT(container, obj) : (int)obj->owt;
	    if (iw + ow >= 0)
		break;
	    wt = iw + ow;
	}
	--qq;
    } else {
	/* there's only one, and we can't lift it */
	qq = 0L;
    }
    obj->quan = savequan;
    obj->owt = saveowt;

    if (qq < count) {
	/* some message will be given */
	Strcpy(obj_nambuf, doname(obj));
	if (container) {
	    Sprintf(where, "NEUES_OBJECT OBJECT KASUS_DATIV in %s ", the(xname(container))); /* EN Sprintf(where, "in %s", the(xname(container))); */
	    verb = "tragen"; /* EN verb = "carry"; */
	} else {
	    Strcpy(where, ""); /* EN Strcpy(where, "lying here"); */
	    verb = telekinesis ? "bekommen" : "aufheben"; /* EN verb = telekinesis ? "acquire" : "lift"; */
	}
    } else {
	/* lint supppression */
	*obj_nambuf = *where = '\0';
	verb = "";
    }
    /* we can carry qq of them */
    if (qq > 0) {
	if (qq < count)
	    You("VERB_KOENNEN nur %s OBJECT KASUS_GENITIV ARTIKEL_BESTIMMTER %s %s%s.", /* EN You("can only %s %s of the %s %s.", */
		(qq == 1L) ? "one" : "einige", obj_nambuf, where, verb); /* EN verb, (qq == 1L) ? "one" : "some", obj_nambuf, where); */ // TODO DE
	*wt_after = wt;
	return qq;
    }

    if (!container) Strcpy(where, "hier");  /* slightly shorter form */ /* EN if (!container) Strcpy(where, "here");  */
#ifndef GOLDOBJ
    if (invent || u.ugold) {
#else
    if (invent || umoney) {
#endif
	prefx1 = "PRONOMEN_PERSONAL VERB_KOENNEN"; /* EN prefx1 = "you cannot "; */
	prefx2 = "nichts mehr "; /* EN prefx2 = ""; */
	suffx  = ""; /* EN suffx  = " any more"; */
    } else {
	prefx1 = (obj->quan == 1L) ? pronominalisierung(xname(obj)) : "sogar one"; /* EN prefx1 = (obj->quan == 1L) ? "it " : "even one "; */ // TODO DE
	prefx2 = "VERB_SEIN OBJECT f�r PRONOMEN_PERSONAL zum "; /* EN prefx2 = "is too heavy for you to "; */
	suffx  = " zu schwer"; /* EN suffx  = ""; */
#ifdef GERMAN
	/* "verb[0] = toupper(verb[0]);" crashes with a segmentation fault */
	Strcpy(verb_tmp, verb);
	verb_tmp[0] = toupper(verb[0]);
	verb = verb_tmp;
#endif
    }
    pline("SUBJECT_IM_SATZ Es %s %s %s, NEUER_SATZ SUBJECT_IM_SATZ aber %s %s%s%s.", /* EN There("%s %s %s, but %s%s%s%s.", */
	  otense(obj, "VERB_LIEGEN"), obj_nambuf, where, /* EN otense(obj, "are"), obj_nambuf, where, */
	  prefx1, prefx2, verb, suffx);

 /* *wt_after = iw; */
    return 0L;
}

/* determine whether character is able and player is willing to carry `obj' */
STATIC_OVL
int 
lift_object(obj, container, cnt_p, telekinesis)
struct obj *obj, *container;	/* object to pick up, bag it's coming out of */
long *cnt_p;
boolean telekinesis;
{
    int result, old_wt, new_wt, prev_encumbr, next_encumbr;

    if (obj->otyp == BOULDER && In_sokoban(&u.uz)) {
	You("cannot get your %s around this %s.", /* EN You("cannot get your %s around this %s.", */ // TODO DE
			body_part(HAND), xname(obj));
	return -1;
    }
    if (obj->otyp == LOADSTONE ||
	    (obj->otyp == BOULDER && throws_rocks(youmonst.data)))
	return 1;		/* lift regardless of current situation */

    *cnt_p = carry_count(obj, container, *cnt_p, telekinesis, &old_wt, &new_wt);
    if (*cnt_p < 1L) {
	result = -1;	/* nothing lifted */
#ifndef GOLDOBJ
    } else if (obj->oclass != COIN_CLASS && inv_cnt() >= 52 &&
		!merge_choice(invent, obj)) {
#else
    } else if (inv_cnt() >= 52 && !merge_choice(invent, obj)) {
#endif
	You("VERB_KOENNEN OBJECT KASUS_DATIV in PRONOMEN_POSSESSIV NOUN_RUCKSACK keine weiteren Gegenst�nde unterbringen."); /* EN Your("knapsack cannot accommodate any more items."); */
	result = -1;	/* nothing lifted */
    } else {
	result = 1;
	prev_encumbr = near_capacity();
	if (prev_encumbr < flags.pickup_burden)
		prev_encumbr = flags.pickup_burden;
	next_encumbr = calc_capacity(new_wt - old_wt);
	if (next_encumbr > prev_encumbr) {
	    if (telekinesis) {
		result = 0;	/* don't lift */
	    } else {
		char qbuf[BUFSZ];
		long savequan = obj->quan;

		obj->quan = *cnt_p;
		Strcpy(qbuf,
			(next_encumbr > HVY_ENCUMBER) ? overloadmsg :
			(next_encumbr > MOD_ENCUMBER) ? nearloadmsg :
			moderateloadmsg);
		Sprintf(eos(qbuf), " %s aufzuheben. Weitermachen?", /* EN Sprintf(eos(qbuf), " %s. Continue?", */
			safe_qbuf(qbuf, sizeof(" aufzuheben. Weitermachen?"), /* EN safe_qbuf(qbuf, sizeof(" . Continue?"), */
				doname(obj), an(simple_typename(obj->otyp)), "etwas")); /* EN doname(obj), an(simple_typename(obj->otyp)), "something")); */
		obj->quan = savequan;
		switch (ynq(qbuf)) {
		case 'q':  result = -1; break;
		case 'n':  result =  0; break;
		default:   break;	/* 'y' => result == 1 */
		}
		clear_nhwindow(WIN_MESSAGE);
	    }
	}
    }

    if (obj->otyp == SCR_SCARE_MONSTER && result <= 0 && !container)
	obj->spe = 0;
    return result;
}

/* To prevent qbuf overflow in prompts use planA only
 * if it fits, or planB if PlanA doesn't fit,
 * finally using the fallback as a last resort.
 * last_restort is expected to be very short.
 */
const char *
safe_qbuf(qbuf, padlength, planA, planB, last_resort)
const char *qbuf, *planA, *planB, *last_resort;
unsigned padlength;
{
	/* convert size_t (or int for ancient systems) to ordinary unsigned */
	unsigned len_qbuf = (unsigned)strlen(qbuf),
	         len_planA = (unsigned)strlen(planA),
	         len_planB = (unsigned)strlen(planB),
	         len_lastR = (unsigned)strlen(last_resort);
	unsigned textleft = QBUFSZ - (len_qbuf + padlength);

	if (len_lastR >= textleft) {
	    impossible("safe_qbuf: last_resort too large at %u characters.",
		       len_lastR);
	    return "";
	}
	return (len_planA < textleft) ? planA :
		    (len_planB < textleft) ? planB : last_resort;
}

/*
 * Pick up <count> of obj from the ground and add it to the hero's inventory.
 * Returns -1 if caller should break out of its loop, 0 if nothing picked
 * up, 1 if otherwise.
 */
int
pickup_object(obj, count, telekinesis)
struct obj *obj;
long count;
boolean telekinesis;	/* not picking it up directly by hand */
{
	int res, nearload;
#ifndef GOLDOBJ
	char where[BUFSZ]; /* EN */
	Strcpy(where, (obj->ox == u.ux && obj->oy == u.uy) ?  /* EN const char *where = (obj->ox == u.ux && obj->oy == u.uy) ? */
			    "hier" : "dort"); /* EN "here" : "there"; */
#endif

	if (obj->quan < count) {
	    impossible("pickup_object: count %ld > quan %ld?",
		count, obj->quan);
	    return 0;
	}

	/* In case of auto-pickup, where we haven't had a chance
	   to look at it yet; affects docall(SCR_SCARE_MONSTER). */
	if (!Blind)
#ifdef INVISIBLE_OBJECTS
		if (!obj->oinvis || See_invisible)
#endif
		obj->dknown = 1;

	if (obj == uchain) {    /* do not pick up attached chain */
	    return 0;
	} else if (obj->oartifact && !touch_artifact(obj,&youmonst)) {
	    return 0;
#ifndef GOLDOBJ
	} else if (obj->oclass == COIN_CLASS) {
	    /* Special consideration for gold pieces... */
	    long iw = (long)max_capacity() - GOLD_WT(u.ugold);
	    long gold_capacity = GOLD_CAPACITY(iw, u.ugold);

	    if (gold_capacity <= 0L) {
		pline(
	       "%s %s %ld NOUN_GOLD_PIECE%s, aber SUBJECT_IM_SATZ PRONOMEN_PERSONAL VERB_KOENNEN nicht mehr tragen.", /* EN "There %s %ld gold piece%s %s, but you cannot carry any more.", */
		      upstart(where), otense(obj, "liegen"), /* EN otense(obj, "are"), */
		      obj->quan, plur(obj->quan)); /* EN obj->quan, plur(obj->quan), where); */
		return 0;
	    } else if (gold_capacity < count) {
		You("VERB_KOENNEN nur %s KASUS_GENITIV ARTIKEL_BESTIMMTER %s liegenden %ld NOUN_GOLD_PIECEs %s.", /* EN You("can only %s %s of the %ld gold pieces lying %s.", */
		    gold_capacity == 1L ? "eine" : "einen Teil", where, obj->quan, /* EN */
		    telekinesis ? "bekommen" : "tragen" /* EN telekinesis ? "acquire" : "carry", */
        ); /* EN gold_capacity == 1L ? "one" : "some", obj->quan, where); */
		pline("%s %ld NOUN_GOLD_PIECE%s aufzuheben.", /* EN pline("%s %ld gold piece%s.", */
		    nearloadmsg, gold_capacity, plur(gold_capacity));
		u.ugold += gold_capacity;
		obj->quan -= gold_capacity;
		costly_gold(obj->ox, obj->oy, gold_capacity);
	    } else {
		u.ugold += count;
		if ((nearload = near_capacity()) != 0)
		    pline("%s %ld NOUN_GOLD_PIECE%s aufzuheben.", /* EN pline("%s %ld gold piece%s.", */
			  nearload < MOD_ENCUMBER ?
			  moderateloadmsg : nearloadmsg,
			  count, plur(count));
		else
		    prinv((char *) 0, obj, count, (char *)0);
		costly_gold(obj->ox, obj->oy, count);
		if (count == obj->quan)
		    delobj(obj);
		else
		    obj->quan -= count;
	    }
	    flags.botl = 1;
	    if (flags.run) nomul(0);
	    return 1;
#endif
	} else if (obj->otyp == CORPSE) {
	    if ( (touch_petrifies(&mons[obj->corpsenm])) && !uarmg
				&& !Stone_resistance && !telekinesis) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
			char kbuf[BUFSZ];

			Strcpy(kbuf, an(corpse_xname(obj, TRUE)));
			pline("SATZBEGINN KASUS_AKKUSATIV %s zu ber�hren ist ein fataler Fehler.", kbuf); /* EN pline("Touching %s is a fatal mistake.", kbuf); */
#ifdef GERMAN
			Sprintf(kbuf, "wegen KASUS_GENITIV %s", an(corpse_xname(obj, TRUE)));
#endif
			instapetrify(kbuf);
		    return -1;
		}
	    } else if (is_rider(&mons[obj->corpsenm])) {
		pline("KASUS_DATIV Bei PRONOMEN_POSSESSIV %s beginnt der Kadaver sich pl�tzlich zu bewegen ...", /* EN pline("At your %s, the corpse suddenly moves...", */
			telekinesis ? "Beschaffungs-NOUN_VERSUCH" : "NOUN_BERUEHRUNG"); /* EN telekinesis ? "attempted acquisition" : "touch"); */
		(void) revive_corpse(obj);
		exercise(A_WIS, FALSE);
		return -1;
	    }
	} else  if (obj->otyp == SCR_SCARE_MONSTER) {
	    if (obj->blessed) obj->blessed = 0;
	    else if (!obj->spe && !obj->cursed) obj->spe = 1;
	    else {
		pline_The("NOUN_SCROLL%s %s zu Staub, als NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL sie %s VERB_WOLLEN.", /* EN pline_The("scroll%s %s to dust as you %s %s up.", */
			plur(obj->quan), otense(obj, "VERB_ZERFALLEN"), /* EN plur(obj->quan), otense(obj, "turn"), */
			telekinesis ? "anheben" : "aufheben"); /* EN telekinesis ? "raise" : "pick", */
			/* EN (obj->quan == 1L) ? "it" : "them"); */
		if (!(objects[SCR_SCARE_MONSTER].oc_name_known) &&
				    !(objects[SCR_SCARE_MONSTER].oc_uname))
		    docall(obj);
		useupf(obj, obj->quan);
		return 1;	/* tried to pick something up and failed, but
				   don't want to terminate pickup loop yet   */
	    }
	}

	if ((res = lift_object(obj, (struct obj *)0, &count, telekinesis)) <= 0)
	    return res;

#ifdef GOLDOBJ
        /* Whats left of the special case for gold :-) */
	if (obj->oclass == COIN_CLASS) flags.botl = 1;
#endif
	if (obj->quan != count && obj->otyp != LOADSTONE)
	    obj = splitobj(obj, count);

	obj = pick_obj(obj);
	obj->was_thrown = 0;

	if (uwep && uwep == obj) mrg_to_wielded = TRUE;
	nearload = near_capacity();
	prinv(nearload == SLT_ENCUMBER ? moderateloadmsg : (char *) 0, // TODO DE
	      obj, count, nearload == SLT_ENCUMBER ? "aufzuheben" : (char *)0);
	mrg_to_wielded = FALSE;
	return 1;
}

/*
 * Do the actual work of picking otmp from the floor or monster's interior
 * and putting it in the hero's inventory.  Take care of billing.  Return a
 * pointer to the object where otmp ends up.  This may be different
 * from otmp because of merging.
 *
 * Gold never reaches this routine unless GOLDOBJ is defined.
 */
struct obj *
pick_obj(otmp)
struct obj *otmp;
{
	obj_extract_self(otmp);
	if (!u.uswallow && otmp != uball && costly_spot(otmp->ox, otmp->oy)) {
	    char saveushops[5], fakeshop[2];

	    /* addtobill cares about your location rather than the object's;
	       usually they'll be the same, but not when using telekinesis
	       (if ever implemented) or a grappling hook */
	    Strcpy(saveushops, u.ushops);
	    fakeshop[0] = *in_rooms(otmp->ox, otmp->oy, SHOPBASE);
	    fakeshop[1] = '\0';
	    Strcpy(u.ushops, fakeshop);
	    /* sets obj->unpaid if necessary */
	    addtobill(otmp, TRUE, FALSE, FALSE);
	    Strcpy(u.ushops, saveushops);
	    /* if you're outside the shop, make shk notice */
	    if (!index(u.ushops, *fakeshop))
		remote_burglary(otmp->ox, otmp->oy);
	}
	if (otmp->no_charge)	/* only applies to objects outside invent */
	    otmp->no_charge = 0;
	newsym(otmp->ox, otmp->oy);
	return addinv(otmp);	/* might merge it with other objects */
}

/*
 * prints a message if encumbrance changed since the last check and
 * returns the new encumbrance value (from near_capacity()).
 */
int
encumber_msg()
{
    static int oldcap = UNENCUMBERED;
    int newcap = near_capacity();

    if(oldcap < newcap) {
	switch(newcap) {
	case 1: Your("NOUN_BEWEGUNGs VERB_SEIN durch OBJECT PRONOMEN_POSSESSIV NOUN_LAST etwas verlangsamt."); /* EN case 1: Your("movements are slowed slightly by your load."); */
		break;
	case 2: You("VERB_BRINGEN OBJECT PRONOMEN_POSSESSIV NOUN_LAST ins Gleichgewicht.  NEUER_SATZ SUBJECT PRONOMEN_POSSESSIV NOUN_BEWEGUNGs VERB_SEIN erschwert."); /* EN case 2: You("rebalance your load.  Movement is difficult."); */
		break;
	case 3: You("%s unter OBJECT KASUS_DATIV PRONOMEN_POSSESSIV ADJEKTIV_SCHWER NOUN_LAST.  NEUER_SATZ SUBJECT Jede NOUN_BEWEGUNG VERB_FALLEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL schwer.", /* EN case 3: You("%s under your heavy load.  Movement is very hard.", */
		    stagger(youmonst.data, "VERB_STAGGER")); /* EN stagger(youmonst.data, "stagger")); */
		break;
	default: You("VERB_KOENNEN OBJECT PRONOMEN_PERSONAL mit NEUES_OBJECT OBJECT KASUS_DATIV PRONOMEN_DIESER NOUN_LAST %s weit bewegen!", /* EN default: You("%s move a handspan with this load!", */
		     newcap == 4 ? "gerade mal eine Handbreit" : "kein St�ck"); /* EN newcap == 4 ? "can barely" : "can't even"); */
		break;
	}
	flags.botl = 1;
    } else if(oldcap > newcap) {
	switch(newcap) {
	case 0: Your("NOUN_BEWEGUNGs VERB_SEIN jetzt unbelastet."); /* EN case 0: Your("movements are now unencumbered."); */
		break;
	case 1: Your("NOUN_BEWEGUNGs VERB_SEIN durch OBJECT PRONOMEN_POSSESSIV NOUN_LAST nur noch leicht verlangsamt."); /* EN case 1: Your("movements are only slowed slightly by your load."); */
		break;
	case 2: You("VERB_BRINGEN OBJECT PRONOMEN_POSSESSIV NOUN_LAST ins Gleichgewicht.  NEUER_SATZ SUBJECT PRONOMEN_POSSESSIV NOUN_BEWEGUNGs VERB_SEIN immer noch erschwert."); /* EN case 2: You("rebalance your load.  Movement is still difficult."); */
		break;
	case 3: You("%s unter OBJECT KASUS_DATIV PRONOMEN_POSSESSIV ADJEKTIV_SCHWER NOUN_LAST.  NEUER_SATZ SUBJECT Jede NOUN_BEWEGUNG VERB_FALLEN KASUS_DATIV PRONOMEN_PERSONAL immer noch schwer.", /* EN case 3: You("%s under your load.  Movement is still very hard.", */
		    stagger(youmonst.data, "VERB_STAGGER")); /* EN stagger(youmonst.data, "stagger")); */
		break;
	}
	flags.botl = 1;
    }

    oldcap = newcap;
    return (newcap);
}

/* Is there a container at x,y. Optional: return count of containers at x,y */
STATIC_OVL int
container_at(x, y, countem)
int x,y;
boolean countem;
{
	struct obj *cobj, *nobj;
	int container_count = 0;
	
	for(cobj = level.objects[x][y]; cobj; cobj = nobj) {
		nobj = cobj->nexthere;
		if(Is_container(cobj)) {
			container_count++;
			if (!countem) break;
		}
	}
	return container_count;
}

STATIC_OVL boolean
able_to_loot(x, y)
int x, y;
{
	if (!can_reach_floor()) {
#ifdef STEED
		if (u.usteed && P_SKILL(P_RIDING) < P_BASIC)
			rider_cant_reach(); /* not skilled enough to reach */
		else
#endif
			You("cannot reach the %s.", surface(x, y)); /* EN You("cannot reach the %s.", surface(x, y)); */ // TODO DE
		return FALSE;
	} else if (is_pool(x, y) || is_lava(x, y)) {
		/* at present, can't loot in water even when Underwater */
		You("VERB_KOENNEN keine Sachen pl�ndern, die sich tief %s befinden.", /* EN You("cannot loot things that are deep in the %s.", */
		    is_lava(x, y) ? "in der Lava" : "unter Wasser"); /* EN is_lava(x, y) ? "lava" : "water"); */
		return FALSE;
	} else if (nolimbs(youmonst.data)) {
		pline("Ohne Gliedma�en VERB_KOENNEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL nichts pl�ndern."); /* EN pline("Without limbs, you cannot loot anything."); */
		return FALSE;
	} else if (!freehand()) {
		pline("Ohne KASUS_AKKUSATIV ARTIKEL_UNBESTIMMTER %s frei, NEUER_SATZ SUBJECT_IM_SATZ VERB_KOENNEN PRONOMEN_PERSONAL nichts pl�ndern.", /* EN pline("Without a free %s, you cannot loot anything.", */
			body_part(HAND));
		return FALSE;
	}
	return TRUE;
}

STATIC_OVL boolean
mon_beside(x,y)
int x, y;
{
	int i,j,nx,ny;
	for(i = -1; i <= 1; i++)
	    for(j = -1; j <= 1; j++) {
	    	nx = x + i;
	    	ny = y + j;
		if(isok(nx, ny) && MON_AT(nx, ny))
			return TRUE;
	    }
	return FALSE;
}

int
doloot()	/* loot a container on the floor or loot saddle from mon. */
{
    register struct obj *cobj, *nobj;
    register int c = -1;
    int timepassed = 0;
    coord cc;
    boolean underfoot = TRUE;
    const char *dont_find_anything = "VERB_FINDEN nichts"; /* EN const char *dont_find_anything = "don't find anything"; */
    struct monst *mtmp;
    char qbuf[BUFSZ];
    int prev_inquiry = 0;
    boolean prev_loot = FALSE;

    if (check_capacity((char *)0)) {
	/* "Can't do that while carrying so much stuff." */
	return 0;
    }
    if (nohands(youmonst.data)) {
	You("VERB_HAVE keine H�nde!");	/* not `body_part(HAND)' */ /* EN You("have no hands!");	*/
	return 0;
    }
    cc.x = u.ux; cc.y = u.uy;

lootcont:

    if (container_at(cc.x, cc.y, FALSE)) {
	boolean any = FALSE;

	if (!able_to_loot(cc.x, cc.y)) return 0;
	for (cobj = level.objects[cc.x][cc.y]; cobj; cobj = nobj) {
	    nobj = cobj->nexthere;

	    if (Is_container(cobj)) {
		Sprintf(qbuf, "Hier liegt %s, pl�ndern?", /* EN Sprintf(qbuf, "There is %s here, loot it?", */
			safe_qbuf("", sizeof("Hier liegt  , pl�ndern?"), /* EN safe_qbuf("", sizeof("There is  here, loot it?"), */
			     doname(cobj), an(simple_typename(cobj->otyp)),
			     "ein Beh�lter")); /* EN "a container")); */
		c = ynq(qbuf);
		if (c == 'q') return (timepassed);
		if (c == 'n') continue;
		any = TRUE;

		if (cobj->olocked) {
		    pline("Hmmm, scheint abgeschlossen zu sein."); /* EN pline("Hmmm, it seems to be locked."); */
		    continue;
		}
		if (cobj->otyp == BAG_OF_TRICKS) {
		    int tmp;
		    You("VERB_OEFFNEN vorsichtig OBJECT ARTIKEL_BESTIMMTER NOUN_BAG ..."); /* EN You("carefully open the bag..."); */
		    pline("Ihm wachsen riesige Z�hne und er bei�t KASUS_AKKUSATIV PRONOMEN_PERSONAL!"); /* EN pline("It develops a huge set of teeth and bites you!"); */
		    tmp = rnd(10);
		    if (Half_physical_damage) tmp = (tmp+1) / 2;
		    losehp(tmp, "ADJEKTIV_CARNIVOROUS NOUN_BAG", KILLED_BY_AN); /* EN losehp(tmp, "carnivorous bag", KILLED_BY_AN); */
		    makeknown(BAG_OF_TRICKS);
		    timepassed = 1;
		    continue;
		}

		You("VERB_OEFFNEN vorsichtig OBJECT %s ...", the(xname(cobj))); /* EN You("carefully open %s...", the(xname(cobj))); */
		timepassed |= use_container(cobj, 0);
		if (multi < 0) return 1;		/* chest trap */
	    }
	}
	if (any) c = 'y';
    } else if (Confusion) {
#ifndef GOLDOBJ
	if (u.ugold){
	    long contribution = rnd((int)min(LARGEST_INT,u.ugold));
	    struct obj *goldob = mkgoldobj(contribution);
#else
	struct obj *goldob;
	/* Find a money object to mess with */
	for (goldob = invent; goldob; goldob = goldob->nobj) {
	    if (goldob->oclass == COIN_CLASS) break;
	}
	if (goldob){
	    long contribution = rnd((int)min(LARGEST_INT, goldob->quan));
	    if (contribution < goldob->quan)
		goldob = splitobj(goldob, contribution);
	    freeinv(goldob);
#endif
	    if (IS_THRONE(levl[u.ux][u.uy].typ)){
		struct obj *coffers;
		int pass;
		/* find the original coffers chest, or any chest */
		for (pass = 2; pass > -1; pass -= 2)
		    for (coffers = fobj; coffers; coffers = coffers->nobj)
			if (coffers->otyp == CHEST && coffers->spe == pass)
			    goto gotit;	/* two level break */
gotit:
		if (coffers) {
	    verbalize("Vielen Dank KASUS_AKKUSATIV f�r PRONOMEN_POSSESSIV NOUN_BEITRAG zum Schuldenabbau."); /* EN verbalize("Thank you for your contribution to reduce the debt."); */
		    (void) add_to_container(coffers, goldob);
		    coffers->owt = weight(coffers);
		} else {
		    struct monst *mon = makemon(courtmon(),
					    u.ux, u.uy, NO_MM_FLAGS);
		    if (mon) {
#ifndef GOLDOBJ
			mon->mgold += goldob->quan;
			delobj(goldob);
			pline("Der Finanzminister bedankt sich KASUS_AKKUSATIV f�r PRONOMEN_POSSESSIV NOUN_SPENDE."); /* EN pline("The exchequer accepts your contribution."); */
		    } else {
			dropx(goldob);
		    }
		}
	    } else {
		dropx(goldob);
#else
			add_to_minv(mon, goldob);
			pline("Der Finanzminister bedankt sich KASUS_AKKUSATIV f�r PRONOMEN_POSSESSIV NOUN_SPENDE."); /* EN pline("The exchequer accepts your contribution."); */
		    } else {
			dropy(goldob);
		    }
		}
	    } else {
		dropy(goldob);
#endif
		pline("Ok, jetzt gibt's hier was zu pl�ndern."); /* EN pline("Ok, now there is loot here."); */
	    }
	}
    } else if (IS_GRAVE(levl[cc.x][cc.y].typ)) {
	You("VERB_MUESSEN OBJECT ARTIKEL_BESTIMMTER NOUN_GRAVE ausgraben, um es pl�ndern zu k�nnen ..."); /* EN You("need to dig up the grave to effectively loot it..."); */
    }
    /*
     * 3.3.1 introduced directional looting for some things.
     */
    if (c != 'y' && mon_beside(u.ux, u.uy)) {
	if (!get_adjacent_loc("In welche Richtung pl�ndern?", "Ung�ltiger Pl�nderplatz", /* EN if (!get_adjacent_loc("Loot in what direction?", "Invalid loot location", */
			u.ux, u.uy, &cc)) return 0;
	if (cc.x == u.ux && cc.y == u.uy) {
	    underfoot = TRUE;
	    if (container_at(cc.x, cc.y, FALSE))
		goto lootcont;
	} else
	    underfoot = FALSE;
	if (u.dz < 0) {
	    You("%s zum Pl�ndern _an_ ARTIKEL_BESTIMMTER %s.", dont_find_anything, /* EN You("%s to loot on the %s.", dont_find_anything, */
		ceiling(cc.x, cc.y));
	    timepassed = 1;
	    return timepassed;
	}
	mtmp = m_at(cc.x, cc.y);
	if (mtmp) timepassed = loot_mon(mtmp, &prev_inquiry, &prev_loot);

	/* Preserve pre-3.3.1 behaviour for containers.
	 * Adjust this if-block to allow container looting
	 * from one square away to change that in the future.
	 */
	if (!underfoot) {
	    if (container_at(cc.x, cc.y, FALSE)) {
		if (mtmp) {
		    You("VERB_KOENNEN dort nichts %spl�ndern, solange OBJECT KASUS_DATIV PRONOMEN_PERSONAL NEUES_OBJECT OBJECT KASUS_NOMINATIV %s im Weg steht.", /* EN You_cant("loot anything %sthere with %s in the way.", */
			    prev_inquiry ? "anderes " : "", mon_nam(mtmp)); /* EN prev_inquiry ? "else " : "", mon_nam(mtmp)); */
		    return timepassed;
		} else {
		    You("VERB_MUESSEN neben einem Beh�lter stehen um ihn zu pl�ndern."); /* EN You("have to be at a container to loot it."); */
		}
	    } else {
		You("VERB_FINDEN dort nichts %szu pl�ndern.", /* EN You("%s %sthere to loot.", dont_find_anything, */
			(prev_inquiry || prev_loot) ? "weiter " : ""); /* EN (prev_inquiry || prev_loot) ? "else " : ""); */
		return timepassed;
	    }
	}
    } else if (c != 'y' && c != 'n') {
	You("VERB_FINDEN %s nichts zu pl�ndern.", /* EN You("%s %s to loot.", dont_find_anything, */
		    underfoot ? "hier" : "dort"); /* EN underfoot ? "here" : "there"); */
    }
    return (timepassed);
}

/* loot_mon() returns amount of time passed.
 */
int
loot_mon(mtmp, passed_info, prev_loot)
struct monst *mtmp;
int *passed_info;
boolean *prev_loot;
{
    int c = -1;
    int timepassed = 0;
#ifdef STEED
    struct obj *otmp;
    char qbuf[QBUFSZ];

    /* 3.3.1 introduced the ability to remove saddle from a steed             */
    /* 	*passed_info is set to TRUE if a loot query was given.               */
    /*	*prev_loot is set to TRUE if something was actually acquired in here. */
    if (mtmp && mtmp != u.usteed && (otmp = which_armor(mtmp, W_SADDLE))) {
	long unwornmask;
	if (passed_info) *passed_info = 1;
	Sprintf(qbuf, "SUBJECT MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL OBJECT KASUS_DATIV %s den Sattel abnehmen?", /* EN Sprintf(qbuf, "Do you want to remove the saddle from %s?", */
		x_monnam(mtmp, ARTICLE_THE, (char *)0, SUPPRESS_SADDLE, FALSE));
	if ((c = yn_function(qbuf, ynqchars, 'n')) == 'y') {
		if (nolimbs(youmonst.data)) {
		    pline("Das geht nicht ohne Gliedmassen."); /* not body_part(HAND) */ /* EN You_cant("do that without limbs."); */
		    return (0);
		}
		if (otmp->cursed) {
		    pline("Das geht nicht. Der Sattel scheint _an_ %s festzukleben.", /* EN You("can't. The saddle seems to be stuck to %s.", */
			x_monnam(mtmp, ARTICLE_THE, (char *)0,
				SUPPRESS_SADDLE, FALSE));
			    
		    /* the attempt costs you time */
			return (1);
		}
		obj_extract_self(otmp);
		if ((unwornmask = otmp->owornmask) != 0L) {
		    mtmp->misc_worn_check &= ~unwornmask;
		    otmp->owornmask = 0L;
		    update_mon_intrinsics(mtmp, otmp, FALSE, FALSE);
		}
		otmp = hold_another_object(otmp, "SUBJECT PRONOMEN_PERSONAL VERB_DROP OBJECT %s SATZKLAMMER!", doname(otmp), /* EN otmp = hold_another_object(otmp, "You drop %s!", doname(otmp), */
					(const char *)0);
		timepassed = rnd(3);
		if (prev_loot) *prev_loot = TRUE;
	} else if (c == 'q') {
		return (0);
	}
    }
#endif	/* STEED */
    /* 3.4.0 introduced the ability to pick things up from within swallower's stomach */
    if (u.uswallow) {
	int count = passed_info ? *passed_info : 0;
	timepassed = pickup(count);
    }
    return timepassed;
}

/*
 * Decide whether an object being placed into a magic bag will cause
 * it to explode.  If the object is a bag itself, check recursively.
 */
STATIC_OVL boolean
mbag_explodes(obj, depthin)
    struct obj *obj;
    int depthin;
{
    /* these won't cause an explosion when they're empty */
    if ((obj->otyp == WAN_CANCELLATION || obj->otyp == BAG_OF_TRICKS) &&
	    obj->spe <= 0)
	return FALSE;

    /* odds: 1/1, 2/2, 3/4, 4/8, 5/16, 6/32, 7/64, 8/128, 9/128, 10/128,... */
    if ((Is_mbag(obj) || obj->otyp == WAN_CANCELLATION) &&
	(rn2(1 << (depthin > 7 ? 7 : depthin)) <= depthin))
	return TRUE;
    else if (Has_contents(obj)) {
	struct obj *otmp;

	for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
	    if (mbag_explodes(otmp, depthin+1)) return TRUE;
    }
    return FALSE;
}

/* A variable set in use_container(), to be used by the callback routines   */
/* in_container(), and out_container() from askchain() and use_container(). */
static NEARDATA struct obj *current_container;
#define Icebox (current_container->otyp == ICE_BOX)

/* Returns: -1 to stop, 1 item was inserted, 0 item was not inserted. */
STATIC_PTR int
in_container(obj)
register struct obj *obj;
{
	boolean floor_container = !carried(current_container);
	boolean was_unpaid = FALSE;
	char buf[BUFSZ];

	if (!current_container) {
		impossible("<in> no current_container?");
		return 0;
	} else if (obj == uball || obj == uchain) {
		You("VERB_MACHEN wohl Witze?!"); /* EN You("must be kidding."); */
		return 0;
	} else if (obj == current_container) {
		pline("Das w�re eine interessante topologische �bung."); /* EN pline("That would be an interesting topological exercise."); */
		return 0;
	} else if (obj->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL)) {
		Norep("SUBJECT PRONOMEN_PERSONAL VERB_KOENNEN nicht etwas %s, was NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL gerade VERB_TRAGEN.", /* EN Norep("You cannot %s %s you are wearing.", */
			Icebox ? "tiefk�hlen" : "verstauen"); /* EN Icebox ? "refrigerate" : "stash", something); */
		return 0;
	} else if ((obj->otyp == LOADSTONE) && obj->cursed) {
		obj->bknown = 1;
	      pline_The("NOUN_GEM_ROCK%s VERB_WOLLEN OBJECT PRONOMEN_PERSONAL nicht verlassen.", plur(obj->quan)); /* EN pline_The("stone%s won't leave your person.", plur(obj->quan)); */
		return 0;
	} else if (obj->otyp == AMULET_OF_YENDOR ||
		   obj->otyp == CANDELABRUM_OF_INVOCATION ||
		   obj->otyp == BELL_OF_OPENING ||
		   obj->otyp == SPE_BOOK_OF_THE_DEAD) {
	/* Prohibit Amulets in containers; if you allow it, monsters can't
	 * steal them.  It also becomes a pain to check to see if someone
	 * has the Amulet.  Ditto for the Candelabrum, the Bell and the Book.
	 */
	    pline("SUBJECT %s VERB_LASSEN sich nicht in solchen Beh�ltnissen einsperren.", The(xname(obj))); /* EN pline("%s cannot be confined in such trappings.", The(xname(obj))); */
	    return 0;
	} else if (obj->otyp == LEASH && obj->leashmon != 0) {
		pline("%s attached to your pet.", Tobjnam(obj, "are")); /* EN pline("%s attached to your pet.", Tobjnam(obj, "are")); */ // TODO DE
		return 0;
	} else if (obj == uwep) {
		if (welded(obj)) {
			weldmsg(obj);
			return 0;
		}
		setuwep((struct obj *) 0);
		if (uwep) return 0;	/* unwielded, died, rewielded */
	} else if (obj == uswapwep) {
		setuswapwep((struct obj *) 0);
		if (uswapwep) return 0;     /* unwielded, died, rewielded */
	} else if (obj == uquiver) {
		setuqwep((struct obj *) 0);
		if (uquiver) return 0;     /* unwielded, died, rewielded */
	}

	if (obj->otyp == CORPSE) {
	    if ( (touch_petrifies(&mons[obj->corpsenm])) && !uarmg
		 && !Stone_resistance) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    char kbuf[BUFSZ];

		    Strcpy(kbuf, an(corpse_xname(obj, TRUE)));
		    pline("SATZBEGINN KASUS_AKKUSATIV %s zu ber�hren, ist ein fataler Fehler.", kbuf); /* EN pline("Touching %s is a fatal mistake.", kbuf); */
#ifdef GERMAN
		    Sprintf(kbuf, "wegen KASUS_GENITIV %s", an(corpse_xname(obj, TRUE)));
#endif
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	/* boxes, boulders, and big statues can't fit into any container */
	if (obj->otyp == ICE_BOX || Is_box(obj) || obj->otyp == BOULDER ||
		(obj->otyp == STATUE && bigmonst(&mons[obj->corpsenm]))) {
		/*
		 *  xname() uses a static result array.  Save obj's name
		 *  before current_container's name is computed.  Don't
		 *  use the result of strcpy() within You() --- the order
		 *  of evaluation of the parameters is undefined.
		 */
		Strcpy(buf, the(xname(obj)));
		pline("SUBJECT %s VERB_PASSEN nicht OBJECT in %s.", buf, /* EN You("cannot fit %s into %s.", buf, */
		    the(xname(current_container)));
		return 0;
	}

	freeinv(obj);

	if (obj_is_burning(obj))	/* this used to be part of freeinv() */
		(void) snuff_lit(obj);

	if (floor_container && costly_spot(u.ux, u.uy)) {
	    if (current_container->no_charge && !obj->unpaid) {
		/* don't sell when putting the item into your own container */
		obj->no_charge = 1;
	    } else if (obj->oclass != COIN_CLASS) {
		/* sellobj() will take an unpaid item off the shop bill
		 * note: coins are handled later */
		was_unpaid = obj->unpaid ? TRUE : FALSE;
		sellobj_state(SELL_DELIBERATE);
		sellobj(obj, u.ux, u.uy);
		sellobj_state(SELL_NORMAL);
	    }
	}
	if (Icebox && !age_is_relative(obj)) {
		obj->age = monstermoves - obj->age; /* actual age */
		/* stop any corpse timeouts when frozen */
		if (obj->otyp == CORPSE && obj->timed) {
			long rot_alarm = stop_timer(ROT_CORPSE, (genericptr_t)obj);
			(void) stop_timer(REVIVE_MON, (genericptr_t)obj);
			/* mark a non-reviving corpse as such */
			if (rot_alarm) obj->norevive = 1;
		}
	} else if (Is_mbag(current_container) && mbag_explodes(obj, 0)) {
		/* explicitly mention what item is triggering the explosion */
		pline(
	      "Als SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT %s hinein-VERB_TUN, NEUER_SATZ SUBJECT_IM_SATZ VERB_WERDEN PRONOMEN_PERSONAL von einer magischen Explosion erfasst!", /* EN "As you put %s inside, you are blasted by a magical explosion!", */
		      doname(obj));
		/* did not actually insert obj yet */
		if (was_unpaid) addtobill(obj, FALSE, FALSE, TRUE);
		obfree(obj, (struct obj *)0);
		delete_contents(current_container);
		if (!floor_container)
			useup(current_container);
		else if (obj_here(current_container, u.ux, u.uy))
			useupf(current_container, obj->quan);
		else
			panic("in_container:  bag not found.");

		losehp(d(6,6),"NOUN_MAGIEEXPLOSION", KILLED_BY_AN); /* EN losehp(d(6,6),"magical explosion", KILLED_BY_AN); */
		current_container = 0;	/* baggone = TRUE; */
	}

	if (current_container) {
	    Strcpy(buf, the(xname(current_container)));
	    You("VERB_LEGEN OBJECT %s NEUES_OBJECT OBJECT in %s.", doname(obj), buf); /* EN You("put %s into %s.", doname(obj), buf); */

	    /* gold in container always needs to be added to credit */
	    if (floor_container && obj->oclass == COIN_CLASS)
		sellobj(obj, current_container->ox, current_container->oy);
	    (void) add_to_container(current_container, obj);
	    current_container->owt = weight(current_container);
	}
	/* gold needs this, and freeinv() many lines above may cause
	 * the encumbrance to disappear from the status, so just always
	 * update status immediately.
	 */
	bot();

	return(current_container ? 1 : -1);
}

STATIC_PTR int
ck_bag(obj)
struct obj *obj;
{
	return current_container && obj != current_container;
}

/* Returns: -1 to stop, 1 item was removed, 0 item was not removed. */
STATIC_PTR int
out_container(obj)
register struct obj *obj;
{
	register struct obj *otmp;
	boolean is_gold = (obj->oclass == COIN_CLASS);
	int res, loadlev;
	long count;

	if (!current_container) {
		impossible("<out> no current_container?");
		return -1;
	} else if (is_gold) {
		obj->owt = weight(obj);
	}

	if(obj->oartifact && !touch_artifact(obj,&youmonst)) return 0;

	if (obj->otyp == CORPSE) {
	    if ( (touch_petrifies(&mons[obj->corpsenm])) && !uarmg
		 && !Stone_resistance) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    char kbuf[BUFSZ];

		    Strcpy(kbuf, an(corpse_xname(obj, TRUE)));
		    pline("SATZBEGINN KASUS_AKKUSATIV %s zu ber�hren, ist ein fataler Fehler.", kbuf); /* EN pline("Touching %s is a fatal mistake.", kbuf); */
#ifdef GERMAN
		    Sprintf(kbuf, "wegen KASUS_GENITIV %s", an(corpse_xname(obj, TRUE)));
#endif
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	count = obj->quan;
	if ((res = lift_object(obj, current_container, &count, FALSE)) <= 0)
	    return res;

	if (obj->quan != count && obj->otyp != LOADSTONE)
	    obj = splitobj(obj, count);

	/* Remove the object from the list. */
	obj_extract_self(obj);
	current_container->owt = weight(current_container);

	if (Icebox && !age_is_relative(obj)) {
		obj->age = monstermoves - obj->age; /* actual age */
		if (obj->otyp == CORPSE)
			start_corpse_timeout(obj);
	}
	/* simulated point of time */

	if(!obj->unpaid && !carried(current_container) &&
	     costly_spot(current_container->ox, current_container->oy)) {
		obj->ox = current_container->ox;
		obj->oy = current_container->oy;
		addtobill(obj, FALSE, FALSE, FALSE);
	}
	if (is_pick(obj) && !obj->unpaid && *u.ushops && shop_keeper(*u.ushops))
		verbalize("Du kleiner Gauner!  Scher dich zum Teufel mit deiner Hacke!"); /* EN verbalize("You sneaky cad! Get out of here with that pick!"); */

	otmp = addinv(obj);
	loadlev = near_capacity();
	prinv(loadlev ?
	      (loadlev < MOD_ENCUMBER ?
	       "You have a little trouble removing" : /* EN "You have a little trouble removing" : */ // TODO DE
	       "You have much trouble removing") : (char *)0, /* EN "You have much trouble removing") : (char *)0, */ // TODO DE
	      otmp, count, (char *)0);

	if (is_gold) {
#ifndef GOLDOBJ
		dealloc_obj(obj);
#endif
		bot();	/* update character's gold piece count immediately */
	}
	return 1;
}

/* an object inside a cursed bag of holding is being destroyed */
STATIC_OVL long
mbag_item_gone(held, item)
int held;
struct obj *item;
{
    struct monst *shkp;
    long loss = 0L;

    if (item->dknown)
	pline("SUBJECT %s %s verschwunden!", Doname2(item), otense(item, "VERB_SEIN")); /* EN pline("%s %s vanished!", Doname2(item), otense(item, "have")); */
    else
	You("%s, wie NEUER_SATZ SUBJECT_IM_SATZ %s VERB_VERSCHWINDEN!", Blind ? "VERB_BEMERKEN" : "VERB_SEHEN", doname(item)); /* EN You("%s %s disappear!", Blind ? "notice" : "see", doname(item)); */

    if (*u.ushops && (shkp = shop_keeper(*u.ushops)) != 0) {
	if (held ? (boolean) item->unpaid : costly_spot(u.ux, u.uy))
	    loss = stolen_value(item, u.ux, u.uy,
				(boolean)shkp->mpeaceful, TRUE);
    }
    obfree(item, (struct obj *) 0);
    return loss;
}

STATIC_OVL void
observe_quantum_cat(box)
struct obj *box;
{
    static NEARDATA const char sc[] = "Schr�dingers NOUN_CAT"; /* EN static NEARDATA const char sc[] = "Schroedinger's Cat"; */
    struct obj *deadcat;
    struct monst *livecat;
    xchar ox, oy;

    box->spe = 0;		/* box->owt will be updated below */
    if (get_obj_location(box, &ox, &oy, 0))
	box->ox = ox, box->oy = oy;	/* in case it's being carried */

    /* this isn't really right, since any form of observation
       (telepathic or monster/object/food detection) ought to
       force the determination of alive vs dead state; but basing
       it just on opening the box is much simpler to cope with */
    livecat = rn2(2) ? makemon(&mons[PM_HOUSECAT],
			       box->ox, box->oy, NO_MINVENT) : 0;
    if (livecat) {
	livecat->mpeaceful = 1;
	set_malign(livecat);
	if (!canspotmon(livecat))
	    You("VERB_GLAUBEN, etwas habe OBJECT PRONOMEN_POSSESSIV %s gestreift.", body_part(FOOT)); /* EN You("think %s brushed your %s.", something, body_part(FOOT)); */
	else
	    pline("SUBJECT %s in der Kiste ist noch am Leben!", Monnam(livecat)); /* EN pline("%s inside the box is still alive!", Monnam(livecat)); */
	(void) christen_monst(livecat, sc);
    } else {
	deadcat = mk_named_object(CORPSE, &mons[PM_HOUSECAT],
				  box->ox, box->oy, sc);
	if (deadcat) {
	    obj_extract_self(deadcat);
	    (void) add_to_container(box, deadcat);
	}
	pline_The("SUBJECT %s in der Kiste ist tot!", /* EN pline_The("%s inside the box is dead!", */
	    Hallucination ? rndmonnam() : "NOUN_CAT"); /* EN Hallucination ? rndmonnam() : "housecat"); */
    }
    box->owt = weight(box);
    return;
}

#undef Icebox

int
use_container(obj, held)
register struct obj *obj;
register int held;
{
	struct obj *curr, *otmp;
#ifndef GOLDOBJ
	struct obj *u_gold = (struct obj *)0;
#endif
	boolean one_by_one, allflag, quantum_cat = FALSE,
		loot_out = FALSE, loot_in = FALSE;
	char select[MAXOCLASSES+1];
	char qbuf[BUFSZ], emptymsg[BUFSZ], pbuf[QBUFSZ];
	long loss = 0L;
	int cnt = 0, used = 0,
	    menu_on_request;

	emptymsg[0] = '\0';
	if (nohands(youmonst.data)) {
		You("VERB_HAVE keine H�nde!");	/* not `body_part(HAND)' */ /* EN You("have no hands!");	*/
		return 0;
	} else if (!freehand()) {
		You("VERB_HAVE PRONOMEN_KEIN ADJEKTIV_FREI %s.", body_part(HAND)); /* EN You("have no free %s.", body_part(HAND)); */
		return 0;
	}
	if (obj->olocked) {
	    pline("SUBJECT %s abgeschlossen zu sein.", Tobjnam(obj, "VERB_SCHEINEN")); /* EN pline("%s to be locked.", Tobjnam(obj, "seem")); */
	    if (held) You("VERB_MUESSEN sie abstellen um sie aufzuschlie�en."); /* EN if (held) You("must put it down to unlock."); */
	    return 0;
	} else if (obj->otrapped) {
	    if (held) You("VERB_OPEN OBJECT %s ...", the(xname(obj))); /* EN if (held) You("open %s...", the(xname(obj))); */
	    (void) chest_trap(obj, HAND, FALSE);
	    /* even if the trap fails, you've used up this turn */
	    if (multi >= 0) {	/* in case we didn't become paralyzed */
		nomul(-1);
		nomovemsg = "";
	    }
	    return 1;
	}
	current_container = obj;	/* for use by in/out_container */

	if (obj->spe == 1) {
	    observe_quantum_cat(obj);
	    used = 1;
	    quantum_cat = TRUE;	/* for adjusting "it's empty" message */
	}
	/* Count the number of contained objects. Sometimes toss objects if */
	/* a cursed magic bag.						    */
	for (curr = obj->cobj; curr; curr = otmp) {
	    otmp = curr->nobj;
	    if (Is_mbag(obj) && obj->cursed && !rn2(13)) {
		obj_extract_self(curr);
		loss += mbag_item_gone(held, curr);
		used = 1;
	    } else {
		cnt++;
	    }
	}

	if (loss)	/* magic bag lost some shop goods */
	    You("VERB_OWE %ld %s f�r verlorene Ware.", loss, currency(loss)); /* EN You("owe %ld %s for lost merchandise.", loss, currency(loss)); */
	obj->owt = weight(obj);	/* in case any items were lost */

	if (!cnt)
	    Sprintf(emptymsg, "SUBJECT %s VERB_SEIN %sleer.", Yname2(obj), /* EN Sprintf(emptymsg, "%s is %sempty.", Yname2(obj), */
		    quantum_cat ? "jetzt " : ""); /* EN quantum_cat ? "now " : ""); */

	if (cnt || flags.menu_style == MENU_FULL) {
	    Strcpy(qbuf, "SUBJECT MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL etwas OBJECT KASUS_DATIV aus "); /* EN Strcpy(qbuf, "Do you want to take something out of "); */
	    Sprintf(eos(qbuf), "%s herausnehmen?", /* EN Sprintf(eos(qbuf), "%s?", */
		    safe_qbuf(qbuf, 1, yname(obj), ysimple_name(obj), "ihm")); /* EN safe_qbuf(qbuf, 1, yname(obj), ysimple_name(obj), "it")); */
	    if (flags.menu_style != MENU_TRADITIONAL) {
		if (flags.menu_style == MENU_FULL) {
		    int t;
		    char menuprompt[BUFSZ];
		    boolean outokay = (cnt != 0);
#ifndef GOLDOBJ
		    boolean inokay = (invent != 0) || (u.ugold != 0);
#else
		    boolean inokay = (invent != 0);
#endif
		    if (!outokay && !inokay) {
			pline("%s", emptymsg);
			You("VERB_HABEN nichts reinzulegen."); /* EN You("don't have anything to put in."); */
			return used;
		    }
		    menuprompt[0] = '\0';
		    if (!cnt) Sprintf(menuprompt, "%s ", emptymsg);
		    Strcat(menuprompt, "Was nun?"); /* EN Strcat(menuprompt, "Do what?"); */
		    t = in_or_out_menu(menuprompt, current_container, outokay, inokay);
		    if (t <= 0) return 0;
		    loot_out = (t & 0x01) != 0;
		    loot_in  = (t & 0x02) != 0;
		} else {	/* MENU_COMBINATION or MENU_PARTIAL */
		    loot_out = (yn_function(qbuf, "ynq", 'n') == 'y');
		}
		if (loot_out) {
		    add_valid_menu_class(0);	/* reset */
		    used |= menu_loot(0, current_container, FALSE) > 0;
		}
	    } else {
		/* traditional code */
ask_again2:
		menu_on_request = 0;
		add_valid_menu_class(0);	/* reset */
		Strcpy(pbuf, ":ynq");
		if (cnt) Strcat(pbuf, "m");
		switch (yn_function(qbuf, pbuf, 'n')) {
		case ':':
		    container_contents(current_container, FALSE, FALSE);
		    goto ask_again2;
		case 'y':
		    if (query_classes(select, &one_by_one, &allflag,
				      "herausnehmen", current_container->cobj, /* EN "take out", current_container->cobj, */
				      FALSE,
#ifndef GOLDOBJ
				      FALSE,
#endif
				      &menu_on_request)) {
			if (askchain((struct obj **)&current_container->cobj,
				     (one_by_one ? (char *)0 : select),
				     allflag, out_container,
				     (int FDECL((*),(OBJ_P)))0,
				     0, "nodot"))
			    used = 1;
		    } else if (menu_on_request < 0) {
			used |= menu_loot(menu_on_request,
					  current_container, FALSE) > 0;
		    }
		    /*FALLTHRU*/
		case 'n':
		    break;
		case 'm':
		    menu_on_request = -2; /* triggers ALL_CLASSES */
		    used |= menu_loot(menu_on_request, current_container, FALSE) > 0;
		    break;
		case 'q':
		default:
		    return used;
		}
	    }
	} else {
	    pline("%s", emptymsg);		/* <whatever> is empty. */
	}

#ifndef GOLDOBJ
	if (!invent && u.ugold == 0) {
#else
	if (!invent) {
#endif
	    /* nothing to put in, but some feedback is necessary */
	    You("VERB_HABEN nichts zum Reinlegen."); /* EN You("don't have anything to put in."); */
	    return used;
	}
	if (flags.menu_style != MENU_FULL) {
	    Sprintf(qbuf, "SATZBEGINN MODIFIER_KONJUNKTIV_II VERB_MOEGEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL %s reintun?", something); /* EN Sprintf(qbuf, "Do you wish to put %s in?", something); */
	    Strcpy(pbuf, ynqchars);
	    if (flags.menu_style == MENU_TRADITIONAL && invent && inv_cnt() > 0)
		Strcat(pbuf, "m");
	    switch (yn_function(qbuf, pbuf, 'n')) {
		case 'y':
		    loot_in = TRUE;
		    break;
		case 'n':
		    break;
		case 'm':
		    add_valid_menu_class(0);	  /* reset */
		    menu_on_request = -2; /* triggers ALL_CLASSES */
		    used |= menu_loot(menu_on_request, current_container, TRUE) > 0;
		    break;
		case 'q':
		default:
		    return used;
	    }
	}
	/*
	 * Gone: being nice about only selecting food if we know we are
	 * putting things in an ice chest.
	 */
	if (loot_in) {
#ifndef GOLDOBJ
	    if (u.ugold) {
		/*
		 * Hack: gold is not in the inventory, so make a gold object
		 * and put it at the head of the inventory list.
		 */
		u_gold = mkgoldobj(u.ugold);	/* removes from u.ugold */
		u_gold->in_use = TRUE;
		u.ugold = u_gold->quan;		/* put the gold back */
		assigninvlet(u_gold);		/* might end up as NOINVSYM */
		u_gold->nobj = invent;
		invent = u_gold;
	    }
#endif
	    add_valid_menu_class(0);	  /* reset */
	    if (flags.menu_style != MENU_TRADITIONAL) {
		used |= menu_loot(0, current_container, TRUE) > 0;
	    } else {
		/* traditional code */
		menu_on_request = 0;
		if (query_classes(select, &one_by_one, &allflag, "reintun", /* EN if (query_classes(select, &one_by_one, &allflag, "put in", */
				   invent, FALSE,
#ifndef GOLDOBJ
				   (u.ugold != 0L),
#endif
				   &menu_on_request)) {
		    (void) askchain((struct obj **)&invent,
				    (one_by_one ? (char *)0 : select), allflag,
				    in_container, ck_bag, 0, "nodot");
		    used = 1;
		} else if (menu_on_request < 0) {
		    used |= menu_loot(menu_on_request,
				      current_container, TRUE) > 0;
		}
	    }
	}

#ifndef GOLDOBJ
	if (u_gold && invent && invent->oclass == COIN_CLASS) {
	    /* didn't stash [all of] it */
	    u_gold = invent;
	    invent = u_gold->nobj;
	    u_gold->in_use = FALSE;
	    dealloc_obj(u_gold);
	}
#endif
	return used;
}

/* Loot a container (take things out, put things in), using a menu. */
STATIC_OVL int
menu_loot(retry, container, put_in)
int retry;
struct obj *container;
boolean put_in;
{
    int n, i, n_looted = 0;
    boolean all_categories = TRUE, loot_everything = FALSE;
    char buf[BUFSZ];
    const char *takeout = "rausnehmen", *putin = "reintun"; /* EN const char *takeout = "Take out", *putin = "Put in"; */ /* entnehmen? verstauen? */
    struct obj *otmp, *otmp2;
    menu_item *pick_list;
    int mflags, res;
    long count;

    if (retry) {
	all_categories = (retry == -2);
    } else if (flags.menu_style == MENU_FULL) {
	all_categories = FALSE;
	Sprintf(buf,"Welche Art Objekte %s?", put_in ? putin : takeout); /* EN Sprintf(buf,"%s what type of objects?", put_in ? putin : takeout); */
	mflags = put_in ? ALL_TYPES | BUC_ALLBKNOWN | BUC_UNKNOWN :
		          ALL_TYPES | CHOOSE_ALL | BUC_ALLBKNOWN | BUC_UNKNOWN;
	n = query_category(buf, put_in ? invent : container->cobj,
			   mflags, &pick_list, PICK_ANY);
	if (!n) return 0;
	for (i = 0; i < n; i++) {
	    if (pick_list[i].item.a_int == 'A')
		loot_everything = TRUE;
	    else if (pick_list[i].item.a_int == ALL_TYPES_SELECTED)
		all_categories = TRUE;
	    else
		add_valid_menu_class(pick_list[i].item.a_int);
	}
	free((genericptr_t) pick_list);
    }

    if (loot_everything) {
	for (otmp = container->cobj; otmp; otmp = otmp2) {
	    otmp2 = otmp->nobj;
	    res = out_container(otmp);
	    if (res < 0) break;
	}
    } else {
	mflags = INVORDER_SORT;
	if (put_in && flags.invlet_constant) mflags |= USE_INVLET;
	Sprintf(buf,"Was %s?", put_in ? putin : takeout); /* EN Sprintf(buf,"%s what?", put_in ? putin : takeout); */
	n = query_objlist(buf, put_in ? invent : container->cobj,
			  mflags, &pick_list, PICK_ANY,
			  all_categories ? allow_all : allow_category);
	if (n) {
		n_looted = n;
		for (i = 0; i < n; i++) {
		    otmp = pick_list[i].item.a_obj;
		    count = pick_list[i].count;
		    if (count > 0 && count < otmp->quan) {
			otmp = splitobj(otmp, count);
			/* special split case also handled by askchain() */
		    }
		    res = put_in ? in_container(otmp) : out_container(otmp);
		    if (res < 0) {
			if (otmp != pick_list[i].item.a_obj) {
			    /* split occurred, merge again */
			    (void) merged(&pick_list[i].item.a_obj, &otmp);
			}
			break;
		    }
		}
		free((genericptr_t)pick_list);
	}
    }
    return n_looted;
}

STATIC_OVL int
in_or_out_menu(prompt, obj, outokay, inokay)
const char *prompt;
struct obj *obj;
boolean outokay, inokay;
{
    winid win;
    anything any;
    menu_item *pick_list;
    char buf[BUFSZ];
    int n;
    const char *menuselector = iflags.lootabc ? "abc" : "oib";

    any.a_void = 0;
    win = create_nhwindow(NHW_MENU);
    start_menu(win);
    if (outokay) {
	any.a_int = 1;
	Sprintf(buf,"Etwas KASUS_DATIV aus %s rausnehmen", the(xname(obj))); /* EN Sprintf(buf,"Take %s out of %s", something, the(xname(obj))); */
	add_menu(win, NO_GLYPH, &any, *menuselector, 0, ATR_NONE,
			buf, MENU_UNSELECTED);
    }
    menuselector++;
    if (inokay) {
	any.a_int = 2;
	Sprintf(buf,"Etwas KASUS_AKKUSATIV in %s reinlegen", the(xname(obj))); /* EN Sprintf(buf,"Put %s into %s", something, the(xname(obj))); */
	add_menu(win, NO_GLYPH, &any, *menuselector, 0, ATR_NONE, buf, MENU_UNSELECTED);
    }
    menuselector++;
    if (outokay && inokay) {
	any.a_int = 3;
	add_menu(win, NO_GLYPH, &any, *menuselector, 0, ATR_NONE,
			"Sowohl als auch", MENU_UNSELECTED); /* EN "Both of the above", MENU_UNSELECTED); */
    }
    end_menu(win, prompt);
    n = select_menu(win, PICK_ONE, &pick_list);
    destroy_nhwindow(win);
    if (n > 0) {
	n = pick_list[0].item.a_int;
	free((genericptr_t) pick_list);
    }
    return n;
}

/*pickup.c*/
