/*
*/

#pragma region  Library Includes

#pragma once

#include "helperFunctions.h"

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#pragma endregion

void pass2(char *sourceFileName, char * intermediateFileName, char **_firstLabel, char **_lastLabel, int programLength)
{
	char *firstLabel = *_firstLabel;
	char *lastLabel = *_lastLabel;

	printf("source name is '%s' intermediate file name is '%s'\n", sourceFileName, intermediateFileName);
	printf("first label is '%s', last label is '%s', and the program length is %i\n", firstLabel, lastLabel, programLength);

	printf("READY FOR PASS 2\n\n");

	//create the variables that will be used to read in our file
	char *line = NULL; //NOTE: this does not need a size because getline handle all of that
	size_t len = 0;

	//-------------------------1st File Reading(to read in the largest error code line for formatting purposes)-------------------------
	//NOTE: this is a ridiculous ammount of work to get a nice looking listing file... but well... I really wanted it to look pretty
	int longestErrorLine = 0;

	FILE *ourIntermediateFile_SYMTBL = fopen(strCat("./", intermediateFileName), "r");
	if (ourIntermediateFile_SYMTBL != NULL)
	{
		int readingInstructions = 1;

		int instructID = 0;
		int currLineCount = 0;

		//keep reading our intermediate file until there is nothing left
		while (getline(&line, &len, ourIntermediateFile_SYMTBL) != -1)
		{
			if (readingInstructions == 1) {
				if (strcmp(subString(line, 0, strlen(line) - 1), "---Symbol Table (string -> int)") == 0)
					readingInstructions = 0; //the next line we use it to populate our symbol table
				else 
				{
					//we have a set of 8 lines... line 7 contains the errors... i need to know what is the longest stream of errors and save that value
					instructID++;
					if (instructID == 7) {
						int thisLength = strlen(line);
						if (thisLength > longestErrorLine)
							longestErrorLine = thisLength;
					}
					if (instructID == 8) {
						currLineCount++;
						instructID = 0;
					}
				}
			}
		}

		fclose(ourIntermediateFile_SYMTBL); //close our intermediate file after reading it
	}
	else
		printf("ERROR --- INTERMEDIATE file did not open properly\n"); //THE ONLY ERROR THAT CANNOT BE IN THE INTERMEDIATE FILE

	printf("READING 1\n\n");

	//-------------------------2nd File Reading(to process the data in the intermeditae file)-------------------------

	char* listingFileName = strCat(subString(sourceFileName, 0, strlen(sourceFileName) - 4), "Listing.txt");
	FILE *ourListingFile = fopen(strCat("./", listingFileName), "w"); //wipes out the file
	char* objectFileName = strCat(subString(sourceFileName, 0, strlen(sourceFileName) - 4), "Object.txt");
	FILE *ourObjectFile = fopen(strCat("./", objectFileName), "w"); //wipes out the file
	if (ourListingFile != NULL && ourObjectFile != NULL)
	{
		FILE *ourIntermediateFile_INSTRUCT = fopen(strCat("./", intermediateFileName), "r");

		if (ourIntermediateFile_INSTRUCT != NULL)
		{
			int instructID = 0;
			char* SourceLine; //source line
			char* LOCCTR; //locctr
			char* Label; //label
			char* Mnemonic; //mnemonic
			char* Operand; //operand
			char* Comment; //comments
			char* Errors; //errrors

			int currLineCount = 0;
			int readingInstructions = 1;

			//keep reading our intermediate file until there is nothing left
			while (getline(&line, &len, ourIntermediateFile_INSTRUCT) != -1)
			{
				if (readingInstructions == 1) {
					if (strcmp(subString(line, 0, strlen(line) - 1), "---Symbol Table (string -> int)") == 0)
						readingInstructions = 0;
					else //else actually reading an instruction
					{
						//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]
						instructID++; //1 -> 8
						switch (instructID)
						{
						case 1: SourceLine = stringCopy(line);  removeSpacesBack(&SourceLine); break;
						case 2: 
							LOCCTR = stringCopy(line);  
							removeSpacesBack(&LOCCTR); 
							if (isEmpty(LOCCTR) == 1)
								LOCCTR = strCat(LOCCTR, "    ");
							else
								LOCCTR = b10Str_To_b16Str(LOCCTR);
							break;
						case 3: Label = stringCopy(line);  removeSpacesBack(&Label); break;
						case 4: Mnemonic = stringCopy(line);  removeSpacesBack(&Mnemonic); break;
						case 5: Operand = stringCopy(line);  removeSpacesBack(&Operand); break;
						case 6: Comment = stringCopy(line);  removeSpacesBack(&Comment); break;
						case 7: Errors = stringCopy(line);  removeSpacesBack(&Errors); break;
						case 8:

							//---change up our loop counters
							currLineCount++;
							instructID = 0;
							int instructNumber = (8 * currLineCount);

							//-------------------------create object code start-------------------------
							if (isBlankLine(SourceLine) == 0) 
							{
								if (errorInErros("910", Errors) == 1)
									LOCCTR = "XXXX";

								char * objectCode = returnEmptyString();

								if (SourceLine[0] == '.') //we have found a comment
									objectCode = strCat(objectCode, "      ");
								else //we have found an instruction
								{
									if (errorInErros("210", Errors) == 0) //you have an valid mnemonic (directive -or- operation)
									{
										if (strlen(Mnemonic) != 2) //we are processing a VALID directive
										{
											if (
												errorInErros("400", Errors) == 0 &&
												errorInErros("410", Errors) == 0 &&
												errorInErros("420", Errors) == 0 &&
												errorInErros("430", Errors) == 0 &&
												errorInErros("440", Errors) == 0 &&
												errorInErros("450", Errors) == 0 &&
												errorInErros("460", Errors) == 0 &&
												errorInErros("470", Errors) == 0 &&
												errorInErros("480", Errors) == 0 &&
												errorInErros("490", Errors) == 0
												)
											{ //We have a VALID operand (varies)

												if (
													errorInErros("x200x", Errors) == 0 && //we have an extra start directive
													errorInErros("x140x", Errors) == 0 //we dont have a directive (only a label)
													)
												{ //We have a VALID mnemonic

													if (strcmp(Mnemonic, "start") == 0) //guaranteed to only happen once
													{ 
														//---PREP to write listing file
														if (errorInErros("x910x", Errors) == 0) 
														{
															objectCode = strCat(objectCode, LOCCTR);

															//--write to object file (IF VALID)
															if (
																//make sure our label is valid
																errorInErros("x120x", Errors) == 0 &&
																errorInErros("x130x", Errors) == 0 &&
																//make sure our program length is valid
																errorInErros("x900x", Errors) == 0
																) 
															{
																char * progLen = b10Int_To_b16Str(programLength);
																fputs(
																	strCat("H", //indicate we have a header record
																		strcat(Label, //print the VALID label of this start directive
																			strCat(concatFront(objectCode, 6 - strlen(objectCode), '0'), //add the start of the program
																				strCat(concatFront(progLen, 6 - strlen(progLen), '0'), //add the length of the program (from param)
																					"\n"
																				)
																			)
																		)
																	),
																	ourObjectFile
																);
															}
															//ELSE... our lable is invalid so we dont write to our object file
														}
														else
															objectCode = strCat(objectCode, "XXXX");
														int add0sp = 6 - strlen(objectCode);
														objectCode = concatBack(objectCode, add0sp, ' ');
													}
													else if (strcmp(Mnemonic, "end") == 0) //guaranteed to only happen once
													{ 
														//---PREP to write to listing file
														objectCode = "      "; //no object code is produced for our listing file
														LOCCTR = "      ";

														//--write to object file (IF VALID)
														int index = getKeyIndexSYMTBL(Operand);
														if(index == -1) //the label we are searching for is not in our symbol table
															Errors = strCatFreeFirst(&Errors, "x420x");
														else {
															char * operand16 = b10Int_To_b16Str(symbolTbl[index].value);
															fputs(
																strCat("E", //indicate we have a header file
																	strCat(concatFront(operand16, 6 - strlen(operand16), '0'),
																		"\n"
																	)
																), ourObjectFile
															);
														}
													}
													else if (strcmp(Mnemonic, "byte") == 0) {
														//TODO... (we only care for what is inbetween the ' and other ') IF x mode(copy thing over) ELSE IF c mode(convert text to hex and copy that over)

														char * between;
														if (Operand[0] == 'x')
															between = subString(Operand, 2, strlen(Operand) - 3);
														else
															between = lettersToHex(subString(Operand, 2, strlen(Operand) - 3));

														int add0sp = 6 - strlen(between);
														if (add0sp > 0)
															objectCode = strCat(objectCode, concatBack(between, add0sp, ' '));
														else
															objectCode = strCat(objectCode, between);
													}
													else if (strcmp(Mnemonic, "word") == 0) {
														char* base16 = b10Str_To_b16Str(Operand);
														int add0sp = 6 - strlen(base16);
														objectCode = strCat(objectCode, concatFront(base16, add0sp, '0'));
													}
													else if (strcmp(Mnemonic, "resb") == 0) {
														objectCode = strCat(objectCode, "4096  ");
													}
													else { //this MUST be "RESW"
														objectCode = strCat(objectCode, "1     ");
													}
												}
												else
													objectCode = strCat(objectCode, "DIRXXX");
											}
											else //we have an INVALID operand (varies)
												objectCode = strCat(objectCode, "DIRXXX");
										}
										else //we are processsing a VALID operation (NOTE: we know the operation is valid because we succesfully retreived its opcode)
										{
											if (strcmp("4C", Mnemonic) == 0) //we have the RSUB operation
												objectCode = strCat(Mnemonic, "0000");
											else //we have any other operation (operand should be a label) [make sure label is valid]
											{
												if (
													errorInErros("300", Errors) == 0 &&
													errorInErros("310", Errors) == 0 &&
													errorInErros("320", Errors) == 0 &&
													errorInErros("330", Errors) == 0
													)
												{ //We have a VALID operand (Label)
													if (Operand[strlen(Operand) - 2] == ',') //indexed label
													{
														char * indy = subString(Operand, 0, strlen(Operand) - 2); //get rid of ,X)
														int index = getKeyIndexSYMTBL(indy);
														if (index == -1) { //we did not find this label in our symbol table
															Errors = strCatFreeFirst(&Errors, "x330x");
															objectCode = strCat(Mnemonic, "XXXX");
														}
														else {
															int val = symbolTbl[index].value;
															val += 32768;
															objectCode = strCat(Mnemonic, b10Int_To_b16Str(val));
														}
													}
													else //non indexed label
													{
														int index = getKeyIndexSYMTBL(Operand);
														if (index == -1) { //we did not find this label in our symbol table
															Errors = strCatFreeFirst(&Errors, "x330x");
															objectCode = strCat(Mnemonic, "XXXX");
														}
														else
															objectCode = strCat(Mnemonic, b10Int_To_b16Str(symbolTbl[index].value));
													}
												}
												else //we have an INVALID operand (label)
													objectCode = strCat(Mnemonic, "XXXX");
											}
											//NOTE: we are NOT addressing operations (add | and | div | mul | or | sub)
										}
									}
									else //we dont have a VALID mnemonic so by definition its impossible to have a VALID operand
										objectCode = concatFront(objectCode, 6, 'X');

									//this is edited so we always have object code of size 6
									int add0s = 6 - strlen(objectCode);
									objectCode = concatFront(objectCode, add0s, '0');
								}

								//-------------------------create object code end-------------------------

								//---print to file
								fputs(strCat(itoa10(instructNumber), "\t"), ourListingFile); //line number (aprox solid size)
								fputs(strCat(LOCCTR, "\t"), ourListingFile); //locctr (solid size)
								fputs(strCat(objectCode, "\t"), ourListingFile); //object code (solid size)
								if(longestErrorLine > 0)
									fputs(strCat(concatBack(Errors, longestErrorLine - strlen(Errors), ' '), "\t"), ourListingFile); //error line (variable size but small)
								fputs(strCat(SourceLine, "\t"), ourListingFile); //source line (variable size but large)
								fputs("\n", ourListingFile);

								//---free all memory
								//TODO... 
							}
							//ELSE... we ignore the blank line

							break;
						default:
							break;
						}
					}
				}
				//ELSE... read until end of line but ignore
			}

			//TODO... find out why this is causing a segmentation fault
			//fclose(ourIntermediateFile_INSTRUCT); //close our intermediate file after reading it
			fclose(ourObjectFile); //close our object file after reading it
		}
		else
			printf("ERROR --- INTERMEDIATE file did not open properly\n"); //THE ONLY ERROR THAT CANNOT BE IN THE INTERMEDIATE FILE
		
		printf("READING 2\n\n");

		fclose(ourListingFile); //close our intermediate file after writing to it
	}
	else
		printf("ERROR --- LISTING file -or- OBJECT file did not open properly\n");
}