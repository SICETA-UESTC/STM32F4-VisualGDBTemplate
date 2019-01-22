#include <stm32f4xx_hal.h>

#define ZLG7290_WRITE_ADDR      0x70
#define ZLG7290_READ_ADDR       0x71

#define ZLG7290_USE_KEYBUFFER   1

void ZLG7290_Init(void);
uint8_t ZLG7290_ReadKey(void);