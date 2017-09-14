/*
 * ASST2
 * Authors: Malik Ameer, Nidhi Patel
 *
*/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>


/*
 * Compresses given string pointed to by input into RLE/LOLS 
 * forms and returns pointer to new string
*/



void* LOLS(char* filename, char* input, int part, int start, int size){
	
	
	if(input == NULL){
		printf("ERROR: EMPTY INPUT FILE\n");
		return NULL;
	}
	
	//Creating new files that will contain compressed parts 
	char newname[strlen(filename) + 9];
	memcpy(newname, filename, strlen(filename));
	int i;
	for(i = 0; i < strlen(filename); i++){
		if(newname[i] == '.'){
			newname[i] = '_';
		}
	}
	
	//Creating renamed files that will contain thread output
	FILE* newFile; 
	
	if(part == 0){
		newname[i] = '_';
		newname[i+1] = 'L';
		newname[i+2] = 'O';
		newname[i+3] = 'L';
		newname[i+4] = 'S';
		newname[i+5] = '0';
		newname[i+6] = '\0';
		newFile = fopen(newname, "a+");
	}
	else{
		//numbuff is used to convert int to char for files names
		char numbuff[4];	
		sprintf(numbuff, "%d", part);
		newname[i] = '_';
		newname[i+1] = 'L';
		newname[i+2] = 'O';
		newname[i+3] = 'L';
		newname[i+4] = 'S';
		newname[i+5] = numbuff[0];
		newname[i+6] = numbuff[1];
		newname[i+7] = numbuff[2];
		newname[i+8] = '\0';
			
		newFile = fopen(newname, "a+");
	}

	int letterCount = 0;
	char currLetter = input[start];
	int pos = start;
	
	//LOLS compression 
	for(; pos <= (start+size); pos++){
		
		if(!isalpha(currLetter)){
			currLetter = input[pos];
			letterCount = 1;
		}
		else if(currLetter == input[pos] && pos != (start+size)){
			letterCount++;
		}
		else if(pos == (start+size)){
			if(letterCount == 1){
				fprintf(newFile, "%c", currLetter);
			}
			else if(letterCount == 2){
				fprintf(newFile, "%c%c", currLetter, currLetter);
			}
			else{
				if(letterCount == 0)
					continue;
				fprintf(newFile, "%d%c", letterCount, currLetter);
			}
		}
		else{
			if(letterCount == 1){
				fprintf(newFile, "%c", currLetter);
			}
			else if(letterCount == 2){
				fprintf(newFile, "%c%c", currLetter, currLetter);
			}
			else{
				if(letterCount == 0)
					continue;
				fprintf(newFile, "%d%c", letterCount, currLetter);
			}
			currLetter = input[pos];
			letterCount = 1;
		}
	}
	
	fclose(newFile);
	return NULL;
}


int main(int argc, char** argv){
	
	
	char* filename = argv[1];
	char* input = argv[2];
	int part = atoi(argv[3]);
	int start = atoi(argv[4]);
	int size = atoi(argv[5]);
	LOLS(filename, input, part, start, size);
	
	return 0;
}