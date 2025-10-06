#ifndef FLASH_H
#define FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

void FLASH_Read (uint32_t addr, uint8_t *dst, uint32_t len);
void FLASH_Write(uint32_t address, const uint8_t *data, uint32_t count);
void Flash_EraseSector(uint32_t sectorAddress);
uint32_t Flash_GetSize(void);

#ifdef __cplusplus
}
#endif

#endif // FLASH_H