// Glenn Hartwell
// Martin de Salterain
// COP3402, Spring 2021
// HW2 Lexical Analyzer

#include "compiler.h"

/******************** IMPORTANT ********************\
|                                                   |
|	During the execution of this lexical analyzer   |
|	if we reach a halt condition because of the new |
|	character we grabbed, we save it to take back   |
|	to the top of the loop. If we reach a halt      |
|	condition in any other case we grab the next    |
|	character to take back to the top.              |
|                                                   |
\***************************************************/ 

// Hashtable container, in this implementation <size> is never used
// however it is included because maintaining the hashtable size is standard
typedef struct Hashtable
{
	lexeme **array;
	int size;
	int capacity;
} Hashtable;

// Allocates all memory necessary for a hashtable of length <capacity>
Hashtable *createHashtable(int capacity)
{	
	int i;
	Hashtable *table = calloc(1, sizeof(Hashtable));
	table->array = calloc(capacity, sizeof(lexeme*));
	for (i = 0; i < capacity; i++)
	{
		table->array[i] = calloc(1, sizeof(lexeme));
	}
	table->capacity = capacity;
	table->size = 0;
	
	return table;
}

// Frees all memory associated with the hashtable
Hashtable *destroyHashtable(Hashtable *table)
{
	int i;
	for (i = 0; i < table->capacity; i++)
	{
		free(table->array[i]);
	}
	free(table->array);
	free(table);
	return NULL;
}

// A hash function for hashing strings
int hash(char *string)
{
	int hash = 7, len = strlen(string);
	int i;
	const int prime = 31;
	for (i = 0; i < len; i++)
	{
		hash = hash + prime * string[i];
	}
	return hash;
}

// Used to insert reserved words and symbols from an array into the hashtable
void hashInsert(Hashtable *table, char **symbols)
{
	int i, j = 0;
	int hash_index, hval;
	for (i = 0; i < NUM_SYMBOLS; i++)
	{
		// REMOVE IF YOU NEED TO ADD THESE GUYS BACK IN LATER
		if (strcmp(symbols[i], "call") == 0 ||
			strcmp(symbols[i], "procedure") == 0 ||
			strcmp(symbols[i], "else") == 0 ||
			strcmp(symbols[i], "return") == 0)
			continue;
					
		hash_index = hval = hash(symbols[i]) % table->capacity;

		// if type is 0 nothing has been hashed there
		while(table->array[hash_index]->type != 0) 
		{
			j++;
			hash_index = (hval + j * j) % table->capacity;
		}
		j = 0;

		strcpy(table->array[hash_index]->name, symbols[i]);
		if (i == NUM_SYMBOLS - 1)
			table->array[hash_index]->type = 1;
		else
			table->array[hash_index]->type = i + 4;
		table->size++;
	}
}

// Used to check if a string is a reserved word (is in the hashtable)
// If it is in the hashtable return the token type, 
// otherwise return the identifier token type
int hashGetTokenType(Hashtable *table, char *string)
{
	int i = 0;
	int hash_index, hval;
	hash_index = hval = hash(string) % table->capacity;
	while (table->array[hash_index]->type != 0)
	{
		if (strcmp(table->array[hash_index]->name, string) == 0)
			return table->array[hash_index]->type;
		i++;
		hash_index = (hval + i * i) % table->capacity;
	}
	return IDENT_TOKEN;
}

lexeme* lexicalAnalysis(char *inputFile, int printFlag)
{
	lexeme *token_list;
	char **string_token_list;
	char scan_buffer[BUFFER_LENGTH];
	char *reserved_symbols[NUM_SYMBOLS] = {"+","-","*","/","odd","=","<>","<","<=",
											">",">=","(",")",",",";",".",":=","begin",
											"end","if","then","while","do","call","const",
											"var","procedure","write","read","else","return","%"};

	// Hash Table Variables
	// capacity is prime and large enough that there are a low number of collisions
	int table_capacity = 127; 
	Hashtable *hashed_symbols;
	
	// Variables to store scanned characters
	char current_char, saved_char;

	// Stores token information to be printed
	lexeme current_token;

	// Flag to determine if there was an identifier errror while scanning digits
	int ident_error = 0;

	// Used for skipping over comments in loop
	int is_comment = 0;

	// Incrementing variables
	int i, j;
	int input_index = 0;
	int tp = 0;
	int stp = 0;

	// Trust me we need this later
	int inputLength = strlen(inputFile);

	// New Token list for passing to parseTokenList()
	token_list = calloc(BUFFER_LENGTH, sizeof(lexeme));


	// Token list for easy printing
	string_token_list = calloc(BUFFER_LENGTH, sizeof(char *));
	for (i = 0; i < BUFFER_LENGTH; i++)
		string_token_list[i] = calloc(MAX_IDENTIFIER_LENGTH + 2, sizeof(char));
	
	// Initialize Hashtable
	hashed_symbols = createHashtable(table_capacity);
	hashInsert(hashed_symbols, reserved_symbols);



/*
	// PRINT INPUT FILE FOR DEBUGGING

	for (i = 0; i < inputLength; i++)
	{
		printf("%c", inputFile[i]);
	}
	printf("\n");
*/



	// Print table header
	if (printFlag)
	{
		printf("Lexeme Table:\n");
		printf("       lexeme         token type\n");
	}

	// Get first character in file then start loop
	saved_char = inputFile[input_index++];
	while(saved_char != '\0')
	{
		// Catches saved characters
		current_char = saved_char;

		if (isalpha(current_char))
		{
			// If character is letter
			i = 0;
			while(isalpha(current_char) || isdigit(current_char))
			{
				// Keep adding letters
				if (i >= BUFFER_LENGTH)
				{
					// Exit loop if we exceed buffer length
					break;
				}
				scan_buffer[i] = current_char;
				current_char = inputFile[input_index++];
				i++;
			}
			if (i >= MAX_IDENTIFIER_LENGTH) 
			{
				// Discard token
				error(ident_too_long);
				saved_char = current_char;																// YOU'RE RIGHT HERE DUMMY
				continue;
			}
			else
			{
				// Store token
				scan_buffer[i] = '\0';
				saved_char = current_char;
				strcpy(current_token.name, scan_buffer);
				current_token.type = hashGetTokenType(hashed_symbols, scan_buffer);
				sprintf(string_token_list[stp], "%d", current_token.type);
				stp++;
				// Only store name in string token list if it's an identifier
				if (current_token.type == IDENT_TOKEN)
				{
					sprintf(string_token_list[stp], "%s", current_token.name);
					stp++;
				}
				// Store token for transport to ANOTHER FILE :D
				token_list[tp++] = current_token;
			}
		}
		else if (isdigit(current_char))
		{
			// If character is a number
			i = 0;
			while (isdigit(current_char))
			{
				// Keep adding numbers
				if (i >= BUFFER_LENGTH)
				{
					// Exit loop if we exceed buffer length
					break;
				}
				scan_buffer[i] = current_char;
				current_char = inputFile[input_index++];
				i++;
			}
			while (isalnum(current_char))
			{
				// Consume characters that cause digit/identifier errors
				current_char = inputFile[input_index++];
				ident_error = 1;
			}
			if (i > MAX_DIGITS) // Digit too long
			{
				// Discard token and save current character

				error(int_too_long);
				saved_char = current_char;
				continue;
			}
			if (ident_error) // Identifier Error
			{
				// Discard token and save current character
				error(digit_ident);
				ident_error = 0;
				saved_char = current_char;
				continue;
			}
			else
			{
				// Terminate string found in loop
				scan_buffer[i] = '\0'; 
				// Store Token and save current character
				saved_char = current_char;
				strcpy(current_token.name, scan_buffer);
				current_token.type = INT_TOKEN;
				sprintf(string_token_list[stp], "%d", current_token.type);
				stp++;
				sprintf(string_token_list[stp], "%s", current_token.name);
				stp++;
				token_list[tp++] = current_token;
			}
		}
		else if (ispunct(current_char))
		{
			// If character is punctuation

			// Check for special symbols
			if (current_char == '<')
			{
				// Save < and get next char
				saved_char = current_char;
				current_char = inputFile[input_index++];
				if (current_char == '=')
				{
					// Token is: <=
					scan_buffer[0] = saved_char;
					scan_buffer[1] = current_char;
					scan_buffer[2] = '\0';
					// Store token and get next character
					strcpy(current_token.name, scan_buffer);
					current_token.type = hashGetTokenType(hashed_symbols, scan_buffer);
					sprintf(string_token_list[stp], "%d", current_token.type);
					stp++;
					token_list[tp++] = current_token;
					saved_char = inputFile[input_index++];
				}
				else if (current_char == '>')
				{
					// Token is: <>
					scan_buffer[0] = saved_char;
					scan_buffer[1] = current_char;
					scan_buffer[2] = '\0';
					// Store Token and get next character
					strcpy(current_token.name, scan_buffer);
					current_token.type = hashGetTokenType(hashed_symbols, scan_buffer);
					sprintf(string_token_list[stp], "%d", current_token.type);
					stp++;
					token_list[tp++] = current_token;
					saved_char = inputFile[input_index++];
				}
				else
				{
					// Some other character

					// Token is simply <
					scan_buffer[0] = saved_char;
					scan_buffer[1] = '\0';
					// Store token and save current character
					saved_char = current_char;
					strcpy(current_token.name, scan_buffer);
					current_token.type = hashGetTokenType(hashed_symbols, scan_buffer);
					sprintf(string_token_list[stp], "%d", current_token.type);
					stp++;
					token_list[tp++] = current_token;
				}
			}
			else if (current_char == '>')
			{
				// Check next char
				saved_char = current_char;
				current_char = inputFile[input_index++];
				if (current_char == '=')
				{
					// Token is: >=
					scan_buffer[0] = saved_char;
					scan_buffer[1] = current_char;
					scan_buffer[2] = '\0';
					// Store token and get next character
					strcpy(current_token.name, scan_buffer);
					current_token.type = hashGetTokenType(hashed_symbols, scan_buffer);
					sprintf(string_token_list[stp], "%d", current_token.type);
					stp++;
					token_list[tp++] = current_token;
					saved_char = inputFile[input_index++];
				}
				else
				{
					// Some other character
					scan_buffer[0] = saved_char;
					scan_buffer[1] = '\0';
					// Store token and save current character
					saved_char = current_char;
					strcpy(current_token.name, scan_buffer);
					current_token.type = hashGetTokenType(hashed_symbols, scan_buffer);
					sprintf(string_token_list[stp], "%d", current_token.type);
					stp++;
					token_list[tp++] = current_token;
				}
			}
			else if (current_char == ':')
			{
				// Check next char
				saved_char = current_char;
				current_char = inputFile[input_index++];
				if (current_char == '=')
				{
					scan_buffer[0] = saved_char;
					scan_buffer[1] = current_char;
					scan_buffer[2] = '\0';
					// Store token and get next character
					strcpy(current_token.name, scan_buffer);
					current_token.type = hashGetTokenType(hashed_symbols, scan_buffer);
					sprintf(string_token_list[stp], "%d", current_token.type);
					stp++;
					token_list[tp++] = current_token;
					saved_char = inputFile[input_index++];
				}
				else
				{
					// Invalid symbol
					error(invalid_symbol);
					saved_char = current_char;
					continue;
				}
			}
			else if (current_char == '/')
			{
				// Save current character and grab next character
				saved_char = current_char;
				current_char = inputFile[input_index++];
				if (current_char == '*')
				{
					// Loop consumes characters until end comment sequence is found
					is_comment = 1;
					while (is_comment)
					{
						if (input_index > inputLength)
							error(hanging_comment);
						current_char = inputFile[input_index++];
						if (current_char == '*')
						{
							current_char = inputFile[input_index++];
							if (current_char == '/')
								is_comment = 0;
						}	
					}
					// Get next character after comment for top of loop
					saved_char = inputFile[input_index++];
					continue;
				}
				else
				{
					// Division symbol
					scan_buffer[0] = saved_char;
					scan_buffer[1] = '\0';
					// Store token and get next character
					strcpy(current_token.name, scan_buffer);
					current_token.type = hashGetTokenType(hashed_symbols, scan_buffer);
					sprintf(string_token_list[stp], "%d", current_token.type);
					stp++;
					token_list[tp++] = current_token;
					saved_char = current_char;
				}
			}
			else
			{
				// All other symbols
				scan_buffer[0] = current_char;
				scan_buffer[1] = '\0';
				// Store token and get next character
				strcpy(current_token.name, scan_buffer);
				current_token.type = hashGetTokenType(hashed_symbols, scan_buffer);
				// If the token type is the identifier token type but the
				// logic knows its punctuation then the symbol is invalid
				if (current_token.type == IDENT_TOKEN)
				{
					error(invalid_symbol);
					saved_char = inputFile[input_index++];
					continue;
				}
				sprintf(string_token_list[stp], "%d", current_token.type);
				stp++;
				token_list[tp++] = current_token;
				current_char = inputFile[input_index++];
				saved_char = current_char;
			}
		}
		else if (iscntrl(current_char) || isspace(current_char))
		{
			// Whitespace

			// If current character is whitespace, get the next character and save
			saved_char = inputFile[input_index++];
			continue;
		}

		// Print token for lexeme table
		if(printFlag && current_token.type != 0)
			printf("%13s %10d\n", current_token.name, current_token.type);
	}

	// Print Token List
	if (printFlag)
	{
		printf("\nToken List:\n");
		for (i = 0; string_token_list[i][0] != '\0'; i++)
		{
			printf("%s%c", string_token_list[i], (string_token_list[i+1][0] == '\0') ? '\n' : ' ');
		}
	}

	// Free all allocated memory 
	hashed_symbols = destroyHashtable(hashed_symbols);
	
	for (i = 0; i < BUFFER_LENGTH; i++)
		free(string_token_list[i]);
	free(string_token_list);

	return token_list;
}
