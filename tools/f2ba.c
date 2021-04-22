#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

/**
 * File to byte Array
 * 
 * */

#define NCOLS 16

typedef struct{
    int offset;
    char *infile;
    char *outfile;
    int data_size;
    char endian;
}Arguments;

const char *data_types[] = {"char", "uint8_t", "uint16_t", "uint32_t", "uint64_t"};

void help(){
    printf("File to byte array ");
    printf("Usage: f2ba <if=input file> <of=output file> [ofs=<offset>] [ds=<bytes>] [endian=<little|big>]\n");
}

#ifndef _WIN32

char *strupr(char *str){
    char *ptr = str;
    while(*str){
        if(!(*str < 'a') && !(*str > 'z')){
            *str = toupper(*str);
        }
        str++;
    }
    return ptr;
}

#endif


int strstW(const char *a, const char *b){
   return (strncmp(a, b, strlen(b)) == 0) ? 1 : 0;
}


void savetofile(char *filename, uint8_t *buf, int len, int data_size, int endian){
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
    fprintf(fp,"#define %s_SIZE    %d\n", strupr(namecpy), len/data_size);
    fprintf(fp,"const %s %s[] = {", data_types[data_size], filename);

    for(int i = 0; i < len; ){
        if((i % NCOLS) == 0){
            fprintf(fp,"\n");
        }
        
        fprintf(fp,"0x");

        if(endian == 0){                             // default big        
            for(int ds = 0; ds < data_size; ds++){   // data_size bytes
                fprintf(fp,"%02X", buf[i + ds]); 
            }
        }else{            
            for(int ds = data_size - 1; ds >= 0; ds--){   // data size bytes
                fprintf(fp,"%02X", buf[ i + ds]); 
            }
        }
        
        i += data_size;                        

        if(i < len ){
            fprintf(fp,",");
        }
    }

    fprintf(fp,"\n};");
    fclose(fp);
}

static void parseCmdl(int argc, char** argv, Arguments* arg){

    arg->offset = 0;
    arg->infile = NULL;
    arg->outfile = NULL;
    arg->data_size = 1;
    arg->endian = 0;

     /* parse options in any order */
    for (int pos = 1; pos < argc; pos++)
    {
        if (strstW(argv[pos], "if=") && arg->infile == NULL){            
            arg->infile = argv[pos] + 3;            
        }

        if (strstW(argv[pos], "of=") && arg->outfile == NULL){
            arg->outfile = argv[pos] + 3;
        }

        if (strstW(argv[pos], "ofs=")){
            arg->offset = atoi(argv[pos] + 4);
        }

        if (strstW(argv[pos], "ds=")){
            arg->data_size = atoi(argv[pos] + 3);
        }
        
        if (strstW(argv[pos], "endian=")){
          if(!strcmp(argv[pos] + 7,"little")){
            arg->endian = 1;
          }
        }
    }

}

int main(int argc, char **argv){
FILE *fp;
char *filename;
int size;
char *buf;
Arguments args;

    if(argc < 3){
        help();
        exit(0);
    }

    parseCmdl(argc, argv, &args);
    
    fp = fopen(args.infile, "rb");

    if(fp == NULL){
        fprintf(stderr,"Unable to open file '%s'\n", filename);
        help();
        exit(-1);
    }

    fseek(fp,0,SEEK_END);
    size = ftell(fp) - args.offset;
    fseek(fp, args.offset, SEEK_SET);

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

    savetofile(args.outfile, (uint8_t*)buf, size, args.data_size, args.endian);

    free(buf);

    return 0;
}