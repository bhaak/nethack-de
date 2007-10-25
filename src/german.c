
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef NO_HACK_H_INCLUDE
# include "hack.h"
#endif

#include "german.h"

#define TBUFSZ 300

//#define DEBUG 1
#define WISH_DEBUG 0

/*

EBNF
satz       = subject verb [ object ];
substantiv = artikel [ adjektiv ] "NOUN_.*" ;
subjekt    = ( "SUBJECT" | "SUBJECT_IM_SATZ" ) ( personal_pronomen | substantiv ) ;
objekt     = "OBJECT" ( personal_pronomen | substantiv ) ;
personal_pronomen = "PRONOMEN_PERSONAL"
adjektiv   = "ADJEKTIV_.*"
artikel    = "ARTIKEL_BESTIMMTER" | "ARTIKEL_UNBESTIMMTER" | "PRONOMEN_POSSESSIV" | "PRONOMEN_KEIN"

SUBJECT:				 Marks the subject and the beginning of a sentence
SUBJECT_IM_SATZ: Marks the subject
OBJECT: Direct object of the sentence
VERB_.*: Predicate
*/

enum Genus subject_genus = 0;
enum Numerus subject_numerus = 0;
enum Person subject_person = 0;

enum Genus   pm_genus = 0;
enum Numerus pm_numerus = 0;
enum Person  pm_person = 0;

enum Genus   do_genus = 0;
enum Numerus do_numerus = 0;

enum Tempus_Modus  verb_tempus_modus = 0;
enum Partizip verb_partizip = 0;
enum Casus  verb_do_casus = 0;
int verb_infinitiv = 0;

/* c_ => current state */
enum Casus   c_casus = 0;
enum Genus   c_genus = 0;
enum Numerus c_numerus = 0;
enum Person  c_person = 0;
enum Artikel c_artikel = 0;


int modifier_corpse = 0;

int partikel_of_as_mit = 0;
int beginning_of_sentence = 0;
const char* verb_praeverb = "";

enum Genus   beliebiger_genus   = maskulin|feminin|neutrum;
enum Numerus beliebiger_numerus = n_singular|n_plural;
enum Artikel beliebiger_artikel = ohne|bestimmter|unbestimmter|grundform;

void print_state()
{
	switch (c_casus) {
	case nominativ: printf("Nominativ "); break;
	case genitiv: printf("Genitiv "); break;
	case dativ: printf("Dativ "); break;
	case akkusativ: printf("Akkusativ "); break;
	default: printf("kein Kasus "); break;	
	}
	switch (c_genus) {
	case maskulin: printf("Maskulin "); break;
	case feminin: printf("Feminin "); break;
	case neutrum: printf("Neutrum "); break;
	default: printf("kein bestimmtes Genus %d ", c_genus); break;
	}
	switch (c_numerus) {
	case n_singular: printf("Singular "); break;
	case n_plural: printf("Plural "); break;
	default: printf("kein Numerus ");
	}
	switch (c_person) {
	case zweitePerson: printf("2.Person "); break;
	case drittePerson: printf("3.Person "); break;
	default: printf("keine Person ");
	}
	switch (c_artikel) {
	case ohne: printf("ohne Artikel "); break;
	case bestimmter: printf("bestimmter Artikel "); break;
	case unbestimmter: printf("unbestimmter Artikel "); break;
	default: printf("kein Artikel ");
	}
	printf("modifier_corpse: %d ", modifier_corpse);
	printf("\n");
}


// returns true, if token has a final lowercase s
int token_is_plural(const char* token) {
	return (token[strlen(token)-1] == 's');

}

int next_token(const char* input, char* output, int pos) {
	int i=pos;
	int j=0;
	char tmp[TBUFSZ];

#ifdef DEBUG
	printf("next_token: -%s-\n", input+i);
#endif
	//do {
		if (!isspace(input[i])) {
			while (isalnum(input[i])||input[i]=='_') {
				output[j++] = input[i++];
			}
		}
		output[j] = '\0';

    if (strncmp("NOUN_POTION", output, 11)==0) {
			// get the token after NOUN_POTION PARTIKEL_OF
			// jump over PARTIKEL_OF
			next_token(input, tmp, pos+1+strlen(output));
			int len_partikel_of = strlen(tmp);
			next_token(input, tmp, pos+2+len_partikel_of+strlen(output));
			//printf("Token nach Token nach NOUN_POTION: %s\n", tmp);
			if ((strncmp(tmp, "NOUN_POT_WATER",14)==0) ||
					(strncmp(tmp, "NOUN_POT_BOOZE",14)==0) ||
					(strncmp(tmp, "NOUN_POT_ACID",13)==0) ||
					(strncmp(tmp, "NOUN_POT_OIL",12)==0) ||
					(strncmp(tmp, "NOUN_POT_FRUIT_JUICE",20)==0) ||
					(strncmp(tmp, "NOUN_POT_HOLY_WATER",19)==0) ||
					(strncmp(tmp, "NOUN_POT_UNHOLY_WATER",19)==0)) {
				if (token_is_plural(output)) { strcpy(output, "NOUN_FLASCHEs"); }
				else { strcpy(output, "NOUN_FLASCHE"); }
			}
		}
		
		//printf("output: %s strlen: %d\n",output, strlen(output));
		/*if (strlen(output)==0) { i++; }
			} while (strlen(output)==0);*/

#ifdef DEBUG
	printf("next_token return: %s\n", output);
#endif
	return (strlen(output)>0);
}

/* get the complete token */
int previous_token(const char* input, char* output, int pos) {
	int i=pos;

#ifdef DEBUG
	printf("previous_token1: -%s-\n", input);
	printf("previous_token2: -%s-\n", input+i);
#endif

	// skip whitespace
	do {
		i--;
	} while ((i>0) && (isspace(input[i])));
	//printf("previous_token3: -%s-\n", input+i);

	/* determine begin of token */
	while ((i>0) && (isalnum(input[i])||input[i]=='_')) {
		i--;
	}
	i++;

	//printf("previous_token4: -%s-\n", input+i);

	int ret = next_token(input, output, i);
#ifdef DEBUG
	printf("previous_token return: %s\n", output);
#endif
	return ret;
}

/* returns true, when the token text is found within input */
int find_token(const char* text, const char* input) {
	int i=0;
	char tmp[TBUFSZ];
	
	while (i<strlen(input)) {
		next_token(input,tmp, i);
		//printf("text: -%s- input: -%s-, tmp: -%s- i: %d\n", text, input, tmp, i);
		if (strcmp(text, tmp)==0) { return 1;}
		i += strlen(tmp);
		if (strlen(tmp)==0) { i++;}
	}
	//printf("find_token return 0\n");
	return 0;
}
/* Converts 'Fuchs' to 'NOUN_FOX'. Always returns the longest match. */
struct substantiv_oder_adjekiv_struct *get_meta_substantiv(const char *wort) {
	int i=0;
	struct substantiv_oder_adjekiv_struct *ret = NULL;
	int len = 0;
	int tmp_len = 0;

	while (worte[i].wort!=NULL) {
		tmp_len = strlen(worte[i].wort);
		if (strncmp(worte[i].wort, wort, tmp_len)==0) {
			if (tmp_len >= len) {
				ret = &worte[i];
				len = tmp_len;
			}
		}
		i++;
	}
	
	return ret;
}

/* Converts 'Fuchs' to 'NOUN_FOX'. Always returns the longest match that starts with "with". */
struct substantiv_oder_adjekiv_struct *get_meta_substantiv_with(const char *wort, char *with) {
	int i=0;
	struct substantiv_oder_adjekiv_struct *ret = NULL;
	int len = 0;
	int tmp_len = 0;

	while (worte[i].wort!=NULL) {
		tmp_len = strlen(worte[i].wort);
		if (strncmp(worte[i].typ, with, strlen(with))==0) {
			if (strncmp(worte[i].wort, wort, tmp_len)==0) {
				if (tmp_len >= len) {
					ret = &worte[i];
					len = tmp_len;
				}
			}
		}
		i++;
	}
	
	return ret;
}

char *strstr2(const char *haystack, const char *needle1, const char *needle2) {
	char *pos = strstr(haystack, needle1);
	if (pos != NULL) { return pos; }
	pos = strstr(haystack, needle2);
	return pos;
}

/* translates a german string to meta*/
void german2meta(const char *str, char *output)
{
	int i=0;
	output[0] = '\0';
	int ring_gefunden = 0;
	int wand_gefunden = 0;
	int potion_gefunden = 0;
	int spellbook_gefunden = 0;
	int scroll_gefunden = 0;
	int leiche_gefunden = 0;
	
	if (WISH_DEBUG) printf("\ngerman2meta %s\n", str);
	//printf("str: %s\n",str);
	//printf("strlen(str): %d\n",strlen(str));
	while (i < strlen(str)) {
		if (WISH_DEBUG) printf("i: %d\n",i);
		if (WISH_DEBUG) printf("1 %s\n",str+i);
		struct substantiv_oder_adjekiv_struct *wort = get_meta_substantiv(str+i);
		if (WISH_DEBUG) printf("1.5 %s\n",str+i);

		// gewisse Worte sind mehrdeutig, z.B. "rot", "Hast"
		if (wort != NULL) {
			if (WISH_DEBUG) printf("2 word: %s; ring_gefunden: %d \n",wort->typ,ring_gefunden);
			if ((strncmp("ADJEKTIV_SPE_", wort->typ, 13)==0) ||
					(strncmp("ADJEKTIV_POT_", wort->typ, 13)==0) ||
					(strncmp("ADJEKTIV_GEM_", wort->typ, 13)==0) ||
					(strncmp("ADJEKTIV_WAND_", wort->typ, 14)==0) ||
					(strncmp("ADJEKTIV_AMULET_", wort->typ, 16)==0)) {
				// diese Adjektive werden mittels den nachfolgenden Substantiven bestimmt
				if (WISH_DEBUG) printf("ADJEKTIV_ 1 %s\n", wort->typ);
				if (WISH_DEBUG) printf("ADJEKTIV_ 2 %s\n", str+i);
				if (strstr2(str+i, "Zauberbuch", "Zauberb�ch")) {
					wort = get_meta_substantiv_with(str+i, "ADJEKTIV_SPE_");
					if (WISH_DEBUG) printf("ADJEKTIV_ 4 %s\n", wort->typ);
				} else if (strstr2(str+i, "Trank", "Tr�nk")) {
					wort = get_meta_substantiv_with(str+i, "ADJEKTIV_POT_");
					if (WISH_DEBUG) printf("ADJEKTIV_ 5 %s\n", wort->typ);
				} else if (strstr2(str+i, "Amulett", "Amulett")) {
					wort = get_meta_substantiv_with(str+i, "ADJEKTIV_AMULET_");
					if (WISH_DEBUG) printf("ADJEKTIV_ 6 %s\n", wort->typ);
				}
			} else if (wand_gefunden) {
				if ((strncmp("NOUN_", wort->typ, 4)==0)){
					wort = get_meta_substantiv_with(str+i, "NOUN_WAND_");
					if (WISH_DEBUG) printf("NOUN_WAND_ 1 %s\n", wort->typ);
				}
			} else if (potion_gefunden) {
				if ((strncmp("NOUN_", wort->typ, 4)==0)){
					wort = get_meta_substantiv_with(str+i, "NOUN_POT_");
					if (WISH_DEBUG) printf("NOUN_POT_ 1 %s\n", wort->typ);
				}
			} else if (scroll_gefunden) {
				if ((strncmp("NOUN_", wort->typ, 4)==0)){
					wort = get_meta_substantiv_with(str+i, "NOUN_SCR_");
					if (WISH_DEBUG) printf("NOUN_SCR_ 1 %s\n", wort->typ);
				}
			} else if (ring_gefunden) {
				if ((strncmp("NOUN_", wort->typ, 4)==0)){
					wort = get_meta_substantiv_with(str+i, "NOUN_RING_");
					if (WISH_DEBUG) printf("NOUN_RING_ 1 %s\n", wort->typ);
				}
			}
		}

		if (wort == NULL) {
			// No match. Advance one character and try again
			strncat(output, str+i, 1);
			i++;
		} else {
			// Found a match. Copy string and jump over word
			if (WISH_DEBUG) printf("3 wort->wort: %s\n",wort->typ);
			if (strncmp("NOUN_RING", wort->typ, 9)==0) {
				ring_gefunden = 1;
			} else if (strncmp("NOUN_WAND", wort->typ, 9)==0) {
				wand_gefunden = 1;
			} else if (strncmp("NOUN_POTION", wort->typ, 11)==0) {
				potion_gefunden = 1;
			} else if (strncmp("NOUN_SPELLBOOK", wort->typ, 14)==0) {
				spellbook_gefunden = 1;
			} else if (strncmp("NOUN_SCROLL", wort->typ, 11)==0) {
				scroll_gefunden = 1;
			}

			if ((ring_gefunden ||
					 wand_gefunden ||
					 potion_gefunden ||
					 spellbook_gefunden ||
					 scroll_gefunden) &&
			    (strcmp("ARTIKEL_BESTIMMTER", wort->typ)==0)) {
				strcat(output, "PARTIKEL_OF");
				i = i + strlen(wort->wort);
			} else if (wand_gefunden && (strncmp("MADE_OF_WAND_", wort->typ, 13)==0)) {
				if (WISH_DEBUG) printf("MADE_OF_WAND 1 %s\n", output);
				// "NOUN_WAND aus " l�schen
				output[strlen(output)-14] = '\0';
				if (WISH_DEBUG) printf("MADE_OF_WAND 2 %s\n", output);
				strcat(output, wort->typ);
				if (WISH_DEBUG) printf("MADE_OF_WAND 3 %s\n", output);
				strcat(output, " NOUN_WAND");
				if (WISH_DEBUG) printf("MADE_OF_WAND 4 %s\n", output);
				i = i + strlen(wort->wort);
			} else if (strncmp("NOUN_FLASCHE", wort->typ, 12)==0) {
				if (token_is_plural(wort->typ)) {
					strcat(output, "NOUN_POTIONs PARTIKEL_OF");
				} else {
					strcat(output, "NOUN_POTION PARTIKEL_OF");
				}
				i = i + strlen(wort->wort);
				potion_gefunden = 1;
			} else if (strncmp("NOUN_CORPSE", wort->typ, 11)==0) {
				strcat(output, "MODIFIER_CORPSE");
				i = i + strlen(wort->wort);
				leiche_gefunden = 1;
			} else if (leiche_gefunden && strncmp("NOUN_", wort->typ, 5)==0) {
				strcat(output, wort->typ);
				strcat(output, " NOUN_CORPSE");
				i = i + strlen(wort->wort);
			} else {
				strcat(output, wort->typ);
				i = i + strlen(wort->wort);
			}
		}
	}
	if (WISH_DEBUG) printf("\ngerman2meta %s\n", output);
}

void german2meta_with(char *str, char *output, char *with)
{
	int i=0;
	output[0] = '\0';
	
	//printf("str: %s\n",str);
	//printf("strlen(str): %d\n",strlen(str));
	while (i < strlen(str)) {
		//printf("i: %d\n",i);
		//printf("1\n",i);
		//printf("1.5 %s\n",str+i);
		struct substantiv_oder_adjekiv_struct *wort = get_meta_substantiv(str+i);
		//printf("2\n",i);
		
		if (wort == NULL) {
			// No match. Advance one character and try again
			strncat(output, str+i, 1);
			i++;
		} else {
			// Found a match. Copy string and jump over word
			//printf("3 wort->wort: %s\n",wort->typ);
			strcat(output, wort->typ);
			i = i + strlen(wort->wort);
			//printf("4\n",i);
		}
	}
	printf("\ngerman2meta %s\n", output);
}

const char* get_verb(const char* verb, enum Person p, enum Numerus n, enum Tempus_Modus tm) {
	int i=0;
#ifdef DEBUG
	printf("%s %d %d", verb, p, n);
#endif
	if (verb_infinitiv) {
		// Infinitiv
		while (verben_infinitiv[i].typ != NULL) {
			if (strcmp(verben_infinitiv[i].typ, verb)==0) {
				return verben_infinitiv[i].verb;
			}
			i++;
		}
	} else if (verb_partizip > 0) {
		// Partizipien
		while (verben_partizip[i].typ != NULL) {
			if (strcmp(verben_partizip[i].typ, verb)==0) {
				if (verb_partizip == partizip_praesens) {
					return verben_partizip[i].praesens;
				} else if (verb_partizip == partizip_perfekt) {
					return verben_partizip[i].perfekt;
				}
			}
			i++;
		}
	} else {
		// konjugierte Verbformen
		while (verben[i].verb != NULL) {
			if ((strcmp(verben[i].typ, verb)==0) && 
					(verben[i].person  & p) && 
					(verben[i].numerus & n) &&
					(verben[i].tempus_modus & tm)) {
				verb_do_casus = verben[i].casus;
				verb_praeverb = verben[i].praeverb;
				return verben[i].verb;
			}
			i++;
		}
	}

	return verb;
}

const char* get_wort(const char* typ, enum Casus c, enum Genus g, enum Numerus n, enum Artikel a) {
	int i=0;
	int len=strlen(typ);
	// NOUN_FOXs -> NOUN_FOX
	// NOUN_FOX  -> NOUN_FOX
	if (typ[len]=='s') { len--; }

	//print_state();

	while (worte[i].wort!=NULL) {
		if ((strncmp(worte[i].typ, typ, len)==0) && 
				(worte[i].casus   & c) && 
				(worte[i].genus   & g) && 
				(worte[i].numerus & n) &&
				(worte[i].artikel & a)) {
			/* make sure, we don't return e.g NOUN_TIN_OPENER when searching NOUN_TIN by getting the longest match */
			if ((strlen(worte[i].typ)-len)<=1) { return worte[i].wort; }
		}
		i++;
	}
	
	return typ;
}

const char* get_substantiv(const char* typ, enum Casus c, enum Numerus n, enum Artikel a) {
	return get_wort(typ, c, beliebiger_genus, n, a);
}

const char* get_fugenelement(const char* typ) {
	int i=0;

	while (worte[i].wort!=NULL) {
		if (strcmp(worte[i].typ, typ)==0) {
			return worte[i].fugenelement;
		}
		i++;
	}
	
	return "";
}

const char* get_adjektiv(const char* typ, enum Casus c, enum Genus g, enum Numerus n, enum Artikel a) {
	int i=0;
	while (worte[i].wort!=NULL) {
		if ((strcmp(worte[i].typ, typ)==0) && 
				(worte[i].casus   & c) && 
				(worte[i].genus   & g) && 
				(worte[i].numerus & n) &&
				(worte[i].artikel & a)) {
			return worte[i].wort;
		}
		i++;
	}
	
	return typ;
}

int analyze_this_as_subject(const char *text) {
	int k;
	if ((subject_person==0) || (subject_genus==0) || (subject_numerus==0)) {
		k=0;
		while (worte[k].wort!=NULL) {
			if (strcmp(worte[k].typ, text)==0) {
				c_person  = drittePerson;
				c_genus = worte[k].genus;
				c_numerus = worte[k].numerus;
				c_artikel = ohne;

				subject_person  = drittePerson;
				subject_genus   = worte[k].genus;
				subject_numerus = worte[k].numerus;
			}
			k++;
		}
		return 1;
	}
	return 0;
}

int analyze_this_as_object(const char *text) {
	int k;
#ifdef DEBUG
	printf("analyze_this_as_object: %s\n", text);
#endif
	if ((do_genus==0) || (do_numerus==0)) {
		k=0;
		while (worte[k].wort!=NULL) {
			if (strcmp(worte[k].typ, text)==0) {
				c_genus = worte[k].genus;
				c_numerus = worte[k].numerus;
				c_casus   = verb_do_casus; // TODO �berpr�fen, ob verb schon angetroffen
				c_artikel = ohne;

				do_genus = worte[k].genus;
				do_numerus = worte[k].numerus;
			}
			k++;
		}
		return 1;
	}
	return 0;
}

int finde_naechstes_subject(const char* text) {
	int i=0;
	char tmp[TBUFSZ];
#ifdef DEBUG
	printf("finde_naechstes_subject: %s\n", text);
	print_state();
#endif
	while (i<strlen(text)) {
		next_token(text,tmp, i);
#ifdef DEBUG
		printf("-%s-\n",tmp);
#endif
		i += strlen(tmp);
		
		if (strcmp("PRONOMEN_PERSONAL", tmp)==0) {
			// Personalpronomen, wir nehmen also die Eingaben des Spielers
			c_genus   = pm_genus;
			c_numerus = pm_numerus;
			c_person  = pm_person;		

			subject_genus   = pm_genus;
			subject_numerus = pm_numerus;
			subject_person  = pm_person;
			
#ifdef DEBUG
			printf("finde_naechstes_subject 1: gefundenes subjekt: -%s-\n",tmp);
#endif
			return 0;
		} else if (strncmp(tmp, "MODIFIER_CORPSE",15)==0) {
			c_genus   = maskulin;
			c_person  = drittePerson;
			subject_genus   = maskulin;
			subject_person  = drittePerson;

			if (find_token("NOUN_CORPSE", text)) {
				c_numerus = n_singular;
				subject_numerus = n_singular;
			} else if (find_token("NOUN_CORPSEs", text)) {
				c_numerus = n_plural;
				subject_numerus = n_plural;
			}

		} else if (strncmp(tmp, "NOUN_",5)==0) {
			analyze_this_as_subject(tmp);
#ifdef DEBUG
			printf("finde_naechstes_subject 2: gefundenes subjekt: -%s-\n",tmp);
#endif

			return 0;
		} else {
			i++;
		}
	}
	return 1;
}


int finde_naechstes_objekt(const char* text) {
	int i=0;
	char tmp[TBUFSZ];
#ifdef DEBUG
	printf("finde_naechstes_objekt: %s\n", text);
	print_state();
#endif
	while (i<strlen(text)) {
		next_token(text,tmp, i);
#ifdef DEBUG
		printf("finde_naechstes_objekt: -%s-\n",tmp);
#endif
		i += strlen(tmp);
	
		if (strcmp("PRONOMEN_PERSONAL", tmp)==0) {
			// Personalpronomen, wir nehmen also die Eingaben des Spielers
			c_genus   = pm_genus;
			c_numerus = pm_numerus;
			c_person  = pm_person;		
			c_casus   = verb_do_casus; // TODO �berpr�fen, ob verb schon angetroffen

			do_genus   = pm_genus;
			do_numerus = pm_numerus;
			
			return 0;
		} else if (strncmp(tmp, "NOUN_",5)==0) {
			analyze_this_as_object(tmp);

			return 0;
		} else if (strncmp(tmp, "MODIFIER_CORPSE",15)==0) {
			c_genus   = maskulin;
			c_person  = drittePerson;		
			c_casus   = verb_do_casus; // TODO �berpr�fen, ob verb schon angetroffen
			c_artikel = ohne;
			if (find_token("NOUN_CORPSE", text)) {
				c_numerus = n_singular;
			} else if (find_token("NOUN_CORPSEs", text)) {
				c_numerus = n_plural;
			}
			return 0;
		} else {
			i++;
		}
	}
	return 1;
}

void clear_subject() {
	subject_person=0;
	subject_genus=0;
	subject_numerus=0;

	modifier_corpse = 0;
}

void clear_object() {
	do_genus=0;
	do_numerus=0;

	modifier_corpse = 0;
}

void clear_verb() {
	//verb_do_casus=0; // TODO
	//verb_praeverb=""; // TODO
	verb_partizip=0;
	verb_infinitiv=0;
}

int finde_naechstes_substantiv(const char* text) {
	int i=0;
	char tmp[TBUFSZ];
#ifdef DEBUG
	printf("finde_naechstes_substantiv: %s\n", text);
#endif
	while (i<strlen(text)) {
		next_token(text,tmp, i);
#ifdef DEBUG
		printf("-%s-\n",tmp);
#endif
		i += strlen(tmp)+1;
		
		if (strcmp("PRONOMEN_PERSONAL", tmp)==0) {
			// Personalpronomen, wir nehmen also die Eingaben des Spielers
			c_genus   = pm_genus;
			c_numerus = pm_numerus;
			c_person  = pm_person;		

			return 0;
		} else if (strncmp(tmp, "MODIFIER_CORPSE",15)==0) {
			c_genus   = maskulin;
			c_person  = drittePerson;		
			if (find_token("NOUN_CORPSE", text)) {
				c_numerus = n_singular;
			} else if (find_token("NOUN_CORPSEs", text)) {
				c_numerus = n_plural;
			}
			return 0;
		} else if (strncmp(tmp, "NOUN_",5)==0) {
			int k=0;
			//printf("1 ####################\n");
			while (worte[k].wort!=NULL) {
				//printf("2 #################### -%s- -%s-\n",worte[k].typ,text);
				if (strcmp(worte[k].typ, tmp)==0) {
					//printf("%s\n", worte[k].typ);
					if (worte[k].casus && c_casus) {
						c_genus = worte[k].genus;
						c_numerus = worte[k].numerus;
						break;
					}
				}
				k++;
			}
			return 0;
		}
	}
	return 1;
}

void append(char* output, const char* input) {
	strcat(output, input);
	if (beginning_of_sentence) {
		if (strlen(input)>0) {
			int pos = strlen(output)-strlen(input);
			output[pos] = toupper(output[pos]);
		}
		beginning_of_sentence = 0;
	}
}

void to_lowercase(char* string, int pos) {
	string[pos] = tolower(string[pos]);
}

void append_kompositum(char* output, const char* firstpart, const char* secondpart) {
	// TODO get Fugenelement for input
	append(output, get_wort(firstpart, nominativ, maskulin|feminin|neutrum, n_singular, c_artikel));
	append(output, get_fugenelement(firstpart));
	int pos = strlen(output);
	append(output, get_wort(secondpart, c_casus, maskulin|feminin|neutrum, n_singular|n_plural, c_artikel));
	to_lowercase(output, pos);
}

char output[TBUFSZ];

char* german(const char *line) {
	subject_genus = 0; subject_numerus = 0; subject_person = 0;
	pm_genus = 0; pm_numerus = 0; pm_person = 0;
	do_genus = 0; do_numerus = 0;
	c_casus = 0; c_genus = 0; c_numerus = 0; c_person = 0; c_artikel = ohne;
	verb_do_casus = 0; 
	verb_tempus_modus = 0; 
	verb_infinitiv = 0;

	int pos=0;
	char tmp[TBUFSZ];
	char tmp2[TBUFSZ];
	int insert_char = 1;
	int open_parenthesis = 0;
	char made_from[TBUFSZ] = "";
	/* Should a noun's first letter be uppercase or lowercase. Used when building composites */
	int noun_lowercase = 0;

	output[0] = '\0';
	
	/* set standard genus and numerus depending on character class */
#ifndef NO_HACK_H_INCLUDE
	if (Role_if(PM_HEALER)  ||
			Role_if(PM_KNIGHT)  ||
			Role_if(PM_MONK)    ||
			Role_if(PM_PRIEST)  ||
			Role_if(PM_SAMURAI) ||
			Role_if(PM_WIZARD))
#else
	if (0)
#endif
	{
		pm_genus   = maskulin; // change to players choice
		pm_person  = zweitePerson;
		pm_numerus = n_plural;
	} else {
		pm_genus   = maskulin; // change to players choice
		pm_numerus = n_singular;
		pm_person  = zweitePerson;
	}

	
	/* a work around: search for the first apparent subject
		 this gets values for the c_* variables, when there is
		 no proper grammar structure, e.g in the inventory or 
	   in the disoveries */
	c_casus = nominativ;
	verb_tempus_modus = praesens;
	verb_do_casus = akkusativ;
	finde_naechstes_subject(line);
	while (pos<=strlen(line)) {
		insert_char = 1;
		next_token(line, tmp, pos);
#ifdef DEBUG
		print_state();
		printf("tmp     : =%s=\n", tmp);
		printf("line+pos: =%s=\n", line+pos);
#endif
		pos += strlen(tmp);
#ifdef DEBUG
		printf("tmp     : =%s=\n", tmp);
		printf("line+pos: =%s=\n", line+pos);
#endif

		/* printf("tmp: %s ",tmp); print_state(); */
		if (strncmp("PRONOMEN_",tmp, 9)==0) {
			if ((strcmp("PRONOMEN_KEIN", tmp)==0) ||
					(strcmp("PRONOMEN_POSSESSIV", tmp)==0) ||
					(strncmp("PRONOMEN_3P", tmp, 11)==0)) {
				 c_artikel = unbestimmter;
			}
			if ((strcmp("PRONOMEN_POSSESSIV", tmp)==0) && 
		 			(pm_numerus == n_plural)) {
				append(output, get_wort("PRONOMEN_POSSESSIV_PL", c_casus, c_genus, c_numerus, c_artikel));
			} else {
				append(output, get_wort(tmp, c_casus, c_genus, c_numerus, c_artikel));
			}

		} else if (strncmp("SUBJECT",tmp,7)==0) {
			clear_subject();
			clear_verb();
			c_casus = nominativ;
			finde_naechstes_subject(line+pos);
			insert_char = 0;
			/* SUBJECT marks also beginning of the sentence */
			if (strcmp("SUBJECT_IM_SATZ",tmp)!=0) { beginning_of_sentence = 1; }

		} else if (strcmp("OBJECT",tmp)==0) {
			finde_naechstes_objekt(line+pos);
			insert_char = 0;

		} else if (strncmp("ARTIKEL_", tmp, 8)==0) {
			//finde_naechstes_substantiv(line+pos);

			if (strcmp("ARTIKEL_BESTIMMTER", tmp)==0) { c_artikel = bestimmter; }
			else if (strcmp("ARTIKEL_UNBESTIMMTER",tmp)==0) { c_artikel = unbestimmter; }

			if ((strcmp("ARTIKEL_NULL",tmp)==0) || // Null-Artikel
					((strcmp("ARTIKEL_UNBESTIMMTER",tmp)==0) && (c_numerus == n_plural))) { // unbestimmter Artikel im Plural
				c_artikel = ohne;
				insert_char = 0;
			} else {
				append(output, get_wort(tmp, c_casus, c_genus, c_numerus, c_artikel));
			}

		} else if (strncmp("RING_UNIDENTIFIED_", tmp, 18)==0) {
			append(output, get_wort(tmp, nominativ, maskulin|feminin|neutrum, n_singular, c_artikel));

			if (line[pos]!=')') {
				insert_char = 0;
				noun_lowercase = 1; 

				/* check for linking element(Fugenelemente) */
				/* Words ending in "ung" always have the linking element 's' */
				if (strncmp("ung", &output[strlen(output)-3],3)==0) { append(output, "s"); }
				/* crude heuristic, this works only because of the limited set of words,
					 Koralle, Perle, Tigerauge get the linking element 'n', but Jade doesn't */
				else if 
					((strncmp("le", &output[strlen(output)-2],2)==0) ||
					 (strncmp("ge", &output[strlen(output)-2],2)==0)) { append(output, "n"); }
			}
			
		} else if (strncmp("NOUN_CORPSE", tmp, 11)==0) {
			if (modifier_corpse > 0) {
				output[strlen(output)-1] = '\0';
			} else {
				append(output, get_substantiv(tmp, c_casus, c_numerus, c_artikel));
			}

		} else if (strcmp("MODIFIER_CORPSE", tmp)==0) {
			enum Numerus corpse_numerus;

		  // erzeugt: Leichnam eines Kobolds / Leichname von Kobolden
			// wenn keine PRONOMEN_POSSESSIV vor MODIFIER_CORPSE steht,
			// wird der bestimmte Artikel eingef�gt
			if (find_token("NOUN_CORPSE", line+pos)) {
				modifier_corpse = 1;
				corpse_numerus = n_singular;
			} else if (find_token("NOUN_CORPSEs", line+pos)) {
				modifier_corpse = 2;
				corpse_numerus = n_plural;
			}

			//print_state();
			//printf("\nprevious_token\n");
			next_token(line, tmp2, pos+1);
			//printf("\nnext_token %s\n",tmp2);
			if (strcmp("MODIFIER_EIGENNAME", tmp2)==0) {
				pos += strlen("MODIFIER_EIGENNAME")+1;
				next_token(line, tmp2, pos+1);
				//printf("\nnext_token %s\n",tmp2);

				append(output, get_wort(tmp2, genitiv, beliebiger_genus, n_singular, beliebiger_artikel));
				pos += strlen(tmp2)+1;

				modifier_corpse = 0;

			} else {
				previous_token(line, tmp2, pos-strlen("MODIFIER_CORPSE"));
				if ((!strcmp("PRONOMEN_POSSESSIV", tmp2)==0) &&
						(!strncmp("ARTIKEL_", tmp2, 8)==0) &&
						(!strncmp("ADJEKTIV_", tmp2, 8)==0) &&
						(corpse_numerus == n_singular)){
					// "maskulin" hardkodiert, muss mit NOUN_CORPSE �bereinstimmen
					append(output, get_wort("ARTIKEL_BESTIMMTER", c_casus, maskulin, corpse_numerus, c_artikel));
					append(output, " ");
				}
			
				append(output, get_wort("NOUN_CORPSE", c_casus, maskulin|feminin|neutrum, corpse_numerus, c_artikel));
				finde_naechstes_substantiv(line+pos);
				if (modifier_corpse == 1) {
					c_casus = genitiv; c_numerus = n_singular;
					if ((!strncmp("ARTIKEL_",line+pos+1,8)==0) &&
							(!strncmp("KASUS_",line+pos+1,6)==0) &&
							(!strncmp("MODIFIER_EIGENNAME",line+pos+1,18)==0)) {
						append(output, " ");
						append(output, get_wort("ARTIKEL_UNBESTIMMTER", c_casus, c_genus, c_numerus, c_artikel));
					}
				} else {
					c_casus = dativ; c_numerus = n_plural;
					append(output, " von");
				}
			}
			//modifier_corpse = 0;
			insert_char = 1;

		} else if (strncmp("NOUN_", tmp, 5)==0) {
#if DEBUG
			printf("token nach NOUN_: %s -%s- %d\n",tmp,tmp2, strncmp("NOUN_CORPSE", tmp2,11));
#endif

			int beginning_of_appended_word = strlen(output);

			next_token(line, tmp2, pos+1);
			if (strncmp(tmp2, "NOUN_MEAT",9)==0) {
				append_kompositum(output, tmp, "NOUN_MEAT");
				insert_char = 0;
				pos += 9;
			} else if (strncmp("NOUN_FLASCHE", tmp, 12)==0) {
				// jump over next token (PARTIKEL_OF)
				int len_partikel_of = strlen(tmp2);
			  pos += len_partikel_of+1;

				append(output, get_substantiv(tmp, c_casus, c_numerus, c_artikel));
				// Fl�ssigkeiten in Flaschen haben nur Singularform 
				c_numerus = n_singular;
			} else {
				append(output, get_substantiv(tmp, c_casus, c_numerus, c_artikel));
#if DEBUG
				print_state();
				printf("NOUN_: neues output: %s, tmp: =%s=, getwort: %s\n", output, tmp, get_wort(tmp, c_casus, c_genus, c_numerus, c_artikel));
#endif
			}
			
			if (noun_lowercase) {
				noun_lowercase = 0;
				output[beginning_of_appended_word] = tolower(output[beginning_of_appended_word]);
			}
			
			if (strncmp("NOUN_TIN", tmp, 8)==0) { partikel_of_as_mit = 1; }

			if (strncmp("NOUN_PAAR", tmp, 9)==0) {
				if (c_casus == nominativ) {
					c_casus = nominativ;
				} else {
					c_casus = genitiv;
				}
				c_numerus = n_plural;
				c_artikel = ohne;
			}
			
			if (strlen(made_from)>0) {
				append(output, " aus ");
				append(output, get_wort(made_from, akkusativ, maskulin|feminin|neutrum, n_singular, c_artikel));
			}
		} else if (strncmp("VERB_",tmp,4)==0) {
#ifdef DEBUG
			printf("VERB_ %d\n", subject_person);
#endif
			if (subject_person==0) { subject_person = zweitePerson; }
			if (subject_numerus==0) { subject_numerus = n_singular; } // change to players choice
			append(output, get_verb(tmp, subject_person, subject_numerus, verb_tempus_modus));
			c_artikel = grundform; // f�r pr�dikativen Gebrauch n�tig "Das Pferd ist gesattelt."

		} else if (strncmp("ADJEKTIV_", tmp, 9)==0) {
#ifdef DEBUG
			printf("ADJEKTIV_ %s\n", tmp);
			printf("c_casus %d\n", c_casus);
			print_state();
#endif
			//finde_naechstes_substantiv(line+pos);
			if ((c_genus == 0) && (c_numerus == 0) && (c_numerus ==0)) {
				// alleinstehendes Adjektiv
				append(output, get_adjektiv(tmp, nominativ, maskulin, n_singular, grundform));
			} else {
				append(output, get_adjektiv(tmp, c_casus, c_genus, c_numerus, c_artikel));
			}

		} else if (strncmp("PARTIKEL_", tmp, 9)==0) {
			//finde_naechstes_substantiv(line+pos);
			if (strcmp("PARTIKEL_OF", tmp)==0) {
				finde_naechstes_substantiv(line+pos);
				if (!partikel_of_as_mit) {
					c_casus = genitiv;

					/* only add the definite article if there isn't another article */
					next_token(line, tmp2, pos+1);
					if (!strncmp("ARTIKEL_", tmp2, 8)==0) {
						c_artikel = bestimmter;
						append(output, get_wort("ARTIKEL_BESTIMMTER", c_casus, c_genus, c_numerus, c_artikel));
					} else {
						insert_char = 0;
					}

				} else {
					partikel_of_as_mit = 0;
					append(output, "mit");
					c_casus = nominativ; /* FIX ME: mit + Dativ-E h�rt sich das sehr seltsam an. */ 
				}
			} else if (strcmp("PARTIKEL_LABELED", tmp)==0) {
				insert_char = 0;
			} else {
				append(output, get_wort(tmp, c_casus, c_genus, c_numerus, c_artikel));
			}
		} else if (strncmp("MADE_OF_", tmp, 8)==0) {
			if (open_parenthesis) {
				append(output, get_wort(tmp, nominativ, maskulin|feminin|neutrum, n_singular, c_artikel));
			} else {
				strcpy(made_from, tmp);
				insert_char = 0;
			}
		} else if (strcmp("SATZKLAMMER", tmp)==0) {
			//printf("satzklammer output: -%s-\n", output);
			//printf("strlen(output)-1      %d\n", strlen(output)-1);
			//printf("strlen(verb_praeverb) %d\n", strlen(verb_praeverb));
			if (strlen(verb_praeverb)>0) {
				if (output[strlen(output)-1]!=' ') { append(output, " "); }
					append(output, verb_praeverb);
			} else {
				if (output[strlen(output)-1]==' ') {
					output[strlen(output)-1] = '\0';
					insert_char = 0;
				}
			}

		} else if (strncmp("NEUES_OBJECT", tmp, 6)==0) {
			insert_char = 0;
			clear_object();
		} else if (strncmp("NEUER_SATZ", tmp, 6)==0) {
			insert_char = 0;
			
			clear_subject();
			clear_object();
			clear_verb();
		} else if (strncmp("KASUS_", tmp, 6)==0) {
			insert_char = 0;

			if      (strcmp("KASUS_NOMINATIV", tmp)==0) { finde_naechstes_substantiv(line+pos); c_casus = nominativ; }
			else if (strcmp("KASUS_GENITIV", tmp)==0)   { finde_naechstes_substantiv(line+pos); c_casus = genitiv; }
			else if (strcmp("KASUS_DATIV", tmp)==0)     { finde_naechstes_substantiv(line+pos); c_casus = dativ; }
			else if (strcmp("KASUS_AKKUSATIV", tmp)==0) { finde_naechstes_substantiv(line+pos); c_casus = akkusativ; }

		} else if (strncmp("MODIFIER_", tmp, 9)==0) {
			insert_char = 0;

			if      (strcmp("MODIFIER_MASKULIN", tmp)==0) { c_genus = maskulin; }
			else if (strcmp("MODIFIER_FEMININ", tmp)==0)  { c_genus = feminin; }
			else if (strcmp("MODIFIER_NEUTRUM", tmp)==0)  { c_genus = neutrum; }

			else if (strcmp("MODIFIER_VERB_ZWEITE_PERSON", tmp)==0) { subject_person = zweitePerson; }
			else if (strcmp("MODIFIER_VERB_DRITTE_PERSON", tmp)==0) { subject_person = drittePerson; }

			else if (strcmp("MODIFIER_VERB_SINGULAR", tmp)==0) { subject_numerus = n_singular; }
			else if (strcmp("MODIFIER_VERB_PLURAL", tmp)==0)   { subject_numerus = n_plural; }

			else if (strcmp("MODIFIER_VERB_PRAESENS", tmp)==0) { verb_tempus_modus = praesens; }
			else if (strcmp("MODIFIER_VERB_PRAETERITUM", tmp)==0) { verb_tempus_modus = praeteritum; }
			else if (strcmp("MODIFIER_VERB_PARTIZIP_PRAESENS", tmp)==0) { verb_partizip = partizip_praesens; }
			else if (strcmp("MODIFIER_VERB_PARTIZIP_PERFEKT", tmp)==0) { verb_partizip = partizip_perfekt; }
			else if (strcmp("MODIFIER_KONJUNKTIV", tmp)==0) { verb_tempus_modus = konjunktiv; }
			else if (strcmp("MODIFIER_KONJUNKTIV_II", tmp)==0) { verb_tempus_modus = konjunktiv_ii; }
			else if (strcmp("MODIFIER_VERB_INFINITIV", tmp)==0) { verb_infinitiv = 1; }
			else if (strcmp("MODIFIER_EIGENNAME", tmp)==0) { }
			else {
				fprintf(stderr, "Unbekannter Modifier %s\n",tmp);
			}

		} else if (strncmp("NUMERUS_", tmp, 8)==0) {
			insert_char = 0;
			if      (strcmp("NUMERUS_SINGULAR", tmp)==0) { c_numerus = n_singular; }
			else if (strcmp("NUMERUS_PLURAL", tmp)==0)   { c_numerus = n_plural; }

		} else {
#ifdef DEBUG
			printf("Nichts zu holen mit %s\n", tmp);
#endif
			append(output, tmp);
		}

#ifdef DEBUG
		printf("line[pos] %c\n", line[pos]);
#endif	
		/* handle cases like ' NOUN_POTION PARTIKEL_OF NOUN_POT_FULL_HEALING (ADJEKTIV_POT_SWIRLY)'
			 ADJEKTIV_POT_SWIRLY should be translated only with the stem */
		if (line[pos]=='(') { c_artikel = grundform; open_parenthesis = 1; }

		
		if ((insert_char)||(!isalpha(line[pos]) && (line[pos]!=' '))) {
			int len=strlen(output);
			//printf("insert_char: %d line[pos]: #%c# output[len]: #%c#\n",insert_char, line[pos], output[len]);
			output[len] = line[pos]; output[len+1] = '\0';
		}
		pos++; 

#ifdef DEBUG
		printf("output: %s\n", output);
#endif
	}

	/* setze ersten Buchstaben als Grossbuchstaben.
	 Zwei F�lle: PRONOMEN_PERSONAL VERB_X
	             Bla bla bla.*/
	if (isupper(line[0])) {
		output[0] = toupper(output[0]);
	}

#ifdef __MINGW32__
	FILE *file = fopen("nethack-de.log", "a");
#else
	FILE *file = fopen("nethack-de.log", "a");
#endif
	fprintf(file, "%s\n",line);
	fprintf(file, "%s\n",output);
	fclose(file);

	return (char *)output;
}

void process(char *text[]) {
	char *ret = NULL;
	printf("%s\n", text[0]);
	ret = german(text[0]);
	printf("%s\n", ret);
	if (strcmp(ret, text[1])!=0) {
		printf("Converted: >%s<\n", ret);
		printf("Expected:  >%s<\nmismatch\n", text[1]);
		exit(10);
	}
	printf("\n");
}

/*
[x] Leichname werden noch falsch behandelt.
You see here a NOUN_LICHEN NOUN_CORPSE.
-> Du siehst hier einen Leichnam einer Flechte.

[X] Der erste Buchstabe der bearbeiteten Zeile darf nur in bestimmten Momenten gross gemacht werden.

TO DO:
Nachgestellte Teile wie "called","of"

Es sind vermutlich einige Saetze mit separierten Verben n�tig, wie ich das mache, weiss ich noch nicht.
du finish eating the NOUN_LICHEN corpse.
	Addendum: aktuelle Idee: SATZKLAMMER steht f�r den Platz, an dem die separierten Teile hinkommen.


Das ist Deutsch, also sollten die Objekte auch umkehrbar sein.
char text5[] = "OBJECT ARTIKEL_UNBESTIMMTER NOUN_HUND VERB_MISS SUBJECT ARTIKEL_BESTIMMTER NOUN_TROLL.";

d - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION of ADJEKTIV_POT_SICKNESS
k - ARTIKEL_UNBESTIMMTER ADJEKTIV_POT_SKY_BLUE NOUN_POTION
q - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION of ADJEKTIV_POT_HALLUCIN
r - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION of ADJEKTIV_POT_INVISIBI
s - ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED NOUN_POTION of ADJEKTIV_POT_MONSTER_DE

�BERSETZUNGSVORSCHL�GE
ein ... Trank der Halluzination
oder
ein Halluzinationstrank

ein ... Trank der Unsichtbarkeit
oder
ein Unsichtbarkeitstrank


(alternate weapon; not wielded)
Zweitwaffe, Sekund�rwaffe

(weapon in hand)
<del>ger�stete Waffe</del>
"gef�hrte Waffe" ist wohl der deutsche Ausdruck

Altert�mliche Grammatik wie im Adelung? "ohne Noth", bey statt bei? Das Bewu�tseyn, Thiere 

Gehobene Ausdr�cke?
> Aar - Adler
> Ross - Pferd
> Gestade - Ufer
Leu, Odem, Eiland, Schwingen, Haupt, Antlitz, Isegrim ... 


You see here a NOUN_LICHEN NOUN_CORPSE.
Du siehst hier einen Flechtenleichnam.
Du siehst hier einen Leichnam einer Flechte.
Kann eine Flechte einen Leichnam haben?
Du siehst hier die sterblichen �berreste einer Flechte.

horn of plenty -> F�llhorn
Bag of holding -> Nimmervoller Beutel (oder  tragbares Loch)

scare monster -> Kreaturenbann?

Nwn


*/
