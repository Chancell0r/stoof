#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void string_reverse1(char *string);
int string_length(char* string);
char *string_reverse2(const char *string);

int main(int argv, char** argc){

	string_reverse1(argc[1]);
	printf("%s \n", argc[1]);
	char* something = string_reverse2(argc[1]);
	printf("%s \n", something);
}

void string_reverse1(char *string)
{
    int length = strlen(string);
    int i = 0;
    char temp;
    char *end = string;
    char *begin = string;

    for(i = 0; i < length - 1; i++){
    	end++;
    }

    for(i = 0; i < (length / 2); i++){
    	temp = *end;
    	*end = *begin;
    	*begin = temp;

    	begin++;
    	end--;
    }
}

char* string_reverse2(const char *string){
	int i;
	int length = strlen(string);
	char temp;
	char* stringCopy = (char*) malloc(sizeof(char*) * (length + 1));
	memcpy(stringCopy, string, length + 1);
	char *end = stringCopy;
    char *begin = stringCopy;


    for(i = 0; i < length - 1; i++){
    	end++;
    }

    for(i = 0; i < (length / 2); i++){
    	temp = *end;
    	*end = *begin;
    	*begin = temp;

    	begin++;
    	end--;
    }

    return stringCopy;
}
