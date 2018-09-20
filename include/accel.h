#ifndef _accel_h_
#define _accel_h_

#include <stdint.h>
#include <spi.h>

typedef struct{
	uint16_t x;
	uint16_t y;
	uint16_t z;
}Accel_Type;


uint8_t ACCEL_Init(void *dev);
uint8_t ACCEL_Read(Accel_Type *acc);

void ACCEL_SetInt(uint16_t threshold);

#endif /* _accel_h_ */
