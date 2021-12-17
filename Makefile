# Glenn Hartwell
# Martin de Salterain
# COP3402, Spring 2021
# HW3 Compiler Makefile

a.out : driver.o error.o lex.o parser.o vm.o
	gcc -o a.out driver.o error.o lex.o parser.o vm.o

driver.o : driver.c compiler.h
	gcc -c driver.c

lex.o : lex.c compiler.h error.c
	gcc -c lex.c

parser.o : parser.c compiler.h error.c
	gcc -c parser.c

vm.o : vm.c compiler.h error.c
	gcc -c vm.c

error.o : error.c compiler.h
	gcc -c error.c

clean :
	rm *.o a.out
