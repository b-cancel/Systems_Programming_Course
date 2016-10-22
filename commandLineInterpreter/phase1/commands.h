/*
Programmer: Bryan Cancel
Date: 10/21/16

this hold the prototypes for all the larger commands
and holds the definitions of the smaller commands
*/

#pragma once
#include "assemble.h"

//--- the prototypes that will eventually be left over after I Implement all the Definitions
void loadCommand(char* filename);
void executeCommand();
void debugCommand();
void dumpCommand(int start, int end);
void assembleCommand(char* filename);


//--- the definitions bellow should each be in their own .cpp file(if they are long enough to merit it), I will do this as I implement them little by little

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

//-----short implementations that don't merit their own file

void helpCommand() {
	printf("running help command\n");

	printf("\n'load filename' - will load the specified file\n");
	printf("'execute' - will execute the program that was previously loaded in memory\n");
	printf("'dump start end' - will call the dump function, passing the values of start and end\n");
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
