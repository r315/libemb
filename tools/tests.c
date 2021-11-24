#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <strfunc.h>

#include "board.h"
#include "nvdata.h"
#include "dbg.h"

uint8_t flash_sector[FLASH_SECTOR_SIZE];

extern nvdata_t nvd;

void out_init(void){ }
char out_getchar(void){ return 255; }
void out_putchar(char c){ putchar(c);}
void out_puts(const char *str){ puts(str); }
uint8_t out_nb(char *c){ return 0; }
uint8_t out_kbhit(void){ return 0;}

static stdout_t out_out = {
	out_init,
	out_getchar,
	out_putchar,
	out_puts,
	out_nb,
	out_kbhit
};
static stdout_t *sto = &out_out;



void dumpData(void){
	dbg_HexDump(flash_sector, FLASH_SECTOR_SIZE);
}


void test_Init(void){
uint8_t tmp[NVDATA_SIZE];

	// initilise nvdata
	memset(flash_sector, 0xFF, FLASH_SECTOR_SIZE);
	NV_Init();
	if(nvd.freeBlock == flash_sector){
		printf("Init data ok\n");
		dumpData();
	}
	else{
		printf("Init data fail\n");
		dumpData();
	}

	// initilise with present data nvdata
	memcpy(flash_sector, "1234567890", 10);
	flash_sector[NVDATA_SIZE] = NVDATA_VALID;

	NV_Init();
	NV_Restore(tmp, 10);
	if(strncmp("1234567890", tmp, 10) == 0){
		printf("Init with data ok\n");
		dumpData();
	}
	else{
		printf("Init with data fail\n");
		dumpData();
	}
	// change data
	tmp[2] = '2';
	NV_Save(tmp, 10);
	if(nvd.freeBlock == flash_sector + 0x40){
		printf("Change data ok\n");
		dumpData();
	}else{
		printf("Change data fail\n");
		dumpData();
	}
	memset(tmp, 0xAA, NVDATA_SIZE);
	NV_Read(0, tmp, 10);
	if(strncmp("1224567890", tmp, 10) == 0){
		printf("Read data ok\n");
		dumpData();
	}
	else{
		printf("Read data fail\n");
		dumpData();
	}
}

void test_write(void){
uint8_t tmp[NVDATA_SIZE];
	memset(tmp, '-', NVDATA_SIZE);
	NV_Write(0, tmp, NVDATA_SIZE);
	NV_Sync();
	if(strncmp(nvd.freeBlock - NVDATA_BLOCK_SIZE, tmp, 10) == 0){
		printf("Write data ok\n");
		dumpData();
	}
	else{
		printf("Write data fail\n");
		dumpData();
	}
	/* Loop sector */
	memcpy(tmp, "LAST", 4);
	NV_Write(0, tmp, 4);
	NV_Sync();
	dumpData();
	memcpy(tmp, "LOOP", 4);
	NV_Write(0, tmp, 4);
	NV_Sync();
	dumpData();
}

void test_nvdata(void){
	printf("NVDATA size: %u\n", NVDATA_SIZE);
	test_Init();
	test_write();
}

void floatUnion(void){
union{
	float f;
	uint32_t u;
}f2u;

	f2u.f = 0.164f;

	float f= (float)0x3e27ef9e;

	printf("Value 0x%x (%.3f) f %.3f\n", f2u.u, f2u.f, f);
}

void test_xpitoa(void){
char out[20];
uint32_t len;
	//len = xpftoa(out, 0.123f, FLOAT_MAX_PRECISION);
	dbg_printf("%.4f\n",131.12345678f);	
}

int main(int argc, char **argv){
dbg_init(sto);

test_xpitoa();
//printf("%6f\n", 0.1234567f);
	
return 0;
}
