/* Includes ------------------------------------------------------------------*/
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

  /**
    * Init USB device Library, add supported class and start the library
    * @retval None
    */
void USB_DEVICE_Init(void)
{
    /* Init Device Library, add supported class and start the library. */
    USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);

    USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);

    USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);

    USBD_Start(&hUsbDeviceFS);
}

/**
* @brief This function handles USB On The Go FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}