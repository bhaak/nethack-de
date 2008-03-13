/*	SCCS Id: @(#)cmd.c	3.4	2003/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "func_tab.h"
/* #define DEBUG */	/* uncomment for debugging */

#ifdef GERMAN
# include "german.h"
#endif

/*
 * Some systems may have getchar() return EOF for various reasons, and
 * we should not quit before seeing at least NR_OF_EOFS consecutive EOFs.
 */
#if defined(SYSV) || defined(DGUX) || defined(HPUX)
#define NR_OF_EOFS	20
#endif

#define CMD_TRAVEL (char)0x90

#ifdef DEBUG
/*
 * only one "wiz_debug_cmd" routine should be available (in whatever
 * module you are trying to debug) or things are going to get rather
 * hard to link :-)
 */
extern int NDECL(wiz_debug_cmd);
#endif

#ifdef DUMB	/* stuff commented out in extern.h, but needed here */
extern int NDECL(doapply); /**/
extern int NDECL(dorub); /**/
extern int NDECL(dojump); /**/
extern int NDECL(doextlist); /**/
extern int NDECL(dodrop); /**/
extern int NDECL(doddrop); /**/
extern int NDECL(dodown); /**/
extern int NDECL(doup); /**/
extern int NDECL(donull); /**/
extern int NDECL(dowipe); /**/
extern int NDECL(do_mname); /**/
extern int NDECL(ddocall); /**/
extern int NDECL(dotakeoff); /**/
extern int NDECL(doremring); /**/
extern int NDECL(dowear); /**/
extern int NDECL(doputon); /**/
extern int NDECL(doddoremarm); /**/
extern int NDECL(dokick); /**/
extern int NDECL(dofire); /**/
extern int NDECL(dothrow); /**/
extern int NDECL(doeat); /**/
extern int NDECL(done2); /**/
extern int NDECL(doengrave); /**/
extern int NDECL(dopickup); /**/
extern int NDECL(ddoinv); /**/
extern int NDECL(dotypeinv); /**/
extern int NDECL(dolook); /**/
extern int NDECL(doprgold); /**/
extern int NDECL(doprwep); /**/
extern int NDECL(doprarm); /**/
extern int NDECL(doprring); /**/
extern int NDECL(dopramulet); /**/
extern int NDECL(doprtool); /**/
extern int NDECL(dosuspend); /**/
extern int NDECL(doforce); /**/
extern int NDECL(doopen); /**/
extern int NDECL(doclose); /**/
extern int NDECL(dosh); /**/
extern int NDECL(dodiscovered); /**/
extern int NDECL(doset); /**/
extern int NDECL(dotogglepickup); /**/
extern int NDECL(dowhatis); /**/
extern int NDECL(doquickwhatis); /**/
extern int NDECL(dowhatdoes); /**/
extern int NDECL(dohelp); /**/
extern int NDECL(dohistory); /**/
extern int NDECL(doloot); /**/
extern int NDECL(dodrink); /**/
extern int NDECL(dodip); /**/
extern int NDECL(dosacrifice); /**/
extern int NDECL(dopray); /**/
extern int NDECL(doturn); /**/
extern int NDECL(doredraw); /**/
extern int NDECL(doread); /**/
extern int NDECL(dosave); /**/
extern int NDECL(dosearch); /**/
extern int NDECL(doidtrap); /**/
extern int NDECL(dopay); /**/
extern int NDECL(dosit); /**/
extern int NDECL(dotalk); /**/
extern int NDECL(docast); /**/
extern int NDECL(dovspell); /**/
extern int NDECL(dotele); /**/
extern int NDECL(dountrap); /**/
extern int NDECL(doversion); /**/
extern int NDECL(doextversion); /**/
extern int NDECL(doswapweapon); /**/
extern int NDECL(dowield); /**/
extern int NDECL(dowieldquiver); /**/
extern int NDECL(dozap); /**/
extern int NDECL(doorganize); /**/
#endif /* DUMB */

#ifdef OVL1
static int NDECL((*timed_occ_fn));
#endif /* OVL1 */

STATIC_PTR int NDECL(doprev_message);
STATIC_PTR int NDECL(timed_occupation);
STATIC_PTR int NDECL(doextcmd);
STATIC_PTR int NDECL(domonability);
STATIC_PTR int NDECL(dotravel);
# ifdef WIZARD
STATIC_PTR int NDECL(wiz_wish);
STATIC_PTR int NDECL(wiz_identify);
STATIC_PTR int NDECL(wiz_map);
STATIC_PTR int NDECL(wiz_genesis);
STATIC_PTR int NDECL(wiz_where);
STATIC_PTR int NDECL(wiz_detect);
STATIC_PTR int NDECL(wiz_panic);
STATIC_PTR int NDECL(wiz_polyself);
STATIC_PTR int NDECL(wiz_level_tele);
STATIC_PTR int NDECL(wiz_level_change);
STATIC_PTR int NDECL(wiz_show_seenv);
STATIC_PTR int NDECL(wiz_show_vision);
STATIC_PTR int NDECL(wiz_mon_polycontrol);
STATIC_PTR int NDECL(wiz_show_wmodes);
STATIC_PTR int NDECL(wiz_showkills);	/* showborn patch */
#ifdef SHOW_BORN
extern void FDECL(list_vanquished, (int, BOOLEAN_P)); /* showborn patch */
#endif /* SHOW_BORN */
#if defined(__BORLANDC__) && !defined(_WIN32)
extern void FDECL(show_borlandc_stats, (winid));
#endif
STATIC_DCL void FDECL(count_obj, (struct obj *, long *, long *, BOOLEAN_P, BOOLEAN_P));
STATIC_DCL void FDECL(obj_chain, (winid, const char *, struct obj *, long *, long *));
STATIC_DCL void FDECL(mon_invent_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(mon_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(contained, (winid, const char *, long *, long *));
STATIC_PTR int NDECL(wiz_show_stats);
#  ifdef PORT_DEBUG
STATIC_DCL int NDECL(wiz_port_debug);
#  endif
# endif
STATIC_PTR int NDECL(enter_explore_mode);
STATIC_PTR int NDECL(doattributes);
STATIC_PTR int NDECL(doconduct); /**/
STATIC_PTR boolean NDECL(minimal_enlightenment);

#ifdef OVLB
STATIC_DCL void FDECL(enlght_line, (const char *,const char *,const char *));
STATIC_DCL char *FDECL(enlght_combatinc, (const char *,int,int,char *));
#ifdef UNIX
static void NDECL(end_of_input);
#endif
#endif /* OVLB */

static const char* readchar_queue="";

STATIC_DCL char *NDECL(parse);
STATIC_DCL boolean FDECL(help_dir, (CHAR_P,const char *));

#ifdef OVL1

STATIC_PTR int
doprev_message()
{
    return nh_doprev_message();
}

/* Count down by decrementing multi */
STATIC_PTR int
timed_occupation()
{
	(*timed_occ_fn)();
	if (multi > 0)
		multi--;
	return multi > 0;
}

/* If you have moved since initially setting some occupations, they
 * now shouldn't be able to restart.
 *
 * The basic rule is that if you are carrying it, you can continue
 * since it is with you.  If you are acting on something at a distance,
 * your orientation to it must have changed when you moved.
 *
 * The exception to this is taking off items, since they can be taken
 * off in a number of ways in the intervening time, screwing up ordering.
 *
 *	Currently:	Take off all armor.
 *			Picking Locks / Forcing Chests.
 *			Setting traps.
 */
void
reset_occupations()
{
	reset_remarm();
	reset_pick();
	reset_trapset();
}

/* If a time is given, use it to timeout this function, otherwise the
 * function times out by its own means.
 */
void
set_occupation(fn, txt, xtime)
int NDECL((*fn));
const char *txt;
int xtime;
{
	if (xtime) {
		occupation = timed_occupation;
		timed_occ_fn = fn;
	} else
		occupation = fn;
	occtxt = txt;
	occtime = 0;
	return;
}

#ifdef REDO

static char NDECL(popch);

/* Provide a means to redo the last command.  The flag `in_doagain' is set
 * to true while redoing the command.  This flag is tested in commands that
 * require additional input (like `throw' which requires a thing and a
 * direction), and the input prompt is not shown.  Also, while in_doagain is
 * TRUE, no keystrokes can be saved into the saveq.
 */
#define BSIZE 20
static char pushq[BSIZE], saveq[BSIZE];
static NEARDATA int phead, ptail, shead, stail;

static char
popch() {
	/* If occupied, return '\0', letting tgetch know a character should
	 * be read from the keyboard.  If the character read is not the
	 * ABORT character (as checked in pcmain.c), that character will be
	 * pushed back on the pushq.
	 */
	if (occupation) return '\0';
	if (in_doagain) return(char)((shead != stail) ? saveq[stail++] : '\0');
	else		return(char)((phead != ptail) ? pushq[ptail++] : '\0');
}

char
pgetchar() {		/* curtesy of aeb@cwi.nl */
	register int ch;

	if(!(ch = popch()))
		ch = nhgetch();
	return((char)ch);
}

/* A ch == 0 resets the pushq */
void
pushch(ch)
char ch;
{
	if (!ch)
		phead = ptail = 0;
	if (phead < BSIZE)
		pushq[phead++] = ch;
	return;
}

/* A ch == 0 resets the saveq.	Only save keystrokes when not
 * replaying a previous command.
 */
void
savech(ch)
char ch;
{
	if (!in_doagain) {
		if (!ch)
			phead = ptail = shead = stail = 0;
		else if (shead < BSIZE)
			saveq[shead++] = ch;
	}
	return;
}
#endif /* REDO */

#endif /* OVL1 */
#ifdef OVLB

STATIC_PTR int
doextcmd()	/* here after # - now read a full-word command */
{
	int idx, retval;

	/* keep repeating until we don't run help or quit */
	do {
	    idx = get_ext_cmd();
	    if (idx < 0) return 0;	/* quit */

	    retval = (*extcmdlist[idx].ef_funct)();
	} while (extcmdlist[idx].ef_funct == doextlist);

	return retval;
}

int
doextlist()	/* here after #? - now list all full-word commands */
{
	register const struct ext_func_tab *efp;
	char	 buf[BUFSZ];
	winid datawin;

	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
	putstr(datawin, 0, "            Erweiterte Befehlsliste"); /* EN putstr(datawin, 0, "            Extended Commands List"); */
	putstr(datawin, 0, "");
	putstr(datawin, 0, "    '#' drücken, dann eingeben:"); /* EN putstr(datawin, 0, "    Press '#', then type:"); */
	putstr(datawin, 0, "");

	for(efp = extcmdlist; efp->ef_txt; efp++) {
		Sprintf(buf, "    %-15s - %s.", efp->ef_txt, efp->ef_desc);
		putstr(datawin, 0, buf);
	}
	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
	return 0;
}

#ifdef TTY_GRAPHICS
#define MAX_EXT_CMD 40		/* Change if we ever have > 40 ext cmds */
/*
 * This is currently used only by the tty port and is
 * controlled via runtime option 'extmenu'
 */
int
extcmd_via_menu()	/* here after # - now show pick-list of possible commands */
{
    const struct ext_func_tab *efp;
    menu_item *pick_list = (menu_item *)0;
    winid win;
    anything any;
    const struct ext_func_tab *choices[MAX_EXT_CMD];
    char buf[BUFSZ];
    char cbuf[QBUFSZ], prompt[QBUFSZ], fmtstr[20];
    int i, n, nchoices, acount;
    int ret,  biggest;
    int accelerator, prevaccelerator;
    int  matchlevel = 0;

    ret = 0;
    cbuf[0] = '\0';
    biggest = 0;
    while (!ret) {
	    i = n = 0;
	    accelerator = 0;
	    any.a_void = 0;
	    /* populate choices */
	    for(efp = extcmdlist; efp->ef_txt; efp++) {
		if (!matchlevel || !strncmp(efp->ef_txt, cbuf, matchlevel)) {
			choices[i++] = efp;
			if ((int)strlen(efp->ef_desc) > biggest) {
				biggest = strlen(efp->ef_desc);
				Sprintf(fmtstr,"%%-%ds", biggest + 15);
			}
#ifdef DEBUG
			if (i >= MAX_EXT_CMD - 2) {
			    impossible("Exceeded %d extended commands in doextcmd() menu",
					MAX_EXT_CMD - 2);
			    return 0;
			}
#endif
		}
	    }
	    choices[i] = (struct ext_func_tab *)0;
	    nchoices = i;
	    /* if we're down to one, we have our selection so get out of here */
	    if (nchoices == 1) {
		for (i = 0; extcmdlist[i].ef_txt != (char *)0; i++)
			if (!strncmpi(extcmdlist[i].ef_txt, cbuf, matchlevel)) {
				ret = i;
				break;
			}
		break;
	    }

	    /* otherwise... */
	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);
	    prevaccelerator = 0;
	    acount = 0;
	    for(i = 0; choices[i]; ++i) {
		accelerator = choices[i]->ef_txt[matchlevel];
		if (accelerator != prevaccelerator || nchoices < (ROWNO - 3)) {
		    if (acount) {
 			/* flush the extended commands for that letter already in buf */
			Sprintf(buf, fmtstr, prompt);
			any.a_char = prevaccelerator;
			add_menu(win, NO_GLYPH, &any, any.a_char, 0,
					ATR_NONE, buf, FALSE);
			acount = 0;
		    }
		}
		prevaccelerator = accelerator;
		if (!acount || nchoices < (ROWNO - 3)) {
		    Sprintf(prompt, "%s [%s]", choices[i]->ef_txt,
				choices[i]->ef_desc);
		} else if (acount == 1) {
		    Sprintf(prompt, "%s oder %s", choices[i-1]->ef_txt, /* EN Sprintf(prompt, "%s or %s", choices[i-1]->ef_txt, */
				choices[i]->ef_txt);
		} else {
			Strcat(prompt," oder "); /* EN Strcat(prompt," or "); */
		    Strcat(prompt, choices[i]->ef_txt);
		}
		++acount;
	    }
	    if (acount) {
		/* flush buf */
		Sprintf(buf, fmtstr, prompt);
		any.a_char = prevaccelerator;
		add_menu(win, NO_GLYPH, &any, any.a_char, 0, ATR_NONE, buf, FALSE);
	    }
	    Sprintf(prompt, "Erweiterte Befehle: %s", cbuf); /* EN Sprintf(prompt, "Extended Command: %s", cbuf); */
	    end_menu(win, prompt);
	    n = select_menu(win, PICK_ONE, &pick_list);
	    destroy_nhwindow(win);
	    if (n==1) {
		if (matchlevel > (QBUFSZ - 2)) {
			free((genericptr_t)pick_list);
#ifdef DEBUG
			impossible("Too many characters (%d) entered in extcmd_via_menu()",
				matchlevel);
#endif
			ret = -1;
		} else {
			cbuf[matchlevel++] = pick_list[0].item.a_char;
			cbuf[matchlevel] = '\0';
			free((genericptr_t)pick_list);
		}
	    } else {
		if (matchlevel) {
			ret = 0;
			matchlevel = 0;
		} else
			ret = -1;
	    }
    }
    return ret;
}
#endif

/* #monster command - use special monster ability while polymorphed */
STATIC_PTR int
domonability()
{
	if (can_breathe(youmonst.data)) return dobreathe();
	else if (attacktype(youmonst.data, AT_SPIT)) return dospit();
	else if (youmonst.data->mlet == S_NYMPH) return doremove();
	else if (attacktype(youmonst.data, AT_GAZE)) return dogaze();
	else if (is_were(youmonst.data)) return dosummon();
	else if (webmaker(youmonst.data)) return dospinweb();
	else if (is_hider(youmonst.data)) return dohide();
	else if (is_mind_flayer(youmonst.data)) return domindblast();
	else if (u.umonnum == PM_GREMLIN) {
	    if(IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		if (split_mon(&youmonst, (struct monst *)0))
		    dryup(u.ux, u.uy, TRUE);
	    } else pline("Hier ist PRONOMEN_KEIN NOUN_FOUNTAIN."); /* EN } else There("is no fountain here."); */
	} else if (is_unicorn(youmonst.data)) {
	    use_unicorn_horn((struct obj *)0);
	    return 1;
	} else if (youmonst.data->msound == MS_SHRIEK) {
	    You("VERB_SHRIEK."); /* EN You("shriek."); */
	    if(u.uburied)
		pline("Unfortunately sound does not carry well through rock."); /* EN pline("Unfortunately sound does not carry well through rock."); */ // TODO DE
	    else aggravate();
	} else if (Upolyd)
		pline("Any special ability you may have is purely reflexive."); /* EN pline("Any special ability you may have is purely reflexive."); */ // TODO DE
	else pline("In KASUS_DATIV PRONOMEN_POSSESSIV ADJEKTIV_NORMAL NOUN_GESTALT SUBJECT_IM_SATZ VERB_BESITZEN PRONOMEN_PERSONAL keine besondere Fähigkeit!"); /* EN else You("don't have a special ability in your normal form!"); */
	return 0;
}

STATIC_PTR int
enter_explore_mode()
{
	if(!discover && !wizard) {
		pline("Achtung!  Vom Erkundungsmodus gibt es keinen Weg zurück ins normale Spiel."); /* EN pline("Beware!  From explore mode there will be no return to normal game."); */
		if (yn("SUBJECT MODIFIER_KONJUNKTIV_II VERB_MOEGEN PRONOMEN_PERSONAL den Erkundungsmodus aktivieren?") == 'y') { /* EN if (yn("Do you want to enter explore mode?") == 'y') { */
			clear_nhwindow(WIN_MESSAGE);
			You("VERB_SEIN jetzt im nicht-bewerteten Erkundungsmodus."); /* EN You("are now in non-scoring explore mode."); */
			discover = TRUE;
		}
		else {
			clear_nhwindow(WIN_MESSAGE);
			pline("Das normale Spiel wird fortgesetzt."); /* EN pline("Resuming normal game."); */
		}
	}
	return 0;
}

#ifdef WIZARD

/* ^W command - wish for something */
STATIC_PTR int
wiz_wish()	/* Unlimited wishes for debug mode by Paul Polderman */
{
	if (wizard) {
	    boolean save_verbose = flags.verbose;

	    flags.verbose = FALSE;
	    makewish();
	    flags.verbose = save_verbose;
	    (void) encumber_msg();
	} else
	    pline("Nicht verfügbarer Befehl '^W'."); /* EN pline("Unavailable command '^W'."); */
	return 0;
}

/* ^I command - identify hero's inventory */
STATIC_PTR int
wiz_identify()
{
	if (wizard)	identify_pack(0);
	else		pline("Nicht verfügbarer Befehl '^I'."); /* EN else		pline("Unavailable command '^I'."); */
	return 0;
}

/* ^F command - reveal the level map and any traps on it */
STATIC_PTR int
wiz_map()
{
	if (wizard) {
	    struct trap *t;
	    long save_Hconf = HConfusion,
		 save_Hhallu = HHallucination;

	    HConfusion = HHallucination = 0L;
	    for (t = ftrap; t != 0; t = t->ntrap) {
		t->tseen = 1;
		map_trap(t, TRUE);
	    }
	    do_mapping();
	    HConfusion = save_Hconf;
	    HHallucination = save_Hhallu;
	} else
	    pline("Nicht verfügbarer Befehl '^F'."); /* EN pline("Unavailable command '^F'."); */
	return 0;
}

/* ^G command - generate monster(s); a count prefix will be honored */
STATIC_PTR int
wiz_genesis()
{
	if (wizard)	(void) create_particular();
	else		pline("Nicht verfügbarer Befehl '^G'."); /* EN else		pline("Unavailable command '^G'."); */
	return 0;
}

/* ^O command - display dungeon layout */
STATIC_PTR int
wiz_where()
{
	if (wizard) (void) print_dungeon(FALSE);
	else	    pline("Nicht verfügbarer Befehl '^O'."); /* EN else	    pline("Unavailable command '^O'."); */
	return 0;
}

/* ^E command - detect unseen (secret doors, traps, hidden monsters) */
STATIC_PTR int
wiz_detect()
{
	if(wizard)  (void) findit();
	else	    pline("Nicht verfügbarer Befehl '^E'."); /* EN else	    pline("Unavailable command '^E'."); */
	return 0;
}

/* ^V command - level teleport */
STATIC_PTR int
wiz_level_tele()
{
	if (wizard)	level_tele();
	else		pline("Nicht verfügbarer Befehl '^V'."); /* EN else		pline("Unavailable command '^V'."); */
	return 0;
}

/* #monpolycontrol command - choose new form for shapechangers, polymorphees */
STATIC_PTR int
wiz_mon_polycontrol()
{
    iflags.mon_polycontrol = !iflags.mon_polycontrol;
    pline("Monstertransformationskontrolle ist %s.", /* EN pline("Monster polymorph control is %s.", */
	  iflags.mon_polycontrol ? "ein" : "aus"); /* EN iflags.mon_polycontrol ? "on" : "off"); */
    return 0;
}

/* #levelchange command - adjust hero's experience level */
STATIC_PTR int
wiz_level_change()
{
    char buf[BUFSZ];
    int newlevel;
    int ret;

    getlin("Auf welche Erfahrungsstufe MODIFIER_KONJUNKTIV_II VERB_MOEGEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL gesetzt werden?", buf); /* EN getlin("To what experience level do you want to be set?", buf); */
    (void)mungspaces(buf);
    if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
    else ret = sscanf(buf, "%d", &newlevel);

    if (ret != 1) {
	pline(Never_mind);
	return 0;
    }
    if (newlevel == u.ulevel) {
	Du_bist("schon so erfahren."); /* EN You("are already that experienced."); */
    } else if (newlevel < u.ulevel) {
	if (u.ulevel == 1) {
	    Du_bist("schon so unerfahren wie möglich."); /* EN You("are already as inexperienced as you can get."); */
	    return 0;
	}
	if (newlevel < 1) newlevel = 1;
	while (u.ulevel > newlevel)
	    losexp("#levelchange");
    } else {
	if (u.ulevel >= MAXULEV) {
	    Du_bist("schon so erfahren wie möglich."); /* EN You("are already as experienced as you can get."); */
	    return 0;
	}
	if (newlevel > MAXULEV) newlevel = MAXULEV;
	while (u.ulevel < newlevel)
	    pluslvl(FALSE);
    }
    u.ulevelmax = u.ulevel;
    return 0;
}

/* #panic command - test program's panic handling */
STATIC_PTR int
wiz_panic()
{
	if (yn("Wirklich panic() aufrufen und KASUS_AKKUSATIV PRONOMEN_POSSESSIV NOUN_GAME beenden?") == 'y') /* EN if (yn("Do you want to call panic() and end your game?") == 'y') */
		panic("Crash-Test."); /* EN panic("crash test."); */
        return 0;
}

/* #polyself command - change hero's form */
STATIC_PTR int
wiz_polyself()
{
        polyself(TRUE);
        return 0;
}

/* #seenv command */
STATIC_PTR int
wiz_show_seenv()
{
	winid win;
	int x, y, v, startx, stopx, curx;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	/*
	 * Each seenv description takes up 2 characters, so center
	 * the seenv display around the hero.
	 */
	startx = max(1, u.ux-(COLNO/4));
	stopx = min(startx+(COLNO/2), COLNO);
	/* can't have a line exactly 80 chars long */
	if (stopx - startx == COLNO/2) startx++;

	for (y = 0; y < ROWNO; y++) {
	    for (x = startx, curx = 0; x < stopx; x++, curx += 2) {
		if (x == u.ux && y == u.uy) {
		    row[curx] = row[curx+1] = '@';
		} else {
		    v = levl[x][y].seenv & 0xff;
		    if (v == 0)
			row[curx] = row[curx+1] = ' ';
		    else
			Sprintf(&row[curx], "%02x", v);
		}
	    }
	    /* remove trailing spaces */
	    for (x = curx-1; x >= 0; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #vision command */
STATIC_PTR int
wiz_show_vision()
{
	winid win;
	int x, y, v;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	Sprintf(row, "Flags: 0x%x could see, 0x%x in sight, 0x%x temp lit",
		COULD_SEE, IN_SIGHT, TEMP_LIT);
	putstr(win, 0, row);
	putstr(win, 0, "");
	for (y = 0; y < ROWNO; y++) {
	    for (x = 1; x < COLNO; x++) {
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else {
		    v = viz_array[y][x]; /* data access should be hidden */
		    if (v == 0)
			row[x] = ' ';
		    else
			row[x] = '0' + viz_array[y][x];
		}
	    }
	    /* remove trailing spaces */
	    for (x = COLNO-1; x >= 1; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, &row[1]);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #wmode command */
STATIC_PTR int
wiz_show_wmodes()
{
	winid win;
	int x,y;
	char row[COLNO+1];
	struct rm *lev;

	win = create_nhwindow(NHW_TEXT);
	for (y = 0; y < ROWNO; y++) {
	    for (x = 0; x < COLNO; x++) {
		lev = &levl[x][y];
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else if (IS_WALL(lev->typ) || lev->typ == SDOOR)
		    row[x] = '0' + (lev->wall_info & WM_MASK);
		else if (lev->typ == CORR)
		    row[x] = '#';
		else if (IS_ROOM(lev->typ) || IS_DOOR(lev->typ))
		    row[x] = '.';
		else
		    row[x] = 'x';
	    }
	    row[COLNO] = '\0';
	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #showkills command */
STATIC_PTR int wiz_showkills()		/* showborn patch */
{
	list_vanquished('y', FALSE);
	return 0;
}

#endif /* WIZARD */


/* -enlightenment and conduct- */
static winid en_win;
static const char
	You_[] = "SUBJECT PRONOMEN_PERSONAL ", /* EN You_[] = "You ", */
	are[]  = "VERB_SEIN ",  were[]  = "MODIFIER_VERB_PRAETERITUM VERB_SEIN ", /* EN are[]  = "are ",  were[]  = "were ", */
	have[] = "VERB_HABEN ", had[]   = "MODIFIER_VERB_PRAETERITUM VERB_HABEN ", /* EN have[] = "have ", had[]   = "had ", */
	can[]  = "VERB_KOENNEN ",  could[] = "MODIFIER_VERB_PRAETERITUM VERB_KOENNEN "; /* EN can[]  = "can ",  could[] = "could "; */
static const char
  have_been[]  = "have been ",
	have_never[] = "VERB_HABEN noch nie ", never[] = "VERB_HABEN niemals "; /* EN have_never[] = "have never ", never[] = "never "; */

#define enl_msg(prefix,present,past,suffix) \
			enlght_line(prefix, final ? past : present, suffix)
#define you_are(attr)	enl_msg(You_,are,were,attr)
#define you_have(attr)	enl_msg(You_,have,had,attr)
#define you_can(attr)	enl_msg(You_,can,could,attr)
#define you_have_been(goodthing) enl_msg(You_,have_been,were,goodthing)
#define you_have_never(badthing) enl_msg(You_,have_never,never,badthing)
#ifdef GERMAN
#define you_have_X(something)	enl_msg(You_,have,have,something)
#else
#define you_have_X(something)	enl_msg(You_,have,(const char *)"",something)
#endif
#ifdef GERMAN
#define you(attr)	enl_msg(You_,"","MODIFIER_VERB_PRAETERITUM ",attr)
#endif

static void
enlght_line(start, middle, end)
const char *start, *middle, *end;
{
	char buf[BUFSZ];

	Sprintf(buf, "%s%s%s.", start, middle, end);
	putstr(en_win, 0, buf);
}

/* format increased damage or chance to hit */
static char *
enlght_combatinc(inctyp, incamt, final, outbuf)
const char *inctyp;
int incamt, final;
char *outbuf;
{
	char numbuf[24];
	const char *modif, *bonus;

	if (final
#ifdef WIZARD
		|| wizard
#endif
	  ) {
	    Sprintf(numbuf, "%s%d",
		    (incamt > 0) ? "+" : "", incamt);
	    modif = (const char *) numbuf;
	} else {
	    int absamt = abs(incamt);

	    if (absamt <= 3) modif = "ADJEKTIV_KLEIN"; /* EN if (absamt <= 3) modif = "small"; */
	    else if (absamt <= 6) modif = "ADJEKTIV_MODERAT"; /* EN else if (absamt <= 6) modif = "moderate"; */
	    else if (absamt <= 12) modif = "ADJEKTIV_GROSS"; /* EN else if (absamt <= 12) modif = "large"; */
	    else modif = "ADJEKTIV_RIESIG"; /* EN else modif = "huge"; */
	}
	bonus = (incamt > 0) ? "BONUS" : "MALUS"; /* EN bonus = (incamt > 0) ? "bonus" : "penalty"; */ /* "NOUN_SCHADENSBONUS, NOUN_SCHADENSMALUS, NOUN_TREFFERMALUS, NOUN_TREFFERBONUS" */
#ifndef GERMAN
	/* "bonus to hit" vs "damage bonus" */
	if (!strcmp(inctyp, "damage")) {
	    const char *ctmp = inctyp;
	    inctyp = bonus;
	    bonus = ctmp;
	}
#endif
	Sprintf(outbuf, "OBJECT %s %s%s", an(modif), inctyp, bonus); /* EN Sprintf(outbuf, "%s %s %s", an(modif), bonus, inctyp); */
	return outbuf;
}

void
enlightenment(final)
int final;	/* 0 => still in progress; 1 => over, survived; 2 => dead */
{
	int ltmp;
	char buf[BUFSZ];

	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, final ? "Finale Eigenschaften:" : "Aktuelle Eigenschaften:"); /* EN putstr(en_win, 0, final ? "Final Attributes:" : "Current Attributes:"); */
	putstr(en_win, 0, "");

#ifdef ELBERETH
	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
				"ARTIKEL_BESTIMMTER NOUN_HAND_OF_ELBERETH", /* EN "the Hand of Elbereth", */
				"ARTIKEL_BESTIMMTER NOUN_ENVOY_OF_BALANCE", /* EN "the Envoy of Balance", */
				"ARTIKEL_BESTIMMTER NOUN_GLORY_OF_ARIOCH" /* EN "the Glory of Arioch" */
	    };
	    you_are(hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
#endif

	/* note: piousness 20 matches MIN_QUEST_ALIGN (quest.h) */
	if (u.ualign.record >= 20)	you_are("fromm"); /* EN if (u.ualign.record >= 20)	you_are("piously aligned"); */
	else if (u.ualign.record > 13)	you_are("inbrünstig"); /* EN else if (u.ualign.record > 13)	you_are("devoutly aligned"); */
	else if (u.ualign.record > 8)	you_are("tiefreligiös"); /* EN else if (u.ualign.record > 8)	you_are("fervently aligned"); */
	else if (u.ualign.record > 3)	you_are("strenggläubig"); /* EN else if (u.ualign.record > 3)	you_are("stridently aligned"); */
	else if (u.ualign.record == 3)	you_are("gläubig"); /* EN else if (u.ualign.record == 3)	you_are("aligned"); */
	else if (u.ualign.record > 0)	you_are("glaubensschwach"); /* EN else if (u.ualign.record > 0)	you_are("haltingly aligned"); */
	else if (u.ualign.record == 0)	you_are("ungläubig"); /* EN else if (u.ualign.record == 0)	you_are("nominally aligned"); */
	else if (u.ualign.record >= -3)	you_are("vom rechten Weg abgekommen"); /* EN else if (u.ualign.record >= -3)	you_have("strayed"); */
	else if (u.ualign.record >= -8)	you_have("gesündigt"); /* EN else if (u.ualign.record >= -8)	you_have("sinned"); */ // TODO DE
	else you_have("transgressed"); /* EN else you_have("transgressed"); */ // TODO DE
#ifdef WIZARD
	if (wizard) {
		Sprintf(buf, " %d", u.ualign.record);
		enl_msg("SUBJECT PRONOMEN_POSSESSIV NOUN_ALIGNMENT ", "VERB_SEIN", "MODIFIER_VERB_PRAETERITUM VERB_SEIN", buf); /* EN enl_msg("Your alignment ", "is", "was", buf); */
	}
#endif

	/*** Resistances to troubles ***/
	if (Fire_resistance) you_are("feuerresistent"); /* EN if (Fire_resistance) you_are("fire resistant"); */
	if (Cold_resistance) you_are("kälteresistent"); /* EN if (Cold_resistance) you_are("cold resistant"); */
	if (Sleep_resistance) you_are("schlafresistent"); /* EN if (Sleep_resistance) you_are("sleep resistant"); */
	if (Disint_resistance) you_are("disintegration-resistant"); /* EN if (Disint_resistance) you_are("disintegration-resistant"); */ // TODO DE
	if (Shock_resistance) you_are("shock resistant"); /* EN if (Shock_resistance) you_are("shock resistant"); */ // TODO DE
	if (Poison_resistance) you_are("giftresistent"); /* EN if (Poison_resistance) you_are("poison resistant"); */
	if (Drain_resistance) you_are("level-drain resistant"); /* EN if (Drain_resistance) you_are("level-drain resistant"); */ // TODO DE
	if (Sick_resistance) you_are("immune to sickness"); /* EN if (Sick_resistance) you_are("immune to sickness"); */ // TODO DE
	if (Antimagic) you_are("magic-protected"); /* EN if (Antimagic) you_are("magic-protected"); */ // TODO DE
	if (Acid_resistance) you_are("säureresistent"); /* EN if (Acid_resistance) you_are("acid resistant"); */
	if (Stone_resistance)
		you_are("petrification resistant"); /* EN you_are("petrification resistant"); */ // TODO DE
	if (Invulnerable) you_are("unverwundbar"); /* EN if (Invulnerable) you_are("invulnerable"); */
	if (u.uedibility) you_can("recognize detrimental food"); /* EN if (u.uedibility) you_can("recognize detrimental food"); */ // TODO DE

	/*** Troubles ***/
	if (Halluc_resistance)
		enl_msg("You resist", "", "ed", " hallucinations"); /* EN enl_msg("You resist", "", "ed", " hallucinations"); */ // TODO DE
	if (final) {
		if (Hallucination) you_are("hallucinating"); /* EN if (Hallucination) you_are("hallucinating"); */ // TODO DE
		if (Stunned) you_are("benommen"); /* EN if (Stunned) you_are("stunned"); */
		if (Confusion) you_are("verwirrt"); /* EN if (Confusion) you_are("confused"); */
		if (Blinded) you_are("geblendet"); /* EN if (Blinded) you_are("blinded"); */
		if (Sick) {
			if (u.usick_type & SICK_VOMITABLE)
				you_are("krank wegen einer Lebensmittelvergiftung"); /* EN you_are("sick from food poisoning"); */
			if (u.usick_type & SICK_NONVOMITABLE)
				you_are("sick from illness"); /* EN you_are("sick from illness"); */ // TODO DE
		}
	}
	if (Stoned) you_are("am Versteinern"); /* EN if (Stoned) you_are("turning to stone"); */
	if (Slimed) you_are("am Verschleimen"); /* EN if (Slimed) you_are("turning into slime"); */
	if (Strangled) you((u.uburied) ? "VERB_SEIN begraben" : "VERB_WERDEN stranguliert"); /* EN if (Strangled) you_are((u.uburied) ? "buried" : "being strangled"); */
	if (Glib) {
		Sprintf(buf, "ADJEKTIV_FETTIG %s", makeplural(body_part(FINGER))); /* EN Sprintf(buf, "slippery %s", makeplural(body_part(FINGER))); */
		you_have(buf);
	}
	if (Fumbling) enl_msg("You fumble", "", "d", ""); /* EN if (Fumbling) enl_msg("You fumble", "", "d", ""); */ // TODO DE
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
		Sprintf(buf, "OBJECT ADJEKTIV_VERLETZT %s", makeplural(body_part(LEG))); /* EN Sprintf(buf, "wounded %s", makeplural(body_part(LEG))); */
		you_have(buf);
	}
#if defined(WIZARD) && defined(STEED)
	if (Wounded_legs && u.usteed && wizard) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    enl_msg(buf, "", " MODIFIER_VERB_PRAETERITUM", " VERB_HABEN ADJEKTIV_VERLETZT NOUN_LEGs"); /* EN enl_msg(buf, " has", " had", " wounded legs"); */
	}
#endif
	if (Sleeping) enl_msg("SUBJECT PRONOMEN_PERSONAL", "", " MODIFIER_VERB_PRAETERITUM", " VERB_HABEN Narkolepsie"); /* EN if (Sleeping) enl_msg("You ", "fall", "fell", " asleep"); */
	if (Hunger) enl_msg("You hunger", "", "ed", " rapidly"); /* EN if (Hunger) enl_msg("You hunger", "", "ed", " rapidly"); */ // TODO DE

	/*** Vision and senses ***/
	if (See_invisible) enl_msg(You_, "VERB_SEHEN", "saw", " invisible"); /* EN if (See_invisible) enl_msg(You_, "see", "saw", " invisible"); */ // TODO DE
	if (Blind_telepat) you_are("telepathisch begabt"); /* EN if (Blind_telepat) you_are("telepathic"); */
	if (Warning) you_are("gewarnt"); /* EN if (Warning) you_are("warned"); */
	if (Warn_of_mon && flags.warntype) {
		Sprintf(buf, "OBJECT KASUS_DATIV PRONOMEN_PERSONAL der Anwesenheit von KASUS_DATIV %s bewusst", /* EN Sprintf(buf, "aware of the presence of %s", */
			(flags.warntype & M2_ORC) ? "NOUN_ORCs" : /* EN (flags.warntype & M2_ORC) ? "orcs" : */
			(flags.warntype & M2_DEMON) ? "NOUN_DEMONs" : /* EN (flags.warntype & M2_DEMON) ? "demons" : */
			something); 
		you_are(buf);
	}
	if (Undead_warning) you_are("warned of undead"); /* EN if (Undead_warning) you_are("warned of undead"); */ // TODO DE
	if (Searching) you_have("automatic searching"); /* EN if (Searching) you_have("automatic searching"); */ // TODO DE
	if (Clairvoyant) you_are("clairvoyant"); /* EN if (Clairvoyant) you_are("clairvoyant"); */ // TODO DE
	if (Infravision) you_have("Infravision"); /* EN if (Infravision) you_have("infravision"); */
	if (Detect_monsters) you_are("sensing the presence of monsters"); /* EN if (Detect_monsters) you_are("sensing the presence of monsters"); */ // TODO DE
	if (u.umconf) you_are("going to confuse monsters"); /* EN if (u.umconf) you_are("going to confuse monsters"); */ // TODO DE

	/*** Appearance and behavior ***/
	if (Adornment) {
	    int adorn = 0;

	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		you_are("poorly adorned"); /* EN you_are("poorly adorned"); */ // TODO DE
	    else
		you_are("adorned"); /* EN you_are("adorned"); */ // TODO DE
	}
	if (Invisible) you_are("ADJEKTIV_INVISIBLE"); /* EN if (Invisible) you_are("invisible"); */
	else if (Invis) you_are("ADJEKTIV_INVISIBLE für andere"); /* EN else if (Invis) you_are("invisible to others"); */
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis)
	    you_are("sichtbar"); /* EN you_are("visible"); */
	if (Displaced) you_are("displaced"); /* EN if (Displaced) you_are("displaced"); */ // TODO DE
	if (Stealth) you_are("stealthy"); /* EN if (Stealth) you_are("stealthy"); */ // TODO DE
	if (Aggravate_monster) enl_msg("You aggravate", "", "d", " monsters"); /* EN if (Aggravate_monster) enl_msg("You aggravate", "", "d", " monsters"); */ // TODO DE
	if (Conflict) enl_msg("You cause", "", "d", " conflict"); /* EN if (Conflict) enl_msg("You cause", "", "d", " conflict"); */ // TODO DE

	/*** Transportation ***/
	if (Jumping) you_can("springen"); /* EN if (Jumping) you_can("jump"); */
	if (Teleportation) you_can("teleportieren"); /* EN if (Teleportation) you_can("teleport"); */
	if (Teleport_control) you_have("teleport control"); /* EN if (Teleport_control) you_have("teleport control"); */ // TODO DE
	if (Lev_at_will) you_are("levitating, at will"); /* EN if (Lev_at_will) you_are("levitating, at will"); */ // TODO DE
	else if (Levitation) you_are("levitating");	/* without control */ /* EN else if (Levitation) you_are("levitating");	*/ // TODO DE
	else if (Flying) you_can("fliegen"); /* EN else if (Flying) you_can("fly"); */
	if (Wwalking) you_can("walk on water"); /* EN if (Wwalking) you_can("walk on water"); */ // TODO DE
	if (Swimming) you_can("schwimmen");        /* EN if (Swimming) you_can("swim");         */
	if (Breathless) you_can("survive without air"); /* EN if (Breathless) you_can("survive without air"); */ // TODO DE
	else if (Amphibious) you_can("Wasser atmen"); /* EN else if (Amphibious) you_can("breathe water"); */
	if (Passes_walls) you_can("durch Wände gehen"); /* EN if (Passes_walls) you_can("walk through walls"); */
#ifdef STEED
	/* If you die while dismounting, u.usteed is still set.  Since several
	 * places in the done() sequence depend on u.usteed, just detect this
	 * special case. */
	if (u.usteed && (final < 2 || strcmp(killer, "NOUN_REITUNFALL"))) { /* EN if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) { */
	    Sprintf(buf, "riding %s", y_monnam(u.usteed)); /* EN Sprintf(buf, "riding %s", y_monnam(u.usteed)); */ // TODO DE
	    you_are(buf);
	}
#endif
	if (u.uswallow) {
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck)); /* EN Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck)); */ // TODO DE
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif
	    you_are(buf);
	} else if (u.ustuck) {
	    Sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by", /* EN (Upolyd && sticks(youmonst.data)) ? "holding" : "held by", */ // TODO DE
		    a_monnam(u.ustuck));
	    you_are(buf);
	}

	/*** Physical attributes ***/
	if (u.uhitinc)
	    you_have(enlght_combatinc("NOUN_TREFFER", u.uhitinc, final, buf)); /* EN you_have(enlght_combatinc("to hit", u.uhitinc, final, buf)); */
	if (u.udaminc)
	    you_have(enlght_combatinc("NOUN_SCHADENS", u.udaminc, final, buf)); /* EN you_have(enlght_combatinc("damage", u.udaminc, final, buf)); */
	if (Slow_digestion) you_have("verlangsamte Verdauung"); /* EN if (Slow_digestion) you_have("slower digestion"); */
	if (Regeneration) enl_msg("You regenerate", "", "d", ""); /* EN if (Regeneration) enl_msg("You regenerate", "", "d", ""); */ // TODO DE
	if (u.uspellprot || Protection) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;

	    if (prot < 0)
		you_are("ineffectively protected"); /* EN you_are("ineffectively protected"); */ // TODO DE
	    else
		you_are("protected"); /* EN you_are("protected"); */ // TODO DE
	}
	if (Protection_from_shape_changers)
		you_are("protected from shape changers"); /* EN you_are("protected from shape changers"); */ // TODO DE
	if (Polymorph) you_are("polymorphing"); /* EN if (Polymorph) you_are("polymorphing"); */ // TODO DE
	if (Polymorph_control) you_have("Transformationskontrolle"); /* EN if (Polymorph_control) you_have("polymorph control"); */
	if (u.ulycn >= LOW_PM) {
		Strcpy(buf, "OBJECT KASUS_NOMINATIV "); Strcat(buf, an(mons[u.ulycn].mname)); /* EN Strcpy(buf, an(mons[u.ulycn].mname)); */
		you_are(buf);
	}
	if (Upolyd) {
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form"); /* EN if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form"); */ // TODO DE
	    else Sprintf(buf, "polymorphed into %s", an(youmonst.data->mname)); /* EN else Sprintf(buf, "polymorphed into %s", an(youmonst.data->mname)); */ // TODO DE
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    you_are(buf);
	}
	if (Unchanging) you_can("not change from your current form"); /* EN if (Unchanging) you_can("not change from your current form"); */ // TODO DE
	if (Fast) you_are(Very_fast ? "very fast" : "fast"); /* EN if (Fast) you_are(Very_fast ? "very fast" : "fast"); */ // TODO DE
	if (Reflecting) you_have("reflection"); /* EN if (Reflecting) you_have("reflection"); */ // TODO DE
	if (Free_action) you_have("free action"); /* EN if (Free_action) you_have("free action"); */ // TODO DE
	if (Fixed_abil) you_have("fixed abilities"); /* EN if (Fixed_abil) you_have("fixed abilities"); */ // TODO DE
	if (Lifesaved)
		enl_msg("PRONOMEN_POSSESSIV NOUN_LEBEN ", "wird gerettet werden", "wäre gerettet worden", ""); /* EN enl_msg("Your life ", "will be", "would have been", " saved"); */
	if (u.twoweap) you_are("wielding two weapons at once"); /* EN if (u.twoweap) you_are("wielding two weapons at once"); */ // TODO DE

	/*** Miscellany ***/
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%s", /* EN Sprintf(buf, "%s%slucky", */
		    ltmp >= 10 ? "außerordentliches " : ltmp >= 5 ? "sehr viel " : "", /* EN ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "", */
				Luck < 0 ? "Pech" : "Glück"); /* EN Luck < 0 ? "un" : ""); */
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", Luck);
#endif
	    you_have(buf); /* EN you_are(buf); */
	}
#ifdef WIZARD
	 else if (wizard) enl_msg("Your luck ", "is", "was", " zero"); /* EN else if (wizard) enl_msg("Your luck ", "is", "was", " zero"); */ // TODO DE
#endif
	if (u.moreluck > 0) you_have("besonders viel Glück"); /* EN if (u.moreluck > 0) you_have("extra luck"); */
	else if (u.moreluck < 0) you_have("vermindertes Glück"); /* EN else if (u.moreluck < 0) you_have("reduced luck"); */
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		enl_msg("SUBJECT PRONOMEN_POSSESSIV NOUN_PECHSTRAEHNE ", "bricht", "brach", " nicht ab"); /* EN enl_msg("Bad luck ", "does", "did", " not time out for you"); */
	    if (ltmp >= 0)
		enl_msg("SUBJECT PRONOMEN_POSSESSIV NOUN_GLUECKSSTRAEHNE ", "bricht", "brach", " nicht ab"); /* EN enl_msg("Good luck ", "does", "did", " not time out for you"); */
	}

	if (u.ugangr) {
	    Sprintf(buf, " %sangry with you", /* EN Sprintf(buf, " %sangry with you", */ // TODO DE
		    u.ugangr > 6 ? "extremely/unglaublich " : u.ugangr > 3 ? "very " : ""); /* EN u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : ""); */ // TODO DE
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ugangr);
#endif
	    enl_msg(u_gname(), " is", " was", buf); /* EN enl_msg(u_gname(), " is", " was", buf); */ // TODO DE
	} else
	    /*
	     * We need to suppress this when the game is over, because death
	     * can change the value calculated by can_pray(), potentially
	     * resulting in a false claim that you could have prayed safely.
	     */
	  if (!final) {
#if 0
	    /* "can [not] safely pray" vs "could [not] have safely prayed" */
	    Sprintf(buf, "%s%ssafely pray%s", can_pray(FALSE) ? "" : "not ",
		    final ? "have " : "", final ? "ed" : "");
#else
	    Sprintf(buf, "%ssafely pray", can_pray(FALSE) ? "" : "not "); /* EN Sprintf(buf, "%ssafely pray", can_pray(FALSE) ? "" : "not "); */ // TODO DE
#endif
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ublesscnt);
#endif
	    you_can(buf);
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* still in progress, or quit/escaped/ascended */
	    p = "VERB_HABEN überlebt, obwohl NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL "; /* EN p = "survived after being killed "; */
	    switch (u.umortality) {
	    case 0:  p = !final ? (char *)0 : "VERB_HABEN überlebt";  break; /* EN case 0:  p = !final ? (char *)0 : "survived";  break; */
	    case 1:  Strcpy(buf, "einmal getötet");  break; /* EN case 1:  Strcpy(buf, "once");  break; */
	    case 2:  Strcpy(buf, "zweimal getötet");  break; /* EN case 2:  Strcpy(buf, "twice");  break; */
	    case 3:  Strcpy(buf, "dreimal getötet");  break; /* EN case 3:  Strcpy(buf, "thrice");  break; */
	    default: Sprintf(buf, "%d-mal getötet", u.umortality); /* EN default: Sprintf(buf, "%d times", u.umortality); */
		     break;
	    }
#ifdef GERMAN
			if (u.umortality > 0) {
			if (final) { Strcat(buf, " MODIFIER_VERB_PRAETERITUM VERB_WERDEN"); }
			else { Strcat(buf, " worden"); }
			}
#endif
	} else {		/* game ended in character's death */
	    p = "VERB_SEIN tot"; /* EN p = "are dead"; */
	    switch (u.umortality) {
	    case 0:  impossible("tot ohne zu sterben?"); /* EN case 0:  impossible("dead without dying?"); */
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, " (zum %d%s Mal!)", u.umortality, /* EN default: Sprintf(buf, " (%d%s time!)", u.umortality, */
			     "."); /* EN ordin(u.umortality)); */
		     break;
	    }
	}

	if (p) enl_msg(You_, "VERB_SEIN ", p, buf); /* EN if (p) enl_msg(You_, "have been killed ", p, buf); */
    }

	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}

#ifdef DUMP_LOG
void
dump_enlightenment(final)
int final;
{
	int ltmp;
	char buf[BUFSZ];
	char buf2[BUFSZ];
	const char *enc_stat[] = { /* copied from botl.c */
	     "",
	     "burdened", /* EN "burdened", */ // TODO DE
	     "stressed", /* EN "stressed", */ // TODO DE
	     "strained", /* EN "strained", */ // TODO DE
	     "overtaxed", /* EN "overtaxed", */ // TODO DE
	     "overloaded" /* EN "overloaded" */ // TODO DE
	};
	char *youwere = "  SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_SEIN "; /* EN char *youwere = "  You were "; */
	char *youhad  = "  SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_HABEN "; /* EN char *youhad  = "  You had "; */
#ifdef GERMAN
	char *youcould  = "  SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_KOENNEN "; /* EN char *youhad  = "  You had "; */
#endif

	dump("", "Finale Eigenschaften"); /* EN dump("", "Final attributes"); */

#ifdef ELBERETH
	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
				"ARTIKEL_BESTIMMTER NOUN_HAND_OF_ELBERETH", /* EN "the Hand of Elbereth", */
				"ARTIKEL_BESTIMMTER NOUN_ENVOY_OF_BALANCE", /* EN "the Envoy of Balance", */
				"ARTIKEL_BESTIMMTER NOUN_GLORY_OF_ARIOCH" /* EN "the Glory of Arioch" */
	    };
	    dump(youwere,
		(char *)hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
#endif

	if (u.ualign.record >= 20)
		dump(youwere, "piously aligned"); /* EN dump(youwere, "piously aligned"); */ // TODO DE
	else if (u.ualign.record > 13)
	    dump(youwere, "devoutly aligned"); /* EN dump(youwere, "devoutly aligned"); */ // TODO DE
	else if (u.ualign.record > 8)
	    dump(youwere, "fervently aligned"); /* EN dump(youwere, "fervently aligned"); */ // TODO DE
	else if (u.ualign.record > 3)
	    dump(youwere, "stridently aligned"); /* EN dump(youwere, "stridently aligned"); */ // TODO DE
	else if (u.ualign.record == 3)
	    dump(youwere, "aligned"); /* EN dump(youwere, "aligned"); */ // TODO DE
	else if (u.ualign.record > 0)
	    dump(youwere, "haltingly aligned"); /* EN dump(youwere, "haltingly aligned"); */ // TODO DE
	else if (u.ualign.record == 0)
	    dump(youwere, "nominally aligned"); /* EN dump(youwere, "nominally aligned"); */ // TODO DE
	else if (u.ualign.record >= -3)	dump("  You have ", "strayed"); /* EN else if (u.ualign.record >= -3)	dump("  You have ", "strayed"); */ // TODO DE
	else if (u.ualign.record >= -8)	dump("  You have ", "sinned"); /* EN else if (u.ualign.record >= -8)	dump("  You have ", "sinned"); */ // TODO DE
	else dump("  You have ", "transgressed"); /* EN else dump("  You have ", "transgressed"); */ // TODO DE
#ifdef WIZARD
	if (wizard) {
		Sprintf(buf, " %d", u.ualign.record);
		dump("  Your alignment was ", buf); /* EN dump("  Your alignment was ", buf); */ // TODO DE
	}
#endif

	/*** Resistances to troubles ***/
	if (Fire_resistance) dump(youwere, "feuerresistent"); /* EN if (Fire_resistance) dump(youwere, "fire resistant"); */
	if (Cold_resistance) dump(youwere, "kälteresistent"); /* EN if (Cold_resistance) dump(youwere, "cold resistant"); */
	if (Sleep_resistance) dump(youwere, "schlafresistent"); /* EN if (Sleep_resistance) dump(youwere, "sleep resistant"); */
	if (Disint_resistance) dump(youwere, "disintegration-resistant"); /* EN if (Disint_resistance) dump(youwere, "disintegration-resistant"); */ // TODO DE
	if (Shock_resistance) dump(youwere, "shock resistant"); /* EN if (Shock_resistance) dump(youwere, "shock resistant"); */ // TODO DE
	if (Poison_resistance) dump(youwere, "giftresistent"); /* EN if (Poison_resistance) dump(youwere, "poison resistant"); */
	if (Drain_resistance) dump(youwere, "level-drain resistant"); /* EN if (Drain_resistance) dump(youwere, "level-drain resistant"); */ // TODO DE
	if (Sick_resistance) dump(youwere, "immune to sickness"); /* EN if (Sick_resistance) dump(youwere, "immune to sickness"); */ // TODO DE
	if (Antimagic) dump(youwere, "magic-protected"); /* EN if (Antimagic) dump(youwere, "magic-protected"); */ // TODO DE
	if (Acid_resistance) dump(youwere, "säureresistent"); /* EN if (Acid_resistance) dump(youwere, "acid resistant"); */
	if (Stone_resistance) dump(youwere, "petrification resistant"); /* EN if (Stone_resistance) dump(youwere, "petrification resistant"); */ // TODO DE
	if (Invulnerable) dump(youwere, "unverwundbar"); /* EN if (Invulnerable) dump(youwere, "invulnerable"); */
	if (u.uedibility) dump(youcould, "recognize detrimental food"); /* EN if (u.uedibility) dump("  You could ", "recognize detrimental food"); */ // TODO DE

	/*** Troubles ***/
	if (Halluc_resistance) 	dump("  ", "You resisted hallucinations"); /* EN if (Halluc_resistance) 	dump("  ", "You resisted hallucinations"); */ // TODO DE
	if (Hallucination) dump(youwere, "hallucinating"); /* EN if (Hallucination) dump(youwere, "hallucinating"); */ // TODO DE
	if (Stunned) dump(youwere, "benommen"); /* EN if (Stunned) dump(youwere, "stunned"); */
	if (Confusion) dump(youwere, "verwirrt"); /* EN if (Confusion) dump(youwere, "confused"); */
	if (Blinded) dump(youwere, "geblendet"); /* EN if (Blinded) dump(youwere, "blinded"); */
	if (Sick) {
		if (u.usick_type & SICK_VOMITABLE)
			dump(youwere, "krank wegen einer Lebensmittelvergiftung"); /* EN dump(youwere, "sick from food poisoning"); */
		if (u.usick_type & SICK_NONVOMITABLE)
			dump(youwere, "sick from illness"); /* EN dump(youwere, "sick from illness"); */ // TODO DE
	}
	if (Stoned) dump(youwere, "am Versteinern"); /* EN if (Stoned) dump(youwere, "turning to stone"); */
	if (Slimed) dump(youwere, "am Verschleimen"); /* EN if (Slimed) dump(youwere, "turning into slime"); */
	if (Strangled)
		dump(youwere, (u.uburied) ? "buried" : "being strangled"); /* EN dump(youwere, (u.uburied) ? "buried" : "being strangled"); */ // TODO DE
	if (Glib) {
		Sprintf(buf, "ADJEKTIV_FETTIG %s", makeplural(body_part(FINGER))); /* EN Sprintf(buf, "slippery %s", makeplural(body_part(FINGER))); */
		dump(youhad, buf);
	}
	if (Fumbling) dump("  ", "You fumbled"); /* EN if (Fumbling) dump("  ", "You fumbled"); */ // TODO DE
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
		Sprintf(buf, "ADJEKTIV_VERLETZT %s", makeplural(body_part(LEG))); /* EN Sprintf(buf, "wounded %s", makeplural(body_part(LEG))); */
		dump(youhad, buf);
	}
#if defined(WIZARD) && defined(STEED)
	if (Wounded_legs && u.usteed && wizard) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    Strcat(buf, " hatte verletzte Beine"); /* EN Strcat(buf, " had wounded legs"); */
	    dump("  ", buf);
	}
#endif
	if (Sleeping) dump("  ", "SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_HABEN Narkolepsie"); /* EN if (Sleeping) dump("  ", "You fell asleep"); */
	if (Hunger) dump("  ", "You hungered rapidly"); /* EN if (Hunger) dump("  ", "You hungered rapidly"); */ // TODO DE

	/*** Vision and senses ***/
	if (See_invisible) dump("  ", "SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_SEHEN Unsichtbare"); /* EN if (See_invisible) dump("  ", "You saw invisible"); */
	if (Blind_telepat) dump("  ", "SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_SEIN telepathisch begabt"); /* EN if (Blind_telepat) dump("  ", "You were telepathic"); */
	if (Warning) dump("  ", "SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_SEIN gewarnt"); /* EN if (Warning) dump("  ", "You were warned"); */
	if (Warn_of_mon && flags.warntype) {
		Sprintf(buf, "OBJECT KASUS_DATIV PRONOMEN_PERSONAL der Anwesenheit von KASUS_DATIV %s bewusst", /* EN Sprintf(buf, "aware of the presence of %s", */
			(flags.warntype & M2_ORC) ? "NOUN_ORCs" : /* EN (flags.warntype & M2_ORC) ? "orcs" : */
			(flags.warntype & M2_DEMON) ? "NOUN_DEMONs" : /* EN (flags.warntype & M2_DEMON) ? "demons" : */
			something); 
		dump(youwere, buf);
	}
	if (Undead_warning) dump(youwere, "warned of undead"); /* EN if (Undead_warning) dump(youwere, "warned of undead"); */ // TODO DE
	if (Searching) dump(youhad, "automatic searching"); /* EN if (Searching) dump(youhad, "automatic searching"); */ // TODO DE
	if (Clairvoyant) dump(youwere, "clairvoyant"); /* EN if (Clairvoyant) dump(youwere, "clairvoyant"); */ // TODO DE
	if (Infravision) dump(youhad, "Infravision"); /* EN if (Infravision) dump(youhad, "infravision"); */
	if (Detect_monsters)
	  dump(youwere, "sensing the presence of monsters"); /* EN dump(youwere, "sensing the presence of monsters"); */ // TODO DE
	if (u.umconf) dump(youwere, "going to confuse monsters"); /* EN if (u.umconf) dump(youwere, "going to confuse monsters"); */ // TODO DE

	/*** Appearance and behavior ***/
	if (Adornment) {
	    int adorn = 0;
	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		dump(youwere, "poorly adorned"); /* EN dump(youwere, "poorly adorned"); */ // TODO DE
	    else
		dump(youwere, "adorned"); /* EN dump(youwere, "adorned"); */ // TODO DE
	}
	if (Invisible) dump(youwere, "ADJEKTIV_INVISIBLE"); /* EN if (Invisible) dump(youwere, "invisible"); */
	else if (Invis) dump(youwere, "ADJEKTIV_INVISIBLE für andere"); /* EN else if (Invis) dump(youwere, "invisible to others"); */
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis)
	    dump(youwere, "sichtbar"); /* EN dump(youwere, "visible"); */
	if (Displaced) dump(youwere, "displaced"); /* EN if (Displaced) dump(youwere, "displaced"); */ // TODO DE
	if (Stealth) dump(youwere, "stealthy"); /* EN if (Stealth) dump(youwere, "stealthy"); */ // TODO DE
	if (Aggravate_monster) dump("  ", "You aggravated monsters"); /* EN if (Aggravate_monster) dump("  ", "You aggravated monsters"); */ // TODO DE
	if (Conflict) dump("  ", "You caused conflict"); /* EN if (Conflict) dump("  ", "You caused conflict"); */ // TODO DE

	/*** Transportation ***/
	if (Jumping) dump(youcould, "springen"); /* EN if (Jumping) dump("  You could ", "jump"); */
	if (Teleportation) dump(youcould, "teleportieren"); /* EN if (Teleportation) dump("  You could ", "teleport"); */
	if (Teleport_control) dump(youhad, "teleport control"); /* EN if (Teleport_control) dump(youhad, "teleport control"); */ // TODO DE
	if (Lev_at_will) dump(youwere, "levitating, at will"); /* EN if (Lev_at_will) dump(youwere, "levitating, at will"); */ // TODO DE
	else if (Levitation)
	  dump(youwere, "levitating");	/* without control */ /* EN dump(youwere, "levitating");	*/ // TODO DE
	else if (Flying) dump(youcould, "fliegen"); /* EN else if (Flying) dump("  You could ", "fly"); */
	if (Wwalking) dump(youcould, "walk on water"); /* EN if (Wwalking) dump("  You could ", "walk on water"); */ // TODO DE
	if (Swimming) dump(youcould, "schwimmen"); /* EN if (Swimming) dump("  You could ", "swim"); */
	if (Breathless) dump(youcould, "survive without air"); /* EN if (Breathless) dump("  You could ", "survive without air"); */ // TODO DE
	else if (Amphibious) dump(youcould, "Wasser atmen"); /* EN else if (Amphibious) dump("  You could ", "breathe water"); */
	if (Passes_walls) dump(youcould, "durch Wände gehen"); /* EN if (Passes_walls) dump("  You could ", "walk through walls"); */
#ifdef STEED
	if (u.usteed && (final < 2 || strcmp(killer, "NOUN_REITUNFALL"))) { /* EN if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) { */
	    Sprintf(buf, "riding %s", y_monnam(u.usteed)); /* EN Sprintf(buf, "riding %s", y_monnam(u.usteed)); */ // TODO DE
	    dump(youwere, buf);
	}
#endif
	if (u.uswallow) {
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck)); /* EN Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck)); */ // TODO DE
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif
	    dump(youwere, buf);
	} else if (u.ustuck) {
	    Sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by", /* EN (Upolyd && sticks(youmonst.data)) ? "holding" : "held by", */ // TODO DE
		    a_monnam(u.ustuck));
	    dump(youwere, buf);
	}

	/*** Physical attributes ***/
	if (u.uhitinc)
	    dump(youhad,
		enlght_combatinc("NOUN_TREFFER", u.uhitinc, final, buf)); /* EN enlght_combatinc("to hit", u.uhitinc, final, buf)); */
	if (u.udaminc)
	    dump(youhad,
		enlght_combatinc("NOUN_SCHADENS", u.udaminc, final, buf)); /* EN enlght_combatinc("damage", u.udaminc, final, buf)); */
	if (Slow_digestion) dump(youhad, "eine verlangsamte Verdauung"); /* EN if (Slow_digestion) dump(youhad, "slower digestion"); */
	if (Regeneration) dump("  ", "You regenerated"); /* EN if (Regeneration) dump("  ", "You regenerated"); */ // TODO DE
	if (u.uspellprot || Protection) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;
	    
	    if (prot < 0)
		dump(youwere, "ineffectively protected"); /* EN dump(youwere, "ineffectively protected"); */ // TODO DE
	    else
		dump(youwere, "protected"); /* EN dump(youwere, "protected"); */ // TODO DE
	}
	if (Protection_from_shape_changers)
		dump(youwere, "protected from shape changers"); /* EN dump(youwere, "protected from shape changers"); */ // TODO DE
	if (Polymorph) dump(youwere, "polymorphing"); /* EN if (Polymorph) dump(youwere, "polymorphing"); */ // TODO DE
	if (Polymorph_control) dump(youhad, "polymorph control"); /* EN if (Polymorph_control) dump(youhad, "polymorph control"); */ // TODO DE
	if (u.ulycn >= LOW_PM) {
		Strcpy(buf, "OBJECT KASUS_NOMINATIV "); Strcat(buf, an(mons[u.ulycn].mname)); /* EN Strcpy(buf, an(mons[u.ulycn].mname)); */
		dump(youwere, buf);
	}
	if (Upolyd) {
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form"); /* EN if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form"); */ // TODO DE
	    else Sprintf(buf, "polymorphed into %s", /* EN else Sprintf(buf, "polymorphed into %s", */ // TODO DE
			 an(youmonst.data->mname));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    dump(youwere, buf);
	}
	if (Unchanging)
	  dump(youcould, "not change from your current form"); /* EN dump("  You could ", "not change from your current form"); */ // TODO DE
	if (Fast) dump(youwere, Very_fast ? "very fast" : "fast"); /* EN if (Fast) dump(youwere, Very_fast ? "very fast" : "fast"); */ // TODO DE
	if (Reflecting) dump(youhad, "reflection"); /* EN if (Reflecting) dump(youhad, "reflection"); */ // TODO DE
	if (Free_action) dump(youhad, "free action"); /* EN if (Free_action) dump(youhad, "free action"); */ // TODO DE
	if (Fixed_abil) dump(youhad, "fixed abilities"); /* EN if (Fixed_abil) dump(youhad, "fixed abilities"); */ // TODO DE
	if (Lifesaved)
		dump("  ", "SUBJECT PRONOMEN_POSSESSIV NOUN_LEBEN wäre gerettet worden"); /* EN dump("  ", "Your life would have been saved"); */
	if (u.twoweap) dump(youwere, "wielding two weapons at once"); /* EN if (u.twoweap) dump(youwere, "wielding two weapons at once"); */ // TODO DE

	/*** Miscellany ***/
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%s (%d)", /* EN Sprintf(buf, "%s%slucky (%d)", */
		    ltmp >= 10 ? "außerordentliches " : ltmp >= 5 ? "sehr viel " : "", /* EN ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "", */
		    Luck < 0 ? "Pech" : "Glück", Luck); /* EN Luck < 0 ? "un" : "", Luck); */
	    dump(youhad, buf); /* EN dump(youwere, buf); */
	}
#ifdef WIZARD
	 else if (wizard) dump("  ", "Your luck was zero"); /* EN else if (wizard) dump("  ", "Your luck was zero"); */ // TODO DE
#endif
	if (u.moreluck > 0) dump(youhad, "besonders viel Glück"); /* EN if (u.moreluck > 0) dump(youhad, "extra luck"); */
	else if (u.moreluck < 0) dump(youhad, "vermindertes Glück"); /* EN else if (u.moreluck < 0) dump(youhad, "reduced luck"); */
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		dump("  ", "SUBJECT PRONOMEN_POSSESSIV NOUN_PECHSTRAEHNE brach nicht ab"); /* EN dump("  ", "Bad luck did not time out for you"); */
	    if (ltmp >= 0)
		dump("  ", "SUBJECT PRONOMEN_POSSESSIV NOUN_GLUECKSSTRAEHNE brach nicht ab"); /* EN dump("  ", "Good luck did not time out for you"); */
	}

	if (u.ugangr) {
	    Sprintf(buf, " %sangry with you", /* EN Sprintf(buf, " %sangry with you", */ // TODO DE
		u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : ""); /* EN u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : ""); */ // TODO DE
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ugangr);
#endif
	    Sprintf(buf2, "%s was %s", u_gname(), buf); /* EN Sprintf(buf2, "%s was %s", u_gname(), buf); */ // TODO DE
	    dump("  ", buf2);
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* quit/escaped/ascended */
	    p = "VERB_HABEN überlebt, obwohl NEUER_SATZ SUBJECT_IM_SATZ PRONOMEN_PERSONAL "; /* EN p = "survived after being killed "; */
	    switch (u.umortality) {
	    case 0:  p = "VERB_HABEN überlebt";  break; /* EN case 0:  p = "survived";  break; */
	    case 1:  Strcpy(buf, "einmal getötet");  break; /* EN case 1:  Strcpy(buf, "once");  break; */
	    case 2:  Strcpy(buf, "zweimal getötet");  break; /* EN case 2:  Strcpy(buf, "twice");  break; */
	    case 3:  Strcpy(buf, "dreimal getötet");  break; /* EN case 3:  Strcpy(buf, "thrice");  break; */
	    default: Sprintf(buf, "%d-mal getötet", u.umortality); /* EN default: Sprintf(buf, "%d times", u.umortality); */
		     break;
	    }
#ifdef GERMAN
			if (u.umortality > 0) {
			if (final) { Strcat(buf, " MODIFIER_VERB_PRAETERITUM VERB_WERDEN"); }
			else { Strcat(buf, " worden"); }
			}
#endif
	} else {		/* game ended in character's death */
	    p = "VERB_SEIN tot"; /* EN p = "are dead"; */
	    switch (u.umortality) {
	    case 0:  impossible("tot ohne zu sterben?"); /* EN case 0:  impossible("dead without dying?"); */
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, " (zum %d%s Mal!)", u.umortality, /* EN default: Sprintf(buf, " (%d%s time!)", u.umortality, */
			     "."); /* EN ordin(u.umortality)); */
		     break;
	    }
	}
	if (p) {
	  Sprintf(buf2, "SUBJECT PRONOMEN_PERSONAL %s %s", p, buf); /* EN Sprintf(buf2, "You %s %s", p, buf); */
	  dump("  ", buf2);
	}
    }
	dump("", "");
	return;

} /* dump_enlightenment */
#endif

/*
 * Courtesy function for non-debug, non-explorer mode players
 * to help refresh them about who/what they are.
 * Returns FALSE if menu cancelled (dismissed with ESC), TRUE otherwise.
 */
STATIC_OVL boolean
minimal_enlightenment()
{
	winid tmpwin;
	menu_item *selected;
	anything any;
	int genidx, n;
	char buf[BUFSZ], buf2[BUFSZ];
	static const char fmtstr[] = "%-15s: %-12s";
	static const char deity_fmtstr[] = "%-17s%s";
#ifdef GERMAN
	char name_str[BUFSZ];
	Strcpy(name_str, german("NOUN_NAME"));
	char race_str[BUFSZ];
	Strcpy(race_str, german("NOUN_RACE"));
	char role_str[BUFSZ];
	Strcpy(role_str, german("NOUN_ROLE"));
	char gender_str[BUFSZ];
	Strcpy(gender_str, german("NOUN_GENDER"));
	char alignment_str[BUFSZ];
	Strcpy(alignment_str, german("NOUN_ALIGNMENT"));
#endif

	any.a_void = 0;
	buf[0] = buf2[0] = '\0';
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, "Start", FALSE); /* EN add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, "Starting", FALSE); */ // TODO DE

	/* Starting name, race, role, gender */
	Sprintf(buf, fmtstr, name_str, plname); /* EN Sprintf(buf, fmtstr, "name", plname); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, race_str, german(urace.noun)); /* EN Sprintf(buf, fmtstr, "race", urace.noun); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, role_str, /* EN Sprintf(buf, fmtstr, "role", */
		(flags.initgend && urole.name.f) ? german(urole.name.f) : german(urole.name.m));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, gender_str, german(genders[flags.initgend].adj)); /* EN Sprintf(buf, fmtstr, "gender", genders[flags.initgend].adj); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Starting alignment */
	Sprintf(buf, fmtstr, alignment_str, german(align_str(u.ualignbase[A_ORIGINAL]))); /* EN Sprintf(buf, fmtstr, "alignment", align_str(u.ualignbase[A_ORIGINAL])); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Current name, race, role, gender */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, "Aktuell", FALSE); /* EN add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, "Current", FALSE); */
	Sprintf(buf, fmtstr, race_str, Upolyd ? german(youmonst.data->mname) : german(urace.noun)); /* EN Sprintf(buf, fmtstr, "race", Upolyd ? youmonst.data->mname : urace.noun); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd) {
	    Sprintf(buf, fmtstr, "NOUN_ROLE (base)", /* EN Sprintf(buf, fmtstr, "role (base)", */ // TODO DE
		(u.mfemale && urole.name.f) ? urole.name.f : urole.name.m);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	} else {
	    Sprintf(buf, fmtstr, role_str, /* EN Sprintf(buf, fmtstr, "role", */
		(flags.female && urole.name.f) ? german(urole.name.f) : german(urole.name.m));
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}
	/* don't want poly_gender() here; it forces `2' for non-humanoids */
	genidx = is_neuter(youmonst.data) ? 2 : flags.female;
	Sprintf(buf, fmtstr, gender_str, german(genders[genidx].adj)); /* EN Sprintf(buf, fmtstr, "gender", genders[genidx].adj); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd && (int)u.mfemale != genidx) {
	    Sprintf(buf, fmtstr, "NOUN_GENDER (base)", german(genders[u.mfemale].adj)); /* EN Sprintf(buf, fmtstr, "gender (base)", genders[u.mfemale].adj); */
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}

	/* Current alignment */
	Sprintf(buf, fmtstr, alignment_str, german(align_str(u.ualign.type))); /* EN Sprintf(buf, fmtstr, "alignment", align_str(u.ualign.type)); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Deity list */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, "Gottheiten", FALSE); /* EN add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, "Deities", FALSE); */ // TODO DE
	Sprintf(buf2, deity_fmtstr, align_gname(A_CHAOTIC),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_CHAOTIC) ? " (s,a)" : /* EN && u.ualign.type == A_CHAOTIC) ? " (s,c)" : */
	    (u.ualignbase[A_ORIGINAL] == A_CHAOTIC)       ? " (s)" : /* EN (u.ualignbase[A_ORIGINAL] == A_CHAOTIC)       ? " (s)" : */
	    (u.ualign.type   == A_CHAOTIC)       ? " (a)" : ""); /* EN (u.ualign.type   == A_CHAOTIC)       ? " (c)" : ""); */
	Sprintf(buf, fmtstr, german("ADJEKTIV_CHAOTIC"), buf2); /* EN Sprintf(buf, fmtstr, "Chaotic", buf2); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_NEUTRAL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_NEUTRAL) ? " (s,a)" : /* EN && u.ualign.type == A_NEUTRAL) ? " (s,c)" : */
	    (u.ualignbase[A_ORIGINAL] == A_NEUTRAL)       ? " (s)" : /* EN (u.ualignbase[A_ORIGINAL] == A_NEUTRAL)       ? " (s)" : */
	    (u.ualign.type   == A_NEUTRAL)       ? " (a)" : ""); /* EN (u.ualign.type   == A_NEUTRAL)       ? " (c)" : ""); */
	Sprintf(buf, fmtstr, german("ADJEKTIV_NEUTRAL"), buf2); /* EN Sprintf(buf, fmtstr, "Neutral", buf2); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_LAWFUL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type &&
		u.ualign.type == A_LAWFUL)  ? " (s,a)" : /* EN u.ualign.type == A_LAWFUL)  ? " (s,c)" : */
	    (u.ualignbase[A_ORIGINAL] == A_LAWFUL)        ? " (s)" : /* EN (u.ualignbase[A_ORIGINAL] == A_LAWFUL)        ? " (s)" : */
	    (u.ualign.type   == A_LAWFUL)        ? " (a)" : ""); /* EN (u.ualign.type   == A_LAWFUL)        ? " (c)" : ""); */
	Sprintf(buf, fmtstr, german("ADJEKTIV_LAWFUL"), buf2); /* EN Sprintf(buf, fmtstr, "Lawful", buf2); */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	end_menu(tmpwin, "Grundeigenschaften"); /* EN end_menu(tmpwin, "Base Attributes"); */
	n = select_menu(tmpwin, PICK_NONE, &selected);
	destroy_nhwindow(tmpwin);
	return (n != -1);
}

STATIC_PTR int
doattributes()
{
	if (!minimal_enlightenment())
		return 0;
	if (wizard || discover)
		enlightenment(0);
	return 0;
}

/* KMH, #conduct
 * (shares enlightenment's tense handling)
 */
STATIC_PTR int
doconduct()
{
	show_conduct(0);
	return 0;
}

void
show_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

	/* Create the conduct window */
	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Freiwilliges Gebaren:"); /* EN putstr(en_win, 0, "Voluntary challenges:"); */
	putstr(en_win, 0, "");

	if (!u.uconduct.food)
	    enl_msg(You_, "VERB_KOMMEN ohne Nahrung aus", "VERB_SEIN ohne Nahrung ausgekommen", ""); /* EN enl_msg(You_, "have gone", "went", " without food"); */
	    /* But beverages are okay */
	else if (!u.uconduct.unvegan)
	    you_have_X("eine streng vegane Diät befolgt"); /* EN you_have_X("followed a strict vegan diet"); */
	else if (!u.uconduct.unvegetarian)
	    you_are("ein Vegetarier"); /* EN you_have_been("vegetarian"); */

	if (!u.uconduct.gnostic)
	    you_are("ein Atheist"); /* EN you_have_been("an atheist"); */

	if (!u.uconduct.weaphit)
	    you_have_never("mit einer geführten Waffe zugeschlagen"); /* EN you_have_never("hit with a wielded weapon"); */
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "used a wielded weapon %ld time%s", /* EN Sprintf(buf, "used a wielded weapon %ld time%s", */ // TODO DE
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    you_have_X(buf);
	}
#endif
	if (!u.uconduct.killer)
	    you_are("ein Pazifist"); /* EN you_have_been("a pacifist"); */

	if (!u.uconduct.literate)
	    you_are("ein Analphabet"); /* EN you_have_been("illiterate"); */
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "read items or engraved %ld time%s", /* EN Sprintf(buf, "read items or engraved %ld time%s", */ // TODO DE
		    u.uconduct.literate, plur(u.uconduct.literate));
	    you_have_X(buf);
	}
#endif

	ngenocided = num_genocides();
	if (ngenocided == 0) {
	    you_have_never("irgendwelche Wesen ausgerottet"); /* EN you_have_never("genocided any monsters"); */
	} else {
	    Sprintf(buf, "genocided %d type%s of monster%s", /* EN Sprintf(buf, "genocided %d type%s of monster%s", */ // TODO DE
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    you_have_X(buf);
	}

	if (!u.uconduct.polypiles)
	    you_have_never("ein Objekt transformiert"); /* EN you_have_never("polymorphed an object"); */
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "polymorphed %ld item%s", /* EN Sprintf(buf, "polymorphed %ld item%s", */ // TODO DE
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    you_have_X(buf);
	}
#endif

	if (!u.uconduct.polyselfs)
	    you_have_never("die Daseinsform gewechselt"); /* EN you_have_never("changed form"); */
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "changed form %ld time%s", /* EN Sprintf(buf, "changed form %ld time%s", */ // TODO DE
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    you_have_X(buf);
	}
#endif

	if (!u.uconduct.wishes)
	    you_have_X("keine Wünsche benutzt"); /* EN you_have_X("used no wishes"); */
	else {
	    Sprintf(buf, "used %ld wish%s", /* EN Sprintf(buf, "used %ld wish%s", */ // TODO DE
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : ""); /* EN u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : ""); */ // TODO DE
	    you_have_X(buf);

	    if (!u.uconduct.wisharti)
		enl_msg(You_, "have not wished", "did not wish", /* EN enl_msg(You_, "have not wished", "did not wish", */ // TODO DE
			" for any artifacts"); /* EN " for any artifacts"); */ // TODO DE
	}

	/* Pop up the window and wait for a key */
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
}

#ifdef DUMP_LOG
void
dump_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

	dump("", "Freiwilliges Gebaren"); /* EN dump("", "Voluntary challenges"); */

	if (!u.uconduct.food)
	    dump("", "  SUBJECT PRONOMEN_PERSONAL VERB_SEIN ohne Nahrung ausgekommen"); /* EN dump("", "  You went without food"); */
	    /* But beverages are okay */
	else if (!u.uconduct.unvegan)
	    dump("", "  SUBJECT PRONOMEN_PERSONAL eine streng vegane Diät befolgt"); /* EN dump("", "  You followed a strict vegan diet"); */
	else if (!u.uconduct.unvegetarian)
	    dump("", "  You were a vegetarian"); /* EN dump("", "  You were a vegetarian"); */ // TODO DE
	else if (Role_if(PM_MONK) && u.uconduct.unvegetarian < 10) {
	    sprintf(buf, "  You ate non-vegetarian food %ld time%s.", /* EN sprintf(buf, "  You ate non-vegetarian food %ld time%s.",  */ // TODO DE
		u.uconduct.unvegetarian, plur(u.uconduct.unvegetarian));
	    dump("", buf);
	}

	if (!u.uconduct.gnostic)
	    dump("", "  You were an atheist"); /* EN dump("", "  You were an atheist"); */ // TODO DE

	if (!u.uconduct.weaphit)
	    dump("", "  You never hit with a wielded weapon"); /* EN dump("", "  You never hit with a wielded weapon"); */ // TODO DE
	else if (Role_if(PM_MONK) && u.uconduct.weaphit < 10) {
	    Sprintf(buf, "hit with a wielded weapon %ld time%s", /* EN Sprintf(buf, "hit with a wielded weapon %ld time%s", */ // TODO DE
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    dump("  You have ", buf); /* EN dump("  You have ", buf); */ // TODO DE
	}
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "hit with a wielded weapon %ld time%s", /* EN Sprintf(buf, "hit with a wielded weapon %ld time%s", */ // TODO DE
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    dump("  You have ", buf); /* EN dump("  You have ", buf); */ // TODO DE
	}
#endif
	if (!u.uconduct.killer)
	    dump("", "  SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_SEIN ein Pazifist"); /* EN dump("", "  You were a pacifist"); */

	if (!u.uconduct.literate)
	    dump("", "  SUBJECT PRONOMEN_PERSONAL MODIFIER_VERB_PRAETERITUM VERB_SEIN ein Analphabet"); /* EN dump("", "  You were illiterate"); */
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "read items or engraved %ld time%s", /* EN Sprintf(buf, "read items or engraved %ld time%s", */ // TODO DE
		    u.uconduct.literate, plur(u.uconduct.literate));
	    dump("  You ", buf); /* EN dump("  You ", buf); */ // TODO DE
	}
#endif

	ngenocided = num_genocides();
	if (ngenocided == 0) {
	    dump("", "  SUBJECT PRONOMEN_PERSONAL VERB_HAVE niemals irgendwelche Wesen ausgerottet"); /* EN dump("", "  You never genocided any monsters"); */
	} else {
	    Sprintf(buf, "genocided %d type%s of monster%s", /* EN Sprintf(buf, "genocided %d type%s of monster%s", */ // TODO DE
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    dump("  You ", buf); /* EN dump("  You ", buf); */ // TODO DE
	}

	if (!u.uconduct.polypiles)
	    dump("", "  SUBJECT PRONOMEN_PERSONAL VERB_HABEN nie ein Objekt transformiert"); /* EN dump("", "  You never polymorphed an object"); */
	else {
	    Sprintf(buf, "polymorphed %ld item%s", /* EN Sprintf(buf, "polymorphed %ld item%s", */ // TODO DE
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    dump("  You ", buf); /* EN dump("  You ", buf); */ // TODO DE
	}

	if (!u.uconduct.polyselfs)
	    dump("", "  You never changed form"); /* EN dump("", "  You never changed form"); */ // TODO DE
	else {
	    Sprintf(buf, "changed form %ld time%s", /* EN Sprintf(buf, "changed form %ld time%s", */ // TODO DE
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    dump("  You ", buf); /* EN dump("  You ", buf); */ // TODO DE
	}

	if (!u.uconduct.wishes)
	    dump("", "  You used no wishes"); /* EN dump("", "  You used no wishes"); */ // TODO DE
	else {
	    Sprintf(buf, "used %ld wish%s", /* EN Sprintf(buf, "used %ld wish%s", */ // TODO DE
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : ""); /* EN u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : ""); */ // TODO DE
	    dump("  You ", buf); /* EN dump("  You ", buf); */ // TODO DE

	    if (!u.uconduct.wisharti)
		dump("", "  You did not wish for any artifacts"); /* EN dump("", "  You did not wish for any artifacts"); */ // TODO DE
	}

	dump("", "");
}
#endif /* DUMP_LOG */

#endif /* OVLB */
#ifdef OVL1

#ifndef M
# ifndef NHSTDC
#  define M(c)		(0x80 | (c))
# else
#  define M(c)		((c) - 128)
# endif /* NHSTDC */
#endif
#ifndef C
#define C(c)		(0x1f & (c))
#endif

static const struct func_tab cmdlist[] = {
	{C('d'), FALSE, dokick}, /* "D" is for door!...?  Msg is in dokick.c */
#ifdef WIZARD
	{C('e'), TRUE, wiz_detect},
	{C('f'), TRUE, wiz_map},
	{C('g'), TRUE, wiz_genesis},
	{C('i'), TRUE, wiz_identify},
#endif
	{C('l'), TRUE, doredraw}, /* if number_pad is set */
#ifdef WIZARD
	{C('o'), TRUE, wiz_where},
#endif
	{C('p'), TRUE, doprev_message},
	{C('r'), TRUE, doredraw},
	{C('t'), TRUE, dotele},
#ifdef WIZARD
	{C('v'), TRUE, wiz_level_tele},
	{C('w'), TRUE, wiz_wish},
#endif
	{C('x'), TRUE, doattributes},
#ifdef SUSPEND
	{C('z'), TRUE, dosuspend},
#endif
	{'a', FALSE, doapply},
	{'A', FALSE, doddoremarm},
	{M('a'), TRUE, doorganize},
/*	'b', 'B' : go sw */
	{'c', FALSE, doclose},
	{'C', TRUE, do_mname},
	{M('c'), TRUE, dotalk},
	{'d', FALSE, dodrop},
	{'D', FALSE, doddrop},
	{M('d'), FALSE, dodip},
	{'e', FALSE, doeat},
	{'E', FALSE, doengrave},
	{M('e'), TRUE, enhance_weapon_skill},
	{'f', FALSE, dofire},
/*	'F' : fight (one time) */
	{M('f'), FALSE, doforce},
/*	'g', 'G' : multiple go */
/*	'h', 'H' : go west */
	{'h', TRUE, dohelp}, /* if number_pad is set */
	{'i', TRUE, ddoinv},
	{'I', TRUE, dotypeinv},		/* Robert Viduya */
	{M('i'), TRUE, doinvoke},
/*	'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N' : move commands */
	{'j', FALSE, dojump}, /* if number_pad is on */
	{M('j'), FALSE, dojump},
	{'k', FALSE, dokick}, /* if number_pad is on */
	{'l', FALSE, doloot}, /* if number_pad is on */
	{M('l'), FALSE, doloot},
/*	'n' prefixes a count if number_pad is on */
	{M('m'), TRUE, domonability},
	{'N', TRUE, ddocall}, /* if number_pad is on */
	{M('n'), TRUE, ddocall},
	{M('N'), TRUE, ddocall},
	{'o', FALSE, doopen},
	{'O', TRUE, doset},
	{M('o'), FALSE, dosacrifice},
	{'p', FALSE, dopay},
	{'P', FALSE, doputon},
	{M('p'), TRUE, dopray},
	{'q', FALSE, dodrink},
	{'Q', FALSE, dowieldquiver},
	{M('q'), TRUE, done2},
	{'r', FALSE, doread},
	{'R', FALSE, doremring},
	{M('r'), FALSE, dorub},
	{'s', TRUE, dosearch, "searching"}, /* EN {'s', TRUE, dosearch, "searching"}, */ // TODO DE
	{'S', TRUE, dosave},
	{M('s'), FALSE, dosit},
	{'t', FALSE, dothrow},
	{'T', FALSE, dotakeoff},
	{M('t'), TRUE, doturn},
/*	'u', 'U' : go ne */
	{'u', FALSE, dountrap}, /* if number_pad is on */
	{M('u'), FALSE, dountrap},
	{'v', TRUE, doversion},
	{'V', TRUE, dohistory},
	{M('v'), TRUE, doextversion},
	{'w', FALSE, dowield},
	{'W', FALSE, dowear},
	{M('w'), FALSE, dowipe},
	{'x', FALSE, doswapweapon},
	{'X', TRUE, enter_explore_mode},
/*	'y', 'Y' : go nw */
	{'z', FALSE, dozap},
	{'Z', TRUE, docast},
	{'<', FALSE, doup},
	{'>', FALSE, dodown},
	{'/', TRUE, dowhatis},
	{'&', TRUE, dowhatdoes},
	{'?', TRUE, dohelp},
	{M('?'), TRUE, doextlist},
#ifdef SHELL
	{'!', TRUE, dosh},
#endif
	{'.', TRUE, donull, "waiting"}, /* EN {'.', TRUE, donull, "waiting"}, */ // TODO DE
	{' ', TRUE, donull, "waiting"}, /* EN {' ', TRUE, donull, "waiting"}, */ // TODO DE
	{',', FALSE, dopickup},
	{':', TRUE, dolook},
	{';', TRUE, doquickwhatis},
	{'^', TRUE, doidtrap},
	{'\\', TRUE, dodiscovered},		/* Robert Viduya */
	{'@', TRUE, dotogglepickup},
	{M('2'), FALSE, dotwoweapon},
	{WEAPON_SYM,  TRUE, doprwep},
	{ARMOR_SYM,  TRUE, doprarm},
	{RING_SYM,  TRUE, doprring},
	{AMULET_SYM, TRUE, dopramulet},
	{TOOL_SYM, TRUE, doprtool},
	{'*', TRUE, doprinuse},	/* inventory of all equipment in use */
	{GOLD_SYM, TRUE, doprgold},
	{SPBOOK_SYM, TRUE, dovspell},			/* Mike Stephenson */
	{'#', TRUE, doextcmd},
	{'_', TRUE, dotravel},
	{0,0,0,0}
};

struct ext_func_tab extcmdlist[] = {
	{"adjust", "adjust inventory letters", doorganize, TRUE}, /* EN {"adjust", "adjust inventory letters", doorganize, TRUE}, */ // TODO DE
	{"chat", "talk to someone", dotalk, TRUE},	/* converse? */ /* EN {"chat", "talk to someone", dotalk, TRUE},	*/ // TODO DE
	{"conduct", "list which challenges you have adhered to", doconduct, TRUE}, /* EN {"conduct", "list which challenges you have adhered to", doconduct, TRUE}, */ // TODO DE
	{"dip", "dip an object into something", dodip, FALSE}, /* EN {"dip", "dip an object into something", dodip, FALSE}, */ // TODO DE
	{"enhance", "advance or check weapons skills", enhance_weapon_skill, /* EN {"enhance", "advance or check weapons skills", enhance_weapon_skill, */ // TODO DE
							TRUE},
	{"force", "force a lock", doforce, FALSE}, /* EN {"force", "force a lock", doforce, FALSE}, */ // TODO DE
	{"invoke", "invoke an object's powers", doinvoke, TRUE}, /* EN {"invoke", "invoke an object's powers", doinvoke, TRUE}, */ // TODO DE
	{"jump", "jump to a location", dojump, FALSE}, /* EN {"jump", "jump to a location", dojump, FALSE}, */ // TODO DE
	{"loot", "loot a box on the floor", doloot, FALSE}, /* EN {"loot", "loot a box on the floor", doloot, FALSE}, */ // TODO DE
	{"monster", "use a monster's special ability", domonability, TRUE}, /* EN {"monster", "use a monster's special ability", domonability, TRUE}, */ // TODO DE
	{"name", "name an item or type of object", ddocall, TRUE}, /* EN {"name", "name an item or type of object", ddocall, TRUE}, */ // TODO DE
	{"offer", "offer a sacrifice to the gods", dosacrifice, FALSE}, /* EN {"offer", "offer a sacrifice to the gods", dosacrifice, FALSE}, */ // TODO DE
	{"pray", "pray to the gods for help", dopray, TRUE}, /* EN {"pray", "pray to the gods for help", dopray, TRUE}, */ // TODO DE
	{"abbrechen", "exit without saving current game", done2, TRUE}, /* EN {"quit", "exit without saving current game", done2, TRUE}, */ // TODO DE
#ifdef STEED
	{"ride", "ride (or stop riding) a monster", doride, FALSE}, /* EN {"ride", "ride (or stop riding) a monster", doride, FALSE}, */ // TODO DE
#endif
	{"rub", "rub a lamp or a stone", dorub, FALSE}, /* EN {"rub", "rub a lamp or a stone", dorub, FALSE}, */ // TODO DE
	{"sit", "sit down", dosit, FALSE}, /* EN {"sit", "sit down", dosit, FALSE}, */ // TODO DE
	{"turn", "turn undead", doturn, TRUE}, /* EN {"turn", "turn undead", doturn, TRUE}, */ // TODO DE
	{"twoweapon", "toggle two-weapon combat", dotwoweapon, FALSE}, /* EN {"twoweapon", "toggle two-weapon combat", dotwoweapon, FALSE}, */ // TODO DE
	{"untrap", "untrap something", dountrap, FALSE}, /* EN {"untrap", "untrap something", dountrap, FALSE}, */ // TODO DE
	{"version", "list compile time options for this version of NetHack", /* EN {"version", "list compile time options for this version of NetHack", */ // TODO DE
		doextversion, TRUE},
	{"wipe", "wipe off your face", dowipe, FALSE}, /* EN {"wipe", "wipe off your face", dowipe, FALSE}, */ // TODO DE
	{"?", "get this list of extended commands", doextlist, TRUE}, /* EN {"?", "get this list of extended commands", doextlist, TRUE}, */ // TODO DE
#if defined(WIZARD)
	/*
	 * There must be a blank entry here for every entry in the table
	 * below.
	 */
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef PORT_DEBUG
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE}, /* showkills (showborn patch) */
        {(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef DEBUG
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE}	/* sentinel */
};

#if defined(WIZARD)
static const struct ext_func_tab debug_extcmdlist[] = {
	{"levelchange", "change experience level", wiz_level_change, TRUE}, /* EN {"levelchange", "change experience level", wiz_level_change, TRUE}, */ // TODO DE
	{"lightsources", "show mobile light sources", wiz_light_sources, TRUE}, /* EN {"lightsources", "show mobile light sources", wiz_light_sources, TRUE}, */ // TODO DE
	{"monpolycontrol", "control monster polymorphs", wiz_mon_polycontrol, TRUE}, /* EN {"monpolycontrol", "control monster polymorphs", wiz_mon_polycontrol, TRUE}, */ // TODO DE
	{"panic", "test panic routine (fatal to game)", wiz_panic, TRUE}, /* EN {"panic", "test panic routine (fatal to game)", wiz_panic, TRUE}, */ // TODO DE
	{"polyself", "polymorph self", wiz_polyself, TRUE}, /* EN {"polyself", "polymorph self", wiz_polyself, TRUE}, */ // TODO DE
#ifdef PORT_DEBUG
	{"portdebug", "wizard port debug command", wiz_port_debug, TRUE}, /* EN {"portdebug", "wizard port debug command", wiz_port_debug, TRUE}, */ // TODO DE
#endif
	{"seenv", "show seen vectors", wiz_show_seenv, TRUE}, /* EN {"seenv", "show seen vectors", wiz_show_seenv, TRUE}, */ // TODO DE
	{"showkills", "show list of monsters killed", wiz_showkills, TRUE}, /* EN {"showkills", "show list of monsters killed", wiz_showkills, TRUE}, */ // TODO DE
	{"stats", "show memory statistics", wiz_show_stats, TRUE}, /* EN {"stats", "show memory statistics", wiz_show_stats, TRUE}, */ // TODO DE
	{"timeout", "look at timeout queue", wiz_timeout_queue, TRUE}, /* EN {"timeout", "look at timeout queue", wiz_timeout_queue, TRUE}, */ // TODO DE
	{"vision", "show vision array", wiz_show_vision, TRUE}, /* EN {"vision", "show vision array", wiz_show_vision, TRUE}, */ // TODO DE
#ifdef DEBUG
	{"wizdebug", "wizard debug command", wiz_debug_cmd, TRUE}, /* EN {"wizdebug", "wizard debug command", wiz_debug_cmd, TRUE}, */ // TODO DE
#endif
	{"wmode", "show wall modes", wiz_show_wmodes, TRUE}, /* EN {"wmode", "show wall modes", wiz_show_wmodes, TRUE}, */ // TODO DE
	{(char *)0, (char *)0, donull, TRUE}
};

/*
 * Insert debug commands into the extended command list.  This function
 * assumes that the last entry will be the help entry.
 *
 * You must add entries in ext_func_tab every time you add one to the
 * debug_extcmdlist().
 */
void
add_debug_extended_commands()
{
	int i, j, k, n;

	/* count the # of help entries */
	for (n = 0; extcmdlist[n].ef_txt[0] != '?'; n++)
	    ;

	for (i = 0; debug_extcmdlist[i].ef_txt; i++) {
	    for (j = 0; j < n; j++)
		if (strcmp(debug_extcmdlist[i].ef_txt, extcmdlist[j].ef_txt) < 0) break;

	    /* insert i'th debug entry into extcmdlist[j], pushing down  */
	    for (k = n; k >= j; --k)
		extcmdlist[k+1] = extcmdlist[k];
	    extcmdlist[j] = debug_extcmdlist[i];
	    n++;	/* now an extra entry */
	}
}


static const char template[] = "%-18s %4ld  %6ld";
static const char count_str[] = "                   count  bytes";
static const char separator[] = "------------------ -----  ------";

STATIC_OVL void
count_obj(chain, total_count, total_size, top, recurse)
	struct obj *chain;
	long *total_count;
	long *total_size;
	boolean top;
	boolean recurse;
{
	long count, size;
	struct obj *obj;

	for (count = size = 0, obj = chain; obj; obj = obj->nobj) {
	    if (top) {
		count++;
		size += sizeof(struct obj) + obj->oxlth + obj->onamelth;
	    }
	    if (recurse && obj->cobj)
		count_obj(obj->cobj, total_count, total_size, TRUE, TRUE);
	}
	*total_count += count;
	*total_size += size;
}

STATIC_OVL void
obj_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct obj *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;

	count_obj(chain, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_invent_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	for (mon = chain; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
contained(win, src, total_count, total_size)
	winid win;
	const char *src;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	count_obj(invent, &count, &size, FALSE, TRUE);
	count_obj(fobj, &count, &size, FALSE, TRUE);
	count_obj(level.buriedobjlist, &count, &size, FALSE, TRUE);
	count_obj(migrating_objs, &count, &size, FALSE, TRUE);
	/* DEADMONSTER check not required in this loop since they have no inventory */
	for (mon = fmon; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);
	for (mon = migrating_mons; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);

	*total_count += count; *total_size += size;

	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count, size;
	struct monst *mon;

	for (count = size = 0, mon = chain; mon; mon = mon->nmon) {
	    count++;
	    size += sizeof(struct monst) + mon->mxlth + mon->mnamelth;
	}
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

/*
 * Display memory usage of all monsters and objects on the level.
 */
static int
wiz_show_stats()
{
	char buf[BUFSZ];
	winid win;
	long total_obj_size = 0, total_obj_count = 0;
	long total_mon_size = 0, total_mon_count = 0;

	win = create_nhwindow(NHW_TEXT);
	putstr(win, 0, "Current memory statistics:");
	putstr(win, 0, "");
	Sprintf(buf, "Objects, size %d", (int) sizeof(struct obj));
	putstr(win, 0, buf);
	putstr(win, 0, "");
	putstr(win, 0, count_str);

	obj_chain(win, "invent", invent, &total_obj_count, &total_obj_size);
	obj_chain(win, "fobj", fobj, &total_obj_count, &total_obj_size);
	obj_chain(win, "buried", level.buriedobjlist,
				&total_obj_count, &total_obj_size);
	obj_chain(win, "migrating obj", migrating_objs,
				&total_obj_count, &total_obj_size);
	mon_invent_chain(win, "minvent", fmon,
				&total_obj_count,&total_obj_size);
	mon_invent_chain(win, "migrating minvent", migrating_mons,
				&total_obj_count, &total_obj_size);

	contained(win, "contained",
				&total_obj_count, &total_obj_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_obj_count, total_obj_size);
	putstr(win, 0, buf);

	putstr(win, 0, "");
	putstr(win, 0, "");
	Sprintf(buf, "Monsters, size %d", (int) sizeof(struct monst));
	putstr(win, 0, buf);
	putstr(win, 0, "");

	mon_chain(win, "fmon", fmon,
				&total_mon_count, &total_mon_size);
	mon_chain(win, "migrating", migrating_mons,
				&total_mon_count, &total_mon_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_mon_count, total_mon_size);
	putstr(win, 0, buf);

#if defined(__BORLANDC__) && !defined(_WIN32)
	show_borlandc_stats(win);
#endif

	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return 0;
}

void
sanity_check()
{
	obj_sanity_check();
	timer_sanity_check();
}

#endif /* WIZARD */

#define unctrl(c)	((c) <= C('z') ? (0x60 | (c)) : (c))
#define unmeta(c)	(0x7f & (c))


void
rhack(cmd)
register char *cmd;
{
	boolean do_walk, do_rush, prefix_seen, bad_command,
		firsttime = (cmd == 0);

	iflags.menu_requested = FALSE;
	if (firsttime) {
		flags.nopick = 0;
		cmd = parse();
	}
	if (*cmd == '\033') {
		flags.move = FALSE;
		return;
	}
#ifdef REDO
	if (*cmd == DOAGAIN && !in_doagain && saveq[0]) {
		in_doagain = TRUE;
		stail = 0;
		rhack((char *)0);	/* read and execute command */
		in_doagain = FALSE;
		return;
	}
	/* Special case of *cmd == ' ' handled better below */
	if(!*cmd || *cmd == (char)0377)
#else
	if(!*cmd || *cmd == (char)0377 || (!flags.rest_on_space && *cmd == ' '))
#endif
	{
		nhbell();
		flags.move = FALSE;
		return;		/* probably we just had an interrupt */
	}
	if (iflags.num_pad && iflags.num_pad_mode == 1) {
		/* This handles very old inconsistent DOS/Windows behaviour
		 * in a new way: earlier, the keyboard handler mapped these,
		 * which caused counts to be strange when entered from the
		 * number pad. Now do not map them until here. 
		 */
		switch (*cmd) {
		    case '5':       *cmd = 'g'; break;
		    case M('5'):    *cmd = 'G'; break;
		    case M('0'):    *cmd = 'I'; break;
        	}
        }
	/* handle most movement commands */
	do_walk = do_rush = prefix_seen = FALSE;
	flags.travel = 0;
	switch (*cmd) {
	 case 'g':  if (movecmd(cmd[1])) {
			flags.run = 2;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case '5':  if (!iflags.num_pad) break;	/* else FALLTHRU */
	 case 'G':  if (movecmd(lowc(cmd[1]))) {
			flags.run = 3;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case '-':  if (!iflags.num_pad) break;	/* else FALLTHRU */
	/* Effects of movement commands and invisible monsters:
	 * m: always move onto space (even if 'I' remembered)
	 * F: always attack space (even if 'I' not remembered)
	 * normal movement: attack if 'I', move otherwise
	 */
	 case 'F':  if (movecmd(cmd[1])) {
			flags.forcefight = 1;
			do_walk = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case 'm':  if (movecmd(cmd[1]) || u.dz) {
			flags.run = 0;
			flags.nopick = 1;
			if (!u.dz) do_walk = TRUE;
			else cmd[0] = cmd[1];	/* "m<" or "m>" */
		    } else
			prefix_seen = TRUE;
		    break;
	 case 'M':  if (movecmd(lowc(cmd[1]))) {
			flags.run = 1;
			flags.nopick = 1;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case '0':  if (!iflags.num_pad) break;
		    (void)ddoinv(); /* a convenience borrowed from the PC */
		    flags.move = FALSE;
		    multi = 0;
		    return;
	 case CMD_TRAVEL:
		    if (iflags.travelcmd) {
			    flags.travel = 1;
			    flags.run = 8;
			    flags.nopick = 1;
			    do_rush = TRUE;
			    break;
		    }
		    /*FALLTHRU*/
	 default:   if (movecmd(*cmd)) {	/* ordinary movement */
			flags.run = 0;	/* only matters here if it was 8 */
			do_walk = TRUE;
		    } else if (movecmd(iflags.num_pad ?
				       unmeta(*cmd) : lowc(*cmd))) {
			flags.run = 1;
			do_rush = TRUE;
		    } else if (movecmd(unctrl(*cmd))) {
			flags.run = 3;
			do_rush = TRUE;
		    }
		    break;
	}

	/* some special prefix handling */
	/* overload 'm' prefix for ',' to mean "request a menu" */
	if (prefix_seen && cmd[1] == ',') {
		iflags.menu_requested = TRUE;
		++cmd;
	}

	if (do_walk) {
	    if (multi) flags.mv = TRUE;
	    domove();
	    flags.forcefight = 0;
	    return;
	} else if (do_rush) {
	    if (firsttime) {
		if (!multi) multi = max(COLNO,ROWNO);
		u.last_str_turn = 0;
	    }
	    flags.mv = TRUE;
	    domove();
	    return;
	} else if (prefix_seen && cmd[1] == '\033') {	/* <prefix><escape> */
	    /* don't report "unknown command" for change of heart... */
	    bad_command = FALSE;
	} else if (*cmd == ' ' && !flags.rest_on_space) {
	    bad_command = TRUE;		/* skip cmdlist[] loop */

	/* handle all other commands */
	} else {
	    register const struct func_tab *tlist;
	    int res, NDECL((*func));

	    for (tlist = cmdlist; tlist->f_char; tlist++) {
		if ((*cmd & 0xff) != (tlist->f_char & 0xff)) continue;

		if (u.uburied && !tlist->can_if_buried) {
		    You_cant("do that while you are buried!"); /* EN You_cant("do that while you are buried!"); */ // TODO DE
		    res = 0;
		} else {
		    /* we discard 'const' because some compilers seem to have
		       trouble with the pointer passed to set_occupation() */
		    func = ((struct func_tab *)tlist)->f_funct;
		    if (tlist->f_text && !occupation && multi)
			set_occupation(func, tlist->f_text, multi);
		    res = (*func)();		/* perform the command */
		}
		if (!res) {
		    flags.move = FALSE;
		    multi = 0;
		}
		return;
	    }
	    /* if we reach here, cmd wasn't found in cmdlist[] */
	    bad_command = TRUE;
	}

	if (bad_command) {
	    char expcmd[10];
	    register char *cp = expcmd;

	    while (*cmd && (int)(cp - expcmd) < (int)(sizeof expcmd - 3)) {
		if (*cmd >= 040 && *cmd < 0177) {
		    *cp++ = *cmd++;
		} else if (*cmd & 0200) {
		    *cp++ = 'M';
		    *cp++ = '-';
		    *cp++ = *cmd++ &= ~0200;
		} else {
		    *cp++ = '^';
		    *cp++ = *cmd++ ^ 0100;
		}
	    }
	    *cp = '\0';
	    if (!prefix_seen || !iflags.cmdassist ||
		!help_dir(0, "Invalid direction key!")) /* EN !help_dir(0, "Invalid direction key!")) */ // TODO DE
		Norep("Unbekannter Befehl '%s'.", expcmd); /* EN Norep("Unknown command '%s'.", expcmd); */
	}
	/* didn't move */
	flags.move = FALSE;
	multi = 0;
	return;
}

int
xytod(x, y)	/* convert an x,y pair into a direction code */
schar x, y;
{
	register int dd;

	for(dd = 0; dd < 8; dd++)
	    if(x == xdir[dd] && y == ydir[dd]) return dd;

	return -1;
}

void
dtoxy(cc,dd)	/* convert a direction code into an x,y pair */
coord *cc;
register int dd;
{
	cc->x = xdir[dd];
	cc->y = ydir[dd];
	return;
}

int
movecmd(sym)	/* also sets u.dz, but returns false for <> */
char sym;
{
	register const char *dp;
	register const char *sdp;
	if(iflags.num_pad) sdp = ndir; else sdp = sdir;	/* DICE workaround */

	u.dz = 0;
	if(!(dp = index(sdp, sym))) return 0;
	u.dx = xdir[dp-sdp];
	u.dy = ydir[dp-sdp];
	u.dz = zdir[dp-sdp];
	if (u.dx && u.dy && u.umonnum == PM_GRID_BUG) {
		u.dx = u.dy = 0;
		return 0;
	}
	return !u.dz;
}

/*
 * uses getdir() but unlike getdir() it specifically
 * produces coordinates using the direction from getdir()
 * and verifies that those coordinates are ok.
 *
 * If the call to getdir() returns 0, Never_mind is displayed.
 * If the resulting coordinates are not okay, emsg is displayed.
 *
 * Returns non-zero if coordinates in cc are valid.
 */
int get_adjacent_loc(prompt,emsg,x,y,cc)
const char *prompt, *emsg;
xchar x,y;
coord *cc;
{
	xchar new_x, new_y;
	if (!getdir(prompt)) {
		pline(Never_mind);
		return 0;
	}
	new_x = x + u.dx;
	new_y = y + u.dy;
	if (cc && isok(new_x,new_y)) {
		cc->x = new_x;
		cc->y = new_y;
	} else {
		if (emsg) pline(emsg);
		return 0;
	}
	return 1;
}

int
getdir(s)
const char *s;
{
	char dirsym;

#ifdef REDO
	if(in_doagain || *readchar_queue)
	    dirsym = readchar();
	else
#endif
	    dirsym = yn_function ((s && *s != '^') ? s : "In what direction?", /* EN dirsym = yn_function ((s && *s != '^') ? s : "In what direction?", */ // TODO DE
					(char *)0, '\0');
#ifdef REDO
	savech(dirsym);
#endif
	if(dirsym == '.' || dirsym == 's')
		u.dx = u.dy = u.dz = 0;
	else if(!movecmd(dirsym) && !u.dz) {
		boolean did_help = FALSE;
		if(!index(quitchars, dirsym)) {
		    if (iflags.cmdassist) {
			did_help = help_dir((s && *s == '^') ? dirsym : 0,
					    "Invalid direction key!"); /* EN "Invalid direction key!"); */ // TODO DE
		    }
		    if (!did_help) pline("What a strange direction!"); /* EN if (!did_help) pline("What a strange direction!"); */ // TODO DE
		}
		return 0;
	}
	if(!u.dz && (Stunned || (Confusion && !rn2(5)))) confdir();
	return 1;
}

STATIC_OVL boolean
help_dir(sym, msg)
char sym;
const char *msg;
{
	char ctrl;
	winid win;
	static const char wiz_only_list[] = "EFGIOVW";
	char buf[BUFSZ], buf2[BUFSZ], *expl;

	win = create_nhwindow(NHW_TEXT);
	if (!win) return FALSE;
	if (msg) {
		Sprintf(buf, "cmdassist: %s", msg);
		putstr(win, 0, buf);
		putstr(win, 0, "");
	}
	if (letter(sym)) { 
	    sym = highc(sym);
	    ctrl = (sym - 'A') + 1;
	    if ((expl = dowhatdoes_core(ctrl, buf2))
		&& (!index(wiz_only_list, sym)
#ifdef WIZARD
		    || wizard
#endif
	                     )) {
		Sprintf(buf, "Are you trying to use ^%c%s?", sym, /* EN Sprintf(buf, "Are you trying to use ^%c%s?", sym, */ // TODO DE
			index(wiz_only_list, sym) ? "" :
			" as specified in the Guidebook"); /* EN " as specified in the Guidebook"); */ // TODO DE
		putstr(win, 0, buf);
		putstr(win, 0, "");
		putstr(win, 0, expl);
		putstr(win, 0, "");
		putstr(win, 0, "To use that command, you press"); /* EN putstr(win, 0, "To use that command, you press"); */ // TODO DE
		Sprintf(buf,
			"the <Ctrl> key, and the <%c> key at the same time.", sym); /* EN "the <Ctrl> key, and the <%c> key at the same time.", sym); */ // TODO DE
		putstr(win, 0, buf);
		putstr(win, 0, "");
	    }
	}
	if (iflags.num_pad && u.umonnum == PM_GRID_BUG) {
	    putstr(win, 0, "Zulässige Richtungstasten in KASUS_DATIV PRONOMEN_POSSESSIV momentanen NOUN_GESTALT (mit aktiviertem \"number_pad\") sind:"); /* EN putstr(win, 0, "Valid direction keys in your current form (with number_pad on) are:"); */
	    putstr(win, 0, "             8   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             2   ");
	} else if (u.umonnum == PM_GRID_BUG) {
	    putstr(win, 0, "Zulässige Richtungstasten in KASUS_DATIV PRONOMEN_POSSESSIV momentanen NOUN_GESTALT sind:"); /* EN putstr(win, 0, "Valid direction keys in your current form are:"); */
	    putstr(win, 0, "             k   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             j   ");
	} else if (iflags.num_pad) {
	    putstr(win, 0, "Zulässige Richtungstasten (mit aktiviertem \"number_pad\") sind:"); /* EN putstr(win, 0, "Valid direction keys (with number_pad on) are:"); */
	    putstr(win, 0, "          7  8  9");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          1  2  3");
	} else {
	    putstr(win, 0, "Zulässige Richtungstasten sind:"); /* EN putstr(win, 0, "Valid direction keys are:"); */
	    putstr(win, 0, "          y  k  u");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          b  j  n");
	};
	putstr(win, 0, "");
	putstr(win, 0, "          <  rauf"); /* EN putstr(win, 0, "          <  up"); */
	putstr(win, 0, "          >  runter"); /* EN putstr(win, 0, "          >  down"); */
	putstr(win, 0, "          .  direct at yourself"); /* EN putstr(win, 0, "          .  direct at yourself"); */ // TODO DE
	putstr(win, 0, "");
	putstr(win, 0, "(Diese Information kann mit !cmdassist in der Konfigdatei unterdrückt werden.)"); /* EN putstr(win, 0, "(Suppress this message with !cmdassist in config file.)"); */
	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return TRUE;
}

#endif /* OVL1 */
#ifdef OVLB

void
confdir()
{
	register int x = (u.umonnum == PM_GRID_BUG) ? 2*rn2(4) : rn2(8);
	u.dx = xdir[x];
	u.dy = ydir[x];
	return;
}

#endif /* OVLB */
#ifdef OVL0

int
isok(x,y)
register int x, y;
{
	/* x corresponds to curx, so x==1 is the first column. Ach. %% */
	return x >= 1 && x <= COLNO-1 && y >= 0 && y <= ROWNO-1;
}

static NEARDATA int last_multi;

/*
 * convert a MAP window position into a movecmd
 */
const char *
click_to_cmd(x, y, mod)
    int x, y, mod;
{
    int dir;
    static char cmd[4];
    cmd[1]=0;

    x -= u.ux;
    y -= u.uy;

    if (iflags.travelcmd) {
        if (abs(x) <= 1 && abs(y) <= 1 ) {
            x = sgn(x), y = sgn(y);
        } else {
            u.tx = u.ux+x;
            u.ty = u.uy+y;
            cmd[0] = CMD_TRAVEL;
            return cmd;
        }

        if(x == 0 && y == 0) {
            /* here */
            if(IS_FOUNTAIN(levl[u.ux][u.uy].typ) || IS_SINK(levl[u.ux][u.uy].typ)) {
                cmd[0]=mod == CLICK_1 ? 'q' : M('d');
                return cmd;
            } else if(IS_THRONE(levl[u.ux][u.uy].typ)) {
                cmd[0]=M('s');
                return cmd;
            } else if((u.ux == xupstair && u.uy == yupstair)
                      || (u.ux == sstairs.sx && u.uy == sstairs.sy && sstairs.up)
                      || (u.ux == xupladder && u.uy == yupladder)) {
                return "<";
            } else if((u.ux == xdnstair && u.uy == ydnstair)
                      || (u.ux == sstairs.sx && u.uy == sstairs.sy && !sstairs.up)
                      || (u.ux == xdnladder && u.uy == ydnladder)) {
                return ">";
            } else if(OBJ_AT(u.ux, u.uy)) {
                cmd[0] = Is_container(level.objects[u.ux][u.uy]) ? M('l') : ',';
                return cmd;
            } else {
                return "."; /* just rest */
            }
        }

        /* directional commands */

        dir = xytod(x, y);

	if (!m_at(u.ux+x, u.uy+y) && !test_move(u.ux, u.uy, x, y, TEST_MOVE)) {
            cmd[1] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
            cmd[2] = 0;
            if (IS_DOOR(levl[u.ux+x][u.uy+y].typ)) {
                /* slight assistance to the player: choose kick/open for them */
                if (levl[u.ux+x][u.uy+y].doormask & D_LOCKED) {
                    cmd[0] = C('d');
                    return cmd;
                }
                if (levl[u.ux+x][u.uy+y].doormask & D_CLOSED) {
                    cmd[0] = 'o';
                    return cmd;
                }
            }
            if (levl[u.ux+x][u.uy+y].typ <= SCORR) {
                cmd[0] = 's';
                cmd[1] = 0;
                return cmd;
            }
        }
    } else {
        /* convert without using floating point, allowing sloppy clicking */
        if(x > 2*abs(y))
            x = 1, y = 0;
        else if(y > 2*abs(x))
            x = 0, y = 1;
        else if(x < -2*abs(y))
            x = -1, y = 0;
        else if(y < -2*abs(x))
            x = 0, y = -1;
        else
            x = sgn(x), y = sgn(y);

        if(x == 0 && y == 0)	/* map click on player to "rest" command */
            return ".";

        dir = xytod(x, y);
    }

    /* move, attack, etc. */
    cmd[1] = 0;
    if(mod == CLICK_1) {
	cmd[0] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
    } else {
	cmd[0] = (iflags.num_pad ? M(ndir[dir]) :
		(sdir[dir] - 'a' + 'A')); /* run command */
    }

    return cmd;
}

STATIC_OVL char *
parse()
{
#ifdef LINT	/* static char in_line[COLNO]; */
	char in_line[COLNO];
#else
	static char in_line[COLNO];
#endif
	register int foo;
	boolean prezero = FALSE;

	multi = 0;
	flags.move = 1;
	flush_screen(1); /* Flush screen buffer. Put the cursor on the hero. */

	if (!iflags.num_pad || (foo = readchar()) == 'n')
	    for (;;) {
		foo = readchar();
		if (foo >= '0' && foo <= '9') {
		    multi = 10 * multi + foo - '0';
		    if (multi < 0 || multi >= LARGEST_INT) multi = LARGEST_INT;
		    if (multi > 9) {
			clear_nhwindow(WIN_MESSAGE);
			Sprintf(in_line, "Count: %d", multi); /* EN Sprintf(in_line, "Count: %d", multi); */ // TODO DE
			pline(in_line);
			mark_synch();
		    }
		    last_multi = multi;
		    if (!multi && foo == '0') prezero = TRUE;
		} else break;	/* not a digit */
	    }

	if (foo == '\033') {   /* esc cancels count (TH) */
	    clear_nhwindow(WIN_MESSAGE);
	    multi = last_multi = 0;
# ifdef REDO
	} else if (foo == DOAGAIN || in_doagain) {
	    multi = last_multi;
	} else {
	    last_multi = multi;
	    savech(0);	/* reset input queue */
	    savech((char)foo);
# endif
	}

	if (multi) {
	    multi--;
	    save_cm = in_line;
	} else {
	    save_cm = (char *)0;
	}
	in_line[0] = foo;
	in_line[1] = '\0';
	if (foo == 'g' || foo == 'G' || foo == 'm' || foo == 'M' ||
	    foo == 'F' || (iflags.num_pad && (foo == '5' || foo == '-'))) {
	    foo = readchar();
#ifdef REDO
	    savech((char)foo);
#endif
	    in_line[1] = foo;
	    in_line[2] = 0;
	}
	clear_nhwindow(WIN_MESSAGE);
	if (prezero) in_line[0] = '\033';
	return(in_line);
}

#endif /* OVL0 */
#ifdef OVLB

#ifdef UNIX
static
void
end_of_input()
{
	exit_nhwindows("End of input?"); /* EN exit_nhwindows("End of input?"); */ // TODO DE
#ifndef NOSAVEONHANGUP
	if (!program_state.done_hup++ && program_state.something_worth_saving)
	    (void) dosave0();
#endif
	clearlocks();
	terminate(EXIT_SUCCESS);
}
#endif

#endif /* OVLB */
#ifdef OVL0

char
readchar()
{
	register int sym;
	int x = u.ux, y = u.uy, mod = 0;

	if ( *readchar_queue )
	    sym = *readchar_queue++;
	else
#ifdef REDO
	    sym = in_doagain ? Getchar() : nh_poskey(&x, &y, &mod);
#else
	    sym = Getchar();
#endif

#ifdef UNIX
# ifdef NR_OF_EOFS
	if (sym == EOF) {
	    register int cnt = NR_OF_EOFS;
	  /*
	   * Some SYSV systems seem to return EOFs for various reasons
	   * (?like when one hits break or for interrupted systemcalls?),
	   * and we must see several before we quit.
	   */
	    do {
		clearerr(stdin);	/* omit if clearerr is undefined */
		sym = Getchar();
	    } while (--cnt && sym == EOF);
	}
# endif /* NR_OF_EOFS */
	if (sym == EOF)
	    end_of_input();
#endif /* UNIX */

	if(sym == 0) {
	    /* click event */
	    readchar_queue = click_to_cmd(x, y, mod);
	    sym = *readchar_queue++;
	}
	return((char) sym);
}

STATIC_PTR int
dotravel()
{
	/* Keyboard travel command */
	static char cmd[2];
	coord cc;

	if (!iflags.travelcmd) return 0;
	cmd[1]=0;
	cc.x = iflags.travelcc.x;
	cc.y = iflags.travelcc.y;
	if (cc.x == -1 && cc.y == -1) {
	    /* No cached destination, start attempt from current position */
	    cc.x = u.ux;
	    cc.y = u.uy;
	}
	pline("Where do you want to travel to?"); /* EN pline("Where do you want to travel to?"); */ // TODO DE
	if (getpos(&cc, TRUE, "das gewünschte Ziel") < 0) { /* EN if (getpos(&cc, TRUE, "the desired destination") < 0) { */
		/* user pressed ESC */
		return 0;
	}
	iflags.travelcc.x = u.tx = cc.x;
	iflags.travelcc.y = u.ty = cc.y;
	cmd[0] = CMD_TRAVEL;
	readchar_queue = cmd;
	return 0;
}

#ifdef PORT_DEBUG
# ifdef WIN32CON
extern void NDECL(win32con_debug_keystrokes);
extern void NDECL(win32con_handler_info);
# endif

int
wiz_port_debug()
{
	int n, k;
	winid win;
	anything any;
	int item = 'a';
	int num_menu_selections;
	struct menu_selection_struct {
		char *menutext;
		void NDECL((*fn));
	} menu_selections[] = {
#ifdef WIN32CON
		{"test win32 keystrokes", win32con_debug_keystrokes},
		{"show keystroke handler information", win32con_handler_info},
#endif
		{(char *)0, (void NDECL((*)))0}		/* array terminator */
	};

	num_menu_selections = SIZE(menu_selections) - 1;
	if (num_menu_selections > 0) {
		menu_item *pick_list;
		win = create_nhwindow(NHW_MENU);
		start_menu(win);
		for (k=0; k < num_menu_selections; ++k) {
			any.a_int = k+1;
			add_menu(win, NO_GLYPH, &any, item++, 0, ATR_NONE,
				menu_selections[k].menutext, MENU_UNSELECTED);
		}
		end_menu(win, "Which port debugging feature?");
		n = select_menu(win, PICK_ONE, &pick_list);
		destroy_nhwindow(win);
		if (n > 0) {
			n = pick_list[0].item.a_int - 1;
			free((genericptr_t) pick_list);
			/* execute the function */
			(*menu_selections[n].fn)();
		}
	} else
		pline("No port-specific debug capability defined.");
	return 0;
}
# endif /*PORT_DEBUG*/

#endif /* OVL0 */
#ifdef OVLB
/*
 *   Parameter validator for generic yes/no function to prevent
 *   the core from sending too long a prompt string to the
 *   window port causing a buffer overflow there.
 */
char
yn_function(query,resp, def)
const char *query,*resp;
char def;
{
	char qbuf[QBUFSZ];
	char returned_char;
	unsigned truncspot, reduction = sizeof(" [N]  ?") + 1;

	if (resp) reduction += strlen(resp) + sizeof(" () ");
	if (strlen(query) < (QBUFSZ - reduction))
		return (*windowprocs.win_yn_function)(query, resp, def);
	paniclog("Query truncated: ", query);
	reduction += sizeof("...");
	truncspot = QBUFSZ - reduction;
	(void) strncpy(qbuf, query, (int)truncspot);
	qbuf[truncspot] = '\0';
	Strcat(qbuf,"...");

//#ifdef GERMAN
	// TODO DE
	//returned_char = (*windowprocs.win_yn_function)(qbuf, resp, def);
	//if (returned_char == 'j') { return 'y'; }
	//else if (returned_char == 'e') { return 'q'; }
	//else { return returned_char; }
//#else
	return (*windowprocs.win_yn_function)(qbuf, resp, def);
//#endif
}
#endif

/*cmd.c*/
