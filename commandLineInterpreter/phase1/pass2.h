/*
*/

#pragma region  Library Includes

#pragma once

//sic engine tie in
#include "sic.h"

//library includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#pragma endregion

#pragma region Prototypes

char* b10Str_To_b16Str(char* base10);
char* b10Int_To_b16Str(int b10Num);

char int_To_Char(int i);

char* concatFront(char *str, int quantity, char c);
char* concatBack(char *str, int quantity, char c);

int errorInErros(char * error, char * errors);

#pragma endregion


#pragma region Stuff also in Pass 1 (Part 1)

//NOTE: I could just call the functions in pass 1 but I don't have the time to try to make it work (after many attempts I was unsuccessful)

void freeMem_2(char **line);

//---SPECIFIC String Processing Prototypes (require repairs)
char* processFirst_2(char** l); //CHECK after repairing substrings
char* processRest_2(char** l);
int removeSpacesFront_2(char** l); //CHECK after repairing substrings
int removeSpacesBack_2(char** l); //works

								//---GENERAL String Processing Prototypes
char* stringCopy_2(char* str);
char* subString_2(char* src, int srcIndex, int strLength);

//---Integer To String Prototypes
char* reverse_2(char* str);
char* itoa10_2(int num);
char* itoa16_2(int num); //TODO... implement

					   //---Helper Prototypes
char* returnEmptyString_2();
void stringToLower_2(char** l);
char* strCat_2(char* startValue, char* addition);
char* strCatFreeFirst_2(char** fS, char* lastString);

//---Error Checking Prototypes
//for line
int isBlankLine_2(char* line);
int isEmpty_2(char* charArray);
//for label
int isLabel_2(char* line);
//for mnemonic
int isDirective_2(char* mneumonic);
//for operand
int isNumber10_2(char* num);
int isNumber16_2(char* num);

//---Symbol Table Prototypes
int resetSYMTBL_2();
int addSYMTBL_2(char* key, int value);
//NOTE: no removal function needed (for now)
int setSYMTBL_2(char* key, int value);
int containsKeySYMTBL_2(char* key);
int getKeyIndexSYMTBL_2(char* key);
int containsValueSYMTBL_2(int value);
int getValueIndexSYMTBL_2(int value);
void printSymbolTable_2();

//---Symbol Table Global Vars
#define MAX_SYMBOLS_2 500

typedef struct charToInt_2 keyToValue_2;
struct charToInt_2 {
	char* key;
	int value;
};

keyToValue_2 symbolTbl_2[MAX_SYMBOLS_2]; //keys must be a continuous stream of characters
int emptyIndex_2;

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

	//-------------------------1st File Reading(to populate symbol table)-------------------------
	//NOTE: I could just pass over the symbol table from pass 1 but I don't have the time to try to make it work (after many attempts I was unsuccessful)

	FILE *ourIntermediateFile_SYMTBL = fopen(strCat_2("./", intermediateFileName), "r");
	if (ourIntermediateFile_SYMTBL != NULL)
	{
		int readingInstructions = 1;
		int firstTime = 1;

		//keep reading our intermediate file until there is nothing left
		while (getline(&line, &len, ourIntermediateFile_SYMTBL) != -1)
		{
			if (strcmp(subString_2(line, 0, strlen(line) - 1), "---Symbol Table (string -> int)") == 0)
				readingInstructions = 0;

			if (readingInstructions == 0) //we are reading in the symbol table
			{
				if (firstTime == 1)
					firstTime = 0;
				else {
					char * label = processFirst_2(&line);
					if (addSYMTBL_2(label, atoi(processFirst_2(&line))) == -2)
						break;
				}
			}
		}

		fclose(ourIntermediateFile_SYMTBL); //close our intermediate file after reading it
	}
	else
		printf("ERROR --- INTERMEDIATE file did not open properly\n"); //THE ONLY ERROR THAT CANNOT BE IN THE INTERMEDIATE FILE

	printf("READING 1\n\n");

	//-------------------------2nd File Reading(to process the data in the intermeditae file)-------------------------

	char* listingFileName = strCat_2(subString_2(sourceFileName, 0, strlen(sourceFileName) - 4), "Listing.txt");
	FILE *ourListingFile = fopen(strCat_2("./", listingFileName), "w"); //wipes out the file
	if (ourListingFile != NULL)
	{
		FILE *ourIntermediateFile_INSTRUCT = fopen(strCat_2("./", intermediateFileName), "r");

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
					if (strcmp(subString_2(line, 0, strlen(line) - 1), "---Symbol Table (string -> int)") == 0)
						readingInstructions = 0;
					else //else actually reading an instruction
					{
						//INT FILE:  [1]copy, [2]locctr, [3]label, [4]mnemonics[operations](looked up)[directive], [5]operand(looked up), [6]comments, [7]errors, [\n]
						instructID++; //1 -> 8
						switch (instructID)
						{
						case 1: SourceLine = stringCopy_2(line);  removeSpacesBack_2(&SourceLine); break;
						case 2: 
							LOCCTR = stringCopy_2(line);  
							removeSpacesBack_2(&LOCCTR); 
							if (isEmpty_2(LOCCTR) == 1)
								LOCCTR = strCat_2(LOCCTR, "    ");
							break;
						case 3: Label = stringCopy_2(line);  removeSpacesBack_2(&Label); break;
						case 4: Mnemonic = stringCopy_2(line);  removeSpacesBack_2(&Mnemonic); break;
						case 5: Operand = stringCopy_2(line);  removeSpacesBack_2(&Operand); break;
						case 6: Comment = stringCopy_2(line);  removeSpacesBack_2(&Comment); break;
						case 7: Errors = stringCopy_2(line);  removeSpacesBack_2(&Errors); break;
						case 8:

							//---change up our loop counters
							currLineCount++;
							instructID = 0;
							int instructNumber = (8 * currLineCount);

							//-------------------------create object code start-------------------------

							char * objectCode = returnEmptyString_2();

							if (SourceLine[0] == '.') //we have found a comment
								objectCode = strCat_2(objectCode, "      ");
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

												//start		-> locctr counter
												//end		-> no obj code
												//byte		-> (we only care for what is inbetween the ' and other ') IF x mode (copy thing over) ELSE IF c mode (convert text to hex and copy that over)
												//word		-> convert operand to HEX... copy over... make sure the string is 6 spaces larger (if no concat 0s in front)
												//resb		-> literally just 4096
												//resw		-> literally just 1

												objectCode = strCat_2(objectCode, "DIRYYY");
											}
											else
												objectCode = strCat_2(objectCode, "DIRXXX");
										}
										else //we have an INVALID operand (varies)
											objectCode = strCat_2(objectCode, "DIRXXX");
									}
									else //we are processsing a VALID operation (NOTE: we know the operation is valid because we succesfully retreived its opcode)
									{
										if (strcmp("4C", Mnemonic) == 0) //we have the RSUB operation
											objectCode = strCat_2(Mnemonic, "0000");
										else //we have any other operation (operand should be a label) [make sure label is valid]
										{
											if (
												errorInErros("300", Errors) == 0 &&
												errorInErros("310", Errors) == 0 &&
												errorInErros("320", Errors) == 0 &&
												errorInErros("330", Errors) == 0
												)
											{ //We have a VALID operand (Label)
												if(Operand[strlen(Operand)-2] == ',')
													objectCode = strCat_2(Mnemonic, "INDY");
												else
													objectCode = strCat_2(Mnemonic, "REGY");
											}
											else //we have an INVALID operand (label)
												objectCode = strCat_2(Mnemonic, "XXXX");
										}
										//NOTE: we are NOT addressing operations (add | and | div | mul | or | sub)
									}
								}
								else //we dont have a VALID mnemonic so by definition its impossible to have a VALID operand
									objectCode = concatFront(objectCode, 6, 'X');
							}

							//this is edited so we always have object code of size 6
							int add0s = 6 - strlen(objectCode);
							char * objectCodeForListing = concatFront(objectCode, add0s, '0');

							//-------------------------create object code end-------------------------

							//---print to file
							fputs(strCat_2(itoa10_2(instructNumber),"\t"), ourListingFile); //line number (aprox solid size)
							fputs(strCat_2(LOCCTR, "\t"), ourListingFile); //locctr (solid size)
							fputs(strCat_2(objectCodeForListing, "\t"), ourListingFile); //object code (solid size)
							fputs(Errors, ourListingFile); //error line (variable size but small)
							fputs(strCat_2(SourceLine, "\t"), ourListingFile); //source line (variable size but large)
							fputs("\n", ourListingFile);

							//---free all memory
							//TODO... 

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
		}
		else
			printf("ERROR --- INTERMEDIATE file did not open properly\n"); //THE ONLY ERROR THAT CANNOT BE IN THE INTERMEDIATE FILE
		
		printf("READING 2\n\n");

		fclose(ourListingFile); //close our intermediate file after writing to it
	}
	else
		printf("ERROR --- LISTING file did not open properly\n");

	/*
	the listing file (that contains the result of assembly) we are creating should include
	-address of statement
	-machine code equivalent
	-source statement (including comments)
	-errors associated with each statement
	*/

	/*
	the object file we are creating should include
	-results of assembly
	-Format varies according to designer
	-Usually read by a linker/loader program
	-Several record types (this also can vary)
	-header record
	-linkage records
	-code records
	-end record
	*/

	//TODO figure out what to do with linkage records...
}

#pragma region Helper Functions

char* b10Str_To_b16Str(char* base10) 
{
	return b10Int_To_b16Str(atoi(base10));
}

char* b10Int_To_b16Str(int b10Num) 
{
	//prep string
	char *result = returnEmptyString_2();

	//concat all remainders
	while (b10Num > 15)
	{
		int res = b10Num / 16;
		int rem = b10Num - (res * 16);
		b10Num = res;

		char* temp = concatFront(result, 1, int_To_Char(rem));
		freeMem_2(&result);
		result = temp;
	}

	//concat last quotient
	char* temp = concatFront(result, 1, int_To_Char(b10Num));
	freeMem_2(&result);
	result = temp;

	//return string
	return result;
}

char int_To_Char(int i) {
	switch (i)
	{
	case 0: return '0';
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	case 10: return 'A';
	case 11: return 'B';
	case 12: return 'C';
	case 13: return 'D';
	case 14: return 'E';
	default: return 'F';
	}
}

char* concatFront(char *str, int quantity, char c)
{
	//memory allocation
	char *newStr = malloc((strlen(str)+quantity+1) * sizeof(char));
	int index = 0;
	//concat desired char
	for (int i = 0; i < quantity; i++, index++)
		newStr[index] = c;
	//concat rest of string
	for (int i = 0; i < strlen(str); i++, index++)
		newStr[index] = str[i];
	//null terminator
	newStr[index] = '\0';
	//pass by reference
	return newStr;
}

char* concatBack(char *str, int quantity, char c) 
{
	char *newStr = malloc((strlen(str) + quantity + 1) * sizeof(char));
	int index = 0;
	//concat rest of string
	for (int i = 0; i < strlen(str); i++, index++)
		newStr[index] = str[i];
	//concat desired char
	for (int i = 0; i < quantity; i++, index++)
		newStr[index] = c;
	//null terminator
	newStr[index] = '\0';
	//pass by reference
	return newStr;
}

//NOTE: this only works because of the very specific ways errors where coded into the project
int errorInErros(char * error, char * errors) //we ASSUME the length of errors are 3 digits
{
	int indexOfError = 0;

	for (int i = 0; i < strlen(errors); i++) 
	{
		if (error[indexOfError] == errors[i])
			indexOfError++; //continue searching this potential match
		else
			indexOfError = 0; //this is not a potential match
		if (indexOfError == 3)
			return 1;
	}

	return 0; //we tried searching all the errors and did not locate the one we were looking for
}

#pragma endregion


#pragma region Stuff Also in Pass 1 (Part 2)

void freeMem_2(char **l) {
	char* line = *l;
	if (strlen(line) > 0)
		free(line);
}

char* processFirst_2(char** l) //actually return our first word found, by reference "return" the line
{
	char* line = *l; //link up to our value (so we can pass by reference)

	if (line[0] != '\0') //make sure we have a line left
	{
		removeSpacesFront_2(&line);

		//make sure we have string left to check after getting rid of all spaces
		if (isEmpty_2(line) == 1)
			return returnEmptyString_2();
		else
		{
			//add anything that isnt a space to our word
			int firstLength = 0;
			while (isspace(line[firstLength]) == 0 && line[firstLength] != '\0')
				firstLength++;

			//NOTE: inclusive index for FIRST start is firstCharIndex... exclusive index for FIRST end is lineID...
			//size of FIRST is  (lineID - firstCharIndex)
			//NOTE: inclusive index for LINE start is lineID... exclusive index for LINE end is NOT RELEVANT (use size)
			//size of LINE is (MAX_SIZE_CONST - lineID)
			//BEWARE: lineID IS NOT ALWAYS A SPACE

			//calculate first substring
			char *first = subString_2(line, 0, firstLength); //memory allocated by substring

														   //calculate line substring
			int sizeOfLine = (strlen(line) - firstLength);
			char *lineWithoutFirst = subString_2(line, firstLength, sizeOfLine);
			*l = lineWithoutFirst;
			freeMem_2(&line);

			return first;
		}
	}
	else
		return returnEmptyString_2();
}

char* processRest_2(char** l) { //remove spaces in front of the line that its passed... return a new string that is exactly the same as the string passed without spaces
	char* line = *l;
	removeSpacesFront_2(&line); //remove the front of the line by reference...
	return stringCopy_2(line); //return a copy of the line without spaces in front...
}

int removeSpacesFront_2(char** l) { //returns how many spaces where removed

	char* line = *l;

	if (strlen(line) > 0) {
		//var init
		int lineID = 0;

		//ignore anything that is a space
		while (isspace(line[lineID]) != 0 && line[lineID] != '\0')
			lineID++;

		char* resultLine;

		//make sure we have string left to check after getting rid of all spaces
		if (line[lineID] == '\0')
			resultLine = returnEmptyString_2(); //nothing useful is left in the line
		else {
			resultLine = subString_2(line, lineID, (strlen(line) - lineID));

			*l = resultLine;
		}

		//free(line); //TODO... I beleive this should work but it doesnt

		return lineID;
	}
	else {
		line = returnEmptyString_2();
		return 0;
	}
}

//fills all available spots with null terminators (since space was probably already alocated for the string and we might use it eventually)
int removeSpacesBack_2(char** l) { //"returns" by reference
	char *line = *l;

	if (strlen(line) > 0) {
		int count = 0;
		int charID = strlen(line) - 1; //we skip the null terminator
		while (charID >= 0 && isspace(line[charID]) != 0) {
			line[charID] = '\0';
			charID--;
			count++;
		}
		line[charID + 1] = '\0';
		return count;
	}
	else {
		line = returnEmptyString_2();
		return 0;
	}
}

//-------------------------GENERAL String Processing Functions-------------------------

char* stringCopy_2(char* str) {
	return subString_2(str, 0, strlen(str));
}

char* subString_2(char* src, int srcStartIndex, int subStringLength) {

	int srcI = srcStartIndex;
	int destI = 0;

	char* dest = malloc((subStringLength + 1) * sizeof(char)); //+1 for null terminator

	int counter = subStringLength;
	while (counter > 0 && srcStartIndex < strlen(src)) {
		counter--;
		dest[destI] = src[srcI];
		destI++;
		srcI++;
	}

	dest[subStringLength] = '\0';

	return dest;
}

//-------------------------Integer To String Functions-------------------------

char* reverse_2(char* str)
{
	if (strlen(str) > 0) {
		char* strCopy = stringCopy_2(str);

		int indexF2B = 0;
		int indexB2F = strlen(str) - 1;

		while (indexF2B < strlen(str)) {
			strCopy[indexF2B] = str[indexB2F];
			indexF2B++;
			indexB2F--;
		}
		strCopy[strlen(str)] = '\0';

		return strCopy;
	}
	else
		return returnEmptyString_2();
}

char* itoa10_2(int num)
{
	char *base10Str;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		base10Str = malloc(2 * sizeof(char));
		base10Str[0] = '0';
		base10Str[1] = '\0';
		return base10Str;
	}
	else
	{
		//---REMOVE negative sign (If we have one)

		int isNeg = 0;
		if (num < 0) {
			num *= -1; //make the number positive
			isNeg = 1; //make it as originally negative
		}

		//---ALLOCATE space for our positive number

		//get size of this number so we can properly allocate space
		int digits = 0;
		int numCopy = num;
		//NOTE: we can assume that at the very least we have 1 digit
		do {
			digits++;
			numCopy = (numCopy / 10); //remove the last digit
		} while (numCopy != 0);

		base10Str = malloc((digits + 1) * sizeof(char)); //digits + null terminator

														 //---CONVERT our positive number

		int index = 0;
		while (num != 0)
		{
			int rem = num % 10;
			base10Str[index] = (rem + '0'); //(rem > 9) ? (rem - 10) + 'a' : rem + '0';
			index++;
			num = num / 10;
		}

		base10Str[index] = '\0'; // Append string terminator  

		char *base10StrRev = reverse_2(base10Str); // Reverse the string
		freeMem_2(&base10Str);

		//---ADD negative sign (if needed)

		char *result;
		if (isNeg == 1)
			result = strCat_2("-", base10StrRev);
		else
			result = stringCopy_2(base10StrRev);
		freeMem_2(&base10StrRev);

		return result;
	}
}

char* itoa16_2(int num)
{
	return '\0';
}

//-------------------------Helper Functions-------------------------

char* returnEmptyString_2()
{
	char* aStr = malloc(sizeof(char));
	aStr[0] = '\0';
	return aStr;
}

void stringToLower_2(char** l) { //"returns" by reference

	char* line = *l;

	if (strlen(line) > 1) {
		for (int i = 0; i < strlen(line); i++)
			line[i] = tolower(line[i]);
	}
	else
		line = returnEmptyString_2();
}

char* strCat_2(char* firstString, char* lastString)
{
	int newLength = (strlen(firstString) + strlen(lastString)); //EX: 4 + 3 = 7 chars
	char* newString = malloc((newLength + 1) * sizeof(char)); //EX: allocate 7 slots (+ 1 for null terminator)

	int index = 0;
	int firstStringIndex = 0;
	int lastStringIndex = 0;
	while (index < newLength) { //EX: 7 chars indices 0 -> 6 [7]
		if (index < strlen(firstString)) //EX: 4 chars indices 0 -> 3 [4]
		{
			newString[index] = firstString[firstStringIndex];
			firstStringIndex++;
		}
		else
		{
			newString[index] = lastString[lastStringIndex];
			lastStringIndex++;
		}
		index++;
	}
	newString[newLength] = '\0'; //at last index insert null terminator

	return newString;
}

char* strCatFreeFirst_2(char** fS, char* lastString)
{
	char* firstString = *fS;

	int newLength = (strlen(firstString) + strlen(lastString)); //EX: 4 + 3 = 7 chars
	char* newString = malloc((newLength + 1) * sizeof(char)); //EX: allocate 7 slots (+ 1 for null terminator)

	int index = 0;
	int firstStringIndex = 0;
	int lastStringIndex = 0;
	while (index < newLength) { //EX: 7 chars indices 0 -> 6 [7]
		if (index < strlen(firstString)) //EX: 4 chars indices 0 -> 3 [4]
		{
			newString[index] = firstString[firstStringIndex];
			firstStringIndex++;
		}
		else
		{
			newString[index] = lastString[lastStringIndex];
			lastStringIndex++;
		}
		index++;
	}

	newString[newLength] = '\0'; //at last index insert null terminator

	freeMem_2(&firstString);

	return newString;
}

//-------------------------Error Checking Functions-------------------------

int isBlankLine_2(char* line) {
	for (int i = 0; i < strlen(line); i++)
		if (isspace(line[i]) == 0)
			return 0;
	return 1;
}


int isEmpty_2(char* charArray) //we only have a null terminator
{
	if (strcmp(charArray, "") == 0 || charArray[0] == '\0')
		return 1;
	else
		return 0;
}

int isLabel_2(char* line) {
	if (isspace(line[0]) == 0)
		return 1;
	else
		return 0;
}

//inefficient but clean up code nicely
int isDirective_2(char* mneumonic) {
	if (strcmp(mneumonic, "start") == 0)
		return 1;
	else if (strcmp(mneumonic, "end") == 0)
		return 1;
	else if (strcmp(mneumonic, "byte") == 0)
		return 1;
	else if (strcmp(mneumonic, "word") == 0)
		return 1;
	else if (strcmp(mneumonic, "resb") == 0)
		return 1;
	else if (strcmp(mneumonic, "resw") == 0)
		return 1;
	else
		return 0;
}

int isNumber10_2(char* num) {
	for (int i = 0; i < strlen(num); i++)
		if (isdigit(num[i]) == 0)
			return 0;
	return 1;
}

int isNumber16_2(char* num) {
	for (int i = 0; i < strlen(num); i++)
		if (isxdigit(num[i]) == 0)
			return 0;
	return 1;
}

//-------------------------Symbol Table Functions (Symbol | Location)-------------------------

int resetSYMTBL_2() {
	for (int i = 0; i < emptyIndex_2; i++) {
		symbolTbl_2[i].key = ""; // we dont need to free memory since we always keep the same 500 slots for any other assembly anyways
		symbolTbl_2[i].value = -1;
	}
	emptyIndex_2 = 0;
}

int addSYMTBL_2(char* key, int value) { //returns 1 if success, 0 if value must be set, -1 if symbolTbl full, -2 if the key passed is no key at all

	if (isEmpty_2(key) == 0) {
		if (emptyIndex_2 < MAX_SYMBOLS_2) {
			int newValueInsert = emptyIndex_2; //location we insert the key and value into their respective arrays
			if (getKeyIndexSYMTBL_2(key) == -1) //they key is not in our arrays
			{
				//NOTE: allocating memory for KEY is not nessesary
				symbolTbl_2[newValueInsert].key = key; //save char
				symbolTbl_2[newValueInsert].value = value;

				emptyIndex_2++;
				return 1;
			}
			else
				return 0;
		}
		else
			return -1;
	}
	else
		return -2;
}

int setSYMTBL_2(char* key, int value) { //returns 1 if key and value pairing found and update, 0 otherwise

	int oldValueOverwrite = getKeyIndexSYMTBL_2(key); //get the index of key...
	if (oldValueOverwrite != -1) { //they key is not in our arrays
								   //old value overwritten
		symbolTbl_2[oldValueOverwrite].value = value;
		return 1;
	}
	else
		return 0;
}

//NOTE: no removal function needed (for now)

int containsKeySYMTBL_2(char* key) {
	if (getKeyIndexSYMTBL_2(key) != -1) //it exists
		return 1;
	else
		return 0;
}

int getKeyIndexSYMTBL_2(char* key) {
	for (int i = 0; i < emptyIndex_2; i++)
		if (strcmp(symbolTbl_2[i].key, key) == 0)
			return i;
	return -1;
}

int containsValueSYMTBL_2(int value) {
	if (getValueIndexSYMTBL_2(value) != -1) //it exists
		return 1;
	else
		return 0;
}

int getValueIndexSYMTBL_2(int value) {
	for (int i = 0; i < emptyIndex_2; i++)
		if ((symbolTbl_2[i].value) == value)
			return i;
	return -1;
}

void printSymbolTable_2() {
	printf("---Symbol Table (string -> int)\n");
	for (int i = 0; i < emptyIndex_2; i++)
		printf("'%s' maps to '%i'\n", symbolTbl_2[i].key, symbolTbl_2[i].value);
	printf("\n");
}

#pragma endregion