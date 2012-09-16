/*	SCCS Id: @(#)sit.c	3.4	2002/09/21	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"

void
take_gold()
{
#ifndef GOLDOBJ
	if (u.ugold <= 0)  {
		You("VERB_HAVE eine seltsame Empfindung."); /* EN You_feel("a strange sensation."); */
	} else {
		You("VERB_BEMERKEN, dass SUBJECT_IM_SATZ PRONOMEN_PERSONAL kein Gold mehr VERB_HAVE!"); /* EN You("notice you have no gold!"); */
		u.ugold = 0;
		flags.botl = 1;
	}
#else
        struct obj *otmp, *nobj;
	int lost_money = 0;
	for (otmp = invent; otmp; otmp = nobj) {
		nobj = otmp->nobj;
		if (otmp->oclass == COIN_CLASS) {
			lost_money = 1;
			delobj(otmp);
		}
	}
	if (!lost_money)  {
		You("VERB_HAVE eine seltsame Empfindung."); /* EN You_feel("a strange sensation."); */
	} else {
		You("VERB_BEMERKEN, dass SUBJECT_IM_SATZ PRONOMEN_PERSONAL kein Gold mehr VERB_HAVE!"); /* EN You("notice you have no money!"); */
		flags.botl = 1;
	}
#endif
}

int
dosit()
{
	static const char sit_message[] = "VERB_SITZEN auf OBJECT KASUS_DATIV ARTIKEL_BESTIMMTER %s."; /* EN static const char sit_message[] = "sit on the %s."; */
	register struct trap *trap;
	register int typ = levl[u.ux][u.uy].typ;


#ifdef STEED
	if (u.usteed) {
	    You("VERB_SITZEN bereits auf KASUS_DATIV %s.", mon_nam(u.usteed)); /* EN You("are already sitting on %s.", mon_nam(u.usteed)); */
	    return (0);
	}
#endif

	if(!can_reach_floor())	{
	    if (Levitation)
		You("VERB_SCHLAGEN Purzelbäume."); /* EN You("tumble in place."); */
	    else
		You("VERB_SITZEN auf Luft."); /* EN You("are sitting on air."); */
	    return 0;
	} else if (is_pool(u.ux, u.uy) && !Underwater) {  /* water walking */
	    goto in_water;
	}

	if(OBJ_AT(u.ux, u.uy)) {
	    register struct obj *obj;

	    obj = level.objects[u.ux][u.uy];
	    You("VERB_SITZEN auf OBJECT KASUS_DATIV %s.", the(xname(obj))); /* EN You("sit on %s.", the(xname(obj))); */
	    if (!(Is_box(obj) || objects[obj->otyp].oc_material == CLOTH))
		pline("Das ist nicht sehr bequem ..."); /* EN pline("It's not very comfortable..."); */

	} else if ((trap = t_at(u.ux, u.uy)) != 0 ||
		   (u.utrap && (u.utraptype >= TT_LAVA))) {

	    if (u.utrap) {
		exercise(A_WIS, FALSE);	/* you're getting stuck longer */
		if(u.utraptype == TT_BEARTRAP) {
		    pline("Mit KASUS_DATIV PRONOMEN_POSSESSIV %s in KASUS_DATIV ARTIKEL_BESTIMMTER NOUN_BEARTRAP SUBJECT_IM_SATZ VERB_KOENNEN PRONOMEN_PERSONAL OBJECT PRONOMEN_PERSONAL nicht hinsetzen.", body_part(FOOT)); /* EN You_cant("sit down with your %s in the bear trap.", body_part(FOOT)); */
		    u.utrap++;
	        } else if(u.utraptype == TT_PIT) {
		    if(trap->ttyp == SPIKED_PIT) {
			You("VERB_SETZEN OBJECT PRONOMEN_PERSONAL auf einen Stachel.  Aua!"); /* EN You("sit down on a spike.  Ouch!"); */
			losehp(1, "durch Sitzen auf einen Eisenstachel", KILLED_WITHOUT_PREPOSITION); /* EN losehp(1, "sitting on an iron spike", KILLED_BY); */
			exercise(A_STR, FALSE);
		    } else
			You("VERB_SETZEN OBJECT PRONOMEN_PERSONAL in die Grube."); /* EN You("sit down in the pit."); */
		    u.utrap += rn2(5);
		} else if(u.utraptype == TT_WEB) {
		    You("VERB_SETZEN OBJECT PRONOMEN_PERSONAL ins Spinnenetz und VERB_VERHEDDERN OBJECT PRONOMEN_PERSONAL noch mehr!"); /* EN You("sit in the spider web and get entangled further!"); */
		    u.utrap += rn1(10, 5);
		} else if(u.utraptype == TT_LAVA) {
		    /* Must have fire resistance or they'd be dead already */
		    You("VERB_SETZEN OBJECT PRONOMEN_PERSONAL in die Lava!"); /* EN You("sit in the lava!"); */
		    u.utrap += rnd(4);
		    losehp(d(2,10), "durch Sitzen in Lava", KILLED_WITHOUT_PREPOSITION); /* EN losehp(d(2,10), "sitting in lava", KILLED_BY); */
		} else if(u.utraptype == TT_INFLOOR) {
		    You("VERB_CAN OBJECT PRONOMEN_PERSONAL nicht richtig hinsetzen!"); /* EN You_cant("maneuver to sit!"); */
		    u.utrap++;
		}
	    } else {
	        You("VERB_SETZEN OBJECT PRONOMEN_PERSONAL hin."); /* EN You("sit down."); */
		dotrap(trap, 0);
	    }
	} else if(Underwater || Is_waterlevel(&u.uz)) {
	    if (Is_waterlevel(&u.uz))
		pline("Hier gibt es keine Sitzkissen."); /* EN There("are no cushions floating nearby."); */
	    else
		You("VERB_SETZEN OBJECT PRONOMEN_PERSONAL auf den schlammigen Untergrund."); /* EN You("sit down on the muddy bottom."); */
	} else if(is_pool(u.ux, u.uy)) {
 in_water:
	    You("VERB_SITZEN im Wasser."); /* EN You("sit in the water."); */
	    if (!rn2(10) && uarm)
		(void) rust_dmg(uarm, "NOUN_ARMOR", 1, TRUE, &youmonst); /* EN (void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst); */
	    if (!rn2(10) && uarmf && uarmf->otyp != WATER_WALKING_BOOTS)
		(void) rust_dmg(uarm, "NOUN_ARMOR", 1, TRUE, &youmonst); /* EN (void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst); */
#ifdef SINKS
	} else if(IS_SINK(typ)) {

	    You(sit_message, defsyms[S_sink].explanation);
	    Your("%s VERB_WERDEN nass.", humanoid(youmonst.data) ? "NOUN_HINTERTEIL" : "NOUN_UNTERSEITE"); /* EN Your("%s gets wet.", humanoid(youmonst.data) ? "rump" : "underside"); */
#endif
	} else if(IS_ALTAR(typ)) {

	    You(sit_message, defsyms[S_altar].explanation);
	    altar_wrath(u.ux, u.uy);

	} else if(IS_GRAVE(typ)) {

	    You(sit_message, defsyms[S_grave].explanation);

	} else if(typ == STAIRS) {

	    You(sit_message, "NOUN_STUFEs"); /* EN You(sit_message, "stairs"); */

	} else if(typ == LADDER) {

	    You(sit_message, "NOUN_LADDER"); /* EN You(sit_message, "ladder"); */

	} else if (is_lava(u.ux, u.uy)) {

	    /* must be WWalking */
	    You(sit_message, "NOUN_LAVA"); /* EN You(sit_message, "lava"); */
	    burn_away_slime();
	    if (likes_lava(youmonst.data)) {
		pline_The("NOUN_LAVA fühlt sich warm an."); /* EN pline_The("lava feels warm."); */
		return 1;
	    }
	    pline_The("NOUN_LAVA VERB_VERBRENNEN OBJECT PRONOMEN_PERSONAL!"); /* EN pline_The("lava burns you!"); */
	    losehp(d((Fire_resistance ? 2 : 10), 10),
		   "durch Sitzen auf Lava", KILLED_WITHOUT_PREPOSITION); /* EN "sitting on lava", KILLED_BY); */

	} else if (is_ice(u.ux, u.uy)) {

	    You(sit_message, defsyms[S_ice].explanation);
	    if (!Cold_resistance) pline_The("NOUN_ICE fühlt sich kalt an."); /* EN if (!Cold_resistance) pline_The("ice feels cold."); */

	} else if (typ == DRAWBRIDGE_DOWN) {

	    You(sit_message, "NOUN_DRAWBRIDGE"); /* EN You(sit_message, "drawbridge"); */

	} else if(IS_THRONE(typ)) {

	    You(sit_message, defsyms[S_throne].explanation);
	    if (rnd(6) > 4)  {
		switch (rnd(13))  {
		    case 1:
			(void) adjattrib(rn2(A_MAX), -rn1(4,3), FALSE);
			losehp(rnd(10), "ADJEKTIV_CURSED NOUN_THRON", KILLED_BY_AN); /* EN losehp(rnd(10), "cursed throne", KILLED_BY_AN); */
			break;
		    case 2:
			(void) adjattrib(rn2(A_MAX), 1, FALSE);
			break;
		    case 3:
			pline("SUBJECT ARTIKEL_UNBESTIMMTER%s elektrischer NOUN_SCHLAG VERB_SCHIESSEN OBJECT durch PRONOMEN_POSSESSIV NOUN_BODY!", /* EN pline("A%s electric shock shoots through your body!", */
			      (Shock_resistance) ? "" : " massiver"); /* EN (Shock_resistance) ? "n" : " massive"); */
			losehp(Shock_resistance ? rnd(6) : rnd(30),
			       "ADJEKTIV_ELEKTRISCH NOUN_STUHL", KILLED_BY_AN); /* EN "electric chair", KILLED_BY_AN); */
			exercise(A_CON, FALSE);
			break;
		    case 4:
			Du_fuehlst_dich("viel, viel besser!"); /* EN You_feel("much, much better!"); */
			if (Upolyd) {
			    if (u.mh >= (u.mhmax - 5))  u.mhmax += 4;
			    u.mh = u.mhmax;
			}
			if(u.uhp >= (u.uhpmax - 5))  u.uhpmax += 4;
			u.uhp = u.uhpmax;
			make_blinded(0L,TRUE);
			make_sick(0L, (char *) 0, FALSE, SICK_ALL);
			heal_legs();
			flags.botl = 1;
			break;
		    case 5:
			take_gold();
			break;
		    case 6:
			if(u.uluck + rn2(5) < 0) {
			    You("VERB_FUEHLEN, NEUER_SATZ SUBJECT_IM_SATZ wie PRONOMEN_POSSESSIV NOUN_LUCK sich ändert."); /* EN You_feel("your luck is changing."); */
			    change_luck(1);
			} else	    makewish();
			break;
		    case 7:
			{
			register int cnt = rnd(10);

			pline("Eine Stimme ertönt:"); /* EN pline("A voice echoes:"); */
			verbalize("Thy audience hath been summoned, %s!", /* EN verbalize("Thy audience hath been summoned, %s!", */ // TODO DE
				  flags.female ? "Gebieterin" : "Gebieter"); /* EN flags.female ? "Dame" : "Sire"); */
			while(cnt--)
			    (void) makemon(courtmon(), u.ux, u.uy, NO_MM_FLAGS);
			break;
			}
		    case 8:
			pline("Eine Stimme ertönt:"); /* EN pline("A voice echoes:"); */
			verbalize("By thy Imperious order, %s...", /* EN verbalize("By thy Imperious order, %s...", */ // TODO DE
				  flags.female ? "Gebieterin" : "Gebieter"); /* EN flags.female ? "Dame" : "Sire"); */
			do_genocide(5);	/* REALLY|ONTHRONE, see do_genocide() */
			break;
		    case 9:
			pline("Eine Stimme ertönt:"); /* EN pline("A voice echoes:"); */
	verbalize("Sei verflucht for sitting upon this most holy throne!"); /* EN verbalize("A curse upon thee for sitting upon this most holy throne!"); */ // TODO DE
			if (Luck > 0)  {
			    make_blinded(Blinded + rn1(100,250),TRUE);
			} else	    rndcurse();
			break;
		    case 10:
			if (Luck < 0 || (HSee_invisible & INTRINSIC))  {
				if (level.flags.nommap) {
					pline(
					"Ein schreckliches Dröhnen erfüllt KASUS_AKKUSATIV PRONOMEN_POSSESSIV NOUN_KOPF!"); /* EN "A terrible drone fills your head!"); */
					make_confused(HConfusion + rnd(30),
									FALSE);
				} else {
					pline("Ein Bild formt sich in KASUS_DATIV PRONOMEN_POSSESSIV NOUN_GEDANKEs."); /* EN pline("An image forms in your mind."); */
					do_mapping();
				}
			} else  {
				Your("NOUN_BLICK klärt sich."); /* EN Your("vision becomes clear."); */
				HSee_invisible |= FROMOUTSIDE;
				newsym(u.ux, u.uy);
			}
			break;
		    case 11:
			if (Luck < 0)  {
			    Du_fuehlst_dich("bedroht."); /* EN You_feel("threatened."); */
			    aggravate();
			} else  {
			    Du_spuerst("einen reißenden Schmerz."); /* EN You_feel("a wrenching sensation."); */
			    tele();		/* teleport him */
			}
			break;
		    case 12:
			Dir_wird("eine Einsicht gewährt!"); /* EN You("are granted an insight!"); */
			if (invent) {
			    /* rn2(5) agrees w/seffects() */
			    identify_pack(rn2(5));
			}
			break;
		    case 13:
			Your("NOUN_VERSTAND verknotet sich zu einer Bretzel!"); /* EN Your("mind turns into a pretzel!"); */
			make_confused(HConfusion + rn1(7,16),FALSE);
			break;
		    default:	impossible("throne effect");
				break;
		}
	    } else {
		if (is_prince(youmonst.data))
		    Du_fuehlst_dich("hier sehr wohl."); /* EN You_feel("very comfortable here."); */
		else
		    Du_fuehlst_dich("irgendwie fehl am Platz ..."); /* EN You_feel("somehow out of place..."); */
	    }

	    if (!rn2(3) && IS_THRONE(levl[u.ux][u.uy].typ)) {
		/* may have teleported */
		levl[u.ux][u.uy].typ = ROOM;
		pline_The("NOUN_THRON VERB_AUFLOESEN sich in ein Logikwölkchen SATZKLAMMER."); /* EN pline_The("throne vanishes in a puff of logic."); */
		newsym(u.ux,u.uy);
	    }

	} else if (lays_eggs(youmonst.data)) {
		struct obj *uegg;

		if (!flags.female) {
			pline("Männchen können keine Eier legen!"); /* EN pline("Males can't lay eggs!"); */
			return 0;
		}

		if (u.uhunger < (int)objects[EGG].oc_nutrition) {
			You("VERB_HAVE nicht genug Energie um ein Ei zu legen."); /* EN You("don't have enough energy to lay an egg."); */
			return 0;
		}

		uegg = mksobj(EGG, FALSE, FALSE);
		uegg->spe = 1;
		uegg->quan = 1;
		uegg->owt = weight(uegg);
		uegg->corpsenm = egg_type_from_parent(u.umonnum, FALSE);
		uegg->known = uegg->dknown = 1;
		attach_egg_hatch_timeout(uegg);
		You("VERB_LEGEN ein Ei."); /* EN You("lay an egg."); */
		dropy(uegg);
		stackobj(uegg);
		morehungry((int)objects[EGG].oc_nutrition);
	} else if (u.uswallow)
		pline("Hier gibt es keine Sitzmöglichkeiten!"); /* EN There("are no seats in here!"); */
	else
		pline("Spaß daran, %s zu sitzen?", auf_dem_Boden(u.ux,u.uy)); /* EN pline("Having fun sitting on the %s?", surface(u.ux,u.uy)); */
	return(1);
}

void
rndcurse()			/* curse a few inventory items at random! */
{
	int	nobj = 0;
	int	cnt, onum;
	struct	obj	*otmp;
	static const char mal_aura[] = "VERB_SPUEREN eine bösartige Aura um OBJECT %s."; /* EN static const char mal_aura[] = "feel a malignant aura surround %s."; */

	if (uwep && (uwep->oartifact == ART_MAGICBANE) && rn2(20)) {
	    You(mal_aura, "ARTIKEL_BESTIMMTER ADJEKTIV_MAGIE_ABSORBIEREND NOUN_KLINGE"); /* EN You(mal_aura, "the magic-absorbing blade"); */
	    return;
	}

	if(Antimagic) {
	    shieldeff(u.ux, u.uy);
	    You(mal_aura, "PRONOMEN_PERSONAL"); /* EN You(mal_aura, "you"); */
	}

	for (otmp = invent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
	    /* gold isn't subject to being cursed or blessed */
	    if (otmp->oclass == COIN_CLASS) continue;
#endif
	    nobj++;
	}
	if (nobj) {
	    for (cnt = rnd(6/((!!Antimagic) + (!!Half_spell_damage) + 1));
		 cnt > 0; cnt--)  {
		onum = rnd(nobj);
		for (otmp = invent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
		    /* as above */
		    if (otmp->oclass == COIN_CLASS) continue;
#endif
		    if (--onum == 0) break;	/* found the target */
		}
		/* the !otmp case should never happen; picking an already
		   cursed item happens--avoid "resists" message in that case */
		if (!otmp || otmp->cursed) continue;	/* next target */

		if(otmp->oartifact && spec_ability(otmp, SPFX_INTEL) &&
		   rn2(10) < 8) {
		    pline("%s!", Tobjnam(otmp, "resist")); /* EN pline("%s!", Tobjnam(otmp, "resist")); */ // TODO DE
		    continue;
		}

		if(otmp->blessed)
			unbless(otmp);
		else
			curse(otmp);
	    }
	    update_inventory();
	}

#ifdef STEED
	/* treat steed's saddle as extended part of hero's inventory */
	if (u.usteed && !rn2(4) &&
		(otmp = which_armor(u.usteed, W_SADDLE)) != 0 &&
		!otmp->cursed) {	/* skip if already cursed */
	    if (otmp->blessed)
		unbless(otmp);
	    else
		curse(otmp);
	    if (!Blind) {
		pline("%s %s %s.", /* EN pline("%s %s %s.", */ // TODO DE
		      s_suffix(upstart(y_monnam(u.usteed))), /* EN s_suffix(upstart(y_monnam(u.usteed))), */ // TODO DE
		      aobjnam(otmp, "glow"), /* EN aobjnam(otmp, "glow"), */ // TODO DE
		      hcolor(otmp->cursed ? NH_BLACK : (const char *)"ADJEKTIV_FARBE_BRAUN")); /* EN hcolor(otmp->cursed ? NH_BLACK : (const char *)"brown")); */
		otmp->bknown = TRUE;
	    }
	}
#endif	/*STEED*/
}

void
attrcurse()			/* remove a random INTRINSIC ability */
{
	switch(rnd(11)) {
	case 1 : if (HFire_resistance & INTRINSIC) {
			HFire_resistance &= ~INTRINSIC;
			Du_fuehlst_dich("wärmer."); /* EN You_feel("warmer."); */
			break;
		}
	case 2 : if (HTeleportation & INTRINSIC) {
			HTeleportation &= ~INTRINSIC;
			Du_fuehlst_dich("weniger sprunghaft."); /* EN You_feel("less jumpy."); */
			break;
		}
	case 3 : if (HPoison_resistance & INTRINSIC) {
			HPoison_resistance &= ~INTRINSIC;
			Dir_ist("ein bisschen schlecht!"); /* EN You_feel("a little sick!"); */
			break;
		}
	case 4 : if (HTelepat & INTRINSIC) {
			HTelepat &= ~INTRINSIC;
			if (Blind && !Blind_telepat)
			    see_monsters();	/* Can't sense mons anymore! */
			Your("NOUN_SINNs VERB_SCHWINDEN OBJECT KASUS_DATIV PRONOMEN_PERSONAL!"); /* EN Your("senses fail!"); */
			break;
		}
	case 5 : if (HCold_resistance & INTRINSIC) {
			HCold_resistance &= ~INTRINSIC;
			Du_fuehlst_dich("kühler."); /* EN You_feel("cooler."); */
			break;
		}
	case 6 : if (HInvis & INTRINSIC) {
			HInvis &= ~INTRINSIC;
			Du_fuehlst_dich("paranoid."); /* EN You_feel("paranoid."); */
			break;
		}
	case 7 : if (HSee_invisible & INTRINSIC) {
			HSee_invisible &= ~INTRINSIC;
			You("MODIFIER_VERB_PRAETERITUM VERB_DENKEN, NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL MODIFIER_KONJUNKTIV_II VERB_HABEN %s!", Hallucination ? "eine Miezekatze gesehen" /* EN You("%s!", Hallucination ? "tawt you taw a puttie tat" */
						: "etwas gesehen"); /* EN : "thought you saw something"); */
			break;
		}
	case 8 : if (HFast & INTRINSIC) {
			HFast &= ~INTRINSIC;
			Du_fuehlst_dich("langsamer."); /* EN You_feel("slower."); */
			break;
		}
	case 9 : if (HStealth & INTRINSIC) {
			HStealth &= ~INTRINSIC;
			Du_fuehlst_dich("tollpatschig."); /* EN You_feel("clumsy."); */
			break;
		}
	case 10: if (HProtection & INTRINSIC) {
			HProtection &= ~INTRINSIC;
			Du_fuehlst_dich("verletzlich."); /* EN You_feel("vulnerable."); */
			break;
		}
	case 11: if (HAggravate_monster & INTRINSIC) {
			HAggravate_monster &= ~INTRINSIC;
			Du_fuehlst_dich("weniger attraktiv."); /* EN You_feel("less attractive."); */
			break;
		}
	default: break;
	}
}

/*sit.c*/
