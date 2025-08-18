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

enum nvdata_e{
    NVDATA_EMPTY = 0xFF,
    NVDATA_VALID = 0xA5,
    NVDATA_CHANGED = 0xB4,
    NVDATA_RESTORED = 0
};

//__attribute__((aligned(32)))
typedef struct nvdata_block {
    uint32_t size;      // number of data bytes
	uint8_t *next;      // Next free block
	uint8_t *data;      // data + state
}nvdata_block_t;

typedef struct nvdata_sector {
    uint32_t start;     // start address of the sector
    uint32_t end;       // end address of the sector
    void(*init)(void);  // function to initialize the sector
    void(*read)(uint32_t addr, uint8_t *src, uint32_t len);
    void(*write)(uint32_t addr, const uint8_t *src, uint32_t len);
    void(*erase)(uint32_t addr);
}nvdata_sector_t;

typedef struct nvdata {
    const nvdata_sector_t sector;
    nvdata_block_t nvb;
}nvdata_t;


/*
Example: for 31 data byte nvdata

#define EEPROM_SIZE        31
#define NVDATA_BLOCK_SIZE (EEPROM_SIZE + 1)  // EEPROM data bytes plus one status byte

static uint8_t eeprom[NVDATA_BLOCK_SIZE];

static nvdata_t app_nvdata = {
    .sector = {
        .start = 0x08000000,
        .end = 0x08010000,
        .init = flash_init_func,
        .read = flash_read_func,
        .write = flash_write_func,
        .erase = flash_erase_sector_func
    },
    .nvb = {
        .size = sizeof(eeprom),
        .data = eeprom
    }
};

*/

/**
 * @brief Initializes internal nvdata structure with next freeblock
 * 			and presistent data
 *
 * @return 0 : if no valid data or sector was initialized, data size
 * 				if valid data read
 */
uint32_t NV_Init(nvdata_t *nvdata);

/**
 * @brief Restores data from internal nvdata structure to a given buffer
 *
 * @return 	: number of bytes restored
 *
 */
uint32_t NV_Restore(uint8_t* data, uint16_t count);
/**
 * @brief Save given data to internal structure and persistent storage
 *
 * @return : Data count if success, 0 othewise
 *
 */
uint32_t NV_Save(const uint8_t* data, uint16_t count);
/**
 * @brief force write of internal structure to persistent memory
 *
 * @return : 0 if nvdata buffer is same as nvblock on sector
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
 * @return : Data count if success, 0 othewise
 *
 */
uint32_t NV_Read(uint16_t offset, uint8_t *data, uint32_t count);
/**
 * @brief Write a given data to internal structure
 *
 * @param offset 	: offset for writing
 * @param data		: source buffer
 * @param count		: number of bytes to be written
 *
 * @return : Data count if success, 0 othewise
 *
 */
uint32_t NV_Write(uint16_t offset, const uint8_t *data, uint32_t count);

/**
 * @brief Erase NV Data
 *
 * @return : 0 if fail to erase flash sector, NVDATA size if ok
 * */
uint32_t NV_Erase(void);


#ifdef __cplusplus
}
#endif

#endif
