#include "blueboard.h"
#include "serial.h"

#define BOARD_SERIAL_HANDLERS           BOARD_SERIAL0_HANDLER
#define BOARD_SERIAL0                   (&BOARD_SERIAL0_HANDLER.out)

#define DBG_PIN                         P2_11

#define UART_FUNCTION_NAME(a, b)        UART##a##_##b
#define HANDLER_NAME(a)                 BOARD_SERIAL##a##_HANDLER

#define UART_FUNCTIONS(N) \
static inline void UART_FUNCTION_NAME(N, Init)(void){ UART_Init(&HANDLER_NAME(N).port); } \
static inline void UART_FUNCTION_NAME(N, PutChar)(char c){ UART_PutChar(&HANDLER_NAME(N).port, c); } \
static inline void UART_FUNCTION_NAME(N, Puts)(const char* str){ UART_Puts(&HANDLER_NAME(N).port, str); } \
static inline char UART_FUNCTION_NAME(N, GetChar)(void){ return UART_GetChar(&HANDLER_NAME(N).port); } \
static inline uint8_t UART_FUNCTION_NAME(N, GetCharNonBlocking)(char *c){ return UART_GetCharNonBlocking(&HANDLER_NAME(N).port, c); } \
static inline uint8_t UART_FUNCTION_NAME(N, Kbhit)(void){ return UART_Kbhit(&HANDLER_NAME(N).port); }


#define ASSIGN_UART_FUNCTIONS(I, N) \
I->out.init = UART_FUNCTION_NAME(N, Init); \
I->out.xputchar = UART_FUNCTION_NAME(N, PutChar); \
I->out.xputs = UART_FUNCTION_NAME(N, Puts); \
I->out.xgetchar = UART_FUNCTION_NAME(N, GetChar); \
I->out.getCharNonBlocking = UART_FUNCTION_NAME(N, GetCharNonBlocking); \
I->out.kbhit = UART_FUNCTION_NAME(N, Kbhit)


serialhandler_t BOARD_SERIAL_HANDLERS;

UART_FUNCTIONS(0)

__attribute__ ((weak)) void app_main(void *ptr){
    stdout_t *serial = (stdout_t*)ptr;

    serial->xputs("\e[2J\rHello");
    while(1){
        char echo;
        
        if(serial->getCharNonBlocking(&echo)){
            serial->xputchar(echo);
        }
    }
}


void SERIAL_Config(serialhandler_t *hserial, uint32_t config){

    if(hserial == NULL){
        return ;
    }

    switch(SERIAL_CONFIG_GET_NUM(config)){
        case SERIAL0:
            ASSIGN_UART_FUNCTIONS(hserial, 0);
            hserial->port.bus = UART_BUS0;
            break;

        case SERIAL1:
        case SERIAL2:
        case SERIAL3:
        case SERIAL4:           

        default:
            return;
    }

    serialbus_t *port = &hserial->port;
    
    port->speed = SERIAL_CONFIG_GET_SPEED(config);
    port->parity = SERIAL_CONFIG_GET_PARITY(config);
    port->stopbit = SERIAL_CONFIG_GET_STOP(config);
    port->datalength = SERIAL_CONFIG_GET_DATA(config);
    
    hserial->out.init();
}

int main()
{
    CLOCK_Init(100);
	//CLOCK_InitUSBCLK();
    BB_Init();

    SERIAL_Config(&BOARD_SERIAL0_HANDLER, SERIAL0 | SERIAL_DATA_8B | SERIAL_PARITY_NONE | SERIAL_STOP_1B | SERIAL_SPEED_115200);    

    //GPIO_Config(DBG_PIN, PIN_OUT_PP);

    app_main(&BOARD_SERIAL0_HANDLER.out);

	return 0;
}
