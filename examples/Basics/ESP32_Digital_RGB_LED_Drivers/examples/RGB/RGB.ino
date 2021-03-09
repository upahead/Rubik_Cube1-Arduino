#include "esp32_digital_led_lib.h"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
void gpioSetup(int gpioNum, int gpioMode, int gpioVal) {
  pinMode(gpioNum, gpioMode);
  digitalWrite(gpioNum,gpioVal);
}

strand_t strand = {.rmtChannel = 0, .gpioNum = 5, .ledType = LED_SK6812_V1, .brightLimit = 128, .numPixels =1};
strand_t * STRANDS [] = { &strand };
int STRANDCNT = COUNT_OF(STRANDS); 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Initializing...");

  digitalLeds_initDriver();

  // Init unused outputs low to reduce noise

  gpioSetup(strand.gpioNum, OUTPUT, LOW);
  int rc = digitalLeds_addStrands(STRANDS, STRANDCNT);
  if (rc) {
    Serial.print("Init rc = ");
    Serial.println(rc);
  }

  if (digitalLeds_initDriver()) {
    Serial.println("Init FAILURE: halting");
    while (true) {};
  }
  off();
  digitalLeds_resetPixels(STRANDS, STRANDCNT);
}

void RGBled_Show_Gradient(uint32_t timeout)
{
  int i, j, k=255;
  for(i=0;i<255;i++)
  {
    k--;
    for(uint16_t g=0;g<strand.numPixels;g++)
      strand.pixels[g]=pixelFromRGBW(i, j, k, 0);
     digitalLeds_drawPixels(STRANDS, STRANDCNT);
     delay(timeout);
  }
  for(j=0;j<255;j++)
  {
    i--;
    for(uint16_t g=0;g<strand.numPixels;g++)
      strand.pixels[g]=pixelFromRGBW(i, j, k, 0);
     digitalLeds_drawPixels(STRANDS, STRANDCNT);
      delay(timeout);
    }
   for(k=0;k<255;k++)
   {
    j--;
    for(uint16_t g=0;g<strand.numPixels;g++)
      strand.pixels[g]=pixelFromRGBW(i, j, k, 0);
     digitalLeds_drawPixels(STRANDS, STRANDCNT);
      delay(timeout);
    }
}

void RGBled_Show_Fill_In(uint8_t r, uint8_t g, uint8_t b, uint32_t delay_time, uint32_t timeout_ms)
{
  pixelColor_t color1;
  color1.r=r;
  color1.g=g;
  color1.b=b;
  color1.w=0;
  unsigned long start_ms = millis();
  for(uint16_t i=0;i<strand.numPixels;i++)
      strand.pixels[i]=pixelFromRGBW(color1.r, color1.g, color1.b,0);
   digitalLeds_drawPixels(STRANDS, STRANDCNT);
   delay(delay_time);
  }
void off()
{
  for(uint16_t g=0;g<strand.numPixels;g++)
      strand.pixels[g]=pixelFromRGBW(0, 0, 0, 0);
  digitalLeds_drawPixels(STRANDS, STRANDCNT);
  digitalLeds_resetPixels(STRANDS, STRANDCNT);
  }
void loop() {
  // put your main code here, to run repeatedly:
  delay(1500);
  RGBled_Show_Gradient(1);
  delay(1500);
 RGBled_Show_Fill_In(0x00,0x00,0xff,0,2000);
  digitalLeds_resetPixels(STRANDS, STRANDCNT);
}
