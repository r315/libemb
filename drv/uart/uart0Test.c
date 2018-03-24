#include <clock.h>
#include "uart.h"
#include "stdout.h"

#define DBG	xprintf
#define UART0_PrintString xprintf
#define UART0_Getchar uartGetChar
#define UART0_Sendchar uartSendChar
#define EOF -1


int main(void){
char c;
//short DLest;
//struct FdrPair fr;
	
	UART_Init(0,19200,SystemCoreClock/4);
	stdoutInit((Stdout*)UART_GetReference());
	
	DBG("\n\nUART Library test\n");
	DBG("==========================\n\n");
	DBG("Press a key within your terminal\n");
	DBG("And the board will echo it back\n");
	
	#if 0
	DLest = _frdivLookup(&fr,9600,18000000);
	
	DBG("FRest: %u\n",fr.fr);
	DBG("DLest: %u\n",DLest);
	DBG("FDR: %X\n",fr.FdrVal);
	#endif
	
	while(1) {
		c = UART_GetChar();	// wait for next character from terminal
		if (c != EOF) {
			// show on debugger console
			if ((c == 9) || (c == 10) || (c == 13) || ((c >= 32) && (c <= 126))) {
				DBG("%c", c);
				
				// Some terminal programs can be configured to send both
				// newline and carriage return when 'enter' key is pressed.
				// However not all can. As semihosting requires a newline
				// in order to flush its buffer, we add one just in case
				// the terminal doesn't send one.
				if (c==13) { DBG("\n"); } 
			}else {
				DBG(".");	// non-printable character
			}
		}
	}
	return 0;
}
