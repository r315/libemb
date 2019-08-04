#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#define EXCEPTION_VECTOR_SIZE 7

int main(int argc, char **argv){
FILE *fp;
char *filename;
uint32_t sum = 0;
uint32_t tmp;

	filename = argv[1];

	if(filename == NULL){
		printf("No input file\n");
		return -1;
	}

	fp = fopen(filename, "r+");

	if(fp == NULL){
		printf("Error opening file: %s\n", filename);
		return -1;
	}


	// Read vectors	
	for(int i = 0; i < EXCEPTION_VECTOR_SIZE; i++){
		errno = 0;
		
		if( fread(&tmp, sizeof(uint32_t), 1, fp) != 1){
    		printf("Error reading vectors: %s\n", strerror(errno));	
			goto on_error;
		}
		printf("0x%08X\n", tmp);
		sum += tmp; 
	}

	// two's complement
	sum = (~sum) + 1;
	printf("\nChecksum word 0x%08X\n", sum);

	// write checksum
	if( fwrite(&sum, sizeof(uint32_t), 1, fp) != 1){
		printf("Error writing checksum: %s\n", strerror(errno));	
		goto on_error;
	}

	//verify checksum
	if( fseek(fp, EXCEPTION_VECTOR_SIZE * sizeof(uint32_t),  SEEK_SET) != 0){
		printf("Error seeking to checksum position: %s\n", strerror(errno));	
		goto on_error;
	}

	if( fread(&tmp, sizeof(uint32_t), 1, fp) != 1){
		printf("Error reading checksum: %s\n", strerror(errno));	
		goto on_error;
	}

	if(tmp != sum){
    	printf("Fail to verify checksum. expected: 0x%08X, read: 0x%08X\n", sum, tmp);
	}	
	

on_error:
	if(fp == NULL){
		fclose(fp);
	}

	printf("\n");
return 0;
}
