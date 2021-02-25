#include <Ip5306.h>


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  IP5306 b;
  Serial.println(b.Ip5306_Check_Power());
  delay(2000);
  
}
