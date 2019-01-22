#include "ov2640.h"
#include "dcmi.h"
#include "i2c.h"

extern I2C_HandleTypeDef hi2c1;
OV2640_HandleTypeDef hcam;

void OV2640_Init(void)
{
    /**OV2640 Control I/O
    PA4     ------> OV_RESET
    PA5     ------> OV_PWDN
    */
    /* Init OV2640 Control GPIO */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = OV2640_RST_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(OV2640_RST_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = OV2640_PWDN_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(OV2640_PWDN_GPIO_PORT, &GPIO_InitStruct);

    OV2640_Reset();

    I2C1_Init();
    ReadID(&hcam);

    if (hcam.ProductID != OV2640_PRODUCT_ID) {
        Error_Handler();
    }

    
    DCMI_Init();
}

void OV2640_Reset(void)
{
    /* Power on */
    HAL_GPIO_WritePin(OV2640_PWDN_GPIO_PORT, OV2640_PWDN_GPIO_PIN, RESET);
    /* Reset */
    HAL_GPIO_WritePin(OV2640_RST_GPIO_PORT, OV2640_RST_GPIO_PIN, RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(OV2640_RST_GPIO_PORT, OV2640_RST_GPIO_PIN, SET);
}

void OV2640_Stop(void)
{
    /* Power off */
    HAL_GPIO_WritePin(OV2640_PWDN_GPIO_PORT, OV2640_PWDN_GPIO_PIN, SET);
}

static void ReadID(OV2640_HandleTypeDef *hcam)
{
    /* Register select */
    SCCB_WriteReg(OV2640_DSP_RA_DLMT, 0x01);

    hcam->ManufacturerID = (SCCB_ReadReg(OV2640_SENSOR_MIDH) << 8) | SCCB_ReadReg(OV2640_SENSOR_MIDL);
    hcam->ProductID = (SCCB_ReadReg(OV2640_SENSOR_PIDH) << 8) | SCCB_ReadReg(OV2640_SENSOR_PIDL);

#ifdef DEBUG
    printf("ov2640 MID: [0x%.4X]\n", hcam->ManufacturerID);
    printf("ov2640 PID: [0x%.4X]\n", hcam->ProductID);
#endif // DEBUG
}

static inline void SCCB_WriteReg(uint16_t addr, uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, OV2640_I2C_WRITE_ADDRESS, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
}

static inline uint8_t SCCB_ReadReg(uint16_t addr)
{
    uint8_t data;
    HAL_I2C_Mem_Read(&hi2c1, OV2640_I2C_READ_ADDRESS, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
    return data;
}