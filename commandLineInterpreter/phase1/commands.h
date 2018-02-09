/*
Programmer: Bryan Cancel
Last Updated: 2-8-18

this holds the prototypes for all the larger commands
and holds the definitions of the smaller commands

NOTE: 
Assemble in defined elsewhere
*/

//---includes for when we add the phase 2
//#pragma once
//#include "assemble.h"

//-------------------------PROTOTYPES-------------------------

//----------Not Fully Implemented Prototypes----------

void loadCommand(char* filename);
void executeCommand();
void debugCommand();
void dumpCommand(int start, int end); //integer base 10 version of the hex digit (if required create a conversion from HEX to DEC and pass here)
void assembleCommand(char* filename);

//----------Fully Implemented Prototypes----------

//FULL dump here
void helpCommand();
//FULL assemble here
void directoryCommand();
void exitCommand();

//-------------------------MAIN FUNCTIONS-------------------------

//----------Not Fully Implemented Functions----------

void loadCommand(char* filename) {
	printf("running load command\n");
}

void executeCommand() {
	printf("running execute command\n");
}

void debugCommand() {
	printf("running debug command\n");
}

void dumpCommand(int start, int end) {
	printf("running dump command\n");
}

void assembleCommand(char* filename) {
	printf("running assemble command\n");
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

void directoryCommand() {

	printf("running directory command\n");

	system("ls -l");
}

void exitCommand() {

	printf("running exit command\n");

	exit(0);
}