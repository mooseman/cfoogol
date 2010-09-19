
/*  fc.c  */  
/*  A small public-domain compiler for an             */ 
/*  Algol-like language.                              */ 
/*  This compiler was written by Per Lindberg.        */ 
/*  Version 5 changes by John Cowan                   */ 
/*  <cowan@snark.thyrsus.com>                         */  
/*  Header file fc.h added by Andy Elvey              */   
/*  It was obtained from comp.sources.misc volume42   */  

/*  USAGE - (after fc.c has been compiled)            */ 
/*  fc infile outfile.c                               */ 
 
/*  It is based on the VALGOL I compiler published by */ 
/*  G.A. Edgar in Dr. Dobb's Journal May 1985.        */  
/*  It was implemented for the purpose of             */    
/*	demonstrating how a simple compiler works.        */  
/*  Therefore, there are no optimizations or other    */  
/*  frills. You might want to add things to it;       */  
/*  go right ahead. Happy hacking!                    */ 

/*  Very many thanks to Per Lindberg and John Cowan   */ 
/*  for writing this code.                            */ 
/*  This code is released to the public domain.       */
/*  "Share and enjoy..."  ;)                          */  


#define UNIX

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <unistd.h> 
#include "fc.h" 

#define is_upper(c) ((c) >= 'A' && (c) <= 'Z')
#define to_lower(c) ((c) - 'A' + 'a')

#define MAXTAB     25           /* Tweak these to your own liking  */
#define MAXTOKEN   80

#define WHITESPACE  0           /* These could be turned into enum */
#define NUMBER      1
#define LETTER      2
#define QUOTE       3
#define SEMICOLON   4
#define RANDOM      5

FILE *inf, *outf;

int labelcount = 0,
    linecount  = 0,
    debug      = 0;

char token[MAXTOKEN],
     pending[MAXTOKEN],
     keytab[MAXTAB][MAXTOKEN],
     symtab[MAXTAB][MAXTOKEN],
     *usage =
     "usage: 'fc [-debug] infile [outfile]'";


int main(int argc, char *argv[]) { 
  if (argc < 2) error(usage);
  if (*argv[1] == '-') { debug = 1; --argc; ++argv; }
  if (argc < 2) error(usage);
  openinfile(argv[1]);
  openoutfile(argv[argc == 2 ? 1 : 2]);
  init();
  if (!PROGRAM()) error("Syntax error");
  fclose(inf);
  fclose(outf);
  
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
  d("openinfile",defaultext(fname,".foo",0),"");
  if ((inf = fopen(defaultext(fname,".foo",0),"r")) == NULL)
    error2("Can't open infile", defaultext(fname,".foo",0));
}


void openoutfile(char *fname) {
  char *defaultext();
  d("openoutfile",defaultext(fname,".c",1),"");
  if ((outf = fopen(defaultext(fname,".c",1),"w")) == NULL)
    error2("Can't open outfile", defaultext(fname,".c",1));
}


void init(void) {
  int i;
  d("init","","");
  get2();
  gettoken();
  for (i = 0; i < MAXTAB; i++) keytab[i][0] = '\0';
}


void error(char *msg) {
  printf("\n\nFoo: %s", msg);
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

int out(char *line) {
  char c, symb[MAXTOKEN], *subst(), *s = symb;
  int printmode = 1, chmode = 1;
  while((c = *line++)) {
    if (c == ' ') { if (chmode) putc('\t',outf);
         chmode = 0;
    } else {
      chmode = 1;
      if (c != 39) { if (printmode) putc(c,outf);
                     else *s++ = c;
      } else if (!printmode) {
             *s = '\0';
             if (*symb) fprintf(outf,"%s",subst(symb));
             printmode = 1;
           } else {
             printmode = 0;
             s = symb;
           }
    }
  }
  putc('\n',outf);
  return 1;
}


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


void label(char *name) {
  char result[6];
  d("label ",name,"");
  sprintf(result,"L%d",labelcount++);
  enter(name,result);
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
  if (!match("begin"))	return 0;	out("#include <stdio.h>");
                                        out("main() {");
  if (!OPT_DECLARATION()) return 0;
  if (!STATEMENT())	return 0;
  while (match(";"))
    if (!STATEMENT())	return 0;
  if (!match("end"))	return 0;	out("}");
  return 1;
}

int OPT_DECLARATION(void) {  
  d("OPT_DECLARATION",token,pending);
  if (DECLARATION()
  && !match(";")) return 0;
  return 1;
}

int DECLARATION(void) { 
  d("DECLARATION",token,pending);
  if (!match("integer")) return 0;	out("int");
  if (!ID_SEQUENCE())	 return 0;	out(";");
  return 1;
}

int ID_SEQUENCE(void) { 
  d("ID_SEQUENCE",token,pending);
  if (!IDENTIFIER())	return 0;
  while (match(",")) {
   out(",");
    if (!IDENTIFIER())	return 0;
	}
  return 1;
}

int IDENTIFIER(void) { 
  d("IDENTIFIER",token,pending);
  if (!id("X"))	return 0;		out("'X'");
	remove_("X");
  return 1;
}

int STATEMENT(void) { 
  d("STATEMENT",token,pending);
  return
  IO_STATEMENT()
  ||
  WHILE_STATEMENT()
  ||
  COND_STATEMENT()
  ||
  BLOCK()
  ||                     /* the order is important here */
  ASSIGN_STATEMENT();
}

int BLOCK(void) { 
  d("BLOCK",token,pending);
  if (!match("begin"))	return 0;	out("{");
  if (DECL_OR_ST())
    while(match(";"))
     if (!STATEMENT())	return 0;
  if (!match("end"))	return 0;	out("}");
  return 1;
}

int DECL_OR_ST(void) { 
  d("DECL_OR_ST",token,pending);
  return
  DECLARATION()
  ||
  STATEMENT();
}

int IO_STATEMENT(void) { 
  d("IO_STATEMENT",token,pending);
  return
  PRINTS_STATEMENT()
  ||
  PRINTN_STATEMENT()
  ||
  PRINT_STATEMENT();
}

int PRINTS_STATEMENT(void) { 
  d("PRINTS_STATEMENT",token,pending);
  if (!match("prints")) return 0;
  if (!match("("))	return 0;
  if (!string("S"))	return 0;	
	 out("printf(\"%s\", 'S');");
	 remove_("S"); 
  if (!match(")"))	return 0;
  return 1;
}

int PRINTN_STATEMENT(void) { 
  d("PRINTN_STATEMENT",token,pending);
  if (!match("printn")) return 0;
  if (!match("("))	return 0; out("printf(\"%d\",");
  if (!EXPRESSION()) return 0; out(");");
  if (!match(")"))	return 0;
  return 1;
}

int PRINT_STATEMENT(void) { 
  d("PRINT_STATEMENT",token,pending);
  if (!match("print"))	return 0; out("printf(\"\\n\");");
  return 1;
}

int COND_STATEMENT(void) { 
  d("COND_STATEMENT",token,pending);
  if (!match("if"))	return 0;	out("if (");
  if (!EXPRESSION())	return 0;	out(")");
  if (!match("then"))	return 0;	
  if (!STATEMENT())	return 0;	
  if (match("else")) {
     out (" else");
    if (!STATEMENT())	return 0;	
	}
  return 1;
}

int WHILE_STATEMENT(void) { 
  d("WHILE_STATEMENT",token,pending);
  if (!match("while"))	return 0;  out("while(");
  if (!EXPRESSION())	return 0;  out(")");
  if (!match("do"))	return 0;	
  if(!STATEMENT())	return 0;	
  return 1;
}

int ASSIGN_STATEMENT(void) { 
  d("ASSIGN_STATEMENT",token,pending);
  if (!id("Var"))	return 0;       out("'Var' =");
  if (!match(":"))	return 0;
  if (!match("="))	return 0;
  if (!EXPRESSION())	return 0;	out(";");
					remove_("Var");
  return 1;
}

int EXPRESSION(void) { 
  d("EXPRESSION",token,pending);
  if (!EXPR1())		return 0;
  if (!OPT_RHS())	return 0;
  return 1;
}

int OPT_RHS(void) { 
  d("OPT_RHS",token,pending);
  return
  RHS_EQ()
  ||
  RHS_NEQ()
  ||
  1;
}

int RHS_EQ(void) { 
  d("RHS_EQ",token,pending);
  if (!match("="))	return 0;	
					out("==");
  if (!EXPR1())		return 0;	
  return 1;
}

int RHS_NEQ(void) { 
  d("RHS_NEQ",token,pending);
  if (!match("#"))	return 0;	
					out("!=");
  if (!EXPR1())		return 0;	
  return 1;
}

int SIGNED_TERM(void) { 
  d("SIGNED_TERM",token,pending);
  return
  PLUS_TERM()
  ||
  MINUS_TERM();
}

int PLUS_TERM(void) { 
  d("PLUS_TERM",token,pending);
  if (!match("+"))	return 0;	out("+");      
  if (!TERM())		return 0;	
  return 1;
}

int MINUS_TERM(void) { 
  d("MINUS_TERM",token,pending);
  if (!match("-"))	return 0;	out("-");
  if (!TERM())		return 0;	
  return 1;
}

int TERM(void) { 
  d("TERM",token,pending);
  if (!PRIMARY())	return 0;
  while (match("*")) { out("*");
    if (!PRIMARY())	return 0;	
  }
  return 1;
}

int PRIMARY(void) { 
  d("PRIMARY",token,pending);
  if (id("Z")) { out("'Z'");       
    remove_("Z");
    return 1;
  }
  if (number("Z")) { out("'Z'");
    remove_("Z");
    return 1;
  }
  if (match("(")) {
      out("(");
    if (!EXPRESSION())	return 0;
    if (!match(")"))	return 0;
       out(")");
    return 1;
  }
  return 0;
}

int EXPR1(void) { 
  d("EXPR1",token,pending);
  if (!TERM())		return 0;
  while(SIGNED_TERM());
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


 

