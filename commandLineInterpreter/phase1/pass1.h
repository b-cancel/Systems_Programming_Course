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
//4. remove arbitrary limitations
//5. code "itoa16" to actually convert an integer into HEX representation
//6. repair "subString" and "subStringRef" functions (they seem like they are working proplery but the dont because code stringCopy by using substring will give you a bad result)

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
#define MAX_OPCODE_SIZE 3 //2 spots and null terminator
//implicit limitations
#define MAX_OPERATION_SIZE 5 //4 spots and null terminator
#define MAX_DIRECTIVE_SIZE 6 //5 spots and null terminator
#define MAX_LOCCTR_SIZE 65535 //(FF,FF) base 16
#define MAX_SIZE  16777215 //(FF,FF,FF) base 16
//arbitrary limitations
#define MAX_CHARS_PER_INSTRUCTION_SECTION 100

#pragma endregion

#pragma region Helper Function Prototypes

//---GENERAL String Processing Prototypes (require repairs)
char* stringCopy(char* str); //NOTE: this could use substring but substring is giving me problems
char* subString(char* src, int srcIndex, int strLength);
void subStringRef(char** source, int srcIndex, int strLength);

//---SPECIFIC String Processing Prototypes (require repairs)
char* processFirst(char** l); //CHECK after repairing substrings
char* processRest(char** l);
int removeSpacesFront(char** l); //CHECK after repairing substrings
int removeSpacesBack(char** l); //works

//---Integer To String Prototypes
char* reverse(char* str);
char* itoa10(int num);
char* itoa16(int num); //TODO... implement

//---Helper Prototypes
char* returnEmptyString();
void stringToLower(char** l);
char* strCat(char* startValue, char* addition);

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
void buildOpCodeTable();
int containsOpCode(char* operand);
int getOpCodeIndex(char* operand);
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

//---OpCode Table Global Vars
typedef struct charToChar charToChar;
struct charToChar {
	char* key; //size 5 as set by constant
	char* value; //size 2 as set by constant
};
charToChar opCodeTbl[MAX_OPCODES];

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
			buildOpCodeTable();

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
					printf("label b4: '%s'\n", label);
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
										}
									}
									else //ELSE... we dont have a hex number
									{
										//SPECIAL ERROR [8 lines] (no valid hex operand for start directive)
										if (printIntermediateFile == 1)
											printf("%s\n\n\n\n\n\nx030x\n\n", line);
										if (writeIntermediateFile == 1)
											fputs(strCat(line, "\n\n\n\n\n\nx030x\n\n"), ourIntermediateFile);
									}
								}
								//ELSE... we have a missing operand we cannot continue
							}
							//ELSE... we did not find the START directive
						}
						else //we did not find an operation
							operation = returnEmptyString();
					}
					else //we did not find a label
						label = returnEmptyString();
				}
				//ELSE... we have not found label in our line... we cannot find START
			}

			printf("label af: '%s'\n", label);
			programFirstLabel = stringCopy(label);

			//if we stoped reading the file because a START with a valid operand was found (we have some commands to read into our file)
			if (startFound == 1) {

				//--------------------------------------------------BETWEEN START and END--------------------------------------------------

				int endFound = 0;

				//NOTE: we use a do while because the line that is currently in the "buffer" is the first line (the one with the START directive)
				do
				{
					char* errors = returnEmptyString();

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
							if (isLabel(tempLine) == 1)
								label = processFirst(&tempLine); //we are guaranteed to find atleast something
							else
								label = returnEmptyString();

							operation = processFirst(&tempLine);

							if (isEmpty(operation) != 1) //we have an operation
							{
								//--------------------PROCESS Full Instruction

								int *newVars = malloc(4 * sizeof(int));

								//grab new vars after running function
								newVars = processFullInstruction(
									&line, &label, &operation, &operand, &errors,
									LOCCTR, locctrAddition, startFound, endFound
								);

								if (strcmp(operation, "end") == 0)
									programLastLabel = operand;

								//set new vars after running function
								LOCCTR = newVars[0]; locctrAddition = newVars[1]; startFound = newVars[2]; endFound = newVars[3];

								//--------------------PRINT Full Instruction

								//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

								programLength = (LOCCTR - startingAddress);
								if (programLength > MAX_PROGRAM_SIZE)
									errors = strCat(errors, "x900x"); //program is too long
								if (LOCCTR > MAX_LOCCTR_SIZE)
									errors = strCat(errors, "x901x");

								comment = processRest(&line);
								errors = strCat(errors, "\0"); //add a null terminator to errors

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
							}
							else //we are missing an operation
							{
								//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

								errors = strCat(errors, "x140x");

								programLength = (LOCCTR - startingAddress);
								if (programLength > MAX_PROGRAM_SIZE)
									errors = strCat(errors, "x900x"); //program is too long
								if (LOCCTR > MAX_LOCCTR_SIZE)
									errors = strCat(errors, "x901x");

								errors = strCat(errors, "\0"); //add a null terminator to errors

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

				} while (getline(&line, &len, ourSourceFile) != -1 && endFound == 0);

				//--------------------------------------------------AFTER END -or- EOF--------------------------------------------------

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
				char* str = strCat(symbolTbl[i].key, " maps to ");
				char* val = itoa10(symbolTbl[i].value);
				str = strCat(str, val);
				str = strCat(str, "\n");
				fputs(str, ourIntermediateFile);
			}
			fputs("\n", ourIntermediateFile);
		}

		fclose(ourIntermediateFile); //close our intermediate file after writing to it

		printf("finished PASS 1\n");

		pass2(filename, interFileName, &programFirstLabel, &programLastLabel, programLength);
	}
	else //INTERMEDIATE did not open properly
		printf("ERROR --- INTERMEDIATE file did not open properly\n"); //THE ONLY ERROR THAT CANNOT BE IN THE INTERMEDIATE FILE

	//NOTE: we cant start pass 2 unless at the very least the intermediate file opened properly in pass 1
}

//-------------------------THE MEAT-------------------------

int* processFullInstruction(
	char **_line, char **_label, char **_operation, char **_operand, char **_errors,
	int LOCCTR, int locctrAddition, int startFound, int endFound //4
)
{
	//Link up to our variables by reference
	char *line = *_line; char *label = *_label; char *operation = *_operation; char *operand = *_operand; char *errors = *_errors;

	//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

	//-------------------------LABEL FIELD-------------------------

	if (isLabel(line) == 1) //we have a label
	{
		label = processFirst(&line);
		if (isEmpty(label) == 1)
			label = returnEmptyString();
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
				case 0: errors = strCat(errors, "x100x"); break; //duplicate label
				case -1: errors = strCat(errors, "x110x"); break; //symbol tbl full
				default: break;
				}
				break;
			case 0: errors = strCat(errors, "x120x"); break; //label starts with digit
			case -1: errors = strCat(errors, "x130x"); break; //label is too long
			default: break;
			}
		}
	}
	else //the label field must equal something so we can print it
		label = returnEmptyString(); //NOTE: this means we have no label but we DO have an operation

	operation = processFirst(&line);

	//NOTE: by now we processed the label IF there was one -AND- added the nessesarily ERRORS

	//-------------------------OPERATION FIELD-------------------------

	int result = getOpCodeIndex(operation);
	char* operationCode = malloc(MAX_OPCODE_SIZE * sizeof(char));

	if (result != -1) //we have this mnemonic
	{
		//---------------MNEMONIC FOUND---------------

		//NOTE: all mnemonic add to the location counter BUT... we only add to it if everything is valid...

		operationCode = opCodeTbl[result].value;

		//for everything except rsub read in an operand
		if (strcmp(operation, "rsub") != 0)
		{
			operand = processFirst(&line);
			if (isEmpty(operand) == 1)
			{
				errors = strCat(errors, "x300x"); //missing operand
				operand = returnEmptyString();
			}
			else
			{
				int lastCharIndex = strlen(operand) - 1;
				char *rawOperand;

				//---Extract Operand (ignore ,X)

				//TODO... UNTESTED...
				if (operand[lastCharIndex] == 'x' && operand[lastCharIndex - 1] == ',') //form 'operand,x'
				{
					rawOperand = subString(operand, 0, strlen(operand) - 2);
					if (isEmpty(rawOperand) == 1)
					{
						errors = strCat(errors, "x300x"); //missing operand
						rawOperand = returnEmptyString();
					}
					//ELSE... we have the operand we require
				}
				else //form 'operand' 
					rawOperand = stringCopy(operand);

				//---Check to see what the "rawOperand" is

				if (isValidLabel(rawOperand) != 1)
				{
					if (isNumber16(operand) == 1)
					{

						int secondHexDigitAtoF = (isxdigit(operand[1]) != 0 && isdigit(operand[1]) == 0) ? 1 : 0; //if we have a HEX number that starts with A -> F
						if (secondHexDigitAtoF && operand[0] == '0') //and we have a leading 0
						{
							//***we have a HEX number that uses a leading 0 to distinguish itself from a Label

							//shift everything to the left
							//NOTE: I would use substring but for some unknown reason it isnt working properly for this particular scenerio
							for (int i = 1; i <= strlen(operand); i++)
								operand[i - 1] = operand[i];
							operand[strlen(operand)] = '\0'; //set null terminator
						}
						//ELSE... our hex number could have just started with a digit 0->9

						//---Now see if this is the type of HEX number that we want

						//TODO... UNTESTED...
						if (strlen(operand) % 2 != 0)
							errors = strCat(errors, "x310x"); //hex number must be in bytes so you must have an even digit count
						else //we can easily handle this operand as a HEX number
							locctrAddition = 3;
					}
					else
						errors = strCat(errors, "x320x"); //hex number required but not found
				}
				else //we can easily handle this operand as a label
				{
					locctrAddition = 3;
					operand = returnEmptyString();
				}
			}
		}
		else //RSUB doesnt require an operand but the operand field must equal something so we can print it
		{
			locctrAddition = 3;
			operand = returnEmptyString();
		}
	}
	else //check if we have a directive
	{
		if (isDirective(operation) == 1)
		{
			//---------------DIRECTIVE FOUND---------------

			operationCode = malloc(MAX_DIRECTIVE_SIZE * sizeof(char));
			operationCode = operation;

			//NOTE: all directives have operands
			operand = processFirst(&line);
			if (isEmpty(operand) == 1) {
				errors = strCat(errors, "x400x"); //missing operand
				operand = returnEmptyString();
			}
			//ELSE... we have the operand we require

			if (strcmp(operation, "start") == 0)
			{
				//NOTE: because we checked that 'a' START directive was found with a VALID operand
				//and using that operand we set our LOCCTR like we should have
				//IF we are here then we know we have an EXTRA START directive
				//except the for the first start we find (which we run in here because we still need to process the validity of its label)
				//we know for a fact that its operand is valid

				if (startFound > 1)
					errors = strCat(errors, "x200x"); //extra start directive 
				startFound++;
			}
			else if (strcmp(operation, "end") == 0)
			{
				endFound = 1; //stop us from processing any more of this file
			}
			else if (strcmp(operation, "byte") == 0) //Stores either character strings (C'...') or hexadecimal values (X'...')
			{
				if (operand[0] == 'c')
				{
					char *tempOperand = subString(operand, 2, strlen(operand) - 3); // the three values are C, ', and '

					if (strlen(tempOperand) <= 30) //max of 30 characters
						locctrAddition = strlen(tempOperand); //add enough space to store this string (one spot for each character)
					else
						errors = strCat(errors, "x430x"); //max of 30 chars
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
								errors = strCat(errors, "x410x"); //must be a hex number 
						}
						else
							errors = strCat(errors, "x440x");  //must be a max of 16 bytes 
					}
					else
						errors = strCat(errors, "x450x"); //number must be byte so must have even number of digits 
				}
				else
					errors = strCat(errors, "x460x"); //you can only pass a string or hex value as the operand to byte 
			}
			else if (strcmp(operation, "word") == 0)
			{
				if (isNumber10(operand) == 1)
				{
					if ((strtol(operand, NULL, 10)) <= MAX_SIZE) //we will be able to store this in a word
						locctrAddition = 3;
					else
						errors = strCat(errors, "x360x"); //we cant store this operand in a word (3 bytes)
				}
				else
					errors = strCat(errors, "x340x"); //you need a dec number passed

			}
			else if (strcmp(operation, "resb") == 0) //Reserves space for n bytes
			{
				if (isNumber10(operand) == 1) //is number
				{
					int newAddition = (strtol(operand, NULL, 10));
					if (newAddition <= MAX_LOCCTR_SIZE)
						locctrAddition = newAddition;
					else
						errors = strCat(errors, "x350x"); //this addition would overflow the LOCCTR
				}
				else
					errors = strCat(errors, "x340x"); //you need a dec number passed
			}
			else if (strcmp(operation, "resw") == 0) //Reserves space for n words (3n bytes)
			{
				if (isNumber10(operand) == 1) //is number
				{
					int newAddition = (strtol(operand, NULL, 10) * 3);
					if (newAddition <= MAX_LOCCTR_SIZE)
						locctrAddition = newAddition;
					else
						errors = strCat(errors, "x350x"); //this addition would overflow the LOCCTR
				}
				else
					errors = strCat(errors, "x340x"); //you need a dec number passed
			}
		}
		else
		{
			//---------------INVALID OPERATION---------------

			operationCode = malloc(MAX_DIRECTIVE_SIZE * sizeof(char));
			operationCode = operation;
			errors = strCat(errors, "x210x"); //invalid mneumonic or directive 

											  //the operand field must equal something so we can print it
			operand = returnEmptyString();
		}
	}

	//Frankly I have no idea why this is required... 
	//but it does what the code without this should do...
	//edit the original variable that was passed by reference...
	*_line = line;
	*_operation = operationCode;
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

//-------------------------GENERAL String Processing Functions-------------------------

char* stringCopy(char* str) {

	int useSubstring = 0;

	if (useSubstring == 1)
		return subString(str, 0, strlen(str));
	else
	{
		char* newStr = malloc((strlen(str) + 1) * sizeof(char));
		for (int i = 0; i < strlen(str); i++)
			newStr[i] = str[i];
		newStr[strlen(str)] = '\0';
		return newStr;
	}
}

char* subString(char* src, int srcIndex, int strLength) {

	int srcI = srcIndex;
	int destI = 0;

	char* dest = malloc(strlen(src) * sizeof(char));

	while (strLength > 0) {
		strLength--;
		dest[destI] = src[srcI];
		destI++;
		srcI++;
	}

	int nullTermIndex = min(strlen(src) - 2, destI);
	dest[nullTermIndex] = '\0';

	return dest;
}

void subStringRef(char** source, int srcIndex, int strLength) { //pass src by reference... it will be returned by reference

	char* src = *source;

	int srcI = srcIndex;
	int destI = 0;

	while (strLength > 0) {
		strLength--;
		src[destI] = src[srcI];
		destI++;
		srcI++;
	}

	int nullTermIndex = min(strlen(src) - 2, destI);
	src[nullTermIndex] = '\0';
}

//-------------------------SPECIFIC String Processing Functions-------------------------

char* processFirst(char** l) //actually return our first word found, by reference "return" the line
{
	char* line = *l; //link up to our value (so we can pass by reference)

	if (line[0] != '\0') //make sure we have a line left
	{
		printf("before line '%s'\n", line);

		removeSpacesFront(&line);

		printf("after line '%s'\n", line);

		//make sure we have string left to check after getting rid of all spaces
		if (isEmpty(line) == 1)
			return line;
		else
		{
			char* first = malloc(MAX_CHARS_PER_INSTRUCTION_SECTION * sizeof(char)); //create value (so we can pass it by value)

			int lineID = 0;

			//used to create both of our substring
			int firstCharIndex = lineID;

			//var init
			int firstID = 0;

			//add anything that isnt a space to our word
			while (isspace(line[lineID]) == 0 && line[lineID] != '\0' && firstID < MAX_CHARS_PER_INSTRUCTION_SECTION) {
				lineID++;
				firstID++;
			}

			//NOTE: inclusive index for FIRST start is firstCharIndex... exclusive index for FIRST end is lineID...
			//size of FIRST is  (lineID - firstCharIndex)
			//NOTE: inclusive index for LINE start is lineID... exclusive index for LINE end is NOT RELEVANT (use size)
			//size of LINE is (MAX_SIZE_CONST - lineID)
			//BEWARE: lineID IS NOT ALWAYS A SPACE

			//calculate first substring
			int sizeOfFirst = (lineID - firstCharIndex);
			first = subString(line, firstCharIndex, sizeOfFirst);

			//calculate line substring
			int sizeOfLine = (strlen(line) - lineID);
			subStringRef(&line, lineID, sizeOfLine);

			return first;
		}
	}
	else
		return returnEmptyString();
}

char* processRest(char** l) { //remove spaces in front of the line that its passed... return a new string that is exactly the same as the string passed without spaces
	char* line = *l;
	removeSpacesFront(&line);
	return stringCopy(line);
}

int removeSpacesFront(char** l) { //returns how many spaces where removed

	char* line = *l;

	if (strlen(line) > 0) {
		//var init
		int lineID = 0;

		//ignore anything that is a space
		while (isspace(line[lineID]) != 0 && line[lineID] != '\0')
			lineID++;

		//make sure we have string left to check after getting rid of all spaces
		if (line[lineID] == '\0')
			line = returnEmptyString(); //nothing useful is left in the line
		else
			subStringRef(&line, lineID, (strlen(line) - lineID));
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
		int charID = strlen(line); //where the null terminator would be
		while (charID >= 0 && isspace(line[charID]) != 0) {
			line[charID] = '\0';
			charID--;
			count++;
		}
		line[charID] = '\0';
		return count;
	}
	else {
		line = returnEmptyString();
		return 0;
	}
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

		base10Str = reverse(base10Str); // Reverse the string

		//---ADD negative sign (if needed)
		
		if (isNeg == 1)
			base10Str = strCat("-", base10Str);

		return base10Str;
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
	else {
		line = returnEmptyString();
		return 0;
	}
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
		free(symbolTbl[i].key);
		symbolTbl[i].value = -1;
	}
	emptyIndex = 0;
}

int addSYMTBL(char* key, int value) { //returns 1 if success, 0 if value must be set, -1 if symbolTbl full

	if (emptyIndex < MAX_SYMBOLS) {
		int newValueInsert = emptyIndex; //location we insert the key and value into their respective arrays
		if (getKeyIndexSYMTBL(key) == -1) { //they key is not in our arrays
											//new value inserted
			symbolTbl[newValueInsert].key = malloc(MAX_SYMBOL_SIZE * sizeof(char)); //allocate memory for char
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

void buildOpCodeTable() {

	int index = 0;
	for (int index = 0; index < MAX_OPCODES; index++) {

		//allocate memory for both values
		opCodeTbl[index].key = malloc(MAX_OPERATION_SIZE * sizeof(char)); //allocate memory for char
		opCodeTbl[index].value = malloc(MAX_OPCODE_SIZE * sizeof(char)); //1 space for the null terminator

																		 //allocate memory for temporary values
		char* theKey = malloc(MAX_OPERATION_SIZE * sizeof(char));;
		char* theValue = malloc(2 * sizeof(char));

		switch (index)
		{
		case 0: //	ADD m 				18 		A <-(A)+(m..m + 2)
			theKey = "add";	theValue = "18";	break;
		case 1: //	AND m 				58 		A <-(A) & (m..m + 2)[bitwise]
			theKey = "and";	theValue = "58";	break;
		case 2: //	COMP m 				28 		cond code <-(A) : (m..m + 2)
			theKey = "comp";	theValue = "28";	break;
		case 3: //	DIV m 				24 		A <-(A) / (m..m + 2)
			theKey = "div";	theValue = "24";	break;
		case 4: //	J m 				3C 		PC <-m
			theKey = "j";	theValue = "3C";	break;
		case 5: //	JEQ m 				30 		PC <-m if cond code set to =
			theKey = "jeq";	theValue = "30";	break;
		case 6: //	JGT m 				34 		PC <-m if cond code set to >
			theKey = "jgt";	theValue = "34";	break;
		case 7: //	JLT m 				38 		PC <-m if cond code set to <
			theKey = "jlt";	theValue = "38";	break;
		case 8: //	JSUB m 				48 		L <-(PC); PC <-m
			theKey = "jsub";	theValue = "48";	break;
		case 9: //	LDA m 				00 		A <-(m..m + 2)
			theKey = "lda";	theValue = "00";	break;
		case 10: //	LDCH m 				50 		A[rightmost byte] <-(m)
			theKey = "ldch";	theValue = "50";	break;
		case 11: //	LDL m 				08 		L <-(m..m + 2)
			theKey = "ldl";	theValue = "08";	break;
		case 12: //	LDX m 				04 		X <-(m..m + 2)
			theKey = "ldx";	theValue = "04";	break;
		case 13: //	MUL m 				20 		A <-(A) * (m..m + 2)
			theKey = "mul";	theValue = "20";	break;
		case 14: //	OR m 				44 		A <-(A) | (m..m + 2)[bitwise]
			theKey = "or";	theValue = "44";	break;
		case 15: //	RD m 				D8 		A[rightmost byte] <-data from device specified by(m)
			theKey = "rd";	theValue = "D8";	break;
		case 16: //	RSUB 				4C 		PC <-(L)
			theKey = "rsub";	theValue = "4C";	break;
		case 17: //	STA m 				0C 		m..m + 2 <-(A)
			theKey = "sta";	theValue = "0C";	break;
		case 18: //	STCH m 				54 		m <-(A)[rightmost byte]
			theKey = "stch";	theValue = "54";	break;
		case 19: //	STL m 				14 		m..m + 2 <-(L)
			theKey = "stl";	theValue = "14";	break;
		case 20: //	STX m 				10 		m..m + 2 <-(X)
			theKey = "stx";	theValue = "10";	break;
		case 21: //	SUB m 				1C 		A <-(A)-(m..m + 2)
			theKey = "sub";	theValue = "1C";	break;
		case 22: //	TD m 				E0 		Test device specified by(m)
			theKey = "td";	theValue = "E0";	break;
		case 23: //	TIX m 				2C 		X <-(X)+1; compare X and (m..m + 2)
			theKey = "tix";	theValue = "2C";	break;
		case 24: //	WD m 				DC 		Device specified by(m) <-(A)[rightmost byte]
			theKey = "wd";	theValue = "DC";	break;
		default:
			break;
		}

		//assign values to dictionary
		opCodeTbl[index].key = theKey;
		opCodeTbl[index].value = theValue;
	}
}

int containsOpCode(char* operand) {
	if (getOpCodeIndex(operand) != -1)
		return 1;
	else
		return 0;
}

int getOpCodeIndex(char* operand) {
	for (int i = 0; i < MAX_OPCODES; i++)
		if (strcmp(opCodeTbl[i].key, operand) == 0)
			return i;
	return -1;
}

void printOpCodeTable() {
	printf("---Op Code Table (string -> int)\n");
	for (int i = 0; i < MAX_OPCODES; i++)
		printf("'%s' maps to '%s'\n", opCodeTbl[i].key, opCodeTbl[i].value);
	printf("\n");
}

#pragma endregion