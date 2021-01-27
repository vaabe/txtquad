#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 5000

struct TextBlock {
	char *text; 
	int type; 
}; 

struct Doc {
	char *filePath; 
	int numChars; 
	int numTextBlocks; 
	struct TextBlock *block; 
}; 

struct TextBlock *new_block(int numTextBlocks, int charCount[], int type)
{
	struct TextBlock *block = malloc(numTextBlocks * sizeof(struct TextBlock)); 

	for (int i = 0; i < numTextBlocks; i++)
	{
		block[i].text = malloc((charCount[i] + 5) * sizeof(char)); 
		block[i].type = type; 
	}

	return block; 
}

void read_doc(struct Doc *doc, int charCount[])
{
	FILE *docFile = fopen(doc->filePath, "r"); 

	if (!docFile) {
		perror("File error"); 
		exit(0); 
	}

	int ty = 2; 

	doc->block = new_block(doc->numTextBlocks, charCount, ty); 

	char buffer[doc->numTextBlocks][MAX_BUFFER]; 

	for (int i = 0; i < doc->numTextBlocks; i++) {
		fgets(buffer[i], sizeof(buffer[i]), docFile); 
		doc->block[i].text = buffer[i]; 

		if (buffer[i][0] == '#') {
			if (buffer[i][1] == '#') {
				doc->block[i].type = 2;
				doc->block[i].text += 3; 
			}
			else {
				doc->block[i].type = 3; 
				doc->block[i].text += 2; 
			}	
		}
		else if (buffer[i][0] == ' ') {
			doc->block[i].type = 0; 
		}
		else {
			doc->block[i].type = 1; 
		}
	}
}

int get_num_blocks(struct Doc doc)
{
	FILE *docFile = fopen(doc.filePath, "r"); 

	if (!docFile) {
		perror("File error"); 
		exit(0); 
	}

	char buffer[MAX_BUFFER]; 
	int blockNum = 0; 

	while (fgets(buffer, sizeof(buffer), docFile) != NULL) {
		blockNum++; 
	}

	fclose(docFile); 
	return blockNum; 
}

int get_num_chars(struct Doc doc)
{
	FILE *docFile = fopen(doc.filePath, "r"); 

	if (!docFile) {
		perror("File error"); 
		exit(0); 
	}

	int n = 0; 

	while(fgetc(docFile) != EOF) {
		n++; 
	}

	return n; 
}

int *get_block_char_count(struct Doc doc)
{
	FILE *docFile = fopen(doc.filePath, "r"); 

	if (!docFile) {
		perror("File error"); 
		exit(0); 
	}

	char buffer[MAX_BUFFER]; 
	int blockNum = 0; 

	int *charCount = calloc(doc.numTextBlocks, sizeof(int)); 
	int n = 0; 

	while (fgets(buffer, sizeof(buffer), docFile) != NULL) {

		int m = 0; 

		while ((int)buffer[m] != 0) {
			m++; 
		}

		charCount[blockNum] = m; 
		
		blockNum++; 
	}

	fclose(docFile); 
	return charCount; 
}

#if 0
void skim_doc(struct Doc doc)
{
	FILE *docFile = fopen(doc.filePath, "r"); 

	if (!docFile)
	{
		perror("File error"); 
		exit(0); 
	}

	int numChars = 0; 
	int numLines = 0;

	char buffer[MAX_BUFFER]; 

	for (int i = 0; i < 3; i++) 
	{
		fgets(buffer, sizeof(buffer), docFile); 
		// printf("%s", buffer); 
	}

	// printf("%c\n", buffer[200]); 

//	while(fgets(buffer, sizeof(buffer), docFile) != NULL) {
//		printf("%s\n", buffer); 
//		numLines++; 
//	}

//	while(fgetc(docFile) != EOF) {
//		numChars++; 
//	}

	// printf("%d\n%d\n", numLines, numChars); 
}
#endif

#endif
