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

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

//Struct for thread arguments
struct t_args{
	char* filename;
	char* input;
	int part;
	int start;
	int size;
};


void* LOLS(void* thread_args){
	
	struct t_args *args = thread_args;

	char* filename = args->filename;
	char* input = args->input;
	int part = args->part;
	int size = args->size;
	int start = args->start;
	
	//Sends parents thread signal to end wait
	pthread_mutex_lock(&m);
	pthread_cond_signal(&c);
	pthread_mutex_unlock(&m);
	
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

void compressT_LOLS(char *filename, int parts){
	
	//check if enough parts exists
	if(parts < 1){
		printf("ERROR: NOT ENOUGH PARTS GIVEN\n");
		return;
	}
	
	/*
	 * Following code checks if file exists, then
	 * finds size of all partitions for each thread
	*/
	if(filename == NULL || access(filename, R_OK) == -1){
		printf("ERROR: FILE DOES NOT EXIST\n");
		return;
	}
	FILE *fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET); 
	
	if(size < parts){
		printf("ERROR: SIZE OF FILE LESS THAN PARTS GIVEN\n");
		return;
	}
	
	//create buffer to copy string into
	char buff[size+1];
	fgets(buff, size+1, (FILE*)fp);
	buff[size] = '\0';
	fclose(fp);
	
	//get partition sizes
	int partSize = size/parts;
	int firstSize = partSize + (size%parts);
	//printf("%d   %d\n", partSize, firstSize);
	
	//creates argument struct for thread creation
	struct t_args *targs = (struct t_args*)malloc(sizeof(struct t_args));
	targs->filename = filename;
	targs->input = buff;
	targs->size = firstSize;
	targs->start = 0;
	
	//tid array to keep track of threads
	pthread_t tid[parts];
	int i;
	for(i = 0; i < parts; i++){
		targs->part = i;
		pthread_create(&tid[i], NULL, *LOLS, targs);
		
		/*
		 * Waits for signal from child thread. 
		 * Wait is needed because args struct is changed
		 * by for-loop before thread can access it
		*/
		pthread_mutex_lock (&m);
		pthread_cond_wait (&c, &m);
		pthread_mutex_unlock (&m);
		
		//Adjusts partition sizes
		if(i == 0){
			targs->start = firstSize;
			targs->size = partSize;
		}
		else{
			targs->start += partSize;
		}
	}
	
	//Join all threads before program ends
	for(i = 0; i < parts; i++){
		pthread_join(tid[i], NULL);
	}
}

int main(int argc, char** argv){
	
	compressT_LOLS("test.txt", 1);
	
	return 0;
}