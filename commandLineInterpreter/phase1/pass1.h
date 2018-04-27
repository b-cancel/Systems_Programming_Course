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

#include "helperFunctions.h"
#include "pass2.h"

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#pragma endregion

#pragma region Prototypes

void pass1(char* filename);

int* processFullInstruction(
	char **_line, char **_label, char **_operation, char **_operand, char **_errors,
	int LOCCTR, int locctrAddition, int startFound, int endFound //4
);

#pragma endregion

#pragma region Main Functions

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

	printf("\nSource File: '%s'\n", filename);
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
									errors = strCatFreeFirst(&errors, "x910x");

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
								//freeMem(&label); //NOTE: dont clear because the symbol table is using this memory
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
									errors = strCatFreeFirst(&errors, "x910x");

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

				char* symStr = strCat(symbolTbl[i].key, " ");
				char* val = itoa10(symbolTbl[i].value);

				symStr = strCatFreeFirst(&symStr, val);
				symStr = strCatFreeFirst(&symStr, "\n");
				fputs(symStr, ourIntermediateFile);
				
			}
			fputs("\n", ourIntermediateFile);
		}

		fclose(ourIntermediateFile); //close our intermediate file after writing to it

		printf("PASS 1 COMPLETE\n\n");

		pass2(filename, interFileName, &programFirstLabel, &programLastLabel, programLength);
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
							errors = strCatFreeFirst(&errors, "x490x"); //we cant store this operand in a word (3 bytes)
					}
					else
						errors = strCatFreeFirst(&errors, "x470x"); //you need a dec number passed

				}
				else if (strcmp(operationName, "resb") == 0) //Reserves space for n bytes
				{
					if (isNumber10(operand) == 1) //is number
					{
						int newAddition = (strtol(operand, NULL, 10));
						if (newAddition <= MAX_LOCCTR_SIZE)
							locctrAddition = newAddition;
						else
							errors = strCatFreeFirst(&errors, "x480x"); //this addition would overflow the LOCCTR
					}
					else
						errors = strCatFreeFirst(&errors, "x470x"); //you need a dec number passed
				}
				else if (strcmp(operationName, "resw") == 0) //Reserves space for n words (3n bytes)
				{
					if (isNumber10(operand) == 1) //is number
					{
						int newAddition = (strtol(operand, NULL, 10) * 3);
						if (newAddition <= MAX_LOCCTR_SIZE)
							locctrAddition = newAddition;
						else
							errors = strCatFreeFirst(&errors, "x480x"); //this addition would overflow the LOCCTR
					}
					else
						errors = strCatFreeFirst(&errors, "x470x"); //you need a dec number passed
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