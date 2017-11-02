#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void split(char *, char *, char *, char *, int *);
int getcmd(char *, int *);
void loadFile(char *,char *);
void exe(char *);
void db();
void dmp(char *, char *);
void assemble(char *);

void err();
void hlp();
int main()
{
	typedef enum {ERROR = -1,LOAD,EXECUTE,DEBUG,DUMP,DIRECTORY,ASSEMBLE,HELP,EXIT} COMMAND;
	char line[80], cmd[10], p1[10], p2[10];
	char buff[255];
	int len, n;
	while(true)
	{
	printf("Hail Master$ ");
	fgets(line,80, stdin);

	len = strlen(line) - 1;
	if(line[len] == '\n')
		line[len] = '\0';

	split(line,cmd,p1,p2,&n);
	
	switch(getcmd(cmd, &n))
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
			assemble(p1);
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



	printf("\n%d things on this line\n", n);
	printf("Command: %s\n", cmd);
	printf("Parameter 1: %s\n", p1);
	printf("parameter 2: %s\n", p2);


	return 0;
}

void split(char *str, char *c, char *p1, char *p2, int *n)
{
	c[0] = p1[0] = p2[0] = '\0';
	int addinx = 0, count = 1;
	bool cmd = false, param1 = false, param2 = false;
	for(int indx = 0; indx < 80; indx++)
	{
		if(str[indx] == '\n' || str[indx] == '\0'){break;}
		
		if(str[indx] == ' ')
		{
			if(c[0] != '\0'){cmd = true;}
			if(p1[0] != '\0'){param1 = true;}
			if(p2[0] != '\0'){param2 = true;}
			addinx = 0;
			continue;
		}

		else if(str[indx] != ' ')
		{
			if(str[indx + 1] == ' ' && str[indx + 2] != ' ')
				count++;
			if(cmd == false)
			{
				c[addinx] = str[indx];
				addinx++;
				c[addinx] = '\0';
				continue;
			}
			if(param1 == false)
			{
				p1[addinx] = str[indx];
				addinx++;
				p1[addinx] = '\0';
				continue;
			}
			if(param2 == false)
			{
				p2[addinx] = str[indx];
				addinx++;
				p2[addinx] = '\0';
				continue;
			}
		}		
	}
	*n = count;
}

int getcmd(char *c, int *n)
{
	if(strcmp(c, "load") == 0){
		if(*n - 1 >= 2 || *n - 1 < 1) return -1;
		return 0;
	}
	else if(strcmp(c, "execute") == 0)return 1;
	else if(strcmp(c, "debug") == 0)return 2;
	else if(strcmp(c, "dump") == 0){
		if(*n - 1 >= 3 || *n - 1 <= 1) return -1;
		return 3;
	}
	else if(strcmp(c, "dir") == 0 || strcmp(c, "directory")==0) return 4;
	else if(strcmp(c, "assemble") == 0){
		if(*n - 1 >= 2 || *n - 1 < 1) return -1;
		return 5;
	}
	else if(strcmp(c, "help") == 0) return 6;
	else if(strcmp(c, "exit") == 0) return 7;
	return -2;
}


void loadFile(char *p1, char *buff)
{
	printf("Load File\n");
	FILE * pFile;
	pFile = fopen(p1,"r");
	fgets(buff, 255,pFile);
	printf("Loaded File\n");
	fclose(pFile);


}
void exe(char *buff)
{
	printf("Executing File\n");
	printf("%s\n",buff);
}
void db()
{
	printf("Opening in debug Mode\n");
}
void dmp(char *p1, char *p2)
{
	printf("Dumping\n");
}
void assemble(char *p1)
{
	printf("Assembling file.\n");
}
void err()
{
	printf("Error Try Again.\n");
}
void hlp()
{
	printf("load [filename]\nexecute\ndebug\ndump [start] [end]\nassemble [filename]\n");
}
