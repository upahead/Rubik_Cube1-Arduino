#include <Wire.h>
#include "MAX30102.h"
#define TIMETOBOOT 3000 // wait for this time(msec) to output data
MAX30102 particleSensor;

double avered = 0; 
double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;
double ESpO2 = 95.0;//initial value of estimated SpO2
double SpO2 = 0; //raw SpO2 before low pass filtered
char choice;

void setup()
{
  Serial.begin(115200);
  Serial.println("Please input your choice: 1(SPO2), 2(hearate)");
  while(Serial.available()<=0);
  choice = Serial.read();
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  
  Serial.println("Place your index finger on the sensor with steady pressure.");
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
}
 

void loop()
{
  float beatsPerMinute;
  int beatAvg;
 
 if(choice == '2')
 { //int32_t red = particleSensor.getIR();
  int32_t irValue = particleSensor.getRed();
  
  particleSensor.get_avgBPM(&irValue,&beatAvg,&beatsPerMinute);
  
  
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  Serial.println();
  }
  else if(choice == '1')
  {
      int32_t red = particleSensor.getIR();
      int32_t irValue = particleSensor.getRed();
      particleSensor.caldata(&avered,&aveir,&sumirrms,&sumredrms,red,irValue);
      particleSensor.get_ESPO2(irValue,&avered,&aveir,&sumirrms,&sumredrms,&SpO2,&ESpO2);
      Serial.print("Red=");
      Serial.print(red);
      Serial.print(", SpO2=");
      Serial.print(SpO2);
      Serial.print(", ESpO2=");
      Serial.print(ESpO2);
      Serial.println();
   }

}
