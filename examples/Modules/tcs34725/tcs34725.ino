#include "tcs34725_driver.h"
#include <Wire.h>





void setup(void) {
  Serial.begin(115200);
  Wire.begin();
}

void loop(void) {
  uint16_t r, g, b, c;
  TCS34725 tcs = TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);      //创建一个TCS34725实例，并初始化集成时间和增益
  tcs.begin();                                                                                      //初始化TCS34725
  tcs.getRawData(&r, &g, &b, &c);                                                                   //得到红，绿，蓝和透明通道的数据
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");                                     //输出数据
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
}
