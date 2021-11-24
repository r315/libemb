#ifndef _board_h_
#define _board_h_

/**
 * Dummy board header for testing
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Symbols for NVDATA */
#define FLASH_SECTOR_SIZE        128
extern uint8_t flash_sector[];
#define NVDATA_SIZE             31
#define NVDATA_SECTOR_INIT      
#define NVDATA_SECTOR_START     flash_sector
#define NVDATA_SECTOR_END       (NVDATA_SECTOR_START + FLASH_SECTOR_SIZE)
#define NVDATA_SECTOR_READ      memcpy
#define NVDATA_SECTOR_WRITE     memcpy
#define NVDATA_SECTOR_ERASE(_X) memset(_X, 0xFF, FLASH_SECTOR_SIZE)     

#include "dbg.h"

#ifdef __cplusplus
}
#endif

#endif
