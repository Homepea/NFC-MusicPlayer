#ifndef __H_WS2812B_H__
#define __H_WS2812B_H__

#include "Common.h"

#define         NEOPIXEL_PIN                10
#define         NUMPIXELS                   19
#define         PIXEL_SHOW_BY_FLAG          0

#define        EM_LIGHT_TURN_OFF            0x00
#define        EM_LIGHT_RAINBOW             0x01
#define        EM_LIGHT_SHOW_HSV            0x02
#define        EM_LIGHT_SOLID               0x03
#define        EM_LIGHT_WATER               0x04
#define        EM_LIGHT_METEOT              0x05
#define        EM_LIGHT_COSTUM              0x06



void Ws2812b_SetBrightness(u8 ubLight);
void Ws2812b_Rainbow(void);
void Task_WS2812B(void *User_Task_WS2812B);
void WS2812B_Test();
void Ws2812b_Show(void);

#endif
