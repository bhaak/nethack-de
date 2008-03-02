/*	SCCS Id: @(#)write.c	3.4	2001/11/29	*/
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL int FDECL(cost,(struct obj *));

/*
 * returns basecost of a scroll or a spellbook
 */
STATIC_OVL int
cost(otmp)
register struct obj *otmp;
{

	if (otmp->oclass == SPBOOK_CLASS)
		return(10 * objects[otmp->otyp].oc_level);

	switch (otmp->otyp) {
# ifdef MAIL
	case SCR_MAIL:
		return(2);
/*		break; */
# endif
	case SCR_LIGHT:
	case SCR_GOLD_DETECTION:
	case SCR_FOOD_DETECTION:
	case SCR_MAGIC_MAPPING:
	case SCR_AMNESIA:
	case SCR_FIRE:
	case SCR_EARTH:
		return(8);
/*		break; */
	case SCR_DESTROY_ARMOR:
	case SCR_CREATE_MONSTER:
	case SCR_PUNISHMENT:
		return(10);
/*		break; */
	case SCR_CONFUSE_MONSTER:
		return(12);
/*		break; */
	case SCR_IDENTIFY:
		return(14);
/*		break; */
	case SCR_ENCHANT_ARMOR:
	case SCR_REMOVE_CURSE:
	case SCR_ENCHANT_WEAPON:
	case SCR_CHARGING:
		return(16);
/*		break; */
	case SCR_SCARE_MONSTER:
	case SCR_STINKING_CLOUD:
	case SCR_TAMING:
	case SCR_TELEPORTATION:
		return(20);
/*		break; */
	case SCR_GENOCIDE:
		return(30);
/*		break; */
	case SCR_BLANK_PAPER:
	default:
		impossible("You can't write such a weird scroll!");
	}
	return(1000);
}

static NEARDATA const char write_on[] = { SCROLL_CLASS, SPBOOK_CLASS, 0 };

int
dowrite(pen)
register struct obj *pen;
{
	register struct obj *paper;
	char namebuf[BUFSZ], *nm, *bp;
	register struct obj *new_obj;
	int basecost, actualcost;
	int curseval;
	char qbuf[QBUFSZ];
	int first, last, i;
	boolean by_descr = FALSE;
	const char *typeword;

	if (nohands(youmonst.data)) {
	    You("VERB_BRAUCHEN H�nde um schreiben zu k�nnen!"); /* EN You("need hands to be able to write!"); */
	    return 0;
	} else if (Glib) {
	    pline("%s OBJECT KASUS_DATIV PRONOMEN_PERSONAL aus OBJECT KASUS_DATIV PRONOMEN_POSSESSIV %s.", /* EN pline("%s from your %s.", */
		  Tobjnam(pen, "VERB_RUTSCHEN"), makeplural(body_part(FINGER))); /* EN Tobjnam(pen, "slip"), makeplural(body_part(FINGER))); */
	    dropx(pen);
	    return 1;
	}

	/* get paper to write on */
	paper = getobj(write_on,"write on"); /* EN paper = getobj(write_on,"write on"); */ // TODO DE
	if(!paper)
		return(0);
	typeword = (paper->oclass == SPBOOK_CLASS) ? "NOUN_SPELLBOOK" : "NOUN_SCROLL"; /* EN typeword = (paper->oclass == SPBOOK_CLASS) ? "spellbook" : "scroll"; */
	if(Blind && !paper->dknown) {
		You("VERB_WISSEN nicht, ob NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_DIESER %s unbeschriftet VERB_SEIN!", typeword); /* EN You("don't know if that %s is blank or not!", typeword); */
		return(1);
	}
	paper->dknown = 1;
	if(paper->otyp != SCR_BLANK_PAPER && paper->otyp != SPE_BLANK_PAPER) {
		pline("SUBJECT ARTIKEL_BESTIMMTER %s ist schon beschriftet!", typeword); /* EN pline("That %s is not blank!", typeword); */
		exercise(A_WIS, FALSE);
		return(1);
	}

	/* what to write */
	Sprintf(qbuf, "What type of %s do you want to write?", typeword); /* EN Sprintf(qbuf, "What type of %s do you want to write?", typeword); */ // TODO DE
	getlin(qbuf, namebuf);
	(void)mungspaces(namebuf);	/* remove any excess whitespace */
	if(namebuf[0] == '\033' || !namebuf[0])
		return(1);
	nm = namebuf;
	if (!strncmpi(nm, "scroll ", 7)) nm += 7; /* EN if (!strncmpi(nm, "scroll ", 7)) nm += 7; */ // TODO DE
	else if (!strncmpi(nm, "spellbook ", 10)) nm += 10; /* EN else if (!strncmpi(nm, "spellbook ", 10)) nm += 10; */ // TODO DE
	if (!strncmpi(nm, "of ", 3)) nm += 3; /* EN if (!strncmpi(nm, "of ", 3)) nm += 3; */ // TODO DE

	if ((bp = strstri(nm, " armour")) != 0) { /* EN if ((bp = strstri(nm, " armour")) != 0) { */ // TODO DE
		(void)strncpy(bp, " armor ", 7);	/* won't add '\0' */ /* EN (void)strncpy(bp, " armor ", 7);	*/ // TODO DE
		(void)mungspaces(bp + 1);	/* remove the extra space */
	}

	first = bases[(int)paper->oclass];
	last = bases[(int)paper->oclass + 1] - 1;
	for (i = first; i <= last; i++) {
		/* extra shufflable descr not representing a real object */
		if (!OBJ_NAME(objects[i])) continue;

		if (!strcmpi(OBJ_NAME(objects[i]), nm))
			goto found;
		if (!strcmpi(OBJ_DESCR(objects[i]), nm)) {
			by_descr = TRUE;
			goto found;
		}
	}

	pline("Es gibt keine _solches_ %s!", typeword); /* EN There("is no such %s!", typeword); */ // TODO DE
	return 1;
found:

	if (i == SCR_BLANK_PAPER || i == SPE_BLANK_PAPER) {
		You_cant("write that!"); /* EN You_cant("write that!"); */ // TODO DE
		pline("It's obscene!"); /* EN pline("It's obscene!"); */ // TODO DE
		return 1;
	} else if (i == SPE_BOOK_OF_THE_DEAD) {
		pline("No mere dungeon adventurer could write that."); /* EN pline("No mere dungeon adventurer could write that."); */ // TODO DE
		return 1;
	} else if (by_descr && paper->oclass == SPBOOK_CLASS &&
		    !objects[i].oc_name_known) {
		/* can't write unknown spellbooks by description */
		pline(
		  "Unfortunately you don't have enough information to go on."); /* EN "Unfortunately you don't have enough information to go on."); */ // TODO DE
		return 1;
	}

	/* KMH, conduct */
	u.uconduct.literate++;

	new_obj = mksobj(i, FALSE, FALSE);
	new_obj->bknown = (paper->bknown && pen->bknown);

	/* shk imposes a flat rate per use, not based on actual charges used */
	check_unpaid(pen);

	/* see if there's enough ink */
	basecost = cost(new_obj);
	if(pen->spe < basecost/2)  {
		Your("NOUN_MARKER hat nicht genug Tinte um das zu schreiben!"); /* EN Your("marker is too dry to write that!"); */
		obfree(new_obj, (struct obj *) 0);
		return(1);
	}

	/* we're really going to write now, so calculate cost
	 */
	actualcost = rn1(basecost/2,basecost/2);
	curseval = bcsign(pen) + bcsign(paper);
	exercise(A_WIS, TRUE);
	/* dry out marker */
	if (pen->spe < actualcost) {
		pen->spe = 0;
		Deinem("NOUN_MARKER geht die Tinte aus!"); /* EN Your("marker dries out!"); */
		/* scrolls disappear, spellbooks don't */
		if (paper->oclass == SPBOOK_CLASS) {
			pline_The(
		       "NOUN_SPELLBOOK ist nicht fertig und NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_POSSESSIV NOUN_SCHRIFT verblasst."); /* EN "spellbook is left unfinished and your writing fades."); */
			update_inventory();	/* pen charges */
		} else {
			pline_The("NOUN_SCROLL ist jetzt nutzlos und verschwindet!"); /* EN pline_The("scroll is now useless and disappears!"); */
			useup(paper);
		}
		obfree(new_obj, (struct obj *) 0);
		return(1);
	}
	pen->spe -= actualcost;

	/* can't write if we don't know it - unless we're lucky */
	if(!(objects[new_obj->otyp].oc_name_known) &&
	   !(objects[new_obj->otyp].oc_uname) &&
	   (rnl(Role_if(PM_WIZARD) ? 3 : 15))) {
		You("%s to write that!", by_descr ? "fail" : "don't know how"); /* EN You("%s to write that!", by_descr ? "fail" : "don't know how"); */ // TODO DE
		/* scrolls disappear, spellbooks don't */
		if (paper->oclass == SPBOOK_CLASS) {
			You(
       "VERB_SCHREIBEN OBJECT KASUS_DATIV in NOUN_SCHOENSCHRIFT:  \"Liebes Tagebuch\", aber es verblasst gleich wieder."); /* EN "write in your best handwriting:  \"My Diary\", but it quickly fades."); */
			update_inventory();	/* pen charges */
		} else {
			if (by_descr) {
			    Strcpy(namebuf, OBJ_DESCR(objects[new_obj->otyp]));
			    wipeout_text(namebuf, (6+MAXULEV - u.ulevel)/6, 0);
			} else
			    Sprintf(namebuf, "%s was here!", plname); /* EN Sprintf(namebuf, "%s was here!", plname); */ // TODO DE
			You("VERB_SCHREIBEN \"%s\" und NEUER_SATZ SUBJECT_IM_SATZ ARTIKEL_BESTIMMTER NOUN_SCROLL VERB_DISAPPEAR.", namebuf); /* EN You("write \"%s\" and the scroll disappears.", namebuf); */
			useup(paper);
		}
		obfree(new_obj, (struct obj *) 0);
		return(1);
	}

	/* useup old scroll / spellbook */
	useup(paper);

	/* success */
	if (new_obj->oclass == SPBOOK_CLASS) {
		/* acknowledge the change in the object's description... */
		pline_The("spellbook warps strangely, then turns %s.", /* EN pline_The("spellbook warps strangely, then turns %s.", */ // TODO DE
		      OBJ_DESCR(objects[new_obj->otyp]));
	}
	new_obj->blessed = (curseval > 0);
	new_obj->cursed = (curseval < 0);
#ifdef MAIL
	if (new_obj->otyp == SCR_MAIL) new_obj->spe = 1;
#endif
	new_obj = hold_another_object(new_obj, "Oops!  %s out of your grasp!", /* EN new_obj = hold_another_object(new_obj, "Oops!  %s out of your grasp!", */ // TODO DE
					       The(aobjnam(new_obj, "slip")), /* EN The(aobjnam(new_obj, "slip")), */ // TODO DE
					       (const char *)0);
	return(1);
}

/*write.c*/
