
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

I am Assuming
(1) the first column is the group of words at the begining of the line that have NO SPACE, NO TABBING before their first letter
	[*] this was done because of the format you told us
		(you told us formatting was)
			[*]{label} instruction {operand{,X}} comment
		(I implied)	
			[*]---NO SPACES OR TABS---{label}--arbitrary number of space (ANOF)---instruction---(ANOF)---{operand{,X}}---(ANOF)---c---(ANOF)---omm---(ANOF)---ent

(2) its okay for the symbol table to be stored in a file for the sake of simplicity (speed is terrible I know)
(3) we dont having different scopes for labels, since it was never indicated
(4) Errors are passed to pass 2 through the intermediate file
(5) i might have random text before START and After END
(6) we are using the sicengine.c file edited by the professor (Egle) to run directives, and get opcode hex equivalents
(7) Symbols are only Labels... because i didnt see anything that told me anything differently

I have decided to
(1) For Symbol Table
	[1] I will store it in a file (because of time constraints to design fool proof data struct)
	[2] with only: (done for simplicity)
		(a) symbol
		(b) address or other
	[3] to avoid the need for tokenizing symbol and address will be on seperate line
		(*) symbols will be on odd line, addresses will be on even ones
(2) For Intermediate File
	[a] include random text before START and After END
		(1) these come in 5 lines at time to follow the base 5 thing bellow
		(2) every single one of these lines will now begin with a '.' to indicate that its being ignored like a comment
	[b] use a base 5 thing to access the 5 lines quickly
		EX: first line = [(5^1)+1] = 6 = FL , FL+1=7, FL+2=8, FL+3=9, FL+4=10 = Last Line
	[c] we handle comments the same way we handle [a], EXCEPT we dont add '.' because it will already have one

I was told
(1) blank lines, spaces, or tabs are ONLY used to seperate each field present
	[*] if indexing is requested -- dont put a space between the comma and the X
(2) assembler will not be case sensitive
(3) all errors should be flagged in a single run and therefore pass 1 should not terminate abnormally
(4) our formatting will be "{label} instruction {operand{,X}} comment"
(5) a Label MAY consist of JUST a:
	[a] comment
	[b] label
(6) Instruction operands must be in the form
	[a] operand
	[b] operand,X
	[*] where the operand is either: 
		(1) a symbol that is used as a label in the source program
			[*] can only be 6 alpha numeric chars long
			[*] MUST start with a letter
		(2) actually a hex address
			[*] hex addresses that would begin with 'A' through 'F' MUST have a leading '0' to distinguish them from a label
*/

/*
	TODO for future expandability and speed
	(1) IF the program is too large... I beleive we use linkage record to link to a different file and finish the program... maybe...
	(2) Seperate Proc. to "Read File"
	(3) Seperate Proc. for file Tokenizing (Seperation of words - this is file type specific but you can still seperate it from the main file)
		'File' Types [a] Command Line [b] Source File [c] Intermediate File
	(4) make opcode table static hash
	(5) make symbol table dynamic hash
	(6) take every to lower or to upper only when needed instead of at the begining
*/
void pass1()
{ 
	printf("pass 1 is being called \n");
	
	//the BYTE directive may take two types of operands: character string is 30 characters
	//the max lenth of hex string is 16 bytes (32 hex digits)
	//the number of hex digits must be even

	/*
		THINGS TO WATCH OUT FOR
			*Duplicate Labels
				-are we going to have scoping rules?
			*Illegal Label (IF)
				-larger than 6 alphanumeric chars
				-starts with a digit
			*Illegal Operation
			*Missing or Illegal operand on data storage directive
			*Missing or Illegal operand on START directive
			*Missing or Illegal operand on END directive (dont need to check if symbol is defined)
			*Too many symbols in source program
				-we can have a max of 500 labels
				-as for other symbols i dont know what this implies...
			*Program Too Long (larger than 32,768 bytes of memory or 7FFFh)
	*/

	//NOTE: LOCCTR (aka location counter) is in hex
	//as each statement is examined its length is added to LOCCTR (for SIC just 3 bytes, for SIX/XE it depends)

	/*
	My Psuedo Code... will be here...

	*/

	//because the instructor said so after we are done we print the symbol table
}

#pragma region Extra Symbol Table and Intermediate File Notes

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

#pragma endregion

