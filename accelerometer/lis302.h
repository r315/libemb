#ifndef _lis302_h_
#define _lis302_h_

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

#define ACCEL_RD      0x80     // select read
#define ACCEL_MU      0x40     // select multiple data
#define WHO_AM_I      0x0F
#define LIS302_ID     0x3B
#define LIS302_CTRL1  0x20
#define LIS302_CTRL2  0x21
#define LIS302_CTRL3  0x22
#define LIS302_STATUS 0x27
#define AXIS_X        0x29
#define AXIS_Y        0x2B
#define AXIS_Z        0x2D
#define LIS302_ZYXDA  0x08

typedef struct {
  double q; //process noise covariance
  double r; //measurement noise covariance
  double x; //value
  double p; //estimation error covariance
  double k; //kalman gain
} Kalman_Filter;


/*http://interactive-matter.eu/blog/2009/12/18/filtering-sensor-data-with-a-kalman-filter/ */
extern Kalman_Filter accel[3];
Kalman_Filter filterInit(double q, double r, double p, double intial_value);
void filterUpdate(Kalman_Filter* state, double measurement);
void getFilteredAxis(signed char *buf);

#if defined(__BB__)
#define ACCEL_CS (1<<6)
#define ACCEL_IO_INIT     LPC_GPIO0->FIODIR |= ACCEL_CS
#define SELECT_ACCEL      LPC_GPIO0->FIOCLR = ACCEL_CS
#define DESELECT_ACCEL    LPC_GPIO0->FIOSET = ACCEL_CS
#endif


#endif
