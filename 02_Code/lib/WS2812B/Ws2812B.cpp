#include "Ws2812B.h"
#include "Adafruit_NeoPixel.h"

Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
extern u16_t uwSongNum, uwSongBak; // 歌曲编号

void Ws2812b_SetBrightness(u8 ubLight)
{
    pixels.setBrightness(ubLight);
}

void Ws2812b_Show(void)
{
#if PIXEL_SHOW_BY_FLAG
    if (gstRouteInfo.boLedShowFlag)
    {
        pixels.show();
        gstRouteInfo.boLedShowFlag = FALSE;
    }
#else
    pixels.show();
#endif
}

// ***********************************************************************
//	@ 函数名称： WS2812B_Test
//	@ 函数功能： 流水灯的测试demo
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void WS2812B_Test(void)
{
    u32 uiColor[CONST_3] = {0xFF0000, 0x00FF00, 0x0000FF};
    static u8 ubIndex = 0;
    // Red
    for (u8 i = 0; i < pixels.numPixels(); i++)
    {
        pixels.setPixelColor(i, uiColor[ubIndex]);
        pixels.show();
        delay(100);
    }

    ubIndex++;
    if (ubIndex >= CONST_3)
    {
        ubIndex = 0;
    }
}

// ***********************************************************************
//	@ 函数名称： Ws2812b_Wheel(void)
//	@ 函数功能： 产生渐变的颜色值
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
u32 Ws2812b_Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// ***********************************************************************
//	@ 函数名称： Ws2812b_Rainbow(void)
//	@ 函数功能： 彩虹样式的特效
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Ws2812b_Rainbow(void)
{
    u16 i, j;

    for (j = 0; j < 256 * 5; j++)
    {
        if (EM_LIGHT_RAINBOW != ((uwSongNum % 6) + 1) // 不是当前模式则直接退出
            || !uwSongNum)                                  // 如果是不显示也直接退出
        {
            return;
        }
        for (i = 0; i < pixels.numPixels(); i++)
        {
            pixels.setPixelColor(i, Ws2812b_Wheel((((i << CONST_8) / pixels.numPixels()) + j) & 0xFF));
        }
#if PIXEL_SHOW_BY_FLAG
        gstRouteInfo.boLedShowFlag = TRUE;
#else
        pixels.show();
#endif
        delay(15);
    }
}

// ***********************************************************************
//	@ 函数名称： Ws2812b_ShowByHSV(void)
//	@ 函数功能： 变化的HSV显示
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Ws2812b_ShowByHSV(void)
{
    for (int i = 0; i < 65535; i += 100)
    {
        if (EM_LIGHT_SHOW_HSV != ((uwSongNum % 6) + 1) // 不是当前模式则直接退出
            || !uwSongNum)                                   // 如果是不显示也直接退出
        {
            return;
        }
        u32 rgbcolor = pixels.ColorHSV(i, 255, 255);
        pixels.fill(rgbcolor, 0, NUMPIXELS);
#if PIXEL_SHOW_BY_FLAG
        gstRouteInfo.boLedShowFlag = TRUE;
#else
        pixels.show();
#endif
        delay(50);
    }
}

// ***********************************************************************
//	@ 函数名称： WS2812B_SolidLight(void)
//	@ 函数功能： 彩虹色的流水灯
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void WS2812B_SolidLight(void)
{
    u32 uiColor[CONST_7] = {0xFF0000, 0xFFA000, 0xFFFF00, 0x008000, 0x00FFFF, 0x0000FF, 0x800080};
    static u8 ubIndex = 0;

    pixels.fill(uiColor[ubIndex], 0, NUMPIXELS);
#if PIXEL_SHOW_BY_FLAG
    gstRouteInfo.boLedShowFlag = TRUE;
#else
    pixels.show();
#endif

    for (u8 i = 0; i < 30; i++) // 用多次循环可以快速退出
    {
        if (EM_LIGHT_SOLID != ((uwSongNum % 6) + 1) // 不是当前模式则直接退出
            || !uwSongNum)                                // 如果是不显示也直接退出
        {
            return;
        }
        delay(10);
    }

    ubIndex++;
    if (ubIndex >= CONST_7)
    {
        ubIndex = 0;
    }
}

// ***********************************************************************
//	@ 函数名称： WS2812B_WaterLight
//	@ 函数功能： 流水灯
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void WS2812B_WaterLight(void)
{
    u32 uiColor[CONST_3] = {0xFF0000, 0x00FF00, 0x0000FF};
    static u8 ubIndex = 0;
    // Red
    for (u8 i = 0; i < pixels.numPixels(); i++)
    {
        pixels.setPixelColor(i, uiColor[ubIndex]);
#if PIXEL_SHOW_BY_FLAG
        gstRouteInfo.boLedShowFlag = TRUE;
#else
        pixels.show();
#endif

        for (u8 j = 0; j < CONST_10; j++) // 用多次循环可以快速退出
        {
            if (EM_LIGHT_WATER != ((uwSongNum % 6) + 1) // 不是当前模式则直接退出
                || !uwSongNum)                                // 如果是不显示也直接退出
            {
                return;
            }
            delay(10);
        }
    }

    ubIndex++;
    if (ubIndex >= CONST_3)
    {
        ubIndex = 0;
    }
}

// ***********************************************************************
//	@ 函数名称： WS2812B_Meteor
//	@ 函数功能： 流星的特效
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void WS2812B_Meteor(void)
{
    u32 uiColor[CONST_7] = {0xFF0000, 0xFFA000, 0xFFFF00, 0x008000, 0x00FFFF, 0x0000FF, 0x800080};
    static u8 ubIndex = 0, num = 15;
    u8 red = (uiColor[ubIndex] >> CONST_16) & 0xFF;
    u8 green = (uiColor[ubIndex] >> CONST_8) & 0xFF;
    u8 blue = (uiColor[ubIndex] >> CONST_0) & 0xFF;
    u8 max_color = red;

    if (green > max_color)
        max_color = green;
    if (blue > max_color)
        max_color = blue;
    uint8_t instance = (max_color - 200) / num;
    for (uint16_t i = 0; i < pixels.numPixels() + num; i++)
    {
        for (uint8_t j = 0; j < num; j++)
        {
            if (i - j >= 0 && i - j < pixels.numPixels())
            {
                int red_after = red - (instance * j);
                int green_after = green - (instance * j);
                int blue_after = blue - (instance * j);

                if (j >= 1)
                {
                    red_after -= 200;
                    green_after -= 200;
                    blue_after -= 200;
                }
                pixels.setPixelColor(i - j, pixels.Color(red_after >= 0 ? red_after : 0, green_after >= 0 ? green_after : 0, blue_after >= 0 ? blue_after : 0));
            }
        }
        if (i - num >= 0 && i - num < pixels.numPixels())
            pixels.setPixelColor(i - num, 0);

#if PIXEL_SHOW_BY_FLAG
        gstRouteInfo.boLedShowFlag = TRUE;
#else
        pixels.show();
#endif

        for (u8 i = 0; i < 10; i++) // 用多次循环可以快速退出
        {
            if (EM_LIGHT_METEOT != ((uwSongNum % 6) + 1) // 不是当前模式则直接退出
                || !uwSongNum)                                 // 如果是不显示也直接退出
            {
                return;
            }
            delay(5);
        }
    }

    if (green > max_color)
        max_color = green;
    if (blue > max_color)
        max_color = blue;
    instance = (max_color - 200) / num;
    for (int i = pixels.numPixels() - 1; i >= -num; i--)
    {
        for (uint8_t j = 0; j < num; j++)
        {
            if (i + j >= 0 && i + j < pixels.numPixels())
            {
                int red_after = red - instance * j;
                int green_after = green - instance * j;
                int blue_after = blue - instance * j;
                if (j >= 1)
                {
                    red_after -= 200;
                    green_after -= 200;
                    blue_after -= 200;
                }
                pixels.setPixelColor(i + j, pixels.Color(red_after >= 0 ? red_after : 0, green_after >= 0 ? green_after : 0, blue_after >= 0 ? blue_after : 0));
            }
        }
        if (i + num >= 0 && i + num < pixels.numPixels())
            pixels.setPixelColor(i + num, 0);

#if PIXEL_SHOW_BY_FLAG
        gstRouteInfo.boLedShowFlag = TRUE;
#else
        pixels.show();
#endif
        for (u8 i = 0; i < 10; i++) // 用多次循环可以快速退出
        {
            if (EM_LIGHT_METEOT != ((uwSongNum % 6) + 1) // 不是当前模式则直接退出
                || !uwSongNum)                                 // 如果是不显示也直接退出
            {
                return;
            }
            delay(5);
        }
    }

    ubIndex++;
    if (ubIndex >= CONST_7)
    {
        ubIndex = 0;
    }
}

// ***********************************************************************
//	@ 函数名称： WS2812B_ShowUser
//	@ 函数功能： 用户自定义的颜色
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void WS2812B_ShowUser(void)
{
    static u16 uwHue = 0;
    uwHue += 500;
    if (uwHue >= 65535)
    {
        uwHue = 0;
    }
    u32 rgbcolor = pixels.ColorHSV(uwHue, 255, 255);
    pixels.fill(rgbcolor, 0, NUMPIXELS);

#if PIXEL_SHOW_BY_FLAG
    gstRouteInfo.boLedShowFlag = TRUE;
#else
    pixels.show();
#endif
    delay(10);
}

// ***********************************************************************
//	@ 函数名称： WS2812B_TurnOff
//	@ 函数功能： 暂时关闭显示
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void WS2812B_TurnOff(void)
{
    pixels.fill(pixels.Color(0, 0, 0), 0, 6);
#if PIXEL_SHOW_BY_FLAG
    gstRouteInfo.boLedShowFlag = TRUE;
#else
    pixels.show();
#endif
    delay(10);
}

// ***********************************************************************
//	@ 函数名称： Task_WS2812B(void *User_Task_BLE)
//	@ 函数功能： Nixie显示的线程
//	@ 函数输入： *User_Task_BLE(固定的形式)
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Task_WS2812B(void *User_Task_WS2812B)
{
    Ws2812b_SetBrightness(255);
    while (1)
    {
        if (uwSongBak != uwSongNum)
        {
            pixels.fill(0, 0, NUMPIXELS);
        }
        uwSongBak = uwSongNum; // 备份当前的歌曲编号
        if (0 == uwSongNum)
        {
            WS2812B_TurnOff(); // 关闭显示
        }
        else
        {
            switch ((uwSongNum % 6) + 1)
            {
            case EM_LIGHT_RAINBOW:
                Ws2812b_Rainbow(); // 彩虹特效
                break;

            case EM_LIGHT_SHOW_HSV:
                Ws2812b_ShowByHSV(); // HSV 变化
                break;

            case EM_LIGHT_SOLID:
                WS2812B_SolidLight(); // 流水灯1
                break;

            case EM_LIGHT_WATER:
                WS2812B_WaterLight(); // 流水灯2
                break;

            case EM_LIGHT_METEOT:
                WS2812B_Meteor(); // 流星雨
                break;

            case EM_LIGHT_COSTUM:
                WS2812B_ShowUser(); // 用户自定义的颜色
                break;

            default:
                break;
            }
        }
        delay(20);
    }
}
