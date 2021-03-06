/*	SCCS Id: @(#)sounds.c	3.4	2002/05/06	*/
/*	Copyright (c) 1989 Janet Walz, Mike Threepoint */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h"
#ifdef USER_SOUNDS
# ifdef USER_SOUNDS_REGEX
#include <regex.h>
# endif
#endif

#ifdef OVLB

static int FDECL(domonnoise,(struct monst *));
static int NDECL(dochat);

#endif /* OVLB */

#ifdef OVL0

static int FDECL(mon_in_room, (struct monst *,int));

/* this easily could be a macro, but it might overtax dumb compilers */
static int
mon_in_room(mon, rmtyp)
struct monst *mon;
int rmtyp;
{
    int rno = levl[mon->mx][mon->my].roomno;

    return rooms[rno - ROOMOFFSET].rtype == rmtyp;
}

void
dosounds()
{
    register struct mkroom *sroom;
    register int hallu, vx, vy;
#if defined(AMIGA) && defined(AZTEC_C_WORKAROUND)
    int xx;
#endif
    struct monst *mtmp;

    if (!flags.soundok || u.uswallow || Underwater) return;

    hallu = Hallucination ? 1 : 0;

    if (level.flags.nfountains && !rn2(400)) {
	static const char * const fountain_msg[4] = {
		"pl�tscherndes Wasser.", /* EN "bubbling water.", */
		"Wasser auf M�nzen prasseln.", /* EN "water falling on coins.", */
		"eine Najade im Wasser planschen.", /* EN "the splashing of a naiad.", */
		"einen Luftbefeuchter!", /* EN "a soda fountain!", */
	};
	You_hear(fountain_msg[rn2(3)+hallu]);
    }
#ifdef SINK
    if (level.flags.nsinks && !rn2(300)) {
	static const char * const sink_msg[3] = {
		"ein stetes Tropfen.", /* EN "a slow drip.", */
		"ein gurgelndes Ger�usch.", /* EN "a gurgling noise.", */
		"einen Geschirrsp�ler!", /* EN "dishes being washed!", */
	};
	You_hear(sink_msg[rn2(2)+hallu]);
    }
#endif
    if (level.flags.has_court && !rn2(200)) {
	static const char * const throne_msg[4] = {
		"das Ger�usch h�fischer Konversation.", /* EN "the tones of courtly conversation.", */
		"das urteilsverk�ndende Pochen eines Szepters.", /* EN "a sceptre pounded in judgment.", */
		"Jemand ruft \"Kopf ab!\"", /* EN "Someone shouts \"Off with %s head!\"", */
		"K�nigin Ber�thiels Katzen!", /* EN "Queen Beruthiel's cats!", */
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping ||
			is_lord(mtmp->data) || is_prince(mtmp->data)) &&
		!is_animal(mtmp->data) &&
		mon_in_room(mtmp, COURT)) {
		/* finding one is enough, at least for now */
		int which = rn2(3)+hallu;

		if (which != 2) You_hear(throne_msg[which]);
		else		pline(throne_msg[2], uhis());
		return;
	    }
	}
    }
    if (level.flags.has_swamp && !rn2(200)) {
	static const char * const swamp_msg[3] = {
		"VERB_HEAR Moskitos!", /* EN "hear mosquitoes!",*/
		"VERB_SMELL Sumpfgas!",	/* so it's a smell...*/ /* EN "smell marsh gas!",	*/
		"VERB_HEAR Donald Duck!", /* EN "hear Donald Duck!",*/
	};
	You(swamp_msg[rn2(2)+hallu]);
	return;
    }
    if (level.flags.has_vault && !rn2(200)) {
	if (!(sroom = search_special(VAULT))) {
	    /* strange ... */
	    level.flags.has_vault = 0;
	    return;
	}
	if(gd_sound())
	    switch (rn2(2)+hallu) {
		case 1: {
		    boolean gold_in_vault = FALSE;

		    for (vx = sroom->lx;vx <= sroom->hx; vx++)
			for (vy = sroom->ly; vy <= sroom->hy; vy++)
			    if (g_at(vx, vy))
				gold_in_vault = TRUE;
#if defined(AMIGA) && defined(AZTEC_C_WORKAROUND)
		    /* Bug in aztec assembler here. Workaround below */
		    xx = ROOM_INDEX(sroom) + ROOMOFFSET;
		    xx = (xx != vault_occupied(u.urooms));
		    if(xx)
#else
		    if (vault_occupied(u.urooms) !=
			 (ROOM_INDEX(sroom) + ROOMOFFSET))
#endif /* AZTEC_C_WORKAROUND */
		    {
			if (gold_in_vault)
			    You_hear(!hallu ? "jemanden Geld z�hlen." : /* EN You_hear(!hallu ? "someone counting money." : */
				"jemanden in Geld schwimmen."); /* EN "the quarterback calling the play."); */
			else
			    You_hear("jemanden suchen."); /* EN You_hear("someone searching."); */
			break;
		    }
		    /* fall into... (yes, even for hallucination) */
		}
		case 0:
		    You_hear("die Schritte einer Wache auf Patrouille."); /* EN You_hear("the footsteps of a guard on patrol."); */ 
		    break;
		case 2:
		    You_hear("Mac Moneysac!"); /* EN You_hear("Ebenezer Scrooge!"); */ /* gibt es eine bessere Verk�rperung eines unsympathischen Geldsacks? */
		    break;
	    }
	return;
    }
    if (level.flags.has_beehive && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->data->mlet == S_ANT && is_flyer(mtmp->data)) &&
		mon_in_room(mtmp, BEEHIVE)) {
		switch (rn2(2)+hallu) {
		    case 0:
			You_hear("ein schwaches Summen."); /* EN You_hear("a low buzzing."); */
			break;
		    case 1:
			You_hear("ein w�tendes Brummen."); /* EN You_hear("an angry drone."); */
			break;
		    case 2:
			You_hear("die Biene Maja!"); /* EN You_hear("bees in your %sbonnet!", */
			    /* EN uarmh ? "" : "(nonexistent) "); */
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_morgue && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_undead(mtmp->data) &&
		mon_in_room(mtmp, MORGUE)) {
		switch (rn2(2)+hallu) {
		    case 0:
			You("VERB_MERKEN pl�tzlich, wie widernat�rlich ruhig es ist."); /* EN You("suddenly realize it is unnaturally quiet.");*/
			break;
		    case 1:
			Dir("SUBJECT_IM_SATZ VERB_STEHEN ARTIKEL_BESTIMMTER %s zu Berge.", /* EN pline_The("%s on the back of your %s stands up.", */
				body_part(HAIR)); /* EN body_part(HAIR), body_part(NECK)); */
			break;
		    case 2:
			pline_The("%s OBJECT KASUS_DATIV auf PRONOMEN_POSSESSIV %s VERB_STEHEN NEUES_OBJECT OBJECT KASUS_DATIV PRONOMEN_PERSONAL zu Berge.", /* EN pline_The("%s on your %s seems to stand up.", */
				body_part(HAIR), body_part(HEAD));
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_barracks && !rn2(200)) {
	static const char * const barracks_msg[4] = {
		"das Schleifen von Schwertern.", /* EN "blades being honed.", */
		"lautes Schnarchen.", /* EN "loud snoring.", */ 
		"das Klackern von W�rfeln.", /* EN "dice being thrown.", */
		"Carl von Clausewitz!", /* EN "General MacArthur!", */
	};
	int count = 0;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_mercenary(mtmp->data) &&
#if 0		/* don't bother excluding these */
		!strstri(mtmp->data->mname, "NOUN_WATCH") && /* EN !strstri(mtmp->data->mname, "watch") && */
		!strstri(mtmp->data->mname, "NOUN_GUARD") && /* EN !strstri(mtmp->data->mname, "guard") && */
#endif
		mon_in_room(mtmp, BARRACKS) &&
		/* sleeping implies not-yet-disturbed (usually) */
		(mtmp->msleeping || ++count > 5)) {
		You_hear(barracks_msg[rn2(3)+hallu]);
		return;
	    }
	}
    }
    if (level.flags.has_zoo && !rn2(200)) {
	static const char * const zoo_msg[3] = {
		"ein Ger�usch wie von einem Elefanten, der eine Erdnuss zertritt.", /* EN "a sound reminiscent of an elephant stepping on a peanut.", */
		"ein Ger�usch wie das Heulen eines Seehundes.", /* EN "a sound reminiscent of a seal barking.", */
		"Doktor Dolittle!", /* EN "Doctor Doolittle!", */
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping || is_animal(mtmp->data)) &&
		    mon_in_room(mtmp, ZOO)) {
		You_hear(zoo_msg[rn2(2)+hallu]);
		return;
	    }
	}
    }
    if (level.flags.has_shop && !rn2(200)) {
	if (!(sroom = search_special(ANY_SHOP))) {
	    /* strange... */
	    level.flags.has_shop = 0;
	    return;
	}
	if (tended_shop(sroom) &&
		!index(u.ushops, ROOM_INDEX(sroom) + ROOMOFFSET)) {
	    static const char * const shop_msg[3] = {
		    "jemanden Ladendiebe verfluchen.", /* EN "someone cursing shoplifters.", */
		    "das Klingeln einer Registrierkasse.", /* EN "the chime of a cash register.", */
		    "den Sommerschlussverkauf!", /* EN "Neiman and Marcus arguing!", */
	    };
	    You_hear(shop_msg[rn2(2)+hallu]);
	}
	return;
    }
    if (Is_oracle_level(&u.uz) && !rn2(400)) {
	/* make sure the Oracle is still here */
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if (!DEADMONSTER(mtmp) && mtmp->data == &mons[PM_ORACLE])
		break;
	/* and don't produce silly effects when she's clearly visible */
	if (mtmp && (hallu || !canseemon(mtmp))) {
	    static const char * const ora_msg[5] = {
		    "das Rauschen des Windes.",		/* Jupiter at Dodona */ /* EN "a strange wind.",		*/
		    "ekstatische Weissagungen.",	/* Apollo at Delphi */ /* EN "convulsive ravings.",	 */
		    "schnarchende Schlangen.",		/* AEsculapius at Epidaurus */ /* EN "snoring snakes.",		 */
		    "jemanden sagen: \"Keine Murmeltiere mehr!\"", /* EN "someone say \"No more woodchucks!\"", */
		    "ein lautes ZOT!"		/* both rec.humor.oracle */ /* EN "a loud ZOT!" */
	    };
	    You_hear(ora_msg[rn2(3)+hallu*2]);
	}
	return;
    }
}

#endif /* OVL0 */
#ifdef OVLB

static const char * const h_sounds[] = {
    "VERB_PIEPSEN", "VERB_HUEPFEN", "VERB_SINGEN", "VERB_RUELPSEN", "VERB_KNARREN", "VERB_HUSTEN", "VERB_RASSELN", /* EN "beep", "boing", "sing", "belche", "creak", "cough", "rattle", */
    "VERB_JOHLEN", "VERB_KNALLEN", "VERB_BIMMELN", "VERB_SCHNUEFFELN", "VERB_PINKELN", "VERB_JIEPEN" /* EN "ululate", "pop", "jingle", "sniffle", "tinkle", "eep" */
};

const char *
growl_sound(mtmp)
register struct monst *mtmp;
{
	const char *ret;

	switch (mtmp->data->msound) {
	case MS_MEW:
	case MS_HISS:
	    ret = "VERB_HISS"; /* EN ret = "hiss"; */
	    break;
	case MS_BARK:
	case MS_GROWL:
	    ret = "VERB_GROWL"; /* EN ret = "growl"; */
	    break;
	case MS_ROAR:
	    ret = "VERB_ROAR"; /* EN ret = "roar"; */
	    break;
	case MS_BUZZ:
	    ret = "VERB_BUZZ"; /* EN ret = "buzz"; */
	    break;
	case MS_SQEEK:
	    ret = "VERB_SQUEAL"; /* EN ret = "squeal"; */
	    break;
	case MS_SQAWK:
	    ret = "VERB_SCREECH"; /* EN ret = "screech"; */
	    break;
	case MS_NEIGH:
	    ret = "VERB_NEIGH"; /* EN ret = "neigh"; */
	    break;
	case MS_WAIL:
	    ret = "VERB_WAIL"; /* EN ret = "wail"; */
	    break;
	case MS_SILENT:
		ret = "VERB_ZITTERN"; /* EN ret = "commotion"; */
		break;
	default:
		ret = "VERB_SCREAM"; /* EN ret = "scream"; */
	}
	return ret;
}

/* the sounds of a seriously abused pet, including player attacking it */
void
growl(mtmp)
register struct monst *mtmp;
{
    register const char *growl_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	growl_verb = h_sounds[rn2(SIZE(h_sounds))];
    else
	growl_verb = growl_sound(mtmp);
    if (growl_verb) {
	pline("SUBJECT %s %s!", Monnam(mtmp), vtense((char *)0, growl_verb)); /* EN pline("%s %s!", Monnam(mtmp), vtense((char *)0, growl_verb)); */
	if(flags.run) nomul(0);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 18);
    }
}

/* the sounds of mistreated pets */
void
yelp(mtmp)
register struct monst *mtmp;
{
    register const char *yelp_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	yelp_verb = h_sounds[rn2(SIZE(h_sounds))];
    else switch (mtmp->data->msound) {
	case MS_MEW:
	    yelp_verb = "VERB_YOWL"; /* EN yelp_verb = "yowl"; */
	    break;
	case MS_BARK:
	case MS_GROWL:
	    yelp_verb = "VERB_YELP"; /* EN yelp_verb = "yelp"; */
	    break;
	case MS_ROAR:
	    yelp_verb = "VERB_SNARL die Z�hne"; /* EN yelp_verb = "snarl"; */
	    break;
	case MS_SQEEK:
	    yelp_verb = "VERB_SQUEAL"; /* EN yelp_verb = "squeal"; */
	    break;
	case MS_SQAWK:
	    yelp_verb = "VERB_SCREAK"; /* EN yelp_verb = "screak"; */
	    break;
	case MS_WAIL:
	    yelp_verb = "VERB_WAIL"; /* EN yelp_verb = "wail"; */
	    break;
    }
    if (yelp_verb) {
	pline("SUBJECT %s %s!", Monnam(mtmp), vtense((char *)0, yelp_verb)); /* EN pline("%s %s!", Monnam(mtmp), vtense((char *)0, yelp_verb)); */
	if(flags.run) nomul(0);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 12);
    }
}

/* the sounds of distressed pets */
void
whimper(mtmp)
register struct monst *mtmp;
{
    register const char *whimper_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	whimper_verb = h_sounds[rn2(SIZE(h_sounds))];
    else switch (mtmp->data->msound) {
	case MS_MEW:
	case MS_GROWL:
	    whimper_verb = "VERB_WHIMPER"; /* EN whimper_verb = "whimper"; */
	    break;
	case MS_BARK:
	    whimper_verb = "VERB_WHINE"; /* EN whimper_verb = "whine"; */
	    break;
	case MS_SQEEK:
	    whimper_verb = "VERB_SQUEAL"; /* EN whimper_verb = "squeal"; */
	    break;
    }
    if (whimper_verb) {
	pline("SUBJECT %s %s.", Monnam(mtmp), vtense((char *)0, whimper_verb)); /* EN pline("%s %s.", Monnam(mtmp), vtense((char *)0, whimper_verb)); */
	if(flags.run) nomul(0);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 6);
    }
}

/* pet makes "I'm hungry" noises */
void
beg(mtmp)
register struct monst *mtmp;
{
    if (mtmp->msleeping || !mtmp->mcanmove ||
	    !(carnivorous(mtmp->data) || herbivorous(mtmp->data)))
	return;

    /* presumably nearness and soundok checks have already been made */
    if (!is_silent(mtmp->data) && mtmp->data->msound <= MS_ANIMAL)
	(void) domonnoise(mtmp);
    else if (mtmp->data->msound >= MS_HUMANOID) {
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);
	verbalize("Ich habe Hunger."); /* EN verbalize("I'm hungry."); */
    }
}

static int
domonnoise(mtmp)
register struct monst *mtmp;
{
    register const char *pline_msg = 0,	/* Monnam(mtmp) will be prepended */
			*verbl_msg = 0;	/* verbalize() */
    struct permonst *ptr = mtmp->data;
    char verbuf[BUFSZ];

    /* presumably nearness and sleep checks have already been made */
    if (!flags.soundok) return(0);
    if (is_silent(ptr)) return(0);

    /* Make sure its your role's quest quardian; adjust if not */
    if (ptr->msound == MS_GUARDIAN && ptr != &mons[urole.guardnum]) {
    	int mndx = monsndx(ptr);
    	ptr = &mons[genus(mndx,1)];
    }

    /* be sure to do this before talking; the monster might teleport away, in
     * which case we want to check its pre-teleport position
     */
    if (!canspotmon(mtmp))
	map_invisible(mtmp->mx, mtmp->my);

    switch (ptr->msound) {
	case MS_ORACLE:
	    return doconsult(mtmp);
	case MS_PRIEST:
	    priest_talk(mtmp);
	    break;
	case MS_LEADER:
	case MS_NEMESIS:
	case MS_GUARDIAN:
	    quest_chat(mtmp);
	    break;
	case MS_SELL: /* pitch, pay, total */
	    shk_chat(mtmp);
	    break;
	case MS_VAMPIRE:
	    {
	    /* vampire messages are varied by tameness, peacefulness, and time of night */
		boolean isnight = night();
		boolean kindred =    (Upolyd && (u.umonnum == PM_VAMPIRE ||
				       u.umonnum == PM_VAMPIRE_LORD));
		boolean nightchild = (Upolyd && (u.umonnum == PM_WOLF ||
				       u.umonnum == PM_WINTER_WOLF ||
	    			       u.umonnum == PM_WINTER_WOLF_CUB));
		const char *racenoun = (flags.female && urace.individual.f) ?
					urace.individual.f : (urace.individual.m) ?
					urace.individual.m : urace.noun;

		if (mtmp->mtame) {
			if (kindred) {
				Sprintf(verbuf, "Guten %s, Meister%s", /* EN Sprintf(verbuf, "Good %s to you Master%s", */
					isnight ? "Abend" : "Tag", /* EN isnight ? "evening" : "day", */
					isnight ? "!" : ".  Warum ruhen wir nicht?"); /* EN isnight ? "!" : ".  Why do we not rest?"); */
				verbl_msg = verbuf;
		    	} else {
		    	    Sprintf(verbuf,"SATZBEGINN %s%s", /* EN Sprintf(verbuf,"%s%s", */
				nightchild ? "Gesch�pf der Nacht, " : "", /* EN nightchild ? "Child of the night, " : "", */
				midnight() ?
					"ich kann diesem Verlangen nicht l�nger widerstehen!" : /* EN "I can stand this craving no longer!" : */
				isnight ?
					"ich bitte KASUS_AKKUSATIV PRONOMEN_PERSONAL, NEUER_SATZ MODIFIER_VERB_IMPERATIV VERB_HELFEN mir, dieses wachsende Verlangen zu stillen!" : /* EN "I beg you, help me satisfy this growing craving!" : */
					"ich f�hle mich etwas ermattet."); /* EN "I find myself growing a little weary."); */
				verbl_msg = verbuf;
			}
		} else if (mtmp->mpeaceful) {
			if (kindred && isnight) {
				Sprintf(verbuf, "W�nsche, wohl zu speisen, %s!", /* EN Sprintf(verbuf, "Good feeding %s!", */
						flags.female ? "Schwester" : "Bruder"); /* EN flags.female ? "sister" : "brother"); */
					verbl_msg = verbuf;
				} else if (nightchild && isnight) {
					Sprintf(verbuf,
					    "Wie sch�n KASUS_AKKUSATIV PRONOMEN_POSSESSIV NOUN_STIMME zu h�ren, Gesch�pf der Nacht!"); /* EN "How nice to hear you, child of the night!"); */
					verbl_msg = verbuf;
				} else
					verbl_msg = "Ich trinke nur ... Zaubertr�nke."; /* EN verbl_msg = "I only drink... potions."; */
			} else {
				int vampindex;
				static const char * const vampmsg[] = {
				       /* These first two (0 and 1) are specially handled below */
					"Ich will KASUS_AKKUSATIV PRONOMEN_POSSESSIV %s aussaugen!", /* EN "I vant to suck your %s!", */
					"Ich werde KASUS_AKKUSATIV %s jagen und zur Strecke bringen!", /* EN "I vill come after %s without regret!", */
				       /* other famous vampire quotes can follow here if desired */
				};
				if (kindred)
				    verbl_msg = "Das ist mein Jagdrevier, in dem SUBJECT_IM_SATZ PRONOMEN_PERSONAL OBJECT PRONOMEN_PERSONAL herumzutreiben VERB_WAGEN!"; /* EN verbl_msg = "This is my hunting ground that you dare to prowl!"; */
				else if (youmonst.data == &mons[PM_SILVER_DRAGON] ||
					 youmonst.data == &mons[PM_BABY_SILVER_DRAGON]) {
				    /* Silver dragons are silver in color, not made of silver */
				    Sprintf(verbuf, "%s! SUBJECT PRONOMEN_POSSESSIV Silber-NOUN_GLANZ schreckt mich nicht!", /* EN Sprintf(verbuf, "%s! Your silver sheen does not frighten me!", */
						youmonst.data == &mons[PM_SILVER_DRAGON] ?
						"Esel" : "Junger Esel"); /* EN "Fool" : "Young Fool"); */
			    verbl_msg = verbuf; 
			} else {
			    vampindex = rn2(SIZE(vampmsg));
			    if (vampindex == 0) {
				Sprintf(verbuf, vampmsg[vampindex], body_part(BLOOD));
	    			verbl_msg = verbuf;
			    } else if (vampindex == 1) {
				Sprintf(verbuf, vampmsg[vampindex],
					Upolyd ? an(mons[u.umonnum].mname) : an(racenoun));
	    			verbl_msg = verbuf;
		    	    } else
			    	verbl_msg = vampmsg[vampindex];
			}
	        }
	    }
	    break;
	case MS_WERE:
	    if (flags.moonphase == FULL_MOON && (night() ^ !rn2(13))) {
		pline("SUBJECT %s VERB_WERFEN OBJECT PRONOMEN_POSSESSIV_SUBJECT %s zur�ck und VERB_STOSSEN %s aus!", /* EN pline("%s throws back %s head and lets out a blood curdling %s!", */
		      Monnam(mtmp), mbodypart(mtmp, HEAD), /* EN Monnam(mtmp), mhis(mtmp), */
		      ptr == &mons[PM_HUMAN_WERERAT] ? "einen markersch�tternden Schrei" : "ein markersch�tterndes Heulen"); /* EN ptr == &mons[PM_HUMAN_WERERAT] ? "shriek" : "howl"); */
		wake_nearto(mtmp->mx, mtmp->my, 11*11);
	    } else
		pline_msg =
		     "VERB_ANTWORTEN unverst�ndlich leise.  NEUER_SATZ SUBJECT PRONOMEN_PERSONAL VERB_VERSTEHEN nur \"Mond\"."; /* EN "whispers inaudibly.  All you can make out is \"moon\"."; */
	    break;
	case MS_BARK:
	    if (flags.moonphase == FULL_MOON && night()) {
		pline_msg = "heult."; /* EN pline_msg = "howls."; */
	    } else if (mtmp->mpeaceful) {
		if (mtmp->mtame &&
			(mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
			 moves > EDOG(mtmp)->hungrytime || mtmp->mtame < 5))
		    pline_msg = "VERB_WHINE."; /* EN pline_msg = "whines."; */
		else if (mtmp->mtame && EDOG(mtmp)->hungrytime > moves + 1000)
		    pline_msg = "wimmert."; /* EN pline_msg = "yips."; */
		else {
		    if (mtmp->data != &mons[PM_DINGO])	/* dingos do not actually bark */
			    pline_msg = "bellt."; /* EN pline_msg = "barks."; */
		}
	    } else {
		pline_msg = "knurrt."; /* EN pline_msg = "growls."; */
	    }
	    break;
	case MS_MEW:
	    if (mtmp->mtame) {
		if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
			mtmp->mtame < 5)
		    pline_msg = "jault."; /* EN pline_msg = "yowls."; */
		else if (moves > EDOG(mtmp)->hungrytime)
		    pline_msg = "miaut."; /* EN pline_msg = "meows."; */
		else if (EDOG(mtmp)->hungrytime > moves + 1000)
		    pline_msg = "schnurrt."; /* EN pline_msg = "purrs."; */
		else
		    pline_msg = "maunzt."; /* EN pline_msg = "mews."; */
		break;
	    } /* else FALLTHRU */
	case MS_GROWL:
	    pline_msg = mtmp->mpeaceful ? "VERB_SNARL die Z�hne." : "VERB_GROWL!"; /* EN pline_msg = mtmp->mpeaceful ? "snarls." : "growls!"; */
	    break;
	case MS_ROAR:
	    pline_msg = mtmp->mpeaceful ? "VERB_SNARL die Z�hne." : "VERB_ROAR!"; /* EN pline_msg = mtmp->mpeaceful ? "snarls." : "roars!"; */
	    break;
	case MS_SQEEK:
	    pline_msg = "quiekt."; /* EN pline_msg = "squeaks."; */
	    break;
	case MS_SQAWK:
	    if (ptr == &mons[PM_RAVEN] && !mtmp->mpeaceful)
	    	verbl_msg = "Nimmermehr!"; /* EN verbl_msg = "Nevermore!"; */
	    else
	    	pline_msg = "kr�chzt."; /* EN pline_msg = "squawks."; */
	    break;
	case MS_HISS:
	    if (!mtmp->mpeaceful)
		pline_msg = "VERB_HISS!"; /* EN pline_msg = "hisses!"; */
	    else return 0;	/* no sound */
	    break;
	case MS_BUZZ:
	    pline_msg = mtmp->mpeaceful ? "summt." : "brummt w�tend."; /* EN pline_msg = mtmp->mpeaceful ? "drones." : "buzzes angrily."; */
	    break;
#ifdef GERMAN
	case MS_GNOME:
		  if (In_mines(&u.uz) && (!rn2(20))) {
					verbl_msg = "H�sch mer amol en Stutz?"; /* Gnome of Zurich */
					break;
			}
	    /* fall through */
#endif
	case MS_GRUNT:
	    pline_msg = "grunzt."; /* EN pline_msg = "grunts."; */
	    break;
	case MS_NEIGH:
	    if (mtmp->mtame < 5)
		pline_msg = "schnaubt."; /* EN pline_msg = "neighs."; */ /* http://www.pferdewissen.ch/stimme.html */
	    else if (moves > EDOG(mtmp)->hungrytime)
		pline_msg = "grummelt."; /* EN pline_msg = "whinnies."; */
	    else
		pline_msg = "wiehert."; /* EN pline_msg = "whickers."; */
	    break;
	case MS_WAIL:
	    pline_msg = "klagt traurig."; /* EN pline_msg = "wails mournfully."; */
	    break;
	case MS_GURGLE:
	    pline_msg = "gluckst."; /* EN pline_msg = "gurgles."; */
	    break;
	case MS_BURBLE:
	    pline_msg = "burbbelt."; /* EN pline_msg = "burbles."; */
	    break;
	case MS_SHRIEK:
	    pline_msg = "VERB_SHRIEK."; /* EN pline_msg = "shrieks."; */
	    aggravate();
	    break;
	case MS_IMITATE:
	    pline_msg = "VERB_IMITIEREN OBJECT PRONOMEN_PERSONAL."; /* EN pline_msg = "imitates you."; */
	    break;
	case MS_BONES:
	    pline("%s VERB_KLAPPERN laut.", Monnam(mtmp)); /* EN pline("%s rattles noisily.", Monnam(mtmp)); */
	    You("VERB_ERSTARREN f�r einen Augenblick."); /* EN You("freeze for a moment."); */
	    nomul(-2);
	    break;
	case MS_LAUGH:
	    {
		static const char * const laugh_msg[4] = {
		    "VERB_GIGGELN.", "VERB_GLUCKSEN.", "VERB_KICHERN.", "VERB_LACHEN.", /* EN "giggles.", "chuckles.", "snickers.", "laughs.", */
		};
		pline_msg = laugh_msg[rn2(4)];
	    }
	    break;
	case MS_MUMBLE:
	    pline_msg = "murmelt unverst�ndlich."; /* EN pline_msg = "mumbles incomprehensibly."; */
	    break;
	case MS_DJINNI:
	    if (mtmp->mtame) {
		verbl_msg = "Sorry, hab' keine W�nsche mehr �brig."; /* EN verbl_msg = "Sorry, I'm all out of wishes."; */
	    } else if (mtmp->mpeaceful) {
		if (ptr == &mons[PM_WATER_DEMON])
		    pline_msg = "gluckst."; /* EN pline_msg = "gurgles."; */
		else
		    verbl_msg = "Ich bin frei!"; /* EN verbl_msg = "I'm free!"; */
	    } else verbl_msg = "Das wird OBJECT PRONOMEN_PERSONAL lehren, mich zu st�ren!"; /* EN } else verbl_msg = "This will teach you not to disturb me!"; */
	    break;
	case MS_BOAST:	/* giants */
	    if (!mtmp->mpeaceful) {
		switch (rn2(4)) {
		case 0: pline("SUBJECT %s VERB_PRAHLEN mit OBJECT KASUS_DATIV %s NOUN_GEM_COLLECTION.", /* EN case 0: pline("%s boasts about %s gem collection.", */
			      Monnam(mtmp), mhis(mtmp));
			break;
		case 1: pline_msg = "beklagt sich �ber eine Lamm-Di�t."; /* EN case 1: pline_msg = "complains about a diet of mutton."; */
			break;
	       default: pline_msg = "br�llt \"Fee Fie Foe Foo!\" und lacht schallend."; /* EN default: pline_msg = "shouts \"Fee Fie Foe Foo!\" and guffaws."; */
			wake_nearto(mtmp->mx, mtmp->my, 7*7);
			break;
		}
		break;
	    }
	    /* else FALLTHRU */
	case MS_HUMANOID:
	    if (!mtmp->mpeaceful) {
		if (In_endgame(&u.uz) && is_mplayer(ptr)) {
		    mplayer_talk(mtmp);
		    break;
		} else return 0;	/* no sound */
	    }
	    /* Generic peaceful humanoid behaviour. */
	    if (mtmp->mflee)
		pline_msg = "VERB_WOLLEN nichts mit KASUS_DATIV PRONOMEN_PERSONAL zu tun haben."; /* EN pline_msg = "wants nothing to do with you."; */
	    else if (mtmp->mhp < mtmp->mhpmax/4)
		pline_msg = "st�hnt."; /* EN pline_msg = "moans."; */
	    else if (mtmp->mconf || mtmp->mstun)
		verbl_msg = !rn2(3) ? "H�h?" : rn2(2) ? "Was?" : "�h?"; /* EN verbl_msg = !rn2(3) ? "Huh?" : rn2(2) ? "What?" : "Eh?"; */
	    else if (!mtmp->mcansee)
		verbl_msg = "Ich kann nicht sehen!"; /* EN verbl_msg = "I can't see!"; */
	    else if (mtmp->mtrapped) {
		struct trap *t = t_at(mtmp->mx, mtmp->my);

		if (t) t->tseen = 1;
		verbl_msg = "Ich sitze in einer Falle!"; /* EN verbl_msg = "I'm trapped!"; */
	    } else if (mtmp->mhp < mtmp->mhpmax/2)
		pline_msg = "VERB_BITTEN OBJECT um ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_OF NOUN_POT_HEALING."; /* EN pline_msg = "asks for a potion of healing."; */
	    else if (mtmp->mtame && !mtmp->isminion &&
						moves > EDOG(mtmp)->hungrytime)
		verbl_msg = "Ich habe Hunger."; /* EN verbl_msg = "I'm hungry."; */
	    /* Specific monsters' interests */
	    else if (is_elf(ptr))
		pline_msg = "flucht �ber Orks."; /* EN pline_msg = "curses orcs."; */
	    else if (is_dwarf(ptr))
		pline_msg = "VERB_SPRECHEN �ber Bergbau."; /* EN pline_msg = "talks about mining."; */
	    else if (likes_magic(ptr))
		pline_msg = "VERB_SPRECHEN �ber das Zauberhandwerk."; /* EN pline_msg = "talks about spellcraft."; */
	    else if (ptr->mlet == S_CENTAUR)
		pline_msg = "fachsimpelt �ber die Jagd."; /* EN pline_msg = "discusses hunting."; */
	    else switch (monsndx(ptr)) {
		case PM_HOBBIT:
		    pline_msg = (mtmp->mhpmax - mtmp->mhp >= 10) ?
				"VERB_BEKLAGEN sich �ber die unangenehmen Bedingungen im Dungeon." /* EN "complains about unpleasant dungeon conditions." */
				: "VERB_FRAGEN OBJECT PRONOMEN_PERSONAL nach dem Einen Ring."; /* EN : "asks you about the One Ring."; */
		    break;
		case PM_ARCHEOLOGIST:
    pline_msg = "redet enthusiastisch �ber einen Artikel im letzten \"H�hlenforscher\"."; /* EN pline_msg = "describes a recent article in \"Spelunker Today\" magazine."; */
		    break;
#ifdef TOURIST
		case PM_TOURIST:
		    verbl_msg = "Aloha."; /* EN verbl_msg = "Aloha."; */
		    break;
#endif
		default:
		    pline_msg = "beginnt eine Diskussion �ber H�hlenforschung."; /* EN pline_msg = "discusses dungeon exploration."; */
		    break;
	    }
	    break;
	case MS_SEDUCE:
#ifdef SEDUCE
	    if (ptr->mlet != S_NYMPH &&
		could_seduce(mtmp, &youmonst, (struct attack *)0) == 1) {
			(void) doseduce(mtmp);
			break;
	    }
	    switch ((poly_gender() != (int) mtmp->female) ? rn2(3) : 0)
#else
	    switch ((poly_gender() == 0) ? rn2(3) : 0)
#endif
	    {
		case 2:
			verbl_msg = "Hallo, Matrose."; /* EN verbl_msg = "Hello, sailor."; */
			break;
		case 1:
			pline_msg = "VERB_GRABEN OBJECT PRONOMEN_PERSONAL an."; /* EN pline_msg = "comes on to you."; */
			break;
		default:
			pline_msg = "VERB_BESCHWATZEN OBJECT PRONOMEN_PERSONAL."; /* EN pline_msg = "cajoles you."; */
	    }
	    break;
#ifdef KOPS
	case MS_ARREST:
	    if (mtmp->mpeaceful)
		verbalize("SUBJECT MODIFIER_VERB_IMPERATIV VERB_BESCHREIBEN bitte den Tathergang, %s.", /* EN verbalize("Just the facts, %s.", */
		      flags.female ? "werte Dame" : "werter Herr"); /* EN flags.female ? "Ma'am" : "Sir"); */
	    else {
		static const char * const arrest_msg[3] = {
		    "Alles, was Sie sagen, kann gegen Sie verwendet werden.", /* EN "Anything you say can be used against you.", */
		    "SUBJECT PRONOMEN_PERSONAL VERB_SEIN verhaftet!", /* EN "You're under arrest!", */
				"Halt! Im Namen des Gesetzes!", /* EN "Stop in the name of the Law!", */
		};
		verbl_msg = arrest_msg[rn2(3)];
	    }
	    break;
#endif
	case MS_BRIBE:
	    if (mtmp->mpeaceful && !mtmp->mtame) {
		(void) demon_talk(mtmp);
		break;
	    }
	    /* fall through */
	case MS_CUSS:
	    if (!mtmp->mpeaceful)
		cuss(mtmp);
	    break;
	case MS_SPELL:
	    /* deliberately vague, since it's not actually casting any spell */
	    pline_msg = "scheint einen Zauberspruch zu murmeln."; /* EN pline_msg = "seems to mutter a cantrip."; */
	    break;
	case MS_NURSE:
	    if (uwep && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep)))
		verbl_msg = "Legen Sie die Waffe beiseite, bevor Sie jemanden damit verletzen!"; /* EN verbl_msg = "Put that weapon away before you hurt someone!"; */
	    else if (uarmc || uarm || uarmh || uarms || uarmg || uarmf)
		verbl_msg = Role_if(PM_HEALER) ?
			  (flags.female ? "Werte Kollegin, Ich kann Ihnen nicht helfen, wenn Sie nicht kooperieren." : /* EN */
			  "Werter Kollege, Ich kann Ihnen nicht helfen, wenn Sie nicht kooperieren.") : /* EN "Doc, I can't help you unless you cooperate." : */
			  "Bitte machen Sie sich zur Untersuchung frei."; /* EN "Please undress so I can examine you."; */
#ifdef TOURIST
	    else if (uarmu)
		verbl_msg = "Ziehen Sie bitte Ihr Hemd aus."; /* EN verbl_msg = "Take off your shirt, please."; */
#endif
	    else verbl_msg = "Entspannen Sie sich, das wird �berhaupt nicht weh tun."; /* EN else verbl_msg = "Relax, this won't hurt a bit."; */
	    break;
	case MS_GUARD:
#ifndef GOLDOBJ
	    if (u.ugold)
#else
	    if (money_cnt(invent))
#endif
		verbl_msg = "Bitte MODIFIER_VERB_IMPERATIV VERB_DROP das Gold SATZKLAMMER und VERB_FOLGEN mir."; /* EN verbl_msg = "Please drop that gold and follow me."; */
	    else
		verbl_msg = "Bitte MODIFIER_VERB_IMPERATIV VERB_FOLGEN mir."; /* EN verbl_msg = "Please follow me."; */
	    break;
	case MS_SOLDIER:
	    {
		static const char * const soldier_foe_msg[3] = {
		    "Widerstand ist zwecklos!", /* EN "Resistance is useless!", */
		    "SUBJECT PRONOMEN_PERSONAL VERB_SEIN geliefert!", /* EN "You're dog meat!", */
		    "SATZBEGINN MODIFIER_VERB_IMPERATIV VERB_ERGEBEN OBJECT PRONOMEN_PERSONAL!", /* EN "Surrender!", */
		},		  * const soldier_pax_msg[3] = {
		    "Mann, was f�r 'ne miese Bezahlung wir kriegen!", /* EN "What lousy pay we're getting here!", */
		    "Den Fra� w�rden nicht mal Orks essen!", /* EN "The food's not fit for Orcs!", */
		    "Meine F��e schmerzen, ich bin schon den ganzen Tag auf den Beinen!", /* EN "My feet hurt, I've been on them all day!", */
		};
		verbl_msg = mtmp->mpeaceful ? soldier_pax_msg[rn2(3)]
					    : soldier_foe_msg[rn2(3)];
	    }
	    break;
	case MS_RIDER:
	    if (ptr == &mons[PM_DEATH] && !rn2(10))
		pline_msg = "ist damit besch�ftigt ein Exemplar von Sandman #8 zu lesen."; /* EN pline_msg = "is busy reading a copy of Sandman #8."; */
	    else verbl_msg = (ptr == &mons[PM_DEATH]) ? "F�R WEN H�LTST DU DICH EIGENTLICH? KRIEG?" : "F�r wen h�ltst du dich eigentlich? Krieg?"; /* EN else verbl_msg = "Who do you think you are, War?"; */
	    break;
    }

    if (pline_msg) pline("SUBJECT %s %s", Monnam(mtmp), pline_msg);
    else if (verbl_msg) verbalize(verbl_msg);
    return(1);
}


int
dotalk()
{
    int result;
    boolean save_soundok = flags.soundok;
    flags.soundok = 1;	/* always allow sounds while chatting */
    result = dochat();
    flags.soundok = save_soundok;
    return result;
}

static int
dochat()
{
    register struct monst *mtmp;
    register int tx,ty;
    struct obj *otmp;

    if (is_silent(youmonst.data)) {
	pline("Als %s SUBJECT_IM_SATZ VERB_KOENNEN PRONOMEN_PERSONAL nicht sprechen.", an(youmonst.data->mname)); /* EN pline("As %s, you cannot speak.", an(youmonst.data->mname)); */
	return(0);
    }
    if (Strangled) {
	You_cant("sprechen.  NEUER_SATZ SUBJECT PRONOMEN_PERSONAL VERB_ERSTICKEN!"); /* EN You_cant("speak.  You're choking!"); */
	return(0);
    }
    if (u.uswallow) {
	pline("Die k�nnen KASUS_AKKUSATIV PRONOMEN_PERSONAL drau�en nicht h�ren."); /* EN pline("They won't hear you out there."); */
	return(0);
    }
    if (Underwater) {
	Your("NOUN_STIMME ist unter Wasser unverst�ndlich."); /* EN Your("speech is unintelligible underwater."); */
	return(0);
    }

    if (!Blind && (otmp = shop_object(u.ux, u.uy)) != (struct obj *)0) {
	/* standing on something in a shop and chatting causes the shopkeeper
	   to describe the price(s).  This can inhibit other chatting inside
	   a shop, but that shouldn't matter much.  shop_object() returns an
	   object iff inside a shop and the shopkeeper is present and willing
	   (not angry) and able (not asleep) to speak and the position contains
	   any objects other than just gold.
	*/
	price_quote(otmp);
	return(1);
    }

    if (!getdir("Mit wem sprechen? (in welche Richtung)")) { /* EN if (!getdir("Talk to whom? (in what direction)")) { */
	/* decided not to chat */
	return(0);
    }

#ifdef STEED
    if (u.usteed && u.dz > 0)
	return (domonnoise(u.usteed));
#endif
    if (u.dz) {
	pline("Die k�nnen KASUS_AKKUSATIV PRONOMEN_PERSONAL da %s nicht h�ren.", u.dz < 0 ? "oben" : "unten"); /* EN pline("They won't hear you %s there.", u.dz < 0 ? "up" : "down"); */
	return(0);
    }

    if (u.dx == 0 && u.dy == 0) {
/*
 * Let's not include this.  It raises all sorts of questions: can you wear
 * 2 helmets, 2 amulets, 3 pairs of gloves or 6 rings as a marilith,
 * etc...  --KAA
	if (u.umonnum == PM_ETTIN) {
	    You("discover that your other head makes boring conversation.");
	    return(1);
	}
*/
	pline("Selbstgespr�che sind eine schlechte Angewohnheit f�r einen Abenteuer."); /* EN pline("Talking to yourself is a bad habit for a dungeoneer."); */
	return(0);
    }

    tx = u.ux+u.dx; ty = u.uy+u.dy;
    mtmp = m_at(tx, ty);

    if (!mtmp || mtmp->mundetected ||
		mtmp->m_ap_type == M_AP_FURNITURE ||
		mtmp->m_ap_type == M_AP_OBJECT)
	return(0);

    /* sleeping monsters won't talk, except priests (who wake up) */
    if ((!mtmp->mcanmove || mtmp->msleeping) && !mtmp->ispriest) {
	/* If it is unseen, the player can't tell the difference between
	   not noticing him and just not existing, so skip the message. */
	if (canspotmon(mtmp))
	    pline("SUBJECT %s VERB_SEEM OBJECT PRONOMEN_PERSONAL nicht zu bemerken.", Monnam(mtmp)); /* EN pline("%s seems not to notice you.", Monnam(mtmp)); */
	return(0);
    }

    /* if this monster is waiting for something, prod it into action */
    mtmp->mstrategy &= ~STRAT_WAITMASK;

    if (mtmp->mtame && mtmp->meating) {
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);
	pline("SUBJECT %s VERB_SCHMATZEN ger�uschvoll.", Monnam(mtmp)); /* EN pline("%s is eating noisily.", Monnam(mtmp)); */
	return (0);
    }

    return domonnoise(mtmp);
}

#ifdef USER_SOUNDS

extern void FDECL(play_usersound, (const char*, int));

typedef struct audio_mapping_rec {
#ifdef USER_SOUNDS_REGEX
	struct re_pattern_buffer regex;
#else
	char *pattern;
#endif
	char *filename;
	int volume;
	struct audio_mapping_rec *next;
} audio_mapping;

static audio_mapping *soundmap = 0;

char* sounddir = ".";

/* adds a sound file mapping, returns 0 on failure, 1 on success */
int
add_sound_mapping(mapping)
const char *mapping;
{
	char text[256];
	char filename[256];
	char filespec[256];
	int volume;

	if (sscanf(mapping, "MESG \"%255[^\"]\"%*[\t ]\"%255[^\"]\" %d",
		   text, filename, &volume) == 3) {
	    const char *err;
	    audio_mapping *new_map;

	    if (strlen(sounddir) + strlen(filename) > 254) {
		raw_print("sound file name too long");
		return 0;
	    }
	    Sprintf(filespec, "%s/%s", sounddir, filename);

	    if (can_read_file(filespec)) {
		new_map = (audio_mapping *)alloc(sizeof(audio_mapping));
#ifdef USER_SOUNDS_REGEX
		new_map->regex.translate = 0;
		new_map->regex.fastmap = 0;
		new_map->regex.buffer = 0;
		new_map->regex.allocated = 0;
		new_map->regex.regs_allocated = REGS_FIXED;
#else
		new_map->pattern = (char *)alloc(strlen(text) + 1);
		Strcpy(new_map->pattern, text);
#endif
		new_map->filename = strdup(filespec);
		new_map->volume = volume;
		new_map->next = soundmap;

#ifdef USER_SOUNDS_REGEX
		err = re_compile_pattern(text, strlen(text), &new_map->regex);
#else
		err = 0;
#endif
		if (err) {
		    raw_print(err);
		    free(new_map->filename);
		    free(new_map);
		    return 0;
		} else {
		    soundmap = new_map;
		}
	    } else {
		Sprintf(text, "cannot read %.243s", filespec);
		raw_print(text);
		return 0;
	    }
	} else {
	    raw_print("syntax error in SOUND");
	    return 0;
	}

	return 1;
}

void
play_sound_for_message(msg)
const char* msg;
{
	audio_mapping* cursor = soundmap;

	while (cursor) {
#ifdef USER_SOUNDS_REGEX
	    if (re_search(&cursor->regex, msg, strlen(msg), 0, 9999, 0) >= 0) {
#else
	    if (pmatch(cursor->pattern, msg)) {
#endif
		play_usersound(cursor->filename, cursor->volume);
	    }
	    cursor = cursor->next;
	}
}

#endif /* USER_SOUNDS */

#endif /* OVLB */

/*sounds.c*/
