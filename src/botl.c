/*	SCCS Id: @(#)botl.c	3.4	1996/07/15	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef GERMAN
# include "german.h"
#endif

#ifdef OVL0
extern const char *hu_stat[];	/* defined in eat.c */

const char * const enc_stat[] = {
	"",
	"Beladen", /* EN "Burdened", */
	"Angestrengt", /* EN "Stressed", */
	"Strapaziert", /* EN "Strained", */
	"Überlastet", /* EN "Overtaxed", */
	"Überladen" /* EN "Overloaded" */
};
#ifdef GERMAN
extern const char *hu_en_stat[];	/* defined in eat.c */
const char * const enc_en_stat[] = {
	"",
	"Burdened",
	"Stressed",
	"Strained",
	"Overtaxed",
	"Overloaded"
};
#endif

STATIC_DCL void NDECL(bot1);
STATIC_DCL void NDECL(bot2);
#endif /* OVL0 */

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)

extern const struct percent_color_option *hp_colors;
extern const struct percent_color_option *pw_colors;
extern const struct text_color_option *text_colors;

struct color_option
text_color_of(text, color_options)
const char *text;
const struct text_color_option *color_options;
{
	if (color_options == NULL) {
		struct color_option result = {NO_COLOR, 0};
		return result;
	}
	if (strstri(color_options->text, text)
	 || strstri(text, color_options->text))
		return color_options->color_option;
	return text_color_of(text, color_options->next);
}

struct color_option
percentage_color_of(value, max, color_options)
int value, max;
const struct percent_color_option *color_options;
{
	if (color_options == NULL) {
		struct color_option result = {NO_COLOR, 0};
		return result;
	}
	if (100 * value <= color_options->percentage * max)
		return color_options->color_option;
	return percentage_color_of(value, max, color_options->next);
}

void
start_color_option(color_option)
struct color_option color_option;
{
#ifdef TTY_GRAPHICS
	int i;
	if (color_option.color != NO_COLOR)
		term_start_color(color_option.color);
	for (i = 0; (1 << i) <= color_option.attr_bits; ++i)
		if (i != ATR_NONE && color_option.attr_bits & (1 << i))
			term_start_attr(i);
#endif  /* TTY_GRAPHICS */
}

void
end_color_option(color_option)
struct color_option color_option;
{
#ifdef TTY_GRAPHICS
	int i;
	if (color_option.color != NO_COLOR)
		term_end_color();
	for (i = 0; (1 << i) <= color_option.attr_bits; ++i)
		if (i != ATR_NONE && color_option.attr_bits & (1 << i))
			term_end_attr(i);
#endif  /* TTY_GRAPHICS */
}

static
void
apply_color_option(color_option, newbot2, statusline)
struct color_option color_option;
const char *newbot2;
int statusline; /* apply color on this statusline: 1 or 2 */
{
	if (!iflags.use_status_colors || !iflags.use_color) return;
	curs(WIN_STATUS, 1, statusline-1);
	start_color_option(color_option);
	putstr(WIN_STATUS, 0, newbot2);
	end_color_option(color_option);
}

void
add_colored_text(text, text_en, newbot2)
const char *text;
const char *text_en;
char *newbot2;
{
	char *nb;
	struct color_option color_option;

	if (*text == '\0') return;

	/* don't add anything if it can't be displayed.
	 * Otherwise the color of invisible text may bleed into
	 * the statusline. */
	if (strlen(newbot2) >= min(MAXCO, CO)-1) return;

	if (!iflags.use_status_colors) {
		Sprintf(nb = eos(newbot2), " %s", text);
                return;
        }

	Strcat(nb = eos(newbot2), " ");
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);

	Strcat(nb = eos(nb), text);
	curs(WIN_STATUS, 1, 1);
	/* ueberpruefe deutsche und englische Bezeichner fuer
	 * die Statuszeilen-Kolorierung */
	color_option = text_color_of(text_en, text_colors);
	if (color_option.color == NO_COLOR) {
		color_option = text_color_of(text, text_colors);
	}
	start_color_option(color_option);
	/* Trim the statusline to always have the end color
	 * to have effect. */
	newbot2[min(MAXCO, CO)-1] = '\0';
	putstr(WIN_STATUS, 0, newbot2);
	end_color_option(color_option);
}

#endif

#ifndef OVLB
STATIC_DCL int mrank_sz;
#else /* OVLB */
STATIC_OVL NEARDATA int mrank_sz = 0; /* loaded by max_rank_sz (from u_init) */
#endif /* OVLB */

#ifdef OVL1

/* convert experience level (1..30) to rank index (0..8) */
int
xlev_to_rank(xlev)
int xlev;
{
	return (xlev <= 2) ? 0 : (xlev <= 30) ? ((xlev + 2) / 4) : 8;
}

#if 0	/* not currently needed */
/* convert rank index (0..8) to experience level (1..30) */
int
rank_to_xlev(rank)
int rank;
{
	return (rank <= 0) ? 1 : (rank <= 8) ? ((rank * 4) - 2) : 30;
}
#endif

const char *
rank_of(lev, monnum, female)
	int lev;
	short monnum;
	boolean female;
{
	register struct Role *role;
	register int i;


	/* Find the role */
	for (role = (struct Role *) roles; role->name.m; role++)
	    if (monnum == role->malenum || monnum == role->femalenum)
	    	break;
	if (!role->name.m)
	    role = &urole;

	/* Find the rank */
	for (i = xlev_to_rank((int)lev); i >= 0; i--) {
	    if (female && role->rank[i].f) return (role->rank[i].f);
	    if (role->rank[i].m) return (role->rank[i].m);
	}

	/* Try the role name, instead */
	if (female && role->name.f) return (role->name.f);
	else if (role->name.m) return (role->name.m);
	return ("Spieler"); /* EN return ("Player"); */
}


const char *
rank()
{
	return(rank_of(u.ulevel, Role_switch, flags.female));
}

int
title_to_mon(str, rank_indx, title_length)
const char *str;
int *rank_indx, *title_length;
{
	register int i, j;


	/* Loop through each of the roles */
	for (i = 0; roles[i].name.m; i++)
	    for (j = 0; j < 9; j++) {
	    	if (roles[i].rank[j].m && !strncmpi(str,
	    			roles[i].rank[j].m, strlen(roles[i].rank[j].m))) {
	    	    if (rank_indx) *rank_indx = j;
	    	    if (title_length) *title_length = strlen(roles[i].rank[j].m);
	    	    return roles[i].malenum;
	    	}
	    	if (roles[i].rank[j].f && !strncmpi(str,
	    			roles[i].rank[j].f, strlen(roles[i].rank[j].f))) {
	    	    if (rank_indx) *rank_indx = j;
	    	    if (title_length) *title_length = strlen(roles[i].rank[j].f);
	    	    return ((roles[i].femalenum != NON_PM) ?
	    	    		roles[i].femalenum : roles[i].malenum);
	    	}
	    }
	return NON_PM;
}

#endif /* OVL1 */
#ifdef OVLB

void
max_rank_sz()
{
	register int i, r, maxr = 0;
	for (i = 0; i < 9; i++) {
	    if (urole.rank[i].m && (r = strlen(urole.rank[i].m)) > maxr) maxr = r;
	    if (urole.rank[i].f && (r = strlen(urole.rank[i].f)) > maxr) maxr = r;
	}
	mrank_sz = maxr;
	return;
}

#endif /* OVLB */
#ifdef OVL0

#ifdef SCORE_ON_BOTL
long
botl_score()
{
    int deepest = deepest_lev_reached(FALSE);
#ifndef GOLDOBJ
    long ugold = u.ugold + hidden_gold();

    if ((ugold -= u.ugold0) < 0L) ugold = 0L;
    return ugold + u.urexp + (long)(50 * (deepest - 1))
#else
    long umoney = money_cnt(invent) + hidden_gold();

    if ((umoney -= u.umoney0) < 0L) umoney = 0L;
    return umoney + u.urexp + (long)(50 * (deepest - 1))
#endif
			  + (long)(deepest > 30 ? 10000 :
				   deepest > 20 ? 1000*(deepest - 20) : 0);
}
#endif

STATIC_OVL void
bot1()
{
	char newbot1[MAXCO];
	char *nb;
	int i=0,j;
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	int save_botlx = flags.botlx;
#endif

	Strcpy(newbot1, "");
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	if (iflags.hitpointbar) {
		flags.botlx = 0;
		curs(WIN_STATUS, 1, 0);
		putstr(WIN_STATUS, 0, newbot1);
		Strcat(newbot1, "[");
		i = 1; /* don't overwrite the string in front */
		curs(WIN_STATUS, 1, 0);
		putstr(WIN_STATUS, 0, newbot1);
	}
#endif
	Strcat(newbot1, plname);
	if('a' <= newbot1[i] && newbot1[i] <= 'z') newbot1[i] += 'A'-'a';
	newbot1[10] = 0;
	Sprintf(nb = eos(newbot1)," ARTIKEL_BESTIMMTER "); /* EN Sprintf(nb = eos(newbot1)," the "); */

	if (Upolyd) {
		char mbot[BUFSZ];
		int k = 0;

		Strcpy(mbot, mons[u.umonnum].mname);
#ifndef GERMAN
		while(mbot[k] != 0) {
		    if ((k == 0 || (k > 0 && mbot[k-1] == ' ')) &&
					'a' <= mbot[k] && mbot[k] <= 'z')
			mbot[k] += 'A' - 'a';
		    k++;
		}
		Sprintf(nb = eos(nb), mbot);
#else
		Sprintf(nb = eos(nb), "%s", mbot);
		Strcpy(newbot1, german(newbot1));
		nb = eos(newbot1);
#endif
	} else
		Sprintf(nb = eos(nb), "%s", rank());
#ifdef GERMAN
		Strcpy(newbot1, german(newbot1));
		nb = eos(newbot1);
#endif

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	if (iflags.hitpointbar) {
		int bar_length = strlen(newbot1)-1;
		char tmp[MAXCO];
		char *p = tmp;
		/* filledbar >= 0 and < MAXCO */
		int hp = (uhp() < 0) ? 0 : uhp();
		int filledbar = hp * bar_length / uhpmax();
		if (filledbar >= MAXCO) { filledbar = MAXCO-1; }
		Strcpy(tmp, newbot1);
		p++;

		/* draw hp bar */
		if (iflags.use_inverse) term_start_attr(ATR_INVERSE);
		p[filledbar] = '\0';
		if (iflags.use_color) {
			/* draw in color mode */
			apply_color_option(percentage_color_of(uhp(), uhpmax(), hp_colors), tmp, 1);
		} else {
			/* draw in inverse mode */
			curs(WIN_STATUS, 1, 0);
			putstr(WIN_STATUS, 0, tmp);
		}
		term_end_color();
		if (iflags.use_inverse) term_end_attr(ATR_INVERSE);

		Strcat(newbot1, "]");
	}
#endif

	Sprintf(nb = eos(nb),"  ");
	i = mrank_sz; /* */ /* EN i = mrank_sz + 15; */
	j = (nb + 2) - newbot1; /* aka strlen(newbot1) but less computation */
	if((i - j) > 0)
		Sprintf(nb = eos(nb),"%*s", i-j, " ");	/* pad with spaces */
	if (ACURR(A_STR) > 18) {
		if (ACURR(A_STR) > STR18(100))
		    Sprintf(nb = eos(nb),"St:%2d ",ACURR(A_STR)-100);
		else if (ACURR(A_STR) < STR18(100))
		    Sprintf(nb = eos(nb), "St:18/%02d ",ACURR(A_STR)-18);
		else
		    Sprintf(nb = eos(nb),"St:18/** ");
	} else
		Sprintf(nb = eos(nb), "St:%-1d ",ACURR(A_STR));
	Sprintf(nb = eos(nb),
		"Ge:%-1d Ko:%-1d In:%-1d We:%-1d Ch:%-1d", /* EN "Dx:%-1d Co:%-1d In:%-1d Wi:%-1d Ch:%-1d", */
		ACURR(A_DEX), ACURR(A_CON), ACURR(A_INT), ACURR(A_WIS), ACURR(A_CHA));
	Sprintf(nb = eos(nb), (u.ualign.type == A_CHAOTIC) ? "  Chaotisch" : /* EN Sprintf(nb = eos(nb), (u.ualign.type == A_CHAOTIC) ? "  Chaotic" : */
			(u.ualign.type == A_NEUTRAL) ? "  Neutral" : "  Rechtschaffen"); /* EN (u.ualign.type == A_NEUTRAL) ? "  Neutral" : "  Lawful"); */
#ifdef SCORE_ON_BOTL
	if (flags.showscore)
	    Sprintf(nb = eos(nb), " P:%ld", botl_score()); /* EN Sprintf(nb = eos(nb), " S:%ld", botl_score()); */
#endif
	curs(WIN_STATUS, 1, 0);
	putstr(WIN_STATUS, 0, newbot1);
	flags.botlx = save_botlx;
}

/* provide the name of the current level for display by various ports */
int
describe_level(buf)
char *buf;
{
	int ret = 1;

	/* TODO:	Add in dungeon name */
	if (Is_knox(&u.uz))
		Sprintf(buf, "%s ", dungeons[u.uz.dnum].dname);
	else if (In_quest(&u.uz))
		Sprintf(buf, "Zuhause %d ", dunlev(&u.uz)); /* EN Sprintf(buf, "Home %d ", dunlev(&u.uz)); */
	else if (In_endgame(&u.uz))
		Sprintf(buf,
			Is_astralevel(&u.uz) ? "Astralebene " : "Schlussphase "); /* EN Is_astralevel(&u.uz) ? "Astral Plane " : "End Game "); */
	else {
		/* ports with more room may expand this one */
		Sprintf(buf, "Dlvl:%-2d ", depth(&u.uz));
		ret = 0;
	}
	return ret;
}

STATIC_OVL void
bot2()
{
	char  newbot2[MAXCO];
	register char *nb;
	int hp, hpmax;
	int cap = near_capacity();
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	int save_botlx = flags.botlx;
#endif

	hp = Upolyd ? u.mh : u.uhp;
	hpmax = Upolyd ? u.mhmax : u.uhpmax;

	if(hp < 0) hp = 0;
	(void) describe_level(newbot2);
	Sprintf(nb = eos(newbot2), "%c:%-2ld", oc_syms[COIN_CLASS],
#ifndef GOLDOBJ
		u.ugold
#else
		money_cnt(invent)
#endif
	       );

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	Strcat(nb = eos(newbot2), " HP:");
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);
	flags.botlx = 0;

	Sprintf(nb = eos(nb), "%d(%d)", hp, hpmax);
	apply_color_option(percentage_color_of(hp, hpmax, hp_colors), newbot2, 2);
#else
	Sprintf(nb = eos(nb), " HP:%d(%d)", hp, hpmax);
#endif
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	Strcat(nb = eos(nb), " Pw:");
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);

	Sprintf(nb = eos(nb), "%d(%d)", u.uen, u.uenmax);
	apply_color_option(percentage_color_of(u.uen, u.uenmax, pw_colors), newbot2, 2);
#else
	Sprintf(nb = eos(nb), " Pw:%d(%d)", u.uen, u.uenmax);
#endif
	Sprintf(nb = eos(nb), " AC:%-2d", u.uac);
	if (Upolyd)
		Sprintf(nb = eos(nb), " HD:%d", mons[u.umonnum].mlevel);
#ifdef EXP_ON_BOTL
	else if(flags.showexp)
		Sprintf(nb = eos(nb), " Ef:%u/%-1ld", u.ulevel,u.uexp); /* EN Sprintf(nb = eos(nb), " Xp:%u/%-1ld", u.ulevel,u.uexp); */
#endif
	else
		Sprintf(nb = eos(nb), " Erf:%u", u.ulevel); /* EN Sprintf(nb = eos(nb), " Exp:%u", u.ulevel); */

	if(flags.time)
	    Sprintf(nb = eos(nb), " Z:%ld", moves); /* EN Sprintf(nb = eos(nb), " T:%ld", moves); */

#ifdef REALTIME_ON_BOTL
  if(iflags.showrealtime) {
    time_t currenttime = get_realtime();
    Sprintf(nb = eos(nb), " %ld:%2.2ld", currenttime / 3600, 
                                       (currenttime % 3600) / 60);
  }
#endif

	if(strcmp(hu_stat[u.uhs], "        "))
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text(hu_stat[u.uhs], hu_en_stat[u.uhs], newbot2);
#else
		Sprintf(nb = eos(nb), " %s", hu_stat[u.uhs]);
#endif
	if(Confusion)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text("Konf", "Conf", newbot2);
#else
		Strcat(nb = eos(nb), " Konf");
#endif
	if(Sick) {
		if (u.usick_type & SICK_VOMITABLE)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
			add_colored_text("Vergift", "FoodPois", newbot2);
#else
			Strcat(nb = eos(nb), " Vergift");
#endif
		if (u.usick_type & SICK_NONVOMITABLE)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
			add_colored_text("Krank", "Ill", newbot2);
#else
			Strcat(nb = eos(nb), " Krank");
#endif
	}
	if(Blind)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text("Blind", "Blind", newbot2);
#else
		Strcat(nb = eos(nb), " Blind");
#endif
	if(Stunned)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text("Benom", "Stun", newbot2);
#else
		Strcat(nb = eos(nb), " Benom");
#endif
	if(Hallucination)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text("Hallu", "Hallu", newbot2);
#else
		Strcat(nb = eos(nb), " Hallu");
#endif
	if(Slimed)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text("Schleim", "Slime", newbot2);
#else
		Strcat(nb = eos(nb), " Schleim");
#endif
	if(cap > UNENCUMBERED)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text(enc_stat[cap], enc_en_stat[cap], newbot2);
#else
		Sprintf(nb = eos(nb), " %s", enc_stat[cap]);
#endif
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);
	flags.botlx = save_botlx;
}

void
bot()
{
	bot1();
	bot2();
	flags.botl = flags.botlx = 0;
}

#endif /* OVL0 */

/*botl.c*/
