/*
Programmer: Bryan Cancel
Last Updated: 3/20/18

Description:
Pass 1 will create:
-symbol table (label, address[-1 for null])
-intermediate file (opcode and operand DO NOT HAVE TO be translated into object code)
	FOR EACH LINE 
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

NOTE:
itoa() doesn't work in unix/linux server (atoi works)

I am Assuming:
(1) Everything in "TODO list (maybe)" below is not a requirement
(2) (a) intermediate file (b) listing file (c) object file -> Dont Require a Specific File Extension (using .txt)
(3) our labels work in a global scope (we dont need to check for scoping rules)
(4) a line can just have a LABEL (this is contrary to this "{label}operation {operand{,X}} {comment}") but you had mentioned it to be a possibility
	-and a line with just a LABEL must still increment the LOCCTR (otherwise you might reference an undesired label)
(5) we dont have to process our LABELS until pass 2 (eventhough we can technically process the ones that are backwards references)
(6) operands that are HEX values... ONLY has to have an even length
(7) start and end dont add to the location counter
*/

//TODO list (must)
//1. remove limit MAX_CHARS_PER_WORD
//2. repair "itoa16" to actually function
//3. write the location counter and as a HEX number onto the intermediate file

//TODO list (maybe)
//1. to the symbol table add (scope info, type[of what?], length[of what?])
//2. to the intermediate file add (pointers to Opcode Table, and pointer to Symbol Table)
//3. convert the symbol table to something actually efficient (Ideally we use a dynamic Hash Table)
//4. get reliable length from getline
//5. improve "removeSpacesBack" to also remove the newline character [it should be doing this now but my prints indicate it isnt]

#pragma once

#define min(a,b) (((a) < (b)) ? (a) : (b))

//constants that will eventually be used throughout code
#define MAX_PROGRAM_SIZE 32768
#define MAX_CHARS_PER_WORD 100
#define MAX_SYMBOLS 500
#define MAX_SYMBOL_SIZE 7 //6 spots and null terminator
#define OPCOUNT_COUNT 25
#define MAX_OPERATION_SIZE 5 //4 spots and null terminator
#define MAX_OPCODE_SIZE 3 //2 spots and null terminator
#define MAX_DIRECTIVE_SIZE 6 //5 spots and null terminator
#define MAX_ERROR_CHARACTERS 125 //24 errors (in do while loop) in total * 5 characters each =aprox= 125 (this will flat out never happen)

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

//---integer to string
char* reverse(char* str, int length);
char* itoa16(int num);
char* itoa10(int num);

//---other prototypes
int validLabel(char* label);
int labelFound(char* line);
int isDirective(char* mneumonic);
int isNumber10(char* num);
int isNumber16(char* num);
char* strCat(char* startValue, char* addition);

//---prototypes for string processing
void stringToLower(char** l);
char* processFirst(char** l);
char* processRest(char** l);
char* stringCopy(char* str);
char* subString(char* src, int srcIndex, int strLength);
void subStringRef(char** source, int srcIndex, int strLength);
int removeSpacesFront(char** l);
int removeSpacesBack(char** l);
int isBlankLine(char* line);

//---protoypes for symbol table
int addSYMTBL(char* key, int value);
//NOTE: no removal function needed (for now)
int setSYMTBL(char* key, int value);
int containsKeySYMTBL(char* key);
int getKeyIndexSYMTBL(char* key);
int containsValueSYMTBL(int value);
int getValueIndexSYMTBL(int value);
void printSymbolTable();

//---prototypes for opcode table
void fillOpCodeTable();
int containsOperation(char* operand);
int getOperationIndex(char* operand);
void printOpCodeTable();

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
charToChar opCodeTbl[OPCOUNT_COUNT];

void pass1(char* filename)
{
	printf("running pass 1\n\n");

	int programLength = 0; //require for the begining of pass 2

	//---Debugging Tools
	int printIntermediateFile = 1; //1 to print, 0 to not print
	int writeIntermediateFile = 1; //1 to write to file, 0 to not write to file

	//place INTERMEDIATE in stream, mae sure INTERMEDIATE file opens for writing properly
	FILE *ourIntermediateFile = fopen("./intermediate.txt", "w"); //wipes out the file
	if (ourIntermediateFile != NULL) 
	{
		//place SOURCE in stream, make sure SOURCE file opens for reading properly
		FILE *ourSourceFile = fopen(filename, "r");
		if (ourSourceFile != NULL)
		{
			fillOpCodeTable();

			//create the variables that will be used to read in our file
			char *line = NULL; //NOTE: this does not need a size because getline handle all of that
			size_t len = 0;

			//{label}operation {operand{,X}} {comment}
			char *label;
			char *mneumonic;
			char *operand;
			char *comment;

			//-------------------------BEFORE START

			int startFound = 0;
			//ignore lines until we find START
			while (startFound == 0 && getline(&line, &len, ourSourceFile) != -1) {

				char *lineCopy = stringCopy(line); //make a copy of the line (because the actual line should be processed below)
				stringToLower(&lineCopy); //make this line case IN-sensitive

										  //we did not find a white space(potential label)
				if (labelFound(line) == 1) {
					label = processFirst(&lineCopy);
					if (validLabel(label) == 1) {
						mneumonic = processFirst(&lineCopy);
						if (strcmp(mneumonic, "start") == 0)
							startFound = 1;
					}
					//ELSE... we have not found a label... we cannot find START
				}
				//ELSE... we have not found label... we cannot find START
			}

			//if we stoped reading the file because START was found (we have some commands to read into our file)
			if (startFound == 1) {

				//-------------------------BETWEEN START and END
				int startingAddress = 0;
				int LOCCTR = 0;
				int endFound = 0;
				//NOTE: we use a do while because the line that is currently in the "buffer" is the first line (the one with the START directive)
				do {
					
					char* errors = malloc(MAX_ERROR_CHARACTERS * sizeof(char));
					errors = "";

					stringToLower(&line); //remove case sensitivity
					char *origLine = malloc(strlen(line) * sizeof(char));
					origLine = stringCopy(line);

					if (isBlankLine(line) != 1)
					{
						//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

						int locctrAddition = 0;

						if(line[0] != '.') //we found a command
						{
							if (labelFound(line) == 1) //we have a label (but is it valid)
							{
								label = processFirst(&line);
								int validResult = validLabel(label);
								int addResult;
								switch (validResult)
								{
								case 1: //LABEL is valid (add to symbol table)
									addResult = addSYMTBL(label, LOCCTR);
									switch (addResult)
									{
									case 1: break;
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
							else
							{
								//the label field must equal something so we can print it
								label = malloc(sizeof(char) * 2);
								label[0] = ' ';
								label[1] = '\0';
							}
							
							//NOTE: by now we processed the label IF there was one -OR- had an ERROR added to it

							if(line[0] != '\0') //we have an mneumonic but is it valid?
							{
								mneumonic = processFirst(&line);
								int result = getOperationIndex(mneumonic);
								char* mneumonicCode = malloc(MAX_OPCODE_SIZE * sizeof(char));

								if (result != -1) //we have this operation
								{
									locctrAddition += 3; //add to the location counter

									mneumonicCode = opCodeTbl[result].value;

									//for everything except rsub read in an operand
									if (strcmp(mneumonic, "rsub") != 0)
									{
										operand = processFirst(&line);
										if (operand[0] == '\0')
											errors = strCat(errors, "x300x"); //missing operand
										else
										{
											int lastCharIndex = strlen(operand) - 1;
											char *rawOperand;

											if (operand[lastCharIndex] == 'x' && operand[lastCharIndex - 1]) //form 'operand,x'
												rawOperand = subString(operand, 0, strlen(operand) - 2);
											else //form 'operand' 
												rawOperand = stringCopy(operand);

											if (validLabel(rawOperand) == 1) {
												//TODO... process this (LABEL)
											}
											else
											{
												if (isNumber16(operand) == 1)
												{
													if (strlen(operand) % 2 == 0)
														; //TODO... process this (thing)
													else 
													{
														//if it begins with 'A' through 'F' we must have a leading '0' (to distinguish from a label)
														if (operand[0] == 0 && isxdigit(operand[1]) != 0 && isdigit(operand[1]) == 0)
														{
															subStringRef(&operand, 1, strlen(operand) - 1); //get rid of 0
															//TODO.... process this (thing)
														}
														else
															errors = strCat(errors, "x310x"); //hex number must be in byte so you must have an even digit count
													}
												}
												else
													errors = strCat(errors, "x320x"); //hex number required but not found
											}
										}
									}
									else
									{
										//the operand field must equal something so we can print it
										operand = malloc(sizeof(char) * 2);
										operand[0] = ' ';
										operand[1] = '\0';
									}
								}
								else //check if we have a directive
								{
									if (isDirective(mneumonic) == 1)
									{
										mneumonicCode = malloc(MAX_DIRECTIVE_SIZE * sizeof(char));
										operand = processFirst(&line);
										mneumonicCode = mneumonic;

										if (strcmp(mneumonic, "start") == 0)
										{
											if (startFound > 1)
												errors = strCat(errors, "x200x"); //extra start directive 
											else //its the first time finding a start directive
											{
												startFound++;

												if (operand[0] == '\0')
													errors = strCat(errors, "x400x"); //missing operand
												else
												{
													if (isNumber10(operand) == 1) //is number
													{
														startingAddress = strtol(operand, NULL, 16); //convert to base 10 from base 16 string
														LOCCTR = startingAddress; //init LOCCTR to starting address
													}
													else
														errors = strCat(errors, "x410x"); //we require a hex number
												}
											}
										}
										else if (strcmp(mneumonic, "end") == 0)
										{
											endFound = 1; //stop us from processing any more of this file

											if (operand[0] == '\0')
												errors = strCat(errors, "x400x"); //missing operand
											else
												; //TODO... process this (label) [location of error 402]

											programLength = (LOCCTR - startingAddress); //save program length
										}
										else if (strcmp(mneumonic, "byte") == 0)
										{
											if (operand[0] == '\0')
												errors = strCat(errors, "x400x"); //missing operand
											else
											{
												if (operand[0] == 'c')
												{
													subStringRef(&operand, 2, strlen(operand) - 3); // the three values are C, ', and '

													//max of 30 characters
													if (strlen(operand) <= 30) {
														locctrAddition = strlen(operand); //add the length of this to LOCCTR
														//TODO... make sure this is all i have to do to process this
													}
													else
														errors = strCat(errors, "x403x"); //max of 30 chars
												}
												else if (operand[0] == 'x')
												{
													subStringRef(&operand, 2, strlen(operand) - 3); // the three values are X, ', and '

													//must be even number of digits
													if (strlen(operand) % 2 == 0)
													{
														//max of 32 hex digits
														if (strlen(operand) <= 32)
														{
															if (isNumber16(operand) == 1) 
															{
																locctrAddition = strlen(operand); //add the length of this to LOCCTR
																//TODO... make sure this is all i have to do to process this
															}
															else
																errors = strCat(errors, "x401x"); //must be a hex number 
														}
														else
															errors = strCat(errors, "x404x");  //must be a max of 16 bytes 
													}
													else
														errors = strCat(errors, "x405x"); //number must be byte so must have even number of digits 
												}
												else
													errors = strCat(errors, "x406x"); //you can only pass a string or hex value as the operand to byte 
											}
										}
										else if (strcmp(mneumonic, "word") == 0)
										{
											if (operand[0] == '\0')
												errors = strCat(errors, "x400x"); //missing operand
											else
												; //TODO... process this (thing)

											locctrAddition += 3;
										}
										else if (strcmp(mneumonic, "resb") == 0)
										{
											if (operand[0] == '\0')
												errors = strCat(errors, "x400x"); //missing operand
											else
											{
												int num = strtol(operand, NULL, 16); //convert to base 10 from base 16 string
												locctrAddition += (num); //TODO... make sure this is all i have to do to process this
											}
										}
										else if (strcmp(mneumonic, "resw") == 0)
										{
											if (operand[0] == '\0')
												errors = strCat(errors, "x400x"); //missing operand
											else
											{
												int num = strtol(operand, NULL, 16); //convert to base 10 from base 16 string
												locctrAddition += (3 * num); //TODO... make sure this is all i have to do to process this
											}
										}
									}
									else
									{
										mneumonicCode = malloc(MAX_DIRECTIVE_SIZE * sizeof(char));
										mneumonicCode = mneumonic;
										errors = strCat(errors, "x210x"); //invalid mneumonic or directive 

										//the operand field must equal something so we can print it
										operand = malloc(sizeof(char));
										operand = '\0';
									}
								}

								LOCCTR += locctrAddition; //now we add how must space this particular command took and move onto the next one

								if ((LOCCTR - startingAddress) > MAX_PROGRAM_SIZE)
									errors = strCat(errors, "x900x"); //program is too long

								comment = processRest(&line);
								errors = strCat(errors, "\0"); //add a null terminator to errors

								//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]
								
								if (printIntermediateFile == 1) {
									printf("%s\n", origLine); //[1]
									printf("%x\n", (LOCCTR - locctrAddition)); //[2] 
									printf("%s\n", label); //[3]
									printf("%s\n", mneumonicCode); //[4]
									printf("%s\n", operand); //[5] (if we had operation -or- a mneumonic this is also taken care of)
									printf("%s\n", comment); //[6]
									printf("%s\n", errors); //[7]
									printf("\n"); //[\n]
								}
								if (writeIntermediateFile == 1) {
									fputs(strCat(origLine, "\n"), ourIntermediateFile); //[1]
									char *eptr;
									fputs(strCat(itoa16(LOCCTR - locctrAddition), "\n"), ourIntermediateFile); //[2] (LOCCTR - locctrAddition)
									fputs(strCat(label, "\n"), ourIntermediateFile); //[3]
									fputs(strCat(mneumonicCode, "\n"), ourIntermediateFile); //[4]
									fputs(strCat(operand, "\n"), ourIntermediateFile); //[5] (if we had operation -or- a mneumonic this is also taken care of)
									fputs(strCat(comment, "\n"), ourIntermediateFile); //[6]
									fputs(strCat(errors, "\n"), ourIntermediateFile); //[7]
									fputs("\n", ourIntermediateFile); //[\n]
								}
							}
							else //we DID NOT find a mneumonic
							{
								//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

								locctrAddition += 3; //add to the location counter

								errors = strCat(errors, "\0"); //add a null terminator to errors

								if (printIntermediateFile == 1) {
									printf("%s\n", origLine); //[1]
									printf("%x\n", LOCCTR); //[2]
									printf("%s\n", label); //[3]
									printf("\n\n\n"); //[4] -> [6]
									printf("%s\n", errors); //[7]
									printf("\n"); //[\n]
								}
								if (writeIntermediateFile == 1) {
									fputs(strCat(origLine, "\n"), ourIntermediateFile); //[1]
									char *eptr;
									fputs(strCat(itoa16(LOCCTR), "\n"), ourIntermediateFile); //[2] (LOCCTR)
									fputs(strCat(label, "\n"), ourIntermediateFile); //[3]
									fputs(strCat(errors, "\n"), ourIntermediateFile); //[4]
									fputs("\n\n\n", ourIntermediateFile); //[5] -> [7]
									fputs("\n", ourIntermediateFile); //[\n]
								}
							}
						}
						else //we found a comment
						{
							//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]

							if (printIntermediateFile == 1) {
								printf("%s\n", origLine); //[1]
								printf("%x\n", LOCCTR); //[2]
								printf("\n\n\n\n\n"); //[3] -> [7]
								printf("\n"); //[\n]
							}
							if (writeIntermediateFile == 1) {
								fputs(strCat(origLine, "\n"), ourIntermediateFile); //[1]
								char* eptr;
								fputs(strCat(itoa16(LOCCTR), "\n"), ourIntermediateFile); //[2] (LOCCTR)
								fputs("\n\n\n\n\n", ourIntermediateFile); //[3] -> [7]
								fputs("\n", ourIntermediateFile); //[\n]
							}
						}
					}
					//ELSE... we ignore this blank line

				} while (getline(&line, &len, ourSourceFile) != -1 && endFound == 0);

				//-------------------------AFTER END

				//if we stopeed reading the file because END was found
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
				//SPECIAL ERROR [8 lines] (no start directive)
				if (printIntermediateFile == 1)
					printf("\n\n\n\n\n\nx010x\n\n");
				if(writeIntermediateFile == 1)
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

		fclose(ourIntermediateFile); //close our intermediate file after writing to it
	}
	else //INTERMEDIATE did not open properly
		printf("ERROR --- INTERMEDIATE file did not open properly\n"); //THE ONLY ERROR THAT CANNOT BE IN THE INTERMEDIATE FILE
}

//-------------------------EXTRA PROCS-------------------------

//----------Integer to String

char* reverse(char* str, int length)
{
	char* strCopy = stringCopy(str);

	int index = 0;
	int index2 = length - 1;

	while (index < length) {
		strCopy[index] = str[index2];
		index++;
		index2--;
	}

	return strCopy;
}

char* itoa16(int num)
{
	char *ret;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		ret = malloc(2 * sizeof(char));
		ret[0] = '0';
		ret[1] = '\0';
		return ret;
	}
	else
	{
		int isNeg = 0;
		if (num < 0) {
			num *= -1; //make the number positive
			isNeg = 1;
		}

		//process the number (this is now guaranteed positive)

		//get size of this number so we can properly allocate space
		int digits = 0;
		int numCopy = num;
		while ((numCopy / 10) != 0) {
			digits++;
			numCopy = numCopy / 10; //remove the last digit
		}

		ret = malloc(digits * sizeof(char));

		// Process individual digits
		int index = 0;
		while (num != 0)
		{
			int rem = num % 10;
			ret[index] = (rem > 9) ? ((rem - 10) + 'a') : (rem + '0');
			index++;
			num = num / 10;
		}

		ret[index] = '\0'; // Append string terminator  

		ret = reverse(ret, index); // Reverse the string

		//add negative sign
		if (isNeg == 1)
			ret = strCat("-", ret);

		return ret;
	}
}

char* itoa10(int num)
{
	char *ret;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		ret = malloc(2 * sizeof(char));
		ret[0] = '0';
		ret[1] = '\0';
		return ret;
	}
	else
	{
		int isNeg = 0;
		if (num < 0) {
			num *= -1; //make the number positive
			isNeg = 1;
		}

		//process the number (this is now guaranteed positive)

		//get size of this number so we can properly allocate space
		int digits = 0;
		int numCopy = num;
		while ((numCopy / 10) != 0) {
			digits++;
			numCopy = numCopy / 10; //remove the last digit
		}

		ret = malloc(digits * sizeof(char));

		// Process individual digits
		int index = 0;
		while (num != 0)
		{
			int rem = num % 10;
			ret[index] = (rem + '0'); //(rem > 9) ? (rem - 10) + 'a' : rem + '0';
			index++;
			num = num / 10;
		}

		ret[index] = '\0'; // Append string terminator  

		ret = reverse(ret, index); // Reverse the string

		//add negative sign
		if (isNeg == 1)
			ret = strCat("-", ret);

		return ret;
	}
}

//----------Handle Tockenizing

char* processFirst(char** l) //actually return our first word found, by reference "return" the line
{
	char* line = *l; //link up to our value (so we can pass by reference)
	char* first = malloc(MAX_CHARS_PER_WORD * sizeof(char)); //create value (so we can pass it by value)
	
	if (line[0] != '\0') //make sure we have a line left
	{
		//var init
		int lineID = 0;

		//ignore anything that is a space
		while (isspace(line[lineID]) != 0 && line[lineID] != '\0') {
			lineID++;
		}

		//make sure we have string left to check after getting rid of all spaces
		if (line[lineID] == '\0') {
			line[0] = '\0'; //nothing useful is left in the line
			return '\0';
		}
		else
		{
			//used to create both of our substring
			int firstCharIndex = lineID;

			//var init
			int firstID = 0;

			//add anything that isnt a space to our word
			while (isspace(line[lineID]) == 0 && line[lineID] != '\0' && firstID < MAX_CHARS_PER_WORD) {
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
		return '\0';
}

//-------------------------Other Functions 

int labelFound(char* line) {
	if (isspace(line[0]) == 0)
		return 1;
	else
		return 0;
}

int validLabel(char* label) { //1 is true, 0 is false because first value is not digit, -1 is false because it too long
	if (strlen(label) < MAX_SYMBOL_SIZE) {
		if (isdigit(label[0]) == 0)
			return 1;
		else
			return 0;
	}
	else
		return -1;
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

char* strCat(char* startValue, char* addedValue) 
{
	int newLength = (strlen(startValue) + strlen(addedValue));
	char* newString = malloc(newLength * sizeof(char));
	
	int index = 0;
	int startIndex = 0;
	int addIndex = 0;
	while (index < newLength) {
		if (index < strlen(startValue)) 
		{
			newString[index] = startValue[startIndex];
			startIndex++;
		}
		else
		{
			newString[index] = addedValue[addIndex];
			addIndex++;
		}
		index++;
	}
	newString[newLength] = '\0';

	return newString;
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

//-------------------------String Parsing and Tokenizing Functions 

char* processRest(char** l) { //remove spaces in front of the line that its passed... return a new string that is exactly the same as the string passed without spaces
	char* line = *l;
	removeSpacesFront(&line);
	return stringCopy(line);
}

char* stringCopy(char* str) {

	return subString(str, 0, strlen(str));
}

void stringToLower(char** l) { //"returns" by reference

	char* line = *l;

	for (int i = 0; i < strlen(line); i++)
		line[i] = tolower(line[i]);
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

int removeSpacesFront(char** l) { //returns how many spaces where removed

	char* line = *l;

	//var init
	int lineID = 0;

	//ignore anything that is a space
	while (lineID <= strlen(line) && isspace(line[lineID]) != 0 && line[lineID] != '\0') {
		lineID++;
	}

	//make sure we have string left to check after getting rid of all spaces
	if (line[lineID] == '\0')
		line = '\0'; //nothing useful is left in the line
	else
		subStringRef(&line, lineID, (strlen(line) - lineID));
	return lineID;
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
	else
		return 0;
}

int isBlankLine(char* line) {
	for (int i = 0; i < strlen(line); i++)
		if (isspace(line[i]) == 0)
			return 0;
	return 1;
}

//-------------------------Symbol Table Functions (Symbol | Location)

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

//-------------------------Op Code Table Functions (Symbol | Location)

void fillOpCodeTable() {

	int index = 0;
	for (int index = 0; index < OPCOUNT_COUNT; index++) {

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

		//un allocate memory for temporary values (this is causing erros in linux)
		//free(theKey);
		//free(theValue);
	}
}

int containsOperation(char* operand) {
	if (getOperationIndex(operand) != -1)
		return 1;
	else
		return 0;
}

int getOperationIndex(char* operand) {
	for (int i = 0; i < OPCOUNT_COUNT; i++)
		if (strcmp(opCodeTbl[i].key, operand) == 0)
			return i;
	return -1;
}

void printOpCodeTable() {
	printf("---Op Code Table (string -> int)\n");
	for (int i = 0; i < OPCOUNT_COUNT; i++)
		printf("'%s' maps to '%s'\n", opCodeTbl[i].key, opCodeTbl[i].value);
	printf("\n");
}