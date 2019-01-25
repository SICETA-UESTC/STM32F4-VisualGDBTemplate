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
#define BLACK           0x0000  //��ɫ
#define DARKGRAY        0x528A  //���ɫ
#define GRAY            0xB5B6  //��ɫ
#define LIGHTGRAY       0xD69A  //ǳ��ɫ
#define WHITE           0xFFFF  //��ɫ
#define RED             0xF800  //��ɫ
#define GREEN           0x07E0  //��ɫ
#define LAWNGREEN       0x7FE0  //��ƺ��
#define BLUE            0x001F  //��ɫ
#define DODGERBLUE      0x1C9F  //����
#define STEELBLUE       0x4416  //����
#define NAVYBLUE        0x0010  //������
#define PERRY           0x2595  //Ѽ����̩��!
#define YELLOW          0xFFE0  //��ɫ
#define CYAN            0x7FFF  //��ɫ
#define BROWN           0x8A24  //��ɫ
#define MAGENTA         0xF81F  //Ʒ��ɫ
#define PURPLE          0xA11E  //��ɫ
#define GOLD            0xFEA0  //��ɫ
#define ORANGE          0xFC60  //��ɫ
#define ORANGERED       0xFA20  //�Ⱥ�ɫ
#define SKYBLUE         0x867F  //����ɫ
#define AZURE           0xF7FF  //ε��ɫ
#define ALICEBLUE       0xF7DF  //����˿��
#define SALMON          0xFC0E  //�������
#define OLIVE           0x9E66  //�����

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

