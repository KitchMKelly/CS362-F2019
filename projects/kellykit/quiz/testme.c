#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

char inputChar()
{
	//Use two separate char arrays
	//One for alphanumeric chars
	char * alphanumeric = "abcdefghijklmnopqrstuvwxyz";

	//And one for non-alphanumeric chars
	//Use backslash to escape doublequote char
	char * non_alphanumeric = "~`!@#$%^&*()_-+={[}]|\:;\"'<,>.?/ ";

	//Determine which char array from which to return a random char
	int charArrayDecider = rand() % 2; //Returns 1 or 0
	
	if (charArrayDecider == 0) {
		int charToReturn = rand() % 27; //Returns 0 to 26 (length of alphanumeric including '\0' - 1)
		return alphanumeric[charToReturn];
	}
	else if (charArrayDecider == 1) {
		int charToReturn = rand() % 33; //Returns 0 to 32 (length of non_alphanumeric including '\0' - 1)
		return non_alphanumeric[charToReturn];
	}
	else {
		printf("Error: charArrayDecider returned unknown value\n");
		return NULL;
	}
}

void inputString(char * returnString)
{
    //Because testme() checks the first 6 chars of the returned string
	//of this function, returning a string shorter than 6 chars will
	//cause a segment fault.
	//It will be easiest to restrict the string size to 6 chars
	//5 random alphabetical chars + '\0'

	//Because testme() only looks for lowercase alphabetic chars,
	//restrict random chars to only lowercase alphabetic chars

	//Because any string created as a local variable will go out of scope
	//after this function ends, need to pass string in by reference and modify its contents

	//Fill the first 5 array positions with random chars,
	//and fill the final position with '\0'
	memset(returnString, '\0', 6 * sizeof(char));
	char alphabet[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
						  'h', 'i', 'j', 'k', 'l', 'm', 'n',
						  'o', 'p', 'q', 'r', 's', 't', 'u',
						  'v', 'w', 'x', 'y', 'z' };
	int i;

	
	/*************************************************************
	* Code for 25% chance of setting returnString to "reset\0" *
	*************************************************************/
	//1 in 4 chance that reset is 3
	int reset = rand() % 4; //Returns 0, 1, 2, or 3

	//If reset isn't 3, return random chars
	if (reset != 3) {
		for (i = 0; i < 5; i++) {
			int randPosition = rand() % 26; //Returns 0 to 25 (length of alphabet - 1)
			returnString[i] = alphabet[randPosition];
		}
	}

	//If reset is 3, return "reset"
	else {
		returnString[0] = 'r';
		returnString[1] = 'e';
		returnString[2] = 's';
		returnString[3] = 'e';
		returnString[4] = 't';
	}

	/********************************************************************************
	* Code for completely random returnString (5 lowercase alphabetic chars + '\0') *
	********************************************************************************/
	/*
	for (i = 0; i < 5; i++) {
		int randPosition = rand() % 26; //Returns 0 to 25 (length of alphabet - 1)
		returnString[i] = alphabet[randPosition];
	}
	*/
}

void testme()
{
	int tcCount = 0;
	char *s = malloc(sizeof(char) * 6);
	 char c;
	int state = 0;
	while (1)
	{
		tcCount++;
		c = inputChar();
		inputString(s);
		printf("Iteration %d: c = %c, s = %s, state = %d\n", tcCount, c, s, state);

		if (c == '[' && state == 0) state = 1;
		if (c == '(' && state == 1) state = 2;
		if (c == '{' && state == 2) state = 3;
		if (c == ' '&& state == 3) state = 4;
		if (c == 'a' && state == 4) state = 5;
		if (c == 'x' && state == 5) state = 6;
		if (c == '}' && state == 6) state = 7;
		if (c == ')' && state == 7) state = 8;
		if (c == ']' && state == 8) state = 9;
		if (s[0] == 'r' && s[1] == 'e'
		   && s[2] == 's' && s[3] == 'e'
		   && s[4] == 't' && s[5] == '\0'
		   && state == 9)
		{
			free(s);
			printf("error ");
			exit(200);
		}
	}
}


int main(int argc, char *argv[])
{
    srand(time(NULL));
    testme();
    return 0;
}