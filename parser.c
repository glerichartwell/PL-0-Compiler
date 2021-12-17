// Glenn Hartwell
// Martin de Salterain
// COP3402, Spring 2021
// HW3 Parser / Code Generator

// /----------\  
// |          |
// |----------|  
// |          |
// |----------|  
// |          |				<------ This is a stack and I was gonna
// |----------|						put what the first four things
// |          |						of the AR are here.
// |----------|  
// |          |
// |----------|  
// |          |
// \----------/

#include "compiler.h"

// Global Variables

// Token Access
lexeme *token;
lexeme *tokenList;
int tokenPointer = 0;

// Assembly Code access
instruction *instructionList;
int codeIdx = 0;

// Symbol Table access
symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
int symbolPointer = 0;

// Indices
int loopIdx = 0;
int symIdx = 0;

// Generates Assembly code and stores it in the list of instructions
// to be executed by the virtual machine
int genCode(int opcode, char *op, int l, int m)
{
	if (l < 0)
		error(SOMETHING IS FUCKING WRONG);

	instructionList[codeIdx].opcode = opcode;
	strcpy(instructionList[codeIdx].op, op);
	instructionList[codeIdx].l = l;
	instructionList[codeIdx].m = m;
	return codeIdx++;

}

// Inserts a symbol into the symbol table
void insertSymbol(int kind, char *name, int val, int level, int addr)
{
	// Error if symbol table is full
	if (symbolPointer >= MAX_SYMBOL_TABLE_SIZE)
		error(sym_table_overflow);
	
	symbolTable[symbolPointer].kind = kind;
	strcpy(symbolTable[symbolPointer].name, name);
	symbolTable[symbolPointer].val = val;
	symbolTable[symbolPointer].level = level;
	symbolTable[symbolPointer].addr = addr;
	symbolPointer++;
}

// Searches the symbol table for <string> in O(n) time
int checkSymbolTable(char *string)
{
	int i;
	for (i = symbolPointer - 1; i >= 0; i--)
	{
		if ((strcmp(symbolTable[i].name, string)) == 0)
			return i;
	}
	return NO_SYMBOL;
}

// Returns the next token in the token list
lexeme *getNextToken()
{
	lexeme *tempToken;
	tempToken = &tokenList[tokenPointer];
	tokenPointer++;
	return tempToken;
}

// Runs functions required by the syntactic class "program"
void program(int level)
{
	token = getNextToken();
	block(level);
	if (token->type != periodsym)
		error(missing_period);
	genCode(9,"SYS", level, 3); 
}

// Runs functions required by the syntactic class "block"
void block(int level)
{
	int numVars;
	// used to "delete" symbols that are out of scope
	int oldSymbolPointer = symbolPointer; 

	jmpIndexInstructionList = genCode(7, "JMP", level, address) // declare this variable

	if (token->type == constsym)
	{
		constDeclaration(level);
	}
	if (token->type == varsym)
	{
		numVars = varDeclaration();
	}
	if (token->type == procsym)
	{
		procDeclaration(level); // THIS IS PREEMPTIVE
	}
	instructionList[jmpIndexInstructionList].m = codeIdx; // This is probably right
	genCode(6, "INC", level, (numVars + NUM_AR_FIELDS));
	statement(level);
	genCode(2, "OPR", level, 0) // Generate RTN instruction
	symbolPointer = oldSymbolPointer;
	return;
}

// Runs functions required by the syntactic class "constdeclaration"
void constDeclaration(int level)
{
	char identName[MAX_IDENTIFIER_LENGTH];
	int identValue;

	do
	{

		token = getNextToken();

		if (token->type != identsym)
		{
			error(missing_ident);
		}

		if (checkSymbolTable(token->name) != NO_SYMBOL) // No duplicate constants
			error(duplicate_symbol);

		strcpy(identName, token->name); // Save identifier name
		
		token = getNextToken();
		if (token->type != eqlsym)
			error(missing_equals);

		token = getNextToken();
		if (token->type != numbersym)
			error(missing_value);

		// Store symbol
		identValue = atoi(token->name);
		insertSymbol(CONST_KIND, identName, identValue, level, -1);
		token = getNextToken();

	} while (token->type == commasym);

	if (token->type != semicolonsym)
		error(missing_semicolon);

	token = getNextToken();
	return;
}

// Runs functions required by the syntactic class "var-declaration"
int varDeclaration(int level)
{
	int numVars = 0;
	do
	{
		token = getNextToken();

		if (token->type != identsym)
			error(missing_ident);

		if (checkSymbolTable(token->name) != NO_SYMBOL) // Unique identifiers
			error(duplicate_symbol);

		// Store symbol
		insertSymbol(VAR_KIND, token->name, 0, level, numVars + NUM_AR_FIELDS);
		numVars++;

		// If variable causes us to exceed MAX_STACK_HEIGHT error
		if (numVars >= (MAX_STACK_HEIGHT - NUM_AR_FIELDS))
			error(stack_overflow);
		token = getNextToken();

	} while (token->type == commasym);

	if (token->type != semicolonsym)
		error(missing_semicolon);

	token = getNextToken();

	return numVars;
}

// PREEMPTIVE / WILL NOT WORK PROPERLY :D

// Runs functions required by the syntactic class "procdeclaration"
void procDeclaration(int level)
{
	do
	{
		token = getNextToken();

		if (token->value != identsym)
			error(missing_ident);
		oldSymbolPointer = symbolPointer;
		insertSymbol(PROC_KIND, token->name, 0, level, codeIdx);
		token = getNextToken();
		if (token->type != semicolonsym)
			error(missing_semicolon);

		block(level + 1);

		if (token->type != semicolonsym)
			error(missing_semicolon);
	} while (token->type == procsym);
}


// Runs functions required by the syntactic class "statement"
void statement(int level)
{
	int symIdx;
	int jpcIdx;

	if (token->type == identsym) // IDENTIFIER
	{
		// Check for symbol
		symIdx = checkSymbolTable(token->name);
		if (symIdx == NO_SYMBOL)
			error(undeclared_symbol);
		if (symbolTable[symIdx].kind != VAR_KIND)
			error(not_var);

		// Check for becomes
		token = getNextToken();
		if (token->type != becomessym)
			error(missing_becomes);

		token = getNextToken();
		expression(level);
		genCode(4, "STO", (level - symbolTable[symIdx].level), symbolTable[symIdx].addr);
		return;
	}
	// Procedure handling for future assignment.
	if (token->type == callsym)
	{
		token = getNextToken();

		if (token->type != identsym)
			error(missing_ident);
		
		symIdx = checkSymbolTable(token->name);
		if (index == NO_SYMBOL)
			error(undeclared_symbol);
		if (symbolTable[symIdx].kind == PROC_KIND)
		{
			genCode(5, "CAL", (level - symbolTable[symIdx].level), symbolTable[symIdx].addr);
		}
		else
			error(bad_call); // add this to error list                <--- HEY FUCKWIT!

		token = getNextToken();
	}
	if (token->type == beginsym) // BEGIN
	{
		// Parse statements
		do
		{
			token = getNextToken();
			statement(level);
		} while (token->type == semicolonsym);

		if (token->type != endsym)
			error(missing_end);

		token = getNextToken();
		return;
	}
	if (token->type == ifsym) // IF
	{
		token = getNextToken();
		condition(level);

		// Save current code index 
		jpcIdx = codeIdx;
		genCode(8, "JPC", level, 0);

		if (token->type != thensym)
			error(missing_then);
		token = getNextToken();
		statement(level);

		instructionList[jpcIdx].m = codeIdx;

		return;
	}
	if (token->type == whilesym) // WHILE
	{
		token = getNextToken();
		// loop index equals current code index
		loopIdx = codeIdx; 
		condition(level);

		if (token->type != dosym)
			error(missing_do);

		token = getNextToken();
		jpcIdx = codeIdx;	
		genCode(8, "JPC", level, 0);
		statement(level);
		genCode(7, "JMP", level, loopIdx); // Return to top of the loop
		instructionList[jpcIdx].m = codeIdx;
		return;
	}
	if (token->type == readsym) // READ
	{
		token = getNextToken();
		if (token->type != identsym)
			error(missing_ident);

		symIdx = checkSymbolTable(token->name);
		if (symIdx == NO_SYMBOL)
			error(undeclared_symbol);

		if (symbolTable[symIdx].kind != VAR_KIND) // Not a variable
			error(not_var);

		token = getNextToken();
		genCode(9, "SYS", level, 2); // READ
		genCode(4, "STO", (level - symbolTable[symIdx].level), symbolTable[symIdx].addr);
		return;
	}
	if (token->type == writesym) // WRITE
	{
		token = getNextToken();
		expression(level);
		genCode(9, "SYS", level, 1); // WRITE
		return;
	}
}

// Runs functions required by the syntactic class "condition"
void condition(int level)
{
	if (token->type == oddsym) // ODD
	{
		token = getNextToken();
		expression(level);
		genCode(2, "ODD", level, 6);
	}
	else
	{
		expression(level);
		if (token->type == eqlsym) // EQUALS
		{
			token = getNextToken();
			expression(level);
			genCode(2, "EQL", level, 8);
		}
		else if (token->type == neqsym) // NOT EQUALS
		{
			token = getNextToken();
			expression(level);
			genCode(2, "NEQ", level, 9);
		}
		else if (token->type == lessym) // LESS THAN
		{
			token = getNextToken();
			expression(level);
			genCode(2, "LSS", level, 10);
		}
		else if (token->type == leqsym) // LESS THAN EQUALS
		{
			token = getNextToken();
			expression(level);
			genCode(2, "LEQ", level, 11);
		}
		else if (token->type == gtrsym) // GREATER THAN
		{
			token = getNextToken();
			expression(level);
			genCode(2, "GTR", level, 12);
		}
		else if (token->type == geqsym) // GREATER THAN EQUALS
		{
			token = getNextToken();
			expression(level);
			genCode(2, "GEQ", level, 13);
		}
		else
		{
			error(missing_comparator);
		}
	}
}

// Runs functions required by the syntactic class "expression"
void expression(int level)
{
	if (token->type == minussym)
	{
		token = getNextToken();
		term(level);
		genCode(2, "NEG", level, 1);
		while (token->type == plussym || token->type == minussym)
		{
			if (token->type == plussym)
			{
				token = getNextToken();
				term(level);
				genCode(2, "ADD", level, 2);
			}
			else
			{
				token = getNextToken();
				term(level);
				genCode(2, "SUB", level, 3);
			}
		}
	}
	else
	{
		if (token->type == plussym)
			token = getNextToken();

		term(level);

		while (token->type == plussym || token->type == minussym)
		{
			if (token->type == plussym)
			{
				token = getNextToken();
				term(level);
				genCode(2, "ADD", level, 2);
			}
			else
			{
				token = getNextToken();
				term(level);
				genCode(2, "SUB", level, 3);
			}
		}
	}
}

// Runs functions required by the syntactic class "term"
void term(int level)
{
	factor(level);
	while (token->type == multsym || token->type == slashsym || token->type == modsym)
	{
		if (token->type == multsym)
		{
			token = getNextToken();
			factor(level);
			genCode(2, "MUL", level, 4);
		}
		else if (token->type == slashsym)
		{
			token = getNextToken();
			factor(level);
			genCode(2, "DIV", level, 5);
		}
		else
		{
			token = getNextToken();
			factor(level);
			genCode(2, "MOD", level, 7);
		}
	}
}

// Runs functions required by the syntactic class "factor"
void factor(int level)
{
	int insertValue;

	if (token->type == identsym) // IDENTIFIER
	{
		symIdx = checkSymbolTable(token->name);
		if (symIdx == NO_SYMBOL)
			error(undeclared_symbol);

		if (symbolTable[symIdx].kind == CONST_KIND)
			genCode(1, "LIT", level, symbolTable[symIdx].val);
		else if (symbolTable[symIdx].kind == VAR_KIND)
			genCode(3, "LOD", (level - symbolTable[symIdx].level), symbolTable[symIdx].addr);
		token = getNextToken();
	}
	else if (token->type == numbersym) // NUMBER
	{
		insertValue = atoi(token->name);
		genCode(1, "LIT", level, insertValue);
		token = getNextToken();
	}
	else if (token->type == lparentsym) // PARENTHESIS
	{
		token = getNextToken();
		expression(level);

		if (token->type != rparentsym)
			error(missing_rparent);

		token = getNextToken();
	}
	else
		// This will haunt me forever. -Eric
		error(invalid_equation);
}

// Parses the token list and returns a list of instructions for the
// virtual machine to execute
instruction *parseTokenList(lexeme *list, int printFlag)
{
	int i;
	int level = 0;
	tokenList = list;
	instructionList = malloc(BUFFER_LENGTH * sizeof(instruction));
	program(level);

	if (printFlag)
	{
		printf("Generated Assembly: \n");
		printf("Line    OP    L    M\n");

		for (i = 0; i < codeIdx; i++)
			printf("%3d%7s%5d%5d\n", i, instructionList[i].op, instructionList[i].l,
			       instructionList[i].m);
	}

	return instructionList;
}
