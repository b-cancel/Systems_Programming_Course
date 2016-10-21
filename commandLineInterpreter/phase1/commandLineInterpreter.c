
/*
Programmer: Bryan Cancel
Date: 9/28/16
significant updates: 10/21/16

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

//NOTE: for organization purposes I have my declarations in .h files AND definitions in .c files

//constants

//library includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//my file includes
//NOTE: avoid including .c file beause the linker will merge the obj files resulting from the compilation of each of the .c files
//NOTE: this is because every .c file get its own .obj file
#include "commands.h"

//prototypes
void clearStrings();
void printStrings();

//REMEMBER char arrays are NOT automatically null terminated, string literals ARE
char buffer[100] = "";
char command[100] = "";
//TODO this only handles 2 params, if needed allow it to handle countably many
char param1[100] = "";
char param2[100] = "";

int numberOfParams = 0;

//NOTE: strcpy is deprecated - it can lead to buffer overflow if you try to copy a string to a buffer that is not large enough to contain it

int main()
{
	//print out welcome message and prompt user
	printf("Welcome to Bryan's Command Line Interpreter! a.k.a. bryAN's Command Line Interpreter a.k.a. ANCLI \n");

	int bufferLength = 0;

	//this will run forever until the exit command runs
	while (1 == 1) {

		//message to prompt user for another command
		printf("ANCLI will: ");

		//get the line of input
		fgets(buffer, sizeof(buffer), stdin);
		//printf("answer: %s\n", buffer);
		if (!strchr(buffer, '\n'))     //newline not found in current buffer
		{
			printf("WARNING --- You typed in more than the allowed %i chars per command \n i will truncate the rest of the characters but the command might not work as intended \n", sizeof(buffer));
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
		char currWord[100] = "";
		int currWordLength = 0;

		int i;
		int pos;
		//Cut up the Line
		for (i = 0, pos = 0; i < (bufferLength + 1); i++)
		{
			//if you find a space either (1) ignore it (2) have it indicate the end of a command or param
			if (isspace(buffer[i]) != 0 || i == bufferLength) //this should also run on the new line char
			{

				if (currWord[0] == '\0') //this means you found a space at the begining of the line before a word was found
					; //so ignore the space
				else
				{
					//TODO could be improved by using strcpy() - get rid of them for loops

					//TODO modify code below so it works with unlimited params
					if (currWordLength > 0) //this is needed so it doenst start taking single spaces as params
					{
						//TODO make sure that the currWord fits inside the param, it will always fit with current numbers but in case we change them

						//asign value to things in order, command, param1, param2
						if (command[0] == '\0') //assign currWord to command
						{
							int copy1;
							//set the command to be our string (it might not fit...)
							for (copy1 = 0; copy1 < currWordLength; copy1++)
							{
								command[copy1] = currWord[copy1];
							}
							command[copy1] = '\0';//add the null terminator
						}
						else
						{
							numberOfParams++;

							if (param1[0] == '\0') //assign currWord to param1
							{
								int copy2;
								//set the command to be our string (it might not fit...)
								for (copy2 = 0; copy2 < currWordLength; copy2++)
								{
									param1[copy2] = currWord[copy2];
								}
								param1[copy2] = '\0';//add the null terminator
							}
							else if (param2[0] == '\0') //assign currWord to param 2
							{
								int copy3;
								//set the command to be our string (it might not fit...)
								for (copy3 = 0; copy3 < currWordLength; copy3++)
								{
									param2[copy3] = currWord[copy3];
								}
								param2[copy3] = '\0';//add the null terminator
							}
							else
								;//ignore it because we only take in 2 params ever...
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

		printStrings(); //this is just a "debug" tool

		//TODO I could put these commands through a hash and then find them in constant time
		//check if valid amount of params for the command
		if (strcmp(command, "load") == 0) {
			printf("entered load command\n");
			if (numberOfParams != 1){
				printf("you have too few OR too many params TRY AGAIN\n");
				printf("the correct formating is 'load filename'\n");
			}
			else
				loadCommand(param1);
		}
		else if (strcmp(command, "execute") == 0) {
			printf("entered execute command\n");
			if (numberOfParams != 0) {
				printf("you have too few OR too many params TRY AGAIN\n");
				printf("the correct formating is 'execute'\n");
			}
			else
				executeCommand();
		}
		else if (strcmp(command, "debug") == 0) {
			printf("entered debug command\n");
			if (numberOfParams != 0) {
				printf("you have too few OR too many params TRY AGAIN\n");
				printf("the correct formating is 'debug'\n");
			}
			else
				debugCommand();
		}
		else if (strcmp(command, "dump") == 0) {
			printf("entered dump command\n");
			if (numberOfParams != 2) {
				printf("you have too few OR too many params TRY AGAIN\n");
				printf("the correct formating is 'dump start end'\n");
			}
			else
				dumpCommand(param1, param2);
		}
		else if (strcmp(command, "help") == 0) {
			printf("entered help command\n");
			if (numberOfParams != 0) {
				printf("you have too few OR too many params TRY AGAIN\n");
				printf("the correct formating is 'help'\n");
			}
			else
				helpCommand();
		}
		else if (strcmp(command, "assemble") == 0) {
			printf("entered assemble command\n");
			if (numberOfParams != 1) {
				printf("you have too few OR too many params TRY AGAIN\n");
				printf("the correct formating is 'assemble filename'\n");
			}
			else
				assembleCommand(param1);
		}
		else if (strcmp(command, "directory") == 0) {
			printf("entered directory command\n");
			if (numberOfParams != 0) {
				printf("you have too few OR too many params TRY AGAIN\n");
				printf("the correct formating is 'directory'\n");
			}
			else
				directoryCommand();
		}
		else if (strcmp(command, "exit") == 0) {
			printf("entered exit command\n");
			if (numberOfParams != 0) {
				printf("you have too few OR too many params TRY AGAIN\n");
				printf("the correct formating is 'exit'\n");
			}
			else
				exitCommand();
		}
		else {
			printf("Sorry, I don't Recognize this command TRY AGAIN\n");
		}
		printf("\n");

		clearStrings();
	}

	return 0;
}

//set the first character to a null terminator so everything I am using will think its empty
void clearStrings() {
	//could use memset

	buffer[0] = '\0';
	command[0] = '\0';
	//TODO if need more than two params... recode this for unlimited params
	param1[0] = '\0';
	param2[0] = '\0';

	numberOfParams = 0;
}

//this is to just check if string parsing was done correctly
void printStrings() {
	printf("Buffer: -%s- \n", buffer);
	printf("Command: -%s- \n", command);
	//TODO if need more than two params... recode this for unlimited params
	//printf("# of Params: %n \n", numberOfParams);
	printf("Param 1: -%s- \n", param1);
	printf("Param 2: -%s- \n", param2);
}