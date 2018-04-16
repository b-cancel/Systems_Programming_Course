/*
Programmer: Bryan Cancel
Last Updated: 3/20/18

NOTE: priority was not given to speed... priority was given to code readability and ease of use (for the programmer using this assembler)

Description:
Pass 1 will create:
-symbol table (label, address)
-intermediate file (opcode and operand DO NOT HAVE TO be translated into object code)
	FOR EACH LINE (REQUIRED)
	-copy of source line
	-value of location counter
	-values of mnemonics used (since they had to be looked up)
	-operand (since you had to get it)
	-error messages (used code and not the actual messages)
that will be used in pass 2

Deliverable:
1. well documented source listing
2. two listings of assembler language source files (one with error, one without errors)
3. a listing of the symbol table produced
4. Copies of both intermediate files (for the files in 2)
5. @toplevel/3334/phase2
with only source files in said folder

I am Assuming:
(1) Everything in "TODO list (maybe)" below is not a requirement
(2) (a) intermediate file (b) listing file (c) object file -> Dont Require a Specific File Extension (using .txt)
(3) our labels work in a global scope (we dont need to check for scoping rules)
(4) this "{label} operation {operand{,X}} {comment}") format must always be followed [so we cannot have a line with JUST a label]
(5) we ignore blank lines in code
(6) ONLY labels can be found in column 1 of a command (NOT operations)
(7) duplicate START directive WILL NOT change the starting address and set the location counter
(8) duplicate label in symbol table WILL NOT update what value that symbol maps to
(9) every number read in from the file including the LOCCTR is alwasy a positive number
(10) LOCCTR cannot exceed (FFFF) base 16 -A.K.A.-(65535) base 10
(11) we begin reading the file as code once we find a command with a (1) directive == start -AND- (2) a valid operand
	we need (1) because otherwise we would not have found the begining of our code
	we need (2) because we set our LOCCTR to START's operand... so if it isnt valid then we cant properly use LOCCTR
	AFTER we find this very specific instruction we start counting anything with a START directive as a duplicate and flag the duplicate error
(12) START must have a Label in front of it
(13) operations ONLY add to the LOCCTR when they should IF the instruction did not cause any errors
---
(14) the hex number passed by START... DOES NOT have to be even lengthed
(15) there is no max size for a HEX number found that is the operand of a mnemonic
(16) the operand of START should be a number in base 16 EVENTHOUGH it has the same arbitrary type "n" and RESB and RESW and they both have base 10 operands given the book [make things easier]
(17) we dont handle 'operand,x' format as an operand of an mnemonic in any special way
(18) program first label and program last label are set regardless of whether or not the label is valid (of course if it is not valid then an error will show up)
*/

//TODO list (maybe)
//1. to the symbol table add (scope info, type[of what?], length[of what?])
//2. to the intermediate file add (pointers to Opcode Table, and pointer to Symbol Table)
//3. convert the symbol table to something actually efficient (Ideally we use a dynamic Hash Table)
//4. code "itoa16" to actually convert an integer into HEX representation
//5. repair some small memory leaks (most of the TODO items in this program)

#pragma region Library Includes

#pragma once
#include "pass2.h"

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#pragma endregion

#pragma region Constants

#define min(a,b) (((a) < (b)) ? (a) : (b))

//explicit limitations
#define MAX_SYMBOLS 500
#define MAX_OPCODES 25
#define MAX_PROGRAM_SIZE  32767
#define MAX_SYMBOL_SIZE 7 //6 spots and null terminator
//implicit limitations
#define MAX_DIRECTIVE_SIZE 6 //5 spots and null terminator
#define MAX_LOCCTR_SIZE 65535 //(FF,FF) base 16
#define MAX_SIZE  16777215 //(FF,FF,FF) base 16

#pragma endregion

#pragma region Helper Function Prototypes

void freeMem(char **line);

//---SPECIFIC String Processing Prototypes (require repairs)
char* processFirst(char** l); //CHECK after repairing substrings
char* processRest(char** l);
int removeSpacesFront(char** l); //CHECK after repairing substrings
int removeSpacesBack(char** l); //works

//---GENERAL String Processing Prototypes
char* stringCopy(char* str); 
char* subString(char* src, int srcIndex, int strLength);

//---Integer To String Prototypes
char* reverse(char* str);
char* itoa10(int num);
char* itoa16(int num); //TODO... implement

//---Helper Prototypes
char* returnEmptyString();
void stringToLower(char** l);
char* strCat(char* startValue, char* addition);
char* strCatFreeFirst(char** fS, char* lastString);

//---Error Checking Prototypes
//for line
int isBlankLine(char* line);
int isEmpty(char* charArray);
//for label
int isLabel(char* line);
int isValidLabel(char* label);
//for mnemonic
int isDirective(char* mneumonic);
//for operand
int isNumber10(char* num);
int isNumber16(char* num);

//---Symbol Table Prototypes
int resetSYMTBL();
int addSYMTBL(char* key, int value);
//NOTE: no removal function needed (for now)
int setSYMTBL(char* key, int value);
int containsKeySYMTBL(char* key);
int getKeyIndexSYMTBL(char* key);
int containsValueSYMTBL(int value);
int getValueIndexSYMTBL(int value);
void printSymbolTable();

//---OpCode Table Prototypes
int containsOpCode(char* operand);
char* getOpCode(char* operand);
char* indexToOpName(int index);
char* indexToOpCode(int index);
void printOpCodeTable();

#pragma endregion

#pragma region Main Program

//---meat of program
int* processFullInstruction(
	char **_line, char **_label, char **_operation, char **_operand, char **_errors,
	int LOCCTR, int locctrAddition, int startFound, int endFound //4
);

//---Symbol Table Global Vars
typedef struct charToInt keyToValue;
struct charToInt {
	char* key;
	int value;
};
keyToValue symbolTbl[MAX_SYMBOLS]; //keys must be a continuous stream of characters
int emptyIndex;

void pass1(char* filename)
{
	//avoid a silly error
	if (isEmpty(filename))
		filename = returnEmptyString();

	//---Vars to Pass to Pass 2
	//symbol table (global)
	//opcode table (global)
	char* programFirstLabel = returnEmptyString();
	char* programLastLabel = returnEmptyString();
	int programLength = 0; //required for the begining of pass 2

						   //---Debugging Tools
	int printIntermediateFile = 0; //1 to print, 0 to not print
	int writeIntermediateFile = 1; //1 to write to file, 0 to not write to file

	printf("Source File: '%s'\n", filename);
	//place INTERMEDIATE in stream, make sure INTERMEDIATE file opens for writing properly
	char* interFileName = strCat(subString(filename, 0, strlen(filename) - 4), "Intermediate.txt");
	printf("Intermediate File: '%s'\n", interFileName);

	FILE *ourIntermediateFile = fopen(strCat("./", interFileName), "w"); //wipes out the file
	if (ourIntermediateFile != NULL)
	{
		//place SOURCE in stream, make sure SOURCE file opens for reading properly
		FILE *ourSourceFile = fopen(filename, "r");
		if (ourSourceFile != NULL)
		{
			resetSYMTBL();

			//create the variables that will be used to read in our file
			char *line = NULL; //NOTE: this does not need a size because getline handle all of that
			size_t len = 0;

			//{label} operation {operand{,X}} {comment}
			char *label;
			char *operation;
			char *operand;
			char *comment;

			//--------------------------------------------------BEFORE START--------------------------------------------------

			int startingAddress = 0;
			int LOCCTR = 0;
			int startFound = 0;

			//ignore lines until we find a command with a (1) directive == start -AND- (2) a valid operand
			while (startFound == 0 && getline(&line, &len, ourSourceFile) != -1)
			{
				char *lineCopy = stringCopy(line); //make a copy of the line (because the actual line should be processed below)
				stringToLower(&lineCopy); //make this line case IN-sensitive

				//we did not find a white space(potential label)
				if (isLabel(lineCopy) == 1)
				{
					label = processFirst(&lineCopy);
					if (isEmpty(label) != 1)
					{
						operation = processFirst(&lineCopy);
						if (isEmpty(operation) != 1)
						{
							if (strcmp(operation, "start") == 0)
							{
								//NOTE: this is required here simply so that we can map the name of the file properly to the LOCCTR when we process the label
								operand = processFirst(&lineCopy);

								if (isEmpty(operand) != 1)
								{
									if (isNumber16(operand) == 1) //is hex number
									{
										int num = strtol(operand, NULL, 16);
										if (num <= MAX_LOCCTR_SIZE)
										{
											startingAddress = num;
											LOCCTR = startingAddress; //init LOCCTR to starting address
											startFound = 1; //we have located our first START directive that has a VALID operand
										}
										else
										{
											//SPECIAL ERROR [8 lines] (no valid operand for start directive because the one read in is too large)
											if (printIntermediateFile == 1)
												printf("%s\n\n\n\n\n\nx040x\n\n", line);
											if (writeIntermediateFile == 1)
												fputs(strCat(line, "\n\n\n\n\n\nx040x\n\n"), ourIntermediateFile);

											//free(label); //TODO... this should work but it doesnt
										}
									}
									else //ELSE... we dont have a hex number
									{
										//SPECIAL ERROR [8 lines] (no valid hex operand for start directive)
										if (printIntermediateFile == 1)
											printf("%s\n\n\n\n\n\nx030x\n\n", line);
										if (writeIntermediateFile == 1)
											fputs(strCat(line, "\n\n\n\n\n\nx030x\n\n"), ourIntermediateFile);

										//free(label); //TODO... this should work but it doesnt
									}
								}
								else //we have a missing operand we cannot continue
								{
									//free(label); //TODO... this should work but it doesnt
								}
								freeMem(&operand); //TODO... this should work but it doesnt
							}
							else //we did not find the START directive
							{
								//free(label); //TODO... this should work but it doesnt
							}
						}
						else //we did not find an operation
						{
							operation = returnEmptyString();
							//free(label); //TODO... this should work but it doesnt
						}
						freeMem(&operation); //TODO... this should work but it doesnt
					}
					else //we did not find a label
					{
						//free(label); //TODO... this should work but it doesnt
						label = returnEmptyString();
					}
				}
				//ELSE... we have not found label in our line... we cannot find START

				//free(line); //TODO... this should work but it doesnt
			}

			freeMem(&programFirstLabel);
			programFirstLabel = stringCopy(label);

			//if we stoped reading the file because a START with a valid operand was found (we have some commands to read into our file)
			if (startFound == 1) {

				//--------------------------------------------------BETWEEN START and END--------------------------------------------------

				int endFound = 0;

				//NOTE: we use a do while because the line that is currently in the "buffer" is the first line (the one with the START directive)
				do
				{
					removeSpacesBack(&line); //remove new line character

					char *origLine = malloc(strlen(line) * sizeof(char)); //save original line (WITH case sensitivity) for writing it in its entirety into the intermediate file
					origLine = stringCopy(line);
					stringToLower(&line); //remove case sensitivity

					if (isBlankLine(line) != 1)
					{
						int locctrAddition = 0;

						if (line[0] != '.') //we found a instruction
						{
							//---Check For Operation

							char *tempLine = stringCopy(line);
							if (isLabel(tempLine) == 1) {
								label = processFirst(&tempLine); //we are guaranteed to find atleast something
								freeMem(&label); //TODO... this should work but it doesnt
							}
							else
								label = returnEmptyString();

							operation = processFirst(&tempLine);

							freeMem(&tempLine);

							if (isEmpty(operation) != 1) //we have an operation
							{
								freeMem(&operation);

								//--------------------PROCESS Full Instruction

								int *newVars = malloc(4 * sizeof(int));
								char* errors = returnEmptyString();

								//grab new vars after running function
								newVars = processFullInstruction(
									&line, &label, &operation, &operand, &errors,
									LOCCTR, locctrAddition, startFound, endFound
								);

								if (strcmp(operation, "end") == 0) {
									freeMem(&programLastLabel);
									programLastLabel = operand;
								}

								//set new vars after running function
								LOCCTR = newVars[0]; locctrAddition = newVars[1]; startFound = newVars[2]; endFound = newVars[3];
								free(newVars);

								//--------------------PRINT Full Instruction

								//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

								programLength = (LOCCTR - startingAddress);
								if (programLength > MAX_PROGRAM_SIZE)
									errors = strCatFreeFirst(&errors, "x900x"); //program is too long
								if (LOCCTR > MAX_LOCCTR_SIZE)
									errors = strCatFreeFirst(&errors, "x901x");

								comment = processRest(&line);
								errors = strCatFreeFirst(&errors, "\0");

								if (printIntermediateFile == 1)
								{
									printf("%s\n", origLine); //[1]
									printf("%x\n", (LOCCTR - locctrAddition)); //[2] 
									printf("%s\n", label); //[3]
									printf("%s\n", operation); //[4]
									printf("%s\n", operand); //[5] (operation code)
									printf("%s\n", comment); //[6]
									printf("%s\n", errors); //[7]
									printf("\n"); //[\n]
								}
								if (writeIntermediateFile == 1)
								{
									fputs(strCat(origLine, "\n"), ourIntermediateFile); //[1]
									fputs(strCat(itoa10(LOCCTR - locctrAddition), "\n"), ourIntermediateFile); //[2] (LOCCTR - locctrAddition)
									fputs(strCat(label, "\n"), ourIntermediateFile); //[3]
									fputs(strCat(operation, "\n"), ourIntermediateFile); //[4]
									fputs(strCat(operand, "\n"), ourIntermediateFile); //[5] (operation code)
									fputs(strCat(comment, "\n"), ourIntermediateFile); //[6]
									fputs(strCat(errors, "\n"), ourIntermediateFile); //[7]
									fputs("\n", ourIntermediateFile); //[\n]
								}

								//free(line);
								freeMem(&label);
								//free(operation); //TODO... this should work but it doesnt
								//free(operand); //TODO... this should work but it doesnt
								//free(comment); //TODO... this should work but it doesnt
								freeMem(&errors);
							}
							else //we are missing an operation
							{
								freeMem(&operation);

								//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

								char* errors = returnEmptyString();
								errors = strCatFreeFirst(&errors, "x140x");

								programLength = (LOCCTR - startingAddress);
								if (programLength > MAX_PROGRAM_SIZE)
									errors = strCatFreeFirst(&errors, "x900x"); //program is too long
								if (LOCCTR > MAX_LOCCTR_SIZE)
									errors = strCatFreeFirst(&errors, "x901x");

								errors = strCatFreeFirst(&errors, "\0"); //add a null terminator to errors

								if (printIntermediateFile == 1)
								{
									printf("%s\n", origLine); //[1]
									printf("%x\n", LOCCTR); //[2]
									printf("%s\n", label); //[3]
									printf("\n\n\n"); //[4] -> [6] (operation, operand, comment)
									printf("%s\n", errors); //[7]
									printf("\n"); //[\n]
								}
								if (writeIntermediateFile == 1)
								{
									fputs(strCat(origLine, "\n"), ourIntermediateFile); //[1]
									fputs(strCat(itoa10(LOCCTR), "\n"), ourIntermediateFile); //[2]
									fputs(strCat(label, "\n"), ourIntermediateFile); //[3]
									fputs("\n\n\n", ourIntermediateFile); //[4] -> [6] (operation, operand, comment)
									fputs(strCat(errors, "\n"), ourIntermediateFile); //[7]
									fputs("\n", ourIntermediateFile); //[\n]
								}

								freeMem(&label);
								freeMem(&errors);
							}
						}
						else //we found a comment
						{
							//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

							if (printIntermediateFile == 1)
								printf("%s\n\n\n\n\n\n\n\n", origLine);
							if (writeIntermediateFile == 1)
								fputs(strCat(origLine, "\n\n\n\n\n\n\n\n"), ourIntermediateFile);
						}
					}
					//ELSE... we ignore this blank line

					//free(line); //TODO... this should work but it doesnt
					freeMem(&origLine);

				} while (getline(&line, &len, ourSourceFile) != -1 && endFound == 0);

				//--------------------------------------------------AFTER END -or- EOF--------------------------------------------------

				printf("after end\n");

				//if we stoped reading the file because END was found
				if (endFound == 0)
				{
					//SPECIAL ERROR [8 lines] (no end directive)
					if (printIntermediateFile == 1)
						printf("\n\n\n\n\n\nx020x\n\n");
					if (writeIntermediateFile == 1)
						fputs("\n\n\n\n\n\nx020x\n\n", ourIntermediateFile);
				}
			}
			else
			{
				//SPECIAL ERROR [8 lines] (no START directive with a VALID operand)
				if (printIntermediateFile == 1)
					printf("\n\n\n\n\n\nx010x\n\n");
				if (writeIntermediateFile == 1)
					fputs("\n\n\n\n\n\nx010x\n\n", ourIntermediateFile);
			}

			fclose(ourSourceFile); //close our source file after reading it
		}
		else
		{
			//SPECIAL ERROR [8 lines] (source did not open)
			if (printIntermediateFile == 1)
				printf("\n\n\n\n\n\nx000x\n\n");
			if (writeIntermediateFile == 1)
				fputs("\n\n\n\n\n\nx000x\n\n", ourIntermediateFile);
		}

		//--------------------------------------------------SYMBOL TABLE TO INTERMEDIATE FILE--------------------------------------------------

		if (printIntermediateFile == 1)
		{
			printf("---Symbol Table (string -> int)\n");
			for (int i = 0; i < emptyIndex; i++)
				printf("'%s' maps to '%i'\n", symbolTbl[i].key, symbolTbl[i].value);
			printf("\n");
		}
		if (writeIntermediateFile == 1)
		{
			fputs("---Symbol Table (string -> int)\n", ourIntermediateFile);
			for (int i = 0; i < emptyIndex; i++) {
				char* symStr = strCat(symbolTbl[i].key, " maps to ");
				char* val = itoa10(symbolTbl[i].value);

				symStr = strCatFreeFirst(&symStr, val);
				symStr = strCatFreeFirst(&symStr, "\n");
				fputs(symStr, ourIntermediateFile);
			}
			fputs("\n", ourIntermediateFile);
		}

		fclose(ourIntermediateFile); //close our intermediate file after writing to it

		pass2(filename, interFileName, &programFirstLabel, &programLastLabel, programLength);
		freeMem(&interFileName);
	}
	else //INTERMEDIATE did not open properly
		printf("ERROR --- INTERMEDIATE file did not open properly\n"); //THE ONLY ERROR THAT CANNOT BE IN THE INTERMEDIATE FILE

	//NOTE: we cant start pass 2 unless at the very least the intermediate file opened properly in pass 1
}

//--------------------------------------------------THE MEAT--------------------------------------------------

int* processFullInstruction(
	char **_line, char **_label, char **_operation, char **_operand, char **_errors,
	int LOCCTR, int locctrAddition, int startFound, int endFound //4
)
{
	//Link up to our variables by reference
	char *line = *_line; char *label = *_label; char *operationName = *_operation; char *operand = *_operand; char *errors = *_errors;
	freeMem(&label); freeMem(&operationName); freeMem(&operand); //free the memory from the previous run (dont free line) | (errors is already empty)

	//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

	//-------------------------LABEL FIELD-------------------------

	if (isLabel(line) == 1) //we have a label
	{
		label = processFirst(&line);
		if (isEmpty(label) == 1) 
		{
			freeMem(&label);
			label = returnEmptyString();
		}
		else //we have some sort of label
		{
			int validResult = isValidLabel(label);
			int addResult;
			switch (validResult)
			{
			case 1: //LABEL is valid (add to symbol table)
				addResult = addSYMTBL(label, LOCCTR);
				switch (addResult)
				{
				case 1: break; //no errors
				case 0: errors = strCatFreeFirst(&errors, "x100x"); break; //duplicate label
				case -1: errors = strCatFreeFirst(&errors, "x110x"); break; //symbol tbl full
				default: break;
				}
				break;
			case 0: errors = strCatFreeFirst(&errors, "x120x"); break; //label starts with digit
			case -1: errors = strCatFreeFirst(&errors, "x130x"); break; //label is too long
			default: break;
			}
		}
	}
	else //the label field must equal something so we can print it
		label = returnEmptyString(); //NOTE: this means we have no label but we DO have an operation

	operationName = processFirst(&line);

	//NOTE: by now we processed the label IF there was one -AND- added the nessesarily ERRORS

	//-------------------------OPERATION FIELD-------------------------

	

	if (strcmp(getOpCode(operationName), "-1") != 0) //we have this mnemonic
	{
		//---------------MNEMONIC FOUND---------------

		char* operationCode = getOpCode(operationName);

		//NOTE: all mnemonic add to the location counter BUT... we only add to it if everything is valid...

		//for everything except rsub read in an operand
		if (strcmp(operationName, "rsub") != 0)
		{
			operand = processFirst(&line);

			if (isEmpty(operand) == 1)
			{
				errors = strCatFreeFirst(&errors, "x300x"); //missing operand
				freeMem(&operand);
				operand = returnEmptyString();
			}
			else
			{
				int lastCharIndex = strlen(operand) - 1;
				char *rawOperand;

				//---Extract Operand (ignore ,X)

				if (operand[lastCharIndex] == 'x' && operand[lastCharIndex - 1] == ',') //form 'operand,x'
				{
					rawOperand = subString(operand, 0, strlen(operand) - 2);
					if (isEmpty(rawOperand) == 1)
					{
						errors = strCatFreeFirst(&errors, "x300x"); //missing operand
						rawOperand = returnEmptyString();
					}
					//ELSE... we have the operand we require
				}
				else //form 'operand' 
					rawOperand = stringCopy(operand);

				//---Check to see what the "rawOperand" is

				if (isValidLabel(rawOperand) != 1)
				{
					if (isNumber16(rawOperand) == 1)
					{

						int secondHexDigitAtoF = (isxdigit(rawOperand[1]) != 0 && isdigit(rawOperand[1]) == 0) ? 1 : 0; //if we have a HEX number that starts with A -> F
						if (secondHexDigitAtoF && rawOperand[0] == '0') //and we have a leading 0
						{
							//we have a HEX number that uses a leading 0 to distinguish itself from a Label

							//shift everything to the left

							rawOperand = subString(rawOperand, 1, strlen(rawOperand) - 1);
						}
						//ELSE... our hex number could have just started with a digit 0->9

						//---Now see if this is the type of HEX number that we want

						if (strlen(rawOperand) % 2 != 0)
							errors = strCatFreeFirst(&errors, "x310x"); //hex number must be in bytes so you must have an even digit count
						else //we can easily handle this operand as a HEX number
							locctrAddition = 3;
					}
					else
						errors = strCatFreeFirst(&errors, "x320x"); //hex number required but not found
				}
				else //we can easily handle this operand as a label
					locctrAddition = 3;

				freeMem(&rawOperand);
			}
		}
		else //RSUB doesnt require an operand but the operand field must equal something so we can print it
		{
			locctrAddition = 3;
			operand = returnEmptyString();
		}
	
		*_operation = operationCode;
		freeMem(&operationName);
	}
	else //check if we have a directive
	{
		if (isDirective(operationName) == 1)
		{
			//---------------DIRECTIVE FOUND---------------

			//NOTE: all directives have operands
			operand = processFirst(&line);
			if (isEmpty(operand) == 1) //missing operand
			{
				errors = strCatFreeFirst(&errors, "x400x");
				freeMem(&operand);
				operand = returnEmptyString();
			}
			else //we have the operand we need
			{
				if (strcmp(operationName, "start") == 0)
				{
					//NOTE: because we checked that 'a' START directive was found with a VALID operand
					//and using that operand we set our LOCCTR like we should have
					//IF we are here then we know we have an EXTRA START directive
					//except the for the first start we find (which we run in here because we still need to process the validity of its label)
					//we know for a fact that its operand is valid

					if (startFound > 1)
						errors = strCatFreeFirst(&errors, "x200x"); //extra start directive 
					startFound++;
				}
				else if (strcmp(operationName, "end") == 0)
				{
					endFound = 1; //stop us from processing any more of this file
				}
				else if (strcmp(operationName, "byte") == 0) //Stores either character strings (C'...') or hexadecimal values (X'...')
				{
					if (operand[0] == 'c')
					{
						char *tempOperand = subString(operand, 2, strlen(operand) - 3); // the three values are C, ', and '

						if (strlen(tempOperand) <= 30) //max of 30 characters
							locctrAddition = strlen(tempOperand); //add enough space to store this string (one spot for each character)
						else
							errors = strCatFreeFirst(&errors, "x430x"); //max of 30 chars

						//free(tempOperand); //TODO... this should work... but it doesnt
					}
					else if (operand[0] == 'x')
					{
						char *tempOperand = subString(operand, 2, strlen(operand) - 3); // the three values are X, ', and '

						if (strlen(tempOperand) % 2 == 0) //must be even number of digits
						{
							//max of 32 hex digits... or 4 bytes... or 8 hex digits
							if (strlen(tempOperand) <= 8)
							{
								if (isNumber16(tempOperand) == 1)
									locctrAddition = (strlen(tempOperand) / 2); //add enough space to store this hex number (one spot for each byte)
								else
									errors = strCatFreeFirst(&errors, "x410x"); //must be a hex number 
							}
							else
								errors = strCatFreeFirst(&errors, "x440x");  //must be a max of 16 bytes 
						}
						else
							errors = strCatFreeFirst(&errors, "x450x"); //number must be byte so must have even number of digits 

						//free(tempOperand); //TODO... this should work... but it doesnt
					}
					else
						errors = strCatFreeFirst(&errors, "x460x"); //you can only pass a string or hex value as the operand to byte 
				}
				else if (strcmp(operationName, "word") == 0)
				{
					if (isNumber10(operand) == 1)
					{
						if ((strtol(operand, NULL, 10)) <= MAX_SIZE) //we will be able to store this in a word
							locctrAddition = 3;
						else
							errors = strCatFreeFirst(&errors, "x360x"); //we cant store this operand in a word (3 bytes)
					}
					else
						errors = strCatFreeFirst(&errors, "x340x"); //you need a dec number passed

				}
				else if (strcmp(operationName, "resb") == 0) //Reserves space for n bytes
				{
					if (isNumber10(operand) == 1) //is number
					{
						int newAddition = (strtol(operand, NULL, 10));
						if (newAddition <= MAX_LOCCTR_SIZE)
							locctrAddition = newAddition;
						else
							errors = strCatFreeFirst(&errors, "x350x"); //this addition would overflow the LOCCTR
					}
					else
						errors = strCatFreeFirst(&errors, "x340x"); //you need a dec number passed
				}
				else if (strcmp(operationName, "resw") == 0) //Reserves space for n words (3n bytes)
				{
					if (isNumber10(operand) == 1) //is number
					{
						int newAddition = (strtol(operand, NULL, 10) * 3);
						if (newAddition <= MAX_LOCCTR_SIZE)
							locctrAddition = newAddition;
						else
							errors = strCatFreeFirst(&errors, "x350x"); //this addition would overflow the LOCCTR
					}
					else
						errors = strCatFreeFirst(&errors, "x340x"); //you need a dec number passed
				}
			}

			*_operation = operationName;
		}
		else
		{
			//---------------INVALID OPERATION---------------
			
			errors = strCatFreeFirst(&errors, "x210x"); //invalid mneumonic or directive 

			//the operand field must equal something so we can print it
			operand = returnEmptyString();

			*_operation = operationName;
		}
	}

	//Frankly I have no idea why this is required... 
	//but it does what the code without this should do...
	//edit the original variable that was passed by reference...
	*_line = line;
	*_label = label;
	//operation value set by persepctive sections
	*_operand = operand;
	*_errors = errors;

	LOCCTR += locctrAddition; //now we add how must space this particular command took and move onto the next one

	//create array to pass integers "by reference"
	int *newVars = malloc(4 * sizeof(int));
	newVars[0] = LOCCTR; newVars[1] = locctrAddition; newVars[2] = startFound; newVars[3] = endFound;
	return newVars;
}

#pragma endregion

#pragma region HELPER FUNCTIONS

void freeMem(char **l) {
	char* line = *l;
	if (strlen(line) > 0)
		free(line);
}

//-------------------------SPECIFIC String Processing Functions-------------------------

char* processFirst(char** l) //actually return our first word found, by reference "return" the line
{
	char* line = *l; //link up to our value (so we can pass by reference)

	if (line[0] != '\0') //make sure we have a line left
	{
		removeSpacesFront(&line);

		//make sure we have string left to check after getting rid of all spaces
		if (isEmpty(line) == 1)
			return returnEmptyString();
		else
		{
			//add anything that isnt a space to our word
			int firstLength = 0;
			while (isspace(line[firstLength]) == 0 && line[firstLength] != '\0')
				firstLength++;

			//NOTE: inclusive index for FIRST start is firstCharIndex... exclusive index for FIRST end is lineID...
			//size of FIRST is  (lineID - firstCharIndex)
			//NOTE: inclusive index for LINE start is lineID... exclusive index for LINE end is NOT RELEVANT (use size)
			//size of LINE is (MAX_SIZE_CONST - lineID)
			//BEWARE: lineID IS NOT ALWAYS A SPACE

			//calculate first substring
			char *first = subString(line, 0, firstLength); //memory allocated by substring

			//calculate line substring
			int sizeOfLine = (strlen(line) - firstLength);
			char *lineWithoutFirst = subString(line, firstLength, sizeOfLine);
			*l = lineWithoutFirst;
			freeMem(&line);

			return first;
		}
	}
	else
		return returnEmptyString();
}

char* processRest(char** l) { //remove spaces in front of the line that its passed... return a new string that is exactly the same as the string passed without spaces
	char* line = *l;
	removeSpacesFront(&line); //remove the front of the line by reference...
	return stringCopy(line); //return a copy of the line without spaces in front...
}

int removeSpacesFront(char** l) { //returns how many spaces where removed

	char* line = *l;

	if (strlen(line) > 0) {
		//var init
		int lineID = 0;

		//ignore anything that is a space
		while (isspace(line[lineID]) != 0 && line[lineID] != '\0')
			lineID++;

		char* resultLine;

		//make sure we have string left to check after getting rid of all spaces
		if (line[lineID] == '\0')
			resultLine = returnEmptyString(); //nothing useful is left in the line
		else{
			resultLine = subString(line, lineID, (strlen(line) - lineID));
			
			*l = resultLine;
		}

		//free(line); //TODO... I beleive this should work but it doesnt

		return lineID;
	}
	else {
		line = returnEmptyString();
		return 0;
	}
}

//fills all available spots with null terminators (since space was probably already alocated for the string and we might use it eventually)
int removeSpacesBack(char** l) { //"returns" by reference
	char *line = *l;

	if (strlen(line) > 0) {
		int count = 0;
		int charID = strlen(line) - 1; //we skip the null terminator
		while (charID >= 0 && isspace(line[charID]) != 0) {
			line[charID] = '\0';
			charID--;
			count++;
		}
		line[charID+1] = '\0';
		return count;
	}
	else {
		line = returnEmptyString();
		return 0;
	}
}

//-------------------------GENERAL String Processing Functions-------------------------

char* stringCopy(char* str) {
	return subString(str, 0, strlen(str));
}

char* subString(char* src, int srcStartIndex, int subStringLength) {

	int srcI = srcStartIndex;
	int destI = 0;

	char* dest = malloc((subStringLength + 1) * sizeof(char)); //+1 for null terminator

	int counter = subStringLength;
	while (counter > 0 && srcStartIndex < strlen(src)) {
		counter--;
		dest[destI] = src[srcI];
		destI++;
		srcI++;
	}

	dest[subStringLength] = '\0';

	return dest;
}

//-------------------------Integer To String Functions-------------------------

char* reverse(char* str)
{
	if (strlen(str) > 0) {
		char* strCopy = stringCopy(str);

		int indexF2B = 0;
		int indexB2F = strlen(str) - 1;

		while (indexF2B < strlen(str)) {
			strCopy[indexF2B] = str[indexB2F];
			indexF2B++;
			indexB2F--;
		}
		strCopy[strlen(str)] = '\0';

		return strCopy;
	}
	else
		return returnEmptyString();
}

char* itoa10(int num)
{
	char *base10Str;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		base10Str = malloc(2 * sizeof(char));
		base10Str[0] = '0';
		base10Str[1] = '\0';
		return base10Str;
	}
	else
	{
		//---REMOVE negative sign (If we have one)

		int isNeg = 0;
		if (num < 0) {
			num *= -1; //make the number positive
			isNeg = 1; //make it as originally negative
		}

		//---ALLOCATE space for our positive number

		//get size of this number so we can properly allocate space
		int digits = 0;
		int numCopy = num;
		//NOTE: we can assume that at the very least we have 1 digit
		do { 
			digits++;
			numCopy = (numCopy / 10); //remove the last digit
		} while (numCopy != 0);

		base10Str = malloc((digits + 1) * sizeof(char)); //digits + null terminator

		//---CONVERT our positive number

		int index = 0;
		while (num != 0)
		{
			int rem = num % 10;
			base10Str[index] = (rem + '0'); //(rem > 9) ? (rem - 10) + 'a' : rem + '0';
			index++;
			num = num / 10;
		}

		base10Str[index] = '\0'; // Append string terminator  

		char *base10StrRev = reverse(base10Str); // Reverse the string
		freeMem(&base10Str);

		//---ADD negative sign (if needed)
		
		char *result;
		if (isNeg == 1)
			result = strCat("-", base10StrRev);
		else
			result = stringCopy(base10StrRev);
		freeMem(&base10StrRev);

		return result;
	}
}

char* itoa16(int num)
{
	return '\0';
}

//-------------------------Helper Functions-------------------------

char* returnEmptyString()
{
	char* aStr = malloc(sizeof(char));
	aStr[0] = '\0';
	return aStr;
}

void stringToLower(char** l) { //"returns" by reference

	char* line = *l;

	if (strlen(line) > 1) {
		for (int i = 0; i < strlen(line); i++)
			line[i] = tolower(line[i]);
	}
	else
		line = returnEmptyString();
}

char* strCat(char* firstString, char* lastString)
{
	int newLength = (strlen(firstString) + strlen(lastString)); //EX: 4 + 3 = 7 chars
	char* newString = malloc((newLength + 1) * sizeof(char)); //EX: allocate 7 slots (+ 1 for null terminator)

	int index = 0;
	int firstStringIndex = 0;
	int lastStringIndex = 0;
	while (index < newLength) { //EX: 7 chars indices 0 -> 6 [7]
		if (index < strlen(firstString)) //EX: 4 chars indices 0 -> 3 [4]
		{
			newString[index] = firstString[firstStringIndex];
			firstStringIndex++;
		}
		else
		{
			newString[index] = lastString[lastStringIndex];
			lastStringIndex++;
		}
		index++;
	}
	newString[newLength] = '\0'; //at last index insert null terminator

	return newString;
}

char* strCatFreeFirst(char** fS, char* lastString)
{
	char* firstString = *fS;

	int newLength = (strlen(firstString) + strlen(lastString)); //EX: 4 + 3 = 7 chars
	char* newString = malloc((newLength + 1) * sizeof(char)); //EX: allocate 7 slots (+ 1 for null terminator)

	int index = 0;
	int firstStringIndex = 0;
	int lastStringIndex = 0;
	while (index < newLength) { //EX: 7 chars indices 0 -> 6 [7]
		if (index < strlen(firstString)) //EX: 4 chars indices 0 -> 3 [4]
		{
			newString[index] = firstString[firstStringIndex];
			firstStringIndex++;
		}
		else
		{
			newString[index] = lastString[lastStringIndex];
			lastStringIndex++;
		}
		index++;
	}

	newString[newLength] = '\0'; //at last index insert null terminator

	freeMem(&firstString);

	return newString;
}

//-------------------------Error Checking Functions-------------------------

int isBlankLine(char* line) {
	for (int i = 0; i < strlen(line); i++)
		if (isspace(line[i]) == 0)
			return 0;
	return 1;
}


int isEmpty(char* charArray) //we only have a null terminator
{
	if (strcmp(charArray, "") == 0 || charArray[0] == '\0')
		return 1;
	else
		return 0;
}

int isLabel(char* line) {
	if (isspace(line[0]) == 0)
		return 1;
	else
		return 0;
}

int isValidLabel(char* label) { //1 is true, 0 is false because first value is not digit, -1 is false because it too long
	if (strlen(label) < MAX_SYMBOL_SIZE) {
		if (isdigit(label[0]) == 0)
			return 1;
		else
			return 0;
	}
	else
		return -1;
}

//inefficient but clean up code nicely
int isDirective(char* mneumonic) {
	if (strcmp(mneumonic, "start") == 0)
		return 1;
	else if (strcmp(mneumonic, "end") == 0)
		return 1;
	else if (strcmp(mneumonic, "byte") == 0)
		return 1;
	else if (strcmp(mneumonic, "word") == 0)
		return 1;
	else if (strcmp(mneumonic, "resb") == 0)
		return 1;
	else if (strcmp(mneumonic, "resw") == 0)
		return 1;
	else
		return 0;
}

int isNumber10(char* num) {
	for (int i = 0; i < strlen(num); i++)
		if (isdigit(num[i]) == 0)
			return 0;
	return 1;
}

int isNumber16(char* num) {
	for (int i = 0; i < strlen(num); i++)
		if (isxdigit(num[i]) == 0)
			return 0;
	return 1;
}


//-------------------------Symbol Table Functions (Symbol | Location)-------------------------

int resetSYMTBL() {
	for (int i = 0; i < emptyIndex; i++) {
		symbolTbl[i].key = ""; // we dont need to free memory since we always keep the same 500 slots for any other assembly anyways
		symbolTbl[i].value = -1;
	}
	emptyIndex = 0;
}

int addSYMTBL(char* key, int value) { //returns 1 if success, 0 if value must be set, -1 if symbolTbl full

	if (emptyIndex < MAX_SYMBOLS) {
		int newValueInsert = emptyIndex; //location we insert the key and value into their respective arrays
		if (getKeyIndexSYMTBL(key) == -1) //they key is not in our arrays
		{ 
			//NOTE: allocating memory for KEY is not nessesary
			symbolTbl[newValueInsert].key = key; //save char
			symbolTbl[newValueInsert].value = value;
			emptyIndex++;
			return 1;
		}
		else
			return 0;
	}
	else
		return -1;
}

int setSYMTBL(char* key, int value) { //returns 1 if key and value pairing found and update, 0 otherwise


	int oldValueOverwrite = getKeyIndexSYMTBL(key); //get the index of key...
	if (oldValueOverwrite != -1) { //they key is not in our arrays
								   //old value overwritten
		symbolTbl[oldValueOverwrite].value = value;
		return 1;
	}
	else
		return 0;
}

//NOTE: no removal function needed (for now)

int containsKeySYMTBL(char* key) {
	if (getKeyIndexSYMTBL(key) != -1) //it exists
		return 1;
	else
		return 0;
}

int getKeyIndexSYMTBL(char* key) {
	for (int i = 0; i < emptyIndex; i++)
		if (strcmp(symbolTbl[i].key, key) == 0)
			return i;
	return -1;
}

int containsValueSYMTBL(int value) {
	if (getValueIndexSYMTBL(value) != -1) //it exists
		return 1;
	else
		return 0;
}

int getValueIndexSYMTBL(int value) {
	for (int i = 0; i < emptyIndex; i++)
		if ((symbolTbl[i].value) == value)
			return i;
	return -1;
}

void printSymbolTable() {
	printf("---Symbol Table (string -> int)\n");
	for (int i = 0; i < emptyIndex; i++)
		printf("'%s' maps to '%i'\n", symbolTbl[i].key, symbolTbl[i].value);
	printf("\n");
}

//-------------------------Op Code Table Functions (Operation | OpCode)-------------------------

int containsOpCode(char* operand) {
	if (strcmp(getOpCode(operand), "-1") == 0)
		return 1;
	else
		return 0;
}

char* getOpCode(char* operand) {
	for (int index = 0; index < MAX_OPCODES; index++) 
		if (strcmp(indexToOpName(index), operand) == 0)	
			return indexToOpCode(index);

	return "-1";
}

char* indexToOpName(int index) 
{
	switch (index)
	{
	case 0:		return "add";	break;
	case 1:		return "and";	break;
	case 2:		return "comp";	break;
	case 3:		return "div";	break;
	case 4:		return "j";		break;
	case 5:		return "jeq";	break;
	case 6:		return "jgt";	break;
	case 7:		return "jlt";	break;
	case 8:		return "jsub";	break;
	case 9:		return "lda";	break;
	case 10:	return "ldch";	break;
	case 11:	return "ldl";	break;
	case 12:	return "ldx";	break;
	case 13:	return "mul";	break;
	case 14:	return "or";	break;
	case 15:	return "rd";	break;
	case 16:	return "rsub";	break;
	case 17:	return "sta";	break;
	case 18:	return "stch";	break;
	case 19:	return "stl";	break;
	case 20:	return "stx";	break;
	case 21:	return "sub";	break;
	case 22:	return "td";	break;
	case 23:	return "tix";	break;
	case 24:	return "wd";	break;
	default:	return "";		break;
	}
}

char* indexToOpCode(int index) 
{
	switch (index)
	{
	case 0:		return "18";	break;
	case 1:		return "58";	break;
	case 2:		return "28";	break;
	case 3:		return "24";	break;
	case 4:		return "3C";	break;
	case 5:		return "30";	break;
	case 6:		return "34";	break;
	case 7:		return "38";	break;
	case 8:		return "48";	break;
	case 9:		return "00";	break;
	case 10:	return "50";	break;
	case 11:	return "08";	break;
	case 12:	return "04";	break;
	case 13:	return "20";	break;
	case 14:	return "44";	break;
	case 15:	return "D8";	break;
	case 16:	return "4C";	break;
	case 17:	return "0C";	break;
	case 18:	return "54";	break;
	case 19:	return "14";	break;
	case 20:	return "10";	break;
	case 21:	return "1C";	break;
	case 22:	return "E0";	break;
	case 23:	return "2C";	break;
	case 24:	return "DC";	break;
	default:	return "";		break;
	}
}

void printOpCodeTable() {
	printf("---Op Code Table (string -> int)\n");
	for (int i = 0; i < MAX_OPCODES; i++)
		printf("'%s' maps to '%s'\n", indexToOpName(i), indexToOpCode(i));
	printf("\n");
}

#pragma endregion