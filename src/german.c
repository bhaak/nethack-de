
#include <string.h>
#include <stdio.h>

#include "hack.h"
#include "german.h"
#define TBUFSZ 300

//#define DEBUG 1

enum Genus subject_genus = 0;
enum Numerus subject_numerus = 0;
enum Person subject_person = 0;

enum Genus   pm_genus = 0;
enum Numerus pm_numerus = 0;
enum Person  pm_person = 0;

enum Genus   do_genus = 0;
enum Numerus do_numerus = 0;

enum Casus  verb_do_casus = 0;

/* c_ => current state */
enum Casus   c_casus = 0;
enum Genus   c_genus = 0;
enum Numerus c_numerus = 0;
enum Person  c_person = 0;
enum Artikel c_artikel = 0;

int corpse = 0;

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
	default: printf("kein Genus "); break;
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
	printf("corpse: %d", corpse);
	printf("\n");
}

const char* get_verb(const char* verb, enum Person p, enum Numerus n) {
	int i=0;
#ifdef DEBUG
	printf("%s %d %d", verb, p, n);
#endif
	while (verben[i].verb != NULL) {
		if ((strcmp(verben[i].typ, verb)==0) && 
				(verben[i].person  & p) && 
				(verben[i].numerus & n)) {
			verb_do_casus = verben[i].casus;
			return verben[i].verb;
		}
		i++;
	}

	return verb;
}

const char* get_wort(const char* typ, enum Casus c, enum Genus g, enum Numerus n) {
	int i=0;
	while (worte[i].wort!=NULL) {
		if ((strcmp(worte[i].typ, typ)==0) && 
				(worte[i].casus   & c) && 
				(worte[i].genus   & g) && 
				(worte[i].numerus & n)) {
			return worte[i].wort;
		}
		i++;
	}
	
	return typ;
}

const char* get_adjektiv(const char* typ, enum Casus c, enum Genus g, enum Numerus n, enum Artikel a) {
	int i=0;
	while (adjektive[i].wort!=NULL) {
		if ((strcmp(adjektive[i].typ, typ)==0) && 
				(adjektive[i].casus   & c) && 
				(adjektive[i].genus   & g) && 
				(adjektive[i].numerus & n) &&
				(adjektive[i].artikel & a)) {
			return adjektive[i].wort;
		}
		i++;
	}
	
	return typ;
}

int finde_naechstes_subject(const char* text) {
	int i=0;
	char tmp[TBUFSZ];
#ifdef DEBUG
	printf("finde_naechstes_subject: %s\n", text);
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
			
			return 0;
		} else if (strncmp(tmp, "NOUN_",5)==0) {
			analyze_this_as_subject(tmp);
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
	char tmp2[TBUFSZ];
	char *subject;
#ifdef DEBUG
	printf("finde_naechstes_objekt: %s\n", text);
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
			c_casus   = verb_do_casus; // TODO �berpr�fen, ob verb schon angetroffen

			do_genus   = pm_genus;
			do_numerus = pm_numerus;
			
			return 0;
		} else if (strncmp(tmp, "NOUN_",5)==0) {
			next_token(text,tmp2,i);
#if DEBUG
			printf("token nach NOUN_ -%s- %d\n",tmp2, strcmp("NOUN_CORPSE", tmp2));
#endif
			analyze_this_as_object(tmp);
			if (strcmp(tmp2, "NOUN_CORPSE")==0) {
				corpse = 1;
			}
			return 0;
		}
	}
	return 1;
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

				do_genus = worte[k].genus;
				do_numerus = worte[k].numerus;
			}
			k++;
		}
		return 1;
	}
	return 0;
}


int finde_naechstes_sustantiv(const char* text) {
	int i=0;
	char tmp[TBUFSZ];
	char *substantiv;
#ifdef DEBUG
	printf("finde_naechstes_sustantiv: %s\n", text);
#endif
	while (i<strlen(text)) {
		next_token(text,tmp, i);
#ifdef DEBUG
		printf("-%s-\n",tmp);
#endif
		i += strlen(tmp)+1;
	
		if (strncmp(tmp, "NOUN_",5)==0) {
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

int next_token(const char* input, char* output, int pos) {
	int i=pos;
	int j=0;
#ifdef DEBUG
	printf("next_token: %s\n", input+i);
#endif

	if (!isspace(input[i])) {
		while (isalpha(input[i])||input[i]=='_') {
			output[j++] = input[i++];
		}
	}
	output[j] = '\0';

#ifdef DEBUG
	printf("next_token return: %s\n", output);
#endif
	return (strlen(output)>0);
}

char output[TBUFSZ];

char* german(const char *line) {
	subject_genus = 0; subject_numerus = 0; subject_person = 0;
	pm_genus = 0; pm_numerus = 0; pm_person = 0;
	do_genus = 0; do_numerus = 0;
	c_casus = 0; c_genus = 0; c_numerus = 0; c_person = 0; c_artikel = ohne;
	verb_do_casus = 0; 

	int pos=0;
	char tmp[TBUFSZ];
	int insert_char = 1;
	int open_parenthesis = 0;
	char made_from[TBUFSZ] = "";
	/* Should a nouns first letter be uppercase or lowercase. Used when building composites */
	int noun_lowercase = 0;

	output[0] = '\0';
	
	/* set standard genus and numerus depending on character class */
#ifdef PM_KNIGHT
	if (Role_if(PM_HEALER)  ||
			Role_if(PM_KNIGHT)  ||
			Role_if(PM_MONK)    ||
			Role_if(PM_PRIEST)  ||
			Role_if(PM_SAMURAI) ||
			Role_if(PM_WIZARD)) {
#else
	if (0) {
#endif
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
	//print_state();
	c_casus = nominativ;
	finde_naechstes_subject(line);
	//print_state();
	while (pos<=strlen(line)) {
		//print_state();
		insert_char = 1;
		next_token(line, tmp, pos);
#ifdef DEBUG
		printf("tmp     : =%s=\n", tmp);
		printf("line+pos: =%s=\n", line+pos);
#endif
		pos += strlen(tmp);

		/* printf("tmp: %s ",tmp); print_state(); */
		if (strncmp("PRONOMEN_",tmp, 9)==0) {
			strcat(output, get_wort(tmp, c_casus, c_genus, c_numerus));

		} else if (strcmp("SUBJECT",tmp)==0) {
			c_casus = nominativ;
			finde_naechstes_subject(line+pos);
			insert_char = 0;

		} else if (strcmp("OBJECT",tmp)==0) {
			finde_naechstes_objekt(line+pos);
			insert_char = 0;

			if (corpse) {
				strcat(output, get_wort("NOUN_CORPSE", c_casus, maskulin|feminin|neutrum, c_numerus));
				verb_do_casus = genitiv; c_casus = genitiv;
				insert_char = 1;
			}
		} else if (strncmp("ARTIKEL_", tmp, 8)==0) {
			//finde_naechstes_sustantiv(line+pos);
			
			if (strcmp("ARTIKEL_BESTIMMTER", tmp)==0) { c_artikel = bestimmter; }
			else if (strcmp("ARTIKEL_UNBESTIMMTER",tmp)==0) { c_artikel = unbestimmter; }

			strcat(output, get_wort(tmp, c_casus, c_genus, c_numerus));

		} else if (strncmp("RING_UNIDENTIFIED_", tmp, 18)==0) {
			strcat(output, get_wort(tmp, nominativ, maskulin|feminin|neutrum, n_singular));

			if (line[pos]!=')') {
				insert_char = 0;
				noun_lowercase = 1; 

				/* check for linking element(Fugenelemente) */
				/* Words ending in "ung" always have the linking element 's' */
				if (strncmp("ung", &output[strlen(output)-3],3)==0) { strcat(output, "s"); }
				/* crude heuristic, this works only because of the limited set of words,
					 Koralle, Perle, Tigerauge get the linking element 'n', but Jade doesn't */
				else if 
					((strncmp("le", &output[strlen(output)-2],2)==0) ||
					 (strncmp("ge", &output[strlen(output)-2],2)==0)) { strcat(output, "n"); }
			}

		} else if (strncmp("NOUN_", tmp, 5)==0) {
			//printf("NOUN_: %s\n", tmp);
			//print_state();
			if (!strcmp("NOUN_CORPSE", tmp)==0) {
				int beginning_of_appended_word = strlen(output);
				// print_state();
				strcat(output, get_wort(tmp, c_casus, c_genus, c_numerus));
				if (noun_lowercase) {
					noun_lowercase = 0;
					output[beginning_of_appended_word] = tolower(output[beginning_of_appended_word]);
				}

				if (corpse) {
					corpse = 0;
					insert_char = 0;
				}
			}

			if (strlen(made_from)>0) {
				strcat(output, " aus ");
				strcat(output, get_wort(made_from, akkusativ, maskulin|feminin|neutrum, n_singular));
			}
		} else if (strncmp("VERB_",tmp,4)==0) {
#ifdef DEBUG
			printf("VERB_ %d\n", subject_person);
#endif
			if (subject_person==0) { subject_person = zweitePerson; }
			if (subject_numerus==0) { subject_numerus = n_singular; } // change to players choice
			strcat(output, get_verb(tmp, subject_person, subject_numerus));

		} else if (strncmp("ADJEKTIV_", tmp, 9)==0) {
			//finde_naechstes_sustantiv(line+pos);
			strcat(output, get_adjektiv(tmp, c_casus, c_genus, c_numerus, c_artikel));

		} else if (strncmp("PARTIKEL_", tmp, 9)==0) {
			//finde_naechstes_sustantiv(line+pos);
			if (strcmp("PARTIKEL_OF", tmp)==0) {
				finde_naechstes_sustantiv(line+pos);
				//print_state();
				//printf("%s\n",line+pos);
				//print_state();
				c_casus = genitiv;
				strcat(output, get_wort("ARTIKEL_BESTIMMTER", c_casus, c_genus, c_numerus));
			} else {
				strcat(output, get_wort(tmp, c_casus, c_genus, c_numerus));
			}
		} else if (strncmp("MADE_OF_", tmp, 8)==0) {
			if (open_parenthesis) {
				strcat(output, get_wort(tmp, nominativ, maskulin|feminin|neutrum, n_singular));
			} else {
				strcpy(made_from, tmp);
				insert_char = 0;
			}
		} else {
#ifdef DEBUG
			printf("Nichts zu holen mit %s\n", tmp);
#endif
			strcat(output, tmp);
		}

#ifdef DEBUG
		printf("line[pos] %c\n", line[pos]);
#endif	
		/* handle cases like ' NOUN_POTION PARTIKEL_OF NOUN_POT_FULL_HEALING (ADJEKTIV_POT_SWIRLY)'
			 ADJEKTIV_POT_SWIRLY should be translated only with the stem */
		if (line[pos]=='(') { c_artikel = grundform; open_parenthesis = 1; }

		int len=strlen(output);
		//printf("insert_char: %d line[pos]: #%c# output[len]: #%c#\n",insert_char, line[pos], output[len]);
		if (insert_char) {
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

	FILE *file = fopen("/tmp/log", "a");
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

#ifndef PM_KNIGHT
int main() {

	char *textz[] = {"  NOUN_RING PARTIKEL_OF ADJEKTIV_RING_CONFLICT (RING_UNIDENTIFIED_ENGAGEMENT)",
									 "  Ring des ADJEKTIV_RING_CONFLICT (Verlobung)"};
	char *texty[] = {"X - ARTIKEL_UNBESTIMMTER RING_UNIDENTIFIED_CORAL NOUN_RING",
									 "X - ein Korallenring"};
	char *textx[] = {"X - ARTIKEL_UNBESTIMMTER RING_UNIDENTIFIED_WOODEN NOUN_RING",
									 "X - ein Holzring"};
	char *textw[] = {"X - ARTIKEL_UNBESTIMMTER RING_UNIDENTIFIED_ENGAGEMENT NOUN_RING",
									 "X - ein Verlobungsring"};
	char *textv[] = {"N - ARTIKEL_UNBESTIMMTER MADE_OF_SPE_VELLUM NOUN_SPELLBOOK.",
									 "N - ein Zauberbuch aus Velum."};
	char *textu[] = {"NOUN_SPELLBOOK PARTIKEL_OF NOUN_SPE_SLEEP (MADE_OF_SPE_VELLUM)",
									 "Zauberbuch des Schlafes (Velum)"};
	char *textt[] = {"z - ARTIKEL_UNBESTIMMTER MADE_OF_WAND_MAPLE NOUN_WAND",
									 "z - ein Zauberstab aus Ahornholz"};
	char *texts[] = {"NOUN_WAND PARTIKEL_OF NOUN_WAND_LIGHT (MADE_OF_WAND_MAPLE)",
									 "Zauberstab des Lichtes (Ahornholz)"};
	char *textr[] = {"K - ARTIKEL_UNBESTIMMTER ADJEKTIV_WAND_CURVED NOUN_WAND",
									 "K - ein gebogener Zauberstab"};
	char *textq[] = {"  NOUN_POTION PARTIKEL_OF NOUN_POT_HEALING (ADJEKTIV_POT_SWIRLY)",
									 "  Trank der Heilung (verwirbelt)"};
	char *textp[] = {"SUBJECT PRONOMEN_PERSONAL VERB_SEE hier OBJECT ARTIKEL_UNBESTIMMTER NOUN_JACKAL NOUN_CORPSE.",
									 "Du siehst hier die sterblichen �berreste eines Schakals."};
	char *texto[] = {"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION PARTIKEL_OF NOUN_POT_BLINDNESS.",
									 "v - ein nicht verfluchter Trank der Blindheit."};
	char *textn[] = {"v - ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED NOUN_WAND PARTIKEL_OF NOUN_WAND_DEATH (0:4)",
									 "v - ein verfluchter Zauberstab des Todes (0:4)"};
	char *textm[] = {"t - ARTIKEL_UNBESTIMMTER NOUN_POTION PARTIKEL_CALLED vielleicht Wasser?",
									 "t - ein Trank bezeichnet mit vielleicht Wasser?"};
	char *textl[] = {"f - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_SACK PARTIKEL_NAMED Necessaire",
									 "f - ein nicht verfluchter Sack genannt Necessaire"};
	char *textk[] = {"l - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_BLINDFOLD",
									 "l - eine nicht verfluchte Augenbinde"};
	char *textj[] = {"SUBJECT PRONOMEN_PERSONAL VERB_SEE hier OBJECT 2 NOUN_LICHENs NOUN_CORPSE.", 
									 "Du siehst hier die sterblichen �berreste von 2 Flechten."};
	char *texti[] = {"SUBJECT PRONOMEN_PERSONAL VERB_SEE hier OBJECT ARTIKEL_UNBESTIMMTER NOUN_LICHEN NOUN_CORPSE.",
									 "Du siehst hier die sterblichen �berreste einer Flechte."};
	char *texth[] = {"ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED ADJEKTIV_POT_SKY_BLUE NOUN_POTION",
									 "Ein verfluchter himmelblauer Trank"};
	char *textg[] = {"SUBJECT 3 ADJEKTIV_POT_SKY_BLUE NOUN_POTIONs",
									 "3 himmelblaue Tr�nke"};
	char *textf[] = {"SUBJECT ARTIKEL_UNBESTIMMTER NOUN_BLINDFOLD",
									 "Eine Augenbinde"};
	char *texte[] = {"SUBJECT PRONOMEN_PERSONAL VERB_CAN wieder sehen.",
									 "Du kannst wieder sehen."};
	char *textd[] = {"SUBJECT PRONOMEN_PERSONAL VERB_SEIN zu klein um die T�re zuzuziehen.",
									 "Du bist zu klein um die T�re zuzuziehen."};
	char *textc[] = {"SUBJECT ARTIKEL_UNBESTIMMTER NOUN_BLACK_DRAGON VERB_MISS OBJECT ARTIKEL_UNBESTIMMTER NOUN_BLACK_DRAGON.",
									 "Ein Schwarzdrache verfehlt einen Schwarzdrachen."};
	char *textb[] = {"SUBJECT ARTIKEL_BESTIMMTER NOUN_BLACK_DRAGON VERB_HIT!",
									 "Der Schwarzdrache trifft!"};
	char *texta[] = {"SUBJECT PRONOMEN_POSSESSIV NOUN_TROLL VERB_KICK OBJECT PRONOMEN_POSSESSIV NOUN_DOG.",
									 "Dein Troll tritt deinen Hund."};

	char *text8[] = {"SUBJECT PRONOMEN_PERSONAL VERB_KILL OBJECT ARTIKEL_BESTIMMTER NOUN_SEWER_RAT!",
									 "Du t�test die Kanalratte!"};
	char *text7[] = {"SUBJECT ARTIKEL_BESTIMMTER NOUN_BESTIE VERB_PASSIV_DESTROY!", 
									 "ARTIKEL_BESTIMMTER NOUN_BESTIE VERB_PASSIV_DESTROY!"};
	char *text6[] = {"SUBJECT ARTIKEL_BESTIMMTER NOUN_TROLL VERB_MISS OBJECT ARTIKEL_UNBESTIMMTER NOUN_DOG.",
									 "Der Troll verfehlt einen Hund."};
	char *text5[] = {"SUBJECT ARTIKEL_BESTIMMTER NOUN_TROLL VERB_MISS OBJECT ARTIKEL_UNBESTIMMTER NOUN_DOG.",
									 "Der Troll verfehlt einen Hund."};
	char *text4[] = {"SUBJECT ARTIKEL_BESTIMMTER NOUN_TROLL VERB_MISS OBJECT PRONOMEN_PERSONAL.",
									 "Der Troll verfehlt dich."};
	char *text3[] = {"SUBJECT ARTIKEL_UNBESTIMMTER NOUN_LICHEN VERB_KICK!", 
									 "Eine Flechte tritt!"};
	char *text2[] = {"SUBJECT ARTIKEL_BESTIMMTER NOUN_LICHEN VERB_KICK!",
									 "Die Flechte tritt!"};
	char *text1[] = {"SUBJECT PRONOMEN_PERSONAL VERB_HEAR ged�mpfte Ger�usche.",
									 "Du h�rst ged�mpfte Ger�usche."};
	char *text0[] = {"SUBJECT ARTIKEL_BESTIMMTER NOUN_JACKAL VERB_BITE!",
									 "Der Schakal bei�t!"};

	process(textz);
	process(texty);
	process(textx);
	process(textw);
	process(textv);
	process(textu);
	process(textt);
	process(texts);
	process(textr);
	process(textq);
	process(textp);
	process(texto);
	process(textn);
	process(textm);
	process(textl);
	process(textk);
	process(textj);
	process(texti);
	process(texth);
	process(textg);
	process(textf);
	process(texte);
	process(textd);
	process(textc);
	process(textb);
	process(texta);
	process(text8);
	process(text7);
	process(text6);
	process(text5);
	process(text4);
	process(text3);
	process(text2);
	process(text1);
	process(text0);

	return 0;
}
#endif

/*
[x] Leichname werden noch falsch behandelt.
You see here a NOUN_LICHEN NOUN_CORPSE.
-> Du siehst hier einen Leichnam einer Flechte.

[X] Der erste Buchstabe der bearbeiteten Zeile darf nur in bestimmten Momenten gross gemacht werden.

TO DO:
Nachgestellte Teile wie "called","of"

Es sind vermutlich einige Saetze mit separierten Verben n�tig, wie ich das mache, weiss ich noch nicht.
du finish eating the NOUN_LICHEN corpse.


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
