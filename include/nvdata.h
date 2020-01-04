#pragma once
#ifndef _nvdata_h_
#define _nvdata_h_

/**
 * Non-volatile data storage
 * 
 * Data is saved in blocks of data size + 1 in the persistent storage (internal/external flash)
 * until the flash sector is full.
 * 
 * Upon a full sector, the sector is erased and data written from the beginning.
 * 
 * Sector start                               sector start + n*(data size + 1)
 * v                                          v
 * +-------------------------+-------+        +-------------------------+-------+
 * |        data             | state | ...... |        data             | state |
 * +-------------------------+-------+        +-------------------------+-------+
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "board.h"	

#ifndef NVDATA_SIZE
#define NVDATA_SIZE         31 
#endif
#define NVDATA_BLOCK_SIZE   (NVDATA_SIZE + 1) // Ideally NVDATA_SECTOR_SIZE mod(NVDATA_BLOCK_SIZE) = 0

enum nvdata_e{
    NVDATA_EMPTY = 0xFF,
    NVDATA_VALID = 0xCC,
    NVDATA_CHANGED = 0xBB,
    NVDATA_RESTORED = 0
};

typedef struct nvdata {
	uint8_t data[NVDATA_SIZE];
	uint8_t state;
	uint8_t *freeBlock;
}__attribute__ ((aligned (32))) nvdata_t;


/**
 * @brief Initializes internal nvdata structure with next freeblock
 * 			and presistent data
 * 
 * @return 0 : if no valid data or sector was initialized, data size 
 * 				if valid data read
 */
uint32_t NV_Init();

/**
 * @brief Restores data from internal nvdata structure to a given buffer
 * 
 * @return 	: number of bytes coppied
 * 			
 */
uint32_t NV_Restore(uint8_t* data, uint16_t count);
/**
 * @brief Save given data to internal structure and persistent storage
 * 
 * @return : 0 if no free blocks, data count if success
 * 
 */
uint32_t NV_Save(uint8_t* data, uint16_t count);
/**
 * @brief force write of internal structure to persistent memory
 * 
 * @return : 0 if failed
 * 
 */
uint32_t NV_Sync(void);
/**
 * @brief Read bytes from internal structure to a given buffer
 * 
 * @param offset 	: offset for reading
 * @param data		: destination buffer
 * @param count		: number of bytes to read
 * 		
 * @return : 0 if offset is invalid or data is not initialised
 * 
 */
uint32_t NV_Read(uint16_t offset, uint8_t *data, uint32_t count);
/**
 * @brief Write a given byte buffer to internal structure
 * 
 * @param offset 	: offset for writing
 * @param data		: source buffer
 * @param count		: number of bytes to be written
 * 		
 * @return : 0 if offset is invalid or data is not initialised
 * 
 */
uint32_t NV_Write(uint16_t offset, uint8_t *data, uint32_t count);


#ifdef __cplusplus
}
#endif

#endif