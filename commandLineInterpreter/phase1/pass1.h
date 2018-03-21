/*
Programmer: Bryan Cancel
Last Updated: 3/20/18

Description:
Pass 1 will create:
-symbol table
-intermediate file
that will be used in pass 2

Deliverable:
1. well documented source listing
2. two listings of assembler language source files (one with error, one without errors)
3. a listing of the symbol table produced
4. Copies of both intermediate files (for the files in 2)
5. @toplevel/3334/phase2
with only source files in said folder

I am Assuming:
(1) you dont need the (a) intermediate file (b) listing file (c) object file... with any specific file extension --- I will use .txt for all... for now...
(2) I can write whatever I want before START and after END and it should not affect file (c), but it will appear as a comment
*/

//TODO list (details)
//1. chop up strings by the isspace delimter instead of just and actual space
//2. get reliable length from getline
//3. set a reliable limit from getline or make sure it has none (so lines can be as long as possible)
//4. try to remove limits created by the characters if possible
//5. find out if case sensitive matter on directive and mneumonics

#pragma once

//constants that will eventually be used throughout code
#define MAX_CHARS_PER_LINE 100
#define MAX_CHARS_PER_WORD 100

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

//prototypes
char* processFirst(char** l);
char* processRest(char** l);
char* subString(char* src, int srcIndex, int strLength);
void subStringRef(char** source, int srcIndex, int strLength);
int removeSpacesFront(char** line);

void pass1(char* filename)
{
	printf("running pass 1\n");

	FILE *ourSourceFile;
	FILE *ourIntermediateFile;
	FILE *ourSymbolTableFile;

	//NOTE: all errors should be passed in a single run (errors should not be printed but instead written to the intermediate file)

	//place SOURCE in stream, make sure SOURCE file opens for reading properly
	ourSourceFile = fopen(filename, "r");
	if (ourSourceFile != NULL)
	{
		//place INTERMEDIATE in stream, mae sure INTERMEDIATE file opens for writing properly
		ourIntermediateFile = fopen("./intermediate.txt", "w");
		if (ourIntermediateFile != NULL) {

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
				printf("line no longer with space in the front '%s'\,", line);

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

//----------Handle Reading

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
			return;
		}

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
		int sizeOfLine = (MAX_CHARS_PER_LINE - lineID);
		subStringRef(&line, lineID, sizeOfLine);

		return first;
	}
	else
		return '\0';
}

char* processRest(char** l) {
	return 'i';
}

//---Helper Functions 

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

//---Extra Functions

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

	int sizeOfLine = (MAX_CHARS_PER_LINE - lineID);
	subStringRef(line, lineID, sizeOfLine);

	return lineID;
}

//----------Symbol Table Print (Symbol | Location)