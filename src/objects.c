/*	SCCS Id: @(#)objects.c	3.4	2002/07/31	*/
/* Copyright (c) Mike Threepoint, 1989.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef OBJECTS_PASS_2_
/* first pass */
struct monst { struct monst *dummy; };	/* lint: struct obj's union */
#include "config.h"
#include "obj.h"
#include "objclass.h"
#include "prop.h"
#include "skills.h"

#else	/* !OBJECTS_PASS_2_ */
/* second pass */
#include "color.h"
#  define COLOR_FIELD(X) X,
#endif	/* !OBJECTS_PASS_2_ */

/*
 * Choose the language
 *
 * objects.c is compiled twice, once in the target language and once in
 * English.  The English version is linked to such things as makedefs so that
 * code that indexes on the English names does not have to be changed.
 *
 * The presence of -DENGLISH on the command line must override any other
 * choice of language, so that the aforementioned English version will build
 * properly.
 */
#ifdef ENGLISH
# include "obj-en.h"  /* English */
#else
# include "obj-de.h"  /* German */
#endif

/* objects have symbols: ) [ = " ( % ! ? + / $ * ` 0 _ . */

/*
 *	Note:  OBJ() and BITS() macros are used to avoid exceeding argument
 *	limits imposed by some compilers.  The ctnr field of BITS currently
 *	does not map into struct objclass, and is ignored in the expansion.
 *	The 0 in the expansion corresponds to oc_pre_discovered, which is
 *	set at run-time during role-specific character initialization.
 */

#ifndef OBJECTS_PASS_2_
/* first pass -- object descriptive text */
# define OBJ(name,desc) name,desc
# define OBJECT(obj,bits,prp,sym,prob,dly,wt,cost,sdam,ldam,oc1,oc2,nut,color) \
	{obj}

NEARDATA struct objdescr obj_descr[] = {
#else
/* second pass -- object definitions */

# define BITS(nmkn,mrg,uskn,ctnr,mgc,chrg,uniq,nwsh,big,tuf,dir,sub,mtrl) \
	nmkn,mrg,uskn,0,mgc,chrg,uniq,nwsh,big,tuf,dir,mtrl,sub /* SCO ODT 1.1 cpp fodder */
# define OBJECT(obj,bits,prp,sym,prob,dly,wt,cost,sdam,ldam,oc1,oc2,nut,color) \
	{0, 0, (char *)0, bits, prp, sym, 0, dly, COLOR_FIELD(color) \
	 prob, wt, cost, sdam, ldam, oc1, oc2, nut}
# ifndef lint
#  define HARDGEM(n) (n >= 8)
# else
#  define HARDGEM(n) (0)
# endif

NEARDATA struct objclass objects[] = {
#endif
/* dummy object[0] -- description [2nd arg] *must* be NULL */
	OBJECT(OBJ(NAM_STRANGE_OBJECT,(char *)0), BITS(1,0,0,0,0,0,0,0,0,0,0,P_NONE,0),
			0, ILLOBJ_CLASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),

/* weapons ... */
#define WEAPON(name,app,kn,mg,bi,prob,wt,cost,sdam,ldam,hitbon,typ,sub,metal,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,mg,1,0,0,1,0,0,bi,0,typ,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, 0, wt, color )
#define PROJECTILE(name,app,kn,prob,wt,cost,sdam,ldam,hitbon,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), \
		BITS(kn,1,1,0,0,1,0,0,0,0,PIERCE,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, 0, wt, color )
#define BOW(name,app,kn,prob,wt,cost,hitbon,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,0,1,0,0,1,0,0,0,0,0,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, 2, 2, hitbon, 0, wt, color )

/* Note: for weapons that don't do an even die of damage (ex. 2-7 or 3-18)
 * the extra damage is added on in weapon.c, not here! */

#define P PIERCE
#define S SLASH
#define B WHACK

/* missiles */
PROJECTILE(NAM_ARROW, (char *)0,
		1, 55, 1, 2, 6, 6, 0, IRON, -P_BOW, HI_METAL),
PROJECTILE(NAM_ELVEN_ARROW, NAM_RUNED_ARROW,
		0, 20, 1, 2, 7, 6, 0, WOOD, -P_BOW, HI_WOOD),
PROJECTILE(NAM_ORCISH_ARROW, NAM_CRUDE_ARROW,
		0, 20, 1, 2, 5, 6, 0, IRON, -P_BOW, CLR_BLACK),
PROJECTILE(NAM_SILVER_ARROW, (char *)0,
		1, 12, 1, 5, 6, 6, 0, SILVER, -P_BOW, HI_SILVER),
PROJECTILE(NAM_YA, NAM_BAMBOO_ARROW,
		0, 15, 1, 4, 7, 7, 1, METAL, -P_BOW, HI_METAL),
PROJECTILE(NAM_CROSSBOW_BOLT, (char *)0,
		1, 55, 1, 2, 4, 6, 0, IRON, -P_CROSSBOW, HI_METAL),

WEAPON(NAM_DART, (char *)0,
	1, 1, 0, 60,  1,  2,  3,  2, 0, P,   -P_DART, IRON, HI_METAL),
WEAPON(NAM_SHURIKEN, NAM_THROWING_STAR,
	0, 1, 0, 35,  1,  5,  8,  6, 2, P,   -P_SHURIKEN, IRON, HI_METAL),
WEAPON(NAM_BOOMERANG, (char *)0,
	1, 1, 0, 15,  5, 20,  9,  9, 0, 0,   -P_BOOMERANG, WOOD, HI_WOOD),

/* spears */
WEAPON(NAM_SPEAR, (char *)0,
	1, 1, 0, 50, 30,  3,  6,  8, 0, P,   P_SPEAR, IRON, HI_METAL),
WEAPON(NAM_ELVEN_SPEAR, NAM_RUNED_SPEAR,
	0, 1, 0, 10, 30,  3,  7,  8, 0, P,   P_SPEAR, WOOD, HI_WOOD),
WEAPON(NAM_ORCISH_SPEAR, NAM_CRUDE_SPEAR,
	0, 1, 0, 13, 30,  3,  5,  8, 0, P,   P_SPEAR, IRON, CLR_BLACK),
WEAPON(NAM_DWARVISH_SPEAR, NAM_STOUT_SPEAR,
	0, 1, 0, 12, 35,  3,  8,  8, 0, P,   P_SPEAR, IRON, HI_METAL),
WEAPON(NAM_SILVER_SPEAR, (char *)0,
	1, 1, 0,  2, 36, 40,  6,  8, 0, P,   P_SPEAR, SILVER, HI_SILVER),
WEAPON(NAM_JAVELIN, NAM_THROWING_SPEAR,
	0, 1, 0, 10, 20,  3,  6,  6, 0, P,   P_JAVELIN, IRON, HI_METAL),

WEAPON(NAM_TRIDENT, (char *)0,
	1, 0, 0,  8, 25,  5,  6,  4, 0, P,   P_TRIDENT, IRON, HI_METAL),
						/* +1 small, +2d4 large */

/* blades */
WEAPON(NAM_DAGGER, (char *)0,
	1, 1, 0, 30, 10,  4,  4,  3, 2, P,   P_DAGGER, IRON, HI_METAL),
WEAPON(NAM_ELVEN_DAGGER, NAM_RUNED_DAGGER,
	0, 1, 0, 10, 10,  4,  5,  3, 2, P,   P_DAGGER, WOOD, HI_WOOD),
WEAPON(NAM_ORCISH_DAGGER, NAM_CRUDE_DAGGER,
	0, 1, 0, 12, 10,  4,  3,  3, 2, P,   P_DAGGER, IRON, CLR_BLACK),
WEAPON(NAM_SILVER_DAGGER, (char *)0,
	1, 1, 0,  3, 12, 40,  4,  3, 2, P,   P_DAGGER, SILVER, HI_SILVER),
WEAPON(NAM_ATHAME, (char *)0,
	1, 1, 0,  0, 10,  4,  4,  3, 2, S,   P_DAGGER, IRON, HI_METAL),
WEAPON(NAM_SCALPEL, (char *)0,
	1, 1, 0,  0,  5,  6,  3,  3, 2, S,   P_KNIFE, METAL, HI_METAL),
WEAPON(NAM_KNIFE, (char *)0,
	1, 1, 0, 20,  5,  4,  3,  2, 0, P|S, P_KNIFE, IRON, HI_METAL),
WEAPON(NAM_STILETTO, (char *)0,
	1, 1, 0,  5,  5,  4,  3,  2, 0, P|S, P_KNIFE, IRON, HI_METAL),
WEAPON(NAM_WORM_TOOTH, (char *)0,
	1, 0, 0,  0, 20,  2,  2,  2, 0, 0,   P_KNIFE, 0, CLR_WHITE),
WEAPON(NAM_CRYSKNIFE, (char *)0,
	1, 0, 0,  0, 20,100, 10, 10, 3, P,   P_KNIFE, MINERAL, CLR_WHITE),

WEAPON(NAM_AXE, (char *)0,
	1, 0, 0, 40, 60,  8,  6,  4, 0, S,   P_AXE, IRON, HI_METAL),
WEAPON(NAM_BATTLE_AXE, NAM_DOUBLE_HEADED_AXE,
	0, 0, 1, 10,120, 40,  8,  6, 0, S,   P_AXE, IRON, HI_METAL),
						/* "double-bitted" ? */

/* swords */
WEAPON(NAM_SHORT_SWORD, (char *)0,
	1, 0, 0,  8, 30, 10,  6,  8, 0, P,   P_SHORT_SWORD, IRON, HI_METAL),
WEAPON(NAM_ELVEN_SHORT_SWORD, NAM_RUNED_SHORT_SWORD,
	0, 0, 0,  2, 30, 10,  8,  8, 0, P,   P_SHORT_SWORD, WOOD, HI_WOOD),
WEAPON(NAM_ORCISH_SHORT_SWORD, NAM_CRUDE_SHORT_SWORD,
	0, 0, 0,  3, 30, 10,  5,  8, 0, P,   P_SHORT_SWORD, IRON, CLR_BLACK),
WEAPON(NAM_DWARVISH_SHORT_SWORD, NAM_BROAD_SHORT_SWORD,
	0, 0, 0,  2, 30, 10,  7,  8, 0, P,   P_SHORT_SWORD, IRON, HI_METAL),
WEAPON(NAM_SCIMITAR, NAM_CURVED_SWORD,
	0, 0, 0, 15, 40, 15,  8,  8, 0, S,   P_SCIMITAR, IRON, HI_METAL),
WEAPON(NAM_SILVER_SABER, (char *)0,
	1, 0, 0,  6, 40, 75,  8,  8, 0, S,   P_SABER, SILVER, HI_SILVER),
WEAPON(NAM_BROADSWORD, (char *)0,
	1, 0, 0,  8, 70, 10,  4,  6, 0, S,   P_BROAD_SWORD, IRON, HI_METAL),
						/* +d4 small, +1 large */
WEAPON(NAM_ELVEN_BROADSWORD, NAM_RUNED_BROADSWORD,
	0, 0, 0,  4, 70, 10,  6,  6, 0, S,   P_BROAD_SWORD, WOOD, HI_WOOD),
						/* +d4 small, +1 large */
WEAPON(NAM_LONG_SWORD, (char *)0,
	1, 0, 0, 50, 40, 15,  8, 12, 0, S,   P_LONG_SWORD, IRON, HI_METAL),
WEAPON(NAM_TWO_HANDED_SWORD, (char *)0,
	1, 0, 1, 22,150, 50, 12,  6, 0, S,   P_TWO_HANDED_SWORD, IRON, HI_METAL),
						/* +2d6 large */
WEAPON(NAM_KATANA, NAM_SAMURAI_SWORD,
	0, 0, 0,  4, 40, 80, 10, 12, 1, S,   P_LONG_SWORD, IRON, HI_METAL),
/* special swords set up for artifacts */
WEAPON(NAM_TSURUGI, NAM_LONG_SAMURAI_SWORD,
	0, 0, 1,  0, 60,500, 16,  8, 2, S,   P_TWO_HANDED_SWORD, METAL, HI_METAL),
						/* +2d6 large */
WEAPON(NAM_RUNESWORD, NAM_RUNED_BROADSWORD,
	0, 0, 0,  0, 40,300,  4,  6, 0, S,   P_BROAD_SWORD, IRON, CLR_BLACK),
						/* +d4 small, +1 large */
						/* +5d2 +d8 from level drain */

/* polearms */
/* spear-type */
WEAPON(NAM_PARTISAN, NAM_VULGAR_POLEARM,
	0, 0, 1,  5, 80, 10,  6,  6, 0, P,   P_POLEARMS, IRON, HI_METAL),
						/* +1 large */
WEAPON(NAM_RANSEUR, NAM_HILTED_POLEARM,
	0, 0, 1,  5, 50,  6,  4,  4, 0, P,   P_POLEARMS, IRON, HI_METAL),
						/* +d4 both */
WEAPON(NAM_SPETUM, NAM_FORKED_POLEARM,
	0, 0, 1,  5, 50,  5,  6,  6, 0, P,   P_POLEARMS, IRON, HI_METAL),
						/* +1 small, +d6 large */
WEAPON(NAM_GLAIVE, NAM_SINGLE_EDGED_POLEARM,
	0, 0, 1,  8, 75,  6,  6, 10, 0, S,   P_POLEARMS, IRON, HI_METAL),
WEAPON(NAM_LANCE, (char *)0,
	1, 0, 0,  4,180, 10,  6,  8, 0, P,   P_LANCE, IRON, HI_METAL),
/* axe-type */
WEAPON(NAM_HALBERD, NAM_ANGLED_POLEAXE,
	0, 0, 1,  8,150, 10, 10,  6, 0, P|S, P_POLEARMS, IRON, HI_METAL),
						/* +1d6 large */
WEAPON(NAM_BARDICHE, NAM_LONG_POLEAXE,
	0, 0, 1,  4,120,  7,  4,  4, 0, S,   P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small, +2d4 large */
WEAPON(NAM_VOULGE, NAM_POLE_CLEAVER,
	0, 0, 1,  4,125,  5,  4,  4, 0, S,   P_POLEARMS, IRON, HI_METAL),
						/* +d4 both */
WEAPON(NAM_DWARVISH_MATTOCK, NAM_BROAD_PICK,
	0, 0, 1, 13,120, 50, 12,  8,-1, B,   P_PICK_AXE, IRON, HI_METAL),

/* curved/hooked */
WEAPON(NAM_FAUCHARD, NAM_POLE_SICKLE,
	0, 0, 1,  6, 60,  5,  6,  8, 0, P|S, P_POLEARMS, IRON, HI_METAL),
WEAPON(NAM_GUISARME, NAM_PRUNING_HOOK,
	0, 0, 1,  6, 80,  5,  4,  8, 0, S,   P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small */
WEAPON(NAM_BILL_GUISARME, NAM_HOOKED_POLEARM,
	0, 0, 1,  4,120,  7,  4, 10, 0, P|S, P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small */
/* other */
WEAPON(NAM_LUCERN_HAMMER, NAM_PRONGED_POLEARM,
	0, 0, 1,  5,150,  7,  4,  6, 0, B|P, P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small */
WEAPON(NAM_BEC_DE_CORBIN, NAM_BEAKED_POLEARM,
	0, 0, 1,  4,100,  8,  8,  6, 0, B|P, P_POLEARMS, IRON, HI_METAL),

/* bludgeons */
WEAPON(NAM_MACE, (char *)0,
	1, 0, 0, 40, 30,  5,  6,  6, 0, B,   P_MACE, IRON, HI_METAL),
						/* +1 small */
WEAPON(NAM_MORNING_STAR, (char *)0,
	1, 0, 0, 12,120, 10,  4,  6, 0, B,   P_MORNING_STAR, IRON, HI_METAL),
						/* +d4 small, +1 large */
WEAPON(NAM_WAR_HAMMER, (char *)0,
	1, 0, 0, 15, 50,  5,  4,  4, 0, B,   P_HAMMER, IRON, HI_METAL),
						/* +1 small */
WEAPON(NAM_CLUB, (char *)0,
	1, 0, 0, 12, 30,  3,  6,  3, 0, B,   P_CLUB, WOOD, HI_WOOD),
#ifdef KOPS
WEAPON(NAM_RUBBER_HOSE, (char *)0,
	1, 0, 0,  0, 20,  3,  4,  3, 0, B,   P_WHIP, PLASTIC, CLR_BROWN),
#endif
WEAPON(NAM_QUARTERSTAFF, NAM_STAFF,
	0, 0, 1, 11, 40,  5,  6,  6, 0, B,   P_QUARTERSTAFF, WOOD, HI_WOOD),
/* two-piece */
WEAPON(NAM_AKLYS, NAM_THONGED_CLUB,
	0, 0, 0,  8, 15,  4,  6,  3, 0, B,   P_CLUB, IRON, HI_METAL),
WEAPON(NAM_FLAIL, (char *)0,
	1, 0, 0, 40, 15,  4,  6,  4, 0, B,   P_FLAIL, IRON, HI_METAL),
						/* +1 small, +1d4 large */
/* misc */
WEAPON(NAM_BULLWHIP, (char *)0,
	1, 0, 0,  2, 20,  4,  2,  1, 0, 0,   P_WHIP, LEATHER, CLR_BROWN),

/* bows */
BOW(NAM_BOW, (char *)0,		1, 24, 30, 60, 0, WOOD, P_BOW, HI_WOOD),
BOW(NAM_ELVEN_BOW, NAM_RUNED_BOW,	0, 12, 30, 60, 0, WOOD, P_BOW, HI_WOOD),
BOW(NAM_ORCISH_BOW, NAM_CRUDE_BOW,	0, 12, 30, 60, 0, WOOD, P_BOW, CLR_BLACK),
BOW(NAM_YUMI, NAM_LONG_BOW,		0,  0, 30, 60, 0, WOOD, P_BOW, HI_WOOD),
BOW(NAM_SLING, (char *)0,		1, 40,  3, 20, 0, LEATHER, P_SLING, HI_LEATHER),
BOW(NAM_CROSSBOW, (char *)0,	1, 45, 50, 40, 0, WOOD, P_CROSSBOW, HI_WOOD),

#undef P
#undef S
#undef B

#undef WEAPON
#undef PROJECTILE
#undef BOW

/* armor ... */
/* IRON denotes ferrous metals, including steel.
 * Only IRON weapons and armor can rust.
 * Only COPPER (including brass) corrodes.
 * Some creatures are vulnerable to SILVER.
 */
#define ARMOR(name,desc,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,sub,metal,c) \
	OBJECT( \
		OBJ(name,desc), BITS(kn,0,1,0,mgc,1,0,0,blk,0,0,sub,metal), power, \
		ARMOR_CLASS, prob, delay, wt, cost, \
		0, 0, 10 - ac, can, wt, c )
#define HELM(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_HELM,metal,c)
#define CLOAK(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_CLOAK,metal,c)
#define SHIELD(name,desc,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,ARM_SHIELD,metal,c)
#define GLOVES(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_GLOVES,metal,c)
#define BOOTS(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_BOOTS,metal,c)

/* helmets */
HELM(NAM_ELVEN_LEATHER_HELM, NAM_LEATHER_HAT,
		0, 0,  0,	6, 1,  3,   8, 9, 0, LEATHER, HI_LEATHER),
HELM(NAM_ORCISH_HELM, NAM_IRON_SKULL_CAP,
		0, 0,  0,	6, 1, 30,  10, 9, 0, IRON, CLR_BLACK),
HELM(NAM_DWARVISH_IRON_HELM, NAM_HARD_HAT,
		0, 0,  0,	6, 1, 40,  20, 8, 0, IRON, HI_METAL),
HELM(NAM_FEDORA, NAM_SCHLAPPHUT, /* EN HELM(NAM_FEDORA, (char *)0, */
		0, 0,  0,	0, 0,  3,   1,10, 0, CLOTH, CLR_BROWN), /* EN 1, 0,  0,	0, 0,  3,   1,10, 0, CLOTH, CLR_BROWN), */
HELM(NAM_CORNUTHAUM, NAM_CONICAL_HAT,
		0, 1,  CLAIRVOYANT,
				3, 1,  4,  80,10, 2, CLOTH, CLR_BLUE),
HELM(NAM_DUNCE_CAP, NAM_CONICAL_HAT,
		0, 1,  0,	3, 1,  4,   1,10, 0, CLOTH, CLR_BLUE),
HELM(NAM_DENTED_POT, (char *)0,
		1, 0,  0,	2, 0, 10,   8, 9, 0, IRON, CLR_BLACK),
/* With shuffled appearances... */
HELM(NAM_HELMET, NAM_PLUMED_HELMET,
		0, 0,  0,      10, 1, 30,  10, 9, 0, IRON, HI_METAL),
HELM(NAM_HELM_OF_BRILLIANCE, NAM_ETCHED_HELMET,
		0, 1,  0,	6, 1, 50,  50, 9, 0, IRON, CLR_GREEN),
HELM(NAM_HELM_OF_OPPOSITE_ALIGNMENT, NAM_CRESTED_HELMET,
		0, 1,  0,	6, 1, 50,  50, 9, 0, IRON, HI_METAL),
HELM(NAM_HELM_OF_TELEPATHY, NAM_VISORED_HELMET,
		0, 1,  TELEPAT, 2, 1, 50,  50, 9, 0, IRON, HI_METAL),

/* suits of armor */
/*
 * There is code in polyself.c that assumes (1) and (2).
 * There is code in obj.h, objnam.c, mon.c, read.c that assumes (2).
 *
 *	(1) The dragon scale mails and the dragon scales are together.
 *	(2) That the order of the dragon scale mail and dragon scales is the
 *	    the same defined in monst.c.
 */
#define DRGN_ARMR(name,mgc,power,cost,ac,color) \
	ARMOR(name,(char *)0,1,mgc,1,power,0,5,40,cost,ac,0,ARM_SUIT,DRAGON_HIDE,color)
/* 3.4.1: dragon scale mail reclassified as "magic" since magic is
   needed to create them */
DRGN_ARMR(NAM_GRAY_DRAGON_SCALE_MAIL,   1, ANTIMAGIC,  1200, 1, CLR_GRAY),
DRGN_ARMR(NAM_SILVER_DRAGON_SCALE_MAIL, 1, REFLECTING, 1200, 1, DRAGON_SILVER),
#if 0	/* DEFERRED */
DRGN_ARMR(NAM_SHIMMERING_DRAGON_SCALE_MAIL, 1, DISPLACED, 1200, 1, CLR_CYAN),
#endif
DRGN_ARMR(NAM_RED_DRAGON_SCALE_MAIL,    1, FIRE_RES,    900, 1, CLR_RED),
DRGN_ARMR(NAM_WHITE_DRAGON_SCALE_MAIL,  1, COLD_RES,    900, 1, CLR_WHITE),
DRGN_ARMR(NAM_ORANGE_DRAGON_SCALE_MAIL, 1, SLEEP_RES,   900, 1, CLR_ORANGE),
DRGN_ARMR(NAM_BLACK_DRAGON_SCALE_MAIL,  1, DISINT_RES, 1200, 1, CLR_BLACK),
DRGN_ARMR(NAM_BLUE_DRAGON_SCALE_MAIL,   1, SHOCK_RES,   900, 1, CLR_BLUE),
DRGN_ARMR(NAM_GREEN_DRAGON_SCALE_MAIL,  1, POISON_RES,  900, 1, CLR_GREEN),
DRGN_ARMR(NAM_YELLOW_DRAGON_SCALE_MAIL, 1, ACID_RES,    900, 1, CLR_YELLOW),

/* For now, only dragons leave these. */
/* 3.4.1: dragon scales left classified as "non-magic"; they confer
   magical properties but are produced "naturally" */
DRGN_ARMR(NAM_GRAY_DRAGON_SCALES,   0, ANTIMAGIC,  700, 7, CLR_GRAY),
DRGN_ARMR(NAM_SILVER_DRAGON_SCALES, 0, REFLECTING, 700, 7, DRAGON_SILVER),
#if 0	/* DEFERRED */
DRGN_ARMR(NAM_SHIMMERING_DRAGON_SCALES, 0, DISPLACED,  700, 7, CLR_CYAN),
#endif
DRGN_ARMR(NAM_RED_DRAGON_SCALES,    0, FIRE_RES,   500, 7, CLR_RED),
DRGN_ARMR(NAM_WHITE_DRAGON_SCALES,  0, COLD_RES,   500, 7, CLR_WHITE),
DRGN_ARMR(NAM_ORANGE_DRAGON_SCALES, 0, SLEEP_RES,  500, 7, CLR_ORANGE),
DRGN_ARMR(NAM_BLACK_DRAGON_SCALES,  0, DISINT_RES, 700, 7, CLR_BLACK),
DRGN_ARMR(NAM_BLUE_DRAGON_SCALES,   0, SHOCK_RES,  500, 7, CLR_BLUE),
DRGN_ARMR(NAM_GREEN_DRAGON_SCALES,  0, POISON_RES, 500, 7, CLR_GREEN),
DRGN_ARMR(NAM_YELLOW_DRAGON_SCALES, 0, ACID_RES,   500, 7, CLR_YELLOW),
#undef DRGN_ARMR

ARMOR(NAM_PLATE_MAIL, (char *)0,
	1, 0, 1, 0,	44, 5, 450, 600,  3, 2, ARM_SUIT, IRON, HI_METAL),
ARMOR(NAM_CRYSTAL_PLATE_MAIL, (char *)0,
	1, 0, 1, 0,	10, 5, 450, 820,  3, 2, ARM_SUIT, GLASS, CLR_WHITE),
#ifdef TOURIST
ARMOR(NAM_BRONZE_PLATE_MAIL, (char *)0,
	1, 0, 1, 0,	25, 5, 450, 400,  4, 0, ARM_SUIT, COPPER, HI_COPPER),
#else
ARMOR(NAM_BRONZE_PLATE_MAIL, (char *)0,
	1, 0, 1, 0,	35, 5, 450, 400,  4, 0, ARM_SUIT, COPPER, HI_COPPER),
#endif
ARMOR(NAM_SPLINT_MAIL, (char *)0,
	1, 0, 1, 0,	62, 5, 400,  80,  4, 1, ARM_SUIT, IRON, HI_METAL),
ARMOR(NAM_BANDED_MAIL, (char *)0,
	1, 0, 1, 0,	72, 5, 350,  90,  4, 0, ARM_SUIT, IRON, HI_METAL),
ARMOR(NAM_DWARVISH_MITHRIL_COAT, (char *)0,
	1, 0, 0, 0,	10, 1, 150, 240,  4, 3, ARM_SUIT, MITHRIL, HI_METAL),
ARMOR(NAM_ELVEN_MITHRIL_COAT, (char *)0,
	1, 0, 0, 0,	15, 1, 150, 240,  5, 3, ARM_SUIT, MITHRIL, HI_METAL),
ARMOR(NAM_CHAIN_MAIL, (char *)0,
	1, 0, 0, 0,	72, 5, 300,  75,  5, 1, ARM_SUIT, IRON, HI_METAL),
ARMOR(NAM_ORCISH_CHAIN_MAIL, NAM_CRUDE_CHAIN_MAIL,
	0, 0, 0, 0,	20, 5, 300,  75,  6, 1, ARM_SUIT, IRON, CLR_BLACK),
ARMOR(NAM_SCALE_MAIL, (char *)0,
	1, 0, 0, 0,	72, 5, 250,  45,  6, 0, ARM_SUIT, IRON, HI_METAL),
ARMOR(NAM_STUDDED_LEATHER_ARMOR, (char *)0,
	1, 0, 0, 0,	72, 3, 200,  15,  7, 1, ARM_SUIT, LEATHER, HI_LEATHER),
ARMOR(NAM_RING_MAIL, (char *)0,
	1, 0, 0, 0,	72, 5, 250, 100,  7, 0, ARM_SUIT, IRON, HI_METAL),
ARMOR(NAM_ORCISH_RING_MAIL, NAM_CRUDE_RING_MAIL,
	0, 0, 0, 0,	20, 5, 250,  80,  8, 1, ARM_SUIT, IRON, CLR_BLACK),
ARMOR(NAM_LEATHER_ARMOR, (char *)0,
	1, 0, 0, 0,	82, 3, 150,   5,  8, 0, ARM_SUIT, LEATHER, HI_LEATHER),
ARMOR(NAM_LEATHER_JACKET, (char *)0,
	1, 0, 0, 0,	12, 0,	30,  10,  9, 0, ARM_SUIT, LEATHER, CLR_BLACK),

#ifdef TOURIST
/* shirts */
ARMOR(NAM_HAWAIIAN_SHIRT, (char *)0,
	1, 0, 0, 0,	 8, 0,	 5,   3, 10, 0, ARM_SHIRT, CLOTH, CLR_MAGENTA),
ARMOR(NAM_T_SHIRT, (char *)0,
	1, 0, 0, 0,	 2, 0,	 5,   2, 10, 0, ARM_SHIRT, CLOTH, CLR_WHITE),
#endif

/* cloaks */
/*  'cope' is not a spelling mistake... leave it be */
CLOAK(NAM_MUMMY_WRAPPING, (char *)0,
		1, 0,	0,	    0, 0,  3,  2, 10, 1, CLOTH, CLR_GRAY),
CLOAK(NAM_ELVEN_CLOAK, NAM_FADED_PALL,
		0, 1,	STEALTH,    8, 0, 10, 60,  9, 3, CLOTH, CLR_BLACK),
CLOAK(NAM_ORCISH_CLOAK, NAM_COARSE_MANTELET,
		0, 0,	0,	    8, 0, 10, 40, 10, 2, CLOTH, CLR_BLACK),
CLOAK(NAM_DWARVISH_CLOAK, NAM_HOODED_CLOAK,
		0, 0,	0,	    8, 0, 10, 50, 10, 2, CLOTH, HI_CLOTH),
CLOAK(NAM_OILSKIN_CLOAK, NAM_SLIPPERY_CLOAK,
		0, 0,	0,	    8, 0, 10, 50,  9, 3, CLOTH, HI_CLOTH),
CLOAK(NAM_ROBE, (char *)0,
		1, 1,	0,	    3, 0, 15, 50,  8, 3, CLOTH, CLR_RED),
CLOAK(NAM_ALCHEMY_SMOCK, NAM_APRON,
		0, 1,	POISON_RES, 9, 0, 10, 50,  9, 1, CLOTH, CLR_WHITE),
CLOAK(NAM_LEATHER_CLOAK, (char *)0,
		1, 0,	0,	    8, 0, 15, 40,  9, 1, LEATHER, CLR_BROWN),
/* With shuffled appearances... */
CLOAK(NAM_CLOAK_OF_PROTECTION, NAM_TATTERED_CAPE,
		0, 1,	PROTECTION, 9, 0, 10, 50,  7, 3, CLOTH, HI_CLOTH),
CLOAK(NAM_CLOAK_OF_INVISIBILITY, NAM_OPERA_CLOAK,
		0, 1,	INVIS,	   10, 0, 10, 60,  9, 2, CLOTH, CLR_BRIGHT_MAGENTA),
CLOAK(NAM_CLOAK_OF_MAGIC_RESISTANCE, NAM_ORNAMENTAL_COPE,
		0, 1,	ANTIMAGIC,  2, 0, 10, 60,  9, 3, CLOTH, CLR_WHITE),
CLOAK(NAM_CLOAK_OF_DISPLACEMENT, NAM_PIECE_OF_CLOTH,
		0, 1,	DISPLACED, 10, 0, 10, 50,  9, 2, CLOTH, HI_CLOTH),

/* shields */
SHIELD(NAM_SMALL_SHIELD, (char *)0,
		1, 0, 0, 0,	     6, 0, 30,	3,  9, 0, WOOD, HI_WOOD),
SHIELD(NAM_ELVEN_SHIELD, NAM_BLUE_AND_GREEN_SHIELD,
		0, 0, 0, 0,	     2, 0, 40,	7,  8, 0, WOOD, CLR_GREEN),
SHIELD(NAM_URUK_HAI_SHIELD, NAM_WHITE_HANDED_SHIELD,
		0, 0, 0, 0,	     2, 0, 50,	7,  9, 0, IRON, HI_METAL),
SHIELD(NAM_ORCISH_SHIELD, NAM_RED_EYED_SHIELD,
		0, 0, 0, 0,	     2, 0, 50,	7,  9, 0, IRON, CLR_RED),
SHIELD(NAM_LARGE_SHIELD, (char *)0,
		1, 0, 1, 0,	     7, 0,100, 10,  8, 0, IRON, HI_METAL),
SHIELD(NAM_DWARVISH_ROUNDSHIELD, NAM_LARGE_ROUND_SHIELD,
		0, 0, 0, 0,	     4, 0,100, 10,  8, 0, IRON, HI_METAL),
SHIELD(NAM_SHIELD_OF_REFLECTION, NAM_POLISHED_SILVER_SHIELD,
		0, 1, 0, REFLECTING, 3, 0, 50, 50,  8, 0, SILVER, HI_SILVER),

/* gloves */
/* these have their color but not material shuffled, so the IRON must stay
 * CLR_BROWN (== HI_LEATHER)
 */
GLOVES(NAM_LEATHER_GLOVES, NAM_OLD_GLOVES,
		0, 0,  0,	  16, 1, 10,  8,  9, 0, LEATHER, HI_LEATHER),
GLOVES(NAM_GAUNTLETS_OF_FUMBLING, NAM_PADDED_GLOVES,
		0, 1,  FUMBLING,   8, 1, 10, 50,  9, 0, LEATHER, HI_LEATHER),
GLOVES(NAM_GAUNTLETS_OF_POWER, NAM_RIDING_GLOVES,
		0, 1,  0,	   8, 1, 30, 50,  9, 0, IRON, CLR_BROWN),
GLOVES(NAM_GAUNTLETS_OF_DEXTERITY, NAM_FENCING_GLOVES,
		0, 1,  0,	   8, 1, 10, 50,  9, 0, LEATHER, HI_LEATHER),

/* boots */
BOOTS(NAM_LOW_BOOTS, NAM_WALKING_SHOES,
		0, 0,  0,	  25, 2, 10,  8,  9, 0, LEATHER, HI_LEATHER),
BOOTS(NAM_IRON_SHOES, NAM_HARD_SHOES,
		0, 0,  0,	   7, 2, 50, 16,  8, 0, IRON, HI_METAL),
BOOTS(NAM_HIGH_BOOTS, NAM_JACKBOOTS,
		0, 0,  0,	  15, 2, 20, 12,  8, 0, LEATHER, HI_LEATHER),
/* With shuffled appearances... */
BOOTS(NAM_SPEED_BOOTS, NAM_COMBAT_BOOTS,
		0, 1,  FAST,	  12, 2, 20, 50,  9, 0, LEATHER, HI_LEATHER),
BOOTS(NAM_WATER_WALKING_BOOTS, NAM_JUNGLE_BOOTS,
		0, 1,  WWALKING,  12, 2, 20, 50,  9, 0, LEATHER, HI_LEATHER),
BOOTS(NAM_JUMPING_BOOTS, NAM_HIKING_BOOTS,
		0, 1,  JUMPING,   12, 2, 20, 50,  9, 0, LEATHER, HI_LEATHER),
BOOTS(NAM_ELVEN_BOOTS, NAM_MUD_BOOTS,
		0, 1,  STEALTH,   12, 2, 15,  8,  9, 0, LEATHER, HI_LEATHER),
BOOTS(NAM_KICKING_BOOTS, NAM_BUCKLED_BOOTS,
		0, 1,  0,         12, 2, 15,  8,  9, 0, IRON, CLR_BROWN),
BOOTS(NAM_FUMBLE_BOOTS, NAM_RIDING_BOOTS,
		0, 1,  FUMBLING,  12, 2, 20, 30,  9, 0, LEATHER, HI_LEATHER),
BOOTS(NAM_LEVITATION_BOOTS, NAM_SNOW_BOOTS,
		0, 1,  LEVITATION,12, 2, 15, 30,  9, 0, LEATHER, HI_LEATHER),
#undef HELM
#undef CLOAK
#undef SHIELD
#undef GLOVES
#undef BOOTS
#undef ARMOR

/* rings ... */
#define RING(name,power,stone,cost,mgc,spec,mohs,metal,color) OBJECT( \
		OBJ(name,stone), \
		BITS(0,0,spec,0,mgc,spec,0,0,0,HARDGEM(mohs),0,P_NONE,metal), \
		power, RING_CLASS, 0, 0, 3, cost, 0, 0, 0, 0, 15, color )
RING(NAM_RING_ADORNMENT, ADORNED, NAM_RING_WOODEN,        100, 1, 1, 2, WOOD, HI_WOOD),
RING(NAM_RING_GAIN_STRENGTH, 0, NAM_RING_GRANITE,         150, 1, 1, 7, MINERAL, HI_MINERAL),
RING(NAM_RING_GAIN_CONSTITUTION, 0, NAM_RING_OPAL,        150, 1, 1, 7, MINERAL,  HI_MINERAL),
RING(NAM_RING_INCREASE_ACCURACY, 0, NAM_RING_CLAY,        150, 1, 1, 4, MINERAL, CLR_RED),
RING(NAM_RING_INCREASE_DAMAGE, 0, NAM_RING_CORAL,         150, 1, 1, 4, MINERAL, CLR_ORANGE),
RING(NAM_RING_PROTECTION, PROTECTION, NAM_RING_BLACK_ONYX,100, 1, 1, 7, MINERAL, CLR_BLACK),
RING(NAM_RING_REGENERATION, REGENERATION, NAM_RING_MOONSTONE,
					    200, 1, 0, 6, MINERAL, HI_MINERAL),
RING(NAM_RING_SEARCHING, SEARCHING, NAM_RING_TIGER_EYE,   200, 1, 0, 6, GEMSTONE, CLR_BROWN),
RING(NAM_RING_STEALTH, STEALTH, NAM_RING_JADE,            100, 1, 0, 6, GEMSTONE, CLR_GREEN),
RING(NAM_RING_SUSTAIN_ABILITY, FIXED_ABIL, NAM_RING_BRONZE,
					    100, 1, 0, 4, COPPER, HI_COPPER),
RING(NAM_RING_LEVITATION, LEVITATION, NAM_RING_AGATE,     200, 1, 0, 7, GEMSTONE, CLR_RED),
RING(NAM_RING_HUNGER, HUNGER, NAM_RING_TOPAZ,             100, 1, 0, 8, GEMSTONE, CLR_CYAN),
RING(NAM_RING_AGGRAVATE_MONSTER, AGGRAVATE_MONSTER, NAM_RING_SAPPHIRE,
					    150, 1, 0, 9, GEMSTONE, CLR_BLUE),
RING(NAM_RING_CONFLICT, CONFLICT, NAM_RING_RUBY,          300, 1, 0, 9, GEMSTONE, CLR_RED),
RING(NAM_RING_WARNING, WARNING, NAM_RING_DIAMOND,         100, 1, 0,10, GEMSTONE, CLR_WHITE),
RING(NAM_RING_POISON_RESISTANCE, POISON_RES, NAM_RING_PEARL,
					    150, 1, 0, 4, IRON, CLR_WHITE),
RING(NAM_RING_FIRE_RESISTANCE, FIRE_RES, NAM_RING_IRON,   200, 1, 0, 5, IRON, HI_METAL),
RING(NAM_RING_COLD_RESISTANCE, COLD_RES, NAM_RING_BRASS,  150, 1, 0, 4, COPPER, HI_COPPER),
RING(NAM_RING_SHOCK_RESISTANCE, SHOCK_RES, NAM_RING_COPPER,
					    150, 1, 0, 3, COPPER, HI_COPPER),
RING(NAM_RING_FREE_ACTION,     FREE_ACTION, NAM_RING_TWISTED,
					    200, 1, 0, 6, IRON, HI_METAL),
RING(NAM_RING_SLOW_DIGESTION,  SLOW_DIGESTION, NAM_RING_STEEL,
					    200, 1, 0, 8, IRON, HI_METAL),
RING(NAM_RING_TELEPORTATION, TELEPORT, NAM_RING_SILVER,   200, 1, 0, 3, SILVER, HI_SILVER),
RING(NAM_RING_TELEPORT_CONTROL, TELEPORT_CONTROL, NAM_RING_GOLD,
					    300, 1, 0, 3, GOLD, HI_GOLD),
RING(NAM_RING_POLYMORPH, POLYMORPH, NAM_RING_IVORY,       300, 1, 0, 4, BONE, CLR_WHITE),
RING(NAM_RING_POLYMORPH_CONTROL, POLYMORPH_CONTROL, NAM_RING_EMERALD,
					    300, 1, 0, 8, GEMSTONE, CLR_BRIGHT_GREEN),
RING(NAM_RING_INVISIBILITY, INVIS, NAM_RING_WIRE,         150, 1, 0, 5, IRON, HI_METAL),
RING(NAM_RING_SEE_INVISIBLE, SEE_INVIS, NAM_RING_ENGAGEMENT,
					    150, 1, 0, 5, IRON, HI_METAL),
RING(NAM_RING_PROTECTION_FROM_SHAPE_CHANGERS, PROT_FROM_SHAPE_CHANGERS, NAM_RING_SHINY,
					    100, 1, 0, 5, IRON, CLR_BRIGHT_CYAN),
#undef RING

/* amulets ... - THE Amulet comes last because it is special */
#define AMULET(name,desc,power,prob) OBJECT( \
		OBJ(name,desc), BITS(0,0,0,0,1,0,0,0,0,0,0,P_NONE,IRON), power, \
		AMULET_CLASS, prob, 0, 20, 150, 0, 0, 0, 0, 20, HI_METAL )

AMULET(NAM_AMULET_OF_ESP,           NAM_AMULET_CIRCULAR,   TELEPAT,    175),
AMULET(NAM_AMULET_OF_LIFE_SAVING,   NAM_AMULET_SPHERICAL,  LIFESAVED,   75),
AMULET(NAM_AMULET_OF_STRANGULATION, NAM_AMULET_OVAL,       STRANGLED,  135),
AMULET(NAM_AMULET_OF_RESTFUL_SLEEP, NAM_AMULET_TRIANGULAR, SLEEPING,   135),
AMULET(NAM_AMULET_VERSUS_POISON,    NAM_AMULET_PYRAMIDAL,  POISON_RES, 165),
AMULET(NAM_AMULET_OF_CHANGE,        NAM_AMULET_SQUARE,     0,          130),
						/* POLYMORPH */
AMULET(NAM_AMULET_OF_UNCHANGING,    NAM_AMULET_CONCAVE,    UNCHANGING,	 45),
AMULET(NAM_AMULET_OF_REFLECTION,    NAM_AMULET_HEXAGONAL,  REFLECTING,  75),
AMULET(NAM_AMULET_OF_MAGICAL_BREATHING, NAM_AMULET_OCTAGONAL,      MAGICAL_BREATHING, 65),
OBJECT(OBJ(NAM_FAKE_AMULET_OF_YENDOR,
	NAM_AMULET_OF_YENDOR), BITS(0,0,1,0,0,0,0,0,0,0,0,0,PLASTIC), 0,
	AMULET_CLASS, 0, 0, 20,    0, 0, 0, 0, 0,  1, HI_METAL),
OBJECT(OBJ(NAM_AMULET_OF_YENDOR,	/* note: description == name */
	NAM_AMULET_OF_YENDOR), BITS(0,0,1,0,1,0,1,1,0,0,0,0,MITHRIL), 0,
	AMULET_CLASS, 0, 0, 20, 30000, 0, 0, 0, 0, 20, HI_METAL),
#undef AMULET

/* tools ... */
/* tools with weapon characteristics come last */
#define TOOL(name,desc,kn,mrg,mgc,chg,prob,wt,cost,mat,color) \
	OBJECT( OBJ(name,desc), \
		BITS(kn,mrg,chg,0,mgc,chg,0,0,0,0,0,P_NONE,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, 0, 0, 0, 0, wt, color )
#define CONTAINER(name,desc,kn,mgc,chg,prob,wt,cost,mat,color) \
	OBJECT( OBJ(name,desc), \
		BITS(kn,0,chg,1,mgc,chg,0,0,0,0,0,P_NONE,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, 0, 0, 0, 0, wt, color )
#define WEPTOOL(name,desc,kn,mgc,bi,prob,wt,cost,sdam,ldam,hitbon,sub,mat,clr) \
	OBJECT( OBJ(name,desc), \
		BITS(kn,0,1,0,mgc,1,0,0,bi,0,hitbon,sub,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, 0, wt, clr )
/* containers */
CONTAINER(NAM_LARGE_BOX, (char *)0,       1, 0, 0,  40,350,   8, WOOD, HI_WOOD),
CONTAINER(NAM_CHEST, (char *)0,           1, 0, 0,  35,600,  16, WOOD, HI_WOOD),
CONTAINER(NAM_ICE_BOX, (char *)0,         1, 0, 0,   5,900,  42, PLASTIC, CLR_WHITE),
CONTAINER(NAM_SACK, NAM_BAG,                0, 0, 0,  35, 15,   2, CLOTH, HI_CLOTH),
CONTAINER(NAM_OILSKIN_SACK, NAM_BAG,        0, 0, 0,   5, 15, 100, CLOTH, HI_CLOTH),
CONTAINER(NAM_BAG_OF_HOLDING, NAM_BAG,      0, 1, 0,  20, 15, 100, CLOTH, HI_CLOTH),
CONTAINER(NAM_BAG_OF_TRICKS, NAM_BAG,       0, 1, 1,  20, 15, 100, CLOTH, HI_CLOTH),
#undef CONTAINER

/* lock opening tools */
TOOL(NAM_SKELETON_KEY, NAM_KEY,     0, 0, 0, 0,  80,  3,  10, IRON, HI_METAL),
#ifdef TOURIST
TOOL(NAM_LOCK_PICK, (char *)0,    1, 0, 0, 0,  60,  4,  20, IRON, HI_METAL),
TOOL(NAM_CREDIT_CARD, (char *)0,  1, 0, 0, 0,  15,  1,  10, PLASTIC, CLR_WHITE),
#else
TOOL(NAM_LOCK_PICK, (char *)0,    1, 0, 0, 0,  75,  4,  20, IRON, HI_METAL),
#endif
/* light sources */
TOOL(NAM_TALLOW_CANDLE, NAM_CANDLE, 0, 1, 0, 0,  20,  2,  10, WAX, CLR_WHITE),
TOOL(NAM_WAX_CANDLE, NAM_CANDLE,    0, 1, 0, 0,   5,  2,  20, WAX, CLR_WHITE),
TOOL(NAM_BRASS_LANTERN, (char *)0,1, 0, 0, 0,  30, 30,  12, COPPER, CLR_YELLOW),
TOOL(NAM_OIL_LAMP, NAM_LAMP,        0, 0, 0, 0,  45, 20,  10, COPPER, CLR_YELLOW),
TOOL(NAM_MAGIC_LAMP, NAM_LAMP,      0, 0, 1, 0,  15, 20,  50, COPPER, CLR_YELLOW),
/* other tools */
#ifdef TOURIST
TOOL(NAM_EXPENSIVE_CAMERA, (char *)0,
				1, 0, 0, 1,  15, 12, 200, PLASTIC, CLR_BLACK),
TOOL(NAM_MIRROR, NAM_LOOKING_GLASS, 0, 0, 0, 0,  45, 13,  10, GLASS, HI_SILVER),
#else
TOOL(NAM_MIRROR, NAM_LOOKING_GLASS, 0, 0, 0, 0,  60, 13,  10, GLASS, HI_SILVER),
#endif
TOOL(NAM_CRYSTAL_BALL, NAM_GLASS_ORB,
				0, 0, 1, 1,  15,150,  60, GLASS, HI_GLASS),
TOOL(NAM_LENSES, (char *)0,	1, 0, 0, 0,   5,  3,  80, GLASS, HI_GLASS),
TOOL(NAM_BLINDFOLD, (char *)0,    1, 0, 0, 0,  50,  2,  20, CLOTH, CLR_BLACK),
TOOL(NAM_TOWEL, (char *)0,        1, 0, 0, 0,  50,  2,  50, CLOTH, CLR_MAGENTA),
#ifdef STEED
TOOL(NAM_SADDLE, (char *)0,       1, 0, 0, 0,   5,200, 150, LEATHER, HI_LEATHER),
TOOL(NAM_LEASH, (char *)0,        1, 0, 0, 0,  65, 12,  20, LEATHER, HI_LEATHER),
#else
TOOL(NAM_LEASH, (char *)0,        1, 0, 0, 0,  70, 12,  20, LEATHER, HI_LEATHER),
#endif
TOOL(NAM_STETHOSCOPE, (char *)0,  1, 0, 0, 0,  25,  4,  75, IRON, HI_METAL),
TOOL(NAM_TINNING_KIT, (char *)0,  1, 0, 0, 1,  15,100,  30, IRON, HI_METAL),
TOOL(NAM_TIN_OPENER, (char *)0,   1, 0, 0, 0,  35,  4,  30, IRON, HI_METAL),
TOOL(NAM_CAN_OF_GREASE, (char *)0,1, 0, 0, 1,  15, 15,  20, IRON, HI_METAL),
TOOL(NAM_FIGURINE, (char *)0,     1, 0, 1, 0,  25, 50,  80, MINERAL, HI_MINERAL),
TOOL(NAM_MAGIC_MARKER, (char *)0, 1, 0, 1, 1,  15,  2,  50, PLASTIC, CLR_RED),
/* traps */
TOOL(NAM_LAND_MINE,(char *)0,     1, 0, 0, 0,   0,300, 180, IRON, CLR_RED),
TOOL(NAM_BEARTRAP, (char *)0,     1, 0, 0, 0,   0,200,  60, IRON, HI_METAL),
/* instruments */
TOOL(NAM_TIN_WHISTLE, NAM_WHISTLE,  0, 0, 0, 0, 100,  3,  10, METAL, HI_METAL),
TOOL(NAM_MAGIC_WHISTLE, NAM_WHISTLE,0, 0, 1, 0,  30,  3,  10, METAL, HI_METAL),
/* "If tin whistles are made out of tin, what do they make foghorns out of?" */
TOOL(NAM_WOODEN_FLUTE, NAM_FLUTE,   0, 0, 0, 0,   4,  5,  12, WOOD, HI_WOOD),
TOOL(NAM_MAGIC_FLUTE, NAM_FLUTE,    0, 0, 1, 1,   2,  5,  36, WOOD, HI_WOOD),
TOOL(NAM_TOOLED_HORN, NAM_HORN,     0, 0, 0, 0,   5, 18,  15, BONE, CLR_WHITE),
TOOL(NAM_FROST_HORN, NAM_HORN,      0, 0, 1, 1,   2, 18,  50, BONE, CLR_WHITE),
TOOL(NAM_FIRE_HORN, NAM_HORN,       0, 0, 1, 1,   2, 18,  50, BONE, CLR_WHITE),
TOOL(NAM_HORN_OF_PLENTY, NAM_HORN,  0, 0, 1, 1,   2, 18,  50, BONE, CLR_WHITE),
TOOL(NAM_WOODEN_HARP, NAM_HARP,     0, 0, 0, 0,   4, 30,  50, WOOD, HI_WOOD),
TOOL(NAM_MAGIC_HARP, NAM_HARP,      0, 0, 1, 1,   2, 30,  50, WOOD, HI_WOOD),
TOOL(NAM_BELL, (char *)0,         1, 0, 0, 0,   2, 30,  50, COPPER, HI_COPPER),
TOOL(NAM_BUGLE, (char *)0,        1, 0, 0, 0,   4, 10,  15, COPPER, HI_COPPER),
TOOL(NAM_LEATHER_DRUM, NAM_DRUM,    0, 0, 0, 0,   4, 25,  25, LEATHER, HI_LEATHER),
TOOL(NAM_DRUM_OF_EARTHQUAKE, NAM_DRUM,
				0, 0, 1, 1,   2, 25,  25, LEATHER, HI_LEATHER),
/* tools useful as weapons */
WEPTOOL(NAM_PICK_AXE, (char *)0,
	1, 0, 0, 20, 100,  50,	6,  3, WHACK,  P_PICK_AXE, IRON, HI_METAL),
WEPTOOL(NAM_GRAPPLING_HOOK, NAM_IRON_HOOK,
	0, 0, 0,  5,  30,  50,  2,  6, WHACK,  P_FLAIL, IRON, HI_METAL),
/* 3.4.1: unicorn horn left classified as "magic" */
WEPTOOL(NAM_UNICORN_HORN, (char *)0,
	1, 1, 1,  0,  20, 100, 12, 12, PIERCE, P_UNICORN_HORN, BONE, CLR_WHITE),

/* two special unique artifact "tools" */
OBJECT(OBJ(NAM_CANDELABRUM_OF_INVOCATION, NAM_CANDELABRUM),
		BITS(0,0,1,0,1,0,1,1,0,0,0,P_NONE,GOLD), 0,
		TOOL_CLASS, 0, 0,10, 5000, 0, 0, 0, 0, 200, HI_GOLD),
OBJECT(OBJ(NAM_BELL_OF_OPENING, NAM_SILVER_BELL),
		BITS(0,0,1,0,1,1,1,1,0,0,0,P_NONE,SILVER), 0,
		TOOL_CLASS, 0, 0,10, 5000, 0, 0, 0, 0, 50, HI_SILVER),
#undef TOOL
#undef WEPTOOL

/* Comestibles ... */
#define FOOD(name,prob,delay,wt,unk,tin,nutrition,color) OBJECT( \
		OBJ(name,(char *)0), BITS(1,1,unk,0,0,0,0,0,0,0,0,P_NONE,tin), 0, \
		FOOD_CLASS, prob, delay, \
		wt, nutrition/20 + 5, 0, 0, 0, 0, nutrition, color )
/* all types of food (except tins & corpses) must have a delay of at least 1. */
/* delay on corpses is computed and is weight dependant */
/* dog eats foods 0-4 but prefers tripe rations above all others */
/* fortune cookies can be read */
/* carrots improve your vision */
/* +0 tins contain monster meat */
/* +1 tins (of spinach) make you stronger (like Popeye) */
/* food CORPSE is a cadaver of some type */
/* meatballs/sticks/rings are only created from objects via stone to flesh */

/* meat */
FOOD(NAM_TRIPE_RATION,       140, 2, 10, 0, FLESH, 200, CLR_BROWN),
FOOD(NAM_CORPSE,               0, 1,  0, 0, FLESH,   0, CLR_BROWN),
FOOD(NAM_EGG,                 85, 1,  1, 1, FLESH,  80, CLR_WHITE),
FOOD(NAM_MEATBALL,             0, 1,  1, 0, FLESH,   5, CLR_BROWN),
FOOD(NAM_MEAT_STICK,           0, 1,  1, 0, FLESH,   5, CLR_BROWN),
FOOD(NAM_HUGE_CHUNK_OF_MEAT,   0,20,400, 0, FLESH,2000, CLR_BROWN),
/* special case because it's not mergable */
OBJECT(OBJ(NAM_MEAT_RING, (char *)0),
    BITS(1,0,0,0,0,0,0,0,0,0,0,0,FLESH),
    0, FOOD_CLASS, 0, 1, 5, 1, 0, 0, 0, 0, 5, CLR_BROWN),

/* fruits & veggies */
FOOD(NAM_KELP_FROND,           0, 1,  1, 0, VEGGY,  30, CLR_GREEN),
FOOD(NAM_EUCALYPTUS_LEAF,      3, 1,  1, 0, VEGGY,  30, CLR_GREEN),
FOOD(NAM_APPLE,               15, 1,  2, 0, VEGGY,  50, CLR_RED),
FOOD(NAM_ORANGE,              10, 1,  2, 0, VEGGY,  80, CLR_ORANGE),
FOOD(NAM_PEAR,                10, 1,  2, 0, VEGGY,  50, CLR_BRIGHT_GREEN),
FOOD(NAM_MELON,               10, 1,  5, 0, VEGGY, 100, CLR_BRIGHT_GREEN),
FOOD(NAM_BANANA,              10, 1,  2, 0, VEGGY,  80, CLR_YELLOW),
FOOD(NAM_CARROT,              15, 1,  2, 0, VEGGY,  50, CLR_ORANGE),
FOOD(NAM_SPRIG_OF_WOLFSBANE,   7, 1,  1, 0, VEGGY,  40, CLR_GREEN),
FOOD(NAM_CLOVE_OF_GARLIC,      7, 1,  1, 0, VEGGY,  40, CLR_WHITE),
FOOD(NAM_SLIME_MOLD,          75, 1,  5, 0, VEGGY, 250, HI_ORGANIC),

/* people food */
FOOD(NAM_LUMP_OF_ROYAL_JELLY,  0, 1,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD(NAM_CREAM_PIE,           25, 1, 10, 0, VEGGY, 100, CLR_WHITE),
FOOD(NAM_CANDY_BAR,           13, 1,  2, 0, VEGGY, 100, CLR_BROWN),
FOOD(NAM_FORTUNE_COOKIE,      55, 1,  1, 0, VEGGY,  40, CLR_YELLOW),
FOOD(NAM_PANCAKE,             25, 2,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD(NAM_LEMBAS_WAFER,        20, 2,  5, 0, VEGGY, 800, CLR_WHITE),
FOOD(NAM_CRAM_RATION,         20, 3, 15, 0, VEGGY, 600, HI_ORGANIC),
FOOD(NAM_FOOD_RATION,        380, 5, 20, 0, VEGGY, 800, HI_ORGANIC),
FOOD(NAM_K_RATION,             0, 1, 10, 0, VEGGY, 400, HI_ORGANIC),
FOOD(NAM_C_RATION,             0, 1, 10, 0, VEGGY, 300, HI_ORGANIC),
FOOD(NAM_TIN,                 75, 0, 10, 1, METAL,   0, HI_METAL),
#undef FOOD

/* potions ... */
#define POTION(name,desc,mgc,power,prob,cost,color) OBJECT( \
		OBJ(name,desc), BITS(0,1,0,0,mgc,0,0,0,0,0,0,P_NONE,GLASS), power, \
		POTION_CLASS, prob, 0, 20, cost, 0, 0, 0, 0, 10, color )
POTION(NAM_POT_GAIN_ABILITY, NAM_POT_RUBY,          1, 0,          42, 300, CLR_RED),
POTION(NAM_POT_RESTORE_ABILITY, NAM_POT_PINK,       1, 0,          40, 100, CLR_BRIGHT_MAGENTA),
POTION(NAM_POT_CONFUSION, NAM_POT_ORANGE,           1, CONFUSION,  42, 100, CLR_ORANGE),
POTION(NAM_POT_BLINDNESS, NAM_POT_YELLOW,           1, BLINDED,    40, 150, CLR_YELLOW),
POTION(NAM_POT_PARALYSIS, NAM_POT_EMERALD,          1, 0,          42, 300, CLR_BRIGHT_GREEN),
POTION(NAM_POT_SPEED, NAM_POT_DARK_GREEN,           1, FAST,       42, 200, CLR_GREEN),
POTION(NAM_POT_LEVITATION, NAM_POT_CYAN,            1, LEVITATION, 42, 200, CLR_CYAN),
POTION(NAM_POT_HALLUCINATION, NAM_POT_SKY_BLUE,     1, HALLUC,     40, 100, CLR_CYAN),
POTION(NAM_POT_INVISIBILITY, NAM_POT_BRILLIANT_BLUE,1, INVIS,      40, 150, CLR_BRIGHT_BLUE),
POTION(NAM_POT_SEE_INVISIBLE, NAM_POT_MAGENTA,      1, SEE_INVIS,  42,  50, CLR_MAGENTA),
POTION(NAM_POT_HEALING, NAM_POT_PURPLE_RED,         1, 0,          57, 100, CLR_MAGENTA),
POTION(NAM_POT_EXTRA_HEALING, NAM_POT_PUCE,         1, 0,          47, 100, CLR_RED),
POTION(NAM_POT_GAIN_LEVEL, NAM_POT_MILKY,           1, 0,          20, 300, CLR_WHITE),
POTION(NAM_POT_ENLIGHTENMENT, NAM_POT_SWIRLY,       1, 0,          20, 200, CLR_BROWN),
POTION(NAM_POT_MONSTER_DETECTION, NAM_POT_BUBBLY,   1, 0,          40, 150, CLR_WHITE),
POTION(NAM_POT_OBJECT_DETECTION, NAM_POT_SMOKY,     1, 0,          42, 150, CLR_GRAY),
POTION(NAM_POT_GAIN_ENERGY, NAM_POT_CLOUDY,         1, 0,          42, 150, CLR_WHITE),
POTION(NAM_POT_SLEEPING,  NAM_POT_EFFERVESCENT,     1, 0,          42, 100, CLR_GRAY),
POTION(NAM_POT_FULL_HEALING,  NAM_POT_BLACK,        1, 0,          10, 200, CLR_BLACK),
POTION(NAM_POT_POLYMORPH, NAM_POT_GOLDEN,           1, 0,          10, 200, CLR_YELLOW),
POTION(NAM_POT_BOOZE, NAM_POT_BROWN,                0, 0,          42,  50, CLR_BROWN),
POTION(NAM_POT_SICKNESS, NAM_POT_FIZZY,             0, 0,          42,  50, CLR_CYAN),
POTION(NAM_POT_FRUIT_JUICE, NAM_POT_DARK,           0, 0,          42,  50, CLR_BLACK),
POTION(NAM_POT_ACID, NAM_POT_WHITE,                 0, 0,          10, 250, CLR_WHITE),
POTION(NAM_POT_OIL, NAM_POT_MURKY,                  0, 0,          30, 250, CLR_BROWN),
POTION(NAM_POT_WATER, NAM_POT_CLEAR,                0, 0,          92, 100, CLR_CYAN),
#undef POTION

/* scrolls ... */
#define SCROLL(name,text,mgc,prob,cost) OBJECT( \
		OBJ(name,text), BITS(0,1,0,0,mgc,0,0,0,0,0,0,P_NONE,PAPER), 0, \
		SCROLL_CLASS, prob, 0, 5, cost, 0, 0, 0, 0, 6, HI_PAPER )
	SCROLL(NAM_SCR_ENCHANT_ARMOR,         NAM_SCR_ZELGO_MER,            1,  63,  80),
	SCROLL(NAM_SCR_DESTROY_ARMOR,         NAM_SCR_JUYED_AWK_YACC,       1,  45, 100),
	SCROLL(NAM_SCR_CONFUSE_MONSTER,       NAM_SCR_NR_9,                 1,  53, 100),
	SCROLL(NAM_SCR_SCARE_MONSTER,         NAM_SCR_XIXAXA_XOXAXA_XUXAXA, 1,  35, 100),
	SCROLL(NAM_SCR_REMOVE_CURSE,          NAM_SCR_PRATYAVAYAH,          1,  65,  80),
	SCROLL(NAM_SCR_ENCHANT_WEAPON,        NAM_SCR_DAIYEN_FOOELS,        1,  80,  60),
	SCROLL(NAM_SCR_CREATE_MONSTER,        NAM_SCR_LEP_GEX_VEN_ZEA,      1,  45, 200),
	SCROLL(NAM_SCR_TAMING,                NAM_SCR_PRIRUTSENIE,          1,  15, 200),
	SCROLL(NAM_SCR_GENOCIDE,              NAM_SCR_ELBIB_YLOH,           1,  15, 300),
	SCROLL(NAM_SCR_LIGHT,                 NAM_SCR_VERR_YED_HORRE,       1,  90,  50),
	SCROLL(NAM_SCR_TELEPORTATION,         NAM_SCR_VENZAR_BORGAVVE,      1,  55, 100),
	SCROLL(NAM_SCR_GOLD_DETECTION,        NAM_SCR_THARR,                1,  33, 100),
	SCROLL(NAM_SCR_FOOD_DETECTION,        NAM_SCR_YUM_YUM,              1,  25, 100),
	SCROLL(NAM_SCR_IDENTIFY,              NAM_SCR_KERNOD_WEL,           1, 180,  20),
	SCROLL(NAM_SCR_MAGIC_MAPPING,         NAM_SCR_ELAM_EBOW,            1,  45, 100),
	SCROLL(NAM_SCR_AMNESIA,               NAM_SCR_DUAM_XNAHT,           1,  35, 200),
	SCROLL(NAM_SCR_FIRE,                  NAM_SCR_ANDOVA_BEGARIN,       1,  30, 100),
	SCROLL(NAM_SCR_EARTH,                 NAM_SCR_KIRJE,                1,  18, 200),
	SCROLL(NAM_SCR_PUNISHMENT,            NAM_SCR_VE_FORBRYDERNE,       1,  15, 300),
	SCROLL(NAM_SCR_CHARGING,              NAM_SCR_HACKEM_MUCHE,         1,  15, 300),
	SCROLL(NAM_SCR_STINKING_CLOUD,	NAM_SCR_VELOX_NEB,            1,  15, 300),
	SCROLL((char *)0,		NAM_SCR_FOOBIE_BLETCH,        1,   0, 100),
	SCROLL((char *)0,		NAM_SCR_TEMOV,                1,   0, 100),
	SCROLL((char *)0,		NAM_SCR_GARVEN_DEH,           1,   0, 100),
	SCROLL((char *)0,		NAM_SCR_READ_ME,              1,   0, 100),
	/* these must come last because they have special descriptions */
#ifdef MAIL
	SCROLL(NAM_SCR_MAIL,                  NAM_SCR_STAMPED,          0,   0,   0),
#endif
	SCROLL(NAM_SCR_BLANK_PAPER,           NAM_SCR_UNLABELED,        0,  28,  60),
#undef SCROLL

/* spellbooks ... */
#define SPELL(name,desc,sub,prob,delay,level,mgc,dir,color) OBJECT( \
		OBJ(name,desc), BITS(0,0,0,0,mgc,0,0,0,0,0,dir,sub,PAPER), 0, \
		SPBOOK_CLASS, prob, delay, \
		50, level*100, 0, 0, 0, level, 20, color )
SPELL(NAM_SPE_DIG,             NAM_SPE_PARCHMENT,   P_MATTER_SPELL, 20,  6, 5, 1, RAY,       HI_PAPER),
SPELL(NAM_SPE_MAGIC_MISSILE,   NAM_SPE_VELLUM,      P_ATTACK_SPELL, 45,  2, 2, 1, RAY,       HI_PAPER),
SPELL(NAM_SPE_FIREBALL,        NAM_SPE_RAGGED,      P_ATTACK_SPELL, 20,  4, 4, 1, RAY,       HI_PAPER),
SPELL(NAM_SPE_CONE_OF_COLD,    NAM_SPE_DOG_EARED,   P_ATTACK_SPELL, 10,  7, 4, 1, RAY,       HI_PAPER),
SPELL(NAM_SPE_SLEEP,           NAM_SPE_MOTTLED,     P_ENCHANTMENT_SPELL, 50,  1, 1, 1, RAY,       HI_PAPER),
SPELL(NAM_SPE_FINGER_OF_DEATH, NAM_SPE_STAINED,     P_ATTACK_SPELL,  5, 10, 7, 1, RAY,       HI_PAPER),
SPELL(NAM_SPE_LIGHT,           NAM_SPE_CLOTH,       P_DIVINATION_SPELL, 45,  1, 1, 1, NODIR,     HI_CLOTH),
SPELL(NAM_SPE_DETECT_MONSTERS, NAM_SPE_LEATHER,     P_DIVINATION_SPELL, 43,  1, 1, 1, NODIR,     HI_LEATHER),
SPELL(NAM_SPE_HEALING,         NAM_SPE_WHITE,       P_HEALING_SPELL, 40,  2, 1, 1, IMMEDIATE, CLR_WHITE),
SPELL(NAM_SPE_KNOCK,           NAM_SPE_PINK,        P_MATTER_SPELL, 35,  1, 1, 1, IMMEDIATE, CLR_BRIGHT_MAGENTA),
SPELL(NAM_SPE_FORCE_BOLT,      NAM_SPE_RED,         P_ATTACK_SPELL, 35,  2, 1, 1, IMMEDIATE, CLR_RED),
SPELL(NAM_SPE_CONFUSE_MONSTER, NAM_SPE_ORANGE,      P_ENCHANTMENT_SPELL, 30,  2, 2, 1, IMMEDIATE, CLR_ORANGE),
SPELL(NAM_SPE_CURE_BLINDNESS,  NAM_SPE_YELLOW,      P_HEALING_SPELL, 25,  2, 2, 1, IMMEDIATE, CLR_YELLOW),
SPELL(NAM_SPE_DRAIN_LIFE,      NAM_SPE_VELVET,      P_ATTACK_SPELL, 10,  2, 2, 1, IMMEDIATE, CLR_MAGENTA),
SPELL(NAM_SPE_SLOW_MONSTER,    NAM_SPE_LIGHT_GREEN, P_ENCHANTMENT_SPELL, 30,  2, 2, 1, IMMEDIATE, CLR_BRIGHT_GREEN),
SPELL(NAM_SPE_WIZARD_LOCK,     NAM_SPE_DARK_GREEN,  P_MATTER_SPELL, 30,  3, 2, 1, IMMEDIATE, CLR_GREEN),
SPELL(NAM_SPE_CREATE_MONSTER,  NAM_SPE_TURQUOISE,   P_CLERIC_SPELL, 35,  3, 2, 1, NODIR,     CLR_BRIGHT_CYAN),
SPELL(NAM_SPE_DETECT_FOOD,     NAM_SPE_CYAN,        P_DIVINATION_SPELL, 30,  3, 2, 1, NODIR,     CLR_CYAN),
SPELL(NAM_SPE_CAUSE_FEAR,      NAM_SPE_LIGHT_BLUE,  P_ENCHANTMENT_SPELL, 25,  3, 3, 1, NODIR,     CLR_BRIGHT_BLUE),
SPELL(NAM_SPE_CLAIRVOYANCE,    NAM_SPE_DARK_BLUE,   P_DIVINATION_SPELL, 15,  3, 3, 1, NODIR,     CLR_BLUE),
SPELL(NAM_SPE_CURE_SICKNESS,   NAM_SPE_INDIGO,      P_HEALING_SPELL, 32,  3, 3, 1, NODIR,     CLR_BLUE),
SPELL(NAM_SPE_CHARM_MONSTER,   NAM_SPE_MAGENTA,     P_ENCHANTMENT_SPELL, 20,  3, 3, 1, IMMEDIATE, CLR_MAGENTA),
SPELL(NAM_SPE_HASTE_SELF,      NAM_SPE_PURPLE,      P_ESCAPE_SPELL, 33,  4, 3, 1, NODIR,     CLR_MAGENTA),
SPELL(NAM_SPE_DETECT_UNSEEN,   NAM_SPE_VIOLET,      P_DIVINATION_SPELL, 20,  4, 3, 1, NODIR,     CLR_MAGENTA),
SPELL(NAM_SPE_LEVITATION,      NAM_SPE_TAN,         P_ESCAPE_SPELL, 20,  4, 4, 1, NODIR,     CLR_BROWN),
SPELL(NAM_SPE_EXTRA_HEALING,   NAM_SPE_PLAID,       P_HEALING_SPELL, 27,  5, 3, 1, IMMEDIATE, CLR_GREEN),
SPELL(NAM_SPE_RESTORE_ABILITY, NAM_SPE_LIGHT_BROWN, P_HEALING_SPELL, 25,  5, 4, 1, NODIR,     CLR_BROWN),
SPELL(NAM_SPE_INVISIBILITY,    NAM_SPE_DARK_BROWN,  P_ESCAPE_SPELL, 25,  5, 4, 1, NODIR,     CLR_BROWN),
SPELL(NAM_SPE_DETECT_TREASURE, NAM_SPE_GRAY,        P_DIVINATION_SPELL, 20,  5, 4, 1, NODIR,     CLR_GRAY),
SPELL(NAM_SPE_REMOVE_CURSE,    NAM_SPE_WRINKLED,    P_CLERIC_SPELL, 25,  5, 3, 1, NODIR,     HI_PAPER),
SPELL(NAM_SPE_MAGIC_MAPPING,   NAM_SPE_DUSTY,       P_DIVINATION_SPELL, 18,  7, 5, 1, NODIR,     HI_PAPER),
SPELL(NAM_SPE_IDENTIFY,        NAM_SPE_BRONZE,      P_DIVINATION_SPELL, 20,  6, 3, 1, NODIR,     HI_COPPER),
SPELL(NAM_SPE_TURN_UNDEAD,     NAM_SPE_COPPER,      P_CLERIC_SPELL, 16,  8, 6, 1, IMMEDIATE, HI_COPPER),
SPELL(NAM_SPE_POLYMORPH,       NAM_SPE_SILVER,      P_MATTER_SPELL, 10,  8, 6, 1, IMMEDIATE, HI_SILVER),
SPELL(NAM_SPE_TELEPORT_AWAY,   NAM_SPE_GOLD,        P_ESCAPE_SPELL, 15,  6, 6, 1, IMMEDIATE, HI_GOLD),
SPELL(NAM_SPE_CREATE_FAMILIAR, NAM_SPE_GLITTERING,  P_CLERIC_SPELL, 10,  7, 6, 1, NODIR,     CLR_WHITE),
SPELL(NAM_SPE_CANCELLATION,    NAM_SPE_SHINING,     P_MATTER_SPELL, 15,  8, 7, 1, IMMEDIATE, CLR_WHITE),
SPELL(NAM_SPE_PROTECTION,	     NAM_SPE_DULL,        P_CLERIC_SPELL, 18,  3, 1, 1, NODIR,     HI_PAPER),
SPELL(NAM_SPE_JUMPING,	     NAM_SPE_THIN,        P_ESCAPE_SPELL, 20,  3, 1, 1, IMMEDIATE, HI_PAPER),
SPELL(NAM_SPE_STONE_TO_FLESH,	 NAM_SPE_THICK,       P_HEALING_SPELL, 15,  1, 3, 1, IMMEDIATE, HI_PAPER),
#if 0	/* DEFERRED */
SPELL(NAM_SPE_FLAME_SPHERE,    NAM_SPE_CANVAS,      P_MATTER_SPELL, 20,  2, 1, 1, NODIR, CLR_BROWN),
SPELL(NAM_SPE_FREEZE_SPHERE,   NAM_SPE_HARDCOVER,   P_MATTER_SPELL, 20,  2, 1, 1, NODIR, CLR_BROWN),
#endif
/* blank spellbook must come last because it retains its description */
SPELL(NAM_SPE_BLANK_PAPER,     NAM_SPE_PLAIN,       P_NONE, 18,  0, 0, 0, 0,         HI_PAPER),
/* a special, one of a kind, spellbook */
OBJECT(OBJ(NAM_SPE_BOOK_OF_THE_DEAD, NAM_SPE_PAPYRUS), BITS(0,0,1,0,1,0,1,1,0,0,0,P_NONE,PAPER), 0,
	SPBOOK_CLASS, 0, 0,20, 10000, 0, 0, 0, 7, 20, HI_PAPER),
#undef SPELL

/* wands ... */
#define WAND(name,typ,prob,cost,mgc,dir,metal,color) OBJECT( \
		OBJ(name,typ), BITS(0,0,1,0,mgc,1,0,0,0,0,dir,P_NONE,metal), 0, \
		WAND_CLASS, prob, 0, 7, cost, 0, 0, 0, 0, 30, color )
WAND(NAM_WAND_LIGHT,          NAM_WAND_GLASS,    95, 100, 1, NODIR,     GLASS,    HI_GLASS),
WAND(NAM_WAND_SECRET_DOOR_DETECTION, NAM_WAND_BALSA,
				   50, 150, 1, NODIR,	  WOOD,     HI_WOOD),
WAND(NAM_WAND_ENLIGHTENMENT,  NAM_WAND_CRYSTAL,  15, 150, 1, NODIR,     GLASS,    HI_GLASS),
WAND(NAM_WAND_CREATE_MONSTER, NAM_WAND_MAPLE,    45, 200, 1, NODIR,     WOOD,     HI_WOOD),
WAND(NAM_WAND_WISHING,        NAM_WAND_PINE,      5, 500, 1, NODIR,     WOOD,     HI_WOOD),
WAND(NAM_WAND_NOTHING,        NAM_WAND_OAK,      25, 100, 0, IMMEDIATE, WOOD,     HI_WOOD),
WAND(NAM_WAND_STRIKING,       NAM_WAND_EBONY,    75, 150, 1, IMMEDIATE, WOOD,     HI_WOOD),
WAND(NAM_WAND_MAKE_INVISIBLE, NAM_WAND_MARBLE,   45, 150, 1, IMMEDIATE, MINERAL,  HI_MINERAL),
WAND(NAM_WAND_SLOW_MONSTER,   NAM_WAND_TIN,      50, 150, 1, IMMEDIATE, METAL,    HI_METAL),
WAND(NAM_WAND_SPEED_MONSTER,  NAM_WAND_BRASS,    50, 150, 1, IMMEDIATE, COPPER,   HI_COPPER),
WAND(NAM_WAND_UNDEAD_TURNING, NAM_WAND_COPPER,   50, 150, 1, IMMEDIATE, COPPER,   HI_COPPER),
WAND(NAM_WAND_POLYMORPH,      NAM_WAND_SILVER,   45, 200, 1, IMMEDIATE, SILVER,   HI_SILVER),
WAND(NAM_WAND_CANCELLATION,   NAM_WAND_PLATINUM, 45, 200, 1, IMMEDIATE, PLATINUM, CLR_WHITE),
WAND(NAM_WAND_TELEPORTATION,  NAM_WAND_IRIDIUM,  45, 200, 1, IMMEDIATE, METAL,    CLR_BRIGHT_CYAN),
WAND(NAM_WAND_OPENING,        NAM_WAND_ZINC,     25, 150, 1, IMMEDIATE, METAL,    HI_METAL),
WAND(NAM_WAND_LOCKING,        NAM_WAND_ALUMINUM, 25, 150, 1, IMMEDIATE, METAL,    HI_METAL),
WAND(NAM_WAND_PROBING,        NAM_WAND_URANIUM,  30, 150, 1, IMMEDIATE, METAL,    HI_METAL),
WAND(NAM_WAND_DIGGING,        NAM_WAND_IRON,     55, 150, 1, RAY,       IRON,     HI_METAL),
WAND(NAM_WAND_MAGIC_MISSILE,  NAM_WAND_STEEL,    50, 150, 1, RAY,       IRON,     HI_METAL),
WAND(NAM_WAND_FIRE,           NAM_WAND_HEXAGONAL,40, 175, 1, RAY,       IRON,     HI_METAL),
WAND(NAM_WAND_COLD,           NAM_WAND_SHORT,    40, 175, 1, RAY,       IRON,     HI_METAL),
WAND(NAM_WAND_SLEEP,          NAM_WAND_RUNED,    50, 175, 1, RAY,       IRON,     HI_METAL),
WAND(NAM_WAND_DEATH,          NAM_WAND_LONG,      5, 500, 1, RAY,       IRON,     HI_METAL),
WAND(NAM_WAND_LIGHTNING,      NAM_WAND_CURVED,   40, 175, 1, RAY,       IRON,     HI_METAL),
WAND((char *)0,        NAM_WAND_FORKED,    0, 150, 1, 0,         WOOD,     HI_WOOD),
WAND((char *)0,        NAM_WAND_SPIKED,    0, 150, 1, 0,         IRON,     HI_METAL),
WAND((char *)0,        NAM_WAND_JEWELED,   0, 150, 1, 0,         IRON,     HI_MINERAL),
#undef WAND

/* coins ... - so far, gold is all there is */
#define COIN(name,prob,metal,worth) OBJECT( \
		OBJ(name,(char *)0), BITS(0,1,0,0,0,0,0,0,0,0,0,P_NONE,metal), 0, \
		COIN_CLASS, prob, 0, 1, worth, 0, 0, 0, 0, 0, HI_GOLD )
	COIN(NAM_GOLD_PIECE,      1000, GOLD,1),
#undef COIN

/* gems ... - includes stones and rocks but not boulders */
#define GEM(name,desc,prob,wt,gval,nutr,mohs,glass,color) OBJECT( \
	    OBJ(name,desc), \
	    BITS(0,1,0,0,0,0,0,0,0,HARDGEM(mohs),0,-P_SLING,glass), 0, \
	    GEM_CLASS, prob, 0, 1, gval, 3, 3, 0, 0, nutr, color )
#define ROCK(name,desc,kn,prob,wt,gval,sdam,ldam,mgc,nutr,mohs,glass,color) OBJECT( \
	    OBJ(name,desc), \
	    BITS(kn,1,0,0,mgc,0,0,0,0,HARDGEM(mohs),0,-P_SLING,glass), 0, \
	    GEM_CLASS, prob, 0, wt, gval, sdam, ldam, 0, 0, nutr, color )
GEM(NAM_GEM_DILITHIUM_CRYSTAL, NAM_GEM_WHITE,      2,  1, 4500, 15,  5, GEMSTONE, CLR_WHITE),
GEM(NAM_GEM_DIAMOND, NAM_GEM_WHITE,                3,  1, 4000, 15, 10, GEMSTONE, CLR_WHITE),
GEM(NAM_GEM_RUBY, NAM_GEM_RED,                     4,  1, 3500, 15,  9, GEMSTONE, CLR_RED),
GEM(NAM_GEM_JACINTH, NAM_GEM_ORANGE,               3,  1, 3250, 15,  9, GEMSTONE, CLR_ORANGE),
GEM(NAM_GEM_SAPPHIRE, NAM_GEM_BLUE,                4,  1, 3000, 15,  9, GEMSTONE, CLR_BLUE),
GEM(NAM_GEM_BLACK_OPAL, NAM_GEM_BLACK,             3,  1, 2500, 15,  8, GEMSTONE, CLR_BLACK),
GEM(NAM_GEM_EMERALD, NAM_GEM_GREEN,                5,  1, 2500, 15,  8, GEMSTONE, CLR_GREEN),
GEM(NAM_GEM_TURQUOISE, NAM_GEM_GREEN,              6,  1, 2000, 15,  6, GEMSTONE, CLR_GREEN),
GEM(NAM_GEM_CITRINE, NAM_GEM_YELLOW,               4,  1, 1500, 15,  6, GEMSTONE, CLR_YELLOW),
GEM(NAM_GEM_AQUAMARINE, NAM_GEM_GREEN,             6,  1, 1500, 15,  8, GEMSTONE, CLR_GREEN),
GEM(NAM_GEM_AMBER, NAM_GEM_YELLOWISH_BROWN,        8,  1, 1000, 15,  2, GEMSTONE, CLR_BROWN),
GEM(NAM_GEM_TOPAZ, NAM_GEM_YELLOWISH_BROWN,       10,  1,  900, 15,  8, GEMSTONE, CLR_BROWN),
GEM(NAM_GEM_JET, NAM_GEM_BLACK,                    6,  1,  850, 15,  7, GEMSTONE, CLR_BLACK),
GEM(NAM_GEM_OPAL, NAM_GEM_WHITE,                  12,  1,  800, 15,  6, GEMSTONE, CLR_WHITE),
GEM(NAM_GEM_CHRYSOBERYL, NAM_GEM_YELLOW,           8,  1,  700, 15,  5, GEMSTONE, CLR_YELLOW),
GEM(NAM_GEM_GARNET, NAM_GEM_RED,                  12,  1,  700, 15,  7, GEMSTONE, CLR_RED),
GEM(NAM_GEM_AMETHYST, NAM_GEM_VIOLET,             14,  1,  600, 15,  7, GEMSTONE, CLR_MAGENTA),
GEM(NAM_GEM_JASPER, NAM_GEM_RED,                  15,  1,  500, 15,  7, GEMSTONE, CLR_RED),
GEM(NAM_GEM_FLUORITE, NAM_GEM_VIOLET,             15,  1,  400, 15,  4, GEMSTONE, CLR_MAGENTA),
GEM(NAM_GEM_OBSIDIAN, NAM_GEM_BLACK,               9,  1,  200, 15,  6, GEMSTONE, CLR_BLACK),
GEM(NAM_GEM_AGATE, NAM_GEM_ORANGE,                12,  1,  200, 15,  6, GEMSTONE, CLR_ORANGE),
GEM(NAM_GEM_JADE, NAM_GEM_GREEN,                  10,  1,  300, 15,  6, GEMSTONE, CLR_GREEN),
GEM(NAM_GEM_WHITE_GLASS, NAM_GEM_WHITE,   77, 1, 0, 6, 5, GLASS, CLR_WHITE),
GEM(NAM_GEM_BLUE_GLASS, NAM_GEM_BLUE,     77, 1, 0, 6, 5, GLASS, CLR_BLUE),
GEM(NAM_GEM_RED_GLASS, NAM_GEM_RED,       77, 1, 0, 6, 5, GLASS, CLR_RED),
GEM(NAM_GEM_YELLOWISH_BROWN_GLASS, NAM_GEM_YELLOWISH_BROWN,
						 77, 1, 0, 6, 5, GLASS, CLR_BROWN),
GEM(NAM_GEM_ORANGE_GLASS, NAM_GEM_ORANGE, 76, 1, 0, 6, 5, GLASS, CLR_ORANGE),
GEM(NAM_GEM_YELLOW_GLASS, NAM_GEM_YELLOW, 77, 1, 0, 6, 5, GLASS, CLR_YELLOW),
GEM(NAM_GEM_BLACK_GLASS,  NAM_GEM_BLACK,  76, 1, 0, 6, 5, GLASS, CLR_BLACK),
GEM(NAM_GEM_GREEN_GLASS, NAM_GEM_GREEN,   77, 1, 0, 6, 5, GLASS, CLR_GREEN),
GEM(NAM_GEM_VIOLET_GLASS, NAM_GEM_VIOLET, 77, 1, 0, 6, 5, GLASS, CLR_MAGENTA),

/* Placement note: there is a wishable subrange for
 * "gray stones" in the o_ranges[] array in objnam.c
 * that is currently everything between luckstones and flint (inclusive).
 */
ROCK(NAM_GEM_LUCKSTONE, NAM_GEM_GRAY,	0, 10,  10, 60, 3, 3, 1, 10, 7, MINERAL, CLR_GRAY),
ROCK(NAM_GEM_LOADSTONE, NAM_GEM_GRAY,	0, 10, 500,  1, 3, 3, 1, 10, 6, MINERAL, CLR_GRAY),
ROCK(NAM_GEM_TOUCHSTONE, NAM_GEM_GRAY,	0,  8,  10, 45, 3, 3, 1, 10, 6, MINERAL, CLR_GRAY),
ROCK(NAM_GEM_FLINT, NAM_GEM_GRAY,		0, 10,  10,  1, 6, 6, 0, 10, 7, MINERAL, CLR_GRAY),
ROCK(NAM_GEM_ROCK, (char *)0,		1,100,  10,  0, 3, 3, 0, 10, 7, MINERAL, CLR_GRAY),
#undef GEM
#undef ROCK

/* miscellaneous ... */
/* Note: boulders and rocks are not normally created at random; the
 * probabilities only come into effect when you try to polymorph them.
 * Boulders weigh more than MAX_CARR_CAP; statues use corpsenm to take
 * on a specific type and may act as containers (both affect weight).
 */
OBJECT(OBJ(NAM_BOULDER,(char *)0), BITS(1,0,0,0,0,0,0,0,1,0,0,P_NONE,MINERAL), 0,
		ROCK_CLASS,   100, 0, 6000,  0, 20, 20, 0, 0, 2000, HI_MINERAL),
OBJECT(OBJ(NAM_STATUE, (char *)0), BITS(1,0,0,1,0,0,0,0,0,0,0,P_NONE,MINERAL), 0,
		ROCK_CLASS,   900, 0, 2500,  0, 20, 20, 0, 0, 2500, CLR_WHITE),

OBJECT(OBJ(NAM_HEAVY_IRON_BALL, (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_NONE,IRON), 0,
		BALL_CLASS,  1000, 0,  480, 10, 25, 25, 0, 0,  200, HI_METAL),
						/* +d4 when "very heavy" */
OBJECT(OBJ(NAM_IRON_CHAIN, (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_NONE,IRON), 0,
		CHAIN_CLASS, 1000, 0,  120,  0,  4,  4, 0, 0,  200, HI_METAL),
						/* +1 both l & s */

OBJECT(OBJ(NAM_BLINDING_VENOM, NAM_SPLASH_OF_VENOM),
		BITS(0,1,0,0,0,0,0,1,0,0,0,P_NONE,LIQUID), 0,
		VENOM_CLASS,  500, 0,	 1,  0,  0,  0, 0, 0,	 0, HI_ORGANIC),
OBJECT(OBJ(NAM_ACID_VENOM, NAM_SPLASH_OF_VENOM),
		BITS(0,1,0,0,0,0,0,1,0,0,0,P_NONE,LIQUID), 0,
		VENOM_CLASS,  500, 0,	 1,  0,  6,  6, 0, 0,	 0, HI_ORGANIC),
		/* +d6 small or large */

/* fencepost, the deadly Array Terminator -- name [1st arg] *must* be NULL */
	OBJECT(OBJ((char *)0,(char *)0), BITS(0,0,0,0,0,0,0,0,0,0,0,P_NONE,0), 0,
		ILLOBJ_CLASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
}; /* objects[] */

#ifndef OBJECTS_PASS_2_

/* perform recursive compilation for second structure */
#  undef OBJ
#  undef OBJECT
#  define OBJECTS_PASS_2_
#include "objects.c"

void NDECL(objects_init);

/* dummy routine used to force linkage */
void
objects_init()
{
    return;
}

#endif	/* !OBJECTS_PASS_2_ */

/*objects.c*/
