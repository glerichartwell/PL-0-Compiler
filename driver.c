// Glenn Hartwell
// Martin de Salterain
// COP3402, Spring 2021
// HW3 Compiler Driver

#include "compiler.h"

int main(int argc, char **argv)
{
	int lFlag = 0, aFlag = 0, vFlag = 0;
	FILE *ifp;
	char *inputFile;
	char c;
	int i;


	if (argc < 2)
		error(missing_file);

	for (i = 2; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'l')
				lFlag = 1;
			else if (argv[i][1] == 'a')
				aFlag = 1;
			else if (argv[i][1] == 'v')
				vFlag = 1;
			else
				error(invalid_directive);
		}
		else
			error(invalid_directive);
	}

	// Open file and check if opened
	ifp = fopen(argv[1], "r");
	if (ifp == NULL)
		error(missing_file);

	// Load PL/0 code into string for processing
	inputFile = malloc(BUFFER_LENGTH * sizeof(char));
	i = 0;
	while ((c = fgetc(ifp)) != EOF)
	{
		inputFile[i++] = c;
	}
	inputFile[i] = '\0';

	// Perform Lexical Analysis
	lexeme *tokenList = lexicalAnalysis(inputFile, lFlag);
	if (aFlag && lFlag)
	{
		printf("\n");
		printf("\n");
	}

	// Parse Token List
	instruction *instructionList = parseTokenList(tokenList, aFlag);
	if ((vFlag && aFlag) || (vFlag && lFlag))
	{
		printf("\n");
		printf("\n");
	}

	// Run Virtual Machine on generated code
	runVirtualMachine(instructionList, vFlag);

	return 0;
}
