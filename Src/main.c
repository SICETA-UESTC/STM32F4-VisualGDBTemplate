/* Includes ------------------------------------------------------------------*/

/* Libraries */
#include <stm32f4xx_hal.h>
#include <stdio.h>
#include <arm_math.h>

/* System Peripherals */
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "fsmc.h"
#include "fatfs.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

/* BSP */
#include "led.h"
#include "zlg7290.h"
#include "lcd.h"
#include "sram.h"

/* Applications */
#include "curve_chart.h"
#include "colors.h"

/* Private Marcos ------------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/

static void SystemClock_Config(void);

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Configure the system clock */
    SystemClock_Config();
    /* TIM7 is used for DelayUs() */
    TIM7_Init();

    /* Initialize USB Vitural COM Port */
    //USB_DEVICE_Init();

    /* Initialize system peripherals */
    DMA_Init();
    FSMC_Init();
    USART1_UART_Init();

    /* Initialize Fat FileSystem for SD Card */
    //FATFS_Init();

    /* Initialize all external hardware */
    //LED_Init();
    LCD_Init(LCD_ORIENTATION_270_DEGREE);
    LCD_DrawString("Lovely", 16, 16, 30, LAWNGREEN);
    LCD_FillRect(128, 8, 32, 16, DODGERBLUE);
    //LCD_SetFont(FONT_TYPE_SANS);

    //LCD_DrawBmpFromFile("0:/bmp/我就和你们一起玩.bmp", 0, 0);

    //LCD_FrameUpdate();

    /* Infinite loop */
    for (;;)
    {
        /* LED Blink */
        LED_Toggle(LED_0);

        HAL_Delay(1000);

        int val;
        if (scanf("%d", &val) != EOF) {
            printf("You've inptted a number: [%d]\n", val);
        }
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /**Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    while (1);
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
    LED_On(LED_0);
    while (1);
}
