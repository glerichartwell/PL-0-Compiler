// Glenn Hartwell
// Martin de Salterain
// COP3402, Spring 2021
// HW3 Compiler Header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Array Length Maxes
#define BUFFER_LENGTH 1000
#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_STACK_HEIGHT 50
#define MAX_CODE_LENGTH 100 // This is not necessary because there is no file io, but hey, I included just in case

// Error Defined Maxes
#define MAX_DIGITS 5
#define MAX_IDENTIFIER_LENGTH 12 // Identifiers can be at max 11 characters + '\0'

// Used when Symbol Table does not contain identifier
#define NO_SYMBOL -1

// Used to determine if a word is an identifier, 
// also used to determine if punctutation is a part of the language
#define IDENT_TOKEN 2
// Used to set an integer's token value
#define INT_TOKEN 3

// Used to insert const, vars, and procedures(in the future) into symbol table
#define CONST_KIND 1
#define VAR_KIND 2
#define PROC_KIND 3

// Constants used to size specific arrays
#define NUM_SYMBOLS 32
#define NUM_AR_FIELDS 4 // This can be edited to handle procedures for future assignments

// Error Indexes
typedef enum 
{
	missing_period = 1,	missing_ident, duplicate_symbol,
	missing_equals,	missing_value, missing_semicolon,
	undeclared_symbol, not_var,	missing_becomes, 
	missing_end, missing_then, missing_do, 
	missing_comparator, missing_rparent, invalid_equation, 
	ident_too_long, int_too_long,	digit_ident,
	invalid_symbol,	missing_file, invalid_directive, 
	bad_instruction, stack_overflow, sym_table_overflow,
	hanging_comment
} error_code;

typedef enum
{
	modsym = 1, identsym, numbersym, plussym, minussym,
	multsym, slashsym, oddsym, eqlsym, neqsym,
	lessym, leqsym, gtrsym, geqsym, lparentsym,
	rparentsym, commasym, semicolonsym, periodsym, becomessym,
	beginsym, endsym, ifsym, thensym, whilesym,
	dosym, callsym,	constsym, varsym, procsym,
	writesym, readsym, elsesym, returnsym
} token_type;

typedef struct lexeme
{
	char name[MAX_IDENTIFIER_LENGTH];
	int value;
	int type;

} lexeme;

typedef struct instruction
{
	int opcode;
	char op[4];
	int l;
	int m;

} instruction;

typedef struct symbol
{
	int kind;
	char name[MAX_IDENTIFIER_LENGTH];
	int val;
	int level;
	int addr;
} symbol;


lexeme* lexicalAnalysis(char *inputfile, int printFlag);
void runVirtualMachine(instruction *instructionList, int printFlag);
instruction *parseTokenList(lexeme *tokenList, int printFlag);
void error(int error_type);
void program();
void block(int level);
void constDeclaration();
int varDeclaration();
// void procDeclaration(); // Used for future assignments
void statement();
void condition();
void expression();
void term();
void factor();
