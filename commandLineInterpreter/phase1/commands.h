/*
Programmer: Bryan Cancel
Last Updated: 2-8-18

this holds the prototypes for all the larger commands
and holds the definitions of the smaller commands

NOTE: 
Assemble in defined elsewhere
*/

//---includes for when we add the phase 2
#pragma once
#include "pass1.h"
#include "phase4.h"

//-------------------------PROTOTYPES-------------------------

//----------To Be Implemented Prototypes----------

void debugCommand();

//----------Not Fully Implemented Prototypes----------

void dumpCommand(char* start, char* end); //integer base 10 version of the hex digit (if required create a conversion from HEX to DEC and pass here)
void loadCommand(char* filename);
void executeCommand();

//----------Fully Implemented Prototypes----------

//FILL dump here
void helpCommand();
void assembleCommand(char* filename);
void directoryCommand();
void exitCommand();
void assembleCommand(char* filename);

//-------------------------MAIN FUNCTIONS-------------------------

//----------To Be Implemented Functions----------

void debugCommand() {
	printf("running debug command\n");
}

//----------Not Fully Implemented Functions----------

void loadCommand(char* filename) {
	printf("running load command\n");

	loadExt(filename);
}

void executeCommand() {
	printf("running execute command\n");
}

void dumpCommand(char* start, char* end) {
	printf("running dump command\n");

	dumpExt(start, end);
}

//----------Fully Implemented Functions----------

void helpCommand() {
	printf("running help command\n");

	printf("\n'load filename' - will load the specified file\n");
	printf("'execute' - will execute the program that was previously loaded in memory\n");
	printf("'dump start end' - will call the dump function, with values start and end\n");
	printf("'help' - will print out a list of available commands\n");
	printf("'assemble filename' - will assemble an SIC assembly language program into a load module and store it in a file\n");
	printf("'directory' - will list the files stored in the current directory\n");
	printf("'exit' - will exit from this simulator\n");
}

void assembleCommand(char* filename) {

	printf("running assemble command\n");

	pass1(filename); //seperate proc for pass 1 to create symbol table and intermediate file
	//PASS1 calls PASS2 after its completed
}

void directoryCommand() {

	printf("running directory command\n");

	system("ls -l"); //for running within command line in linux/unix
	system("dir"); //for running within command line in windows
}

void exitCommand() {

	printf("running exit command\n");

	exit(0);
}