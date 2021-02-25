#ifndef _SK6812_DRIVER_H
#define _SK6812_DRIVER_H
#include "Arduino.h"

//Cooperate with delay_ NS use, the approximate delay time
#define DELAY_NS_800000     4000
#define DELAY_NS_300        1
#define DELAY_NS_900        6
#define DELAY_NS_600        3

class SK6812{
  private:
    uint8_t pin;

  public:
    SK6812();
    ~SK6812(){}
    void SK6812_Delay_ns(uint32_t ns);
    void SK6812_Reset(void);
    void SK6812_Write_0(void);
    void SK6812_Write_1(void);
    void SK6812_Write_Byte(uint8_t byte);
    void SK6812_Set_RGBColor(uint8_t red, uint8_t green, uint8_t blue);
    void SK6812_Gradient_Color(uint32_t Time);
};

#endif /* _SK6812_DRIVER_H */
