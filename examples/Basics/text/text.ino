#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();
void setup() {
  // put your setup code here, to run once:
   tft.init();
   tft.setRotation(3);
   tft.fillScreen(TFT_WHITE);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i=100;
  String c="c:";
  char a[4] = {'y','u',':'};
  tft.setTextColor(TFT_BLACK,TFT_WHITE);
  tft.setTextFont(4);
  tft.drawString(c,0,0);
  tft.drawString(c,20,20,2);
  tft.drawString(a,30,30);
  tft.drawString(a,40,0,2);
  tft.drawCentreString(c,60,0,2);
  tft.drawCentreString(a,70,70,2);
  tft.drawRightString(c,0,70,2);
  tft.drawRightString("Font",100,100,2);
  tft.drawNumber(30,100,120);
  tft.drawNumber(30,140,120,2);
  tft.drawFloat(29.3,3,120,30);
  tft.drawFloat(29.4,4,120,0);
  tft.setCursor(120,60);
  tft.println("loss");
  tft.println(i);
}
