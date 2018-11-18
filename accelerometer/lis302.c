#include <LPC17xx.h>
#include <accel.h>
#include <spi.h>

#include "lis302.h"

static Spi_Type *accel_spi;

//Kalman_Filter accel[3];
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
	DelayMs(100);

    SELECT_ACCEL;
	SPI_Send(accel_spi, LIS302_READ | LIS302_WHO_AM_I);
    id = SPI_Send(accel_spi, 0xFF);
    DESELECT_ACCEL;

	if(id != LIS302_ID){
       return 0;
    }      

	SELECT_ACCEL;
	SPI_Send(accel_spi, LIS302_WRITE | LIS302_CTRL1);
	SPI_Send(accel_spi, 0x41);          // power it up, enable axis
	DESELECT_ACCEL;

	SELECT_ACCEL;
	SPI_Send(accel_spi, LIS302_WRITE | LIS302_CTRL2);
	SPI_Send(accel_spi, 0x0);          
	DESELECT_ACCEL;

	SELECT_ACCEL;
	SPI_Send(accel_spi, LIS302_WRITE | LIS302_CTRL3);
	SPI_Send(accel_spi, 0x0);          
	DESELECT_ACCEL;
	
/*	for(i=0;i<3;i++)        // q, r , p , initial val
		accel[i] = filterInit(0.125, 32, 1, 0);
	*/
    return id;
}
//--------------------------------------
//
//--------------------------------------
uint8_t ACCEL_Read(Accel_Type *acc)
{
uint8_t status;

	// Read Status
	SELECT_ACCEL;	
	SPI_Send(accel_spi, LIS302_READ | LIS302_STATUS);
	status  = SPI_Send(accel_spi, 0xFF);
	DESELECT_ACCEL;

	// Check is has new data
	if( (status & LIS302_STATUS_ZYXDA) == 0 )
		return 0;	

    SELECT_ACCEL;
	SPI_Send(accel_spi, LIS302_READ | LIS302_READ_MU | LIS302_OUT_X);
	acc->x = (int8_t)SPI_Send(accel_spi, 0xFF);
	SPI_Send(accel_spi, 0xFF);
	acc->y = (int8_t)SPI_Send(accel_spi, 0xFF);
	SPI_Send(accel_spi, 0xFF);
	acc->z = (int8_t)SPI_Send(accel_spi, 0xFF);
	SPI_Send(accel_spi, 0xFF);
	DESELECT_ACCEL;	

	return 1;	
}

/*
//--------------------------------------
// ACCEL_AXIS(OUT_X);
//--------------------------------------
signed char getAxis(unsigned char axis)
{
signed char tmp;   
	
	for(tmp=0; tmp<50; tmp++){
		SELECT_ACCEL;	
		SPI_Send(accel_spi, LIS302_READ | LIS302_STATUS);
		if(SPI_Send(accel_spi, 0xFF) & LIS302_ZYXDA){
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
{
unsigned char i;
	getAllaxis(buf);
	
	for(i=0;i<3;i++)
	{
		filterUpdate(&accel[i],buf[i]);	
		buf[i] = accel[i].x;		
	}
}
*/