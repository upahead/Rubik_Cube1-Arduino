#include "sgp30.h"

/**
  * @brief  Send a 16bit command
  * @param  cmd_val : command
  * @retval success:return 0; failed:return 1
  */
uint8_t sgp30::SGP30_I2c_Write_Cmd(uint16_t cmd_val)
{
	uint8_t cmd[2];
	cmd[0] = cmd_val >> 8;
	cmd[1] = cmd_val;
	Wire.beginTransmission(SGP30_ADDR);
  Wire.write(cmd,2);
    if((Wire.endTransmission()))
    {	
    	Serial.println("end error");
    	return 1;
    }
    return 0;
}

/**
  * @brief  Read register data
  * @param  len:Data length 
  * @param  data_buf:The first address of the data to be read 
  * @retval none
  */
void sgp30::SGP30_I2c_Read(uint8_t len, uint8_t *data_buf)
{
	
	int i=0;
	Wire.requestFrom(SGP30_ADDR, len);
	while(i<len)
	{
		data_buf[i++]=Wire.read();
	}
}

/**
  * @brief  Send soft reset
  * @param  void
  * @retval success:return 0; failed:return 1
  */
uint8_t sgp30::SGP30_Soft_Reset()
{
	Wire.beginTransmission(SGP30_ADDR);
    Wire.write(SGP30_SOFT_RESET_CMD);
    if((Wire.endTransmission()))
    {	
    	Serial.println("end error");
    	return 1;
    }
    return 0;	
}

/**
  * @brief  initialization
  * @param  void
  * @retval success:return 0; failed:return 1
  */
uint8_t sgp30::SGP30_Init()
{
	if (SGP30_Soft_Reset())
	{
		return 1;
	}
	delay(100);

	if (SGP30_I2c_Write_Cmd(SGP30_INIT_AIR_QUALITY))
	{
		return 1;
	}
	delay(100);

	return 0;
}

/**
  * @brief  CRC check
  * @param  ----
  * @retval CRC Code
  */
uint8_t sgp30::CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder;	    
    uint8_t  i = 0, j = 0;  

    remainder = initial_value;

    for(j = 0; j < 2;j++){
        remainder ^= message[j];
        for (i = 0; i < 8; i++){
            if (remainder & 0x80){
                remainder = (remainder << 1)^SGP30_CRC8_POLYNOMIAL;
            }else{
                remainder = (remainder << 1);
            }
        }
    }
    return remainder;
}

/**
  * @brief  Collect data
  * @param  void
  * @retval success:return 0; failed:return 1
  */
uint8_t sgp30::SGP30_Get_Value()
{
    uint8_t recv_buf[6]={0,0,0,0,0,0};

    if(SGP30_I2c_Write_Cmd(SGP30_MEASURE_AIR_QUALITY))
        return 1;
    
    delay(1000);

    SGP30_I2c_Read(6, recv_buf);

    if (CheckCrc8(&recv_buf[0], 0xFF) != recv_buf[2])
        return 1;

    if (CheckCrc8(&recv_buf[3], 0xFF) != recv_buf[5])
        return 1;

    co2_val  = recv_buf[0] << 8 | recv_buf[1];      
    tvoc_val = recv_buf[3] << 8 | recv_buf[4];      

    return 0;
}

/**
  * @brief  return CO2 value
  * @param  void
  * @retval co2_val：CO2 value
  */
uint16_t sgp30::get_co2_val()
{
	return co2_val;
}

/**
  * @brief  reutrn Formaldehyde value
  * @param  void
  * @retval tvoc_val：Formaldehyde value
  */
uint16_t sgp30::get_tvoc_val()
{
	return tvoc_val;	
}
