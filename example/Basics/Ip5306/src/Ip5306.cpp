#include"Ip5306.h"

/**
  * @brief  Read a byte of data
  * @param  address:Register address 
  * @retval none
  */
void IP5306::readByte(uint8_t address)
{
	Wire.beginTransmission(IP5306_ADDR);
	Wire.write(address);
  if(Wire.endTransmission() == 0 && Wire.requestFrom(IP5306_CHECK_POWER, 1)) 
    data = Wire.read() & 0xF0;
}

/**
  * @brief  Check the power supply
  * @param  void
  * @retval Electricity value
  */
uint8_t IP5306::Ip5306_Check_Power(void)
{
    readByte(IP5306_CHECK_POWER);
    if(get_data() == 0x00)
        return 100;
    else if(get_data() == 0x80)
        return 75;
    else if(get_data() == 0xC0)
        return 50;
    else if(get_data() == 0xE0)
        return 25;
    else
        return 0;
}

/**
  * @brief  Check if it is charging
  * @param  void
  * @retval 1：charging 0：no charge
  */
uint8_t IP5306::Ip5306_Check_Charge(void)
{
    readByte(IP5306_CHECK_CHANGE);
    if(get_data()>>3 & 0x01){
      return 1;
    }
    return 0;
}

/**
  * @brief  Check if it is full
  * @param  void
  * @retval 1：full 0：no full
  */
uint8_t IP5306::Ip5306_Check_Full(void)
{
    readByte(IP5306_CHECK_FULL);
    if(get_data()>>3 & 0x01){
      return 1;
    }
    return 0;
}
