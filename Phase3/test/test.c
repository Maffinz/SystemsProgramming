#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int main()
{
	char *str = (char*) malloc(6);
	int index = 1;
	strcpy(str, "X'F1'");
	char *hex = (char*) malloc(6);
	int hexinx = 0;

	while(str[index++] != '\0')
	{
		if(str[index] == '\'') continue;
		hex[hexinx] = str[index];
	       hexinx++;	
	}
	int byte = (int)strtol(hex, NULL, 16);
	printf(" Decimal: %d\n HexaDec: %x\n String: %c\n", byte,byte,byte);
	return 0;
}
