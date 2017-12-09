#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"

//Phase 4
#include "stdbool.h"
#include "sicengine.c"

void loadCommand(char *);
void dumpCommand(char *, char *);
void executeCommand();


void load(FILE *);
void dump(int, int);

int startLoc = -1;


void loadCommand(char *par1){
	int length = strlen(par1);

	if(length == 0){
		printf("Load command needs <filename>.\n");
	}
	else{
		//open object file
		FILE *f1object = fopen(par1, "r");

		//actually runs
		load(f1object);

		printf("File loaded!\n");

		fclose(f1object);
	}
}


void dumpCommand(char *par1, char *par2){
	int length1 = strlen(par1);
	int length2 = strlen(par2);

	if (length1 == 0){
		printf("Dump command needs two parameters");
	}
	else {
		//strings into ints
		sscanf(par1, "%x", &length1);
		sscanf(par2, "%x", &length2);

		//actually runs
		dump(length1, length2); 
	}
}

void executeCommand(){
	
	unsigned long temp = (long unsigned long)startLoc;
	//from sicengine.c
	SICRun(&temp, 0);
}


void load(FILE *fobject){

	char record[100], name[6], stad[6], plength[6];
	char temp[6], temp2[2], temp3[2], end[6];

	int start, totalLength, length, recordAd, recordNum;
	int location = 1;//H
	int bytehex = 0;
	char *byte;

	//start by getting header record
	fgets(record, 100, fobject);

	//copy name to array and increment loc of record
	for(int i = 1; i <= 6; i++)
	{
		name[i-1] = record[i];//exclude H
		location++;
	}

	//in other array get what is next (start address)
	for(int i = 0; i < 6; i++)
	{
		stad[i] = record[location];
		location++;
	}
	//start address into actual number
	sscanf(stad, "%x", &start);

	// following the program length
	for(int i = 0; i < 6; i++)
	{
		plength[i] = record[location];
		location++;
	}
	//total length into actual number
	sscanf(plength, "%x", &totalLength);

	//start from beginning text record,
	//location at 1 to exclude T
	location = 1;
	fgets(record, 100, fobject);
	length = strlen(record)-1;

	//loop through file
	while(record[0] != 'E'){

		//start record address
		for(int i = 1; i<=6; i++){
			temp[i-1] = record[i];
			location++;
		}
		//convert it to hex
		sscanf(temp, "%x", &recordAd);

		//first two after address
		for(int i = 0; i < 2; i++){
			temp2[i] = record[location];
			location++;
		}
		//number of records
		sscanf(temp2, "%d", &recordNum);

		while(location < length){
			//next two following ch
			for(int i = 0; i < 2; i++){
				temp3[i] = record[location];
				location++;
			}

			//store in bytes
			sscanf(temp3, "%02x", &bytehex);
			byte = (char *)&bytehex;
			//load into memory
			PutMem(recordAd, byte, 0);	//sicengine- always 0
			recordAd++;
		}
		//next text record
		fgets(record, 100, fobject);
		length =strlen(record)-1;
		location = 1;
	}
	//end record
	for(int i = 1; i<=6; i++){
		end[i-1]=record[i];
	}
	sscanf(end, "%x", &startLoc);

}
void dump(int startAd, int endAd){
	unsigned char value;
	int counter =0;

	printf("%x: ", startAd);
	for(int i = startAd; i<=endAd; i++){
		if(counter == 0x10){
			counter = 0;
			printf("\n%x: ",i);
		}
		
		GetMem(i, &value, 0);	//sicengine
		printf("%02x  ", value);
		counter++;
	}
	printf("\n\n");
}




int main(){

char a[100], pm1[50], pm2[50], command[50];
SICInit();//initialize simulator

//repeat until exit
while (strcmp(a, "exit") != 0) {

	printf("ENTER A COMMAND \n");
	//command
	fgets(a, 100, stdin);

	sscanf(a,"%s %s %s", command, pm1, pm2);

//compare input from user
	if (strcmp(command, "load") == 0){
		loadCommand(pm1);//loads into memory
		
	}
	else if (strcmp(command, "execute") == 0){
		executeCommand();//calls SIC simulator
		
	}
	else if (strcmp(command, "debug") == 0){
		printf("Command not prepared.\n");
	}
	else if (strcmp(command, "dump") == 0){
		dumpCommand(pm1, pm2);//displays a portion of contents of memory 
		
	}
	else if (strcmp(command, "help") == 0){
		printf("----------Commands----------\n");
		printf("load <filename>: Will Load the file. \n");
		printf("execute: Execute The File Previously Loaded. \n");
		printf("debug: Execute in Debug Mode. \n");
		printf("dump <start> <end>: Dislpay Memory Locations From <start>(hex) and <end>(hex). \n");
		printf("help: Print out Commands Available. \n");
		printf("assemble <filename>: Assemble SIC File. \n");
		printf("directory: List all Files / Folders on directory. \n");
		printf("exit: Exit The simulation. \n");
	}
	else if (strcmp(command, "assemble") == 0){
		//assemCommand(pm1);
		printf("Command not prepared.\n");
	}
	else if (strcmp(command, "directory") == 0){
		system("ls -l");
	}
	else if(strcmp(command, "exit") == 0){
		break;
	}
	else {
		printf("Command not found, type help to display all commands. \n");
		fgets(a,100, stdin);
	}
}
return 0;
}
