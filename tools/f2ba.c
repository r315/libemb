#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * File to byte Array
 * 
 * */

#define NCOLS 32

void help(){
    printf("File to byte array ");
    printf("Usage: f2ba <input file> <output file>\n");
}

void savetofile(char *filename, uint8_t *buf, int len){
FILE *fp;
char namecpy[20];

    fp = fopen(filename, "wb");

    if(fp == NULL){
        fprintf(stderr,"Unable to open output file '%s'\n", filename);
        exit(-1);
    }

    char *name = strchr(filename, '.');
    *name = '\0';

    //fprintf(fp,"#if 0\n%.*s\n#endif\n", len, buf); // print file content if printable chars??
    strcpy(namecpy, filename);
    fprintf(fp,"#define %s_SIZE    %d\n", strupr(namecpy), len);
    fprintf(fp,"const char %s[] = {", filename);

    for(int i = 0; i < len; i++){
        if((i % NCOLS) == 0){
            fprintf(fp,"\n");
        }
        
        fprintf(fp,"0x%02X", buf[i]);

        if(i < len - 1){
            fprintf(fp,",");
        }
    }

    fprintf(fp,"\n};");
    fclose(fp);
}


int main(int argc, char **argv){
FILE *fp;
char *filename;
int size;
char *buf;

    if(argc < 3){
        help();
        exit(0);
    }

    filename = argv[1];
    
    fp = fopen(filename, "rb");

    if(fp == NULL){
        fprintf(stderr,"Unable to open file '%s'\n", filename);
        exit(-1);
    }

    fseek(fp,0,SEEK_END);
    size = ftell(fp);
    fseek(fp,0,SEEK_SET);

    buf = (char*)malloc(size);

    if(buf == NULL){
        fprintf(stderr,"Unable to allocate %u bytes of memory\n", size);
        fclose(fp);
        exit(-2);
    }

    int res = fread(buf, 1, size, fp);

    if(res != size){
        fprintf(stderr,"Error reading file, expected size %d, read %d\n", size, res); 
        free(buf);
        fclose(fp);
        exit(-3);
    }

    fclose(fp);

    savetofile(argv[2], (uint8_t*)buf, size);

    free(buf);

    return 0;
}