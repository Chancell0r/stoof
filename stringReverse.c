/**
 *@author: Jeff Kohls
 *
 *
 *Two different string reversals for Synapse's application process.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void string_reverse1(char *string);
int string_length(char* string);
char *string_reverse2(const char *string);

int main(int argc, char** argv){


    if(argc < 2){
        printf("please provide valid string to be reversed\n");
        return -1;
    }

    if(argc > 2){
        printf("this program only accepts one parameter\n");
        return -1;
    }

	string_reverse1(argv[1]);
	printf("%s \n", argv[1]);

	char* something = string_reverse2(argv[1]);


    if(something){
    	printf("%s \n", something);
    	free(something);
    }

    return 0;
}

/*
 *Reverse a string by manipulating the passed in char*
 *
 *@param *string - a string pointer that is to be reversed
 */
void string_reverse1(char *string)
{
    int length = strlen(string);
    if(length < 1 || !string){
        return;
    }
    int i = 0;
    char temp;
    char *end = string;
    char *begin = string;

    //point to the last letter of the string
    end += ((length - 1) * sizeof(char));

    for(i = 0; i < (length / 2); i++){
    	temp = *end;
    	*end = *begin;
    	*begin = temp;

    	begin++;
    	end--;
    }
}

/*
 *Reverse a string by copying the constant string coming in to a new character pointer and reversing the strings in
 *  the reversed pointer.
 *
 *@param *string - a string pointer that is to be reversed but not changed directly since it is constant
 */
char* string_reverse2(const char *string){
	int i;
	int length = strlen(string);
	char temp;

    if(length < 1 || !string){
        return NULL;
    }

    //allocate memory to the size of the string plus 1 to account for the trailing '\0'
	char* stringCopy = (char*) malloc(sizeof(char) * (length + 1));

    //copy the memory from one pointer to the new one
	memcpy(stringCopy, string, length + 1);

	char *end = stringCopy;
    char *begin = stringCopy;

    //point to last letter of the string
    end += ((length - 1) * sizeof(char));

    for(i = 0; i < (length / 2); i++){
    	temp = *end;
    	*end = *begin;
    	*begin = temp;

    	begin++;
    	end--;
    }

    return stringCopy;
}
