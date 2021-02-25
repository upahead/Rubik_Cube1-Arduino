#ifndef __IP5306_H
#define __IP5306_H

#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Wire.h>
#include <Arduino.h>

#define IP5306_I2C_SCL_IO            4
#define IP5306_I2C_SDA_IO            13
#define IP5306_I2C_MASTER_NUM        I2C_NUM_1  

#define IP5306_ADDR                  0x75       //IIC correspondence address of IP5306
#define IP5306_CHECK_POWER 			 0x78       
#define IP5306_CHECK_CHANGE 		 0x70       
#define IP5306_CHECK_FULL            0x71       


class IP5306{
	private:
		uint8_t data;
	public:
		IP5306(){}
		uint8_t get_data(void){return data;}
		uint8_t Ip5306_Check_Power(void);
		void readByte(uint8_t address);
		uint8_t Ip5306_Check_Charge(void);
		uint8_t Ip5306_Check_Full(void);
    ~IP5306(){}
};


#endif /* __IP5306_DRIVER_H */
