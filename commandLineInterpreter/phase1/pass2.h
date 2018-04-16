/*
this will create the

-listing file
-object file

by:
1. converting mnemonics and symbols to machine code
2. converting constants
3. processing remaining assembler directives
4. writing maching code to both files
*/

#pragma once
#include "pass1.h"

void pass2(char *sourceName, char * intermediateName, char **_firstLabel, char **_lastLabel, int programLength)
{
	char *firstLabel = *_firstLabel;
	char *lastLabel = *_lastLabel;

	printf("source name is '%s' intermediate file name is '%s'\n", sourceName, intermediateName);
	printf("first label is '%s', last label is '%s', and the program length is %i\n", firstLabel, lastLabel, programLength);

	//buildOpCodeTable();

	printf("READY FOR PASS 2\n");

	/*
	the listing file (that contains the result of assembly) we are creating should include
	-address of statement
	-machine code equivalent
	-source statement (including comments)
	-errors associated with each statement
	*/

	/*
	the object file we are creating should include
	-results of assembly
	-Format varies according to designer
	-Usually read by a linker/loader program
	-Several record types (this also can vary)
	-header record
	-linkage records
	-code records
	-end record
	*/

	//TODO figure out what to do with linkage records...
}
