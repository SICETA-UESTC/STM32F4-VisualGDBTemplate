#include "number_input.h"
#include "zlg7290.h"
#include "lcd.h"
#include "colors.h"

#include <string.h>

static uint8_t s_offset;
static _Bool s_is_dot_inputted;
static uint8_t s_input_buffer[16];

_Bool GetInputFloat(float *inputVal)
{
    s_is_dot_inputted = 0;
    if (GetUserInput())
    {
        sscanf(s_input_buffer, "%f", inputVal);
        return 1;
    }
    return 0;
}

_Bool GetInputInt(uint32_t *inputVal)
{
    s_is_dot_inputted = 1; //获取整数不允许输入小数点

    if (GetUserInput())
    {
        sscanf(s_input_buffer, "%d", inputVal);
        return 1;
    }
    return 0;
}

static _Bool GetUserInput(void)
{
    s_offset = 0;
    LCD_DrawString("请输入数值:", 32, NUMINPUT_X, NUMINPUT_Y, RED);

    for (;;)
    {
        switch (ZLG7290_ReadKey())
        {
            case 2: NumberInput(1); break;
            case 3: NumberInput(2); break;
            case 4: NumberInput(3); break;
            case 10: NumberInput(4); break;
            case 11: NumberInput(5); break;
            case 12: NumberInput(6); break;
            case 18: NumberInput(7); break;
            case 19: NumberInput(8); break;
            case 20: NumberInput(9); break;
            case 27: NumberInput(0); break;
            case 28: DotInput(); break;

            case 5: BackSpace(); break;	//退格

            case 13:
                LCD_FillRect(NUMINPUT_X, NUMINPUT_Y, 180 + (s_offset + 1) * 16, 32, BLACK);  //取消输入
                return 0;

            case 21:
                s_input_buffer[s_offset] = '\0';
                LCD_FillRect(NUMINPUT_X, NUMINPUT_Y, 180 + (s_offset + 1) * 16, 32, BLACK); //输入结束
                return 1;

                break;
        };
        HAL_Delay(33);
    }
}

static inline void DotInput(void)
{
    if (s_is_dot_inputted) {
        return;
    }

    LCD_FillRect(DIGITBOX_X + s_offset * 16, DIGITBOX_Y, 16, 32, BLACK);
    LCD_DrawCharASCII('.', 32, DIGITBOX_X + s_offset * 16, DIGITBOX_Y, WHITE);
    s_input_buffer[s_offset++] = '.';
    LCD_DrawCharASCII('|', 32, DIGITBOX_X + s_offset * 16, DIGITBOX_Y, GRAY);

    s_is_dot_inputted = 1;
}

static inline void NumberInput(uint8_t num)
{
    if (s_offset >= 10) {
        return;
    }

    LCD_FillRect(DIGITBOX_X + s_offset * 16, DIGITBOX_Y, 16, 32, BLACK);
    LCD_DrawNumber(DIGITBOX_X + s_offset * 16, DIGITBOX_Y, 32, num, WHITE);
    s_input_buffer[s_offset++] = num + '0';
    LCD_DrawCharASCII('|', 32, DIGITBOX_X + s_offset * 16, DIGITBOX_Y, GRAY);
}

static inline void BackSpace(void)
{
    if (s_input_buffer[s_offset] == '.') {
        s_is_dot_inputted = 0;
    }
    if (s_offset == 0) {
        return;
    }

    LCD_FillRect(DIGITBOX_X + s_offset * 16, DIGITBOX_Y, 16, 32, BLACK);
    s_offset--;
    LCD_FillRect(DIGITBOX_X + s_offset * 16, DIGITBOX_Y, 32, 32, BLACK);
    LCD_DrawCharASCII('|', 32, DIGITBOX_X + s_offset * 16, DIGITBOX_Y, GRAY);
}