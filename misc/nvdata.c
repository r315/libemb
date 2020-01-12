#include <string.h>
#include <nvdata.h>

nvdata_t nvd = { .freeBlock = NULL};
static uint32_t checkEmptyBlock(uint8_t *address);
static uint32_t  commit_nv(void);

/**
 * Each sector is divided in blocks with size NVDATA_BLOCK_SIZE, which corresponds to
 * n data bytes + 1 byte for status
 * On each write of one byte or more, a new block is written after the current block.
 * When the sector is fully written, it is erased and the operation repeats
 * */

/**
 * @brief Initializes internal nvdata structure with next freeblock
 * 			and presistent data
 * 
 * @return :	0 if no valid data or sector was initialized, data size 
 * 				if valid data read
 */
uint32_t NV_Init() {
	uint8_t *ptr = ((uint8_t*)NVDATA_SECTOR_START + NVDATA_SIZE);
	uint8_t *lastWritten = NULL;

	nvd.freeBlock = NULL;
	memset(nvd.data, 0xff, NVDATA_SIZE);

	NVDATA_SECTOR_INIT;

	do {
		if (*ptr == NVDATA_VALID) {
			lastWritten = ptr;
		}
		else if (*ptr == NVDATA_EMPTY) {
			//Set freeblock to the found "empty" block
			nvd.freeBlock = ptr - NVDATA_SIZE;
			
			if(checkEmptyBlock(nvd.freeBlock) == 0){
				// Data corrupted
				break;
			}

			if (lastWritten != NULL) {
				//get data from previous block, if has valid data
				NVDATA_SECTOR_READ(nvd.data, (uint8_t*)(lastWritten - NVDATA_SIZE), NVDATA_SIZE);
				nvd.state = NVDATA_RESTORED;
				return NVDATA_SIZE;
			}			
			nvd.state = NVDATA_EMPTY;
			return 0;
		}
	} while ((uint32_t)(ptr += NVDATA_BLOCK_SIZE) < (uint32_t)NVDATA_SECTOR_END);
	// no data found initialize sector
	NVDATA_SECTOR_ERASE((uint32_t)NVDATA_SECTOR_START);
	nvd.freeBlock = (uint8_t*)NVDATA_SECTOR_START;
	nvd.state = NVDATA_EMPTY;
	return 0;
}

/**
 * @brief Restores data from internal nvdata structure to a given buffer
 * 
 * @return 	: number of bytes coppied
 * 			
 */
uint32_t NV_Restore(uint8_t* data, uint16_t count) {
	
	if (count > NVDATA_SIZE)
		count = NVDATA_SIZE;

	if (nvd.freeBlock == NULL)
		return 0;

	memcpy(data, nvd.data, count);

	return count;
}
/**
 * @brief Save given data to internal structure and persistent storage
 * 
 * @return : 0 if no free blocks, data count if success
 * 
 */
uint32_t NV_Save(uint8_t* data, uint16_t count) {
	// Truncate to NVDATA_SIZE
	if (count > NVDATA_SIZE)
		count = NVDATA_SIZE;

	if (nvd.freeBlock == NULL)
		return 0;

	memcpy(nvd.data, data, count);
	return commit_nv();
}

/**
 * @brief synchronizes internal data with persistent data
 * 			if internal data has changed
 * 
 * @return : 	0 if data is synchronized, NVDATA_SIZE if data was commited
 */
uint32_t NV_Sync(void){
uint32_t res = 0;
	if(nvd.state == NVDATA_CHANGED){
		res = commit_nv();		
	}
	return res;
}

/**
 * @brief Read bytes from internal structure to a given buffer
 * 
 * @param offset 	: offset for reading
 * @param data		: destination buffer
 * @param count		: number of bytes to read
 * 		
 * @return : 0 if offset is invalid or internal data is not initialised
 * 
 */
uint32_t NV_Read(uint16_t offset, uint8_t *data, uint32_t count){
	if((&nvd.data[offset] > &nvd.data[NVDATA_SIZE]) || (nvd.freeBlock == NULL))
		return 0;

	if(&nvd.data[offset + count] > &nvd.data[NVDATA_SIZE])
		count = &nvd.data[NVDATA_SIZE] - &nvd.data[offset];

	memcpy(data, &nvd.data[offset], count);
	return count;
}

/**
 * @brief Write a given byte buffer to internal structure
 * 
 * @param offset 	: offset for writing
 * @param data		: source buffer
 * @param count		: number of bytes to be written
 * 		
 * @return : 0 if offset is invalid or internal data is not initialised
 * 
 */
uint32_t NV_Write(uint16_t offset, uint8_t *data, uint32_t count){
	if((&nvd.data[offset] > &nvd.data[NVDATA_SIZE]) || (nvd.freeBlock == NULL))
		return 0;

	if(&nvd.data[offset + count] > &nvd.data[NVDATA_SIZE])
		count = &nvd.data[NVDATA_SIZE] - &nvd.data[offset];

	memcpy(&nvd.data[offset], data, count);
	nvd.state = NVDATA_CHANGED;
	return count;
}

/**
 * private functions
 */
static uint32_t checkEmptyBlock(uint8_t *address){
	for (uint16_t i = 0; i < NVDATA_BLOCK_SIZE; i++){
		if(*(address++) != (uint8_t)0xFF){
			return 0;
		}
	}
	return 1;
}

/**
 * @brief verify the last written block against the
 * 			current data on the internal structure.
 * 
 * @return : 1 if the content is the same, 0 if not
 * 
 * */
static uint32_t verify(void){
uint8_t *ptr = nvd.data - NVDATA_BLOCK_SIZE;

	for (uint16_t i = 0; i < NVDATA_SIZE; i++)
	{
		if(nvd.data[i] != ptr[i])
			return 0;
	}
	return 1;
}

/**
 * @brief Perform sector end check and perform sector erase
 * 			if required, then write full data block
 * 
 * @return : NVDATA_SIZE on success, 0 if fail
 */
static uint32_t  commit_nv(void){

	if ((uint32_t)(nvd.freeBlock + NVDATA_BLOCK_SIZE) >= (uint32_t)NVDATA_SECTOR_END) {
		NVDATA_SECTOR_ERASE((uint32_t)NVDATA_SECTOR_START);
		nvd.freeBlock = (uint8_t*)NVDATA_SECTOR_START;
	}

	nvd.state = NVDATA_VALID;
	NVDATA_SECTOR_WRITE(nvd.freeBlock, nvd.data, NVDATA_BLOCK_SIZE);
	nvd.freeBlock += NVDATA_BLOCK_SIZE;

	if(!verify()){
		return 0;
	}

	return NVDATA_SIZE;
}