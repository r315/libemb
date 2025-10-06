
#include <stdint.h>
#include "at32f4xx.h"

#define FLASH_KEY1                  ((uint32_t)0x45670123)
#define FLASH_KEY2                  ((uint32_t)0xCDEF89AB)

extern void DelayMs(uint32_t ms);

/**
 * @brief Unlock the FLASH control register access
 *
 * @return 1: success, 0: otherwise
 */
static uint8_t flash_unlock(void)
{
    if(FLASH->CTRL & FLASH_CTRL_LCK){
        FLASH->FCKEY = FLASH_KEY1;
        FLASH->FCKEY = FLASH_KEY2;
    }

    return !(FLASH->CTRL & FLASH_CTRL_LCK);
}

/**
 * @brief Lock the FLASH control register access
 *
 */
static void flash_lock(void)
{
    FLASH->CTRL = FLASH_CTRL_LCK;
}

/**
 * @brief Wait for flash operation to end
 *
 * @return 1: operation done, 0: timeout
 */
static uint8_t flash_wait_busy(void)
{
    uint32_t timeout = 30;

    while(FLASH->STS & FLASH_STS_BSY) {
        DelayMs(1000);

        if(!(--timeout)) {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Programs data into flash memory.
 * This programs each byte individually, meaning
 * that there is no alignment problem with address
 * or data.
 *
 * @param address   Flash destination address
 * @param data      Data bytes to program
 * @param count     Number of bytes to program
 */
void FLASH_Write(uint32_t address, const uint8_t *data, uint32_t count)
{
    uint8_t *psrc = (uint8_t*)data;
    uint8_t success;

    if(flash_unlock()){
        for (uint16_t i = 0; i < count; i++, psrc++){
            FLASH->CTRL |= FLASH_CTRL_PRGM;

            *((__IO uint8_t*)address + i) = *psrc;
            success = flash_wait_busy();

            FLASH->CTRL |= FLASH_CTRL_PRGM;

            if(!success){
                break;
            }
        }
    }

    flash_lock();
}

/**
 * @brief Erase 1k selctor on flash
 *
 * @param address:  start address for erasing
 * @return : 0 on fail
 * */
void Flash_EraseSector(uint32_t address)
{
    if(flash_unlock()){
        /* Proceed to erase the page */
        FLASH->CTRL |= FLASH_CTRL_PGERS;
        FLASH->ADDR = address;
        FLASH->CTRL |= FLASH_CTRL_STRT;
    }

    flash_wait_busy();

    FLASH->CTRL = 0;

    flash_lock();
}

void FLASH_Read (uint32_t addr, uint8_t *dst, uint32_t len)
{
    const uint8_t *src = (uint8_t*)addr;
    while(len--){
        *dst++ = *src++;
    }
}

uint32_t Flash_GetSize(void)
{
    return (1 << 17);
}