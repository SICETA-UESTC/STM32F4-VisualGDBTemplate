#include "zlg7290.h"
#include "i2c.h"

#if ZLG7290_USE_KEYBUFFER
#include "ring_buffer.h"
#endif

extern I2C_HandleTypeDef hi2c1;

#if ZLG7290_USE_KEYBUFFER

static uint8_t s_key_buffer[16];
static RingBufferTypeDef s_key_fifo;

#else

static _Bool s_is_key_down;
static uint8_t s_key_val;

#endif


void ZLG7290_Init(void)
{
    I2C1_Init();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 4, 3);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

#if ZLG7290_USE_KEYBUFFER
    RingBuffer_Init(&s_key_fifo, s_key_buffer, sizeof(s_key_buffer));
#else
    s_is_key_down = 0;
    s_key_val = 0;
#endif
}

uint8_t ZLG7290_ReadKey(void)
{
#if ZLG7290_USE_KEYBUFFER
    if (RingBuffer_GetBytesCount(&s_key_fifo) == 0) {
        return 0;
    }

    uint8_t key_val;
    RingBuffer_ReadBytes(&s_key_buffer, &key_val, 1);

    return key_val;
#else
    if (!s_is_key_down) {
        return 0;
    }

    s_is_key_down = 0;

    uint8_t data = 0x01;
    HAL_I2C_Master_Transmit(&hi2c1, ZLG7290_WRITE_ADDR, &data, 1, 0xFF);
    HAL_I2C_Master_Receive(&hi2c1, ZLG7290_READ_ADDR, &data, 1, 0xFF);
    return data;
#endif
}

void EXTI0_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_0)) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);

#if ZLG7290_USE_KEYBUFFER
        uint8_t data = 0x01;
        HAL_I2C_Master_Transmit(&hi2c1, ZLG7290_WRITE_ADDR, &data, 1, 0xFF);
        HAL_I2C_Master_Receive(&hi2c1, ZLG7290_READ_ADDR, &data, 1, 0xFF);

        RingBuffer_WriteBytes(&s_key_fifo, &data, 1);
#else
        s_is_key_down = 1;
#endif
    }
}