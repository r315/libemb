/**
* @file     qspi.h
* @brief    Contains the spi API header.
*
* @version   1.0
* @date      Fev. 2026
* @author    Hugo Reis
**********************************************************************/

#ifndef _QSPI_H_
#define _QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "dma.h"

#define QSPI_BUS0    0
#define QSPI_BUS1    1

typedef enum {
    QSPI_OK,
    QSPI_ERR,
    QSPI_BUSY,
    QSPI_TIMEOUT,
    QSPI_ERR_PARM
}qspires_t;

typedef struct qspibus{
    void *handle;           // Handle to internal spi structure
    uint8_t  bus;           // bus number 0,1...
    uint32_t freq;          // Clock frequency in khz
    uint32_t size;          // Memory device size in bytes
    uint8_t cfg;            // Configuration
}qspibus_t;

typedef struct qspitrf {
    uint8_t inst;           // Instruction
    uint8_t op;             // 0 write, 1 read
    uint8_t dummy;          // Dummy cycles
    uint8_t *buffer;
    uint32_t addr;          // Address
    uint32_t alternate;
    uint32_t len;           // Number of data bytes
    struct {
        uint8_t absize : 2; // alternate size 0-3 -> 8-32bits
        uint8_t adsize : 2; // address size 0-3 -> 8-32bits
    };
    struct {                // Phase mode, 0 none, 1 single line 2 dual 3 quad
        uint8_t dmode  : 2; // Data
        uint8_t admode : 2; // Address
        uint8_t abmode : 2; // Alternate
        uint8_t imode  : 2; // Instruction
    };
}qspitrf_t;

qspires_t QSPI_Init(qspibus_t *bus);
qspires_t QSPI_Command(qspibus_t *qspibus, qspitrf_t *trf);
qspires_t QSPI_MapMode(qspibus_t *qspibus, qspitrf_t *trf);

#ifdef __cplusplus
}
#endif

#endif /* _QSPI_H_ */
