
/*  sql.c  */  
/*  A small public-domain parser for a                */ 
/*  SQL-like language.                                */ 
/*  Author - Andy Elvey                               */     
/*  Based on the "foogol" compiler written by         */ 
/*  Per Lindberg.                                     */ 
/*  Header file sql.h added by Andy Elvey             */   
/*  USAGE - (after sql.c has been compiled)           */ 
/*  sql infile                                        */  
/*  This code is released to the public domain.       */
/*  "Share and enjoy..."  ;)                          */  


#define UNIX

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <unistd.h> 
#include "sql.h" 

#define is_upper(c) ((c) >= 'A' && (c) <= 'Z')
#define to_lower(c) ((c) - 'A' + 'a')

#define MAXTAB     25           /* Tweak these to your own liking  */
#define MAXTOKEN   80

#define WHITESPACE  0           /* These could be turned into enum */
#define NUMBER      1
#define LETTER      2
#define QUOTE       3
#define SEMICOLON   4
#define STAR        5    
#define RANDOM      6

FILE *inf; 

int labelcount = 0,
    linecount  = 0,
    debug      = 0;

char token[MAXTOKEN],
     pending[MAXTOKEN],
     keytab[MAXTAB][MAXTOKEN],
     symtab[MAXTAB][MAXTOKEN],
     *usage =
     "usage: 'sql [-debug] infile'";


int main(int argc, char *argv[]) { 
  if (argc < 2) error(usage);
  if (*argv[1] == '-') { debug = 1; --argc; ++argv; }
  if (argc < 2) error(usage);
  openinfile(argv[1]);
  init();
  if (!PROGRAM()) error("Syntax error");
  fclose(inf);
  return 0;   
}


char *defaultext(char *fname, char *ext, int force) {
  static char result[255];
  char c, *point, *s = result;
  strcpy(result,fname);
  while (*s) ++s;
  point = s;
  while (c = *s, s > result && c != '.') --s;
  if (c == '.') {                                /* some extension exists */
    point = s;
    if (!force) return result;            /* don't worry about what it is */
  }
  strcpy(point,ext);                 /* put default extension after point */
  return result;
}


void openinfile(char *fname) {
  char *defaultext();
  d("openinfile",defaultext(fname,".sql",0),"");
  if ((inf = fopen(defaultext(fname,".sql",0),"r")) == NULL)
    error2("Can't open infile", defaultext(fname,".sql",0));
}


void init(void) {
  int i;
  d("init","","");
  get2();
  gettoken();
  for (i = 0; i < MAXTAB; i++) keytab[i][0] = '\0';
}


void error(char *msg) {
  printf("\n\nSQL: %s", msg);
  if (linecount) printf(" at line %d",linecount + 1);
  printf("\n");
  exit(1);
}


void error2(char *s1, char *s2) {
  static char msg[80];
  sprintf(msg,"%s\"%s\"",s1,s2);
  error(msg);
}


void lowcase(char *s) {
  char c;
  for ( (c = *s); (c = *s); ++s) 
    if (is_upper(c)) *s = to_lower(c) ;
}


/* Basic I/O functions */

void gettoken(void) {
  strcpy(token,pending); get2();
  if (!strcmp("/",token) && !strcmp("*",pending)) {
    d("comment:",token,pending);
    while (strcmp("*",token) || strcmp("/",pending)) {
      strcpy(token,pending); get2();
      d("        ",token,"");
    }
    strcpy(token,pending); get2();
    strcpy(token,pending); get2();
  }
d("gettoken returning",token,pending);
}


void get2(void) {
  int c0, c, typ, count = 1;
  char *p = pending;
  while((typ=type(c0=getc(inf))) == WHITESPACE) if (c0 == '\n') ++linecount;
  if (c0 != EOF) *p++ = c0;
  if (typ == QUOTE) {
    while ((c = getc(inf)) != EOF && type(c) != QUOTE) {
      if (++count == MAXTOKEN) error("String too long");
      *p++ = c;
    }
    *p++ = '"';
  }
  else {
    while ((type(c=getc(inf)) == typ
            || (typ == LETTER && type(c) == NUMBER) )
	    && typ != RANDOM
            && c != EOF) {
      *p++ = c;
      typ = type(c);
      if (++count == MAXTOKEN) error("Too long input token");
    }
    ungetc(c,inf);
  }
  *p = '\0';  
}


int type(int c) {
  if (c == EOF) return -1;
  if (c >= '0' && c <= '9') return(NUMBER);
  if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return(LETTER);
  if (c == ' ' || c == '\t' || c == '\n') return(WHITESPACE); /*  */
  if (c == '"') return (QUOTE);
  if (c == ';') return (SEMICOLON);
  if (c == '*') return (STAR);
  return(RANDOM);
}


/* Basic input matching functions */

int match(char *s) {
d("match",token,s);
  lowcase(token);
  if (strcmp(s,token)) return 0;
  gettoken();          return 1;
}


int id(char *name) {
  int t;
  char c, *p = token;
  d("id",token,name);
  if (type(*p++) != LETTER) return 0;
  while ((c = *p++)) {
    t = type(c);
    if (t != NUMBER && t != LETTER) return(0);
  }
  lowcase(token);
  enter(name,token);
  gettoken();
  return(1);
}


int number(char *name) {
  char c, *p = token;
  d("number",token,name);
  while ((c = *p++)) {
    if (type(c) != NUMBER) return(0); 
  }  
  enter(name,token);
  gettoken();
  return(1);
}


int string(char *name) {
  d("string",token,name);
  if (*token != '"') return 0;
  enter(name,token);
  gettoken();
  return 1;
}


/* Internal symbol table */

void enter(char *key, char *val) {
  int i;
  d("enter ",val, key);
  for (i = 0; i < MAXTAB; i++) {
    if (keytab[i][0] == '\0') {
      strcpy(keytab[i],key);
      strcpy(symtab[i],val);
      return;
    }
  }
  error2("INTERNAL SYMTAB ENTER ERROR, can't enter ", val); 
  return;  
}


int lookup(char *key) {
  int i;
  for (i = MAXTAB-1; i >= 0 ; i--) {
    if (!strcmp(key,keytab[i])) {
      d("lookup ",symtab[i],key);
      return i;
    }
  }
  error2("INTERNAL SYMTAB LOOKUP ERROR, can't find ", key); 
  return -1; 
}


char *subst(char *key) {
  return symtab[lookup(key)];
}


void remove_(char *key) {
  keytab[lookup(key)][0] = '\0';
}


/* Syntax definition. This is the neat part! */

int PROGRAM(void) { 
  d("PROGRAM",token,pending);
  if (!match("select"))	return 0;        
  if (!IDENTIFIER())  return 0;      
  if (!match("from"))	return 0;        
  if (!IDENTIFIER())  return 0;      
  if (!match(";"))	return 0;                                      
  return 1;
}

int IDENTIFIER(void) { 
  d("IDENTIFIER",token,pending);
  if (!id("X"))	return 0; 
	remove_("X");
  return 1;
} 






/* And finally, the debug function... */
int d(char *s1, char *s2, char *s3) {
  if (debug) {
    printf("%s",s1);
    if (*s2) printf(" \"%s\"",s2);
    if (*s3) printf(" \"%s\"",s3);
    putchar('\n');
  }
  return 1;
}

/*  ***** END OF CODE *****  */  







