# HW4
 Parser & Code Generator

*********************
Glenn Hartwell
Martin de Salterain
COP 3402, Spring 2021
*********************

1. Description
	This program jointly compiles multiple source files (driver.c, parser.c, lex.c, vm.c, error.c) to generate a "compiler" executable
	which takes a source file written in PL/0 and executes it using an integrated virtual machine.

2. Compilation & Execution
	You will need a C program (gcc preferred) on a UNIX system or UNIX bash terminal (such as Ubuntu). A "make" package must also be installed at the terminal.

	To compile and run the program, type the following commands in your UNIX terminal:

			$ make a.out
			$ ./a.out file_name.txt

	There are three optional runtime directives:
			-l displays the output of the lexical analyzer, including the token list.
			-a displays the output of the parser, including the generated assembly in the form of an instruction list.
			-v displays the output of the virtual machine, including the contents of the stack at each step during execution.

	These directives can be executed like so:

			$ ./a.out file_name.txt -l
			$ ./a.out file_name.txt -a
			$ ./a.out file_name.txt -v

	**IMPORTANT**
	Be sure to replace "file_name" with the name of the instruction input file you wish to run.


To compile: 				$ make a.out
To run: 					$ ./a.out <input file name> <directives>
To clear compiled files:	$ make clean
