#ifndef _GERMAN_H
#define _GERMAN_H
#include <stddef.h>

enum Output_Encoding { OUTPUT_ASCII=0, OUTPUT_LATIN1=1, OUTPUT_UTF8=2 };
enum Output_Encoding german_output_encoding;

char *this_makes_you_feel;

enum Casus   { nominativ=1, genitiv=2, dativ=4, akkusativ=8 };
enum Genus   { maskulin=1, feminin=2, neutrum=4 };
enum Numerus { n_singular=1, n_plural=2 };
enum Person  { erstePerson=1, zweitePerson=2, drittePerson=4 };
enum Artikel { ohne=1, bestimmter=2, unbestimmter=4, grundform=8 };
enum Tempus_Modus { praesens=1, praeteritum=2, konjunktiv=4, konjunktiv_ii=8 };
enum Partizip { partizip_praesens=1, partizip_perfekt=2 };

/* used for nouns and adjectives, as there are nouns, 
   that are inflected like adjectives */
struct substantiv_oder_adjekiv_struct {
	const char  *wort;
	const char  *typ;
  const char  *fugenelement;
	enum Casus   casus;
	enum Genus   genus;
	enum Numerus numerus;
	enum Artikel artikel;
};

struct verb_alt_struct {
	const char *verb;
	const char *typ;
	const char *praeverb;
	enum Person person;
	enum Numerus numerus;
	enum Casus casus;
};

struct verb_struct {
	const char *verb;
	const char *typ;
	const char *praeverb;
	enum Person person;
	enum Numerus numerus;
	enum Tempus_Modus tempus_modus;
	enum Casus casus;
};

struct verb_infinitiv_struct {
	const char *verb;
	const char *zu;
	const char *typ;
};

struct verb_partizip_struct {
	const char *praesens;
	const char *perfekt;
	const char *typ;
};

struct verb_imperativ_struct {
	const char *singular;
	const char *plural;
	const char *praeverb;
	const char *typ;
};

extern struct substantiv_oder_adjekiv_struct worte[];
extern struct verb_alt_struct verben_alt[];
extern struct verb_infinitiv_struct verben_infinitiv[];
extern struct verb_partizip_struct verben_partizip[];
extern struct verb_struct verben[];
extern struct verb_imperativ_struct verben_imperativ[];

extern int ihrzen;

#ifndef TRUE
# define TRUE    (1)
# define FALSE   (0)
#endif

char* german(const char *line);
void german2meta(const char *str, char *result);
struct substantiv_oder_adjekiv_struct *get_meta_substantiv_with(const char *wort, char *with);
char* genitivattribut_zu_wort(const char *attribut, const char *wort);

char *normalisierung(char *output, const char *input);

int gott_weiblich(const char *name);
const char *gott_genitiv(const char *name);
const char *von_gott(const char *name);

char* pronominalisierung(const char *token);
char* fugenwort(const char *token);
char* einer_der(const char *token);
int german_the(const char *token);
char* relativpronomen(const char *token);
char* indefinitpronomen_eines(const char *token);
enum Genus genus_von(const char *token);
const char * german_branch_name(const char *);

#endif /* !_GERMAN_H */
