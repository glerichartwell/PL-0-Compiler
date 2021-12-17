// Glenn Hartwell
// Martin de Salterain
// COP3402, Spring 2021
// HW1 Virtual Machine

#include "compiler.h"

// Returns the index of the stack indicating the base of the activation
// record found at some given lexicographical level.
int base(int stack[], int level, int bp)
{
	int base = bp;
	int counter = level;
	while (counter > 0)
	{
		base = stack[base];
		counter--;
	}
	return base;
}

void runVirtualMachine(instruction *instructionList, int printFlag)
{
	int *stack = calloc(MAX_STACK_HEIGHT, sizeof(int));
	int *ar_delimiter_stack = calloc(MAX_STACK_HEIGHT, sizeof(int));

	instruction *ir = NULL;

	int sp = -1;
	int bp = 0;
	int pc = 0;
	int halt = 1;
	int i, line_number, ar_end_index;
	int scan_buffer;

	char op[4]; // For storing instruction abbreviation

	ir = calloc(1, sizeof(instruction));

	// Print header for stack trace.
	if (printFlag)
	{
		printf("                  PC   BP   SP   stack\n");
		printf("Initial values:   %2d   %2d   %2d   \n", pc, bp, sp);
	}

	while (halt)
	{
		// Fetch
		ir->opcode = instructionList[pc].opcode;
		ir->l = instructionList[pc].l;
		ir->m = instructionList[pc].m;
		line_number = pc;
		pc++;

		// Execute
		switch(ir->opcode)
		{
			//LIT: Pushes a literal value "m" onto the stack.
			case 1:
				sp++;
				stack[sp] = ir->m;
				strcpy(op, "LIT");
				break;

			// OPR: Performs some operation based on the value of m.
			case 2:
				switch (ir->m)
				{
					case 0: // RTN: Restore the environment of the caller.
						ar_delimiter_stack[bp - 1] = 0; // Facilitates print format
						stack[bp - 1] = stack[sp]; // Retain value on top of the stack.
						sp = bp - 1;
						bp = stack[sp + 2]; // Base of restored AR = DL of current AR.
						pc = stack[sp + 3]; // Restore program counter to return address.
						strcpy(op, "RTN");
						break;

					case 1: // NEG
						stack[sp] = -1 * stack[sp];
						strcpy(op, "NEG");
						break;

					case 2: // ADD
						sp--;
						stack[sp] = stack[sp] + stack[sp + 1];
						strcpy(op, "ADD");
						break;

					case 3: // SUB
						sp--;
						stack[sp] = stack[sp] - stack[sp + 1];
						strcpy(op, "SUB");
						break;

					case 4: // MUL
						 sp--;
						stack[sp] = stack[sp] * stack[sp + 1];
						strcpy(op, "MUL");
						break;

					case 5: // DIV
						sp--;
						stack[sp] = stack[sp] / stack[sp + 1];
						strcpy(op, "DIV");
						break;

					case 6: // ODD
						stack[sp] = stack[sp] % 2;
						strcpy(op, "ODD");
						break;

					case 7: // MOD
						sp--;
						stack[sp] = stack[sp] % stack[sp + 1];
						strcpy(op, "MOD");
						break;

					case 8: // EQL (==)
						sp--;
						stack[sp] = (stack[sp] == stack[sp + 1]);
						strcpy(op, "EQL");
						break;

					case 9:	// NEQ (!=)
						sp--;
						stack[sp] = (stack[sp] != stack[sp + 1]);
						strcpy(op, "NEQ");
						break;

					case 10: // LSS (<)
						sp--;
						stack[sp] = (stack[sp] < stack[sp + 1]);
						strcpy(op, "LSS");
						break;

					case 11: // LEQ (<=)
						sp--;
						stack[sp] = (stack[sp] <= stack[sp + 1]);
						strcpy(op, "LEQ");
						break;

					case 12: // GTR (>)
						sp--;
						stack[sp] = (stack[sp] > stack[sp + 1]);
						strcpy(op, "GTR");
						break;

					case 13: // GEQ (>=)
						sp--;
						stack[sp] = (stack[sp] >= stack[sp + 1]);
						strcpy(op, "GEQ");
						break;
				}
				break;

			// LOD: Pushes an element at "l" lexicographical levels
			// down plus some offset "m" onto the stack .
			case 3:
				sp++;
				stack[sp] = stack[base(stack, ir->l, bp) + ir->m];
				strcpy(op, "LOD");
				break;

			// STO: Stores the element from the top of the stack at some index "l"
			// lexicographical levels down plus an offset "m".
			case 4:
				stack[base(stack, ir->l, bp) + ir->m] = stack[sp];
				sp--;
				strcpy(op, "STO");
				break;

			// CAL: Calls a fuction whose instruction is indexed in the instructionList by "m."
			case 5:
				ar_end_index = sp;
				stack[sp + 1] = base(stack, ir->l, bp);           // static link (SL)
				stack[sp + 2] = bp;                               // dynamic link (DL)
				stack[sp + 3] = pc;                               // return address (RA)
				stack[sp + 4] = stack[sp];                        // parameter (P)
				bp = sp + 1;                                      // new base pointer (BP)

				// Point program counter to instruction of the callee.
				pc = ir->m;
				strcpy(op, "CAL");
				break;

			// INC: Used to increment the stack pointer by some offset "m."
			case 6:
				sp = sp + ir->m;
				strcpy(op, "INC");
				break;

			// JMP: Sets the next instruction to one at index "m."
			case 7:
				pc = ir->m;
				strcpy(op, "JMP");
				break;

			// JPC: Executes a conditional jump based on the value at the top of the stack.
			case 8:
				if (stack[sp] == 0)
					pc = ir->m;
				sp--;

				strcpy(op, "JPC");
				break;

			// SYS: Initiates a system call based on the value of m.
			case 9:
				strcpy(op, "SYS");
				switch(ir->m)
				{
					case 1: // Print top of stack value // write in PL/0
						printf("Top of Stack Value: ");
						printf("%d\n", stack[sp]);
						sp--;
						break;

					case 2: // Receive input from user. // read in PL/0
						sp++;

						printf("Please Enter an Integer: ");
						scanf("%d", &stack[sp]);
						printf("\n");
						break;

					case 3: // Halt program
						halt = 0;
						break;
				}
			break;

			default:
				error(bad_instruction);
				break;
		}

		// Print current state.
		if (printFlag)
		{
			printf("%2d %s  %2d %2d     %2d   %2d   %2d", line_number, op, ir->l, ir->m, pc, bp, sp);
			printf("  ");
			for(i = 0; i <= sp; i++)
			{
				printf(" %d", stack[i]);
				if (ar_delimiter_stack[i] == 1)
					printf(" |");
			}
			printf("\n");
		}
		// Facilitates print format
		if (ir->opcode == 5)
			ar_delimiter_stack[ar_end_index] = 1;
	}
	return;
}
