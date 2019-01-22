#include "sram.h"
#include "fsmc.h"

void SRAM_WriteBytes(uint32_t offset, uint8_t* src, uint32_t count)
{
    __IO uint16_t *addr = (__IO uint16_t*)(FSMC_SRAM_BASE_ADDR + offset);
    uint16_t *src16 = (uint16_t *)src;

    for (uint32_t i = 0; i < count >> 1; i++) {
        *(addr++) = *(src16++);
    }

    if (count & 1) {
        *addr = (uint16_t)(src[count - 1] << 4);
    }
}

void SRAM_ReadBytes(uint32_t offset, uint8_t* dst, uint32_t count)
{
    __IO uint16_t *addr = (__IO uint16_t*)(FSMC_SRAM_BASE_ADDR + offset);
    uint16_t *dst16 = (uint16_t *)dst;

    for (uint32_t i = 0; i < count >> 1; i++) {
        *(dst16++) = *(addr++);
    }

    if (count & 1) {
        dst[count - 1] = (uint8_t)(*addr >> 4);
    }
}


