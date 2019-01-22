#include <stdio.h>
#include "redirect_stdio.h"

#ifdef REDIRECT_IO_TO_UART
#include "usart.h"
extern UART_HandleTypeDef huart1;

#endif // REDIRECT_IO_TO_UART

#ifdef REDIRECT_IO_TO_USBD_CDC
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#endif // REDIRECT_IO_TO_UART

/* Redirect std ostream*/
ssize_t _write(int fd, const uint8_t *buffer, size_t length)
{
#ifdef REDIRECT_IO_TO_UART
    if (HAL_UART_Transmit(&huart1, buffer, length, 1000) == HAL_OK) {
        return length;
    }
    else {
        return -1;
    }
#endif

#ifdef REDIRECT_IO_TO_USBD_CDC
    length = VCP_Transmit(buffer, length);
    return (length > 0) ? length : -1;
#endif
}

/* Redirect std istream*/
ssize_t _read(int fd, uint8_t *buffer, size_t length)
{
#ifdef REDIRECT_IO_TO_UART
    length = USART1_FIFO_ReadBytes(buffer, length);
#endif

#ifdef REDIRECT_IO_TO_USBD_CDC
    length =  VCP_Recieve(buffer, length);
#endif
    return (length > 0) ? length : -1;
}