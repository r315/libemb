#include <stdint.h>
#include "serial.h"

extern serialhandler_t BOARD_SERIAL0_HANDLER;

void UART0_IRQHandler(void){
	UART_IRQHandler(&BOARD_SERIAL0_HANDLER.port);
}
