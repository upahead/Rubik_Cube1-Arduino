#include <Wire.h>
#include "bh1750fvi_driver.h"



void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  BH1750FVI b;                                                      
  float temp = b.get_Data();
  Serial.println(temp);                  
  delay(1000);
}
