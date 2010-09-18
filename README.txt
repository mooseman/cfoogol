	***** cfoogol - a small public domain compiler *****

While looking around for public-domain parsers and interpreters, I 
found this neat little compiler from comp.sources.misc (volume 42).  
When you compile cfoogol.c, you'll get lots of warnings but the code 
does compile successfully, producing an executable, and the executable 
works too - compiling a foogol program and emitting C code.  

Note - the "cb" part (to make the emitted code more readable) doesn't 
seem to work, but that's only a fairly minor thing. 

This compiler was written by Per Lindberg (with changes by John Cowan).  The original README from comp.misc.sources is shown below.     		
*****************************************************************************

README.txt 					Last modified: 2010-09-18


			The FOOGOL-IV compiler
		   release notes and documentation
			   Per Lindberg, QZ
                  The mad programmer strikes again!

                        Version 5.0 changes by
                 John Cowan <cowan@snark.thyrsus.com>

NAME
	fc - foogol compiler

SYNOPSIS
	fc [ -d ] infile [ outfile ]

DESCRIPTION
	fc compiles a foogol program into ugly but portable C.
	Default extensions are ".foo" for the source file and ".c"
	for the compiled file. In other words, the resulting outfile
	is C language, and can be assembled and linked with the
        vanilla UNIX cc program.

	Options: (There is only one switch so far...)

	-d	Sets the debug option, which makes the compiler print
		out internal diagnostics. Useful for debugging and
		understanding the compiler.

	The foogol object code has to be linked
	with the C library in order to be able to do I/O.
	Example:
		fc foo
		cc foo.c -o foo

        You can make the C code more readable with:
                cb -j -s foo.c
        which eliminates superfluous newlines and imposes K&R style.

	The foogol language is basically a very small ALGOL. The
	current syntactic elements are:

	PROGRAM ::=		begin
				[ DECLARATION ; ]
				STATEMENT [ ; STATEMENT ]...
				end

	DECLARATION	::=	integer ID_SEQUENCE

	ID_SEQUENCE	::=	IDENTIFIER [ , IDENTIFIER ]

	STATEMENT	::=	IO_STATEMENT
			!	WHILE_STATEMENT
			!	COND_STATEMENT
			!	BLOCK
			!	ASSIGN_STATEMENT

	BLOCK	    ::=		begin
				[ DECLARATION ]
				[ ; STATEMENT ]...
				end

	IO_STATEMENT	::=	prints ( STRING )
			!	printn ( EXPRESSION )
			!	print

	COND_STATEMENT	::=	if EXPRESSION then STATEMENT
				[ else STATEMENT ]

	WHILE_STATEMENT	::=	while EXPRESSION do STATEMENT

	ASSIGN_STATEMENT::=	IDENTIFIER := EXPRESSION

	EXPRESSION	::=	EXPR1 [ RHS ]

	RHS		::=	= EXPR1
			!	# EXPR1

	SIGNED_TERM	::=	+ TERM
			!	- TERM

	TERM		::=	PRIMARY [ * PRIMARY ]...

	PRIMARY		::=	IDENTIFIER
			!	NUMBER
			!	( EXPRESSION )

	EXPR1		::=	TERM [ SIGNED_TERM ]...

	IDENTIFIER	::=	<the usual thing, and no word reserved>

	NUMBER		::=	<the usual thing, unsigned integers>

	STRING		::=	<the usual thing>

	Example program:

	begin
	  integer n, div, sub, test, testcopy, found, max;
	  test := 2; max := 10; /* number of primes wanted */
	  while n # max do begin
	    div:= test-1; found:= 0;
	    while div-1 do begin
	      testcopy:= test; sub:= 0;
	      while testcopy do begin
	        sub:= sub+1; if sub = div then sub:= 0;
	        testcopy:= testcopy-1
	      end;
	      if sub = 0 then found:= 1;
	      div:= div-1
	    end;
	    if found = 0 then begin
	      n:= n+1;
	      printn(test); prints(" is prime number "); printn(n); print
	    end;
	    test:= test+1
	  end
	end

	The syntax is highly flexible, which means it might easily be
	changed due to some whim. The source code should be checked
	for details and changes before bugs are reported.

	The compiler is written by Per Lindberg, and placed in the
	public domain. The Hacker's Ethic applies. It is based on the
	VALGOL I compiler published by G.A. Edgar in Dr. Dobb's
	Journal May 1985. It was implemented for the purpose of
	demonstrating how a simple compiler works. Therefore, there
	are no optimizations or other frills. You might want to add
	things to it; go right ahead. Happy hacking!

        This version (5.0) is the first to generate C.  Previous versions
        generated Unix-style assembly language for the VAX.  Since
        VAXen are nearly defunct, C seemed a better choice of output
        language.  The necessary changes were very easy.

FILES
	fc.c	Source code for the foogol compiler
	fc	The foogol compiler
	fc.doc	This file
	bar.foo	Your program...

SEE ALSO
	cc, cb

BUGS

	Because parsing is by simple recursive-descent and backtracking,
	there is only one cheerful error message: "Syntax error". No
	hints on missing or superflous semicolons or such hand-holding.
	You're supposed to write correct programs in foogol, Buster!

	Please remember that this is just a 500-line toy
	compiler, so don't expect too much of it.



