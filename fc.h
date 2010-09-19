#ifndef FC_H
#define FC_H

/* Defaultext  */ 
char *defaultext(char *fname, char *ext, int force);

/*  Openinfile  */  
void openinfile(char *fname);  

/*  Openoutfile  */  
void openoutfile(char *fname);  

/*  init  */ 
void init(void);  

/*  Error function */ 
void error( char *msg);  

/*  Error2 function */  
void error2(char *s1, char *s2);  

/* Lowcase  */ 
void lowcase(char *s); 

/* Out  */ 
int out(char *line); 

/* Gettoken */ 
void gettoken(void); 

/* Get2  */ 
void get2(void); 

/*  Type  */ 
int type(int c); 

/*  Match  */  
int match(char *s); 

/*  Id  */ 
int id(char *name); 

/*  Number  */ 
int number(char *name);

/*  String  */ 
int string(char *name);

/*  Label  */ 
void label(char *name); 

/*  Enter  */ 
void enter(char *key, char *val); 

/*  Lookup  */ 
int lookup(char *key);  

/*  Subst  */  
char *subst(char *key);  

/*  Remove_  (note the underscore -  */ 
/*  stdio.h has a remove function)   */ 
void remove_(char *key);  

/* PROGRAM  */ 
int PROGRAM(void);  

/*  OPT_DECLARATION  */ 
int OPT_DECLARATION(void); 

/*  DECLARATION  */  
int DECLARATION(void); 

/*  ID_SEQUENCE  */  
int ID_SEQUENCE(void);  

/*  IDENTIFIER  */  
int IDENTIFIER(void);  

/*  STATEMENT  */  
int STATEMENT(void); 

/*  BLOCK  */  
int BLOCK(void);  

/*  DECL_OR_ST  */  
int DECL_OR_ST(void);  

/*  IO_STATEMENT  */  
int IO_STATEMENT(void);  

/*  PRINTS_STATEMENT  */  
int PRINTS_STATEMENT(void);  

/*  PRINTN_STATEMENT  */  
int PRINTN_STATEMENT(void); 

/*  PRINT_STATEMENT  */  
int PRINT_STATEMENT(void);  

/*  COND_STATEMENT  */  
int COND_STATEMENT(void);  

/*  WHILE_STATEMENT  */  
int WHILE_STATEMENT(void);  

/*  ASSIGN_STATEMENT  */  
int ASSIGN_STATEMENT(void);  

/*  EXPRESSION  */  
int EXPRESSION(void);  

/*  OPT_RHS  */  
int OPT_RHS(void);  

/*  RHS_EQ  */  
int RHS_EQ(void);  

/*  RHS_NEQ  */  
int RHS_NEQ(void);  

/*  SIGNED_TERM  */  
int SIGNED_TERM(void);  

/*  PLUS_TERM  */  
int PLUS_TERM(void);  

/*  MINUS_TERM  */  
int MINUS_TERM(void);  

/*  TERM  */ 
int TERM(void);  

/*  PRIMARY  */  
int PRIMARY(void);  

/*  EXPR1  */  
int EXPR1(void); 

/*  D  */  
int d(char *s1, char *s2, char *s3);  
  
#endif 


 






 
