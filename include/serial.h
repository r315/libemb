#ifndef _serial_h_
#define _serial_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "uart.h"
#include "stdout.h"

typedef enum serialx{
    SERIAL0 = 0,
    SERIAL1,
    SERIAL2,
    SERIAL3,
    SERIAL4
}serial_e;

typedef struct serialhandler {
    stdout_t out;
    serialbus_t port;
}serialhandler_t;

void SERIAL_Config(serialhandler_t *hserial);

#ifdef __cplusplus
}
#endif
#endif /* _serial_h_ */
