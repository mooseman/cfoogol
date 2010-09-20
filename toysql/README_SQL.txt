     *****  sql.c  *****  

 This is a small public-domain parser for a tiny fragment of the 
SQL language.  It is based on the "foogol" parser written by 
Per Lindberg.  

This parser simply parses the expression - 
select varname from tablename;  

"Varname" and "tablename" can be any valid "identifier" 
strings.  

 Unlike the foogol compiler, this parser does not produce any 
output - it only parses.  However, it may still be useful as a 
very simple example of a parser. 

As usual, this code is released to the public domain. 
"Share and enjoy..."  ;)   


