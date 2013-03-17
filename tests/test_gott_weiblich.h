#include "hack.h"

#include "german.h"

struct flag flags;

//struct Role urole;

char plname[PL_NSIZ];
char pl_character[PL_CSIZ];

int
rn2(int x)/* 0 <= rn2(x) < x */
{
	return 0;
}

void
panic(const char * str,...)
{
	fprintf(stderr, "%s\n", str);
	abort();
}

struct permonst mons[] = {{0}};
struct window_procs windowprocs = {0};







#include "../src/hacklib.c"
#include "../src/role.c"
#include "../src/german_vocabulary.c"
