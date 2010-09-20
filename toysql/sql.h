#ifndef SQL_H
#define SQL_H

/* Defaultext  */ 
char *defaultext(char *fname, char *ext, int force);

/*  Openinfile  */  
void openinfile(char *fname);  

/*  init  */ 
void init(void);  

/*  Error function */ 
void error( char *msg);  

/*  Error2 function */  
void error2(char *s1, char *s2);  

/* Lowcase  */ 
void lowcase(char *s); 

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

/*  IDENTIFIER  */ 
int IDENTIFIER(void);  

/*  D  */  
int d(char *s1, char *s2, char *s3);  
  
#endif 


 






 
