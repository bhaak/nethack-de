/*	SCCS Id: @(#)mhitu.c	3.4	2003/11/26	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"

#ifdef GERMAN
# include "german.h"
#endif


STATIC_VAR NEARDATA struct obj *otmp;

STATIC_DCL void FDECL(urustm, (struct monst *, struct obj *));
# ifdef OVL1
STATIC_DCL boolean FDECL(u_slip_free, (struct monst *,struct attack *));
STATIC_DCL int FDECL(passiveum, (struct permonst *,struct monst *,struct attack *));
# endif /* OVL1 */

#ifdef OVLB
# ifdef SEDUCE
STATIC_DCL void FDECL(mayberem, (struct obj *, const char *));
# endif
#endif /* OVLB */

STATIC_DCL boolean FDECL(diseasemu, (struct permonst *));
STATIC_DCL int FDECL(hitmu, (struct monst *,struct attack *));
STATIC_DCL int FDECL(gulpmu, (struct monst *,struct attack *));
STATIC_DCL int FDECL(explmu, (struct monst *,struct attack *,BOOLEAN_P));
STATIC_DCL void FDECL(missmu,(struct monst *,BOOLEAN_P,struct attack *));
STATIC_DCL void FDECL(mswings,(struct monst *,struct obj *));
STATIC_DCL void FDECL(wildmiss, (struct monst *,struct attack *));

STATIC_DCL void FDECL(hurtarmor,(int));
STATIC_DCL void FDECL(hitmsg,(struct monst *,struct attack *));

/* See comment in mhitm.c.  If we use this a lot it probably should be */
/* changed to a parameter to mhitu. */
static int dieroll;

#ifdef OVL1


STATIC_OVL void
hitmsg(mtmp, mattk)
register struct monst *mtmp;
register struct attack *mattk;
{
	int compat;

	/* Note: if opposite gender, "seductively" */
	/* If same gender, "engagingly" for nymph, normal msg for others */
	if((compat = could_seduce(mtmp, &youmonst, mattk))
			&& !mtmp->mcan && !mtmp->mspec_used) {
		pline("SUBJECT %s %s %s %s.", Monnam(mtmp), /* EN pline("%s %s you %s.", Monnam(mtmp), */
			Blind ? "VERB_SPRECHEN" : "VERB_LAECHELN OBJECT PRONOMEN_PERSONAL", /* EN Blind ? "talks to" : "smiles at", */
			compat == 2 ? "einnehmend" : "verführerisch", /* EN compat == 2 ? "engagingly" : "seductively"); */
			Blind ? "mit OBJECT PRONOMEN_PERSONAL" : "an");
	} else
	    switch (mattk->aatyp) {
		case AT_BITE:
			pline("SUBJECT %s VERB_BITE!", Monnam(mtmp)); /* EN pline("%s bites!", Monnam(mtmp)); */
			break;
		case AT_KICK:
			pline("SUBJECT %s VERB_KICK%c", Monnam(mtmp), /* EN pline("%s kicks%c", Monnam(mtmp), */
				    thick_skinned(youmonst.data) ? '.' : '!');
			break;
		case AT_STNG:
			pline("SUBJECT %s VERB_STING!", Monnam(mtmp)); /* EN pline("%s stings!", Monnam(mtmp)); */
			break;
		case AT_BUTT:
			pline("SUBJECT %s VERB_BUTT!", Monnam(mtmp)); /* EN pline("%s butts!", Monnam(mtmp)); */
			break;
		case AT_TUCH:
			pline("SUBJECT %s VERB_TOUCH OBJECT PRONOMEN_PERSONAL!", Monnam(mtmp)); /* EN pline("%s touches you!", Monnam(mtmp)); */
			break;
		case AT_TENT:
			pline("SUBJECT %s VERB_FESTSAUGEN sich OBJECT KASUS_DATIV an PRONOMEN_PERSONAL SATZKLAMMER!", /* EN pline("%s tentacles suck you!", */
				        genitivattribut_zu_wort(Monnam(mtmp),"NOUN_TENTAKELs")); /* EN s_suffix(Monnam(mtmp))); */
			break;
		case AT_EXPL:
		case AT_BOOM:
			pline("SUBJECT %s VERB_EXPLODIEREN!", Monnam(mtmp)); /* EN pline("%s explodes!", Monnam(mtmp)); */
			break;
		default:
			pline("SUBJECT %s VERB_HIT!", Monnam(mtmp)); /* EN pline("%s hits!", Monnam(mtmp)); */
	    }
}

STATIC_OVL void
missmu(mtmp, nearmiss, mattk)		/* monster missed you */
register struct monst *mtmp;
register boolean nearmiss;
register struct attack *mattk;
{
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);

	if(could_seduce(mtmp, &youmonst, mattk) && !mtmp->mcan)
	    pline("SUBJECT %s VERB_HERANMACHEN sich an OBJECT PRONOMEN_PERSONAL SATZKLAMMER.", Monnam(mtmp)); /* EN pline("%s pretends to be friendly.", Monnam(mtmp)); */
	else {
	    if (!flags.verbose || !nearmiss)
		pline("SUBJECT %s VERB_MISS OBJECT PRONOMEN_PERSONAL.", Monnam(mtmp)); /* EN pline("%s misses.", Monnam(mtmp)); */
	    else
		pline("SUBJECT %s VERB_MISS OBJECT PRONOMEN_PERSONAL knapp!", Monnam(mtmp)); /* EN pline("%s just misses!", Monnam(mtmp)); */
	}
	stop_occupation();
}

STATIC_OVL void
mswings(mtmp, otemp)		/* monster swings obj */
register struct monst *mtmp;
register struct obj *otemp;
{
	if (!flags.verbose || Blind || !mon_visible(mtmp))
		return;
	pline("SUBJECT %s %s mit KASUS_DATIV PRONOMEN_POSSESSIV_SUBJECT %s SATZKLAMMER.", Monnam(mtmp), /* EN pline("%s %s %s %s.", Monnam(mtmp), */
	      (objects[otemp->otyp].oc_dir & PIERCE) ?  "VERB_ZUSTECHEN" : "VERB_ZUSCHLAGEN", /* EN (objects[otemp->otyp].oc_dir & PIERCE) ? "thrusts" : "swings", */
	      singular(otemp, xname)); /* EN mhis(mtmp), singular(otemp, xname)); */
}

/* return how a poison attack was delivered */
const char *
mpoisons_subj(mtmp, mattk)
struct monst *mtmp;
struct attack *mattk;
{
	if (mattk->aatyp == AT_WEAP) {
	    struct obj *mwep = (mtmp == &youmonst) ? uwep : MON_WEP(mtmp);
	    /* "Foo's attack was poisoned." is pretty lame, but at least
	       it's better than "sting" when not a stinging attack... */
	    return (!mwep || !mwep->opoisoned) ? "NOUN_ATTACK" : "NOUN_WEAPON"; /* EN return (!mwep || !mwep->opoisoned) ? "attack" : "weapon"; */
	} else {
	    return (mattk->aatyp == AT_TUCH) ? "NOUN_BERUEHRUNG" : /* EN return (mattk->aatyp == AT_TUCH) ? "contact" : */
		   (mattk->aatyp == AT_GAZE) ? "NOUN_GAZE" : /* EN (mattk->aatyp == AT_GAZE) ? "gaze" : */
		   (mattk->aatyp == AT_BITE) ? "NOUN_BITE" : "NOUN_STING"; /* EN (mattk->aatyp == AT_BITE) ? "bite" : "sting"; */
	}
}

/* called when your intrinsic speed is taken away */
void
u_slow_down()
{
	HFast = 0L;
	if (!Fast)
	    You("VERB_WERDEN langsamer."); /* EN You("slow down."); */
	else	/* speed boots */
	    Your("NOUN_QUICKNESS VERB_WIRKEN weniger natürlich."); /* EN Your("quickness feels less natural."); */
	exercise(A_DEX, FALSE);
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL void
wildmiss(mtmp, mattk)		/* monster attacked your displaced image */
	register struct monst *mtmp;
	register struct attack *mattk;
{
	int compat;

	/* no map_invisible() -- no way to tell where _this_ is coming from */

	if (!flags.verbose) return;
	if (!cansee(mtmp->mx, mtmp->my)) return;
		/* maybe it's attacking an image around the corner? */

	compat = (mattk->adtyp == AD_SEDU || mattk->adtyp == AD_SSEX) &&
		 could_seduce(mtmp, &youmonst, (struct attack *)0);

	if (!mtmp->mcansee || (Invis && !perceives(mtmp->data))) {
	    const char *swings =
		mattk->aatyp == AT_BITE ? "VERB_SCHNAPPEN" : /* EN mattk->aatyp == AT_BITE ? "snaps" : */
		mattk->aatyp == AT_KICK ? "VERB_TRETEN" : /* EN mattk->aatyp == AT_KICK ? "kicks" : */
		(mattk->aatyp == AT_STNG ||
		 mattk->aatyp == AT_BUTT ||
		 nolimbs(mtmp->data)) ? "lunges" : "swings"; /* EN nolimbs(mtmp->data)) ? "lunges" : "swings"; */ // TODO DE

	    if (compat)
		pline("SUBJECT %s VERB_VERSUCHEN OBJECT PRONOMEN_PERSONAL zu berühren, VERB_MISS OBJECT PRONOMEN_PERSONAL aber!", Monnam(mtmp)); /* EN pline("%s tries to touch you and misses!", Monnam(mtmp)); */
	    else
		switch(rn2(3)) {
		case 0: pline("SUBJECT %s %s wild OBJECT KASUS_DATIV nach PRONOMEN_PERSONAL und VERB_MISS NEUES_OBJECT OBJECT PRONOMEN_PERSONAL!", Monnam(mtmp), /* EN case 0: pline("%s %s wildly and misses!", Monnam(mtmp), */
			      swings);
		    break;
		case 1: pline("SUBJECT %s VERB_ANGREIFEN einen Platz OBJECT KASUS_DATIV neben PRONOMEN_PERSONAL SATZKLAMMER.", Monnam(mtmp)); /* EN case 1: pline("%s attacks a spot beside you.", Monnam(mtmp)); */
		    break;
		case 2: pline("SUBJECT %s VERB_SCHLAGEN %s!", Monnam(mtmp), /* EN case 2: pline("%s strikes at %s!", Monnam(mtmp), */
				levl[mtmp->mux][mtmp->muy].typ == WATER
				    ? "ins Wasser" : "in die Luft"); /* EN ? "empty water" : "thin air"); */
		    break;
		default:pline("SUBJECT %s %s wild OBJECT KASUS_DATIV nach PRONOMEN_PERSONAL!", Monnam(mtmp), swings); /* EN default:pline("%s %s wildly!", Monnam(mtmp), swings); */
		    break;
		}

	} else if (Displaced) {
	    if (compat)
		pline("SUBJECT %s VERB_ANLAECHELN %s OBJECT %sADJEKTIV_DISPLACED NOUN_DISPLACEDIMAGE SATZKLAMMER ...", /* EN pline("%s smiles %s at your %sdisplaced image...", */
			Monnam(mtmp),
			compat == 2 ? "einnehmend" : "verführerisch", /* EN compat == 2 ? "engagingly" : "seductively", */
			Invis ? "ADJEKTIV_UNSICHTBAR, " : ""); /* EN Invis ? "invisible " : ""); */
	    else
		pline("SUBJECT %s VERB_SCHLAGEN OBJECT KASUS_DATIV nach PRONOMEN_POSSESSIV %sADJEKTIV_DISPLACED NOUN_DISPLACEDIMAGE und VERB_MISS OBJECT PRONOMEN_PERSONAL!", /* EN pline("%s strikes at your %sdisplaced image and misses you!", */
			/* Note: if you're both invisible and displaced,
			 * only monsters which see invisible will attack your
			 * displaced image, since the displaced image is also
			 * invisible.
			 */
			Monnam(mtmp),
			Invis ? "ADJEKTIV_INVISIBLE, " : ""); /* EN Invis ? "invisible " : ""); */

	} else if (Underwater) {
	    /* monsters may miss especially on water level where
	       bubbles shake the player here and there */
	    if (compat)
		pline("SUBJECT %s VERB_GREIFEN OBJECT KASUS_DATIV nach PRONOMEN_POSSESSIV ADJEKTIV_VERZERRT NOUN_ABBILD.",Monnam(mtmp)); /* EN pline("%s reaches towards your distorted image.",Monnam(mtmp)); */
	    else
		pline("SUBJECT %s wird von den Wasserspiegelungen getäuscht und trifft nicht!",Monnam(mtmp)); /* EN pline("%s is fooled by water reflections and misses!",Monnam(mtmp)); */

	} else impossible("%s attacks you without knowing your location?",
		Monnam(mtmp));
}

void
expels(mtmp, mdat, message)
register struct monst *mtmp;
register struct permonst *mdat; /* if mtmp is polymorphed, mdat != mtmp->data */
boolean message;
{
	if (message) {
		if (is_animal(mdat))
			You("VERB_WERDEN MODIFIER_VERB_PARTIZIP_PERFEKT VERB_REGURGITATE!"); /* EN You("get regurgitated!"); */
		else {
			char blast[40];
			register int i;

			blast[0] = '\0';
			for(i = 0; i < NATTK; i++)
				if(mdat->mattk[i].aatyp == AT_ENGL)
					break;
			if (mdat->mattk[i].aatyp != AT_ENGL)
			      impossible("Swallower has no engulfing attack?");
			else {
				if (is_whirly(mdat)) {
					switch (mdat->mattk[i].adtyp) {
						case AD_ELEC:
							Strcpy(blast,
						      " mit einem Funkenregen"); /* EN " in a shower of sparks"); */
							break;
						case AD_COLD:
							Strcpy(blast,
							" mit Schneegestöber"); /* EN " in a blast of frost"); */
							break;
					}
				} else
					Strcpy(blast, " mit einem Rülpser"); /* EN Strcpy(blast, " with a squelch"); */
				You("VERB_WERDEN%s OBJECT KASUS_DATIV aus %s MODIFIER_VERB_PARTIZIP_PERFEKT VERB_EXPEL!", /* EN You("get expelled from %s%s!", */
				    blast, mon_nam(mtmp)); /* EN mon_nam(mtmp), blast); */
			}
		}
	}
	unstuck(mtmp);	/* ball&chain returned in unstuck() */
	mnexto(mtmp);
	newsym(u.ux,u.uy);
	spoteffects(TRUE);
	/* to cover for a case where mtmp is not in a next square */
	if(um_dist(mtmp->mx,mtmp->my,1))
		pline("Rüüllppss...  SUBJECT PRONOMEN_PERSONAL VERB_SCHLAGEN etwas weiter weg hart auf."); /* EN pline("Brrooaa...  You land hard at some distance."); */
}

#endif /* OVLB */
#ifdef OVL0

/* select a monster's next attack, possibly substituting for its usual one */
struct attack *
getmattk(mptr, indx, prev_result, alt_attk_buf)
struct permonst *mptr;
int indx, prev_result[];
struct attack *alt_attk_buf;
{
    struct attack *attk = &mptr->mattk[indx];

    /* prevent a monster with two consecutive disease or hunger attacks
       from hitting with both of them on the same turn; if the first has
       already hit, switch to a stun attack for the second */
    if (indx > 0 && prev_result[indx - 1] > 0 &&
	    (attk->adtyp == AD_DISE ||
		attk->adtyp == AD_PEST ||
		attk->adtyp == AD_FAMN) &&
	    attk->adtyp == mptr->mattk[indx - 1].adtyp) {
	*alt_attk_buf = *attk;
	attk = alt_attk_buf;
	attk->adtyp = AD_STUN;
    }
    return attk;
}

/*
 * mattacku: monster attacks you
 *	returns 1 if monster dies (e.g. "yellow light"), 0 otherwise
 *	Note: if you're displaced or invisible the monster might attack the
 *		wrong position...
 *	Assumption: it's attacking you or an empty square; if there's another
 *		monster which it attacks by mistake, the caller had better
 *		take care of it...
 */
int
mattacku(mtmp)
	register struct monst *mtmp;
{
	struct	attack	*mattk, alt_attk;
	int	i, j, tmp, sum[NATTK];
	struct	permonst *mdat = mtmp->data;
	boolean ranged = (distu(mtmp->mx, mtmp->my) > 3);
		/* Is it near you?  Affects your actions */
	boolean range2 = !monnear(mtmp, mtmp->mux, mtmp->muy);
		/* Does it think it's near you?  Affects its actions */
	boolean foundyou = (mtmp->mux==u.ux && mtmp->muy==u.uy);
		/* Is it attacking you or your image? */
	boolean youseeit = canseemon(mtmp);
		/* Might be attacking your image around the corner, or
		 * invisible, or you might be blind....
		 */
	
	if(!ranged) nomul(0);
	if(mtmp->mhp <= 0 || (Underwater && !is_swimmer(mtmp->data)))
	    return(0);

	/* If swallowed, can only be affected by u.ustuck */
	if(u.uswallow) {
	    if(mtmp != u.ustuck)
		return(0);
	    u.ustuck->mux = u.ux;
	    u.ustuck->muy = u.uy;
	    range2 = 0;
	    foundyou = 1;
	    if(u.uinvulnerable) return (0); /* stomachs can't hurt you! */
	}

#ifdef STEED
	else if (u.usteed) {
		if (mtmp == u.usteed)
			/* Your steed won't attack you */
			return (0);
		/* Orcs like to steal and eat horses and the like */
		if (!rn2(is_orc(mtmp->data) ? 2 : 4) &&
				distu(mtmp->mx, mtmp->my) <= 2) {
			/* Attack your steed instead */
			i = mattackm(mtmp, u.usteed);
			if ((i & MM_AGR_DIED))
				return (1);
			if (i & MM_DEF_DIED || u.umoved)
				return (0);
			/* Let your steed retaliate */
			return (!!(mattackm(u.usteed, mtmp) & MM_DEF_DIED));
		}
	}
#endif

	if (u.uundetected && !range2 && foundyou && !u.uswallow) {
		u.uundetected = 0;
		if (is_hider(youmonst.data)) {
		    coord cc; /* maybe we need a unexto() function? */
		    struct obj *obj;

		    You("VERB_FALLEN OBJECT KASUS_DATIV _von_ %s!", ceiling(u.ux,u.uy)); /* EN You("fall from the %s!", ceiling(u.ux,u.uy)); */
		    if (enexto(&cc, u.ux, u.uy, youmonst.data)) {
			remove_monster(mtmp->mx, mtmp->my);
			newsym(mtmp->mx,mtmp->my);
			place_monster(mtmp, u.ux, u.uy);
			if(mtmp->wormno) worm_move(mtmp);
			teleds(cc.x, cc.y, TRUE);
			set_apparxy(mtmp);
			newsym(u.ux,u.uy);
		    } else {
			pline("SUBJECT %s VERB_WERDEN OBJECT KASUS_DATIV von ARTIKEL_UNBESTIMMTER ADJEKTIV_HERABFALLEND %s (NEUES_OBJECT OBJECT KASUS_DATIV PRONOMEN_PERSONAL) getötet!", /* EN pline("%s is killed by a falling %s (you)!", */
					Monnam(mtmp), youmonst.data->mname);
			killed(mtmp);
			newsym(u.ux,u.uy);
			if (mtmp->mhp > 0) return 0;
			else return 1;
		    }
		    if (youmonst.data->mlet != S_PIERCER)
			return(0);	/* trappers don't attack */

		    obj = which_armor(mtmp, WORN_HELMET);
		    if (obj && is_metallic(obj)) {
			Your("NOUN_HIEB VERB_ABGLEITEN OBJECT von %s SATZKLAMMER.", /* EN Your("blow glances off %s helmet.", */
			               genitivattribut_zu_wort(mon_nam(mtmp), "NOUN_HELMET")); /* EN s_suffix(mon_nam(mtmp))); */
		    } else {
			if (3 + find_mac(mtmp) <= rnd(20)) {
			    pline("SUBJECT %s VERB_WERDEN OBJECT KASUS_DATIV von ARTIKEL_UNBESTIMMTER ADJEKTIV_HERABFALLEND NOUN_PIERCER (NEUES_OBJECT OBJECT KASUS_DATIV PRONOMEN_PERSONAL) getroffen!", /* EN pline("%s is hit by a falling piercer (you)!", */
								Monnam(mtmp));
			    if ((mtmp->mhp -= d(3,6)) < 1)
				killed(mtmp);
			} else
			  pline("SUBJECT %s VERB_WERDEN beinahe OBJECT KASUS_DATIV von ARTIKEL_UNBESTIMMTER ADJEKTIV_HERABFALLEND NOUN_PIERCER (NEUES_OBJECT OBJECT KASUS_DATIV PRONOMEN_PERSONAL) getroffen!", /* EN pline("%s is almost hit by a falling piercer (you)!", */
								Monnam(mtmp));
		    }
		} else {
		    if (!youseeit)
			pline("Etwas versucht sich dorthin zu bewegen, wo SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT PRONOMEN_PERSONAL VERB_VERSTECKEN."); /* EN pline("It tries to move where you are hiding."); */
		    else {
			/* Ugly kludge for eggs.  The message is phrased so as
			 * to be directed at the monster, not the player,
			 * which makes "laid by you" wrong.  For the
			 * parallelism to work, we can't rephrase it, so we
			 * zap the "laid by you" momentarily instead.
			 */
			struct obj *obj = level.objects[u.ux][u.uy];

			if (obj ||
			      (youmonst.data->mlet == S_EEL && is_pool(u.ux, u.uy))) {
			    int save_spe = 0; /* suppress warning */
			    if (obj) {
				save_spe = obj->spe;
				if (obj->otyp == EGG) obj->spe = 0;
			    }
			    if (youmonst.data->mlet == S_EEL)
		pline("SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_WARTEN, %s!  NEUER_SATZ Da SUBJECT_IM_SATZ VERB_VERSTECKEN sich ARTIKEL_UNBESTIMMTER %s namens %s!", /* EN pline("Wait, %s!  There's a hidden %s named %s there!", */
				m_monnam(mtmp), youmonst.data->mname, plname);
			    else
	     pline("SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_WARTEN, %s!  NEUER_SATZ Da SUBJECT_IM_SATZ VERB_VERSTECKEN sich ARTIKEL_UNBESTIMMTER %s namens %s OBJECT KASUS_DATIV unter %s!", /* EN pline("Wait, %s!  There's a %s named %s hiding under %s!", */
				m_monnam(mtmp), youmonst.data->mname, plname,
				doname(level.objects[u.ux][u.uy]));
			    if (obj) obj->spe = save_spe;
			} else
			    impossible("hiding under nothing?");
		    }
		    newsym(u.ux,u.uy);
		}
		return(0);
	}
	if (youmonst.data->mlet == S_MIMIC && youmonst.m_ap_type &&
		    !range2 && foundyou && !u.uswallow) {
		if (!youseeit) pline("Es bleibt KASUS_DATIV an PRONOMEN_PERSONAL kleben."); /* EN if (!youseeit) pline("It gets stuck on you."); */
		else pline("Warte, %s!  SUBJECT Das ist ARTIKEL_UNBESTIMMTER %s namens %s!", /* EN else pline("Wait, %s!  That's a %s named %s!", */
			   m_monnam(mtmp), youmonst.data->mname, plname);
		u.ustuck = mtmp;
		youmonst.m_ap_type = M_AP_NOTHING;
		youmonst.mappearance = 0;
		newsym(u.ux,u.uy);
		return(0);
	}

	/* player might be mimicking an object */
	if (youmonst.m_ap_type == M_AP_OBJECT && !range2 && foundyou && !u.uswallow) {
	    if (!youseeit)
		 pline("SUBJECT %s %s!", Something, /* EN pline("%s %s!", Something, */
			(likes_gold(mtmp->data) && youmonst.mappearance == GOLD_PIECE) ?
			"VERB_VERSUCHEN OBJECT PRONOMEN_PERSONAL einzusacken" : "VERB_STOEREN OBJECT PRONOMEN_PERSONAL"); /* EN "tries to pick you up" : "disturbs you"); */
	    else pline("Warte, %s!  SUBJECT PRONOMEN_DIESER %s ist eigentlich %s namens %s!", /* EN else pline("Wait, %s!  That %s is really %s named %s!", */
			m_monnam(mtmp),
			mimic_obj_name(&youmonst),
			an(mons[u.umonnum].mname),
			plname);
	    if (multi < 0) {	/* this should always be the case */
		char buf[BUFSZ];
		Sprintf(buf, "SUBJECT PRONOMEN_PERSONAL VERB_SCHEINEN wieder OBJECT KASUS_NOMINATIV %s zu sein.", /* EN Sprintf(buf, "You appear to be %s again.", */
			Upolyd ? (const char *) an(youmonst.data->mname) :
			    (const char *) "PRONOMEN_PERSONAL selbst"); /* EN (const char *) "yourself"); */
		unmul(buf);	/* immediately stop mimicking */
	    }
	    return 0;
	}

/*	Work out the armor class differential	*/
	tmp = AC_VALUE(u.uac) + 10;		/* tmp ~= 0 - 20 */
	tmp += mtmp->m_lev;
	if(multi < 0) tmp += 4;
	if((Invis && !perceives(mdat)) || !mtmp->mcansee)
		tmp -= 2;
	if(mtmp->mtrapped) tmp -= 2;
	if(tmp <= 0) tmp = 1;

	/* make eels visible the moment they hit/miss us */
	if(mdat->mlet == S_EEL && mtmp->minvis && cansee(mtmp->mx,mtmp->my)) {
		mtmp->minvis = 0;
		newsym(mtmp->mx,mtmp->my);
	}

/*	Special demon handling code */
	if(!mtmp->cham && is_demon(mdat) && !range2
	   && mtmp->data != &mons[PM_BALROG]
	   && mtmp->data != &mons[PM_SUCCUBUS]
	   && mtmp->data != &mons[PM_INCUBUS])
	    if(!mtmp->mcan && !rn2(13))	msummon(mtmp);

/*	Special lycanthrope handling code */
	if(!mtmp->cham && is_were(mdat) && !range2) {

	    if(is_human(mdat)) {
		if(!rn2(5 - (night() * 2)) && !mtmp->mcan) new_were(mtmp);
	    } else if(!rn2(30) && !mtmp->mcan) new_were(mtmp);
	    mdat = mtmp->data;

	    if(!rn2(10) && !mtmp->mcan) {
	    	int numseen, numhelp;
		char buf[BUFSZ], genericwere[BUFSZ];

		Strcpy(genericwere, "NOUN_CREATURE"); /* EN Strcpy(genericwere, "creature"); */
		numhelp = were_summon(mdat, FALSE, &numseen, genericwere);
		if(youseeit) {
			pline("SUBJECT %s ruft um Hilfe!", Monnam(mtmp)); /* EN pline("%s summons help!", Monnam(mtmp)); */
			if (numhelp > 0) {
			    if (numseen == 0)
			Du_fuehlst_dich("umzingelt."); /* EN You_feel("hemmed in."); */
			} else pline("Aber niemand kommt."); /* EN } else pline("But none comes."); */
		} else {
			const char *from_nowhere;

			if (flags.soundok) {
				pline("SUBJECT %s %s!", Something, /* EN pline("%s %s!", Something, */
					makeplural(growl_sound(mtmp)));
				from_nowhere = "";
			} else from_nowhere = " aus dem Nichts"; /* EN } else from_nowhere = " from nowhere"; */
			if (numhelp > 0) {
			    if (numseen < 1) Du_fuehlst_dich("umzingelt."); /* EN if (numseen < 1) You_feel("hemmed in."); */
			    else {
				if (numseen == 1)
			    		Sprintf(buf, "%s VERB_AUFTAUCHEN", /* EN Sprintf(buf, "%s appears", */
							an(genericwere));
			    	else
			    		Sprintf(buf, "%s VERB_AUFTAUCHEN", /* EN Sprintf(buf, "%s appear", */
							makeplural(genericwere));
				pline("SUBJECT %s%s SATZKLAMMER!", upstart(buf), from_nowhere); /* EN pline("%s%s!", upstart(buf), from_nowhere); */
			    }
			} /* else no help came; but you didn't know it tried */
		}
	    }
	}

	if(u.uinvulnerable) {
	    /* monsters won't attack you */
	    if(mtmp == u.ustuck)
		pline("SUBJECT %s VERB_LOESEN OBJECT PRONOMEN_POSSESSIV_SUBJECT NOUN_GRIFF ein bisschen.", Monnam(mtmp)); /* EN pline("%s loosens its grip slightly.", Monnam(mtmp)); */
	    else if(!range2) {
		if (youseeit || sensemon(mtmp))
		    pline("SUBJECT %s VERB_WOLLEN OBJECT PRONOMEN_PERSONAL gerade angreifen, VERB_ZIEHEN sich aber zurück.", /* EN pline("%s starts to attack you, but pulls back.", */
			  Monnam(mtmp));
		else
				Du_spuerst("eine Bewegung OBJECT KASUS_DATIV in PRONOMEN_POSSESSIV NOUN_NAEHE."); /* EN You_feel("%s move nearby.", something); */
	    }
	    return (0);
	}

	/* Unlike defensive stuff, don't let them use item _and_ attack. */
	if(find_offensive(mtmp)) {
		int foo = use_offensive(mtmp);

		if (foo != 0) return(foo==1);
	}

	for(i = 0; i < NATTK; i++) {

	    sum[i] = 0;
	    mattk = getmattk(mdat, i, sum, &alt_attk);
	    if (u.uswallow && (mattk->aatyp != AT_ENGL))
		continue;
	    switch(mattk->aatyp) {
		case AT_CLAW:	/* "hand to hand" attacks */
		case AT_KICK:
		case AT_BITE:
		case AT_STNG:
		case AT_TUCH:
		case AT_BUTT:
		case AT_TENT:
			if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
					!touch_petrifies(youmonst.data))) {
			    if (foundyou) {
				if(tmp > (j = rnd(20+i))) {
				    if (mattk->aatyp != AT_KICK ||
					    !thick_skinned(youmonst.data))
					sum[i] = hitmu(mtmp, mattk);
				} else
				    missmu(mtmp, (tmp == j), mattk);
			    } else
				wildmiss(mtmp, mattk);
			}
			break;

		case AT_HUGS:	/* automatic if prev two attacks succeed */
			/* Note: if displaced, prev attacks never succeeded */
			if((!range2 && i>=2 && sum[i-1] && sum[i-2])
							|| mtmp == u.ustuck)
				sum[i]= hitmu(mtmp, mattk);
			break;

		case AT_GAZE:	/* can affect you either ranged or not */
			/* Medusa gaze already operated through m_respond in
			 * dochug(); don't gaze more than once per round.
			 */
			if (mdat != &mons[PM_MEDUSA])
				sum[i] = gazemu(mtmp, mattk);
			break;

		case AT_EXPL:	/* automatic hit if next to, and aimed at you */
			if(!range2) sum[i] = explmu(mtmp, mattk, foundyou);
			break;

		case AT_ENGL:
			if (!range2) {
			    if(foundyou) {
				if(u.uswallow || tmp > (j = rnd(20+i))) {
				    /* Force swallowing monster to be
				     * displayed even when player is
				     * moving away */
				    flush_screen(1);
				    sum[i] = gulpmu(mtmp, mattk);
				} else {
				    missmu(mtmp, (tmp == j), mattk);
				}
			    } else if (is_animal(mtmp->data)) {
				pline("SUBJECT %s VERB_VERSCHLUCKEN etwas Luft!", Monnam(mtmp)); /* EN pline("%s gulps some air!", Monnam(mtmp)); */
			    } else {
				if (youseeit)
				    pline("SUBJECT %s VERB_STUERZEN sich OBJECT auf PRONOMEN_PERSONAL und VERB_ZIEHEN sich wieder zurück!", /* EN pline("%s lunges forward and recoils!", */
					  Monnam(mtmp));
				else
				    You_hear("ganz nahe %s.", /* EN You_hear("a %s nearby.", */
					     is_whirly(mtmp->data) ?
						"ein Rauschen" : "ein Platschen"); /* EN "rushing noise" : "splat"); */
			   }
			}
			break;
		case AT_BREA:
			if(range2) sum[i] = breamu(mtmp, mattk);
			/* Note: breamu takes care of displacement */
			break;
		case AT_SPIT:
			if(range2) sum[i] = spitmu(mtmp, mattk);
			/* Note: spitmu takes care of displacement */
			break;
		case AT_WEAP:
			if(range2) {
#ifdef REINCARNATION
				if (!Is_rogue_level(&u.uz))
#endif
					thrwmu(mtmp);
			} else {
			    int hittmp = 0;

			    /* Rare but not impossible.  Normally the monster
			     * wields when 2 spaces away, but it can be
			     * teleported or whatever....
			     */
			    if (mtmp->weapon_check == NEED_WEAPON ||
							!MON_WEP(mtmp)) {
				mtmp->weapon_check = NEED_HTH_WEAPON;
				/* mon_wield_item resets weapon_check as
				 * appropriate */
				if (mon_wield_item(mtmp) != 0) break;
			    }
			    if (foundyou) {
				otmp = MON_WEP(mtmp);
				if(otmp) {
				    hittmp = hitval(otmp, &youmonst);
				    tmp += hittmp;
				    mswings(mtmp, otmp);
				}
				if(tmp > (j = dieroll = rnd(20+i)))
				    sum[i] = hitmu(mtmp, mattk);
				else
				    missmu(mtmp, (tmp == j), mattk);
				/* KMH -- Don't accumulate to-hit bonuses */
				if (otmp)
					tmp -= hittmp;
			    } else
				wildmiss(mtmp, mattk);
			}
			break;
		case AT_MAGC:
			if (range2)
			    sum[i] = buzzmu(mtmp, mattk);
			else {
			    if (foundyou)
				sum[i] = castmu(mtmp, mattk, TRUE, TRUE);
			    else
				sum[i] = castmu(mtmp, mattk, TRUE, FALSE);
			}
			break;

		default:		/* no attack */
			break;
	    }
	    if(flags.botl) bot();
	/* give player a chance of waking up before dying -kaa */
	    if(sum[i] == 1) {	    /* successful attack */
		if (u.usleep && u.usleep < monstermoves && !rn2(10)) {
		    multi = -1;
		    nomovemsg = "Der Kampf VERB_REISSEN OBJECT PRONOMEN_PERSONAL aus dem Schlaf."; /* EN nomovemsg = "The combat suddenly awakens you."; */
		}
	    }
	    if(sum[i] == 2) return 1;		/* attacker dead */
	    if(sum[i] == 3) break;  /* attacker teleported, no more attacks */
	    /* sum[i] == 0: unsuccessful attack */
	}
	return(0);
}

#endif /* OVL0 */
#ifdef OVLB

/*
 * helper function for some compilers that have trouble with hitmu
 */

STATIC_OVL void
hurtarmor(attk)
int attk;
{
	int	hurt;

	switch(attk) {
	    /* 0 is burning, which we should never be called with */
	    case AD_RUST: hurt = 1; break;
	    case AD_CORR: hurt = 3; break;
	    default: hurt = 2; break;
	}

	/* What the following code does: it keeps looping until it
	 * finds a target for the rust monster.
	 * Head, feet, etc... not covered by metal, or covered by
	 * rusty metal, are not targets.  However, your body always
	 * is, no matter what covers it.
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !rust_dmg(uarmh, xname(uarmh), hurt, FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)rust_dmg(uarmc, xname(uarmc), hurt, TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)rust_dmg(uarm, xname(uarm), hurt, TRUE, &youmonst);
#ifdef TOURIST
		else if (uarmu)
		    (void)rust_dmg(uarmu, xname(uarmu), hurt, TRUE, &youmonst);
#endif
		break;
	    case 2:
		if (!uarms || !rust_dmg(uarms, xname(uarms), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !rust_dmg(uarmg, xname(uarmg), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !rust_dmg(uarmf, xname(uarmf), hurt, FALSE, &youmonst))
		    continue;
		break;
	    }
	    break; /* Out of while loop */
	}
}

#endif /* OVLB */
#ifdef OVL1

STATIC_OVL boolean
diseasemu(mdat)
struct permonst *mdat;
{
	if (Sick_resistance) {
		Du_fuehlst_dich("leicht krank."); /* EN You_feel("a slight illness."); */
		return FALSE;
	} else {
		make_sick(Sick ? Sick/3L + 1L : (long)rn1(ACURR(A_CON), 20),
			mdat->mname, TRUE, SICK_NONVOMITABLE);
		return TRUE;
	}
}

/* check whether slippery clothing protects from hug or wrap attack */
STATIC_OVL boolean
u_slip_free(mtmp, mattk)
struct monst *mtmp;
struct attack *mattk;
{
	struct obj *obj = (uarmc ? uarmc : uarm);

#ifdef TOURIST
	if (!obj) obj = uarmu;
#endif
	if (mattk->adtyp == AD_DRIN) obj = uarmh;

	/* if your cloak/armor is greased, monster slips off; this
	   protection might fail (33% chance) when the armor is cursed */
	if (obj && (obj->greased || obj->otyp == OILSKIN_CLOAK) &&
		(!obj->cursed || rn2(3))) {
	    pline("SUBJECT %s %s PRONOMEN_POSSESSIV %s %s %s!", /* EN pline("%s %s your %s %s!", */
		  Monnam(mtmp),
		  (mattk->adtyp == AD_WRAP) ?
			"VERB_GLEITEN OBJECT KASUS_DATIV von" : "VERB_PACKEN OBJECT PRONOMEN_PERSONAL, VERB_KOENNEN sich aber nicht NEUES_OBJECT OBJECT KASUS_DATIV an", /* EN "slips off of" : "grabs you, but cannot hold onto", */
		  obj->greased ? "ADJEKTIV_GREASED" : "ADJEKTIV_SLIPPERY", /* EN obj->greased ? "greased" : "slippery", */
		  /* avoid "slippery slippery cloak"
		     for undiscovered oilskin cloak */
		  (obj->greased || objects[obj->otyp].oc_name_known) ?
			xname(obj) : cloak_simple_name(obj), /* EN xname(obj) : cloak_simple_name(obj)); */
		  (mattk->adtyp == AD_WRAP) ? "ab" : "festhalten"); /* EN */

	    if (obj->greased && !rn2(2)) {
		pline("Das Fett löst sich auf."); /* EN pline_The("grease wears off."); */
		obj->greased = 0;
		update_inventory();
	    }
	    return TRUE;
	}
	return FALSE;
}

/* armor that sufficiently covers the body might be able to block magic */
int
magic_negation(mon)
struct monst *mon;
{
	struct obj *armor;
	int armpro = 0;

	armor = (mon == &youmonst) ? uarm : which_armor(mon, W_ARM);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmc : which_armor(mon, W_ARMC);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmh : which_armor(mon, W_ARMH);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

	/* armor types for shirt, gloves, shoes, and shield don't currently
	   provide any magic cancellation but we might as well be complete */
#ifdef TOURIST
	armor = (mon == &youmonst) ? uarmu : which_armor(mon, W_ARMU);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
#endif
	armor = (mon == &youmonst) ? uarmg : which_armor(mon, W_ARMG);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmf : which_armor(mon, W_ARMF);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarms : which_armor(mon, W_ARMS);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

#ifdef STEED
	/* this one is really a stretch... */
	armor = (mon == &youmonst) ? 0 : which_armor(mon, W_SADDLE);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
#endif

	return armpro;
}

/*
 * hitmu: monster hits you
 *	  returns 2 if monster dies (e.g. "yellow light"), 1 otherwise
 *	  3 if the monster lives but teleported/paralyzed, so it can't keep
 *	       attacking you
 */
STATIC_OVL int
hitmu(mtmp, mattk)
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	register struct permonst *mdat = mtmp->data;
	register int uncancelled, ptmp;
	int dmg, armpro, permdmg;
	char	 buf[BUFSZ];
	struct permonst *olduasmon = youmonst.data;
	int res;

	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);

/*	If the monster is undetected & hits you, you should know where
 *	the attack came from.
 */
	if(mtmp->mundetected && (hides_under(mdat) || mdat->mlet == S_EEL)) {
	    mtmp->mundetected = 0;
	    if (!(Blind ? Blind_telepat : Unblind_telepat)) {
		struct obj *obj;
		const char *what;

		if ((obj = level.objects[mtmp->mx][mtmp->my]) != 0) {
		    if (Blind && !obj->dknown)
			what = something;
		    else if (is_pool(mtmp->mx, mtmp->my) && !Underwater)
			what = "Wasser"; /* EN what = "the water"; */
		    else
			what = doname(obj);

		    pline("SUBJECT %s VERB_VERSTECKEN sich unter KASUS_DATIV %s!", Amonnam(mtmp), what); /* EN pline("%s was hidden under %s!", Amonnam(mtmp), what); */
		}
		newsym(mtmp->mx, mtmp->my);
	    }
	}

/*	First determine the base damage done */
	dmg = d((int)mattk->damn, (int)mattk->damd);
	if(is_undead(mdat) && midnight())
		dmg += d((int)mattk->damn, (int)mattk->damd); /* extra damage */

/*	Next a cancellation factor	*/
/*	Use uncancelled when the cancellation factor takes into account certain
 *	armor's special magic protection.  Otherwise just use !mtmp->mcan.
 */
	armpro = magic_negation(&youmonst);
	uncancelled = !mtmp->mcan && ((rn2(3) >= armpro) || !rn2(50));

	permdmg = 0;
/*	Now, adjust damages via resistances or specific attacks */
	switch(mattk->adtyp) {
	    case AD_PHYS:
		if (mattk->aatyp == AT_HUGS && !sticks(youmonst.data)) {
		    if(!u.ustuck && rn2(2)) {
			if (u_slip_free(mtmp, mattk)) {
			    dmg = 0;
			} else {
			    u.ustuck = mtmp;
			    pline("SUBJECT %s VERB_PACKEN OBJECT PRONOMEN_PERSONAL!", Monnam(mtmp)); /* EN pline("%s grabs you!", Monnam(mtmp)); */
			}
		    } else if(u.ustuck == mtmp) {
			exercise(A_STR, FALSE);
			You("VERB_WERDEN %s.", /* EN You("are being %s.", */
			      (mtmp->data == &mons[PM_ROPE_GOLEM])
			      ? "gewürgt" : "gequetscht"); /* EN ? "choked" : "crushed"); */
		    }
		} else {			  /* hand to hand weapon */
		    if(mattk->aatyp == AT_WEAP && otmp) {
			if (otmp->otyp == CORPSE
				&& touch_petrifies(&mons[otmp->corpsenm])) {
			    dmg = 1;
			    pline("SUBJECT %s VERB_HIT OBJECT PRONOMEN_PERSONAL NEUES_OBJECT OBJECT KASUS_DATIV mit ARTIKEL_BESTIMMTER MODIFIER_CORPSE %s NOUN_CORPSE.", /* EN pline("%s hits you with the %s corpse.", */
				Monnam(mtmp), mons[otmp->corpsenm].mname);
			    if (!Stoned)
				goto do_stone;
			}
			dmg += dmgval(otmp, &youmonst);
			if (dmg <= 0) dmg = 1;
			if (!(otmp->oartifact &&
				artifact_hit(mtmp, &youmonst, otmp, &dmg,dieroll)))
			     hitmsg(mtmp, mattk);
			if (!dmg) break;
			if (u.mh > 1 && u.mh > ((u.uac>0) ? dmg : dmg+u.uac) &&
				   objects[otmp->otyp].oc_material == IRON &&
					(u.umonnum==PM_BLACK_PUDDING
					|| u.umonnum==PM_BROWN_PUDDING)) {
			    /* This redundancy necessary because you have to
			     * take the damage _before_ being cloned.
			     */
			    if (u.uac < 0) dmg += u.uac;
			    if (dmg < 1) dmg = 1;
			    if (dmg > 1) exercise(A_STR, FALSE);
			    u.mh -= dmg;
			    flags.botl = 1;
			    dmg = 0;
			    if(cloneu())
			    You("VERB_TEILEN OBJECT PRONOMEN_PERSONAL, als NEUER_SATZ SUBJECT_IM_SATZ %s OBJECT PRONOMEN_PERSONAL VERB_HIT!",mon_nam(mtmp)); /* EN You("divide as %s hits you!",mon_nam(mtmp)); */
			}
			urustm(mtmp, otmp);
		    } else if (mattk->aatyp != AT_TUCH || dmg != 0 ||
				mtmp != u.ustuck)
			hitmsg(mtmp, mattk);
		}
		break;
	    case AD_DISE:
		hitmsg(mtmp, mattk);
		if (!diseasemu(mdat)) dmg = 0;
		break;
	    case AD_FIRE:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("SUBJECT PRONOMEN_PERSONAL %s!", on_fire(youmonst.data, mattk)); /* EN pline("You're %s!", on_fire(youmonst.data, mattk)); */
		    if (youmonst.data == &mons[PM_STRAW_GOLEM] ||
		        youmonst.data == &mons[PM_PAPER_GOLEM]) {
			    You("VERB_BRUTZELN!"); /* EN You("roast!"); */
			    /* KMH -- this is okay with unchanging */
			    rehumanize();
			    break;
		    } else if (Fire_resistance) {
			pline_The("NOUN_FEUER VERB_FUEHLEN sich nicht heiss an!"); /* EN pline_The("fire doesn't feel hot!"); */
			dmg = 0;
		    }
		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(SCROLL_CLASS, AD_FIRE);
		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(POTION_CLASS, AD_FIRE);
		    if((int) mtmp->m_lev > rn2(25))
			destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		} else dmg = 0;
		break;
	    case AD_COLD:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("You're covered in frost!"); /* EN pline("You're covered in frost!"); */ // TODO DE
		    if (Cold_resistance) {
			pline_The("frost doesn't seem cold!"); /* EN pline_The("frost doesn't seem cold!"); */ // TODO DE
			dmg = 0;
		    }
		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(POTION_CLASS, AD_COLD);
		} else dmg = 0;
		break;
	    case AD_ELEC:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    You("VERB_KRIEGEN eine gewischt!"); /* EN You("get zapped!"); */
		    if (Shock_resistance) {
			pline("Der Stromschlag kitzelt KASUS_AKKUSATIV PRONOMEN_PERSONAL nur!"); /* EN pline_The("zap doesn't shock you!"); */
			dmg = 0;
		    }
		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(WAND_CLASS, AD_ELEC);
		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(RING_CLASS, AD_ELEC);
		} else dmg = 0;
		break;
	    case AD_SLEE:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(5)) {
		    if (Sleep_resistance) break;
		    fall_asleep(-rnd(10), TRUE);
		    if (Blind) pline("Etwas versetzt KASUS_AKKUSATIV PRONOMEN_PERSONAL in Schlaf!"); /* EN if (Blind) You("are put to sleep!"); */
		    else pline("SUBJECT %s VERB_VERSETZEN OBJECT PRONOMEN_PERSONAL in Schlaf!", mon_nam(mtmp)); /* EN else You("are put to sleep by %s!", mon_nam(mtmp)); */
		}
		break;
	    case AD_BLND:
		if (can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0)) {
		    if (!Blind) pline("SUBJECT %s VERB_BLENDEN OBJECT PRONOMEN_PERSONAL!", Monnam(mtmp)); /* EN if (!Blind) pline("%s blinds you!", Monnam(mtmp)); */
		    make_blinded(Blinded+(long)dmg,FALSE);
		    if (!Blind) Your(vision_clears);
		}
		dmg = 0;
		break;
	    case AD_DRST:
		ptmp = A_STR;
		goto dopois;
	    case AD_DRDX:
		ptmp = A_DEX;
		goto dopois;
	    case AD_DRCO:
		ptmp = A_CON;
dopois:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(8)) {
		    Sprintf(buf, "%s", /* EN Sprintf(buf, "%s %s", */
			    genitivattribut_zu_wort(Monnam(mtmp), mpoisons_subj(mtmp, mattk))); /* EN s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk)); */
		    poisoned(buf, ptmp, mdat->mname, 30);
		}
		break;
	    case AD_DRIN:
		hitmsg(mtmp, mattk);
		if (defends(AD_DRIN, uwep) || !has_head(youmonst.data)) {
		    You("VERB_SEEM unverletzt SATZKLAMMER."); /* EN You("don't seem harmed."); */
		    /* Not clear what to do for green slimes */
		    break;
		}
		if (u_slip_free(mtmp,mattk)) break;

		if (uarmh && rn2(8)) {
		    /* not body_part(HEAD) */
		    Your("NOUN_HELMET VERB_BLOCKEN OBJECT ARTIKEL_BESTIMMTER NOUN_ATTACK NEUES_OBJECT OBJECT auf PRONOMEN_POSSESSIV NOUN_KOPF."); /* EN Your("helmet blocks the attack to your head."); */
		    break;
		}
		if (Half_physical_damage) dmg = (dmg+1) / 2;
		mdamageu(mtmp, dmg);

		if (!uarmh || uarmh->otyp != DUNCE_CAP) {
		    Your("NOUN_GEHIRN wird gefressen!"); /* EN Your("brain is eaten!"); */
		    /* No such thing as mindless players... */
		    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
			int lifesaved = 0;
			struct obj *wore_amulet = uamul;

			while(1) {
			    /* avoid looping on "die(y/n)?" */
			    if (lifesaved && (discover || wizard)) {
				if (wore_amulet && !uamul) {
				    /* used up AMULET_OF_LIFE_SAVING; still
				       subject to dying from brainlessness */
				    wore_amulet = 0;
				} else {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    Du_fuehlst_dich("wie eine Vogelscheuche."); /* EN You_feel("like a scarecrow."); */
				    break;
				}
			    }

			    if (lifesaved)
				pline("Unglücklicherweise ist PRONOMEN_POSSESSIV NOUN_GEHIRN immer noch futsch."); /* EN pline("Unfortunately your brain is still gone."); */
			    else
				Your("ADJEKTIV_LETZT NOUN_GEDANKEs VERB_SCHWINDEN dahin."); /* EN Your("last thought fades away."); */
			    killer = "Gedankenlosigkeit"; /* EN killer = "brainlessness"; */
			    killer_format = KILLED_BY;
			    done(DIED);
			    lifesaved++;
			}
		    }
		}
		/* adjattrib gives dunce cap message when appropriate */
		(void) adjattrib(A_INT, -rnd(2), FALSE);
		forget_levels(25);	/* lose memory of 25% of levels */
		forget_objects(25);	/* lose memory of 25% of objects */
		exercise(A_WIS, FALSE);
		break;
	    case AD_PLYS:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(3)) {
		    if (Free_action) {
			You("VERB_VERSTEIFEN OBJECT PRONOMEN_PERSONAL für einen Moment.");            /* EN You("momentarily stiffen.");             */
		    } else {
			if (Blind) You("VERB_ERSTARREN!"); /* EN if (Blind) You("are frozen!"); */
			else You("VERB_ERSTARREN OBJECT KASUS_DATIV vor %s!", mon_nam(mtmp)); /* EN else You("are frozen by %s!", mon_nam(mtmp)); */
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10));
			exercise(A_DEX, FALSE);
		    }
		}
		break;
	    case AD_DRLI:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(3) && !Drain_resistance) {
		    losexp("Lebensenergieentzug"); /* EN losexp("life drainage"); */
		}
		break;
	    case AD_LEGS:
		{ register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
		  const char *sidestr = (side == RIGHT_SIDE) ? "ADJEKTIV_RECHT" : "ADJEKTIV_LINK"; /* EN const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left"; */

		/* This case is too obvious to ignore, but Nethack is not in
		 * general very good at considering height--most short monsters
		 * still _can_ attack you when you're flying or mounted.
		 * [FIXME: why can't a flying attacker overcome this?]
		 */
		  if (
#ifdef STEED
			u.usteed ||
#endif
				    Levitation || Flying) {
		    pline("SUBJECT %s VERB_VERSUCHEN OBJECT PRONOMEN_POSSESSIV %s %s zu packen!", Monnam(mtmp), /* EN pline("%s tries to reach your %s %s!", Monnam(mtmp), */
			  sidestr, body_part(LEG));
		    dmg = 0;
		  } else if (mtmp->mcan) {
		    pline("SUBJECT %s VERB_STUPSEN OBJECT an PRONOMEN_POSSESSIV %s %s!", Monnam(mtmp), /* EN pline("%s nuzzles against your %s %s!", Monnam(mtmp), */
			  sidestr, body_part(LEG));
		    dmg = 0;
		  } else {
		    if (uarmf) {
			if (rn2(2) && (uarmf->otyp == LOW_BOOTS ||
					     uarmf->otyp == IRON_SHOES))
			    pline("SUBJECT %s VERB_PIEKSEN OBJECT PRONOMEN_PERSONAL NEUES_OBJECT OBJECT in ARTIKEL_BESTIMMTER ADJEKTIV_UNGESCHUETZT NOUN_TEIL OBJECT KASUS_GENITIV PRONOMEN_POSSESSIV %s %s!", /* EN pline("%s pricks the exposed part of your %s %s!", */
				Monnam(mtmp), sidestr, body_part(LEG));
			else if (!rn2(5))
			    pline("SUBJECT %s VERB_DURCHSTECHEN OBJECT PRONOMEN_POSSESSIV %s NOUN_BOOTS!", /* EN pline("%s pricks through your %s boot!", */
				Monnam(mtmp), sidestr);
			else {
			    pline("SUBJECT %s VERB_ZERKRATZEN OBJECT PRONOMEN_POSSESSIV %s NOUN_BOOTS!", Monnam(mtmp), /* EN pline("%s scratches your %s boot!", Monnam(mtmp), */
				sidestr);
			    dmg = 0;
			    break;
			}
		    } else pline("SUBJECT %s VERB_PIEKSEN OBJECT PRONOMEN_PERSONAL NEUES_OBJECT OBJECT in PRONOMEN_POSSESSIV %s %s!", Monnam(mtmp), /* EN } else pline("%s pricks your %s %s!", Monnam(mtmp), */
			  sidestr, body_part(LEG));
		    set_wounded_legs(side, rnd(60-ACURR(A_DEX)));
		    exercise(A_STR, FALSE);
		    exercise(A_DEX, FALSE);
		  }
		  break;
		}
	    case AD_STON:	/* cockatrice */
		hitmsg(mtmp, mattk);
		if(!rn2(3)) {
		    if (mtmp->mcan) {
			if (flags.soundok)
			    You_hear("ein Husten OBJECT KASUS_DATIV von %s!", mon_nam(mtmp)); /* EN You_hear("a cough from %s!", mon_nam(mtmp)); */
		    } else {
			if (flags.soundok)
			    You_hear("OBJECT %s!", genitivattribut_zu_wort(mon_nam(mtmp), "NOUN_ZISCHEN")); /* EN You_hear("%s hissing!", s_suffix(mon_nam(mtmp))); */
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
 do_stone:
			    if (!Stoned && !Stone_resistance
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				Stoned = 5;
				delayed_killer = mtmp->data->mname;
				if (mtmp->data->geno & G_UNIQ) {
				    if (!type_is_pname(mtmp->data)) {
					static char kbuf[BUFSZ];

					/* "the" buffer may be reallocated */
					Strcpy(kbuf, the(delayed_killer));
					delayed_killer = kbuf;
				    }
				    killer_format = KILLED_BY;
				} else killer_format = KILLED_BY_AN;
				return(1);
				/* You("turn to stone..."); */
				/* done_in_by(mtmp); */
			    }
			}
		    }
		}
		break;
	    case AD_STCK:
		hitmsg(mtmp, mattk);
		if (uncancelled && !u.ustuck && !sticks(youmonst.data))
			u.ustuck = mtmp;
		break;
	    case AD_WRAP:
		if ((!mtmp->mcan || u.ustuck == mtmp) && !sticks(youmonst.data)) {
		    if (!u.ustuck && !rn2(10)) {
			if (u_slip_free(mtmp, mattk)) {
			    dmg = 0;
			} else {
			    pline("SUBJECT %s VERB_UMSCHLINGEN OBJECT PRONOMEN_PERSONAL!", /* EN pline("%s swings itself around you!", */
				  Monnam(mtmp));
			    u.ustuck = mtmp;
			}
		    } else if(u.ustuck == mtmp) {
			if (is_pool(mtmp->mx,mtmp->my) && !Swimming
			    && !Amphibious) {
			    boolean moat =
				(levl[mtmp->mx][mtmp->my].typ != POOL) &&
				(levl[mtmp->mx][mtmp->my].typ != WATER) &&
				!Is_medusa_level(&u.uz) &&
				!Is_waterlevel(&u.uz);

			    pline("SUBJECT %s VERB_ERTRAENKEN OBJECT PRONOMEN_PERSONAL ...", Monnam(mtmp)); /* EN pline("%s drowns you...", Monnam(mtmp)); */
			    killer_format = KILLED_BY_AN;
			    Sprintf(buf, "%s KASUS_GENITIV wegen %s", /* EN Sprintf(buf, "%s by %s", */
				    moat ? "NOUN_MOAT" : "NOUN_POOL_OF_WATER", /* EN moat ? "moat" : "pool of water", */
				    an(mtmp->data->mname));
			    killer = buf;
			    done(DROWNING);
			} else if(mattk->aatyp == AT_HUGS)
			    You("VERB_WERDEN gequetscht."); /* EN You("are being crushed."); */
		    } else {
			dmg = 0;
			if(flags.verbose)
			    pline("SUBJECT %s VERB_STREIFEN OBJECT PRONOMEN_POSSESSIV %s.", Monnam(mtmp), /* EN pline("%s brushes against your %s.", Monnam(mtmp), */
				   body_part(LEG));
		    }
		} else dmg = 0;
		break;
	    case AD_WERE:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(4) && u.ulycn == NON_PM &&
			!Protection_from_shape_changers &&
			!defends(AD_WERE,uwep)) {
		    Du_fuehlst_dich("fiebrig."); /* EN You_feel("feverish."); */
		    exercise(A_CON, FALSE);
		    u.ulycn = monsndx(mdat);
		}
		break;
	    case AD_SGLD:
		hitmsg(mtmp, mattk);
		if (youmonst.data->mlet == mdat->mlet) break;
		if(!mtmp->mcan) stealgold(mtmp);
		break;

	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
		if (is_animal(mtmp->data)) {
			hitmsg(mtmp, mattk);
			if (mtmp->mcan) break;
			/* Continue below */
		} else if (dmgtype(youmonst.data, AD_SEDU)
#ifdef SEDUCE
			|| dmgtype(youmonst.data, AD_SSEX)
#endif
						) {
			pline("SUBJECT %s %s.", Monnam(mtmp), mtmp->minvent ?  /* EN pline("%s %s.", Monnam(mtmp), mtmp->minvent ? */
		    "prahlt mit den Sachen, die ein Schatzsucher gespendet hat" : /* EN "brags about the goods some dungeon explorer provided" : */
		    "erwähnt, wie schwierig Diebstahl in letzter Zeit geworden ist."); /* EN "makes some remarks about how difficult theft is lately"); */
			if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
			return 3;
		} else if (mtmp->mcan) {
		    if (!Blind) {
			pline("SUBJECT %s VERB_VERSUCHEN OBJECT PRONOMEN_PERSONAL zu %s, NEUER_SATZ aber SUBJECT_IM_SATZ PRONOMEN_PERSONAL VERB_SCHEINEN %s.", /* EN pline("%s tries to %s you, but you seem %s.", */
			    Adjmonnam(mtmp, "ADJEKTIV_UNATTRAKTIV"), /* EN Adjmonnam(mtmp, "plain"), */
			    flags.female ? "MODIFIER_VERB_INFINITIV VERB_BEZIRZEN" : "VERB_SEDUCE", /* EN flags.female ? "charm" : "seduce", */
			    flags.female ? "unaffected" : "kein Interesse zu haben"); /* EN flags.female ? "unaffected" : "uninterested"); */ // TODO DE
		    }
		    if(rn2(3)) {
			if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
			return 3;
		    }
		    break;
		}
		buf[0] = '\0';
		switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if (!is_animal(mtmp->data) && !tele_restrict(mtmp))
			    (void) rloc(mtmp, FALSE);
			if (is_animal(mtmp->data) && *buf) {
			    if (canseemon(mtmp))
				pline("SUBJECT %s VERB_VERSUCHEN OBJECT KASUS_DATIV mit %s MODIFIER_VERB_INFINITIV davonzu-%s.", /* EN pline("%s tries to %s away with %s.", */ /* fortrennen, wegrennen, weglaufen, davonlaufen, weglaufen, davonrennen */
				      Monnam(mtmp),
				      buf, locomotion(mtmp->data, "VERB_LAUFEN") /* EN locomotion(mtmp->data, "run"), */
				      ); /* EN buf); */
			}
			monflee(mtmp, 0, FALSE, FALSE);
			return 3;
		}
		break;
#ifdef SEDUCE
	    case AD_SSEX:
		if(could_seduce(mtmp, &youmonst, mattk) == 1
			&& !mtmp->mcan)
		    if (doseduce(mtmp))
			return 3;
		break;
#endif
	    case AD_SAMU:
		hitmsg(mtmp, mattk);
		/* when the Wiz hits, 1/20 steals the amulet */
		if (u.uhave.amulet ||
		     u.uhave.bell || u.uhave.book || u.uhave.menorah
		     || u.uhave.questart) /* carrying the Quest Artifact */
		    if (!rn2(20)) stealamulet(mtmp);
		break;

	    case AD_TLPT:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    if(flags.verbose)
			Your("NOUN_POSITION VERB_SCHEINEN plötzlich sehr unbestimmt!"); /* EN Your("position suddenly seems very uncertain!"); */
		    tele();
		}
		break;
	    case AD_RUST:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (u.umonnum == PM_IRON_GOLEM) {
			You("VERB_ROSTEN!"); /* EN You("rust!"); */
			/* KMH -- this is okay with unchanging */
			rehumanize();
			break;
		}
		hurtarmor(AD_RUST);
		break;
	    case AD_CORR:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		hurtarmor(AD_CORR);
		break;
	    case AD_DCAY:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("VERB_VERMODERN!"); /* EN You("rot!"); */
			/* KMH -- this is okay with unchanging */
			rehumanize();
			break;
		}
		hurtarmor(AD_DCAY);
		break;
	    case AD_HEAL:
		/* a cancelled nurse is just an ordinary monster */
		if (mtmp->mcan) {
		    hitmsg(mtmp, mattk);
		    break;
		}
		if(!uwep
#ifdef TOURIST
		   && !uarmu
#endif
		   && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    boolean goaway = FALSE;
		    pline("SUBJECT %s VERB_HIT!  (Macht aber nichts, oder?)", Monnam(mtmp)); /* EN pline("%s hits!  (I hope you don't mind.)", Monnam(mtmp)); */
		    if (Upolyd) {
			u.mh += rnd(7);
			if (!rn2(7)) {
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			    if (!rn2(13)) goaway = TRUE;
			}
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			if (!rn2(7)) {
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10))
				u.uhpmax++;
			    if (!rn2(13)) goaway = TRUE;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    }
		    if (!rn2(3)) exercise(A_STR, TRUE);
		    if (!rn2(3)) exercise(A_CON, TRUE);
		    if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    flags.botl = 1;
		    if (goaway) {
			mongone(mtmp);
			return 2;
		    } else if (!rn2(33)) {
			if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
			monflee(mtmp, d(3, 6), TRUE, FALSE);
			return 3;
		    }
		    dmg = 0;
		} else {
		    if (Role_if(PM_HEALER)) {
			if (flags.soundok && !(moves % 5))
		      verbalize("Doc, I can't help you unless you cooperate."); /* EN verbalize("Doc, I can't help you unless you cooperate."); */ // TODO DE
			dmg = 0;
		    } else hitmsg(mtmp, mattk);
		}
		break;
	    case AD_CURS:
		hitmsg(mtmp, mattk);
		if(!night() && mdat == &mons[PM_GREMLIN]) break;
		if(!mtmp->mcan && !rn2(10)) {
		    if (flags.soundok) {
			if (Blind) You_hear("Gelächter."); /* EN if (Blind) You_hear("laughter."); */
			else       pline("SUBJECT %s VERB_CHUCKLE.", Monnam(mtmp)); /* EN else       pline("%s chuckles.", Monnam(mtmp)); */
		    }
		    if (u.umonnum == PM_CLAY_GOLEM) {
			pline("Ein Zeichen verschwindet KASUS_DATIV von PRONOMEN_POSSESSIV NOUN_STIRN!"); /* EN pline("Some writing vanishes from your head!"); */
			/* KMH -- this is okay with unchanging */
			rehumanize();
			break;
		    }
		    attrcurse();
		}
		break;
	    case AD_STUN:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(4)) {
		    make_stunned(HStun + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_ACID:
		hitmsg(mtmp, mattk);
		if (!mtmp->mcan && !rn2(3))
		    if (Acid_resistance) {
			pline("You're covered in acid, but it seems harmless."); /* EN pline("You're covered in acid, but it seems harmless."); */ // TODO DE
			dmg = 0;
		    } else {
			pline("You're covered in acid!	It burns!"); /* EN pline("You're covered in acid!	It burns!"); */ // TODO DE
			exercise(A_STR, FALSE);
		    }
		else		dmg = 0;
		break;
	    case AD_SLOW:
		hitmsg(mtmp, mattk);
		if (uncancelled && HFast &&
					!defends(AD_SLOW, uwep) && !rn2(4))
		    u_slow_down();
		break;
	    case AD_DREN:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(4))
		    drain_en(dmg);
		dmg = 0;
		break;
	    case AD_CONF:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(4) && !mtmp->mspec_used) {
		    mtmp->mspec_used = mtmp->mspec_used + (dmg + rn2(6));
		    if(Confusion)
			 You("VERB_WERDEN noch verwirrter."); /* EN You("are getting even more confused."); */
		    else You("VERB_WERDEN verwirrt."); /* EN else You("are getting confused."); */
		    make_confused(HConfusion + dmg, FALSE);
		}
		dmg = 0;
		break;
	    case AD_DETH:
		pline("SUBJECT %s VERB_STRECKEN seine Hand OBJECT KASUS_DATIV nach PRONOMEN_PERSONAL aus.", Monnam(mtmp)); /* EN pline("%s reaches out with its deadly touch.", Monnam(mtmp)); */
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("War das die Hand des Todes?"); /* EN pline("Was that the touch of death?"); */
		    break;
		}
		switch (rn2(20)) {
		case 19: case 18: case 17:
		    if (!Antimagic) {
			killer_format = KILLED_BY_AN;
			killer = "NOUN_HAND_DES_TODES"; /* EN killer = "touch of death"; */
			done(DIED);
			dmg = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You("VERB_SPUEREN, NEUER_SATZ wie SUBJECT_IM_SATZ PRONOMEN_POSSESSIV NOUN_LEBENSKRAFT VERB_SCHWINDEN ..."); /* EN You_feel("your life force draining away..."); */
		    permdmg = 1;	/* actual damage done below */
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("SATZBEGINN PRONOMEN_POSSESSIV NOUN_LUCK, hat nicht geklappt!"); /* EN pline("Lucky for you, it didn't work!"); */
		    dmg = 0;
		    break;
		}
		break;
	    case AD_PEST:
		pline("SUBJECT %s VERB_AUSSTRECKEN die Hand SATZKLAMMER und NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL VERB_BEKOMMEN hohes Fieber und Schüttelfrost.", /* EN pline("%s reaches out, and you feel fever and chills.", */
			Monnam(mtmp));
		(void) diseasemu(mdat); /* plus the normal damage */
		break;
	    case AD_FAMN:
		pline("SUBJECT %s VERB_AUSSTRECKEN die Hand SATZKLAMMER und NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_POSSESSIV NOUN_KOERPER VERB_DAHINSIECHEN SATZKLAMMER.", /* EN pline("%s reaches out, and your body shrivels.", */
			Monnam(mtmp));
		exercise(A_CON, FALSE);
		if (!is_fainted()) morehungry(rn1(40,40));
		/* plus the normal damage */
		break;
	    case AD_SLIM:    
		hitmsg(mtmp, mattk);
		if (!uncancelled) break;
		if (flaming(youmonst.data)) {
		    pline("Der Schleim verbrennt!"); /* EN pline_The("slime burns away!"); */
		    dmg = 0;
		} else if (Unchanging ||
				youmonst.data == &mons[PM_GREEN_SLIME]) {
		    You("are unaffected."); /* EN You("are unaffected."); */ // TODO DE
		    dmg = 0;
		} else if (!Slimed) {
		    Du_fuehlst_dich("nicht so gut."); /* EN You("don't feel very well."); */
		    Slimed = 10L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Igitt!"); /* EN pline("Yuck!"); */
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		hitmsg(mtmp, mattk);
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		if (uncancelled) {
		    struct obj *obj = some_armor(&youmonst);

		    if (drain_item(obj)) {
			Your("%s weniger wirksam.", aobjnam(obj, "VERB_SCHEINEN")); /* EN Your("%s less effective.", aobjnam(obj, "seem")); */
		    }
		}
		break;
	    default:	dmg = 0;
			break;
	}
	if(u.uhp < 1) done_in_by(mtmp);

/*	Negative armor class reduces damage done instead of fully protecting
 *	against hits.
 */
	if (dmg && u.uac < 0) {
		dmg -= rnd(-u.uac);
		if (dmg < 1) dmg = 1;
	}

	if(dmg) {
	    if (Half_physical_damage
					/* Mitre of Holiness */
		|| (Role_if(PM_PRIEST) && uarmh && is_quest_artifact(uarmh) &&
		    (is_undead(mtmp->data) || is_demon(mtmp->data))))
		dmg = (dmg+1) / 2;

	    if (permdmg) {	/* Death's life force drain */
		int lowerlimit, *hpmax_p;
		/*
		 * Apply some of the damage to permanent hit points:
		 *	polymorphed	    100% against poly'd hpmax
		 *	hpmax > 25*lvl	    100% against normal hpmax
		 *	hpmax > 10*lvl	50..100%
		 *	hpmax >  5*lvl	25..75%
		 *	otherwise	 0..50%
		 * Never reduces hpmax below 1 hit point per level.
		 */
		permdmg = rn2(dmg / 2 + 1);
		if (Upolyd || u.uhpmax > 25 * u.ulevel) permdmg = dmg;
		else if (u.uhpmax > 10 * u.ulevel) permdmg += dmg / 2;
		else if (u.uhpmax > 5 * u.ulevel) permdmg += dmg / 4;

		if (Upolyd) {
		    hpmax_p = &u.mhmax;
		    /* [can't use youmonst.m_lev] */
		    lowerlimit = min((int)youmonst.data->mlevel, u.ulevel);
		} else {
		    hpmax_p = &u.uhpmax;
		    lowerlimit = u.ulevel;
		}
		if (*hpmax_p - permdmg > lowerlimit)
		    *hpmax_p -= permdmg;
		else if (*hpmax_p > lowerlimit)
		    *hpmax_p = lowerlimit;
		else	/* unlikely... */
		    ;	/* already at or below minimum threshold; do nothing */
		flags.botl = 1;
	    }

	    mdamageu(mtmp, dmg);
	}

	if (dmg)
	    res = passiveum(olduasmon, mtmp, mattk);
	else
	    res = 1;
	stop_occupation();
	return res;
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL int
gulpmu(mtmp, mattk)	/* monster swallows you, or damage if u.uswallow */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	struct trap *t = t_at(u.ux, u.uy);
	int	tmp = d((int)mattk->damn, (int)mattk->damd);
	int	tim_tmp;
	register struct obj *otmp2;
	int	i;

	if (!u.uswallow) {	/* swallows you */
		if (youmonst.data->msize >= MZ_HUGE) return(0);
		if ((t && ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT))) &&
		    sobj_at(BOULDER, u.ux, u.uy))
			return(0);

		if (Punished) unplacebc();	/* ball&chain go away */
		remove_monster(mtmp->mx, mtmp->my);
		mtmp->mtrapped = 0;		/* no longer on old trap */
		place_monster(mtmp, u.ux, u.uy);
		u.ustuck = mtmp;
		newsym(mtmp->mx,mtmp->my);
#ifdef STEED
		if (is_animal(mtmp->data) && u.usteed) {
			char buf[BUFSZ];
			/* Too many quirks presently if hero and steed
			 * are swallowed. Pretend purple worms don't
			 * like horses for now :-)
			 */
			Strcpy(buf, mon_nam(u.usteed));
			pline ("SUBJECT %s VERB_STUERZEN sich OBJECT auf PRONOMEN_PERSONAL und VERB_REISSEN OBJECT PRONOMEN_PERSONAL NEUES_OBJECT OBJECT KASUS_DATIV von %s!", /* EN pline ("%s lunges forward and plucks you off %s!", */
				Monnam(mtmp), buf);
			dismount_steed(DISMOUNT_ENGULFED);
		} else
#endif
		pline("SUBJECT %s VERB_ENGULF OBJECT PRONOMEN_PERSONAL!", Monnam(mtmp)); /* EN pline("%s engulfs you!", Monnam(mtmp)); */
		stop_occupation();
		reset_occupations();	/* behave as if you had moved */

		if (u.utrap) {
			You("VERB_KOMMEN OBJECT KASUS_DATIV aus ARTIKEL_BESTIMMTER %s frei!", /* EN You("are released from the %s!", */
				u.utraptype==TT_WEB ? "NOUN_WEB" : "NOUN_TRAP"); /* EN u.utraptype==TT_WEB ? "web" : "trap"); */
			u.utrap = 0;
		}

		i = number_leashed();
		if (i > 0) {
		    const char *s = (i > 1) ? "NOUN_LEASHs" : "NOUN_LEASH"; /* EN const char *s = (i > 1) ? "leashes" : "leash"; */
		    pline_The("%s %s sich.", s, vtense(s, "VERB_LOESEN")); /* EN pline_The("%s %s loose.", s, vtense(s, "snap")); */
		    unleash_all();
		}

		if (touch_petrifies(youmonst.data) && !resists_ston(mtmp)) {
			minstapetrify(mtmp, TRUE);
			if (mtmp->mhp > 0) return 0;
			else return 2;
		}

		display_nhwindow(WIN_MESSAGE, FALSE);
		vision_recalc(2);	/* hero can't see anything */
		u.uswallow = 1;
		/* u.uswldtim always set > 1 */
		tim_tmp = 25 - (int)mtmp->m_lev;
		if (tim_tmp > 0) tim_tmp = rnd(tim_tmp) / 2;
		else if (tim_tmp < 0) tim_tmp = -(rnd(-tim_tmp) / 2);
		tim_tmp += -u.uac + 10;
		u.uswldtim = (unsigned)((tim_tmp < 2) ? 2 : tim_tmp);
		swallowed(1);
		for (otmp2 = invent; otmp2; otmp2 = otmp2->nobj)
		    (void) snuff_lit(otmp2);
	}

	if (mtmp != u.ustuck) return(0);
	if (u.uswldtim > 0) u.uswldtim -= 1;

	switch(mattk->adtyp) {

		case AD_DGST:
		    if (Slow_digestion) {
			/* Messages are handled below */
			u.uswldtim = 0;
			tmp = 0;
		    } else if (u.uswldtim == 0) {
			pline("SUBJECT %s VERB_VERDAUEN OBJECT PRONOMEN_PERSONAL vollends!", Monnam(mtmp)); /* EN pline("%s totally digests you!", Monnam(mtmp)); */
			tmp = u.uhp;
			if (Half_physical_damage) tmp *= 2; /* sorry */
		    } else {
			pline("SUBJECT %s VERB_VERDAUEN OBJECT PRONOMEN_PERSONAL%s!", Monnam(mtmp), /* EN pline("%s%s digests you!", Monnam(mtmp), */
			      (u.uswldtim == 2) ? " etwas" : /* EN (u.uswldtim == 2) ? " thoroughly" : */
			      (u.uswldtim == 1) ? " gründlich" : ""); /* EN (u.uswldtim == 1) ? " utterly" : ""); */
			exercise(A_STR, FALSE);
		    }
		    break;
		case AD_PHYS:
		    if (mtmp->data == &mons[PM_FOG_CLOUD]) {
			You("VERB_SEIN von feuchtem Wasserdampf eingehüllt und %s!", /* EN You("are laden with moisture and %s", */
			    flaming(youmonst.data) ? "VERB_DAMPFEN gewaltig!" : /* EN flaming(youmonst.data) ? "are smoldering out!" : */
			    Breathless ? "VERB_FINDEN es etwas unangenehm." : /* EN Breathless ? "find it mildly uncomfortable." : */
			    amphibious(youmonst.data) ? "VERB_FUEHLEN OBJECT PRONOMEN_PERSONAL sauwohl." : /* EN amphibious(youmonst.data) ? "feel comforted." : */
			    "VERB_KOENNEN kaum atmen!"); /* EN "can barely breathe!"); */
			/* NB: Amphibious includes Breathless */
			if (Amphibious && !flaming(youmonst.data)) tmp = 0;
		    } else {
			pline("Schutt und Geröll donnern KASUS_AKKUSATIV auf PRONOMEN_PERSONAL herab!"); /* EN You("are pummeled with debris!"); */
			exercise(A_STR, FALSE);
		    }
		    break;
		case AD_ACID:
		    if (Acid_resistance) {
			You("are covered with a seemingly harmless goo."); /* EN You("are covered with a seemingly harmless goo."); */ // TODO DE
			tmp = 0;
		    } else {
		      if (Hallucination) pline("Autsch!  SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_WERDEN verschleimt!"); /* EN if (Hallucination) pline("Ouch!  You've been slimed!"); */
		      else You("are covered in slime!  It burns!"); /* EN else You("are covered in slime!  It burns!"); */ // TODO DE
		      exercise(A_STR, FALSE);
		    }
		    break;
		case AD_BLND:
		    if (can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0)) {
			if(!Blind) {
			    You("VERB_KOENNEN hier drinnen nichts sehen!"); /* EN You_cant("see in here!"); */
			    make_blinded((long)tmp,FALSE);
			    if (!Blind) Your(vision_clears);
			} else
			    /* keep him blind until disgorged */
			    make_blinded(Blinded+1,FALSE);
		    }
		    tmp = 0;
		    break;
		case AD_ELEC:
		    if(!mtmp->mcan && rn2(2)) {
			pline("Die Luft um KASUS_AKKUSATIV PRONOMEN_PERSONAL knistert vor Elektrizität."); /* EN pline_The("air around you crackles with electricity."); */
			if (Shock_resistance) {
				shieldeff(u.ux, u.uy);
				You("VERB_SCHEINEN unverletzt."); /* EN You("seem unhurt."); */
				ugolemeffects(AD_ELEC,tmp);
				tmp = 0;
			}
		    } else tmp = 0;
		    break;
		case AD_COLD:
		    if(!mtmp->mcan && rn2(2)) {
			if (Cold_resistance) {
				shieldeff(u.ux, u.uy);
				You("VERB_FROESTELN leicht."); /* EN You_feel("mildly chilly."); */
				ugolemeffects(AD_COLD,tmp);
				tmp = 0;
			} else You("VERB_SEIN am Erfrieren!"); /* EN } else You("are freezing to death!"); */
		    } else tmp = 0;
		    break;
		case AD_FIRE:
		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance) {
				shieldeff(u.ux, u.uy);
				You("VERB_SCHWITZEN leicht."); /* EN You_feel("mildly hot."); */
				ugolemeffects(AD_FIRE,tmp);
				tmp = 0;
			} else You("VERB_VERBRENNEN zu einem Aschehäufchen!"); /* EN } else You("are burning to a crisp!"); */
			burn_away_slime();
		    } else tmp = 0;
		    break;
		case AD_DISE:
		    if (!diseasemu(mtmp->data)) tmp = 0;
		    break;
		default:
		    tmp = 0;
		    break;
	}

	if (Half_physical_damage) tmp = (tmp+1) / 2;

	mdamageu(mtmp, tmp);
	if (tmp) stop_occupation();

	if (touch_petrifies(youmonst.data) && !resists_ston(mtmp)) {
	    pline("SUBJECT %s %s OBJECT PRONOMEN_PERSONAL hastig SATZKLAMMER!", Monnam(mtmp), /* EN pline("%s very hurriedly %s you!", Monnam(mtmp), */
		  is_animal(mtmp->data)? "VERB_REGURGITATE" : "VERB_EXPEL"); /* EN is_animal(mtmp->data)? "regurgitates" : "expels"); */
	    expels(mtmp, mtmp->data, FALSE);
	} else if (!u.uswldtim || youmonst.data->msize >= MZ_HUGE) {
	    You("VERB_WERDEN MODIFIER_VERB_PARTIZIP_PERFEKT %s!", is_animal(mtmp->data)? "VERB_REGURGITATE" : "VERB_EXPEL"); /* EN You("get %s!", is_animal(mtmp->data)? "regurgitated" : "expelled"); */
	    if (flags.verbose && (is_animal(mtmp->data) ||
		    (dmgtype(mtmp->data, AD_DGST) && Slow_digestion)))
		pline("Offensichtlich SUBJECT_IM_SATZ VERB_MOEGEN %s OBJECT PRONOMEN_POSSESSIV NOUN_GESCHMACK nicht.", mon_nam(mtmp)); /* EN pline("Obviously %s doesn't like your taste.", mon_nam(mtmp)); */
	    expels(mtmp, mtmp->data, FALSE);
	}
	return(1);
}

STATIC_OVL int
explmu(mtmp, mattk, ufound)	/* monster explodes in your face */
register struct monst *mtmp;
register struct attack  *mattk;
boolean ufound;
{
    if (mtmp->mcan) return(0);

    if (!ufound)
	pline("SUBJECT %s VERB_SPRENGEN sich OBJECT KASUS_DATIV neben PRONOMEN_PERSONAL %s!", /* EN pline("%s explodes at a spot in %s!", */
	    canseemon(mtmp) ? Monnam(mtmp) : "NOUN_ETWAS", /* EN canseemon(mtmp) ? Monnam(mtmp) : "It", */
	    levl[mtmp->mux][mtmp->muy].typ == WATER
		? "ins Wasser" : "in die Luft"); /* EN ? "empty water" : "thin air"); */
    else {
	register int tmp = d((int)mattk->damn, (int)mattk->damd);
	register boolean not_affected = defends((int)mattk->adtyp, uwep);

	hitmsg(mtmp, mattk);

	switch (mattk->adtyp) {
	    case AD_COLD:
		not_affected |= Cold_resistance;
		goto common;
	    case AD_FIRE:
		not_affected |= Fire_resistance;
		goto common;
	    case AD_ELEC:
		not_affected |= Shock_resistance;
common:

		if (!not_affected) {
		    if (ACURR(A_DEX) > rnd(20)) {
			You("VERB_VERSUCHEN der Druckwelle auszuweichen."); /* EN You("duck some of the blast."); */
			tmp = (tmp+1) / 2;
		    } else {
		        if (flags.verbose) pline("Die Druckwelle erfasst OBJECT KASUS_AKKUSATIV PRONOMEN_PERSONAL!"); /* EN if (flags.verbose) You("get blasted!"); */
		    }
		    if (mattk->adtyp == AD_FIRE) burn_away_slime();
		    if (Half_physical_damage) tmp = (tmp+1) / 2;
		    mdamageu(mtmp, tmp);
		}
		break;

	    case AD_BLND:
		not_affected = resists_blnd(&youmonst);
		if (!not_affected) {
		    /* sometimes you're affected even if it's invisible */
		    if (mon_visible(mtmp) || (rnd(tmp /= 2) > u.ulevel)) {
			You("VERB_WERDEN von einem Lichtblitz geblendet!"); /* EN You("are blinded by a blast of light!"); */
			make_blinded((long)tmp, FALSE);
			if (!Blind) Your(vision_clears);
		    } else if (flags.verbose)
			You("VERB_HABEN den Eindruck, dass es nicht besonders hell war."); /* EN You("get the impression it was not terribly bright."); */
		}
		break;

	    case AD_HALU:
		not_affected |= Blind ||
			(u.umonnum == PM_BLACK_LIGHT ||
			 u.umonnum == PM_VIOLET_FUNGUS ||
			 dmgtype(youmonst.data, AD_STUN));
		if (!not_affected) {
		    boolean chg;
		    if (!Hallucination)
			You("VERB_WERDEN von einer Welle kaleidoskopischen Lichtes überwältigt!"); /* EN You("are caught in a blast of kaleidoscopic light!"); */
		    chg = make_hallucinated(HHallucination + (long)tmp,FALSE,0L);
		    You("%s.", chg ? "VERB_FLIPPEN aus" : "seem unaffected"); /* EN You("%s.", chg ? "are freaked out" : "seem unaffected"); */ // TODO DE
		}
		break;

	    default:
		break;
	}
	if (not_affected) {
	    You("seem unaffected by it."); /* EN You("seem unaffected by it."); */ // TODO DE
	    ugolemeffects((int)mattk->adtyp, tmp);
	}
    }
    mondead(mtmp);
    wake_nearto(mtmp->mx, mtmp->my, 7*7);
    if (mtmp->mhp > 0) return(0);
    return(2);	/* it dies */
}

int
gazemu(mtmp, mattk)	/* monster gazes at you */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	switch(mattk->adtyp) {
	    case AD_STON:
		if (mtmp->mcan || !mtmp->mcansee) {
		    if (!canseemon(mtmp)) break;	/* silently */
		    pline("SUBJECT %s %s.", Monnam(mtmp), /* EN pline("%s %s.", Monnam(mtmp), */
			  (mtmp->data == &mons[PM_MEDUSA] && mtmp->mcan) ?
				"VERB_AUSSEHEN gar nicht so übel SATZKLAMMER." : /* EN "doesn't look all that ugly" : */
				"VERB_GAZE ohne Wirkung"); /* EN "gazes ineffectually"); */
		    break;
		}
		if (Reflecting && couldsee(mtmp->mx, mtmp->my) &&
		    mtmp->data == &mons[PM_MEDUSA]) {
		    /* hero has line of sight to Medusa and she's not blind */
		    boolean useeit = canseemon(mtmp);

		    if (useeit)
		    	(void) ureflects("%s is reflected by your %s.", /* EN (void) ureflects("%s gaze is reflected by your %s.", */ // TODO DE
		    			genitivattribut_zu_wort(Monnam(mtmp), "NOUN_GAZE")); /* EN s_suffix(Monnam(mtmp))); */
		    if (mon_reflects(mtmp, !useeit ? (char *)0 :
		    			"The gaze is reflected away by %s!")) /* EN "The gaze is reflected away by %s %s!")) */ // TODO DE
		    	    break;
			if (!m_canseeu(mtmp)) { /* probably you're invisible */
			if (useeit)
			    pline(
		      "SUBJECT %s VERB_SCHEINEN gar nicht zu bemerken, dass %s gaze was reflected.", /* EN "%s doesn't seem to notice that %s gaze was reflected.", */ // TODO DE
				  Monnam(mtmp), mhis(mtmp));
			    break;
			}
		    if (useeit)
			pline("SUBJECT %s VERB_VERSTEINERN!", Monnam(mtmp)); /* EN pline("%s is turned to stone!", Monnam(mtmp)); */
		    stoned = TRUE;
		    killed(mtmp);

		    if (mtmp->mhp > 0) break;
		    return 2;
		}
		if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		    !Stone_resistance) {
		    You("VERB_ERWIDERN OBJECT %s.", genitivattribut_zu_wort(mon_nam(mtmp),"NOUN_BLICK")); /* EN You("meet %s gaze.", s_suffix(mon_nam(mtmp))); */
		    stop_occupation();
		    if(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			break;
		    You("VERB_VERSTEINERN ..."); /* EN You("turn to stone..."); */
		    killer_format = KILLED_BY;
		    killer = mtmp->data->mname;
		    done(STONING);
		}
		break;
	    case AD_CONF:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
		    int conf = d(3,4);

		    mtmp->mspec_used = mtmp->mspec_used + (conf + rn2(6));
		    if(!Confusion)
			pline("SUBJECT %s VERB_VERWIRREN OBJECT PRONOMEN_PERSONAL!", /* EN pline("%s gaze confuses you!", */
			                  genitivattribut_zu_wort(Monnam(mtmp), "NOUN_GAZE")); /* EN s_suffix(Monnam(mtmp))); */
		    else
			You("VERB_WERDEN immer verwirrter."); /* EN You("are getting more and more confused."); */
		    make_confused(HConfusion + conf, FALSE);
		    stop_occupation();
		}
		break;
	    case AD_STUN:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
		    int stun = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (stun + rn2(6));
		    pline("SUBJECT %s VERB_STARREN OBJECT PRONOMEN_PERSONAL durchdringend an!", Monnam(mtmp)); /* EN pline("%s stares piercingly at you!", Monnam(mtmp)); */
		    make_stunned(HStun + stun, TRUE);
		    stop_occupation();
		}
		break;
	    case AD_BLND:
		if (!mtmp->mcan && canseemon(mtmp) && !resists_blnd(&youmonst)
			&& distu(mtmp->mx,mtmp->my) <= BOLT_LIM*BOLT_LIM) {
		    int blnd = d((int)mattk->damn, (int)mattk->damd);

		    You("VERB_WERDEN OBJECT KASUS_DATIV von %s geblendet!", /* EN You("are blinded by %s radiance!", */
			              genitivattribut_zu_wort(mon_nam(mtmp),"NOUN_RADIANCE")); /* EN s_suffix(mon_nam(mtmp))); */
		    make_blinded((long)blnd,FALSE);
		    stop_occupation();
		    /* not blind at this point implies you're wearing
		       the Eyes of the Overworld; make them block this
		       particular stun attack too */
		    if (!Blind) Your(vision_clears);
		    else make_stunned((long)d(1,3),TRUE);
		}
		break;
	    case AD_FIRE:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
		    int dmg = d(2,6);

		    pline("SUBJECT %s VERB_ANGREIFEN OBJECT PRONOMEN_PERSONAL mit einem feurigen Blick an!", Monnam(mtmp)); /* EN pline("%s attacks you with a fiery gaze!", Monnam(mtmp)); */
		    stop_occupation();
		    if (Fire_resistance) {
			pline_The("NOUN_FEUER VERB_FUEHLEN sich nicht heiss an!"); /* EN pline_The("fire doesn't feel hot!"); */
			dmg = 0;
		    }
		    burn_away_slime();
		    if ((int) mtmp->m_lev > rn2(20))
			destroy_item(SCROLL_CLASS, AD_FIRE);
		    if ((int) mtmp->m_lev > rn2(20))
			destroy_item(POTION_CLASS, AD_FIRE);
		    if ((int) mtmp->m_lev > rn2(25))
			destroy_item(SPBOOK_CLASS, AD_FIRE);
		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;
#ifdef PM_BEHOLDER /* work in progress */
	    case AD_SLEE:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee &&
		   multi >= 0 && !rn2(5) && !Sleep_resistance) {

		    fall_asleep(-rnd(10), TRUE);
		    pline("SUBJECT %s VERB_MACHEN OBJECT PRONOMEN_PERSONAL ganz schläfrig ...", /* EN pline("%s gaze makes you very sleepy...", */
			  genitivattribut_zu_wort(Monnam(mtmp), "NOUN_GAZE")); /* EN s_suffix(Monnam(mtmp))); */
		}
		break;
	    case AD_SLOW:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee &&
		   (HFast & (INTRINSIC|TIMEOUT)) &&
		   !defends(AD_SLOW, uwep) && !rn2(4))

		    u_slow_down();
		    stop_occupation();
		break;
#endif
	    default: impossible("Gaze attack %d?", mattk->adtyp);
		break;
	}
	return(0);
}

#endif /* OVLB */
#ifdef OVL1

void
mdamageu(mtmp, n)	/* mtmp hits you for n points damage */
register struct monst *mtmp;
register int n;
{
	flags.botl = 1;
	if (Upolyd) {
		u.mh -= n;
		if (u.mh < 1) rehumanize();
	} else {
		u.uhp -= n;
		if(u.uhp < 1) done_in_by(mtmp);
	}
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL void
urustm(mon, obj)
register struct monst *mon;
register struct obj *obj;
{
	boolean vis;
	boolean is_acid;

	if (!mon || !obj) return; /* just in case */
	if (dmgtype(youmonst.data, AD_CORR))
	    is_acid = TRUE;
	else if (dmgtype(youmonst.data, AD_RUST))
	    is_acid = FALSE;
	else
	    return;

	vis = cansee(mon->mx, mon->my);

	if ((is_acid ? is_corrodeable(obj) : is_rustprone(obj)) &&
	    (is_acid ? obj->oeroded2 : obj->oeroded) < MAX_ERODE) {
		if (obj->greased || obj->oerodeproof || (obj->blessed && rn2(3))) {
		    if (vis)
			pline("Somehow, %s is not affected.", /* EN pline("Somehow, %s weapon is not affected.", */ // TODO DE
						genitivattribut_zu_wort(mon_nam(mon), "NOUN_WEAPON")); /* EN s_suffix(mon_nam(mon))); */
		    if (obj->greased && !rn2(2)) obj->greased = 0;
		} else {
		    if (vis)
			pline("SUBJECT %s %s%s!", /* EN pline("%s %s%s!", */
			        genitivattribut_zu_wort(Monnam(mon), /* EN s_suffix(Monnam(mon)), */
				aobjnam(obj, (char *)0)), (is_acid ? "VERB_CORRODE" : "VERB_ROSTEN"), /* EN aobjnam(obj, (is_acid ? "corrode" : "rust")), */
			        (is_acid ? obj->oeroded2 : obj->oeroded)
				    ? " weiter" : ""); /* EN ? " further" : ""); */
		    if (is_acid) obj->oeroded2++;
		    else obj->oeroded++;
		}
	}
}

#endif /* OVLB */
#ifdef OVL1

int
could_seduce(magr,mdef,mattk)
struct monst *magr, *mdef;
struct attack *mattk;
/* returns 0 if seduction impossible,
 *	   1 if fine,
 *	   2 if wrong gender for nymph */
{
	register struct permonst *pagr;
	boolean agrinvis, defperc;
	xchar genagr, gendef;

	if (is_animal(magr->data)) return (0);
	if(magr == &youmonst) {
		pagr = youmonst.data;
		agrinvis = (Invis != 0);
		genagr = poly_gender();
	} else {
		pagr = magr->data;
		agrinvis = magr->minvis;
		genagr = gender(magr);
	}
	if(mdef == &youmonst) {
		defperc = (See_invisible != 0);
		gendef = poly_gender();
	} else {
		defperc = perceives(mdef->data);
		gendef = gender(mdef);
	}

	if(agrinvis && !defperc
#ifdef SEDUCE
		&& mattk && mattk->adtyp != AD_SSEX
#endif
		)
		return 0;

	if(pagr->mlet != S_NYMPH
		&& ((pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS])
#ifdef SEDUCE
		    || (mattk && mattk->adtyp != AD_SSEX)
#endif
		   ))
		return 0;
	
	if(genagr == 1 - gendef)
		return 1;
	else
		return (pagr->mlet == S_NYMPH) ? 2 : 0;
}

#endif /* OVL1 */
#ifdef OVLB

#ifdef SEDUCE
/* Returns 1 if monster teleported */
int
doseduce(mon)
register struct monst *mon;
{
	register struct obj *ring, *nring;
	boolean fem = (mon->data == &mons[PM_SUCCUBUS]); /* otherwise incubus */
	char qbuf[QBUFSZ];

	if (mon->mcan || mon->mspec_used) {
		pline("SUBJECT %s tut so, als habe %s %sMigräne.", /* EN pline("%s acts as though %s has got a %sheadache.", */
		      Monnam(mon), mhe(mon),
		      mon->mcan ? "starke " : ""); /* EN mon->mcan ? "severe " : ""); */
		return 0;
	}

	if (unconscious()) {
		pline("SUBJECT %s VERB_WIRKEN bestürzt OBJECT über PRONOMEN_POSSESSIV ADJEKTIV_VERMINDERT NOUN_ANSPRECHBARKEIT.", /* EN pline("%s seems dismayed at your lack of response.", */
		      Monnam(mon));
		return 0;
	}

	if (Blind) pline("Jemand streichelt KASUS_AKKUSATIV PRONOMEN_PERSONAL ..."); /* EN if (Blind) pline("It caresses you..."); */
	else Du_fuehlst_dich("sehr stark OBJECT KASUS_DATIV zu %s hingezogen.", mon_nam(mon)); /* EN else You_feel("very attracted to %s.", mon_nam(mon)); */

	for(ring = invent; ring; ring = nring) {
	    nring = ring->nobj;
	    if (ring->otyp != RIN_ADORNMENT) continue;
	    if (fem) {
		if (rn2(20) < ACURR(A_CHA)) {
		    Sprintf(qbuf, "SUBJECT \"PRONOMEN_DIESER %s sieht hübsch aus.  Darf ich ihn haben?\"", /* EN Sprintf(qbuf, "\"That %s looks pretty.  May I have it?\"", */
			safe_qbuf("",sizeof("\"Dieser  sieht hübsch aus.  Darf ich ihn haben?\""), /* EN safe_qbuf("",sizeof("\"That  looks pretty.  May I have it?\""), */
			xname(ring), simple_typename(ring->otyp), "NOUN_RING")); /* EN xname(ring), simple_typename(ring->otyp), "ring")); */
		    makeknown(RIN_ADORNMENT);
		    if (yn(qbuf) == 'n') continue;
		} else pline("SUBJECT %s entscheidet, dass ihr KASUS_NOMINATIV PRONOMEN_POSSESSIV %s gefällt und nimmt ihn.", /* EN } else pline("%s decides she'd like your %s, and takes it.", */
			Blind ? "Sie" : Monnam(mon), xname(ring)); /* EN Blind ? "She" : Monnam(mon), xname(ring)); */
		makeknown(RIN_ADORNMENT);
		if (ring==uleft || ring==uright) Ring_gone(ring);
		if (ring==uwep) setuwep((struct obj *)0);
		if (ring==uswapwep) setuswapwep((struct obj *)0);
		if (ring==uquiver) setuqwep((struct obj *)0);
		freeinv(ring);
		(void) mpickobj(mon,ring);
	    } else {
		char buf[BUFSZ];

		if (uleft && uright && uleft->otyp == RIN_ADORNMENT
				&& uright->otyp==RIN_ADORNMENT)
			break;
		if (ring==uleft || ring==uright) continue;
		if (rn2(20) < ACURR(A_CHA)) {
		    Sprintf(qbuf,"\"SUBJECT PRONOMEN_DIESER %s VERB_AUSSEHEN hübsch SATZKLAMMER.  NEUER_SATZ SUBJECT MODIFIER_KONJUNKTIV_II PRONOMEN_PERSONAL ihn für mich tragen?\"", /* EN Sprintf(qbuf,"\"That %s looks pretty.  Would you wear it for me?\"", */
			safe_qbuf("",
			    sizeof("\"SUBJECT PRONOMEN_DIESER  VERB_AUSSEHEN hübsch SATZKLAMMER.  NEUER_SATZ SUBJECT MODIFIER_KONJUNKTIV_II PRONOMEN_PERSONAL ihn für mich tragen?\""), /* EN sizeof("\"That  looks pretty.  Would you wear it for me?\""), */
			    xname(ring), simple_typename(ring->otyp), "NOUN_RING")); /* EN xname(ring), simple_typename(ring->otyp), "ring")); */
		    makeknown(RIN_ADORNMENT);
		    if (yn(qbuf) == 'n') continue;
		} else {
		    pline("SUBJECT %s meint, NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL MODIFIER_KONJUNKTIV_II VERB_WERDEN hübscher OBJECT KASUS_DATIV mit %s %s aussehen,", /* EN pline("%s decides you'd look prettier wearing your %s,", */
			Blind ? "NOUN_PRONOMEN_3P_M_PERSONAL" : Monnam(mon), shk_your(buf, ring), xname(ring)); /* EN Blind ? "He" : Monnam(mon), xname(ring)); */
		    pline("und steckt ihn KASUS_DATIV PRONOMEN_PERSONAL an den Finger."); /* EN pline("and puts it on your finger."); */
		}
		makeknown(RIN_ADORNMENT);
		if (!uright) {
		    pline("SUBJECT %s VERB_STECKEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL NEUES_OBJECT OBJECT %s NEUES_OBJECT OBJECT an PRONOMEN_POSSESSIV ADJEKTIV_RECHT %s.", /* EN pline("%s puts %s on your right %s.", */
			Blind ? "NOUN_PRONOMEN_3P_M_PERSONAL" : Monnam(mon), the(xname(ring)), body_part(HAND)); /* EN Blind ? "He" : Monnam(mon), the(xname(ring)), body_part(HAND)); */
		    setworn(ring, RIGHT_RING);
		} else if (!uleft) {
		    pline("SUBJECT %s VERB_STECKEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL NEUES_OBJECT OBJECT %s NEUES_OBJECT OBJECT an PRONOMEN_POSSESSIV ADJEKTIV_LINK %s.", /* EN pline("%s puts %s on your left %s.", */
			Blind ? "NOUN_PRONOMEN_3P_M_PERSONAL" : Monnam(mon), the(xname(ring)), body_part(HAND)); /* EN Blind ? "He" : Monnam(mon), the(xname(ring)), body_part(HAND)); */
		    setworn(ring, LEFT_RING);
		} else if (uright && uright->otyp != RIN_ADORNMENT) {
		    Strcpy(buf, xname(uright));
		    pline("SUBJECT %s VERB_ERSETZEN OBJECT PRONOMEN_POSSESSIV %s NEUES_OBJECT OBJECT KASUS_DATIV mit PRONOMEN_POSSESSIV %s.", /* EN pline("%s replaces your %s with your %s.", */
			Blind ? "NOUN_PRONOMEN_3P_M_PERSONAL" : Monnam(mon), buf, xname(ring)); /* EN Blind ? "He" : Monnam(mon), buf, xname(ring)); */
		    Ring_gone(uright);
		    setworn(ring, RIGHT_RING);
		} else if (uleft && uleft->otyp != RIN_ADORNMENT) {
		    Strcpy(buf, xname(uleft));
		    pline("SUBJECT %s VERB_ERSETZEN OBJECT PRONOMEN_POSSESSIV %s NEUES_OBJECT OBJECT KASUS_DATIV mit PRONOMEN_POSSESSIV %s.", /* EN pline("%s replaces your %s with your %s.", */
			Blind ? "NOUN_PRONOMEN_3P_M_PERSONAL" : Monnam(mon), buf, xname(ring)); /* EN Blind ? "He" : Monnam(mon), buf, xname(ring)); */
		    Ring_gone(uleft);
		    setworn(ring, LEFT_RING);
		} else impossible("ring replacement");
		Ring_on(ring);
		prinv((char *)0, ring, 0L);
	    }
	}

	if (!uarmc && !uarmf && !uarmg && !uarms && !uarmh
#ifdef TOURIST
								&& !uarmu
#endif
									)
		pline("SUBJECT %s VERB_FLUESTERN süße Nichtigkeiten OBJECT in PRONOMEN_POSSESSIV NOUN_OHR.", /* EN pline("%s murmurs sweet nothings into your ear.", */
			Blind ? (fem ? "NOUN_PRONOMEN_3P_F_PERSONAL" : "NOUN_PRONOMEN_3P_M_PERSONAL") : Monnam(mon)); /* EN Blind ? (fem ? "She" : "He") : Monnam(mon)); */
	else
		pline("SUBJECT %s VERB_FLUESTERN OBJECT in PRONOMEN_POSSESSIV NOUN_OHR, NEUER_SATZ während SUBJECT_IM_SATZ %s OBJECT KASUS_DATIV PRONOMEN_PERSONAL beim Ausziehen VERB_HELFEN.", /* EN pline("%s murmurs in your ear, while helping you undress.", */
			Blind ? (fem ? "NOUN_PRONOMEN_3P_F_PERSONAL" : "NOUN_PRONOMEN_3P_M_PERSONAL") : Monnam(mon), Blind ? (fem ? "NOUN_PRONOMEN_3P_F_PERSONAL" : "NOUN_PRONOMEN_3P_M_PERSONAL") : Monnam(mon)); /* EN Blind ? (fem ? "She" : "He") : Monnam(mon)); */
	mayberem(uarmc, cloak_simple_name(uarmc));
	if(!uarmc)
		mayberem(uarm, "NOUN_SUIT"); /* EN mayberem(uarm, "suit"); */
	mayberem(uarmf, "NOUN_BOOTSs"); /* EN mayberem(uarmf, "boots"); */
	if(!uwep || !welded(uwep))
		mayberem(uarmg, "NOUN_GLOVESs"); /* EN mayberem(uarmg, "gloves"); */
	mayberem(uarms, "NOUN_SHIELD"); /* EN mayberem(uarms, "shield"); */
	mayberem(uarmh, "NOUN_HELMET"); /* EN mayberem(uarmh, "helmet"); */
#ifdef TOURIST
	if(!uarmc && !uarm)
		mayberem(uarmu, "NOUN_SHIRT"); /* EN mayberem(uarmu, "shirt"); */
#endif

	if (uarm || uarmc) {
		verbalize("Du bist so ein %s. Würdest du doch nur ...", /* EN verbalize("You're such a %s; I wish...", */
				flags.female ? "süßes Ding" : "süßer Bengel"); /* EN flags.female ? "sweet lady" : "nice guy"); */
		if (!tele_restrict(mon)) (void) rloc(mon, FALSE);
		return 1;
	}
	if (u.ualign.type == A_CHAOTIC)
		adjalign(1);

	/* by this point you have discovered mon's identity, blind or not... */
	pline("Die Zeit steht still, während SUBJECT_IM_SATZ PRONOMEN_PERSONAL und %s euch in den Armen liegen ...", /* EN pline("Time stands still while you and %s lie in each other's arms...", */
		noit_mon_nam(mon));
	if (rn2(35) > ACURR(A_CHA) + ACURR(A_INT)) {
		/* Don't bother with mspec_used here... it didn't get tired! */
		pline("SUBJECT %s VERB_SCHEINEN es mehr genossen zu haben als OBJECT KASUS_NOMINATIV PRONOMEN_PERSONAL ...", /* EN pline("%s seems to have enjoyed it more than you...", */
			noit_Monnam(mon));
		switch (rn2(5)) {
			case 0: Du_fuehlst_dich("energielos."); /* EN case 0: You_feel("drained of energy."); */
				u.uen = 0;
				u.uenmax -= rnd(Half_physical_damage ? 5 : 10);
			        exercise(A_CON, FALSE);
				if (u.uenmax < 0) u.uenmax = 0;
				break;
			case 1: You("VERB_SEIN deprimiert."); /* EN case 1: You("are down in the dumps."); */
				(void) adjattrib(A_CON, -1, TRUE);
			        exercise(A_CON, FALSE);
				flags.botl = 1;
				break;
			case 2: Your("NOUN_SINNs fühlen sich abgestumpft an."); /* EN case 2: Your("senses are dulled."); */
				(void) adjattrib(A_WIS, -1, TRUE);
			        exercise(A_WIS, FALSE);
				flags.botl = 1;
				break;
			case 3:
				if (!resists_drli(&youmonst)) {
				    Du_fuehlst_dich("nicht in Form."); /* EN You_feel("out of shape."); */
				    losexp("NOUN_UEBERANSTRENGUNG"); /* EN losexp("overexertion"); */
				} else {
				    You("VERB_HABEN ein seltsames Gefühl ..."); /* EN You("have a curious feeling..."); */
				}
				break;
			case 4: {
				int tmp;
				Du_fuehlst_dich("erschöpft."); /* EN You_feel("exhausted."); */
			        exercise(A_STR, FALSE);
				tmp = rn1(10, 6);
				if(Half_physical_damage) tmp = (tmp+1) / 2;
				losehp(tmp, "NOUN_ERSCHOEPFUNG", KILLED_BY); /* EN losehp(tmp, "exhaustion", KILLED_BY); */
				break;
			}
		}
	} else {
		mon->mspec_used = rnd(100); /* monster is worn out */
		You("VERB_SCHEINEN es mehr genossen zu haben als OBJECT KASUS_NOMINATIV %s ...", /* EN You("seem to have enjoyed it more than %s...", */
		    noit_mon_nam(mon));
		switch (rn2(5)) {
		case 0: You_feel("voll aufgeladen."); /* EN case 0: You_feel("raised to your full potential."); */
			exercise(A_CON, TRUE);
			u.uen = (u.uenmax += rnd(5));
			break;
		case 1: Du_fuehlst_dich("gut genug um es nochmal zu tun."); /* EN case 1: You_feel("good enough to do it again."); */
			(void) adjattrib(A_CON, 1, TRUE);
			exercise(A_CON, TRUE);
			flags.botl = 1;
			break;
		case 2: You("VERB_WERDEN OBJECT %s nie vergessen ...", noit_mon_nam(mon)); /* EN case 2: You("will always remember %s...", noit_mon_nam(mon)); */
			(void) adjattrib(A_WIS, 1, TRUE);
			exercise(A_WIS, TRUE);
			flags.botl = 1;
			break;
		case 3: pline("Das war eine sehr lehrreiche Erfahrung."); /* EN case 3: pline("That was a very educational experience."); */
			pluslvl(FALSE);
			exercise(A_WIS, TRUE);
			break;
		case 4: Du_fuehlst_dich("vollständig gesundet!"); /* EN case 4: You_feel("restored to health!"); */
			u.uhp = u.uhpmax;
			if (Upolyd) u.mh = u.mhmax;
			exercise(A_STR, TRUE);
			flags.botl = 1;
			break;
		}
	}

	if (mon->mtame) /* don't charge */ ;
	else if (rn2(20) < ACURR(A_CHA)) {
		pline("SUBJECT %s verlangt Bezahlung, aber NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL VERB_WEIGERN OBJECT PRONOMEN_PERSONAL ...", /* EN pline("%s demands that you pay %s, but you refuse...", */
					noit_Monnam(mon));
					//Blind ? (fem ? "her" : "him") : mhim(mon)); /* EN Blind ? (fem ? "her" : "him") : mhim(mon)); */
	} else if (u.umonnum == PM_LEPRECHAUN)
		pline("SUBJECT %s VERB_VERSUCHEN OBJECT PRONOMEN_POSSESSIV NOUN_GELD zu klauen, hat aber keine Erfolg ...", /* EN pline("%s tries to take your money, but fails...", */
				noit_Monnam(mon));
	else {
#ifndef GOLDOBJ
		long cost;

		if (u.ugold > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)u.ugold + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > u.ugold) cost = u.ugold;
		if (!cost) verbalize("Die Runde geht aufs Haus!"); /* EN if (!cost) verbalize("It's on the house!"); */
		else {
		    pline("SUBJECT %s VERB_NEHMEN OBJECT %ld %s für geleistete Dienste!", /* EN pline("%s takes %ld %s for services rendered!", */
			    noit_Monnam(mon), cost, currency(cost));
		    u.ugold -= cost;
		    mon->mgold += cost;
		    flags.botl = 1;
		}
#else
		long cost;
                long umoney = money_cnt(invent);

		if (umoney > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)umoney + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > umoney) cost = umoney;
		if (!cost) verbalize("Das geht aufs Haus!"); /* EN if (!cost) verbalize("It's on the house!"); */
		else { 
		    pline("SUBJECT %s VERB_NEHMEN OBJECT %ld %s für geleistete Dienste!", /* EN pline("%s takes %ld %s for services rendered!", */
			    noit_Monnam(mon), cost, currency(cost));
                    money2mon(mon, cost);
		    flags.botl = 1;
		}
#endif
	}
	if (!rn2(25)) mon->mcan = 1; /* monster is worn out */
	if (!tele_restrict(mon)) (void) rloc(mon, FALSE);
	return 1;
}

STATIC_OVL void
mayberem(obj, str)
register struct obj *obj;
const char *str;
{
	char qbuf[QBUFSZ];

	if (!obj || !obj->owornmask) return;

	if (rn2(20) < ACURR(A_CHA)) {
		Sprintf(qbuf,"\"Soll ich dir KASUS_AKKUSATIV ARTIKEL_BESTIMMTER %s ausziehen, %s?\"", /* EN Sprintf(qbuf,"\"Shall I remove your %s, %s?\"", */
			str,
			(!rn2(2) ? "Liebling" : !rn2(2) ? "Schnucki" : "Schatz")); /* EN (!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart")); */
		if (yn(qbuf) == 'n') return;
	} else {
		char hairbuf[BUFSZ];

		Sprintf(hairbuf, "let me run my fingers through your %s", /* EN Sprintf(hairbuf, "let me run my fingers through your %s", */ // TODO DE
			body_part(HAIR));
		verbalize("Zieh KASUS_AKKUSATIV ARTIKEL_BESTIMMTER %s aus; %s.", str, /* EN verbalize("Take off your %s; %s.", str, */
			(obj == uarm)  ? "rück' ein bisschen näher" : /* EN (obj == uarm)  ? "let's get a little closer" : */
			(obj == uarmc || obj == uarms) ? "das ist nur im Weg" : /* EN (obj == uarmc || obj == uarms) ? "it's in the way" : */
			(obj == uarmf) ? "lass mich deine Füße massieren" : /* EN (obj == uarmf) ? "let me rub your feet" : */
			(obj == uarmg) ? "die sind so klobig" : /* EN (obj == uarmg) ? "they're too clumsy" : */
#ifdef TOURIST
			(obj == uarmu) ? "lass mich dich massieren" : /* EN (obj == uarmu) ? "let me massage you" : */
#endif
			/* obj == uarmh */
			hairbuf);
	}
	remove_worn_item(obj, TRUE);
}
#endif  /* SEDUCE */

#endif /* OVLB */

#ifdef OVL1

STATIC_OVL int
passiveum(olduasmon,mtmp,mattk)
struct permonst *olduasmon;
register struct monst *mtmp;
register struct attack *mattk;
{
	int i, tmp;

	for (i = 0; ; i++) {
	    if (i >= NATTK) return 1;
	    if (olduasmon->mattk[i].aatyp == AT_NONE ||
	    		olduasmon->mattk[i].aatyp == AT_BOOM) break;
	}
	if (olduasmon->mattk[i].damn)
	    tmp = d((int)olduasmon->mattk[i].damn,
				    (int)olduasmon->mattk[i].damd);
	else if(olduasmon->mattk[i].damd)
	    tmp = d((int)olduasmon->mlevel+1, (int)olduasmon->mattk[i].damd);
	else
	    tmp = 0;

	/* These affect the enemy even if you were "killed" (rehumanized) */
	switch(olduasmon->mattk[i].adtyp) {
	    case AD_ACID:
		if (!rn2(2)) {
		    pline("SUBJECT %s VERB_WERDEN OBJECT KASUS_DATIV mit PRONOMEN_POSSESSIV NOUN_ACID vollgespritzt!", Monnam(mtmp)); /* EN pline("%s is splashed by your acid!", Monnam(mtmp)); */
		    if (resists_acid(mtmp)) {
			pline("%s is not affected.", Monnam(mtmp)); /* EN pline("%s is not affected.", Monnam(mtmp)); */ // TODO DE
			tmp = 0;
		    }
		} else tmp = 0;
		if (!rn2(30)) erode_armor(mtmp, TRUE);
		if (!rn2(6)) erode_obj(MON_WEP(mtmp), TRUE, TRUE);
		goto assess_dmg;
	    case AD_STON: /* cockatrice */
	    {
		long protector = attk_protection((int)mattk->aatyp),
		     wornitems = mtmp->misc_worn_check;

		/* wielded weapon gives same protection as gloves here */
		if (MON_WEP(mtmp) != 0) wornitems |= W_ARMG;

		if (!resists_ston(mtmp) && (protector == 0L ||
			(protector != ~0L &&
			    (wornitems & protector) != protector))) {
		    if (poly_when_stoned(mtmp->data)) {
			mon_to_stone(mtmp);
			return (1);
		    }
		    pline("SUBJECT %s VERB_VERSTEINERN!", Monnam(mtmp)); /* EN pline("%s turns to stone!", Monnam(mtmp)); */
		    stoned = 1;
		    xkilled(mtmp, 0);
		    if (mtmp->mhp > 0) return 1;
		    return 2;
		}
		return 1;
	    }
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
	    	if (otmp) {
	    	    (void) drain_item(otmp);
	    	    /* No message */
	    	}
	    	return (1);
	    default:
		break;
	}
	if (!Upolyd) return 1;

	/* These affect the enemy only if you are still a monster */
	if (rn2(3)) switch(youmonst.data->mattk[i].adtyp) {
	    case AD_PHYS:
	    	if (youmonst.data->mattk[i].aatyp == AT_BOOM) {
	    	    You("VERB_EXPLODIEREN!"); /* EN You("explode!"); */
	    	    /* KMH, balance patch -- this is okay with unchanging */
	    	    rehumanize();
	    	    goto assess_dmg;
	    	}
	    	break;
	    case AD_PLYS: /* Floating eye */
		if (tmp > 127) tmp = 127;
		if (u.umonnum == PM_FLOATING_EYE) {
		    if (!rn2(4)) tmp = 127;
		    if (mtmp->mcansee && haseyes(mtmp->data) && rn2(3) &&
				(perceives(mtmp->data) || !Invis)) {
			if (Blind)
			    pline("Als ARTIKEL_UNBESTIMMTER ADJEKTIV_BLIND %s SUBJECT_IM_SATZ VERB_KOENNEN PRONOMEN_PERSONAL OBJECT PRONOMEN_PERSONAL nicht verteidigen.", /* EN pline("As a blind %s, you cannot defend yourself.", */
							youmonst.data->mname);
		        else {
			    if (mon_reflects(mtmp,
					    "Your gaze is reflected by %s.")) /* EN "Your gaze is reflected by %s %s.")) */ // TODO DE
				return 1;
			    pline("SUBJECT %s VERB_ERSTARREN OBJECT durch PRONOMEN_POSSESSIV NOUN_GAZE!", Monnam(mtmp)); /* EN pline("%s is frozen by your gaze!", Monnam(mtmp)); */
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = tmp;
			    return 3;
			}
		    }
		} else { /* gelatinous cube */
		    pline("SUBJECT %s VERB_ERSTARREN OBJECT vor PRONOMEN_PERSONAL.", Monnam(mtmp)); /* EN pline("%s is frozen by you.", Monnam(mtmp)); */
		    mtmp->mcanmove = 0;
		    mtmp->mfrozen = tmp;
		    return 3;
		}
		return 1;
	    case AD_COLD: /* Brown mold or blue jelly */
		if (resists_cold(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("SUBJECT %s VERB_FROESTELN leicht.", Monnam(mtmp)); /* EN pline("%s is mildly chilly.", Monnam(mtmp)); */
		    golemeffects(mtmp, AD_COLD, tmp);
		    tmp = 0;
		    break;
		}
		pline("SATZBEGINN OBJECT KASUS_DATIV MODIFIER_VERB_DRITTE_PERSON %s VERB_SEIN plötzlich sehr kalt!", Monnam(mtmp)); /* EN pline("%s is suddenly very cold!", Monnam(mtmp)); */
		u.mh += tmp / 2;
		if (u.mhmax < u.mh) u.mhmax = u.mh;
		if (u.mhmax > ((youmonst.data->mlevel+1) * 8))
		    (void)split_mon(&youmonst, mtmp);
		break;
	    case AD_STUN: /* Yellow mold */
		if (!mtmp->mstun) {
		    mtmp->mstun = 1;
		    pline("SUBJECT %s %s.", Monnam(mtmp), /* EN pline("%s %s.", Monnam(mtmp), */
			  makeplural(stagger(mtmp->data, "VERB_STAGGER"))); /* EN makeplural(stagger(mtmp->data, "stagger"))); */
		}
		tmp = 0;
		break;
	    case AD_FIRE: /* Red mold */
		if (resists_fire(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("SUBJECT %s VERB_SCHWITZEN leicht.", Monnam(mtmp)); /* EN pline("%s is mildly warm.", Monnam(mtmp)); */
		    golemeffects(mtmp, AD_FIRE, tmp);
		    tmp = 0;
		    break;
		}
		pline("SATZBEGINN OBJECT KASUS_DATIV MODIFIER_VERB_DRITTE_PERSON %s VERB_SEIN plötzlich sehr heiß!", Monnam(mtmp)); /* EN pline("%s is suddenly very hot!", Monnam(mtmp)); */
		break;
	    case AD_ELEC:
		if (resists_elec(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("SUBJECT %s VERB_STEHEN etwas unter Strom.", Monnam(mtmp)); /* EN pline("%s is slightly tingled.", Monnam(mtmp)); */
		    golemeffects(mtmp, AD_ELEC, tmp);
		    tmp = 0;
		    break;
		}
		pline("SUBJECT %s VERB_BEKOMMEN OBJECT KASUS_DATIV von PRONOMEN_PERSONAL einen Stromschlag!", Monnam(mtmp)); /* EN pline("%s is jolted with your electricity!", Monnam(mtmp)); */
		break;
	    default: tmp = 0;
		break;
	}
	else tmp = 0;

    assess_dmg:
	if((mtmp->mhp -= tmp) <= 0) {
		pline("SUBJECT %s VERB_STERBEN!", Monnam(mtmp)); /* EN pline("%s dies!", Monnam(mtmp)); */
		xkilled(mtmp,0);
		if (mtmp->mhp > 0) return 1;
		return 2;
	}
	return 1;
}

#endif /* OVL1 */
#ifdef OVLB

#include "edog.h"
struct monst *
cloneu()
{
	register struct monst *mon;
	int mndx = monsndx(youmonst.data);

	if (u.mh <= 1) return(struct monst *)0;
	if (mvitals[mndx].mvflags & G_EXTINCT) return(struct monst *)0;
	mon = makemon(youmonst.data, u.ux, u.uy, NO_MINVENT|MM_EDOG);
	mon = christen_monst(mon, plname);
	initedog(mon);
	mon->m_lev = youmonst.data->mlevel;
	mon->mhpmax = u.mhmax;
	mon->mhp = u.mh / 2;
	u.mh -= mon->mhp;
	flags.botl = 1;
	return(mon);
}

#endif /* OVLB */

/*mhitu.c*/
