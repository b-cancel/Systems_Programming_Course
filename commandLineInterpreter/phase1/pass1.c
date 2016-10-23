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
----------
(8) each line is not longer than "MAX_CHARS_PER_LINE"
(9) the largest size of (label) (instruction) (operand-size of 6) (comment-size of 100)

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
*/

//constants that will eventually be used throughout code
#define MAX_CHARS_PER_LINE 100;

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//prototypes for helper funs used bellow
void handleComment(char* commentLine);
char* getFirstWord(char* codeLine, int length); //this returns the first word (ignore space in front)
char* removeFirstWord(char* codeLine); //this should remove the first word from codeline and return codeline
void printSymbolTableFile();

void pass1()
{ 
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
			*finding another START Directive

			//the BYTE directive may take two types of operands: character string is 30 characters
			//the max lenth of hex string is 16 bytes (32 hex digits)
			//the number of hex digits must be even

			//NOTE: LOCCTR (aka location counter) is in hex
			//as each statement is examined its length is added to LOCCTR (for SIC just 3 bytes, for SIX/XE it depends)
	*/

	printf("pass 1 is being called \n");	

	FILE *ourSourceFile;
	FILE *ourIntermediateFile;

	//place our files in their perspective streams and make sure they opened properly
	if (fopen_s(&ourSourceFile, "./source.txt", "r") == 0 && fopen_s(&ourIntermediateFile, "./intermediate.txt", "w") ==0)
	{
		//TODO have this transition happens once and only once (right now we are constantly checking for the two things below)
		/*
		these two allow you to have "comments" before START and after END
			STAGE1(before START) startFound=0; endFound =0
			STAGE2(after START before END) startFound=1; endFound=0;
			STAGE3(after END) startFound=1; endFound=1;
		*/
		int startFound = 0; // 1 for true; if found then start our regular pass1 process; else consider it a comment (space saving, multiline type)
		int endFound = 0; // 1 for true; if found the end our regular pass1 process... consider lines a comment (space saving, multiline type); else continue regular pass1 process

		int currentLineToFill = 0; //this keeps track of what line on the in the intermediate file

		char startingAddress = 0x0;
		char LOCCTR = 0x0; //location counter

		char sourceLine[100]; //this keeps track of the line being read in
		char errors[100] = ""; //this is going to keep track of all the errors (errors separated by a horizontal line '|')

		while (fgets(sourceLine,100, ourSourceFile) != NULL) //while end of file HASN'T been reached
		{
			printf("in while loop\n");

			char tempSourceLine[100] = ""; //here we will store the line as we cut it up because we want to keep the original

			//----------setup our containers for our formatting: "{label} instruction {operand{,X}} comment"
			char label[10] = "";
			char instruction[10] = "";
			//FORM: operand OR operand,X
			//we are using 7 chars in case we need to use the leading 0
			char operand[7] = ""; //(IF LABEL) MUST be (1) alphanumeric chars and (2) start with a letter (IF HEX ADDRESS) if start with a char 'A'-'F' must have a leading 0
			char comment[100] = "";

			//TODO make this some sort of function... its repeated twice so far in this file
			//----------make sure line doenst exceed our desired line char size, If so warn the user in intermediate file and empty
			if (!strchr(sourceLine, '\n'))     //newline not found in current buffer
			{
				strcat_s(errors, 100, "line too long-");
				while (fgetc(stdin) != '\n') {};	//discard chars until newline
			}
			sourceLine[strlen(sourceLine) - 1] = '\0'; //set the last char in the sourceBuffer as a null terminator [precaution]

			//----------begin tokening the line
			if (startFound == 0) //if START directive NOT yet found... try to find it
			{
				
				printf("STAGE 1 \n");
				
				//TODO implement label restrictions
				//grab LABEL
				strncpy_s(label,10, getFirstWord(sourceLine,10),10);
			
				printf("LABEL: -%i- | INSTRUCT -%i- | OPERAND -%i- | COMMENT -%i- \n", strlen(label), strlen(instruction), strlen(operand), strlen(comment)); //for debuging
				printf("LABEL: -%s- | INSTRUCT -%s- | OPERAND -%s- | COMMENT -%s- \n", label, instruction, operand, comment); //for debuging

				//remove the first word so we can now search for the second
				strncpy_s(tempSourceLine, 100, removeFirstWord(sourceLine), 100);

				printf("left over: %n\n", tempSourceLine);

				printf("LABEL: -%i- | INSTRUCT -%i- | OPERAND -%i- | COMMENT -%i- \n", strlen(label), strlen(instruction), strlen(operand), strlen(comment)); //for debuging
				printf("LABEL: -%s- | INSTRUCT -%s- | OPERAND -%s- | COMMENT -%s- \n", label, instruction, operand, comment); //for debuging

				//TODO instruction restrictions
				//grab INSTRUCTION
				strncpy_s(instruction, 10, getFirstWord(tempSourceLine,10), 10);

				if (strcmp(instruction, "START")) //if we have NOW found START directive
				{
					//tell the program start has been found
					startFound == 1;

					//remove the first word so we can now search for the second
					strncpy_s(tempSourceLine, 100, removeFirstWord(sourceLine), 100);

					//grab OPERAND
					strncpy_s(operand, 10, getFirstWord(tempSourceLine, 10), 10);

					//grab comment
					strncpy_s(comment, 10, tempSourceLine, 10);

					//save #{Operand} as starting address
					startingAddress = operand;

					//initialize LOCCTR to starting address
					LOCCTR = startingAddress;
				}
				//else we are working with a implied comment before START

				printf("LABEL: -%s- | INSTRUCT -%s- | OPERAND -%s- | COMMENT -%s- \n", label, instruction,operand,comment); //for debuging
			}
			else if(endFound == 0) //if between START and END directive
			{
				printf("STAGE 2 \n");

				//----------split up the line

				if (isspace(sourceLine[0]) == 0) //we have a LABEL
				{
					//grab the label

					//search Symbol Table for Label
					
					//if Found then set ERROR - duplicate label
					//else inser (Label, LOCCTR) into Symbo Table
				}
				
				//grab instruction (aka OPCODE)

				//grab operand OR operand,X

				//rest of line is comment

				//----------decide what to do with opcode

				if (instruction == 0) //If the instruction is in the opcode table
				{
					//add 3 to the location counter LOCCTR

					//replace opcode with hex equivalent
				}
				else //if we are looking at a directive
				{
					if (strcmp(instruction, "END")) //if we have NOW found the END directive
					{
						endFound == 1;
					}
					else if (strcmp(instruction, "WORD"))
					{
						//add 3 to LOCCTR
					}
					else if(strcmp(instruction, "RESB"))
					{
						//add #[Operand] to LOCCTR
					}
					else if (strcmp(instruction, "RESW"))
					{
						//add 3*#[Operand] to LOCCTR
					}
					else if (strcmp(instruction, "BYTE"))
					{
						//find length of constants in bytes
						//add length to LOCCTR
					}
					else
					{
						//set error flag because the instruction isnt an opcode and insnt a directive
						
						if(strcmp(instruction, "START"))
							strcat_s(errors, 100, "# of START dir > 1-");
						else
							strcat_s(errors, 100, "Opcode/Directive not recognized-");
					}
				}

				
			}
			else //if END directive found
			{
				printf("STAGE 3 \n");

				handleComment(sourceLine);
				currentLineToFill++;
			}

			//----------For Writing To Intermediate File

			if (startFound == 1 && endFound == 0) //only write the five fields if we are within the assembly portion of the file
			{
				//make sure current line we are on is a line of base 5 (FIRST 5: (1,2,3,4,5) | (6,7,8,9,10) | (11,12,13,14,15)
				if (((currentLineToFill - 1) % 5) != 0) //if we aren't on the appropriate line... we adjust
				{
					//ex: if number is 9
					int extra = (5 - (currentLineToFill % 5)); //(5-4) = 1
					for (extra; extra > 0; extra-=1)
					{
						fprintf_s(ourIntermediateFile, "-----free space to keep formatting-----");
					}
					currentLineToFill += extra + 1; // 9 + extra(1) = 9+1 = 10 + 1 = 11 a.k.a. good spot to place new data
				}

				//write source line
				fprintf_s(ourIntermediateFile, "Source: %s",sourceLine);

				//write value of location counter
				fprintf_s(ourIntermediateFile, "LOCCTR: %s",LOCCTR);

				//write values of mneumonics (since they had to be looked up)
				fprintf_s(ourIntermediateFile, "Opcode: %s",instruction);

				//write values of operands since we had to get them (dont separate things between commas)
				fprintf_s(ourIntermediateFile, "Operand: %s",operand);

				//error messages if any - otherwise blank line
				fprintf_s(ourIntermediateFile, "Erros:  %s", errors);

			}
			else //we are working with a comment before START or after END
			{
				//if it doesnt have the comment indicator that give it the comment indicator
				if (sourceLine[0] != '.')
				{
					char temp[100] = ".";
					strcat_s(temp, 100, sourceLine);
					temp[strlen(temp) - 1] = '\0'; //add null terminator
					fprintf_s(ourIntermediateFile, "%s", temp);
				}
				else
				{
					fprintf_s(ourIntermediateFile, "%s", sourceLine);
				}	

				currentLineToFill++;
			}
		}

		//close our files
		fclose(ourSourceFile);
		fclose(ourIntermediateFile);
		//close our symbol table here
	}
	
	//After finished reading file, print symbol table
	printSymbolTableFile();
	
	
}

void printSymbolTableFile()
{
	//open symbol table file
	FILE *symtab;

	//if successfully opened symbol table file
	if (fopen_s(&symtab, "./symtab.txt", "r") == 0)
	{
		char symtabLine[100]="";

		while (fgets(symtabLine, 100, symtab) != NULL) //as long as eof isnt reached
		{
			
			if (!strchr(symtabLine, '\n'))     //if newline not found - we need to clip line
			{
				while (fgetc(symtab) != '\n' && !feof(symtab)) {};	//discard chars until newline
			}
			//this will not crash for lines that do not overflow because those lines will have a \n as a last char which we then replace for \0
			symtabLine[strlen(symtabLine)-1] = '\0'; //set null terminator

			printf("%s-\n", symtabLine);
		}
	}
	else
	{
		printf("there was an error when trying to print the symbol table");
	}

	//close symbol table file
	fclose(symtab);
}

void handleComment(char* commentLine)
{
	if (commentLine[0] != '.')//if there isnt a period in front of the comment, place one
	{

	}

	//add to intermediate file on next line
}

char* getFirstWord(char* codeLine, int length) //this returns the first word (ignore space in front)
{
	char theWord[100] = "";

	int i = 0;
	int wordLoc = 0;
	for (i; i < strlen(codeLine); i += 1) //loop through the codeLine
	{
		if (isspace(codeLine[i])!=0)
		{
			if (theWord[0] != '\0')
				break; //we have found our first word
			//else we are just getting rid of space in the front
		}
		else //a char was found so add it to the word
		{
			theWord[wordLoc] = codeLine[i];
			wordLoc++;
		}
	}

	printf("1 strlen %i\n",strlen(theWord));

	
	if (strlen(theWord) < length)
	{
		theWord[strlen(theWord)] = '\0';
	}
	else //if all avail space for the wordToCheck are filled... we still need our null terminator
	{
		theWord[strlen(theWord) - 1] = '\0'; //will remove a character but we need our null terminator
	}

	//NOTE: this may or may not have a null terminating char... we will have to give it its null terminator afterwards

	printf("2 strlen %i\n", strlen(theWord));

	return theWord;

}

char* removeFirstWord(char* codeLine) //this should remove the first word from codeline and return codeline
{
	char newCodeLine[100] = "";
	int charsToRemove = 0;
	char temp[100] = "";

	int i = 0;
	for (i; i < strlen(codeLine); i += 1) //loop through the codeLine till the end of the first word
	{
		if (isspace(codeLine[i]) != 0)
		{
			if (temp[0] != '\0')
				break; //we have found our first word
			else
				charsToRemove+=1;//else we are just getting rid of space in the front
		}
		else //a char was found so add it to the word
		{
			charsToRemove+=1;
			temp[0] = ".";
		}
	}

	int nCLloc = 0;
	int loopStoper = strlen(codeLine); 
	for (charsToRemove, nCLloc; charsToRemove < loopStoper; charsToRemove += 1, nCLloc++) //loop through the codeLine till the end of the first word
	{
		newCodeLine[nCLloc] = codeLine[charsToRemove];
	}

	//the original line should already have an null terminator so we dont need to add one

	return newCodeLine;
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

