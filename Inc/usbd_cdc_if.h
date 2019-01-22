#pragma once
#include "usbd_cdc.h"

#define VCP_RX_BUFFER_SIZE 1024
#define VCP_TX_BUFFER_SIZE 1024

#define VCP_TIMEOUT     500

/* CDC I/O functions */
size_t VCP_Transmit(const uint8_t* buffer, size_t size);
size_t VCP_Recieve(uint8_t* buffer, size_t size);

/** CDC Interface callback. */
static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *length);
static int8_t CDC_Transmit_FS(uint8_t* buffer, uint32_t length);