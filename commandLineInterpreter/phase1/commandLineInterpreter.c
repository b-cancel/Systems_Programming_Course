/*
Programmer: Bryan Cancel
Last Updated: 2-8-18

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

NOTE: any NOTE TO SELF... will be denoted NTS:
I am using this because its my first time using C
*/

//NOTE: for organization purposes I have my declarations in .h files AND definitions in .c files

//constants

//---library includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//---my file includes
//NTS: avoid including .c file beause the linker will merge the obj files resulting from the compilation of each of the .c files
//NTS: this is because every .c file get its own .obj file
#include "commands.h"
#include "pass1.h"

//prototypes
void clearStrings();
void printStrings();

void clearCommand(); //secret command, ignore me please

//NTS: sizeof(buffer) evaluates to 100 [because its the length i have set], [doesnt care if null terminator is or is not there]
//NTS: strlen(buffer) returns the count of characters in the char array. [does not include null terminator]
//NTS: char arrays are NOT automatically null terminated, string literals ARE
char buffer[100] = ""; 
char command[100] = "";
char param1[100] = "";
char param2[100] = "";

int numberOfParams = -1;

int main()
{
	//print out welcome message and prompt user
	printf("Welcome to Bryan [CAN]cel's [C]ommand [L]ine [I]nterpreter! A.K.A. CAN C.L.I. or just CAN\n\n");

	//this will run forever until the exit command runs
	while (1 == 1) {

		clearStrings();

		//-----ask the user for a command and read it

		//message to prompt user for another command
		printf("What would you like me to do?: ");

		//get the line of input
		//NTS: read and store until (num-1) characters have been read or either a newline or the end-of-file is reached
		//NTS: will include the newline in that spare space
		fgets(buffer, sizeof(buffer), stdin); //NTS: always leaves space for the null terminator (so our length will NEVER be what we set it to... always 1 smaller)

		//-----let the user know if the command is too large

		if (strchr(buffer, '\n') == NULL) //newline not found in current buffer
		{
			printf("\nERROR --- You typed in more than the allowed %i chars per command\nCOMMAND IGNORED\n\n", (int)sizeof(buffer));
			//NTS: fgetc moves the "internal stream position indicator" to the next letter after reading the current one
			while (fgetc(stdin) != '\n'); //discard chars until newline
		}
		else {
			printf("\n");

			buffer[strlen(buffer) - 1] = '\0'; //confirm our last location contains a null terminator [precaution]

			//-----make the command case insensitive

			//make entire command not case sensitive(all to LOWER)
			int charID;
			//for (charID = 0; charID < strlen(buffer); charID++)
			//	if (buffer[charID] >= 65 && buffer[charID] <= 90) //A -> Z
			//		buffer[charID] = buffer[charID] + 32; //toLowerCase

			//-----cut up the information we received in the buffer

			//NOTE: this was coded so that you could plug in literally anything inbetween commands except (1) letters and (2) numbers

			//NTS: \0 is not alphanumeric

			//we use the values of numberOfParams to determine where to place our character
			//0 command, 1 param1, 2 param2

			charID = 0;
			while (charID < strlen(buffer)) //NTS: will not loop over null terminator
			{
				numberOfParams++; //becomes 0 for the first command

				if (numberOfParams > 2) { //TODO.... check
					printf("ERROR --- You typed in more than the allowed 2 parameters per command\nCOMMAND IGNORED\n\n");
					break;
				}
				else {
					//get rid of anything that is a space
					while (isspace(buffer[charID]) != 0 && buffer[charID] != '\0') {
						charID++;
					}

					if (buffer[charID] == '\0') {
						numberOfParams--;
						break;
					}

					//add anything that isnt a space to our word
					while (isspace(buffer[charID]) == 0 && buffer[charID] != '\0') {

						int currLength;
						//insert the alpha numeric character into our "param"
						switch (numberOfParams)
						{
						case 0:
							currLength = strlen(command);
							command[currLength] = tolower(buffer[charID]);
							command[currLength + 1] = '\0';
							break;
						case 1:
							currLength = strlen(param1);
							param1[currLength] = buffer[charID];
							param1[currLength + 1] = '\0';
							break;
						default: //case 2
							currLength = strlen(param2);
							param2[currLength] = buffer[charID];
							param2[currLength + 1] = '\0';
							break;
						}

						charID++;
					}

					if (buffer[charID] == '\0') {
						break;
					}
				}
			}

			//-----find command and check if we were passed the proper count of params

			if (strcmp(command, "load") == 0) {
				printf("entered \"load\" command\n");
				if (numberOfParams != 1) {
					printf("ERROR --- you have too few OR too many parameters\n");
					printf("the correct formating is 'load filename'\n");
				}
				else
					loadCommand(param1);
			}
			else if (strcmp(command, "execute") == 0) {
				printf("entered \"execute\" command\n");
				if (numberOfParams != 0) {
					printf("ERROR --- you have too few OR too many parameters\n");
					printf("the correct formating is 'execute'\n");
				}
				else
					executeCommand();
			}
			else if (strcmp(command, "debug") == 0) {
				printf("entered \"debug\" command\n");
				if (numberOfParams != 0) {
					printf("ERROR --- you have too few OR too many parameters\n");
					printf("the correct formating is 'debug'\n");
				}
				else
					debugCommand();
			}
			else if (strcmp(command, "dump") == 0) {
				printf("entered \"dump\" command\n");
				if (numberOfParams != 2) {
					printf("ERROR --- you have too few OR too many parameters\n");
					printf("the correct formating is 'dump start end'\n");
				}
				else
					dumpCommand(1, 1); //TODO eventually switch this out for the actual params
			}
			else if (strcmp(command, "help") == 0) {
				printf("entered \"help\" command\n");
				if (numberOfParams != 0) {
					printf("ERROR --- you have too few OR too many parameters\n");
					printf("the correct formating is 'help'\n");
				}
				else
					helpCommand();
			}
			else if (strcmp(command, "assemble") == 0) {
				printf("entered \"assemble\" command\n");
				if (numberOfParams != 1) {
					printf("ERROR --- you have too few OR too many parameters\n");
					printf("the correct formating is 'assemble filename'\n");
				}
				else
					assembleCommand(param1);
			}
			else if (strcmp(command, "directory") == 0) {
				printf("entered \"directory\" command\n");
				if (numberOfParams != 0) {
					printf("ERROR --- you have too few OR too many parameters\n");
					printf("the correct formating is 'directory'\n");
				}
				else
					directoryCommand();
			}
			else if (strcmp(command, "exit") == 0) {
				printf("entered \"exit\" command\n");
				if (numberOfParams != 0) {
					printf("ERROR --- you have too few OR too many parameters\n");
					printf("the correct formating is 'exit'\n");
				}
				else
					exitCommand();
			}
			else if (strcmp(command, "clear") == 0) {
				printf("entered \"clear\" command\n");
				if (numberOfParams != 0) {
					printf("ERROR --- you have too few OR too many parameters\n");
					printf("the correct formating is 'clear'\n");
				}
				else
					clearCommand();
			}
			else {
				printf("Sorry, I don't Recognize this command.\n");
				printf("Type \"Help\" to view a list of commands along with a breif description\n");
			}
			printf("\n");
		}
	}

	return 0;
}


//-------------------------HELPER FUNCTIONS-------------------------

//set the first character to a null terminator so everything I am using will think its empty
void clearStrings() {
	buffer[0] = '\0';
	command[0] = '\0';
	param1[0] = '\0';
	param2[0] = '\0';

	numberOfParams = -1; //this becomes 0 in the while loop that breaks up the buffer
}

//-------------------------DEBUGGING FUNCTIONS-------------------------

//this is to just check if string parsing was done correctly
void printStrings() {
	printf("-----\n");
	printf("Buffer: -%s- \n", buffer);
	printf("Command: -%s- \n", command);
	printf("# of Params: %i \n", numberOfParams);
	printf("Param 1: -%s- \n", param1);
	printf("Param 2: -%s- \n", param2);
	printf("-----\n");
}

void clearCommand() {

	printf("ran SECRET COMMAND! pretend it doesn't exist please and thank you :)");

	system("clear");
}