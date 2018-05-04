/*
Programmer: Bryan Cancel
Last Updated: 4/27/18

1. Appropriate data types and variables to interact with the architecture of the SIC
computer. Some of these will be given through the header file sic.h
2. An Execute procedure which calls the actual SIC simulator.
3. A Load procedure which will load a SIC machine language program from a file into
memory. The format of the file should be as described in the text.
4. A Dump procedure which will display the contents of a portion of memory
requested by the user. This should display the memory address, followed by the
contents (bytes). Display the contents horizontally in groups of 16 or so to have
more locations show on the screen. This is very useful to see if your other
procedures are working properly. If the display will be more than one screenful,
have the program pause and wait for the user.
5. Have the Assemble procedure call the Assembler written in phases two and three, if
you haven't already done so.
6. [OPTIONAL] A Debug procedure which will stop before carrying out each
instruction (a call to the SIC simulator), and wait for directions from the user.
These directions will be single letter commands to print the contents of all the
registers (A, X, L, PC, SW). The user should also be able to display and change the
contents of the index register, X, the linkage register, L, the accumulator, A, and any
memory location. The user should be able to request the execution of the next
instruction (single step), or to stop debugging and allow the program to execute to
completion, or to halt the execution and return to the command level.

Deliverables
1. A documented listing of the program.
2. A listing of the machine language data file you used for testing (good & bad).
3. Sample runs, demonstrating that all instructions work. Include a dump of memory
after the load, and after execution of your program.
*/

#pragma once

#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

ADDRESS startAddress;
ADDRESS endAddress;

void loadExt(char* sourceFileName) 
{
	FILE *testFile = fopen("convTest.txt", "w");
	for (int i = 0; i < 256; i++) {
		int dec1 = i;
		char* hex1 = b10Int_To_b16Str(dec1, 0);
		unsigned char encoding = hexToLetter(hex1);
		unsigned char* en = malloc(2 * sizeof(unsigned char));
		en[0] = encoding;
		en[1] = '\0';
		char* hex2 = letterToHex(encoding);
		int dec2 = b16Str_To_b10Int(hex2);
		fputs(strCat(hex1,strCat(" == ",strCat(en,strCat(" == ",strCat(hex2, "\n"))))), testFile);
	}
	fclose(testFile);

	char* objectFileName = strCat(subString(sourceFileName, 0, strlen(sourceFileName) - 4), "Object.txt");

	printf("\nLoading Object File: '%s'\n", objectFileName);

	FILE *ourObjectFile = fopen(strCat("./", objectFileName), "r"); //wipes out the file
	if (ourObjectFile != NULL) 
	{
		//create the variables that will be used to read in our file
		char *line = NULL; //NOTE: this does not need a size because getline handle all of that
		size_t len = 0;

		while (getline(&line, &len, ourObjectFile) != -1)
		{
			if (line[0] == 'T')  //text record
			{
				//-----get the address
				char* address = subString(line, 1, 6);
				ADDRESS addressAdd = strtol(address, NULL, 16);

				//-----get the size
				char* size = subString(line, 7, 2);
				int sizeInt = strtol(size, NULL, 16);

				//-----remove what we just extracted from the line
				char* theLine = subString(line, 9, strlen(line) - 9);

				if(startAddress == 0)
					startAddress = addressAdd;
				
				//loop through all of our instructions and load them into memory a WORD at a time (2 chars + null terminator)
				while (1 == 1)
				{
					int index;
					if (isBlankLine(theLine) == 0)
					{
						char* byte = subString(theLine, 0, 2); //one byte is 2 characters...
						stringToUpper(&byte);
						
						//convert those 2 characters to their perspective actual character encoding as 1 character
						unsigned char *res = malloc(2 * sizeof(unsigned char));
						res[0] = hexToLetter(byte);
						res[1] = '\0';
						//printf("'%s'", byte);
						//printf("->");
						printf("'%s'", res);
						PutMem(addressAdd, res, 0);

						//unsigned char *aByte = malloc(2 * sizeof(unsigned char));
						//aByte[0] = ' ';
						//aByte[1] = '\0';
						//GetMem(addressAdd, &aByte, 0);
						//printf("'%c' == '%c'\n", res[0], aByte[0]);

						theLine = subString(theLine, 2, strlen(theLine) - 2);
						addressAdd++;
					}
					else
						break;
				}
				printf("\n\n");

				endAddress = addressAdd;
			}
			else if (line[0] == 'E') //end record
				;
			else //header record
				;
			//freeMem(&line);
		}

		printf("BASE 16: '%x' -> '%x'\n\n", startAddress, endAddress);

		printf("LOAD COMPLETE\n\n");
	}
	else
		printf("ERROR --- OBJECT file did not open properly or does not exist\n");
}
	
void dumpExt(char* start, char* end) 
{
	printf("dumping %s and %s\n", start, end);

	if (isNumber16(start) == 1 && isNumber16(end) == 1) 
	{
		printf("Numbers Read As Base 16\n");

		ADDRESS currAdd = strtol(start, NULL, 16);
		ADDRESS endAdd = strtol(end, NULL, 16);

		unsigned char *aByte = malloc(2 * sizeof(unsigned char));
		aByte[0] = ' ';
		aByte[1] = '\0';

		printf("the numbers again '%i' and '%i'\n", currAdd, endAdd);
		
		while (currAdd < endAdd)
		{
			GetMem(currAdd, &aByte, 0);
			unsigned char* hex = letterToHex(aByte[0]);
			printf("'%s'", aByte);
			currAdd++;
		}
		printf("\n");
		
		//extern void GetMem(ADDRESS, BYTE*, int);
	}
	else
		printf("ERROR --- Your number are not in base 16\n");
}