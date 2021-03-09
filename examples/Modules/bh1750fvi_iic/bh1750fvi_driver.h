#ifndef __BH1750FVI_DRIVER_H
#define __BH1750FVI_DRIVER_H

#include <Wire.h>
#define BH1750FVI_ADDR                  0x23		//IIC correspondence address of bh1750fvi 

/*
 *Bh1750fvi supports single or continuous measurement modes, and each measurement mode provides 
  three resolutions of 0.5Lux, 1Lux and 4lux.
 *The higher the resolution, the longer the measurement time. In the single measurement mode, 
  the sensor will automatically enter the power down mode after each measurement.
 *One Time H-Resolution Mode 0010 0000
 *One Time H-Resolution Mode2 0010 0001
 *One Time L-Resolution Mode2 0010 0011
 */

#define ONE_TIME_H_RESOLUTION_MODE 0x20         

class BH1750FVI{
	private:
	  uint8_t buf[4] = {0};
    uint32_t dis_data;               
    float temp;
	
	public:
		BH1750FVI(){}
		~BH1750FVI(){}
    float get_Data(void);        
		void BH1750FVI_READ_DATA(void);	
};

#endif /* __BH1750FVI_DRIVER_H */
