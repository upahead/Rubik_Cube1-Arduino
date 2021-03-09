#include "sgp30.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  sgp30 b;
  b.SGP30_Init();
  b.SGP30_Get_Value();
  Serial.print("CO2: ");
  Serial.println(b.get_co2_val());
  Serial.print("TVOC: ");
  Serial.println(b.get_tvoc_val());
  delay(2000);
}
