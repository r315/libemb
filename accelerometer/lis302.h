#ifndef _lis302_h_
#define _lis302_h_

#ifdef __cplusplus
extern "C" {
#endif

/************************************
LIS302D Pinout (Top View)
        __SCK___
 Vdd_io|1  14 13|SDI
    GND|2     12|SDO
    RSV|3     11|RSV
    GND|4     10|GND           Z  
    GND|5      9|INT2       Y\ | /x
    Vdd|6  7   8|INT1         \|/
       |___CS___|
**************************************/	   

#define LIS302_READ          0x80
#define LIS302_WRITE         0x00
#define LIS302_READ_MU       0x40     // select multiple data
#define LIS302_WHO_AM_I      0x0F
#define LIS302_ID            0x3B
#define LIS302_CTRL1         0x20
#define LIS302_CTRL2         0x21
#define LIS302_CTRL3         0x22
#define LIS302_STATUS        0x27
#define LIS302_OUT_X         0x29
#define LIS302_OUT_Y         0x2B
#define LIS302_OUT_Z         0x2D

// Status Bits
#define LIS302_STATUS_ZYXOR  0x01
#define LIS302_STATUS_ZOR    0x02
#define LIS302_STATUS_YOR    0x04
#define LIS302_STATUS_XOR    0x08
#define LIS302_STATUS_ZYXDA  0x10
#define LIS302_STATUS_ZDA    0x20
#define LIS302_STATUS_YDA    0x40
#define LIS302_STATUS_XDA    0x80

/*
typedef struct {
  double q; //process noise covariance
  double r; //measurement noise covariance
  double x; //value
  double p; //estimation error covariance
  double k; //kalman gain
} Kalman_Filter;
*/

/*
http://interactive-matter.eu/blog/2009/12/18/filtering-sensor-data-with-a-kalman-filter/ 
extern Kalman_Filter accel[3];
Kalman_Filter filterInit(double q, double r, double p, double intial_value);
void filterUpdate(Kalman_Filter* state, double measurement);
void getFilteredAxis(signed char *buf);
*/

#if defined(__BB__)
#define ACCEL_PORT        LPC_GPIO0
#define ACCEL_CS          (1<<6)
#define ACCEL_IO_INIT     ACCEL_PORT->FIODIR |= ACCEL_CS
#define SELECT_ACCEL      ACCEL_PORT->FIOCLR = ACCEL_CS
#define DESELECT_ACCEL    ACCEL_PORT->FIOSET = ACCEL_CS
#else
#define ACCEL_CS
#define ACCEL_IO_INIT
#define SELECT_ACCEL
#define DESELECT_ACCEL
#endif

#ifdef __cplusplus
}
#endif

#endif
