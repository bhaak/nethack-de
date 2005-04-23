
#include <string.h>
#include <stdio.h>

//#include "hack.h"
#include "german.h"
#define TBUFSZ 300

//#define DEBUG 1

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

enum Casus  verb_do_casus = 0;

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
	printf("modifier_corpse: %d ", modifier_corpse);
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
			verb_praeverb = verben[i].praeverb;
			return verben[i].verb;
		}
		i++;
	}

	return verb;
}

const char* get_wort(const char* typ, enum Casus c, enum Genus g, enum Numerus n) {
	int i=0;
	int len=strlen(typ);
	// NOUN_FOXs -> NOUN_FOX
	// NOUN_FOX  -> NOUN_FOX
	if (typ[len]=='s') { len--; }
	
	while (worte[i].wort!=NULL) {
		if ((strncmp(worte[i].typ, typ, len)==0) && 
				(worte[i].casus   & c) && 
				(worte[i].genus   & g) && 
				(worte[i].numerus & n)) {
			return worte[i].wort;
		}
		i++;
	}
	
	return typ;
}

const char* get_substantiv(const char* typ, enum Casus c, enum Numerus n) {
	return get_wort(typ, c, maskulin|feminin|neutrum, n);
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
	char tmp2[TBUFSZ];
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
			
#ifdef DEBUG
			printf("finde_naechstes_subject: gefundenes subjelt: -%s-\n",tmp);
#endif
			return 0;
		} else if (strncmp(tmp, "NOUN_",5)==0) {
			analyze_this_as_subject(tmp);
#ifdef DEBUG
			printf("finde_naechstes_subject: gefundenes subjekt: -%s-\n",tmp);
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
		i += strlen(tmp);
	
		if (strcmp("PRONOMEN_PERSONAL", tmp)==0) {
			// Personalpronomen, wir nehmen also die Eingaben des Spielers
			c_genus   = pm_genus;
			c_numerus = pm_numerus;
			c_person  = pm_person;		
			c_casus   = verb_do_casus; // TODO überprüfen, ob verb schon angetroffen

			do_genus   = pm_genus;
			do_numerus = pm_numerus;
			
			return 0;
		} else if (strncmp(tmp, "NOUN_",5)==0) {
			analyze_this_as_object(tmp);

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
				c_casus   = verb_do_casus; // TODO überprüfen, ob verb schon angetroffen

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
	printf("next_token: -%s-\n", input+i);
#endif
	//do {
		if (!isspace(input[i])) {
			while (isalpha(input[i])||input[i]=='_') {
				output[j++] = input[i++];
			}
		}
		output[j] = '\0';
		//printf("output: %s strlen: %d\n",output, strlen(output));
		/*if (strlen(output)==0) { i++; }
			} while (strlen(output)==0);*/

#ifdef DEBUG
	printf("next_token return: %s\n", output);
#endif
	return (strlen(output)>0);
}

/* returns true, when the token text is found within input */
int find_token(const char* text, const char* input) {
	int i=0;
	char tmp[TBUFSZ];
	
	//printf("\nSchleife\n");

	while (i<strlen(input)) {
		next_token(input,tmp, i);
		//printf("text: -%s- input: -%s-, tmp: -%s- i: %d\n", text, input, tmp, i);
		if (strcmp(text, tmp)==0) { return 1;}
		i += strlen(tmp);
		if (strlen(tmp)==0) { i++;}
	}
	//printf("Schleife Ende \n");
	//printf("return 0\n");
	return 0;
}

int append(char* output, const char* input) {
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

int append_kompositum(char* output, const char* firstpart, const char* secondpart) {
	// TODO get Fugenelement for input
	append(output, get_wort(firstpart, nominativ, maskulin|feminin|neutrum, n_singular));
	int pos = strlen(output);
	append(output, get_wort(secondpart, c_casus, maskulin|feminin|neutrum, n_singular|n_plural));
	to_lowercase(output, pos);
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
	char tmp2[TBUFSZ];
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
#ifdef DEBUG
		printf("tmp     : =%s=\n", tmp);
		printf("line+pos: =%s=\n", line+pos);
#endif

		/* printf("tmp: %s ",tmp); print_state(); */
		if (strncmp("PRONOMEN_",tmp, 9)==0) {
			append(output, get_wort(tmp, c_casus, c_genus, c_numerus));

		} else if (strncmp("SUBJECT",tmp,7)==0) {
			clear_subject();
			c_casus = nominativ;
			finde_naechstes_subject(line+pos);
			insert_char = 0;
			/* SUBJECT marks also beginning of the sentence */
			if (strcmp("SUBJECT_IM_SATZ",tmp)!=0) { beginning_of_sentence = 1; }

		} else if (strcmp("OBJECT",tmp)==0) {
			finde_naechstes_objekt(line+pos);
			insert_char = 0;

		} else if (strncmp("ARTIKEL_", tmp, 8)==0) {
			//finde_naechstes_sustantiv(line+pos);

			if (strcmp("ARTIKEL_BESTIMMTER", tmp)==0) { c_artikel = bestimmter; }
			else if (strcmp("ARTIKEL_UNBESTIMMTER",tmp)==0) { c_artikel = unbestimmter; }

			append(output, get_wort(tmp, c_casus, c_genus, c_numerus));

		} else if (strncmp("RING_UNIDENTIFIED_", tmp, 18)==0) {
			append(output, get_wort(tmp, nominativ, maskulin|feminin|neutrum, n_singular));

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
			output[strlen(output)-1] = '\0';

		} else if (strcmp("MODIFIER_CORPSE", tmp)==0) {
			if (find_token("NOUN_CORPSE", line+pos)) { modifier_corpse = 1; }
			else if (find_token("NOUN_CORPSEs", line+pos)) { modifier_corpse = 2; }

			append(output, get_wort("NOUN_CORPSE", c_casus, maskulin|feminin|neutrum, n_singular|n_plural));
			if (modifier_corpse == 1) {
				c_casus = genitiv; c_numerus = n_singular;
				if (!strncmp("ARTIKEL_",line+pos+1,7)==0) {
					append(output, " ");
					append(output, get_wort("ARTIKEL_UNBESTIMMTER", c_casus, c_genus, c_numerus));
				}
			} else {
				c_casus = dativ; c_numerus = n_plural;
				append(output, " von");
			}
			modifier_corpse = 0;
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
			} else {
				append(output, get_substantiv(tmp, c_casus, c_numerus));
#if DEBUG
				print_state();
				printf("NOUN_: neues output: %s, tmp: =%s=, getwort: %s\n", output, tmp, get_wort(tmp, c_casus, c_genus, c_numerus));
#endif
			}
			
			if (noun_lowercase) {
				noun_lowercase = 0;
				output[beginning_of_appended_word] = tolower(output[beginning_of_appended_word]);
			}
			
			if (strncmp("NOUN_TIN", tmp, 8)==0) { partikel_of_as_mit = 1; }

			if (strlen(made_from)>0) {
				append(output, " aus ");
				append(output, get_wort(made_from, akkusativ, maskulin|feminin|neutrum, n_singular));
			}
		} else if (strncmp("VERB_",tmp,4)==0) {
#ifdef DEBUG
			printf("VERB_ %d\n", subject_person);
#endif
			if (subject_person==0) { subject_person = zweitePerson; }
			if (subject_numerus==0) { subject_numerus = n_singular; } // change to players choice
			append(output, get_verb(tmp, subject_person, subject_numerus));

		} else if (strncmp("ADJEKTIV_", tmp, 9)==0) {
			//finde_naechstes_sustantiv(line+pos);
			append(output, get_adjektiv(tmp, c_casus, c_genus, c_numerus, c_artikel));

		} else if (strncmp("PARTIKEL_", tmp, 9)==0) {
			//finde_naechstes_sustantiv(line+pos);
			if (strcmp("PARTIKEL_OF", tmp)==0) {
				finde_naechstes_sustantiv(line+pos);
				if (!partikel_of_as_mit) {
					c_casus = genitiv;

					/* only add the definite article if there isn't another article */
					next_token(line, tmp2, pos+1);
					if (!strncmp("ARTIKEL_", tmp2, 8)==0) {
						append(output, get_wort("ARTIKEL_BESTIMMTER", c_casus, c_genus, c_numerus));
					} else {
						insert_char = 0;
					}

				} else {
					partikel_of_as_mit = 0;
					append(output, "mit");
					c_casus = nominativ; /* FIX ME: mit + Dativ-E hört sich das sehr seltsam an. */ 
				}
			} else {
				append(output, get_wort(tmp, c_casus, c_genus, c_numerus));
			}
		} else if (strncmp("MADE_OF_", tmp, 8)==0) {
			if (open_parenthesis) {
				append(output, get_wort(tmp, nominativ, maskulin|feminin|neutrum, n_singular));
			} else {
				strcpy(made_from, tmp);
				insert_char = 0;
			}
		} else if (strcmp("SATZKLAMMER", tmp)==0) {
			//printf("satzklammer output: -%s-\n", output);
			//printf("strlen %d\n", strlen(output)-1);
			if (output[strlen(output)-1]!=' ') { append(output, " "); }
			append(output, verb_praeverb);

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
	 Zwei Fälle: PRONOMEN_PERSONAL VERB_X
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

/*
[x] Leichname werden noch falsch behandelt.
You see here a NOUN_LICHEN NOUN_CORPSE.
-> Du siehst hier einen Leichnam einer Flechte.

[X] Der erste Buchstabe der bearbeiteten Zeile darf nur in bestimmten Momenten gross gemacht werden.

TO DO:
Nachgestellte Teile wie "called","of"

Es sind vermutlich einige Saetze mit separierten Verben nötig, wie ich das mache, weiss ich noch nicht.
du finish eating the NOUN_LICHEN corpse.
	Addendum: aktuelle Idee: SATZKLAMMER steht für den Platz, an dem die separierten Teile hinkommen.


Das ist Deutsch, also sollten die Objekte auch umkehrbar sein.
char text5[] = "OBJECT ARTIKEL_UNBESTIMMTER NOUN_HUND VERB_MISS SUBJECT ARTIKEL_BESTIMMTER NOUN_TROLL.";

d - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION of ADJEKTIV_POT_SICKNESS
k - ARTIKEL_UNBESTIMMTER ADJEKTIV_POT_SKY_BLUE NOUN_POTION
q - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION of ADJEKTIV_POT_HALLUCIN
r - ARTIKEL_UNBESTIMMTER ADJEKTIV_UNCURSED NOUN_POTION of ADJEKTIV_POT_INVISIBI
s - ARTIKEL_UNBESTIMMTER ADJEKTIV_CURSED NOUN_POTION of ADJEKTIV_POT_MONSTER_DE

ÜBERSETZUNGSVORSCHLÄGE
ein ... Trank der Halluzination
oder
ein Halluzinationstrank

ein ... Trank der Unsichtbarkeit
oder
ein Unsichtbarkeitstrank


(alternate weapon; not wielded)
Zweitwaffe, Sekundärwaffe

(weapon in hand)
<del>gerüstete Waffe</del>
"geführte Waffe" ist wohl der deutsche Ausdruck

Altertümliche Grammatik wie im Adelung? "ohne Noth", bey statt bei? Das Bewußtseyn, Thiere 

Gehobene Ausdrücke?
> Aar - Adler
> Ross - Pferd
> Gestade - Ufer
Leu, Odem, Eiland, Schwingen, Haupt, Antlitz, Isegrim ... 


You see here a NOUN_LICHEN NOUN_CORPSE.
Du siehst hier einen Flechtenleichnam.
Du siehst hier einen Leichnam einer Flechte.
Kann eine Flechte einen Leichnam haben?
Du siehst hier die sterblichen Überreste einer Flechte.

horn of plenty -> Füllhorn
Bag of holding -> Nimmervoller Beutel (oder  tragbares Loch)

scare monster -> Kreaturenbann?

Nwn


*/
