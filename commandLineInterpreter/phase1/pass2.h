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

void pass2()
{
	printf("pass 2 is being called magically because we have its protoype in assemble.h already \n");

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
