#include "usbd_cdc_if.h"
#include "ring_buffer.h"
#include <memory.h>

static uint8_t s_vcp_rx_packet_buffer[USB_FS_MAX_PACKET_SIZE];

static uint8_t s_vcp_rx_buffer[VCP_RX_BUFFER_SIZE];
static uint8_t s_vcp_tx_buffer[VCP_TX_BUFFER_SIZE];

RingBufferTypeDef s_vcp_rx_fifo;
//VCP_RingBufferTypeDef s_txbuffer;

USBD_CDC_LineCodingTypeDef s_linecoding = {
    .bitrate = 115200,      //baud rate
    .format = 0x00,         //stop bits-1
    .paritytype = 0x00,     //no parity
    .datatype = 0x08,       //8-bits data
};

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
    CDC_Init_FS,
    CDC_DeInit_FS,
    CDC_Control_FS,
    CDC_Receive_FS
};

/* Public functions ---------------------------------------------------------*/
size_t VCP_Transmit(const uint8_t* buffer, size_t length)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;

    uint32_t tick_start = HAL_GetTick();

    while (hcdc->TxState != 0) {
        if (HAL_GetTick() - tick_start > VCP_TIMEOUT) {
            return 0;
        }
    }

    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, buffer, length);
    USBD_CDC_TransmitPacket(&hUsbDeviceFS);

    return length;
}

size_t VCP_Recieve(uint8_t* buffer, size_t length)
{
    return RingBuffer_ReadBytes(&s_vcp_rx_fifo, buffer, length);
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
    /* Set Application Buffers */
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, s_vcp_rx_packet_buffer);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);

    RingBuffer_Init(&s_vcp_rx_fifo, s_vcp_rx_buffer, VCP_RX_BUFFER_SIZE);
    return (USBD_OK);
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
    return (USBD_OK);
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
    switch (cmd)
    {
        case CDC_SEND_ENCAPSULATED_COMMAND:

            break;

        case CDC_GET_ENCAPSULATED_RESPONSE:

            break;

        case CDC_SET_COMM_FEATURE:

            break;

        case CDC_GET_COMM_FEATURE:

            break;

        case CDC_CLEAR_COMM_FEATURE:

            break;

            /*******************************************************************************/
            /* Line Coding Structure                                                       */
            /*-----------------------------------------------------------------------------*/
            /* Offset | Field       | Size | Value  | Description                          */
            /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
            /* 4      | bCharFormat |   1  | Number | Stop bits                            */
            /*                                        0 - 1 Stop bit                       */
            /*                                        1 - 1.5 Stop bits                    */
            /*                                        2 - 2 Stop bits                      */
            /* 5      | bParityType |  1   | Number | Parity                               */
            /*                                        0 - None                             */
            /*                                        1 - Odd                              */
            /*                                        2 - Even                             */
            /*                                        3 - Mark                             */
            /*                                        4 - Space                            */
            /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
            /*******************************************************************************/
        case CDC_SET_LINE_CODING:
            memcpy(&s_linecoding, pbuf, sizeof(USBD_CDC_LineCodingTypeDef));
            break;

        case CDC_GET_LINE_CODING:
            memcpy(pbuf, &s_linecoding, sizeof(USBD_CDC_LineCodingTypeDef));
            break;

        case CDC_SET_CONTROL_LINE_STATE:

            break;

        case CDC_SEND_BREAK:

            break;

        default:
            break;
    }

    return (USBD_OK);
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* buffer, uint32_t *length)
{
    RingBuffer_WriteBytes(&s_vcp_rx_fifo, buffer, *length);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    return (USBD_OK);
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
static int8_t CDC_Transmit_FS(uint8_t* buffer, uint32_t length)
{
    uint8_t result = USBD_OK;

    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
    if (hcdc->TxState != 0) {
        return USBD_BUSY;
    }
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, buffer, length);
    result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    return result;
}