
//NOTE: I am not going to go through the hassle of changing file extensions for now

/*
this will create the

-symbol table
-intermediate file

to be used in pass 2

by
1. assigning address to all statements
2. saving addresses assign to label in symbol table
3. performing some processing of assembler directives
*/
void pass1()
{ 
	printf("pass 1 is being called \n");

	//TODO... remember to make seperate proc. to (1) read file (2) handle tokenizing of source file
	//Remember SIC asm format is like this
	//label(first column) | operation(mnemonic or directive) | operand(symbol value or hex address)  | comment

	//TODO... remember that I need to be able to ignore multiple line comments on top or bellow the source file
	//TODO... figure out how to determine what is or isnt in the first column... atleast 1 tab or 1 space or what?
	//TODO... figure out how to handle comment lines
	//TODO... where to hold opcodes
	
	/*
	the opcode table we are using should be contained as a static hash table
	but for time constrainst I will just do a regular search
	*/

	/*
	the symbol table we are creating should include (best organized as a dynamic hash table)
	-symbol
	-associated value (usually and address)

	May also include
	*type
	*length
	*scope info
	*/

	/*
	the intermediate file we are creating should include
	-source statement
	-address
	-mnemonic or its value
	-operand(s)
	-errors in statement

	May also include
	*pointers to OPTAB
	*pointer to SYMTAB
	*/

	//NOTE: LOCCTR (aka location counter) is in hex
	//as each statement is examined its length is added to LOCCTR (for SIC just 3 bytes, for SIX/XE it depends)

	/*
	My Psuedo Code

	read first input line
	if(opcode == 'start'){
		save #{Operand} as starting address
		initialize LOCCTR to starting address
		writeline to intermediate file
		read next input lineand write to intermediate file
	}
	else
		initialize LOCCTR to 0

	while( opcod != end) 
		if (this is not a comment line)
			

	write las line to intermediate file
	save (LOCCTR - starting address) as program length

	*/
}
