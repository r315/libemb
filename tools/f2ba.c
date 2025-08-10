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
    char *fmt;
}Arguments;

const char *data_types[] = {"char", "uint8_t", "uint16_t", "uint32_t", "uint64_t"};

void help(){
    printf("File to byte array ");
    printf("Usage: f2ba <input file> <output file> [ofs=<offset>] [ds=<bytes>] [endian=<little|big>] [fmt=<c|txt>]\n");
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


void savetofile(Arguments *args, uint8_t *buf, int len){
    FILE *fp;

    fp = fopen(args->outfile, "wb");

    if(fp == NULL){
        fprintf(stderr,"Unable to open output file '%s'\n", args->outfile);
        exit(-1);
    }

    if(!strcmp(args->fmt, "c")){
        strchr(args->outfile, '.')[0] = '\0'; // trim to file name

        //fprintf(fp,"#if 0\n%.*s\n#endif\n", len, buf); // print file content if printable chars??

        fprintf(fp,"#define %s_SIZE    %d\n", strupr(args->outfile), len/args->data_size);
        fprintf(fp,"const %s %s[] = {", data_types[args->data_size], args->outfile);

        for(int i = 0; i < len; ){
            if((i % NCOLS) == 0){
                fprintf(fp,"\n");
            }

            fprintf(fp,"0x");

            if(args->endian == 0){                             // default big
                for(int ds = 0; ds < args->data_size; ds++){   // args->data_size bytes
                    fprintf(fp,"%02X", buf[i + ds]);
                }
            }else{
                for(int ds = args->data_size - 1; ds >= 0; ds--){   // data size bytes
                    fprintf(fp,"%02X", buf[ i + ds]);
                }
            }

            i += args->data_size;

            if(i < len ){
                fprintf(fp,",");
            }
        }

        fprintf(fp,"\n};");
    }else{
        for(int i = 0; i < len; ){
            if(args->endian == 0){                             // default big
                for(int ds = 0; ds < args->data_size; ds++){   // args->data_size bytes
                    fprintf(fp,"%02X", buf[i + ds]);
                }
            }else{
                for(int ds = args->data_size - 1; ds >= 0; ds--){   // data size bytes
                    fprintf(fp,"%02X", buf[ i + ds]);
                }
            }

            i += args->data_size;

            if(i < len ){
                fprintf(fp,"\n");
            }
        }
    }

    fclose(fp);
}

static void parseCmdl(int argc, char** argv, Arguments* arg){

    arg->offset = 0;
    arg->infile = argv[1];
    arg->outfile = argv[2];
    arg->data_size = 1;
    arg->endian = 0;
    arg->fmt = "c";

     /* parse options in any order */
    for (int pos = 1; pos < argc; pos++)
    {
        /*if (strstW(argv[pos], "if=") && arg->infile == NULL){
            arg->infile = argv[pos] + 3;
        }

        if (strstW(argv[pos], "of=") && arg->outfile == NULL){
            arg->outfile = argv[pos] + 3;
        }
        */
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

        if (strstW(argv[pos], "fmt=")){
            arg->fmt = argv[pos] + 4;
        }
    }

}

int main(int argc, char **argv){
FILE *fp;
char *filename;
int len;
char *buf;
Arguments args;

    if(argc < 3){
        help();
        exit(0);
    }

    parseCmdl(argc, argv, &args);

    fp = fopen(args.infile, "rb");

    if(fp == NULL){
        fprintf(stderr,"Unable to open input file '%s'\n", args.infile);
        help();
        exit(-1);
    }

    fseek(fp,0,SEEK_END);
    len = ftell(fp) - args.offset;
    fseek(fp, args.offset, SEEK_SET);

    buf = (char*)malloc(len);

    if(buf == NULL){
        fprintf(stderr,"Unable to allocate %u bytes of memory\n", len);
        fclose(fp);
        exit(-2);
    }

    int res = fread(buf, 1, len, fp);

    if(res != len){
        fprintf(stderr,"Error reading file, expected size %d, read %d\n", len, res);
        free(buf);
        fclose(fp);
        exit(-3);
    }

    fclose(fp);

    savetofile(&args, (uint8_t*)buf, len);

    free(buf);

    return 0;
}
