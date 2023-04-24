#ifndef _serial_h_
#define _serial_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "uart.h"
#include "stdout.h"

#define SERIAL_DATA_8B      (8 << 24)
#define SERIAL_PARITY_NONE  (0 << 28)
#define SERIAL_PARITY_EVEN  (1 << 28)
#define SERIAL_PARITY_ODD   (2 << 28)
#define SERIAL_STOP_1B      (1 << 30)
#define SERIAL_STOP_2B      (2 << 30)
#define SERIAL_SPEED_9600   (9600 << 4)
#define SERIAL_SPEED_115200 (115200 << 4)
#define SERIAL_SPEED_230400 (230400 << 4)

#define SERIAL_CONFIG_GET_SPEED(cfg)    ((cfg >> 4) & 0xFFFFF)     // 20bit
#define SERIAL_CONFIG_GET_NUM(cfg)      ((cfg >> 0) & 15)  // 4bit
#define SERIAL_CONFIG_GET_DATA(cfg)     ((cfg >> 24) & 15)  // 4 bit
#define SERIAL_CONFIG_GET_PARITY(cfg)   ((cfg >> 28) & 3)   // 2 bit
#define SERIAL_CONFIG_GET_STOP(cfg)     ((cfg >> 30) & 3)   // 2 bit

typedef enum serialx{
    SERIAL0 = 0,
    SERIAL1,
    SERIAL2,
    SERIAL3,
    SERIAL4
}serial_e;

// Arduino style API
typedef struct serial_s{
    int(*available)(void);
    int(*read)(void);
    int(*readBytes)(uint8_t*, int);
    int(*write)(uint8_t*, int);
}serial_t;

typedef struct serialhandler {
    serialbus_t port;
    stdout_t out;
    serial_t serial;    
}serialhandler_t;

void SERIAL_Config(serialhandler_t *hserial, uint32_t config);
void SERIAL_Init(void);
stdout_t *SERIAL_GetStdout(int32_t nr);
serial_t *SERIAL_GetSerial(int32_t nr);
serialbus_t *SERIAL_GetSerialBus(int32_t nr);

#ifdef __cplusplus
}
#endif
#endif /* _serial_h_ */
