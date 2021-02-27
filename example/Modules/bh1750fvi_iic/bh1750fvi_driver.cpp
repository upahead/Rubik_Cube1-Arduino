#include <Wire.h>
#include "Arduino.h"
#include"bh1750fvi_driver.h"


/**
  * @brief  Read brightness data once
  * @param  void
  * @retval none
  */
void BH1750FVI::BH1750FVI_READ_DATA(void)
{
	Wire.beginTransmission(BH1750FVI_ADDR);                       
    Wire.write(ONE_TIME_H_RESOLUTION_MODE);                       
	 uint8_t tmp = Wire.endTransmission();                        
   if (tmp)                                                     
   	{
        	Serial.println("endTransmission Error!!!!");        
            exit(0); 
    	}

    Wire.requestFrom(BH1750FVI_ADDR, 2*sizeof(uint8_t));       
    for (uint8_t i = 0; i < 2; ++i)                             
        buf[i] = Wire.read();
    delay(1);
    
    if (Wire.available() != 0)                                  
    {  
        Serial.println("available error!!!!");                  
        exit(0);
    }
	    dis_data = buf[0];
    	dis_data = (dis_data<<8)+buf[1];                          
    	temp = (float)dis_data/1.2;                               //Get the integer part
    	temp += (float)((int)(10*dis_data/1.2)%10)/10;            //Get the fractional part and get the complete data
}

/**
  * @brief  Get the brightness data
  * @param  void
  * @retval brightness data，（0-65535），unit: lx
  */
float BH1750FVI::get_Data(void)
{
		BH1750FVI_READ_DATA();
		return temp;
} 