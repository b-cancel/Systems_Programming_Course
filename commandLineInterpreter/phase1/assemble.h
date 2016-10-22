/*
Programmer: Bryan Cancel
Date: 10/21/16

This is going to hold the prototypes for our 2 pass assembler
and some notes on the SIC architecture for quick referancing

I am Assuming:
(1) that a linkage records in the object file don't exist because I wasnt able to find any mention of it anywhere... not even on the web...
(2) you dont need the (a) intermediate file (b) listing file (c) object file... with any specific file extension --- I will use .txt for all... for now...
(3) I can write whatever I want before START and after END and it should not affect file (c), but it will appear in files (a) and (b) as a comment
*/

#pragma once

//prototypes
void pass1(); //for pass1 func in pass1.c
void pass2(); //for pass2 func in pass2.c

void assembleCommand(char* filename) {

	printf("running assemble command\n");
	
	pass1(); //seperate proc for pass 1 to create symbol table and intermediate file
	pass2(); //seperate proc for pass 2 to create object file and listing file
}

/*
NOTES on SIC Machine Architecture (NOT SIC/XE)

Memory
	memory - 8 bytes
	word - 3 bytes
	size 2^15 = 32,768 bytes of memory AT MOST
	address: byte or lowest numbered byte in a word... TODO... this means...?

Data Format
	integer: words 2's comp
	character: byte, ASCII rep

Regs
	A - 0 - Accumulator
	X - 1 - Index Reg
	L - 2 - Linkage Reg
	PC - 8 - Program Counter
	SW - 9 Status word

Instruct Format (a word)
	opcode - 8 bits
	index bit - 1 bit
	address - 15 bits

Addressing Mode
	Direct: index bit = 0, TA = address
	Indexed: index bit = 1, TA = address + (X)

25 Instructions

Input and Output happens to/from rightmost byte of reg A --- 1 byte at a time --- each device is assigned an 8 bit code

Some Directive aka psuedo-ops
START
END
BYTE
WORD
RESB
RESW

OTHER NOTES FROM WIKI
stores most memory addresses hexadecimal integer format... TODO... this means???
stores all data in binary, and uses the two's complement to represent negative values at the machine level
Memory storage in SIC consists of 8-bit bytes, and all memory addresses in SIC are byte addresses. 
Any three consecutive bytes form a 24-bit 'word' value, addressed by the location of the lowest numbered byte in the word value. 
Numeric values are stored as word values, and 
character values use the 8-bit ASCII system
*/