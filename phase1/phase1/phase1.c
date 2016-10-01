
/*
Programmer: Bryan Cancel
Date: 9/28/16

Task: Write a command Line Interpreter that will function as a simple OS for the rest of our project

Requirements:
written in C
must run in linux with no added compiler options
No precompiled routines to break up the line (sscanf)
instead use loops and comparison
not menu driven

Deliverable:
well documented source listing
@toplevel/3334/phase1
with only source files in said folder
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//avail commands
void loadCommand(char* filename);
void executeCommand();
void debugCommand();
void dumpCommand(int start, int end);
void helpCommand();
void assembleCommand(char* filename);
void directoryCommand();
void exitCommand();

void clearBuffers();
void printBuffer();

char buffer[100] = { NULL };
char command[100] = { NULL };
//TODO allow this to handle as many params as passed (for now 2 is fine)
char param1[100] = { NULL };
char param2[100] = { NULL };

int numberOfParams = 0;

int main()
{
	//print out welcome message and prompt user
	printf("Welcome to Bryan's Command Line Interpreter! a.k.a. bryAN's Command Line Interpreter a.k.a. ANCLI \n");

	int exitNow = 0; //0 is false, 1 is true (determines whether or not to exit the program)

	int bufferLength = 0;

	while (exitNow == 0) {

		//message to prompt user for another command
		printf("ANCLI will: ");

		//get the line of input
		fgets(buffer, sizeof(buffer), stdin);
		//printf("answer: %s\n", buffer);
		if (!strchr(buffer, '\n'))     //newline not found in current buffer
		{
			printf("WARNING --- You typed in more than the allowed %i chars per command/ni will truncate the rest of the characters but the command might not work as intended", sizeof(buffer));
			while (fgetc(stdin) != '\n');//discard chars until newline
		}

		//save userInputLength for easier access later
		bufferLength = strlen(buffer) - 1;

		//replace last char for null terminator (might lose some data but i told that to the user already)
		buffer[bufferLength] = '\0';

		//printf("Buffer Length: %i",bufferLength);

		//make entire command not case sensitive(all to LOWER)
		int a;
		for (a = 0; a < bufferLength; a++)
			if (buffer[a] >= 65 && buffer[a] <= 90)
				buffer[a] = buffer[a] + 32;

		//setup for processing the line
		char currWord[100] = { NULL };
		int currWordLength = 0;

		int i;
		int pos;
		//Cut up the Line
		for (i = 0, pos = 0; i < (bufferLength + 1); i++)
		{
			//if you find a space either (1) ignore it (2) have it indicate the end of a command or param
			if (isspace(buffer[i]) != 0 || i == bufferLength) //this should also run on the new line char
			{

				if (strcmp(currWord, "") == 0) //this means you found a space at the begining of the line before a word was found
					;
				else
				{
					//TODO could be improved by using strcpy() - get rid of them for loops

					//TODO modify code below so it works with unlimited params
					if (currWordLength > 0) //this is needed so it doenst start taking single spaces as params
					{
						//TODO make sure that the currWord fits inside the param, it will always fit with current numbers but in case we change them

						//asign value to things in order, command, param1, param2
						if (strcmp(command, "") == 0) //assign currWord to command
						{
							int copy1;
							//set the command to be our string (it might not fit...)
							for (copy1 = 0; copy1 < currWordLength; copy1++)
							{
								command[copy1] = currWord[copy1];
							}
						}
						else if (strcmp(param1, "") == 0) //assign currWord to param1
						{
							int copy2;
							//set the command to be our string (it might not fit...)
							for (int copy2 = 0; copy2 < currWordLength; copy2++)
							{
								param1[copy2] = currWord[copy2];
							}
							numberOfParams++;
						}
						else if (strcmp(param2, "") == 0) //assign currWord to param 2
						{
							int copy3;
							//set the command to be our string (it might not fit...)
							for (int copy3 = 0; copy3 < currWordLength; copy3++)
							{
								param2[copy3] = currWord[copy3];
							}
							numberOfParams++;
						}
						else
						{
							numberOfParams++;
							//do nothing with it... just get rid of the junk...
						}
					}

					//erase the word we are working with, since we no longer use it
					currWordLength = 0;
					pos = 0;
				}
			}
			else
			{
				//add this letter to the currWord
				if (pos < 100)
				{
					currWord[pos] = buffer[i];
					currWordLength++;
					pos++;
				}
				else
				{
					//this will literally never happen becuase of the sizes of everything at the begining, but in case we change the sizes i decied to add it in
					printf("your params are too long, try again");
					break;
				}

			}
		}


		printf("-%s-\n", command);
		printf("-%s-\n", param1);
		printf("-%s-\n", param2);
		printf("param# %i\n", numberOfParams);

		//TODO I could put these commands through a hash and then find them in constant time - micro optimization

		//check if valid command by going through the statements
		if (strcmp(command, "load") == 0) {
			printf("entered load command\n");
			if (numberOfParams < 1) {
				printf("you have too few params TRY AGAIN\n");
				printf("the correct formating is 'load filename'\n");
			}
			else
			{
				if (numberOfParams > 1)
				{
					printf("WARNING --- you have too many params\nbut I will truncate and proceed\n");
					printf("the correct formating is 'load filename'\n");
				}
				loadCommand("filename");
			}
		}
		else if (strcmp(command, "execute") == 0) {
			printf("entered execute command\n");
			if (numberOfParams > 0) {
				printf("WARNING --- you have too many params\nbut I will truncate and proceed\n");
				printf("the correct formating is 'execute'\n");
			}
			executeCommand();
		}
		else if (strcmp(command, "debug") == 0) {
			printf("entered debug command\n");
			if (numberOfParams > 0) {
				printf("WARNING --- you have too many params\nbut I will truncate and proceed\n");
				printf("the correct formating is 'debug'\n");
			}
			debugCommand();
		}
		else if (strcmp(command, "dump") == 0) {
			printf("entered dump command\n");
			if (numberOfParams < 2) {
				printf("you have too few params TRY AGAIN\n");
				printf("the correct formating is 'dump start end'\n");
			}
			else
			{
				if (numberOfParams > 2)
				{
					printf("WARNING --- you have too many params\nbut I will truncate and proceed\n");
					printf("the correct formating is 'dumb start end'\n");
				}
				dumpCommand(0, 1);
			}
		}
		else if (strcmp(command, "help") == 0) {
			printf("entered help command\n");
			if (numberOfParams > 0) {
				printf("WARNING --- you have too many params\nbut I will truncate and proceed\n");
				printf("the correct formating is 'help'\n");
			}
			helpCommand();
		}
		else if (strcmp(command, "assemble") == 0) {
			printf("entered assemble command\n");
			if (numberOfParams < 1) {
				printf("you have too few params TRY AGAIN\n");
				printf("the correct formating is 'assemble filename'\n");
			}
			else
			{
				if (numberOfParams > 1) {
					printf("WARNING --- you have too many params\nbut I will truncate and proceed\n");
					printf("the correct formating is 'assemble filename'\n");
				}
				assembleCommand("filename");
			}
		}
		else if (strcmp(command, "directory") == 0) {
			printf("entered directory command\n");
			if (numberOfParams > 0) {
				printf("WARNING --- you have too many params\nbut I will truncate and proceed\n");
				printf("the correct formating is 'directory'\n");
			}
			directoryCommand();
		}
		else if (strcmp(command, "exit") == 0) {
			printf("entered exit command\n");
			if (numberOfParams > 0) {
				printf("WARNING --- you have too many params\nbut I will truncate and proceed\n");
				printf("the correct formating is 'exit'\n");
			}
			exitCommand();
		}
		else {
			printf("Sorry, I don't Recognize this command TRY AGAIN\n");
		}
		printf("\n");

		clearBuffers();
	}

	return 0;
}

//Available Commands
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

void helpCommand() {
	printf("running help command\n");

	//implemented
	printf("\n'load filename' - will load the specified file\n");
	printf("'execute' - will execute the program that was previously loaded in memory\n");
	printf("'dump start end' - will call the dump function, passing the values of start and end\n");
	printf("'help' - will print out a list of available commands\n");
	printf("-assemble filename' - will assemble an SIC assembly language program into a load module and store it in a file\n");
	printf("'directory' - will list the files stored in the current directory\n");
	printf("'exit' - will exit from this simulator\n");

}

void assembleCommand(char* filename) {
	printf("running assemble command\n");
}

void directoryCommand() {
	printf("running directory command\n");

	//implemented
	system("ls -l");
}

void exitCommand() {
	printf("running exit command\n");

	//implemented
	exit(0);
}


void clearBuffers() {
	memset(buffer, 0, sizeof(buffer));

	//reset char arrays
	//TODO recode this for unlimited params
	memset(command, 0, sizeof(command));
	memset(param1, 0, sizeof(param1));
	memset(param2, 0, sizeof(param2));

	numberOfParams = 0;
}

void printBuffer() {
	int b;
	for (b = 0; b < 100; b++)
	{
		printf("%c", buffer[b]);
		printf("-");
	}
	printf("\n");
}
