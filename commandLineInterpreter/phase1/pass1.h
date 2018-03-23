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

I am Assuming: (should find out these details if possible)
(1) Everything in "TODO list (maybe)" below is not a requirement
(2) (a) intermediate file (b) listing file (c) object file -> Dont Require a Specific File Extension (using .txt)
(3) our labels work in a global scope
(4) a valid LABEL must be in front of the START directive
(5) a line can only have a LABEL (this is contrary to this "{label}operation {operand{,X}} {comment}") but you had mentioned it to be a possibility
(6) the location counter is not incresed by comments
*/

//TODO list (must)
//1. remove limit MAX_CHARS_PER_WORD

//TODO list (maybe)
//1. to the symbol table add (scope info, type[of what?], length[of what?])
//2. to the intermediate file add (pointers to Opcode Table, and pointer to Symbol Table)
//3. convert the symbol table to something actually efficient (Ideally we use a dynamic Hash Table)
//4. get reliable length from getline
//5. improve "removeSpacesBack" to also remove the newline character [it should be doing this now but my prints indicate it isnt]

#pragma once

#define min(a,b) (((a) < (b)) ? (a) : (b))

//constants that will eventually be used throughout code
#define MAX_CHARS_PER_WORD 100
#define MAX_SYMBOLS 500
#define MAX_SYMBOL_SIZE 7 //6 spots and null terminator
#define OPCOUNT_COUNT 25
#define MAX_OPERATION_SIZE 5 //4 spots and null terminator
#define MAX_DIRECTIVE_SIZE 6 //5 spots and null terminator
#define MAX_OPCODE_SIZE 3 //2 spots and null terminator

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

//---other prototypes
int validLabel(char* label);
int labelFound(char* line);
int isDirective(char* mneumonic);

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
	char* value; //size of 2 because that is how much we always need
};
charToChar opCodeTbl[OPCOUNT_COUNT];

void pass1(char* filename)
{
	printf("running pass 1\n");

	//---testing area start

	//---testing area end

	//---Stream Inits
	FILE *ourSourceFile;
	FILE *ourIntermediateFile;

	//NOTE: all errors should be passed in a single run (errors should not be printed but instead written to the intermediate file)

	//place SOURCE in stream, make sure SOURCE file opens for reading properly
	ourSourceFile = fopen(filename, "r");
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
		if (startFound != 0) {

			//place INTERMEDIATE in stream, mae sure INTERMEDIATE file opens for writing properly
			ourIntermediateFile = fopen("./intermediate.txt", "w");
			if (ourIntermediateFile != NULL) {

				//-------------------------BETWEEN START and END
				int LOCCTR = 0;
				int endFound = 0;
				//NOTE: we use a do while because the line that is currently in the "buffer" is the first line (the one with the START directive)
				do {
					stringToLower(&line); //remove case sensitivity

					if (isBlankLine(line) != 1) 
					{
						//INT FILE:  [1]copy, [2]locctr, [3]mnemonics[operations](looked up)[directive], [4]operand(looked up), [5]errors, [\n]

						printf("sourceLine '%s'", line); //[1]

						if (line[0] == '.') //we are handling a comment
							printf("\n\n\n\n?\n"); //[2] -> [5][\n] (because we must retain format for quick accessing in Phase 3)
						else //we are NOT handling a comment
						{
							if (labelFound(line) == 1) //we have a label (but is it valid)
							{
								label = processFirst(&line);
								int validResult = validLabel(label);
								int addResult;
								switch (validResult)
								{
									case 1: //LABEL is valid (add to symbol table)
										addResult = addSYMTBL(label, -1);
										switch (addResult)
										{
											case 1: break;
											case 0: printf("ERROR --- duplicate label\n"); break;
											case -1: printf("ERROR --- label too long\n"); break;
											case -2: printf("ERROR --- symbol table is full (exceeded 500 symbol limt)"); break;
											default: break;
										}
										break;
									case 0: printf("ERROR --- invalid label (first value is not a alphabetic value)\n"); break;
									case -1: printf("ERROR ---- invalid label (is too long)\n"); break;
									default: break;
								}
							}
							else 
							{
								//the label field must equal something so we can print it
								label = malloc(sizeof(char));
								label = '\0';
							}

							//NOTE: by now we processed the label IF there was one -OR- had an ERROR added to it

							printf("location counter '%x'\n", LOCCTR); //[2]

							//make sure there are still things to process
							if (line[0] == '\0') 
							{
								printf("Mnemonic 'N/A'\n"); //[3]
								printf("Operand 'N/A'\n"); //[4]
								printf("ERRORS\n"); //[5]
								printf("?\n"); //[\n]
							}
							else //we have an mneumonic but is it valid?
							{ 
								mneumonic = processFirst(&line);
								int result = getOperationIndex(mneumonic);
								char* mneumonicCode = malloc(MAX_OPCODE_SIZE * sizeof(char));

								if (result != -1) //we have this operation
								{
									mneumonicCode = opCodeTbl[result].value;

									//for everything except rsub read in a operand
									if (strcmp(mneumonic, "rsub") != 0) {
										operand = processFirst(&line);

										//TODO... check operand validity
									}
									else
									{
										//the operand field must equal something so we can print it
										operand = malloc(sizeof(char));
										operand = '\0';
									}
								}
								else //check if we have a directive
								{
									if (isDirective(mneumonic) == 1) 
									{
										mneumonicCode = malloc(MAX_DIRECTIVE_SIZE * sizeof(char));
										operand = processFirst(&line);

										//TODO... check operand validity

										if (strcmp(mneumonic, "start") == 0)
										{
											if (startFound <= 1) { //its the first time finding a start directive

												//START n 	Program is to be loaded at location n (given in hexadecimal)

												startFound++;
											}
											else {
												printf("ERROR --- you have located an extra start directive");
											}
										}
										else if (strcmp(mneumonic, "end") == 0) {

											//END label 	Physical end of program; label is first executable program statement

										}
										else if (strcmp(mneumonic, "byte") == 0) {

											//BYTE v 		Stores either character strings (C'...') or hexadecimal values (X'...')

										}
										else if (strcmp(mneumonic, "word") == 0) {

											//WORD v 		Stores the value v in a WORD

										}
										else if (strcmp(mneumonic, "resb") == 0) {

											//RESB n 		Reserves space for n bytes

										}
										else if (strcmp(mneumonic, "resw") == 0) {

											//RESW n 		Reserves space for n words (3n bytes)

										}
									}
									else {

										//the operand field must equal something so we can print it
										operand = malloc(sizeof(char));
										operand = '\0';

										mneumonicCode = "--"; //set our translated non existed mneumonic value
										printf("ERROR --- invalid Op Code -or- Directive\n");
									}								
								}

								printf("Op Code -or- Directive '%s'\n", mneumonicCode); //[3]
								//NOTE: everything except operation RSUB has 1 operand (with different qualifications)
								printf("Operand '%s'\n", operand); //[4] (if we had operation -or- a mneumonic this is also taken care of)
								comment = processRest(&line);
								printf("Comment '%s'\n", comment); //[5]
								printf("?\n"); //[\n]
							}

							LOCCTR += 3;
						}
					}
					//ELSE... we ignore this blank line

				} while (getline(&line, &len, ourSourceFile) != -1 && endFound == 0);

				//-------------------------AFTER END

				//if we stopeed reading the file because END was found
				if (endFound != 0) {

				}
				else
					printf("ERROR --- this file does not have a END directive\n"); //NOTE: this error is not written to the intermediate file because it doesnt technically apply to a particular line because the line doesnt exist

				fclose(ourIntermediateFile); //close our intermediate file after writing to it
			}
			else //INTERMEDIATE did not open properly
				printf("ERROR --- INTERMEDIATE file did not open properly\n"); //NOTE: these errors can not be writtent to the intermediate file because there is none
		}
		else
			printf("ERROR --- this file does not have a START directive\n"); //NOTE: this error is not written to the intermediate file because the intermediate file contains errors in code... but there is techincally no code
		int endFound = 0;

		fclose(ourSourceFile); //close our source file after reading it
	}
	else //SOURCE did not open properly
		printf("ERROR --- SOURCE did not exist -OR- did not open properly\n"); //NOTE: these errors can not be writtent to the intermediate file because there is none
}

//-------------------------EXTRA PROCS-------------------------

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

	int nullTermIndex = min(strlen(src) - 1, destI);
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

	int nullTermIndex = min(strlen(src) - 1, destI);
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

int addSYMTBL(char* key, int value) { //returns 1 if success, 0 if value must be set, -1 if key to long, -2 if symbolTbl full

	if (emptyIndex < MAX_SYMBOLS) {
		if (strlen(key) < MAX_SYMBOL_SIZE) {
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
	else
		return -2;
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