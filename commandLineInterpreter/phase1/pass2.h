/*
Programmer: Bryan Cancel
Last Updated: 4/27/18

Deliverables
1. a documented source listing
2. two listing of assembler language source files (one with errors, one with no errors)
3. a copy of the listing file and the object files generated
4. a copy of this should be in the 3334/phase3 directory
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

void pass2(char *sourceFileName, char * intermediateFileName, int programLength)
{
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

	//-------------------------2nd File Reading(to process the data in the intermeditae file)-------------------------

	char* listingFileName = strCat(subString(sourceFileName, 0, strlen(sourceFileName) - 4), "Listing.txt");
	FILE *ourListingFile = fopen(strCat("./", listingFileName), "w"); //wipes out the file
	char* objectFileName = strCat(subString(sourceFileName, 0, strlen(sourceFileName) - 4), "Object.txt");
	FILE *ourObjectFile = fopen(strCat("./", objectFileName), "w"); //wipes out the file
	printf("Listing File: '%s'\n", listingFileName);
	printf("Object File: '%s'\n", objectFileName);
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

			int textRecordSize = 0;
			int textRecordBegin = 0;
			char* textRecordInstructs = returnEmptyString();
			char* textRecordAddress = returnEmptyString();

			int forceTextRecordCreation = 0;
			char* endRecord = returnEmptyString();

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
								LOCCTR = b10Str_To_b16Str(LOCCTR,0);
							break;
						case 3: Label = stringCopy(line);  removeSpacesBack(&Label); break;
						case 4: Mnemonic = stringCopy(line);  removeSpacesBack(&Mnemonic); break;
						case 5: Operand = stringCopy(line);  removeSpacesBack(&Operand); break;
						case 6: Comment = stringCopy(line);  removeSpacesBack(&Comment); break;
						case 7: Errors = stringCopy(line);  removeSpacesBack(&Errors); break;
						case 8:

							forceTextRecordCreation = 0;

							//---change up our loop counters
							currLineCount++;
							instructID = 0;
							int instructNumber = (8 * currLineCount);

							//-------------------------create object code start-------------------------
							if (isBlankLine(SourceLine) == 0) 
							{
								int textRecord_A_Size = 0;
								char *textRecord_A_Instruct = returnEmptyString();

								if (errorInErros("910", Errors) == 1)
									LOCCTR = "XXXX";

								char * objectCode = returnEmptyString();

								if (SourceLine[0] == '.') //we have found a comment
									objectCode = strCat(objectCode, "      ");
								else //we have found an instruction
								{
									if (errorInErros("210", Errors) == 0) //you have an valid mnemonic (directive -or- operation)
									{
										//-------------------------prep for obj file start-------------------------

										if (textRecordBegin == 0) {
											textRecordBegin = 1;
											textRecordAddress = concatFront(LOCCTR, 6 - strlen(LOCCTR), '0');
										}

										if (strlen(Mnemonic) != 2) //we are processing a VALID directive
										{
											if (
												//check for operand errors for these directives
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
																char * progLen = b10Int_To_b16Str(programLength,0);
																if (longestErrorLine <= 1) {
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
															forceTextRecordCreation = 1;
															char * operand16 = b10Int_To_b16Str(symbolTbl[index].value,0);
															endRecord = strCat("E", //indicate we have a header file
																strCat(concatFront(operand16, 6 - strlen(operand16), '0'),
																	"\n"
																)
															);
														}
													}
													else if (strcmp(Mnemonic, "byte") == 0) 
													{
														if (Operand[0] == 'x') { //ALSO guranteed to be evenn lengthed (by pass 1 error checking)
															char *between = subString(Operand, 2, strlen(Operand) - 3);
															textRecord_A_Size = (strlen(between)/2);
															textRecord_A_Instruct = stringCopy(between);
															objectCode = strCat(objectCode, concatBack(between, 6 - strlen(between), ' '));
														}
														else //NOTE: adding 0s here for the sake of priting will skew what the objectCode means (since each pair of digits or byte maps to a character)
														{ 
															//---extract the ONLY CASE SENSITIVE Operand 
															//remember (1) we know for a fact that there are no errors (2) byte directive can have labels but dont need to
															char* sourceCopy = stringCopy(SourceLine);
															char* Operand_CaseSensitive = returnEmptyString();
															//remove label
															if (hasLabel(sourceCopy) == 1)
																processFirst(&sourceCopy);
															//remove byte directive
															processFirst(&sourceCopy);
															//remove operand
															Operand_CaseSensitive = processFirst(&sourceCopy);

															//---covert the CASE SENSITIVE Operand Into hex
															objectCode = lettersToHex(subString(Operand_CaseSensitive, 2, strlen(Operand_CaseSensitive) - 3)); //GURANTED to be even lengthed (can be larger than 3 bytes)
															textRecord_A_Size = (strlen(objectCode)/2);
															textRecord_A_Instruct = stringCopy(objectCode);
														}
														//NOTE: we have to make sure these values are passed in sets of bytes (even length digits on the object code)
													}
													else if (strcmp(Mnemonic, "word") == 0) {
														char* base16 = b10Str_To_b16Str(Operand,0);
														int add0sp = 6 - strlen(base16);
														objectCode = strCat(objectCode, concatFront(base16, add0sp, '0'));
														textRecord_A_Size = 3;
														textRecord_A_Instruct = stringCopy(objectCode);
													}
													else if (strcmp(Mnemonic, "resb") == 0) {
														forceTextRecordCreation = 1; //instead of allocating spaces 
														objectCode = strCat(objectCode, "4096  ");
														textRecord_A_Instruct = returnEmptyString(); //"  ";
													}
													else { //this MUST be "RESW"
														forceTextRecordCreation = 1; //instead of allocating spaces
														objectCode = strCat(objectCode, "1     ");
														textRecord_A_Instruct = returnEmptyString(); //"      ";
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
															objectCode = strCat(Mnemonic, b10Int_To_b16Str(val,0));
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
															objectCode = strCat(Mnemonic, b10Int_To_b16Str(symbolTbl[index].value,0));
													}
												}
												else //we have an INVALID operand (label)
													objectCode = strCat(Mnemonic, "XXXX");
											}
											//NOTE: we are NOT addressing operations (add | and | div | mul | or | sub)

											//check oject code to see if this operation is fully valid or not
											int xFound = 0;
											for (int i = 0; i < strlen(objectCode); i++) {
												if (objectCode[i] == 'X') {
													xFound = 1;
													break;
												}
											}

											if (xFound == 0) { //we have a valid operation
												textRecord_A_Size += 3;
												textRecord_A_Instruct = stringCopy(objectCode);
											}
										}

										//-------------------------prep for obj file end-------------------------
									}
									else //we dont have a VALID mnemonic so by definition its impossible to have a VALID operand
										objectCode = concatFront(objectCode, 6, 'X');

									//this is edited so we always have object code of size 6
									int add0s = 6 - strlen(objectCode);
									objectCode = concatFront(objectCode, add0s, '0');
								}

								if (longestErrorLine <= 1) {

									//(0) for when we find a RESW or RESB and there is nothing written into the current record
									if(forceTextRecordCreation == 1 && textRecordSize == 0)
										textRecordBegin = 0;
									else 
									{
										//(1) text records are getting too long 
										//(2) we found the end record
										//(3) after reading in some portion of code into a text record we find a RESW or a RESB
										int futureSize = (textRecordSize + textRecord_A_Size);
										if (futureSize >= 30 || forceTextRecordCreation == 1) //we need to write out the record and reset everything
										{
											if (futureSize > 30)
											{
												fputs(
													strCat("T", //indicate its a text record
														strCat(textRecordAddress, //print the address where this record starts
															strCat(b10Int_To_b16Str(textRecordSize, 1), //print the quantity of bytes in this record
																strCat(textRecordInstructs, //print the instructions
																	"\n"
																)
															)
														)
													)
													, ourObjectFile
												);

												//we have already begun
												textRecordAddress = concatFront(LOCCTR, 6 - strlen(LOCCTR), '0'); //we already have the address													  
												textRecordInstructs = stringCopy(textRecord_A_Instruct); //we already have the first instruct
												textRecordSize = textRecord_A_Size;
											}
											else
											{
												textRecordInstructs = strCatFreeFirst(&textRecordInstructs, textRecord_A_Instruct); //keep adding instructions
												textRecordSize += textRecord_A_Size;
												fputs(
													strCat("T", //indicate its a text record
														strCat(textRecordAddress, //print the address where this record starts
															strCat(b10Int_To_b16Str(textRecordSize, 1), //print the quantity of bytes in this record
																strCat(textRecordInstructs, //print the instructions
																	"\n"
																)
															)
														)
													)
													, ourObjectFile
												);

												textRecordBegin = 0;
												//we grab the address this next iteration
												//we will receive the first instruct next iteration
												textRecordInstructs = returnEmptyString();
												textRecordSize = 0;
											}

											if (forceTextRecordCreation == 1)
												fputs(endRecord, ourObjectFile);
										}
										else //we need to keep can keep adding to this text record
										{
											textRecordInstructs = strCatFreeFirst(&textRecordInstructs, textRecord_A_Instruct); //keep adding instructions
											textRecordSize += textRecord_A_Size;
										}
									}
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
			fclose(ourIntermediateFile_INSTRUCT); //close our intermediate file after reading it
			fclose(ourObjectFile); //close our object file after reading it
		}
		else
			printf("ERROR --- INTERMEDIATE file did not open properly\n"); //THE ONLY ERROR THAT CANNOT BE IN THE INTERMEDIATE FILE
		
		printf("PASS 2 COMPLETE\n\n");

		//---print the symbol table at the end of the listing file
		fputs("---Symbol Table (string -> int)\n", ourListingFile);
		for (int i = 0; i < emptyIndex; i++) {

			char* symStr = strCat(symbolTbl[i].key, " ");
			char* val = itoa10(symbolTbl[i].value);

			symStr = strCatFreeFirst(&symStr, val);
			symStr = strCatFreeFirst(&symStr, "\n");
			fputs(symStr, ourListingFile);

		}
		fputs("\n", ourListingFile);

		fclose(ourListingFile); //close our intermediate file after writing to it
	}
	else
		printf("ERROR --- LISTING file -or- OBJECT file did not open properly\n");
}