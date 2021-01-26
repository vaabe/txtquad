#ifndef FAFFING_H
#define FAFFING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ESSAY_LEN 500

char essayFilePath[50] = "./examples/essay.txt"; 

int get_essay_len() 
{
	FILE *essayFile; 
	essayFile = fopen(essayFilePath, "r"); 
	int n = 0; 

	while(fgetc(essayFile) != EOF) {
		n++; 
	}

	return n; 
}

char *get_essay_chars(int essayLen) 
{
	FILE *essayFile; 
	essayFile = fopen(essayFilePath, "r"); 
	char *essay = malloc(essayLen * sizeof(char)); 
	char c; 
	// int n = 0; 

	for (int n = 0; n < essayLen; n++) {
		c = fgetc(essayFile); 
		if (c == EOF) {
			break; 
		}
		essay[n] = c; 
	}

	fclose(essayFile); 

	return essay; 
}

#endif
