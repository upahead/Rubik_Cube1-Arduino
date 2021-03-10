#include <TFT_eSPI.h>
TFT_eSPI tft;
void setup() {
  // put your setup code here, to run once:
  tft.init();
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  // put your main code here, to run repeatedly:
  tft.drawCircle(160,120,50,TFT_RED);
  delay(1000);
  tft.fillCircle(120,160,50,TFT_BLUE); 
  //tft.fillScreen(TFT_WHITE);
  tft.fillEllipse(120,160,25,40,TFT_GOLD);
  delay(1000);
  tft.drawEllipse(160,120,25,40,TFT_PINK);
  //tft.fillScreen(TFT_BLACK);
  tft.drawRect(120,160,20,40,TFT_YELLOW);
  delay(1000);
  tft.drawRoundRect(160,120,20,40,6,TFT_OLIVE);
  //tft.fillScreen(TFT_BLACK);
  tft.fillRoundRect(120,160,20,40,6,TFT_YELLOW);
  tft.fillRect(160,120,20,40,TFT_VIOLET);
  tft.drawTriangle(200,0,200,60,150,0,TFT_SILVER);
  tft.fillTriangle(200,319,200,290,150,300,TFT_BROWN); 
  tft.drawFastVLine(0,0,40,TFT_WHITE);
  tft.drawFastHLine(0,120,40,TFT_SKYBLUE);
  delay(3000);
  tft.fillScreen(TFT_BLACK);
}
