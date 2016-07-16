#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// The "basic" and "programmer" commands
enum Command {
	CONST,
	PUSH,
	POP,
	PRINTNUM,
	ADD,
	SUB,
	MPY,
	DIV,
	MOD,
	LABEL,
	BRANCHn,
	BRANCHnz,
	BRANCHnzp,
	BRANCHz,
	BRANCHp,
	BRANCHzp,
	BRANCHnp,
	JSR,
	JMPR, 
	NIL
};

// Create boolean values to flag for errors
enum BOOLEAN {
	false,
	true
};

// Initialize error flag to false
enum BOOLEAN error = false;

// Registers R0 through R7
int R[8];

// Structure to create a doubly-linked stack
typedef struct Stack_Vals {
  int value;
  struct Stack_Vals* next;
  struct Stack_Vals* prev;
} StackVal;

// Head of the stack
StackVal* stack = 0;
int stackSize = 0;

//A stack of instructions
typedef struct Instr_Vals {
	enum Command cmd;
	int reg;
	char* label;
	int constant;
	int pc;
	struct Instr_Vals* next;
	struct Instr_Vals* prev;
} InstrVal;

InstrVal * instructions = 0;

//An array of labels
char** labelIDs;

//An array of lines from the script
char** script_line;


/***************STACK OPERATIONS*****************/

//Push a value onto the top of the stack
void push(int value) {
  StackVal* nextNode = malloc(sizeof(StackVal));
  StackVal* prevNode = malloc(sizeof(StackVal));
  if (!nextNode || !prevNode) {
    printf("Error in creating new nodes for push!\n");
    error = true;
    return;
  }
  //Setup the new pointers and values
  nextNode->value = value;
  nextNode->next = stack;
  nextNode->prev = NULL;
  prevNode = stack;
  nextNode->prev = prevNode; 
  stack = nextNode;
  ++stackSize;//Increase stack size
}

//Return the value at the top of the stack
int pop() {
  int popped = 0;
  if (!stack) {
  	error = true;
    printf("Cannot pop from an empty stack\n");
    return 0;
  }
  //Retrieve the value and set the top of the stack to the next node
  popped = stack->value;
  StackVal* nextNode = stack->next;
  free(stack);
  stack = nextNode;
  stackSize = stackSize - 1;
  return popped;
}

//Determined dynamically in main()
int instruction_count;

// Return the pc value associated with a given label
int getPC(char* label) {
	for (int i = 0; i < instruction_count; ++i) {
		if (instructions[i].cmd == LABEL) {
			if (strcmp(labelIDs[i], label) == 0) {
				return instructions[i].pc;
			}
		}
	}
	printf("Error: label %s not found in stack\n", label);
	return instruction_count;
}

/***************************************************
Main reads in the input file, parses the script,
and executes the instructions, outputting the results
to a file "infilename.extension.out"
****************************************************/

int main(int argc, char * argv[]) {
	FILE * in;
	FILE * out;
	//Create the out filename: "[in-name].out"
	char filename[80];
	char * ext = ".out";
	char * empty = "";
	strcpy (filename, argv[1]);
	strcat (filename, ext);
	
	//Check for the proper number of arguments
	if (argc != 2) {
		error = true;
		printf("Usage: ./pcalc [script]; must have exactly 1 input script\n");
	}

	//Open the file to read in the commands
	in = fopen(argv[1], "r");
	if (!in) {
		printf("Failed to open %s\n", argv[1]);
		error = true;
	}
	//Create the file to write out to
	out = fopen(filename, "w");
	if (!out) {
		printf("Failed to create %s\n", filename);
		error = true;
	}


	int value; //Value for CONST commands
	char line [256]; //Space to store a line from the input script
	char command[9]; //The largest command is of length 9
	char regName[2]; //The register name
	int pc = 0; //Program counter

	//Count the number of instructions to allocate memory
	int n = 0;
	while (fgets(line, sizeof(line), in) != NULL) {
		++n;
	}
	instruction_count = n;
	instructions = malloc(sizeof(InstrVal) * n);

	labelIDs = malloc(n * sizeof(char*));
	script_line = malloc(n * sizeof(char*));
	for (int i = 0; i < n; ++i) {
		labelIDs[i] = malloc((200) * sizeof(char));
		script_line[i] = malloc(256 * sizeof(char));
	}

	n = 0;
	rewind(in);

	// Read in the script
	while (fgets(script_line[n], sizeof(line), in) != NULL) {
		sscanf(script_line[n], "%s", command);
		if (strcmp(command, "CONST") == 0) {
			sscanf(script_line[n], "%s %s %i", command, regName, &value);
			int reg = regName[1] - '0';
			instructions[n].cmd = CONST;
			instructions[n].reg = reg;
			instructions[n].constant = value;
			instructions[n].pc = n;
		} else if (strcmp(command, "PUSH") == 0) {
			sscanf(script_line[n], "%s %s", command, regName);
			int reg = regName[1] - '0';
			instructions[n].cmd = PUSH;
			instructions[n].reg = reg;
			instructions[n].pc = n;
		} else if (strcmp(command, "POP") == 0) {
			sscanf(script_line[n], "%s %s", command, regName);
			int reg = regName[1] - '0';
			instructions[n].cmd = POP;
			instructions[n].reg = reg;
			instructions[n].pc = n;
		} else if (strcmp(command, "PRINTNUM") == 0) {
			instructions[n].cmd = PRINTNUM;
			instructions[n].pc = n;
		} else if (strcmp(command, "ADD") == 0) {
			instructions[n].cmd = ADD;
			instructions[n].pc = n;
		} else if (strcmp(command, "SUB") == 0) {
			instructions[n].cmd = SUB;
			instructions[n].pc = n;
		} else if (strcmp(command, "MPY") == 0) {
			instructions[n].cmd = MPY;
			instructions[n].pc = n;
		} else if (strcmp(command, "DIV") == 0) {
			instructions[n].cmd = DIV;
			instructions[n].pc = n;
		} else if (strcmp(command, "MOD") == 0) {
			instructions[n].cmd = MOD;
			instructions[n].pc = n;
		} else if (strcmp(command, "LABEL") == 0) {
			instructions[n].cmd = LABEL;
			sscanf(script_line[n], "%s %s", command, labelIDs[n]);
			instructions[n].label = labelIDs[n];
			instructions[n].pc = n;
		} else if (strcmp(command, "BRANCHnzp") == 0) {
			sscanf(script_line[n], "%*s %*s %s", labelIDs[n]);
			instructions[n].cmd = BRANCHnzp;
			int reg = regName[1] - '0';
			instructions[n].reg = reg;
			instructions[n].label = labelIDs[n];
			instructions[n].pc = n;
		} else if (strcmp(command, "BRANCHp") == 0) {
			sscanf(script_line[n], "%s %s %s", command, regName, labelIDs[n]);
			instructions[n].cmd = BRANCHp;
			int reg = regName[1] - '0';
			instructions[n].reg = reg;
			instructions[n].label = labelIDs[n];
			instructions[n].pc = n;
		} else if (strcmp(command, "BRANCHn") == 0) {
			sscanf(script_line[n], "%s %s %s", command, regName, labelIDs[n]);
			instructions[n].cmd = BRANCHn;
			int reg = regName[1] - '0';
			instructions[n].reg = reg;
			instructions[n].label = labelIDs[n];
			instructions[n].pc = n;
		} else if (strcmp(command, "BRANCHz") == 0) {
			sscanf(script_line[n], "%s %s %s", command, regName, labelIDs[n]);
			instructions[n].cmd = BRANCHz;
			int reg = regName[1] - '0';
			instructions[n].reg = reg;
			instructions[n].label = labelIDs[n];
			instructions[n].pc = n;
		} else if (strcmp(command, "BRANCHnz") == 0) {
			sscanf(script_line[n], "%s %s %s", command, regName, labelIDs[n]);
			instructions[n].cmd = BRANCHnz;
			int reg = regName[1] - '0';
			instructions[n].reg = reg;
			instructions[n].label = labelIDs[n];
			instructions[n].pc = n;
		} else if (strcmp(command, "BRANCHnp") == 0) {
			sscanf(script_line[n], "%s %s %s", command, regName, labelIDs[n]);
			instructions[n].cmd = BRANCHnp;
			int reg = regName[1] - '0';
			instructions[n].reg = reg;
			instructions[n].label = labelIDs[n];
			instructions[n].pc = n;
		} else if (strcmp(command, "BRANCHzp") == 0) {
			sscanf(script_line[n], "%s %s %s", command, regName, labelIDs[n]);
			instructions[n].cmd = BRANCHzp;
			int reg = regName[1] - '0';
			instructions[n].reg = reg;
			instructions[n].label = labelIDs[n];
			instructions[n].pc = n;
		} else if (strcmp(command, "JSR") == 0) {
			sscanf(script_line[n], "%s %s", command, labelIDs[n]);
			instructions[n].cmd = JSR;
			instructions[n].label = labelIDs[n];
			instructions[n].pc = n;
		} else if (strcmp(command, "JMPR") == 0) {
			sscanf(script_line[n], "%s %s", command, regName);
			int reg = regName[1] - '0';
			instructions[n].cmd = JMPR;
			instructions[n].reg = reg;
			instructions[n].pc = n;
		} else {
			instructions[n].cmd = NIL;
			instructions[n].pc = n;
			++n;
			continue;
		}
		++n;
	}

	//Execute the script
	while (pc < n) {
		switch(instructions[pc].cmd) {
			case CONST:
				//Load the constant into the desired register
				R[instructions[pc].reg] = instructions[pc].constant;
				break;
			case PUSH:
				push(R[instructions[pc].reg]);
				break;
			case POP:
				R[instructions[pc].reg] = pop();
				break;
			case PRINTNUM:
				if (!stack) {
					printf("Cannot print from an empty stack\n");
				} else {
					printf("%d\n", stack->value);
					fprintf(out, "%d\n", stack->value);
				}
				break;
			case ADD:
				if (stackSize < 2) {
					error = true;
					printf("Cannot add with fewer than 2 items on the stack\n");
				} else {
					int x = pop();
					int y = pop();
					push(x + y);
				}
				break;
			case SUB:
				if (stackSize < 2) {
					error = true;
					printf("Cannot sub with fewer than 2 items on the stack\n");
				} else {
					int x = pop();
					int y = pop();
					push(x - y);
				}
				break;
			case MPY:
				if (stackSize < 2) {
					error = true;
					printf("Cannot mpy with fewer than 2 items on the stack\n");
				} else {
					int x = pop();
					int y = pop();
					push(x * y);
				}
				break;
			case DIV:
				if (stackSize < 2) {
					error = true;
					printf("Cannot div with fewer than 2 items on the stack\n");
				} else {
					int x = pop();
					int y = pop();
					if (y == 0) {
						error = true;
						printf("Cannot divide by zero\n");
					} else {
						push(x / y);
					}
				}
				break;
			case MOD:
				if (stackSize < 2) {
					error = true;
					printf("Cannot mod with fewer than 2 items on the stack\n");
				} else {
					int x = pop();
					int y = pop();
					push(x % y);
				}
				break;
			case LABEL:
				break;
			case BRANCHn:
				pc = R[instructions[pc].reg] < 0 ? getPC(instructions[pc].label) - 1 : pc;
				break;
			case BRANCHnz:
				pc = R[instructions[pc].reg] <= 0 ? getPC(instructions[pc].label) - 1 : pc;
				break;
			case BRANCHnzp:
				pc = getPC(instructions[pc].label) - 1;
				break;
			case BRANCHz:
				pc = R[instructions[pc].reg] == 0 ? getPC(instructions[pc].label) - 1 : pc;
				break;
			case BRANCHp:
				pc = R[instructions[pc].reg] > 0 ? getPC(instructions[pc].label) - 1 : pc;
				break;
			case BRANCHzp:
				pc = R[instructions[pc].reg] >= 0 ? getPC(instructions[pc].label) - 1 : pc;
				break;
			case BRANCHnp:
				pc = R[instructions[pc].reg] != 0 ? getPC(instructions[pc].label) - 1 : pc;
				break;
			case JSR:
				push(pc + 1);
				pc = getPC(instructions[pc].label);
				break;
			case JMPR:
			{
				int pc_val = R[instructions[pc].reg] - 1;
				if (pc_val < 0 || pc_val > instruction_count) {
					error = true;
					printf("JMPR to instruction out of bounds: %d\n", R[instructions[pc].reg]);
				} else {
					pc = pc_val;
				}
				break;
			}
			case NIL:
				break;
		}
		++pc;
	}

	//Close the files and free up memory that was used
	fclose(in);
	fclose(out);
	free(stack);
	for (int i = 0; i < instruction_count; ++i) {
		free(labelIDs[i]);
		free(script_line[i]);
	}
	free(instructions);
	free(labelIDs);
	free(script_line);
	if (error) {
		return 1;
	} else {
		return 0;
	}
}
