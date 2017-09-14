/*
 * ASST2
 * Authors: Malik Ameer, Nidhi Patel
 *
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

void compressR_LOLS(char *filename, int parts){
	
	//check if enough parts exists
	if(parts < 1){
		printf("ERROR: NOT ENOUGH PARTS GIVEN\n");
		return;
	}
	
	/*
	 * Following code checks if file exists, then
	 * finds size of all partitions for each process
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
	
	int part;
	int psize = firstSize;
	int start = 0;
	
	//Create buffers to convert args into char* for exec
	char partbuff[20];
	char startbuff[20];
	sprintf(startbuff, "%d", start);
	
	char psizebuff[20];
	sprintf(psizebuff, "%d", psize);
	
	//PID storage array
	pid_t pids[parts];
	
	int i;
	for (i = 0; i < parts; ++i){
		part = i;
		sprintf(partbuff, "%d", part);
		pids[i] = fork();

		if (pids[i] < 0){
			perror("fork");
			abort();
		}
		else if (pids[i] == 0){
			execl("./compressRworker", "./compressRworker", filename, buff, partbuff, startbuff, psizebuff, NULL);
			exit(0);
		}
		
		if(i == 0){
			start = firstSize;
			sprintf(startbuff, "%d", start);
			psize = partSize;
			sprintf(psizebuff, "%d", psize);
		}
		else{
			start += partSize;
			sprintf(startbuff, "%d", start);
		}
	}
	
	//wait until all PIDs are complete
	int status;
	pid_t pid;
	int a = parts;
	while(a > 0){
		pid = wait(&status);
		--a;
	}
}

int main(int argc, char** argv){
	compressR_LOLS("test.txt", 1);
	return 0;
}