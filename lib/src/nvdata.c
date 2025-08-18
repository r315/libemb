#include <string.h>
#include <stdio.h>
#include "nvdata.h"
#include "debug.h"

#define NVDATA_BLOCK_SIZE   (nvdata->nvb.size) // Ideally NVDATA_SECTOR_SIZE mod(NVDATA_BLOCK_SIZE) = 0
#define NVDATA_SIZE         (NVDATA_BLOCK_SIZE - 1)
#define NVDATA_STATE        (nvdata->nvb.data[NVDATA_SIZE])

#ifdef ENABLE_DEBUG_NVDATA
#define DBG_TAG     "NVDATA : "
#define DBG_NVDATA_INF(...) DBG_INF(DBG_TAG __VA_ARGS__)
#define DBG_NVDATA_WRN(...) DBG_WRN(DBG_TAG __VA_ARGS__)
#define DBG_NVDATA_ERR(...) DBG_ERR(DBG_TAG __VA_ARGS__)
#define DBG_NVDATA_PRINTF(...) DBG_PRINTF(__VA_ARGS__)
#else
#define DBG_NVDATA_INF(...)
#define DBG_NVDATA_WRN(...)
#define DBG_NVDATA_ERR(...)
#define DBG_NVDATA_PRINTF(...)
#endif

static uint32_t checkEmptyBlock(uint8_t *address);
static uint32_t commit_nv(void);
static uint32_t verify(void);
static nvdata_t *nvdata;

/**
 * Each sector is divided in blocks of size NVDATA_BLOCK_SIZE, which corresponds to
 * n data bytes + 1 byte for status
 * On each write of one byte or more, a new block is written after the current block.
 * When the sector is fully written, it is erased and the operation repeats
 * */

/**
 * @brief Initializes internal nvdata structure with next freeblock
 *             and presistent data
 *
 * @return :    0 if no valid data or sector was initialized, data size
 *                 if valid data read
 */
uint32_t NV_Init(nvdata_t *nv) {
    if(nv == NULL)
        return 0;

    nvdata = nv;

    // Get first block status byte, aka last byte of block
    uint8_t *ptr = ((uint8_t*)nvdata->sector.start + NVDATA_SIZE);
    uint8_t *lastWritten = NULL;
    // Erase nvdata buffer
    nvdata->nvb.next = NULL;
    memset(nvdata->nvb.data, 0xff, NVDATA_SIZE);
    // initialize flash if needed
    if(nvdata->sector.init){
        nvdata->sector.init();
    }

    DBG_NVDATA_INF("Sector 0x%lx, block size is %u bytes", nvdata->sector.start, nvdata->nvb.size);
    // Iterate blocks starting from the end of first block
    do {
        if (*ptr == NVDATA_VALID) {
            // Found valid data block, save it for later
            lastWritten = ptr - NVDATA_SIZE;
            DBG_NVDATA_INF("Valid block found at 0x%x", (unsigned int)lastWritten);
        }
        else if (*ptr == NVDATA_EMPTY) {
            // Current block seams empty, save it on .next pointer
            nvdata->nvb.next = ptr - NVDATA_SIZE;
            // Check if this block is really cleared
            if(checkEmptyBlock(nvdata->nvb.next) == 0){
                // was not, so must be corrupted
                DBG_NVDATA_WRN("Corrupted block");
                DBG_NVDATA_WRN("Erasing");
                return NV_Erase();
            }
            // Confirmed to be empty, is it the next one after last written?
            if (lastWritten != NULL) {
                // yep, so get data from previous block, since was the laston writen and has valid data
                nvdata->sector.read((uint32_t)lastWritten, nvdata->nvb.data, NVDATA_SIZE);
                NVDATA_STATE = NVDATA_RESTORED;
                DBG_NVDATA_INF("Data restored from block 0x%x", (unsigned int)lastWritten);
                return NVDATA_SIZE;
            }
        }
        // Advance next block
    } while ((uint32_t)(ptr += NVDATA_BLOCK_SIZE) < (uint32_t)nvdata->sector.end);
    // No data found
    DBG_NVDATA_WRN("Initialyzed with no data");
    NVDATA_STATE = NVDATA_EMPTY;
    nvdata->nvb.next = (uint8_t*)nvdata->sector.start;
    return 0;
}

/**
 * @brief Restores data from internal nvdata structure to a given buffer
 *
 * @return     : number of bytes copied
 *
 */
uint32_t NV_Restore(uint8_t* data, uint16_t count) {

    if (nvdata->nvb.next == NULL){
        DBG_NVDATA_WRN("Invalid block");
        return 0;
    }

    if (count > NVDATA_SIZE){
        count = NVDATA_SIZE;
    }

    memcpy(data, nvdata->nvb.data, count);

    DBG_NVDATA_INF("Restored %u bytes", count);

    return count;
}
/**
 * @brief Save given data to internal structure and persistent storage
 *
 * @return : Data count if success, 0 otherwise
 *
 */
uint32_t NV_Save(const uint8_t* data, uint16_t count) {

    if (nvdata->nvb.next == NULL){
        DBG_NVDATA_WRN("Invalid block");
        return 0;
    }

    // Truncate to NVDATA_SIZE
    if (count > NVDATA_SIZE){
        count = NVDATA_SIZE;
    }

    memcpy(nvdata->nvb.data, data, count);

    return commit_nv() ? count : 0;
}

/**
 * @brief synchronizes internal data with persistent data
 *             if internal data has changed
 *
 * @return :     0: Failed to synchronize, data differs data on block
 *               1: Data is synchronized
 *               2: Data was commited
 */
uint32_t NV_Sync(void)
{
    DBG_NVDATA_INF("Synchronizing eeprom buffer with flash");
    if(NVDATA_STATE == NVDATA_CHANGED){
        if(!verify()){
            return commit_nv() ? 2 : 0;
        }
    }
    return 1;
}

/**
 * @brief Read bytes from internal structure to a given buffer
 *
 * @param offset     : offset for reading
 * @param data        : destination buffer
 * @param count        : number of bytes to read
 *
 * @return : 0 if offset is invalid or internal data is not initialised
 *
 */
uint32_t NV_Read(uint16_t offset, uint8_t *data, uint32_t len){

    if ((nvdata->nvb.next == NULL) || (offset > NVDATA_SIZE))
        return 0;

    if (((offset + len) > NVDATA_SIZE)) {
        len = NVDATA_SIZE - offset;
    }

    memcpy(data, &nvdata->nvb.data[offset], len);

    DBG_NVDATA_INF("Read %lu bytes from offset %u", len, offset);

    return len;
}

/**
 * @brief Write a given byte buffer to internal structure
 *
 * @param offset     : offset for writing
 * @param data        : source buffer
 * @param count        : number of bytes to be written
 *
 * @return : 0 if offset is invalid or internal data is not initialised
 *
 */
uint32_t NV_Write(uint16_t offset, const uint8_t *data, uint32_t len){

    if ((nvdata->nvb.next == NULL) || (offset > NVDATA_SIZE))
        return 0;

    if (((offset + len) > NVDATA_SIZE)) {
        len = NVDATA_SIZE - offset;
    }

    memcpy(nvdata->nvb.data + offset, data, len);
    NVDATA_STATE = NVDATA_CHANGED;

    DBG_NVDATA_INF("Wrote %lu bytes to offset %u", len, offset);

    return len;
}

/**
 * @brief Erase NV Data
 *
 * @return : 1: if erased, 0 otherwise
 * */
uint32_t NV_Erase(void){
    // no data found erase sector and nvdata buffer
    nvdata->sector.erase((uint32_t)nvdata->sector.start);
    nvdata->nvb.next = (uint8_t*)nvdata->sector.start;
    memset(nvdata->nvb.data, 0xff, NVDATA_BLOCK_SIZE);
    return checkEmptyBlock(nvdata->nvb.next);
}

/**
 * private functions
 */
static uint32_t checkEmptyBlock(uint8_t *address){
    DBG_NVDATA_PRINTF(DBG_TAG"Checking block 0x%x ", (unsigned int)address);
    for (uint16_t i = 0; i < NVDATA_BLOCK_SIZE; i++){
        if(*(address++) != (uint8_t)0xFF){
            DBG_NVDATA_PRINTF("Found data at offset 0x%x\n", i);
            return 0;
        }
    }
    DBG_NVDATA_PRINTF("Empty\n");
    return 1;
}

/**
 * @brief verify the last written data is the
 * same data on the internal structure. Block status
 * byte is not checked
 *
 *
 * @return : 1 if the content of nvdata buffer is the same
 *           as nvdata block on sector, 0 if not
 *
 * */
static uint32_t verify(void){
    uint8_t *ptr;

    // Get data start pointer, if next block is on start of sector
    // data to be verified is ln last block of sector
    ptr = (nvdata->nvb.next == (uint8_t*)nvdata->sector.start) ?
        (uint8_t*)(nvdata->sector.end - NVDATA_BLOCK_SIZE) :
        nvdata->nvb.next - NVDATA_BLOCK_SIZE;
    DBG_NVDATA_INF("Verifying block at 0x%x", (unsigned int)ptr);
    for (uint16_t i = 0; i < NVDATA_SIZE; i++)
    {
        if(nvdata->nvb.data[i] != ptr[i]){
            DBG_NVDATA_INF("Data missmatch at 0x%x", (unsigned int)(ptr + i));
            return 0;
        }
    }

    DBG_NVDATA_INF("Data match");
    return 1;
}

/**
 * @brief Perform sector end check and perform sector erase
 *             if required, then write full data block
 *
 * @return : 1 on success, 0 if fail
 */
static uint32_t  commit_nv(void)
{
    DBG_NVDATA_INF("Commiting data to block 0x%x", (unsigned int)nvdata->nvb.next);
    // Erase flash sector if last was written
    if (nvdata->nvb.next == (uint8_t*)nvdata->sector.start) {
        nvdata->sector.erase(nvdata->sector.start);
    }
    // Set block status byte on internal data
    NVDATA_STATE = NVDATA_VALID;
    // Write block to flash sector
    nvdata->sector.write((uint32_t)nvdata->nvb.next, nvdata->nvb.data, NVDATA_BLOCK_SIZE);
    nvdata->nvb.next += NVDATA_BLOCK_SIZE;
    // wrap sector on last block
    if (nvdata->nvb.next >= (uint8_t*)nvdata->sector.end) {
        nvdata->nvb.next = (uint8_t*)nvdata->sector.start;
    }
    // if verify is successful, status byte is also assumed to be ok.
    return verify();
}
