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
int err = -1;

	filename = argv[1];

	if(filename == NULL){
		printf("No input file\n");
		return err;
	}

	fp = fopen(filename, "r+b"); // Read/Update

	if(fp == NULL){
		printf("Error opening file: %s\n", filename);
		return err;
	}


	// Read vectors	
	for(int i = 0; i < EXCEPTION_VECTOR_SIZE; i++){
		if( fread(&tmp, sizeof(uint32_t), 1, fp) != 1){
    		printf("Error reading vector: %d\n", i);	
			goto on_error;
		}
		sum += tmp; 
	}

	// two's complement
	sum = (~sum) + 1;
	printf("\nChecksum word 0x%08X\n", sum);

	fseek(fp, EXCEPTION_VECTOR_SIZE * sizeof(uint32_t),  SEEK_SET);

	// write checksum at offset 0x1C (vector 7)
	if( fwrite(&sum, sizeof(sum), 1, fp) != 1){
		printf("Error writing checksum: %s\n", strerror(errno));	
		goto on_error;
	}	

	fflush (fp);

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
		goto on_error;
	}	
	
	err = 0;

on_error:
	if(fp != NULL){
		fclose(fp);
	}

	printf("\n");
  return err;
}
