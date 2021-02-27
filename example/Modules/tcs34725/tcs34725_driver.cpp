/*!
 *  @file Adafruit_TCS34725.cpp
 *
 *  @mainpage Driver for the TCS34725 digital color sensors.
 *
 *  @section intro_sec Introduction
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing
 *  products from Adafruit!
 *
 *  @section author Author
 *
 *  KTOWN (Adafruit Industries)
 *
 *  @section license License
 *
 *  BSD (see license.txt)
 *
 *  @section HISTORY
 *
 *  v1.0 - First release
 */

#include "tcs34725_driver.h"

/*
  * @brief  Writes a register and an 8 bit value over I2C
  * @param  reg:Register address
  * @param  value：data
  * @retval none
*/
void TCS34725::write8 (uint8_t reg, uint32_t value)
{
  Wire.beginTransmission(TCS34725_ADDRESS);
  Wire.write(TCS34725_COMMAND_BIT | reg);
   Wire.write(value);
  if((Wire.endTransmission()))
    Serial.println("end error");
}

/*
  * @brief  Reads an 8 bit value over I2C
  * @param  reg:Register address 
  * @retval value
*/
uint8_t TCS34725::read8(uint8_t reg)
{
  uint8_t temp=-2;
  Wire.beginTransmission(TCS34725_ADDRESS);
  Wire.write(TCS34725_COMMAND_BIT | reg);

  if(Wire.endTransmission())
    Serial.println("end error");
  Wire.requestFrom(TCS34725_ADDRESS, 1);
  temp=Wire.read();
  return temp;
}

/*
  * @brief  Reads a 16 bit values over I2C
  * @param  reg_addr:Register address
  * @retval temp:value
*/
uint16_t TCS34725::read16(uint8_t reg)
{
  uint16_t x; uint16_t t;

  Wire.beginTransmission(TCS34725_ADDRESS);
  Wire.write(TCS34725_COMMAND_BIT | reg);
  Wire.endTransmission();

  Wire.requestFrom(TCS34725_ADDRESS, 2);
  t = Wire.read();
  x = Wire.read();
  x <<= 8;
  x |= t;
  return x;
}

/*
  * @brief  Enables the device
  * @param  void 
  * @retval none
*/
void TCS34725::enable(void)
{
  write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
  delay(3);
  write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);  
}

/*
  * @brief  Disables the device (putting it in lower power sleep mode)
  * @param  void 
  * @retval none
*/
void TCS34725::disable(void)
{
  /* Turn the device off to save power */
  uint8_t reg = 0;
  reg = read8(TCS34725_ENABLE);
  write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

/*
  * @brief  Constructor
  * @param  it: Integration Time 
  * @param  gain: gain 
  * @retval none
*/
TCS34725::TCS34725(tcs34725IntegrationTime_t it, tcs34725Gain_t gain) 
{
  _tcs34725Initialised = false;
  _tcs34725IntegrationTime = it;
  _tcs34725Gain = gain;
}

/*
  * @brief  Initializes I2C and configures the sensor
  * @param  void
  * @retval true:True if initialization was successful, otherwise false.
*/
bool TCS34725::begin(void) 
{
  
  /* Make sure we're actually connected */
  uint8_t x = read8(TCS34725_ID);
  if ((x != 0x44) && (x != 0x10))
  {
    Serial.println("error");
    return false;
  }
  _tcs34725Initialised = true;

  /* Set default integration time and gain */
  setIntegrationTime(_tcs34725IntegrationTime);
  setGain(_tcs34725Gain);

  /* Note: by default, the device is in power down mode on bootup */
  enable();

  return true;
}
  
/*
  * @brief  Sets the integration time for the TC34725
  * @param  it: Integration Time 
  * @retval none
*/
void TCS34725::setIntegrationTime(tcs34725IntegrationTime_t it)
{
  if (!_tcs34725Initialised) begin();

  /* Update the timing register */
  write8(TCS34725_ATIME, it);

  /* Update value placeholders */
  _tcs34725IntegrationTime = it;
}

/*
  * @brief  Adjusts the gain on the TCS34725
  * @param  gain:  Gain (sensitivity to light) 
  * @retval none
*/
void TCS34725::setGain(tcs34725Gain_t gain)
{
  if (!_tcs34725Initialised) begin();

  /* Update the timing register */
  write8(TCS34725_CONTROL, gain);

  /* Update value placeholders */
  _tcs34725Gain = gain;
}

/**
  * @brief  Get single channel data
  * @param  reg: Register address
  * @retval value
  */
uint16_t TCS34725::TCS34725_GetChannelData(uint8_t reg)
{
    uint16_t data = 0;
    if (!_tcs34725Initialised) begin();
    data = read16(reg);
    
    return data;
}

/*
  * @brief  Reads the raw red, green, blue and clear channel values
  * @param  *r: Red value
  * @param  *g: Green value
  * @param  *b: Blue value
  * @param  *c: Clear channel value
  * @retval none
*/
void TCS34725::getRawData (uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
  if (!_tcs34725Initialised) begin();

  *c = read16(TCS34725_CDATAL);
  *r = read16(TCS34725_RDATAL);
  *g = read16(TCS34725_GDATAL);
  *b = read16(TCS34725_BDATAL);
  
  /* Set a delay for the integration time */
  switch (_tcs34725IntegrationTime)
  {
    case TCS34725_INTEGRATIONTIME_2_4MS:
      delay(3);
      break;
    case TCS34725_INTEGRATIONTIME_24MS:
      delay(24);
      break;
    case TCS34725_INTEGRATIONTIME_50MS:
      delay(50);
      break;
    case TCS34725_INTEGRATIONTIME_101MS:
      delay(101);
      break;
    case TCS34725_INTEGRATIONTIME_154MS:
      delay(154);
      break;
    case TCS34725_INTEGRATIONTIME_700MS:
      delay(700);
      break;
  }
}
