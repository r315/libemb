#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * File to byte Array
 * 
 * */

#define NCOLS 32

typedef struct{
    int offset;
    char *infile;
    char *outfile;
}Arguments;

void help(){
    printf("File to byte array ");
    printf("Usage: f2ba <-if input file> <-of output file> [-ofs <offset>]\n");
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

static void parseCmdl(int ac, char** av, Arguments* arg){

    arg->offset = 0;
    arg->infile = NULL;
    arg->outfile = NULL;

     /* parse options in any order */
    for (int pos = 1; pos < ac && av[pos][0] == '-'; pos++)
    {
        if (!strcmp(av[pos], "-if"))
        {
             arg->infile = av[++pos];
        }

        if (!strcmp(av[pos], "-of"))
        {
            arg->outfile = av[++pos];
        }

        if (!strcmp(av[pos], "-ofs"))
        {
            arg->offset = atoi(av[++pos]);
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

    savetofile(args.outfile, (uint8_t*)buf, size);

    free(buf);

    return 0;
}