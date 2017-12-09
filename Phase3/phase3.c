#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define NUMELEM(x) (sizeof(x) / sizeof(x[0]))


void split(char *, char *, char *, char *, int *); //Splits Line into words
int getcmd(char *, int *); //Returns Value for giving command
void loadFile(char *,char *); //Load file (Not Implemented yet)
void exe(char *); //Execute laoded file
void db(); //Opens in debug mode
void dmp(char *, char *); //Dump file
void assemble(int, bool); //Assemble SIC files
char* itoa(int value, char* result, int base);
int decToHexa(int n);

void addLabel(int); //Adds label to Struct
//void addAddress(int); //Add Address of label to struct
bool labelIsFound();

bool LookForLabel(char *);

int GetMemLoc(char *, FILE *); //Gets Memmory Location of Address
short GetOpcode(char *);

void err(); //show Error 
void hlp(); //Show help

FILE *source, *intermediate, *opcode, *symbolTable; //FILES

//Labels
typedef struct{
	char label[10];
	int address;
}LABELS;

LABELS lab[500];
int labelsCounter = 0; //Keep track of how many label are in the system

typedef struct {
	char *label;
	char *mnemonic;
	char *operand;
} TOKEN;

TOKEN tok;

typedef struct {
	char OP[7];
	short FORM;
} OPCODE;

//Short should be hexadecimal
OPCODE Ops[] = {{"ADD", 0x18},   {"AND", 0x58},    {"COMP", 0x28},   {"DIV", 0x24},
                {"J", 0x3C},     {"JEQ", 0x30},    {"JGT", 0x34},    {"JLT", 0x38},
                {"JSUB", 0x48},   {"LDA", 0x00},    {"LDCH", 0x50},   {"LDL", 0x08},
                {"LDX", 0x04},   {"MUL", 0x20},    {"OR", 0x44},    {"RD", 0xD8},
                {"RSUB", 0x4C},   {"STA", 0x0C},     {"STCH", 0x54},   {"STL", 0x14},
                {"STX", 0x10}, {"SUB", 0x1C}, {"TD", 0xE0}, {"TIX", 0x2C}, {"WD", 0xDC}};

int main()
{
	typedef enum {ERROR = -1,LOAD,EXECUTE,DEBUG,DUMP,DIRECTORY,ASSEMBLE,HELP,EXIT} COMMAND; //Each command get enumed
	char line[80], cmd[50], p1[50], p2[50];
	char buff[255];
	int len, n;
	while(true) //Loops Until user enters "Exit"
	{
	printf("Hail Master$ ");
	fgets(line,80, stdin); //Gets input from user

	len = strlen(line) - 1;
	if(line[len] == '\n')
		line[len] = '\0';

	split(line,cmd,p1,p2,&n); //Split Line into Words [Command] [Parameter1] [Parameter2]
	
	switch(getcmd(cmd, &n)) //Gets the value, and executes
	{
		case LOAD:
			loadFile(p1,buff);
			break;
		case EXECUTE:
			exe(buff);
			break;
		case DEBUG:
			db();
			break;
		case DUMP:
			dmp(p1,p2);
			break;
		case DIRECTORY:
			system("ls");
			break;
		case ASSEMBLE:
			//assemble(p1);
			break;
		case HELP:
			hlp();
			break;
		case EXIT:
			exit(0);
			break;
		case ERROR:
			err();
			break;
		default:
			printf("'%s': not found, type 'help'\n", cmd);
			break;
	}
	}


	//Testing
	printf("\n%d things on this line\n", n);
	printf("Command: %s\n", cmd);
	printf("Parameter 1: %s\n", p1);
	printf("parameter 2: %s\n", p2);


	return 0;
}

int decToHexa(int n)
{   
    // char array to store hexadecimal number
    char hexaDeciNum[100];
     
    // counter for hexadecimal number array
    int i = 0;
    while(n!=0)
    {   
        // temporary variable to store remainder
        int temp  = 0;
         
        // storing remainder in temp variable.
        temp = n % 16;
         
        // check if temp < 10
        if(temp < 10)
        {
            hexaDeciNum[i] = temp + 48;
            i++;
        }
        else
        {
            hexaDeciNum[i] = temp + 55;
            i++;
        }
         
        n = n/16;
    }
    //Reverse
    int len = (int)strlen(hexaDeciNum) - 1;
    char temp[100];
    int tempIndex = 0;
    while(len >= 0)
    {
    	temp[tempIndex++] = hexaDeciNum[len--];
    }
    return (int)strtol(temp, NULL, 10);
}

void split(char *str, char *c, char *p1, char *p2, int *n) //Splits Line into words
{
	c[0] = p1[0] = p2[0] = '\0'; //Start Everything on NULL
	//Initialized
	int addinx = 0, count = 1;
	bool cmd = false, param1 = false, param2 = false;
	
	//Loops the whole line
	for(int indx = 0; indx < 80; indx++)
	{
		if(str[indx] == '\n' || str[indx] == '\0'){break;} //Exits loop if Next character is "New line"
		
		if(str[indx] == ' ') //Check if Character is a space
		{
			if(c[0] != '\0'){cmd = true;} // If command array is not empty Change bool to true
			if(p1[0] != '\0'){param1 = true;}// If Parameter array is not empty change bool to true 
			if(p2[0] != '\0'){param2 = true;}// if parameter2 array is not emty change bool to true
			addinx = 0;//Reset Index Used for [Command] [P1][p2]
			continue;//Continues with the loop
		}

		else if(str[indx] != ' ') //If current line index is not space -> add to The Current Array
		{
			if(str[indx + 1] == ' ' && str[indx + 2] != ' ') //Check at space if next is not space add to count
				count++;
			if(cmd == false) // Goes into Command array if False
			{
				c[addinx] = str[indx];
				addinx++;
				c[addinx] = '\0';
				continue;
			}
			if(param1 == false) // Goes into param1  array if false
			{
				p1[addinx] = str[indx];
				addinx++;
				p1[addinx] = '\0';
				continue;
			}
			if(param2 == false) // Goes into param2 array if false
			{
				p2[addinx] = str[indx];
				addinx++;
				p2[addinx] = '\0';
				continue;
			}
		}		
	}
	*n = count; //Set N to count, count being the number of words counted + 1 -.-
}

int GetMemLoc(char *tk, FILE *symb)
{
	char *token;
	char line[500];
	int addr;
	//Open File
	symbolTable = fopen("symbolTable.txt", "r");

	//printf("=========\n");
	//Loop over File
	while(fgets(line, 500, symbolTable))
	{
		line[strcspn(line, "\n")] = '\0'; // Remove New Line and Add null
		//Memory Address
		token = strtok(line, " \t");
		addr = (int)strtol(token, NULL, 10);

		//Get Symbol
		token = strtok(NULL, " \t");
		
		//printf("%s v %s\n", token, tk);
		
		if(strcmp(token, tk) == 0)
		{
			fclose(symbolTable);
			//printf("=========\n");
			return addr;
		}
	}
	fclose(symbolTable);
	//printf("=========\n");
	return 1;
	//fclose(symbolTable);

}
short GetOpcode(char *s)
{
	for(int x = 0; x < NUMELEM(Ops); ++x)
	{
		//printf("Inside GetOpCode: %x", Ops[x].FORM);//(int)strtol(Ops[x].OP, NULL, 16));
		if(strcmp(Ops[x].OP, s) == 0) //If its the same
			return Ops[x].FORM;
	}
}

int getcmd(char *c, int *n) //Return a value to the corresponding Command
{
	if(strcmp(c, "load") == 0){
		if(*n - 1 >= 2 || *n - 1 < 1) return -1; //Check for correct number of paramete
		return 0;
	}
	else if(strcmp(c, "execute") == 0)return 1;
	else if(strcmp(c, "debug") == 0)return 2;
	else if(strcmp(c, "dump") == 0){
		if(*n - 1 >= 3 || *n - 1 <= 1) return -1; //Checks for correct number of parameter
		return 3;
	}
	else if(strcmp(c, "dir") == 0 || strcmp(c, "directory")==0) return 4;
	else if(strcmp(c, "assemble") == 0){
		if(*n - 1 >= 2 || *n - 1 < 1) return -1; //Checks for correct numbe of parameters
		return 5;
	}
	else if(strcmp(c, "help") == 0) return 6;
	else if(strcmp(c, "exit") == 0){
		if(*n >1) return -1;
		return 7;
	}
	return -2;
}

void addLabel(int locCounter)
{
	if(labelsCounter >= 500) return; //Stop adding more

	static int labelCounter = 0;
	strcpy(lab[labelCounter].label, tok.label);
	lab[labelCounter++].address = locCounter;
	labelsCounter++;
}

bool labelIsFound()
{
	for(int i = 0; i < labelsCounter; i++)
	{
		if(strcmp(lab[i].label, tok.label) == 0) return true;
	}
	
	return false;
}

bool LookForLabel(char *label)
{
	//Loop over Labels
	for(int i = 0; i <= 500; i++)
	{
		if(strcmp(label, lab[i].label) == 0) return true; //Label Found
	}

	//Label Not Found
	return false;
}

void assemble(int progSize, bool anyErrors)
{
	//printf("Assemble Function\n");
	//File Declared
	FILE *objectFile, *listingFile;
	//Opening Files
	objectFile = fopen("objectFile.txt", "w");
	listingFile = fopen("listingFile.txt", "w");
	intermediate = fopen("intermediate.txt", "r");
	symbolTable = fopen	("symbolTable.txt", "r");
	//Local Struct
	typedef struct{
		int address;
		char *label;
		char *mnemonic;
		char *operand;
		int err;
	} ASSEMBLE;
	ASSEMBLE ObjFile;

	//Allocating Memory
	ObjFile.label = (char *)malloc(6);
	ObjFile.mnemonic = (char *)malloc(6);
	ObjFile.operand = (char *)malloc(6);
	//Done Allocating Memory

	//Declare Variables
	char line[500];
	char *token; 
	bool _FoundLabel;

	char *machineCode = (char*) malloc(100); // 60
	char *incommingMachineCode = (char *) malloc(100);
	char *sizeOfMachineCode = (char *) malloc(5); // 2
	char *tMnemonic = (char *) malloc(10);
	char *tOperand = (char *) malloc(10);
	int addre;
	int objAddress;
	memset(incommingMachineCode, '\0', 100);
	memset(machineCode, '\0', 100);
	// Loop over the Source "AGAIN"
	while(fgets(line, 500, intermediate))
	{
		//memset(machineCode, '\0', 100);
		//memset(incommingMachineCode, '\0', 100)

		//Get Address and Store
		token = strtok(line, " \t");
		ObjFile.address = (int)strtol(token, NULL, 10);
		//Done Storing Address

		//Get Next Token 
		token = strtok(NULL, " \t");
		//Look For Label
		if(LookForLabel(token)) //If Label HAs been Found 
		{
			//Store Label
			strcpy(ObjFile.label, token);
			//Get Next Token
			token = strtok(NULL, " \t");
		}
		//Regardless, Store Mnemonic
		strcpy(ObjFile.mnemonic, token);


		if(strcmp(ObjFile.mnemonic, "RESB") == 0 || strcmp(ObjFile.mnemonic, "RESW") == 0) // If RSUB
		{
			//printf("%s Encountered\n", ObjFile.mnemonic);
			// Print and Reset machineCode and incomming MachineCode
			if(strlen(machineCode) > 0)
			{
				fprintf(objectFile, "T%02x %x %s\n", objAddress, strlen(machineCode) / 2, machineCode);
				memset(machineCode, '\0', 100);
				objAddress = addre;
			}
		}
		//Done Storing Data...
		else if(strcmp(ObjFile.mnemonic, "START") == 0) //First Line
		{	
			//Get Operand
			token = strtok(NULL, " \t");
			//Print to File
			fprintf(objectFile, "H%s_%06d%05X\n", ObjFile.label, (int)strtol(token, NULL, 10), progSize);

			//Get Error Code
			token = strtok(NULL, " \t");
			//Store
			ObjFile.err = atoi(token);
		}
		else if(strcmp(ObjFile.mnemonic, "END") == 0)
		{
			//End of file

			//Get Token
			token = strtok(NULL, " \t");
			int addr = GetMemLoc(token, symbolTable);

			//Print End of File
			fprintf(objectFile, "E%06d", addr);
			return;
		}
		else
		{
			//Gets Operand
			token = strtok(NULL, " \t");
			strcpy(ObjFile.operand, token);
			if(strcmp(ObjFile.mnemonic, "RSUB") != 0)
			{
				token = strtok(NULL, " \t");				
			}
			token[strcspn(token, "\n")] = '\0';
			ObjFile.err = atoi(token);
			//Operand is Save

			if(strlen(incommingMachineCode) > 0)
			{
				
				//Checks For BYE
				if(strcmp(ObjFile.mnemonic, "BYTE") == 0)
				{
					//Create a String
					char *byte = (char *) malloc(7);
					//Set the String to 0 Expect the last NULL
					memset(byte, '0', 6);
					
					if(ObjFile.operand[0] == 'C')
					{
						//Get What it's inside of the Single ' '
						int cIndex = 2;

						while(ObjFile.operand[cIndex] != '\'')
						{
							//Make a variable that will store the character
							char *var = (char *)malloc(2);
							//set the first index to the character found
							var[0] = ObjFile.operand[cIndex];
							var[1] = '\0';
							//Convert Letter to Interger
							int hex = (int)var[0];
							//Create a string that will concat everyhing
							char *cat = (char *)malloc(6);
							//Conver to String
							cat = itoa(hex, cat, 16);
							//Concat
							strcat(byte, cat);
							//Increments
							cIndex++;
						}

						//Concat To incomming Machine code
						strcat(incommingMachineCode, byte);
					}
					else if(ObjFile.operand[0] == 'X') //This will be an X
					{
						int cIndex = 2;
						int hIndex = 0;
						char *hex = (char *) malloc(10);
						memset(hex, '0', 6);

						while(ObjFile.operand[cIndex] != '\'')
						{
							//Copy The hexaDeciimals into the Array
							hex[hIndex] = ObjFile.operand[cIndex];
							//Increment
							hIndex++;
							cIndex++;
						}
						//Change Hex to Actual Hex
						int WORD = (int)strtol(hex, NULL, 16);
						char *_word = (char *) malloc(10);
						//memset(_word, '0', 6);

						itoa(WORD, _word, 16);

						strcat(incommingMachineCode, _word);
					}
				}

				else
				{
					objAddress = addre;
					//There is Something inide incommingMachineCode Get Rid of it
					strcat(machineCode, incommingMachineCode);
				}
				
			}

			if(strlen(incommingMachineCode) <= 0)
			{
				//printf("incommingMachineCode <= 0: %s \n", ObjFile.mnemonic);
				if(strcmp(ObjFile.mnemonic, "BYTE") == 0)
				{
					printf("BYTE %s \n", ObjFile.operand);
					//Create a String
					char *byte = (char *) malloc(7);
					//Set the String to 0 Expect the last NULL
					memset(byte, '0', 6);
					
					if(ObjFile.operand[0] == 'C')
					{
						//Get What it's inside of the Single ' '
						int cIndex = 2;

						//printf("C - Index1 %c \n C - Index2 %c\n", ObjFile.mnemonic[1], ObjFile.mnemonic[2]);

						while(ObjFile.operand[cIndex] != '\'')
						{
							printf("C Working %d\n", cIndex);
							//Make a variable that will store the character
							char *var = (char *)malloc(2);
							//set the first index to the character found
							var[0] = ObjFile.operand[cIndex];
							var[1] = '\0';
							//Convert Letter to Interger
							int hex = (int)var[0];
							//Create a string that will concat everyhing
							char *cat = (char *)malloc(6);
							//Conver to String
							cat = itoa(hex, cat, 16);
							//Concat
							strcat(byte, cat);
							//Increments
							cIndex++;
						}

						//Concat To incomming Machine code
						strcat(incommingMachineCode, byte);
					}
					else if(ObjFile.mnemonic[1] == '\'') //This will be an X
					{
						int cIndex = 2;
						int hIndex = 0;
						char *hex = (char *) malloc(10);
						memset(hex, '0', 6);

						while(ObjFile.operand[cIndex] != '\'')
						{
							printf("Working %d\n", hIndex);
							//Copy The hexaDeciimals into the Array
							hex[hIndex] = ObjFile.operand[cIndex];
							//Increment
							hIndex++;
							cIndex++;
						}
						//Change Hex to Actual Hex
						int WORD = (int)strtol(hex, NULL, 16);
						char *_word = (char *) malloc(10);
						memset(_word, '0', 6);

						itoa(WORD, _word, 16);

						strcat(incommingMachineCode, _word);
					}
				}
				else if(strcmp(ObjFile.mnemonic, "WORD") == 0)
				{
					//Change to int
					//int word = atoi(ObjFile.operand);
					char *_word = (char *) malloc(6);
					memset(_word, '0', 5);

					strcpy(_word, ObjFile.operand);

					printf("_word %s\n", _word);

					strcat(incommingMachineCode, _word);

				}
				else
				{
					addre = GetMemLoc(ObjFile.operand, symbolTable);
					//printf("addre: %d\n", addre);

					itoa(GetOpcode(ObjFile.mnemonic), tMnemonic, 16);
					strcat(incommingMachineCode, tMnemonic);

					itoa(addre, tOperand, 10);
					strcat(incommingMachineCode, tOperand);
				}
				
			}


			// READ FROM SOURCE "WRITE TO LISTING FILE"
			{
				
			}
			//END OF READING FROM SOURCE


			
			if(strlen(machineCode) == 0)
			{
				strcat(machineCode, incommingMachineCode);
			}
			else if(strlen(machineCode) + strlen(incommingMachineCode) > 60)
			{
				fprintf(objectFile, "T%02x %x %s\n", addre, strlen(machineCode), machineCode);
				memset(machineCode, '\0', 100);
			}
			else //Still space 
			{
				if(strcmp(ObjFile.mnemonic, "BYTE") == 0)
				{

				}
				else if(strcmp(ObjFile.mnemonic, "WORD") == 0)
				{
					int add = 3;

					char *cat = (char *) malloc(6);
					memset(cat, '0', 5);

					char *this = (char *) malloc(7);
					itoa(add, this, 10);

					strcat(incommingMachineCode, this);
					
				}
				else
				{
					//Cat
					strcat(machineCode, incommingMachineCode);
					//Clear incommingMachineCode
					memset(incommingMachineCode, '\0', 100);
				}
				

			}			//2 Hex = 1 Byte
			// 30 Bytes or 60 Hex in one Line Hence the 1E ??

			//T (Memory Location)(Size of Machine Code)(Machine Code)
			//fprintf(objectFile, "T%s%x%s", GetMemLoc(token, symbolTable), machineCode);
		}

	}
	fclose(objectFile);
	fclose(listingFile);
	fclose(intermediate);
	fclose(symbolTable);

	printf("Files closed");

	if(anyErrors)
	{
		if(remove("objectFile.txt") == 0)
			printf("Program Has Errors.\n");
	}


}

char* itoa(int value, char* result, int base)
{
		// check that the base if valid
		if (base < 2 || base > 36) { *result = '\0'; return result; }

		char* ptr = result, *ptr1 = result, tmp_char;
		int tmp_value;

		do {
			tmp_value = value;
			value /= base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
		} while ( value );

		// Apply negative sign
		if (tmp_value < 0) *ptr++ = '-';
		*ptr-- = '\0';
		while(ptr1 < ptr) {
			tmp_char = *ptr;
			*ptr--= *ptr1;
			*ptr1++ = tmp_char;
		}
		return result;
}

void loadFile(char *p1, char *buff) //Loads a files ITS ASSEMBLE :-(
{
	typedef enum { NOERROR = 0, INVALID_MNEMONIC, INVALID_LABEL, INVALID_OPERAND, DUP_LABEL, MIS_START, MISS_OPERAND, TO_MANY_LABELS, PROG_LONG } ERRORS;
//	FILE *source, *intermediate, *opcode, *symbolTable; // Creats the Files pointer
	char line[500]; // String for each line in the Files "p1"
	char *token; 	// Created for STRTOK
	int startingAddress, addressCounter;
	const char *symbols[500]; //Keep track of labels
	//Opening READ only FILES
	source = fopen(p1, "r");
	opcode = fopen("opcode.txt","w");
	//Opening Write Files
	intermediate = fopen("intermediate.txt","w");
	symbolTable = fopen("symbolTable.txt","w");
	if(source == NULL || opcode == NULL || intermediate == NULL || symbolTable == NULL) 
	{
		printf("File did not opened Correctly\n"); //Checking If Files Opened Correctly
		return;
	}

	//token = strtok(line, " \t");
	
	//=============================
	//Allocating memory
	tok.label = (char *) malloc(6);
	tok.mnemonic = (char *) malloc(6);
	tok.operand = (char *) malloc(6);
	//=============================
	
	char *pastToken; //Keep track of past tokens
	token = line;	
	int labelCounter = 0; //Keep track of labelss
	//Starts Reading from "p1"
	bool _ERRORS = false;
	bool anyErrors = false;
	while(fgets(line, 500, source))
	{
		bool labelFound = true, _Error = true;
		int _ERROR = NOERROR;
		
		if(line[0] == ' ' || line[0] == '\t') labelFound = false;
		if(line[0] == '.') continue;

		token = strtok(line, " \t\r\n\v\f");
		
		line[strcspn(line, "\n")] = '\0';

		if(labelFound)
		{
			int resMem = 0;
			strcpy(tok.label, token); //Sets label
			token = strtok(NULL, " \t\r\n\v\f"); // gets next word on the line
			
			if(labelsCounter > 0)
			{
				if(labelsCounter > 500) _ERROR = TO_MANY_LABELS;
				if(strcmp(tok.label, "START") == 0) _ERROR = MIS_START;
				if(labelIsFound() && _ERROR == NOERROR)
					_ERROR = DUP_LABEL;
			}
			strcpy(tok.mnemonic, token); //sets mnemonic
			
			token = strtok(NULL, " \t\r\n\v\f");

			strcpy(tok.operand, token); //sets operand

			if(strcmp(tok.mnemonic, "START") == 0) //Checks if "START" is present on the line
			{
				//printf("tok.operand %s\n", tok.operand);
				startingAddress = atoi(tok.operand);
				//printf("startinAddress %d\n", startingAddress);
				addressCounter = startingAddress;
				//printf("addressCounter %d\n", addressCounter);
			}

			//if(strcmp(tok.mnemonic, "START") == 0|| strcmp(tok.mnemonic, "STL") == 0) addressCounter = startingAddress;

			//Checking for ERRORS
			//======================================
			//Checking LABEL ERROR/
			if(tok.label[0] < 'A' || tok.label[0] > 'Z') _ERROR = INVALID_LABEL;
			//======================================

			//======================================
			//Checking if MNEMONIC is Wrong
			
			if( _ERROR == NOERROR)
			{
				for(int x = 0; x < NUMELEM(Ops); ++x)
				{
					if(strcmp(Ops[x].OP, tok.mnemonic) == 0) //If its the same
						_Error = false;
				}
				if(strcmp(tok.mnemonic, "START") == 0) _Error = false;
				else if(strcmp(tok.mnemonic, "END") == 0) _Error = false;
				else if(strcmp(tok.mnemonic, "BYTE") == 0) _Error  = false;
				else if(strcmp(tok.mnemonic, "WORD") == 0) _Error = false;
				else if(strcmp(tok.mnemonic, "RESB") == 0)  _Error = false;
				else if(strcmp(tok.mnemonic, "RESW") == 0)  _Error = false;
			}
			if(_Error && _ERROR == NOERROR) _ERROR = INVALID_MNEMONIC;
			//======================================
			
			//======================================
			//Checking for Directives
			if(strcmp(tok.mnemonic, "WORD") == 0)
			{
				resMem = 2;
			}
			if(strcmp(tok.mnemonic, "RESW") == 0)
			{
				resMem = 3 * (int)strtol(tok.operand, NULL, 10);
			}
			if(strcmp(tok.mnemonic, "RESB") == 0)
			{
				resMem = (int)strtol(tok.operand, NULL, 10);
				resMem = decToHexa(resMem);
			}
			if(strcmp(tok.mnemonic, "BYTE") == 0 && _ERROR == NOERROR)
			{
				//printf("%c\n", tok.operand[strlen(tok.operand) -1 ]);

				if(tok.operand[0] != 'X' && tok.operand[0] != 'C') _ERROR = INVALID_OPERAND; //NOT A C or an X at the start of byte
				else
				{
					if(tok.operand[0] == 'C')
					{
						//TEMP VARIABLES
						int charCount = 0;
						int index = 2;
						while(tok.operand[index++] != '\'')
						{
							charCount++;
						}
						resMem += charCount * 2;
					}
					else if(tok.operand[0] == 'X')
					{
						char *hex = (char*) malloc(6); // Allocating Memory for HEX
						int index = 1;

						int hexInx = 0;

						while(tok.operand[index++] != '\0')
						{
							if(tok.operand[index] == '\'') continue;// Making sure it doesnt break
							hex[hexInx] = tok.operand[index]; // copying whats inside;
							hexInx++;
						}
						int hexd = (int)strtol(hex, NULL, 16);
						resMem =  hexd;
					}
				}
				//else
				//{
				//	if(tok.operand[0] == 'X')
				//}
				if(tok.operand[1] != '\'' ||  tok.operand[strlen(tok.operand)-1] != '\'') _ERROR = INVALID_OPERAND; //Missing ' at the start and the end '
			}
			//if(strcmp(tok.mnemonic, "END") == 0 && strcmp(tok.operand, symbols[1]) > 0 || strcmp(tok.operand, symbols[1]) < 0 && _ERROR == NOERROR) _ERROR = INVALID_OPERAND;
			if(strcmp(tok.mnemonic, "START") == 0 || strcmp(tok.mnemonic, "RESB") == 0 || strcmp(tok.mnemonic, "RESW") == 0)
			{
				for(int i = 0; i < strlen(tok.operand); ++i)
				{
					if(!isdigit(tok.operand[i]))
					{
					    _ERROR = INVALID_OPERAND;
						break;
					}
				}
			}
			//======================================
			//PRINTING TO THE INTERMEDIATE
			fprintf(intermediate, "%d\t %s\t %s\t %s\t %d\n", addressCounter, tok.label, tok.mnemonic, tok.operand, _ERROR);
			fprintf(symbolTable, "%d\t %s\n", addressCounter, tok.label);
			//ADD TO LABEL STRUCT
			addLabel(addressCounter);
			//=======================
			//Increment Address Location
			if(strcmp(tok.mnemonic, "BYTE") == 0) addressCounter += resMem;
			else if(strcmp(tok.mnemonic, "RESB") == 0) addressCounter += resMem;
			else if(strcmp(tok.mnemonic, "RESW") == 0) addressCounter += resMem;
			else if(strcmp(tok.mnemonic, "WORD") == 0) addressCounter += resMem;
			else if(strcmp(tok.mnemonic, "START") > 0 || strcmp(tok.mnemonic, "START") < 0) addressCounter += 3;

			if(_Error > 0) _ERRORS = true;
		}
		else // NO LABEL
		{
			if(strcmp(token, "BYTE") == 0 || strcmp(token, "START") == 0 || strcmp(token, "WORD") == 0 || strcmp(token, "RESB") == 0 || strcmp(token, "RESW") == 0) _ERROR = INVALID_LABEL;	
			if(strcmp(token, "RSUB") == 0)
			{
				//addressCounter += 3; //CHANGE MEMORY LOCATION
				fprintf(intermediate, "%d\t\t %s\t\t %d\n", addressCounter, token, _ERROR);
				addressCounter += 3;
				continue;
			}

			strcpy(tok.mnemonic, token);
			token = strtok(NULL, " \t\r\n\v\f");
			if(token == NULL)
			{
				 _ERROR = MISS_OPERAND;
				strcpy(tok.operand, " ");	
			}
			else strcpy(tok.operand, token);
			if(_Error && _ERROR == NOERROR)
			{
				for(int x = 0; x < NUMELEM(Ops); ++x)
				{
					if(strcmp(Ops[x].OP, tok.mnemonic) == 0) //If its the same
					{
						_Error = false;
						break;
					}
				}
				if(strcmp(tok.mnemonic, "START") == 0) _Error = false;
				else if(strcmp(tok.mnemonic, "END") == 0) _Error = false;
				else if(strcmp(tok.mnemonic, "BYTE") == 0) _Error  = false;
				else if(strcmp(tok.mnemonic, "WORD") == 0) _Error = false;
				else if(strcmp(tok.mnemonic, "RESB") == 0)  _Error = false;
				else if(strcmp(tok.mnemonic, "RESW") == 0)  _Error = false;
			}

			if(_Error)
				_ERROR = INVALID_MNEMONIC;

			//Check for illegal operands

			//Things to check

			//1. C'Char'
			//2. X'Char'
			if(strcmp(tok.operand, "BYTE") == 0 && _ERROR == NOERROR) //ALREADY ADDED TO COUNTER INSIDE
			{
				//Check for opening ' and closing '
				if(tok.operand[1] != '\'' || tok.operand[strlen(tok.operand) - 1] != '\'') _ERROR = INVALID_OPERAND;
			}

			//If It doesnt not start with ' or does not end with '

				


			//End of Checking illegal operands
//			strcpy(tok.operand, token);
//			printf("This is the operand: \n", tok.operand);
			//Write to File
			//CHANGE MEMORY LOCATION
			//addressCounter += 3;
			fprintf(intermediate, "%d\t\t %s\t %s\t %d \n", addressCounter, tok.mnemonic, tok.operand, _ERROR);
			addressCounter += 3;
			if(_Error > 0) _ERRORS = true;
		}
		
		//Outside of If / else

		if(_Error > 0) //Any Error
			anyErrors = true;
		

	}
	
	//printf(" LABEL: %s\n MNEMONIC: %s\n OPERAND: %s\n", tok.label, tok.mnemonic, tok.operand);
	fclose(intermediate);
	fclose(source);
	fclose(opcode);
	fclose(symbolTable);
	assemble(addressCounter - startingAddress, anyErrors);

}
void exe(char *buff) //Executes Loaded File
{
	printf("Executing File\n");
	printf("%s\n",buff);
}
void db() //open in debug mode
{
	printf("Opening in debug Mode\n");
}
void dmp(char *p1, char *p2) //Dump files
{
	printf("Dumping\n");
}

void err() //Shows Error
{
	printf("Error Try Again.\n");
}
void hlp() //Shows Available commads
{
	printf("load [filename]\nexecute\ndebug\ndump [start] [end]\nassemble [filename]\n");
}
