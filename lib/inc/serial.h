#ifndef _serial_h_
#define _serial_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "uart.h"

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

typedef enum serial_e{
    SERIAL0 = 0,
    SERIAL1,
    SERIAL2,
    SERIAL3,
    SERIAL4
}serial_t;

typedef struct serialops_s{
    int(*available)(void);
    int(*read)(char*, int);
    int(*write)(const char*, int);
    int(*readchar)(void);
    int(*writechar)(char);
}serialops_t;

typedef struct serialport_s {
    serialbus_t port;
    serialops_t serial;
}serialport_t;

/**
 * @brief Initializes serial ports available on board
 *
 * This function implementation is board specific
 * and can be called on system startup
 *
 */
void SERIAL_Init(void);

/**
 * @brief Configures serial port with given parameters
 *
 * Used by SERIAL_Init, also can be used to reconfigure
 * serial port by user
 *
 * @param hserial   pointer to serial handler structure
 *                  obtained by SERIAL_GetSerialBus()
 *
 * @param config    New configuration bit mask
 */
void SERIAL_Config(serialport_t *hserial, uint32_t config);

/**
 * @brief Get serial bus handler.
 * The returned pointer points to a serialport_t struct
 * containing HW controller configuration and fifos
 *
 * @param nr            Number of bus starting from 0.
 *                      -1 returns default port operations
 *
 * @return serialops_t*
 */
serialbus_t *SERIAL_GetSerialBus(int32_t nr);

/**
 * @brief Get serial operation for a given bus.
 *
 * @param nr            Number of bus starting from 0.
 *                      -1 returns default port operations
 *
 * @return serialops_t*
 */
serialops_t *SERIAL_GetSerialOps(int32_t nr);


#ifdef __cplusplus
}
#endif
#endif /* _serial_h_ */
