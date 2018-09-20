
#include <accel.h>
#include <spi.h>
#include <clock.h>

#include "lis302.h"

static Spi_Type *accel_spi;

Kalman_Filter accel[3];
//--------------------------------------
//
//--------------------------------------
uint8_t ACCEL_Init(void *dev)
{
uint8_t i, id = 0;
	
	accel_spi = (Spi_Type *)dev;

	ACCEL_IO_INIT;

	SELECT_ACCEL;
	SPI_Send(accel_spi, LIS302_CTRL1);
    id = SPI_Send(accel_spi, 0x00);      // power down
    DESELECT_ACCEL;
	CLOCK_DelayMs(100);
    SELECT_ACCEL;
	SPI_Send(accel_spi, ACCEL_RD|WHO_AM_I);
    id = SPI_Send(accel_spi, 0xFF);
    DESELECT_ACCEL;
    
    if(id == LIS302_ID)
    {
        SELECT_ACCEL;
        SPI_Send(accel_spi, LIS302_CTRL1);
        SPI_Send(accel_spi, 0x40);          // power it up
        DESELECT_ACCEL;
    }        
	
	for(i=0;i<3;i++)        // q, r , p , initial val
		accel[i] = filterInit(0.125, 32, 1, 0);
	
    return id;
}
//--------------------------------------
// ACCEL_AXIS(OUT_X);
//--------------------------------------
signed char getAxis(unsigned char axis)
{
signed char tmp;   
	
	for(tmp=0; tmp<50; tmp++){
		SELECT_ACCEL;	
		SPI_Send(accel_spi, ACCEL_RD|LIS302_STATUS);
		if(SPI_Send(accel_spi, 0xFF)&LIS302_ZYXDA){
			DESELECT_ACCEL;
			break;
		}
		DESELECT_ACCEL;
	}	
	
	SELECT_ACCEL;	
	SPI_Send(accel_spi, ACCEL_RD|axis);
	tmp = SPI_Send(accel_spi, 0xFF);
	DESELECT_ACCEL;
	return tmp;
}
//--------------------------------------
//
//--------------------------------------
uint8_t ACCEL_Read(Accel_Type *acc)
{
uint8_t i;

	for(i = 0; i<50; i++){
		SELECT_ACCEL;	
		SPI_Send(accel_spi, ACCEL_RD|LIS302_STATUS);
		if(SPI_Send(accel_spi, 0xFF)&LIS302_ZYXDA){
			DESELECT_ACCEL;
			break;
		}
		DESELECT_ACCEL;
	}	

    SELECT_ACCEL;
	SPI_Send(accel_spi, ACCEL_RD|ACCEL_MU|AXIS_X);
	acc->x = SPI_Send(accel_spi, 0xFF);
	SPI_Send(accel_spi, 0xFF);
	acc->y = SPI_Send(accel_spi, 0xFF);
	SPI_Send(accel_spi, 0xFF);
	acc->z = SPI_Send(accel_spi, 0xFF);
	SPI_Send(accel_spi, 0xFF);
	DESELECT_ACCEL;		
}
//--------------------------------------
//
//--------------------------------------
Kalman_Filter filterInit(double q, double r, double p, double intial_value)
{
  Kalman_Filter result;
  result.q = q;
  result.r = r;
  result.p = p;
  result.x = intial_value;

  return result;
}
//--------------------------------------
//
//--------------------------------------
void filterUpdate(Kalman_Filter* state, double measurement)
{
  //prediction update
  //omit x = x
  state->p = state->p + state->q;

  //measurement update
  state->k = state->p / (state->p + state->r);
  state->x = state->x + state->k * (measurement - state->x);
  state->p = (1 - state->k) * state->p;
}
//--------------------------------------
//
//--------------------------------------
void getFilteredAxis(signed char *buf)
{/*
unsigned char i;
	getAllaxis(buf);
	
	for(i=0;i<3;i++)
	{
		filterUpdate(&accel[i],buf[i]);	
		buf[i] = accel[i].x;		
	}*/
}
