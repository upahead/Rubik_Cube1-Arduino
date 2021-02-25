#include "sk6812_driver.h"

/**
  * @brief  Initialize sk6812gpio
  * @param  void
  * @retval none
  */
SK6812::SK6812()
{
  pin=5;
	pinMode(pin,OUTPUT);
}

/**
  * @brief  delayed(ns)
  * @param  Delay time
  * @retval none
  */
void SK6812::SK6812_Delay_ns(uint32_t ns)
{
	volatile uint32_t i=0;
	for(i=0; i<ns; i++);
}

/**
  * @brief  Sk6812 reset
  * @param  void
  * @retval none
  */
void SK6812::SK6812_Reset()
{
	digitalWrite(pin,LOW);
	SK6812_Delay_ns(DELAY_NS_800000);
}

/**
  * @brief  write 0
  * @param  void
  * @retval none
  */
void SK6812::SK6812_Write_0()
{
	digitalWrite(pin,HIGH);
	SK6812_Delay_ns(DELAY_NS_300);
	digitalWrite(pin,LOW);
	SK6812_Delay_ns(DELAY_NS_900);
}

/**
  * @brief  write 1
  * @param  void
  * @retval none
  */
void SK6812::SK6812_Write_1()
{
	digitalWrite(pin,HIGH);
	SK6812_Delay_ns(DELAY_NS_600);
	digitalWrite(pin,LOW);
	SK6812_Delay_ns(DELAY_NS_600);
}

/**
  * @brief  Write a data
  * @param  byte : data
  * @retval none
  */
void SK6812::SK6812_Write_Byte(uint8_t byte)
{
	uint8_t i;
    for(i=0;i<8;i++){
        if(byte&0x80){
		    SK6812_Write_1();
	    }else{
		    SK6812_Write_0();
	    }
		byte <<= 1;
    }
}

/**
  * @brief  Write a color data
  * @param  red : Red Data
  * @param  green : Green Data
  * @param  blue : Blue Data
  * @retval none
  */
void SK6812::SK6812_Set_RGBColor(uint8_t red, uint8_t green, uint8_t blue)
{
    SK6812_Reset();
	SK6812_Write_Byte(green);
    SK6812_Write_Byte(red);
	  
	  SK6812_Write_Byte(blue);
}

/**
  * @brief  Gradient color
  * @param  time ： Transition time(Recommended 1000-10000)
  * @retval void
  */
void SK6812::SK6812_Gradient_Color(uint32_t Time)
{
    uint8_t i=0;
    uint8_t j=0;
    uint8_t k=255;
    for(i=0; i<255; i++){
        k--;
        SK6812_Set_RGBColor(i, j, k);
        SK6812_Delay_ns(Time);
    }
    for(j=0; j<255; j++){
        i--;
        SK6812_Set_RGBColor(i, j, k);
        SK6812_Delay_ns(Time);
    }
    for(k=0; k<255; k++){
        j--;
        SK6812_Set_RGBColor(i, j, k);
        SK6812_Delay_ns(Time);
    }
}
