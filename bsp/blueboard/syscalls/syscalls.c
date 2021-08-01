/*
 * syscalls.c
 *
 *  Created on: 03.12.2009
 *      Author: Martin Thomas, 3BSD license
 */
 /* MOdified by Sagar G V, Feb 2011.
 Ported to LPC17xx.
 */

#include "syscalls.h"


#ifdef __cplusplus
extern "C" {
#endif


#undef errno
extern int errno;

int _kill(int pid, int sig)
{
	pid = pid; sig = sig; /* avoid warnings */
	errno = EINVAL;
	return -1;
}

void _exit(int status)
{
	status = status;
	while(1) {;}
}

int _getpid(void)
{
	return 1;
}


extern char _end; 	/* Defined by the linker at .bss section end*/
static caddr_t heap_end;

char* get_heap_end(void){
	return (char*) heap_end;
}

__attribute__( ( naked ) ) char* get_MSP(void){ /* return MSP	 */
unsigned int result=0;
  asm volatile ("MRS %0, msp\n\t" 
                  "MOV r0, %0 \n\t"
                  "BX  lr     \n\t"  : "=r" (result) );
  return (char*)(result);
}

__attribute__( ( naked ) ) char* get_PSP(void){ /* return PSP	 */
unsigned int result=0;
  asm volatile ("MRS %0, psp\n\t" 
                  "MOV r0, %0 \n\t"
                  "BX  lr     \n\t"  : "=r" (result) );
  return (char*)(result);
}

caddr_t _sbrk(int incr){
caddr_t prev_heap_end;

	if (heap_end == 0) {
		heap_end = (caddr_t)&_end;
	}
	
	prev_heap_end = heap_end;
	
	if (heap_end + incr > get_MSP()) {		
		abort();
	}

	heap_end += incr;
	return prev_heap_end;
}

int _close(int file)
{
	file = file; /* avoid warning */
	return -1;
}

int _fstat(int file, struct stat *st)
{
	file = file; /* avoid warning */
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	file = file; /* avoid warning */
	return 1;
}

int _lseek(int file, int ptr, int dir) {
	file = file; /* avoid warning */
	ptr = ptr; /* avoid warning */
	dir = dir; /* avoid warning */
	return 0;
}

int _read(int file, char *ptr, int len)
{
	file = file; /* avoid warning */
	ptr = ptr; /* avoid warning */
	len = len; /* avoid warning */

	ptr = ptr;
	return 0;

}

int _write(int file, char *ptr, int len)
{
	int todo;
	ptr = ptr;
	file = file; /* avoid warning */
	for (todo = 0; todo < len; todo++) {
		ptr++;
	}
	return len;
}


#ifdef __cplusplus
}
#endif
