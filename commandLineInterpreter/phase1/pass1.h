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

I Know We Need
(1) Constant Symbol Table Size of 500
(2) Ignore anything above START or END ...TODO...
(3) Order of Command in Assembly is (Label|Operation|Operand|Comments)
	{label}instruction {operand{,X}} {comment}
		-items in curly braces are optional
(4) a line may just have a comment or a label
	-a label must begin in column 1 (assuming columns are 1 based)
	-blank column1 means we have no label
(5) a period in column 1 indicates the entire line is a comment
(6) blank lines are ignored
(?) The only restriction is that there must be at least one space or tab separating each field present. The
only exception is if indexing is requested -- don't put a space between the comma and the X.

I am Assuming: (should find out these details if possible)
(1) Everything in "TODO list (maybe)" below is not a requirement
(2) (a) intermediate file (b) listing file (c) object file -> Dont Require a Specific File Extension (using .txt)
(3) we dont care for case sensitivity
*/

//TODO list (must)
//1. remove limit MAX_CHARS_PER_WORD
//2. maybe read in the Op Code Table

//TODO list (maybe)
//1. to the symbol table add (scope info, type[of what?], length[of what?])
//2. to the intermediate file add (pointers to Opcode Table, and pointer to Symbol Table)
//3. convert the symbol table to something actually efficient (Ideally we use a dynamic Hash Table)
//4. get reliable length from getline

#pragma once

//constants that will eventually be used throughout code
#define MAX_CHARS_PER_WORD 100
#define MAX_SYMBOLS 500
#define MAX_SYMBOL_SIZE 7 //6 spots and 1 null terminator
#define OPCOUNT_COUNT 25
#define LARGEST_OPCODE_LENGTH 5 //4 spots and null terminator

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

//---prototypes for string processing
char* processFirst(char** l);
char* subString(char* src, int srcIndex, int strLength);
void subStringRef(char** source, int srcIndex, int strLength);
int removeSpacesFront(char** line);

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
int containsOpCode(char* operand);
int getOpCodeIndex(char* operand);
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
		//place INTERMEDIATE in stream, mae sure INTERMEDIATE file opens for writing properly
		ourIntermediateFile = fopen("./intermediate.txt", "w");
		if (ourIntermediateFile != NULL) {

			fillOpCodeTable();

			int startFound = 0; // 1 for true; if found then start our regular pass1 process; else consider it a comment (space saving, multiline type)
			int endFound = 0; // 1 for true; if found the end our regular pass1 process... consider lines a comment (space saving, multiline type); else continue regular pass1 process

			char *line = NULL; //NOTE: this does not need a size because getline handle all of that
			size_t len = 0; //TODO... todo list item
			int currentLineToFill = 1;

			while (getline(&line, &len, ourSourceFile) != -1) {

				printf("Retrieved line %i with inncacurate size %zu\n", currentLineToFill,len); 
				printf("'%s'\n", line);

				//---test code
				
				char *p1 = processFirst(&line);
				printf("after removing string '%s' we have line '%s'\n", p1, line);

				removeSpacesFront(&line);
				printf("line no longer with space in the front '%s'\n", line);

				//---test code
				
				currentLineToFill++;
			}

			//-------------------------BEFORE START (startFound = 0, endFound = 0)

			//-------------------------BETWEEN START and END (startFound > 0, endFound = 0;

			//TODO... handle directives START, END, BYTE, WORD, RESB, RESW
			//TODO... things to watch our for in notes doc

			//-------------------------AFTER END (startFound > 0, endFound > 0)

			fclose(ourIntermediateFile); //close our intermediate file after writing to it
		}
		else //INTERMEDIATE did not open properly
			printf("ERROR --- INTERMEDIATE file did not open properly\n"); //NOTE: these errors can not be writtent to the intermediate file because there is none

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
			line = '\0'; //nothing useful is left in the line
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

//-------------------------String Parsing and Tokenizing Functions 

char* subString(char* src, int srcIndex, int strLength) {

	int srcI = srcIndex;
	int destI = 0;

	char* dest = malloc(MAX_CHARS_PER_WORD * sizeof(char));

	while (strLength > 0) {
		strLength--;
		dest[destI] = src[srcI];
		destI++;
		srcI++;
	}

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
	src[destI + 1] = '\0';
}

int removeSpacesFront(char** line) { //returns how many spaces where removed

	char* l = *line;

	//var init
	int lineID = 0;

	//ignore anything that is a space
	while (isspace(l[lineID]) != 0 && l[lineID] != '\0') {
		lineID++;
	}

	//make sure we have string left to check after getting rid of all spaces
	if (line[lineID] == '\0')
		line = '\0'; //nothing useful is left in the line

	int sizeOfLine = (strlen(l) - lineID);
	subStringRef(line, lineID, sizeOfLine);

	return lineID;
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
		opCodeTbl[index].key = malloc(LARGEST_OPCODE_LENGTH * sizeof(char)); //allocate memory for char
		opCodeTbl[index].value = malloc(2 * sizeof(char));

		//allocate memory for temporary values
		char* theKey = malloc(LARGEST_OPCODE_LENGTH * sizeof(char));;
		char* theValue = malloc(2 * sizeof(char));

		switch (index)
		{
		case 0: //	ADD m 				18 		A <-(A)+(m..m + 2)
			theKey = "ADD";	theValue = "18";	break;
		case 1: //	AND m 				58 		A <-(A) & (m..m + 2)[bitwise]
			theKey = "AND";	theValue = "58";	break;
		case 2: //	COMP m 				28 		cond code <-(A) : (m..m + 2)
			theKey = "COMP";	theValue = "28";	break;
		case 3: //	DIV m 				24 		A <-(A) / (m..m + 2)
			theKey = "DIV";	theValue = "24";	break;
		case 4: //	J m 				3C 		PC <-m
			theKey = "J";	theValue = "3C";	break;
		case 5: //	JEQ m 				30 		PC <-m if cond code set to =
			theKey = "JEQ";	theValue = "30";	break;
		case 6: //	JGT m 				34 		PC <-m if cond code set to >
			theKey = "JGT";	theValue = "34";	break;
		case 7: //	JLT m 				38 		PC <-m if cond code set to <
			theKey = "JLT";	theValue = "38";	break;
		case 8: //	JSUB m 				48 		L <-(PC); PC <-m
			theKey = "JSUB";	theValue = "48";	break;
		case 9: //	LDA m 				00 		A <-(m..m + 2)
			theKey = "LDA";	theValue = "00";	break;
		case 10: //	LDCH m 				50 		A[rightmost byte] <-(m)
			theKey = "LDCH";	theValue = "50";	break;
		case 11: //	LDL m 				08 		L <-(m..m + 2)
			theKey = "LDL";	theValue = "08";	break;
		case 12: //	LDX m 				04 		X <-(m..m + 2)
			theKey = "LDX";	theValue = "04";	break;
		case 13: //	MUL m 				20 		A <-(A) * (m..m + 2)
			theKey = "MUL";	theValue = "20";	break;
		case 14: //	OR m 				44 		A <-(A) | (m..m + 2)[bitwise]
			theKey = "OR";	theValue = "44";	break;
		case 15: //	RD m 				D8 		A[rightmost byte] <-data from device specified by(m)
			theKey = "RD";	theValue = "D8";	break;
		case 16: //	RSUB 				4C 		PC <-(L)
			theKey = "RSUB";	theValue = "4C";	break;
		case 17: //	STA m 				0C 		m..m + 2 <-(A)
			theKey = "STA";	theValue = "0C";	break;
		case 18: //	STCH m 				54 		m <-(A)[rightmost byte]
			theKey = "STCH";	theValue = "54";	break;
		case 19: //	STL m 				14 		m..m + 2 <-(L)
			theKey = "STL";	theValue = "14";	break;
		case 20: //	STX m 				10 		m..m + 2 <-(X)
			theKey = "STX";	theValue = "10";	break;
		case 21: //	SUB m 				1C 		A <-(A)-(m..m + 2)
			theKey = "SUB";	theValue = "1C";	break;
		case 22: //	TD m 				E0 		Test device specified by(m)
			theKey = "TD";	theValue = "E0";	break;
		case 23: //	TIX m 				2C 		X <-(X)+1; compare X and (m..m + 2)
			theKey = "TIX";	theValue = "2C";	break;
		case 24: //	WD m 				DC 		Device specified by(m) <-(A)[rightmost byte]
			theKey = "WD";	theValue = "DC";	break;
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

int containsOpCode(char* operand) {
	if (getOpCodeIndex(operand) != -1)
		return 1;
	else
		return 0;
}

int getOpCodeIndex(char* operand) {
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