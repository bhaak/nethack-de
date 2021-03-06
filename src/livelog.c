/* Write live game progress changes to a log file 
 * Needs xlog-v3 patch. */

#include "hack.h"

#ifdef GERMAN
# include "german.h"
#endif

#ifdef LIVELOGFILE

/* Encodes the current xlog "achieve" status to an integer */
long
encodeachieve(void)
{
  /* Achievement bitfield:
   * bit  meaning
   *  0   obtained the Bell of Opening
   *  1   entered gehennom (by any means)
   *  2   obtained the Candelabrum of Invocation
   *  3   obtained the Book of the Dead
   *  4   performed the invocation ritual
   *  5   obtained the amulet
   *  6   entered elemental planes
   *  7   entered astral plane
   *  8   ascended (not "escaped in celestial disgrace!")
   *  9   obtained the luckstone from the Mines
   *  10  obtained the sokoban prize
   *  11  killed medusa
   */

  long r;

  r = 0;

  if(achieve.get_bell)           r |= 1L << 0;
  if(achieve.enter_gehennom)     r |= 1L << 1;
  if(achieve.get_candelabrum)    r |= 1L << 2;
  if(achieve.get_book)           r |= 1L << 3;
  if(achieve.perform_invocation) r |= 1L << 4;
  if(achieve.get_amulet)         r |= 1L << 5;
  if(In_endgame(&u.uz))          r |= 1L << 6;
  if(Is_astralevel(&u.uz))       r |= 1L << 7;
  if(achieve.ascended)           r |= 1L << 8;
  if(achieve.get_luckstone)      r |= 1L << 9;
  if(achieve.finish_sokoban)     r |= 1L << 10;
  if(achieve.killed_medusa)      r |= 1L << 11;

  return r;
}

/* Keep the last xlog "achieve" value to be able to compare */
long last_achieve_int;

/* Generic buffer for snprintf */
#define STRBUF_LEN (4096)
char strbuf[STRBUF_LEN];

/* Open the live log file */
boolean livelog_start() {


	last_achieve_int = encodeachieve();

	return TRUE;
}

/* Locks the live log file and writes 'buffer' */
void livelog_write_string(char* buffer) {
	FILE* livelogfile;
	if(lock_file(LIVELOGFILE, SCOREPREFIX, 10)) {
		if(!(livelogfile = fopen_datafile(LIVELOGFILE, "a", SCOREPREFIX))) {
			pline("Cannot open live log file!");
		} else {
			fprintf(livelogfile, buffer);
			(void) fclose(livelogfile);
		}
		unlock_file(LIVELOGFILE);
	}
}

/* Writes changes in the achieve structure to the live log.
 * Called from various places in the NetHack source,
 * usually where xlog's achieve is set. */
void livelog_achieve_update() {
	long achieve_int, achieve_diff;

	achieve_int = encodeachieve();
	achieve_diff = last_achieve_int ^ achieve_int;

	/* livelog_achieve_update is sometimes called when there's
	 * no actual change. */
	if(achieve_diff == 0) {
		return;
	}

	snprintf(strbuf, STRBUF_LEN,
		"player=%s:turns=%ld:achieve=0x%lx:achieve_diff=0x%lx\n",
		plname, 
		moves, 
		achieve_int,
		achieve_diff);
	livelog_write_string(strbuf);

	last_achieve_int = achieve_int;
}

/* Reports wishes */
void
livelog_wish(item)
char *item;
{
	snprintf(strbuf, STRBUF_LEN,
		"player=%s:turns=%ld:wish=%s\n",
		plname,
		moves,
		item);
	livelog_write_string(strbuf);
}

/* Shout */
#ifdef LIVELOG_SHOUT

int 
doshout()
{
	char buf[BUFSZ], qbuf[QBUFSZ];
	char* p;
	   
	Sprintf(qbuf,"Was MODIFIER_KONJUNKTIV_II VERB_MOEGEN SUBJECT_IM_SATZ PRONOMEN_PERSONAL schreien?"); /* EN Sprintf(qbuf,"Shout what?"); */
	getlin(qbuf, buf);
	
	You("VERB_SCHREIEN ins Leere: %s", buf); /* EN You("shout into the void: %s", buf); */

	/* filter livelog delimiter */
	for (p = buf; *p != 0; p++) 
		if( *p == ':' )
			*p = ' ';

	snprintf(strbuf, STRBUF_LEN,
		"player=%s:turns=%ld:shout=%s\n",
		plname,
		moves,
		buf);
	livelog_write_string(strbuf);
	
	return 0;
}

#endif /* LIVELOG_SHOUT */

#ifdef LIVELOG_BONES_KILLER
void
livelog_bones_killed(mtmp)
struct monst *mtmp;
{
	char *name = NAME(mtmp);

	if (name &&
	    mtmp->former_rank && strlen(mtmp->former_rank) > 0) {
#ifdef GERMAN
		char former_rank[BUFSZ];
		char mname[BUFSZ];
		Strcpy(former_rank, german(mtmp->former_rank));
		Strcpy(mname, german(mtmp->data->mname));
#endif
		/* $player killed the $bones_monst of $bones_killed the former
		 * $bones_rank on $turns on dungeon level $dlev! */
		snprintf(strbuf, STRBUF_LEN,
				"player=%s:turns=%ld:dlev=%d:"
				"bones_killed=%s:bones_rank=%s:bones_monst=%s\n",
				plname,
				moves,
				depth(&u.uz),
				name,
				former_rank, /* EN mtmp->former_rank, */
				mname); /* EN mtmp->data->mname); */
		livelog_write_string(strbuf);
	} else if ((mtmp->data->geno & G_UNIQ)
#ifdef BLACKMARKET
	           || (mtmp->data == &mons[PM_BLACK_MARKETEER])
#endif
		  ) {
		char *n = german(noit_mon_nam(mtmp)); /* EN char *n = noit_mon_nam(mtmp); */
		/* $player killed a uniq monster */
		snprintf(strbuf, STRBUF_LEN,
				"player=%s:turns=%ld:killed_uniq=%s\n",
				plname,
				moves,
				n);
		livelog_write_string(strbuf);
	}
}
#endif /* LIVELOG_BONES_KILLER */

/** Reports shoplifting */
void
livelog_shoplifting(shk_name, shop_name, total)
const char* shk_name;
const char* shop_name;
long total;
{
#ifdef GERMAN
	char shk[BUFSZ];
	char shop[BUFSZ];
	Strcpy(shk, german(shk_name));
	Strcpy(shop, german(shop_name));
#endif
	/* shopkeeper: Name of the shopkeeper (e.g. Kopasker)
	   shop:       Name of the shop (e.g. general store)
	   shoplifted: Merchandise worth this many Zorkmids was stolen */
	snprintf(strbuf, STRBUF_LEN,
		"player=%s:turns=%ld:shopkeeper=%s:shop=%s:shoplifted=%ld\n",
		plname,
		moves,
		shk, /* EN shk_name, */
		shop, /* EN shop_name, */
		total);
	livelog_write_string(strbuf);
}

#endif /* LIVELOGFILE */
