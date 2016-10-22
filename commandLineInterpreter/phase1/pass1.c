
/*
Programmer: Bryan Cancel
Date: 10/21/16

Pass 1 will create:
-symbol table
-intermediate file
that will be used in pass 2

NOTE: notes on what these files should and can include are at the bottom

by
1. assigning address to all statements
2. saving addresses assign to label in symbol table
3. performing some processing of assembler directives
*/

/*
	TODO for future expandability
	(1) IF the program is to large... I beleive we use linkage record to link to a different file and finish the program... maybe...
	(2) Seperate Proc. to "Read File"
	(3) Seperate Proc. for file Tokenizing (Seperation of words - this is file type specific but you can still seperate it from the main file)
		'File' Types [a] Command Line [b] Source File [c] Intermediate File
	(4) make opcode table static hash
	(5) make symbol table dynamic hash
*/
void pass1()
{ 
	printf("pass 1 is being called \n");

	//SIC asm format: label(first column) | operation(mnemonic or directive) | operand(symbol value or hex address) | comment

	/*
		THINGS TO WATCH OUT FOR
			*Duplicate Labels
			*Illegal Label... TODO... find out when this would happen
			*Illegal Operation
			*Missing or Illegal operand on data storage directive
			*Missing or Illegal operand on START directive
			*Missing or Illegal operand on END directive (dont need to check if symbol is defined)
			*Too many symbols in source program... TODO... find out when this would happen
			*Program Too Long (larger than 32,768 bytes of memory or 7FFFh)
	*/

	//when an Error is flagged the assembler should keep processing the source file so all errors are flagged in a single run... aka it should NOT terminate until finished
	//these errors shouldn't be printed but they should be passed to the second pass... TODO... through intermediate file?

	//TODO... remember that I need to be able to ignore multiple line comments on top or bellow the source file
	//TODO... figure out how to determine what is or isnt in the first column... atleast 1 tab or 1 space or what?
	//TODO... figure out how to handle comment lines
	//TODO... where to hold opcodes
	//TODO... figure out if spaces matter for anything except seperating first column from rest
	
	/*
	the opcode table we are using should be contained as a static hash table
	but for time constrainst I will just do a regular search on... data struct to be determined...
	*/

	

	//NOTE: LOCCTR (aka location counter) is in hex
	//as each statement is examined its length is added to LOCCTR (for SIC just 3 bytes, for SIX/XE it depends)

	/*
	My Psuedo Code... will be here...

	*/

	//because the instructor said so after we are done we print the symbol table
}

//opcode should be static hash, symbol table should be dynamic hash

/*
the symbol table we are creating should include (best organized as a dynamic hash table)
-symbol
-associated value (usually and address)

May also include (not really needed, so im not really adding these)
*type
*length
*scope info
*/

/*
the intermediate file we are creating should include
-copy of source line
-value of location counter
-values of mnemonics used (since they had to be looked up)
-operands since we had to get them (With a space between each... or...?)
-error messages(better to save these as codes for easy look up later in pass 2)

May also include (not really needed, so im not really adding these)
*pointers to OPTAB
*pointer to SYMTAB
*/