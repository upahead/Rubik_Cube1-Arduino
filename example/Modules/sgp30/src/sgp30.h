#ifndef __SGP30_H
#define __SGP30_H

#include <Arduino.h>
#include <Wire.h>

#define SGP30_ADDR                  0x58          //IIC correspondence address of SGP30

#define SGP30_SOFT_RESET_CMD        0x06          
#define SGP30_INIT_AIR_QUALITY      0x2003        
#define SGP30_MEASURE_AIR_QUALITY   0x2008        
#define SGP30_CRC8_POLYNOMIAL       0x31

class sgp30{
	public:
		uint8_t SGP30_I2c_Write_Cmd(uint16_t cmd_val);
		void SGP30_I2c_Read(uint8_t len, uint8_t *data_buf);
		uint8_t SGP30_Soft_Reset(void);
		uint8_t SGP30_Init(void);
		uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value);
		uint8_t SGP30_Get_Value();
		uint16_t get_co2_val();
		uint16_t get_tvoc_val();

	private:
		uint16_t co2_val, tvoc_val;
};

#endif 
