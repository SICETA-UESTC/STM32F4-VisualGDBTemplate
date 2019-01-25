/**
  ******************************************************************************
  * @file       colors.h
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.18
  * @brief      Color presets and conversions
  ******************************************************************************
  */

/* Preprocessor Directives ---------------------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Public Marcos -------------------------------------------------------------*/

#define RGB565          0
#define RGB888          1
#define RGBA8888        2
#define ALPHA8          3

/* RGB565 Color presets */
#define BLACK           0x0000  //黑色
#define DARKGRAY        0x528A  //深灰色
#define GRAY            0xB5B6  //灰色
#define LIGHTGRAY       0xD69A  //浅灰色
#define WHITE           0xFFFF  //白色
#define RED             0xF800  //红色
#define GREEN           0x07E0  //绿色
#define LAWNGREEN       0x7FE0  //草坪绿
#define BLUE            0x001F  //蓝色
#define DODGERBLUE      0x1C9F  //宝蓝
#define STEELBLUE       0x4416  //铁蓝
#define NAVYBLUE        0x0010  //海军蓝
#define PERRY           0x2595  //鸭嘴兽泰瑞!
#define YELLOW          0xFFE0  //黄色
#define CYAN            0x7FFF  //青色
#define BROWN           0x8A24  //棕色
#define MAGENTA         0xF81F  //品红色
#define PURPLE          0xA11E  //紫色
#define GOLD            0xFEA0  //金色
#define ORANGE          0xFC60  //橙色
#define ORANGERED       0xFA20  //橙红色
#define SKYBLUE         0x867F  //天蓝色
#define AZURE           0xF7FF  //蔚蓝色
#define ALICEBLUE       0xF7DF  //爱丽丝蓝
#define SALMON          0xFC0E  //三文鱼红
#define OLIVE           0x9E66  //橄榄绿

static inline uint16_t pack_rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t color = 0x00;
    color |= (r & 0xF8) << 8;
    color |= (g & 0xFC) << 3;
    color |= (b & 0xF8) >> 3;
    return color;
}

static inline uint16_t rgb888_2_rgb565(uint32_t rgb_888)
{
    return pack_rgb565((rgb_888 >> 16) & 0xFF,
                       (rgb_888 >> 8) & 0xFF,
                       (rgb_888) & 0xFF);
}

