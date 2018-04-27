/*
Programmer: Bryan Cancel
Last Updated: 4/27/18
*/

#pragma once

#pragma region Constants

#define min(a,b) (((a) < (b)) ? (a) : (b))

//explicit limitations
#define MAX_SYMBOLS 500
#define MAX_OPCODES 25
#define MAX_PROGRAM_SIZE  32767
#define MAX_SYMBOL_SIZE 7 //6 spots and null terminator
//implicit limitations
#define MAX_DIRECTIVE_SIZE 6 //5 spots and null terminator
#define MAX_LOCCTR_SIZE 65535 //(FF,FF) base 16
#define MAX_SIZE  16777215 //(FF,FF,FF) base 16

#pragma endregion

#pragma region Data Structures

//---Symbol Table Global Vars
typedef struct charToInt keyToValue;
struct charToInt {
	char* key;
	int value;
};

keyToValue symbolTbl[MAX_SYMBOLS]; //keys must be a continuous stream of characters
int emptyIndex;

#pragma endregion

#pragma region Prototypes

char* b10Str_To_b16Str(char* base10, int isInByteFormat);
char* b10Int_To_b16Str(int b10Num, int isInByteFormat);

char int_To_Char(int i);

char* concatFront(char *str, int quantity, char c);
char* concatBack(char *str, int quantity, char c);

int errorInErros(char * error, char * errors);

char* lettersToHex(char * letters);

#pragma endregion

#pragma region Helper Function Prototypes

void freeMem(char **line);

//---SPECIFIC String Processing Prototypes (require repairs)
char* processFirst(char** l); //CHECK after repairing substrings
char* processRest(char** l);
int removeSpacesFront(char** l); //CHECK after repairing substrings
int removeSpacesBack(char** l); //works

								//---GENERAL String Processing Prototypes
char* stringCopy(char* str);
char* subString(char* src, int srcIndex, int strLength);

//---Integer To String Prototypes
char* reverse(char* str);
char* itoa10(int num);
char* itoa16(int num); //TODO... implement

					   //---Helper Prototypes
char* returnEmptyString();
void stringToLower(char** l);
char* strCat(char* startValue, char* addition);
char* strCatFreeFirst(char** fS, char* lastString);

//---Error Checking Prototypes
//for line
int isBlankLine(char* line);
int isEmpty(char* charArray);
//for label
int isLabel(char* line);
int isValidLabel(char* label);
//for mnemonic
int isDirective(char* mneumonic);
//for operand
int isNumber10(char* num);
int isNumber16(char* num);

//---Symbol Table Prototypes
int resetSYMTBL();
int addSYMTBL(char* key, int value);
//NOTE: no removal function needed (for now)
int setSYMTBL(char* key, int value);
int containsKeySYMTBL(char* key);
int getKeyIndexSYMTBL(char* key);
int containsValueSYMTBL(int value);
int getValueIndexSYMTBL(int value);
void printSymbolTable();

//---OpCode Table Prototypes
int containsOpCode(char* operand);
char* getOpCode(char* operand);
char* indexToOpName(int index);
char* indexToOpCode(int index);
void printOpCodeTable();

#pragma endregion

#pragma region Functions

char* b10Str_To_b16Str(char* base10, int forceByteFormat)
{
	return b10Int_To_b16Str(atoi(base10), forceByteFormat);
}

char* b10Int_To_b16Str(int b10Num, int forceByteFormat)
{
	//prep string
	char *result = returnEmptyString();

	//concat all remainders
	while (b10Num > 15)
	{
		int res = b10Num / 16;
		int rem = b10Num - (res * 16);
		b10Num = res;

		char* temp = concatFront(result, 1, int_To_Char(rem));
		freeMem(&result);
		result = temp;
	}

	//concat last quotient
	char* temp = concatFront(result, 1, int_To_Char(b10Num));
	freeMem(&result);
	result = temp;

	//add 0 if needed
	if (forceByteFormat == 1 && strlen(result) % 2 != 0) {
		char * temp2 = concatFront(result, 1, '0');
		freeMem(&result);
		result = temp2;
	}

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
	char *newStr = malloc((strlen(str) + quantity + 1) * sizeof(char));
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

char* lettersToHex(char * letters) {
	char * hex = returnEmptyString();
	for (int i = 0; i < strlen(letters); i++) {
		char * temp = b10Int_To_b16Str((int)letters[i], 0); //return number (0 -> 255)base 10 -OR- (00 -> FF)base 16 -AS- a string in hex
														 //printf("char '%c' to int '%i' to string '%s'\n", letters[i], (int)letters[i], temp);
		hex = strCatFreeFirst(&hex, temp);
	}
	return hex;
}

#pragma endregion

#pragma region HELPER FUNCTIONS

void freeMem(char **l) {
	char* line = *l;
	if (strlen(line) > 0)
		free(line);
}

//-------------------------SPECIFIC String Processing Functions-------------------------

char* processFirst(char** l) //actually return our first word found, by reference "return" the line
{
	char* line = *l; //link up to our value (so we can pass by reference)

	if (line[0] != '\0') //make sure we have a line left
	{
		removeSpacesFront(&line);

		//make sure we have string left to check after getting rid of all spaces
		if (isEmpty(line) == 1)
			return returnEmptyString();
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
			char *first = subString(line, 0, firstLength); //memory allocated by substring

														   //calculate line substring
			int sizeOfLine = (strlen(line) - firstLength);
			char *lineWithoutFirst = subString(line, firstLength, sizeOfLine);
			*l = lineWithoutFirst;
			freeMem(&line);

			return first;
		}
	}
	else
		return returnEmptyString();
}

char* processRest(char** l) { //remove spaces in front of the line that its passed... return a new string that is exactly the same as the string passed without spaces
	char* line = *l;
	removeSpacesFront(&line); //remove the front of the line by reference...
	return stringCopy(line); //return a copy of the line without spaces in front...
}

int removeSpacesFront(char** l) { //returns how many spaces where removed

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
			resultLine = returnEmptyString(); //nothing useful is left in the line
		else {
			resultLine = subString(line, lineID, (strlen(line) - lineID));

			*l = resultLine;
		}

		//free(line); //TODO... I beleive this should work but it doesnt

		return lineID;
	}
	else {
		line = returnEmptyString();
		return 0;
	}
}

//fills all available spots with null terminators (since space was probably already alocated for the string and we might use it eventually)
int removeSpacesBack(char** l) { //"returns" by reference
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
		line = returnEmptyString();
		return 0;
	}
}

//-------------------------GENERAL String Processing Functions-------------------------

char* stringCopy(char* str) {
	return subString(str, 0, strlen(str));
}

char* subString(char* src, int srcStartIndex, int subStringLength) {

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

char* reverse(char* str)
{
	if (strlen(str) > 0) {
		char* strCopy = stringCopy(str);

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
		return returnEmptyString();
}

char* itoa10(int num)
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

		char *base10StrRev = reverse(base10Str); // Reverse the string
		freeMem(&base10Str);

		//---ADD negative sign (if needed)

		char *result;
		if (isNeg == 1)
			result = strCat("-", base10StrRev);
		else
			result = stringCopy(base10StrRev);
		freeMem(&base10StrRev);

		return result;
	}
}

char* itoa16(int num)
{
	return '\0';
}

//-------------------------Helper Functions-------------------------

char* returnEmptyString()
{
	char* aStr = malloc(sizeof(char));
	aStr[0] = '\0';
	return aStr;
}

void stringToLower(char** l) { //"returns" by reference

	char* line = *l;

	if (strlen(line) > 1) {
		for (int i = 0; i < strlen(line); i++)
			line[i] = tolower(line[i]);
	}
	else
		line = returnEmptyString();
}

char* strCat(char* firstString, char* lastString)
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

char* strCatFreeFirst(char** fS, char* lastString)
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

	freeMem(&firstString);

	return newString;
}

//-------------------------Error Checking Functions-------------------------

int isBlankLine(char* line) {
	for (int i = 0; i < strlen(line); i++)
		if (isspace(line[i]) == 0)
			return 0;
	return 1;
}


int isEmpty(char* charArray) //we only have a null terminator
{
	if (strcmp(charArray, "") == 0 || charArray[0] == '\0')
		return 1;
	else
		return 0;
}

int isLabel(char* line) {
	if (isspace(line[0]) == 0)
		return 1;
	else
		return 0;
}

int isValidLabel(char* label) { //1 is true, 0 is false because first value is not digit, -1 is false because it too long
	if (strlen(label) < MAX_SYMBOL_SIZE) {
		if (isdigit(label[0]) == 0)
			return 1;
		else
			return 0;
	}
	else
		return -1;
}

//inefficient but clean up code nicely
int isDirective(char* mneumonic) {
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

int isNumber10(char* num) {
	for (int i = 0; i < strlen(num); i++)
		if (isdigit(num[i]) == 0)
			return 0;
	return 1;
}

int isNumber16(char* num) {
	for (int i = 0; i < strlen(num); i++)
		if (isxdigit(num[i]) == 0)
			return 0;
	return 1;
}

//-------------------------Symbol Table Functions (Symbol | Location)-------------------------

int resetSYMTBL() {
	for (int i = 0; i < emptyIndex; i++) {
		symbolTbl[i].key = ""; // we dont need to free memory since we always keep the same 500 slots for any other assembly anyways
		symbolTbl[i].value = -1;
	}
	emptyIndex = 0;
}

int addSYMTBL(char* key, int value) { //returns 1 if success, 0 if value must be set, -1 if symbolTbl full

	if (emptyIndex < MAX_SYMBOLS) {
		int newValueInsert = emptyIndex; //location we insert the key and value into their respective arrays
		if (getKeyIndexSYMTBL(key) == -1) //they key is not in our arrays
		{
			//NOTE: allocating memory for KEY is not nessesary
			symbolTbl[newValueInsert].key = key; //save char
			symbolTbl[newValueInsert].value = value;

			emptyIndex++;
			return 1;
		}
		else
			return 0;
	}
	else
		return -1;
}

int setSYMTBL(char* key, int value) { //returns 1 if key and value pairing found and update, 0 otherwise

	int oldValueOverwrite = getKeyIndexSYMTBL(key); //get the index of key...
	if (oldValueOverwrite != -1) { //they key is not in our arrays
								   //old value overwritten
		symbolTbl[oldValueOverwrite].value = value;
		return 1;
	}
	else
		return 0;
}

//NOTE: no removal function needed (for now)

int containsKeySYMTBL(char* key) {
	if (getKeyIndexSYMTBL(key) != -1) //it exists
		return 1;
	else
		return 0;
}

int getKeyIndexSYMTBL(char* key) {
	for (int i = 0; i < emptyIndex; i++)
		if (strcmp(symbolTbl[i].key, key) == 0)
			return i;
	return -1;
}

int containsValueSYMTBL(int value) {
	if (getValueIndexSYMTBL(value) != -1) //it exists
		return 1;
	else
		return 0;
}

int getValueIndexSYMTBL(int value) {
	for (int i = 0; i < emptyIndex; i++)
		if ((symbolTbl[i].value) == value)
			return i;
	return -1;
}

void printSymbolTable() {
	printf("---Symbol Table (string -> int)\n");
	for (int i = 0; i < emptyIndex; i++)
		printf("'%s' maps to '%i'\n", symbolTbl[i].key, symbolTbl[i].value);
	printf("\n");
}

//-------------------------Op Code Table Functions (Operation | OpCode)-------------------------

int containsOpCode(char* operand) {
	if (strcmp(getOpCode(operand), "-1") == 0)
		return 1;
	else
		return 0;
}

char* getOpCode(char* operand) {
	for (int index = 0; index < MAX_OPCODES; index++)
		if (strcmp(indexToOpName(index), operand) == 0)
			return indexToOpCode(index);

	return "-1";
}

char* indexToOpName(int index)
{
	switch (index)
	{
	case 0:		return "add";	break;
	case 1:		return "and";	break;
	case 2:		return "comp";	break;
	case 3:		return "div";	break;
	case 4:		return "j";		break;
	case 5:		return "jeq";	break;
	case 6:		return "jgt";	break;
	case 7:		return "jlt";	break;
	case 8:		return "jsub";	break;
	case 9:		return "lda";	break;
	case 10:	return "ldch";	break;
	case 11:	return "ldl";	break;
	case 12:	return "ldx";	break;
	case 13:	return "mul";	break;
	case 14:	return "or";	break;
	case 15:	return "rd";	break;
	case 16:	return "rsub";	break;
	case 17:	return "sta";	break;
	case 18:	return "stch";	break;
	case 19:	return "stl";	break;
	case 20:	return "stx";	break;
	case 21:	return "sub";	break;
	case 22:	return "td";	break;
	case 23:	return "tix";	break;
	case 24:	return "wd";	break;
	default:	return "";		break;
	}
}

char* indexToOpCode(int index)
{
	switch (index)
	{
	case 0:		return "18";	break;
	case 1:		return "58";	break;
	case 2:		return "28";	break;
	case 3:		return "24";	break;
	case 4:		return "3C";	break;
	case 5:		return "30";	break;
	case 6:		return "34";	break;
	case 7:		return "38";	break;
	case 8:		return "48";	break;
	case 9:		return "00";	break;
	case 10:	return "50";	break;
	case 11:	return "08";	break;
	case 12:	return "04";	break;
	case 13:	return "20";	break;
	case 14:	return "44";	break;
	case 15:	return "D8";	break;
	case 16:	return "4C";	break;
	case 17:	return "0C";	break;
	case 18:	return "54";	break;
	case 19:	return "14";	break;
	case 20:	return "10";	break;
	case 21:	return "1C";	break;
	case 22:	return "E0";	break;
	case 23:	return "2C";	break;
	case 24:	return "DC";	break;
	default:	return "";		break;
	}
}

void printOpCodeTable() {
	printf("---Op Code Table (string -> int)\n");
	for (int i = 0; i < MAX_OPCODES; i++)
		printf("'%s' maps to '%s'\n", indexToOpName(i), indexToOpCode(i));
	printf("\n");
}

#pragma endregion