#include <sk6812_driver.h>



void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  SK6812 b;                                           //创建SK6812的一个实例
  b.SK6812_Set_RGBColor(0xff, 0x12, 0x34);            //写入颜色数据
 // b.SK6812_Gradient_Color(10000);                     //显示渐变色
}
