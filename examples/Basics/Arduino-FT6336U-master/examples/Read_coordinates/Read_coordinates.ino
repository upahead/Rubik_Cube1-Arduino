#include "FT6336U.h"

#define I2C_SDA 21
#define I2C_SCL 22
#define RST_N_PIN 38
#define INT_N_PIN 37

FT6336U ft6336u(I2C_SDA, I2C_SCL, RST_N_PIN, INT_N_PIN); 

void setup() {
    Serial.begin(115200); 
    ft6336u.begin(); 
}

void loop() {
  delay(100);
    if(digitalRead(INT_N_PIN) != -1) {
        Serial.print("FT6336U TD Status: "); 
        Serial.println(ft6336u.read_td_status());  
        Serial.print("FT6336U Touch Event/ID 1: ("); 
        Serial.print(ft6336u.read_touch1_event()); Serial.print(" / "); Serial.print(ft6336u.read_touch1_id()); Serial.println(")"); 
        Serial.print("FT6336U Touch Position 1: ("); 
        Serial.print(ft6336u.read_touch1_x()); Serial.print(" , "); Serial.print(ft6336u.read_touch1_y()); Serial.println(")"); 
    }
  delay(100);
}
