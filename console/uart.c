
#include <serialport.h>


void init(void){
}   
char getchar(void){
}
void putchar(char c){
}
void puts(const char* str){
}  
uint8_t getCharNonBlocking(char *c){
}
uint8_t kbhit(void){
}


SerialPort uart = {
    .init = init,
    .getchar = getchar,
    .putchar = putchar,
    .puts = puts,
    .getCharNonBlocking = getCharNonBlocking,
    .kbhit = kbhit
};
