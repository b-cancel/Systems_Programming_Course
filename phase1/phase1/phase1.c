
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

void clearBuffer() {

}

int main()
{
	//print out welcome message and prompt user
	printf("Welcome to Bryan's Command Line Interpreter! a.k.a. bryAN's Command Line Interpreter a.k.a. ANCLI \n");

	int exitNow = 0; //0 is false, 1 is true (determines whether or not to exit the program)

	/*100 was arbitrarily chosen, you shouldn't have a command larger than that
	if you do it will be left in buffer*/

	char userInput[100]; //if i assign this { NULL } like the rest, strlen wont work

	char command[10] = { NULL };

	//for now we only have 2 params so only worry about that
	//TODO allow this to handle as many params as passed
	char param1[100] = { NULL };
	char param2[100] = { NULL };
	int numberOfParams = 0;

	int userInputLength = 0;

	while (exitNow == 0) {

		//prompting user
		printf("ANCLI will: ");
		fgets(userInput, 100, stdin);

		//save userInputLength for easier access later
		userInputLength = strlen(userInput) - 1;

		//check that the input line isnt too long
		if (userInputLength > 100)
		{
			printf("You Typed " + userInputLength);
			printf(" characters, but the limit is " + 100);
			printf(" --- I will clear the buffer and you can try again\n");

			clearBuffer();
		}
		else
		{
			//replace newline char for null terminator
			if (userInput[userInputLength] == '\n')
				userInput[userInputLength] = '\0';

			//make entire command not case sensitive(all to LOWER)
			for (int i = 0; i < userInputLength; i++)
			{
				if (userInput[i] >= 65 && userInput[i] <= 90)
				{
					userInput[i] = userInput[i] + 32;
				}
			}

			char currWord[50] = { NULL };
			int currWordLength = 0;

			//cut out the command and THEN the 2 params
			for (int i = 0, pos = 0; i < userInputLength; i++)
			{
				printf("%c", userInput[i]);
				printf("\n");
				if (isspace(userInput[i]) != 0)
				{
					printf("found a space\n");

					if (strcmp(currWord, "") == 0)
						;
					else
					{
						//asign value to things in order, command, param1, param2
						if (strcmp(command, "") == 0) //assign currWord to command
						{
							print("COMMAND");
							printf("%s", currWord);
							//set the command to be our string
							for (int copy = 0; copy < currWordLength; copy++)
							{
								command[copy] = currWord[copy];
							}
						}
						else if (strcmp(param1, "") == 0) //assign currWord to param1
						{
							print("PARAM1");
							printf("%s", currWord);
							//set the command to be our string
							for (int copy = 0; copy < currWordLength; copy++)
							{
								param1[copy] = currWord[copy];
							}
						}
						else if (strcmp(param2, "") == 0) //assign currWord to param 2
						{
							print("PARAM2");
							printf("%s", currWord);
							//set the command to be our string
							for (int copy = 0; copy < currWordLength; copy++)
							{
								param2[copy] = currWord[copy];
							}
						}
						else
						{
							printf("im in the else");
							;//do nothing with it... just get rid of the junk...
						}

						//clear currWord
						for (int i = 0; i < 50; i++)
							currWord[i] = NULL;

						currWordLength = 0;
					}
				}
				else
				{
					//add this letter to the currWord
					currWord[pos] = userInput[i];
					currWordLength++;
					pos++;
				}
			}
			
			printf("00");
			printf(command);
			printf("00\n");

			printf("00");
			printf(param1);
			printf("00\n");

			printf("00");
			printf(param2);
			printf("00\n");

			//cut out all the params (regardless of how many)

			//check if valid command by going through the statements
			/*
			if (strcmp(command,"load")==0) {
				printf("entered load command\n");
				if (numberOfParams != 1) {
					printf("you have too many or too few params\n");
					printf("the correct formating is 'load filename'\n");
				}
				else
				{
					printf("running load command\n");
				}
			}
			else if (command == "execute") {
				printf("entered execute command\n");
				if (numberOfParams != 0) {
					printf("you have too many or too few params\n");
					printf("the correct formating is 'execute'\n");
				}
				else {
					printf("running execute command\n");
				}
			}
			else if (command == "debug") {
				printf("entered debug command");
				if (numberOfParams != 0) {
					printf("you have too many or too few params\n");
					printf("the correct formating is 'debug'\n");
				}
				else {
					printf("running debug command\n");
				}
			}
			else if (command == "dump") {
				printf("entered dump command");
				if (numberOfParams != 2) {
					printf("you have too many or too few params\n");
					printf("the correct formating is 'dump start end'\n");
				}
				else {
					printf("running dump command\n");
				}
			}
			else if (command == "help") {
				printf("entered help command\n");
				if (numberOfParams != 0) {
					printf("you have too many or too few params\n");
					printf("the correct formating is 'help'\n");
				}
				else {
					printf("running help command\n");
					//TODO IMPLEMENT

					
				}
			}
			else if (command == "assemble") {
				printf("entered assemble command\n");
				if (numberOfParams != 1) {
					printf("you have too many or too few params\n");
					printf("the correct formating is 'assemble filename'\n");
				}
				else
				{
					printf("running assemble command\n");
				}
			}
			else if (command == "directory") {
				printf("entered execute command\n");
				if (numberOfParams != 0) {
					printf("you have too many or too few params\n");
					printf("the correct formating is 'directory'\n");
				}
				else {
					printf("running directory command\n");
					//TODO implement

				}
			}
			else if (command == "exit") {
				printf("entered exit command\n");
				if (numberOfParams != 0) {
					printf("you have too many or too few params\n");
					printf("the correct formating is 'exit'\n");
				}
				else {
					printf("running exit command\n");
					//TODO implement
					exit(0);
				}
			}
			else {
				printf("Sorry, I don't Recognize this command\n");
			}
			*/
			clearBuffer();
			printf("\n");
		}

	}

	return 0;
}
