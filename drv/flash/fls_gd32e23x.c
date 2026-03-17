#include <string.h>
#include "flash.h"
#include "gd32e23x.h"

extern void DelayMs(uint32_t ms);

/**
 * @brief Unlock the FLASH control register access
 *
 * @return 1: success, 0: otherwise
 */
static uint8_t flashUnlock(void)
{
    if((RESET != (FMC_CTL & FMC_CTL_LK))){
        /* write the FMC key */
        FMC_KEY = UNLOCK_KEY0;
        FMC_KEY = UNLOCK_KEY1;
    }

    return !(FMC_CTL & FMC_CTL_LK);
}

/**
 * @brief Lock the FLASH control register access
 *
 */
static void flashLock(void)
{
    FMC_CTL |= FMC_CTL_LK;
}

/**
 * @brief Wait for flash operation to end
 *
 * @return 1: operation done, 0: timeout
 */
static uint8_t flashWaitBusy(void)
{
    uint32_t Timeout = 50000;

    while(FMC_STAT & FMC_STAT_BUSY) {
        DelayMs(1000);
        if(!(--Timeout)) {
            return 0;
        }
    }

    return 1;
}
/**
  * @brief  Program halfword (16-bit) at a specified address
  * @note   The function flashUnlock() should be called before to unlock the FLASH interface
  *         The function flashLock() should be called after to lock the FLASH interface
  *
  * @note   If an erase and a program operations are requested simultaneously,
  *         the erase operation is performed before the program one.
  *
  * @note   FLASH should be previously erased before new programmation (only exception to this
  *         is when 0x0000 is programmed)
  *
  * @param  Address:      Specifies the address to be programmed.
  * @param  Data:         Specifies the data to be programmed
  *
  * @return 1: success, 0: timeout
  */
 static uint8_t flashProgramHalfWord(uint32_t Address, uint16_t Data)
 {
    FMC_CTL |= FMC_CTL_PG;
    *(__IO uint16_t*)Address = Data;

    return flashWaitBusy();
 }


/**
 * @brief
 * @param addr
 * @param dst
 * @param len
 */
void FLASH_Read (uint32_t addr, uint8_t *dst, uint32_t len)
{
    memcpy(dst, (void*)addr, len);
}
/**
 * @brief
 * @param address
 * @param data
 * @param count
 */
void FLASH_Write(uint32_t address, const uint8_t *data, uint32_t count)
{
    uint16_t *psrc = (uint16_t*)data;

    if(flashUnlock()){
        for (uint16_t i = 0; i < count; i+= 2, psrc++){
            if(!flashProgramHalfWord(address + i, *psrc)){
                break;
            }
        }
    }

    flashLock();
}

/**
 * @brief Erase 1k selctor on flash
 *
 * @param address:  start address for erasing
 * @return : 0 on fail
 * */
void Flash_EraseSector(uint32_t address)
{
    if(flashUnlock()){
        /* Proceed to erase the page */
        FMC_CTL |= FMC_CTL_PER;
        FMC_ADDR = address;
        FMC_CTL |= FMC_CTL_START;
    }

    flashWaitBusy();

    FMC_CTL = 0;

    flashLock();
}