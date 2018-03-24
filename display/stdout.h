#ifndef _stdout_h_
#define _stdout_h_

typedef struct{
	void (*xputc)(char);
	char (*xgetchar)(void);
}Stdout;

void stdoutInit(Stdout *outdevice);
void xputc(char c);
void xprintf(const char* str, ...);
char xgetchar(void);
void xputs (const char* str);

#endif /* _stdout_h_ */