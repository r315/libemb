#include <stdio.h>
#include <string.h>
#include <stdint.h>

char *getOptValue(char *opt, uint32_t argc, char **argv){
    for(uint32_t i = 0; i < argc; i++){
        if(strcmp(opt, argv[i]) == 0){
            return argv[i+1];
        }
    }
    return NULL;
}

char *skipSpaces(char *str){
	while((*str == ' ' || *str == '\t') && *str != '\0') 
		str++;
	return str;
}

uint32_t strToArray(char *str, char **argv){
uint32_t argc = 0;

    if(str == NULL){
        return 0;
    }

	str = skipSpaces(str);

	if(*str == '\0')
		return 0;

	argv[argc++] = str;

    while(*str != '\0'){
        if(*str == ' '){
			*str = '\0';
			str = skipSpaces(str + 1);
			if(*str != '\0')
				argv[argc++] = str;
        }else{
			str++;
		}
    }

	return argc;
}

int main(int argc, char **argv){

char testStr[] = "    \t-w param1 -r             \t\tparam2     ";
char *array[10];

printf("\"%s\" ", testStr);
uint32_t count = strToArray(testStr, array);

printf("Was split in to %u words:\n",count);

for (int i = 0; i < count; ++i) 
	printf("%s\n", array[i]);

printf("value for %s = %s\n", array[0], getOptValue(array[0], count, array));

return 0;
}
