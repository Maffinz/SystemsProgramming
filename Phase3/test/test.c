#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

void printToFile(FILE *pass)
{
	fprintf(pass, "Hello");
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


int main()
{
	int add = 3;

	char *cat = (char *) malloc(6);
	memset(cat, '0', 5);

	char *this = (char *) malloc(7);
	itoa(add, this, 10);

	strcat(cat, this);

	printf("cat: %s", cat);


	return 0;

}
