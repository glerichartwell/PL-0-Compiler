// Glenn Hartwell
// Martin de Salterain
// COP3402, Spring 2021
// HW3 Error Handler

#include "compiler.h"

char *errorMessage[] = 
{
	"",
	// 1
	"Error : program must end with period\n",
	// 2
	"Error : const, var, and read keywords must be followed by identifier\n",
	// 3
	"Error : symbol name has already been declared\n",
	// 4
	"Error : constants must be assigned with =\n",
	// 5
	"Error : constants must be assigned an integer value\n",
	// 6
	"Error : constant and variable declarations must be followed by a semicolon\n",
	// 7
	"Error : undeclared symbol\n",
	// 8
	"Error : only variable values may be altered\n",
	// 9
	"Error : assignment statements must use :=\n",
	// 10
	"Error : begin must be followed by end\n",
	// 11
	"Error : if must be followed by then\n",
	// 12
	"Error : while must be followed by do\n",
	// 13
	"Error : condition must contain comparison operator\n",
	// 14
	"Error : right parenthesis must follow left parenthesis\n",
	// 15, I can potentially add operators to this list with my conditional in parser.c
	"Error : arithmetic equations must contain operands, parentheses, numbers, or symbols\n",
	// 16
	"Error : Identifier names cannot exceed 11 characters\n",
	// 17
	"Error : Numbers cannot exceed 5 digits\n",
	// 18
	"Error : Identifiers cannot begin with a digit\n",
	// 19
	"Error : Invalid Symbol\n",
	// 20
	"Fatal Error : No input file.\n",
	// 21
	"Error : Invalid Directive\n",
	// 22
	"Fatal Error : This instruction does not exist.\n",
	// 23
	"Fatal Error : Not enough room in the stack. Aborting program...\n",
	// 24
	"Fatal Error : Not enough room in the symbol table. Aborting program...\n",
	// 25
	"Error : comments must be closed with '*/'\n"
};

void error(int errorType)
{
	printf("%s", errorMessage[errorType]);
	exit(1); // Comment out to allow execution despite errors
}
