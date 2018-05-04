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

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>