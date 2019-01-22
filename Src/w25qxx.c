#include "w25qxx.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi1;

uint16_t W25QXX_ID;

void W25QXX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	DISABLE_W25QXX_CS;
	SPI1_Init();

	ReadID();
//	printf("0x%X\n", W25QXX_ID);
}

void W25QXX_Write(uint32_t Addr, uint8_t* pData, uint32_t BytesToWrite)
{
	uint8_t Offset = Addr % PAGE_SIZE;

	if (Offset != 0)
	{
		uint8_t PageRemainBytes = PAGE_SIZE - Offset;

		if (BytesToWrite <= PageRemainBytes)
		{
			W25QXX_PageWrite(Addr, pData, BytesToWrite);
			return;
		}

		W25QXX_PageWrite(Addr, pData, PageRemainBytes);
		Addr += PageRemainBytes;
		pData += PageRemainBytes;
		BytesToWrite -= PageRemainBytes;
	}

	uint8_t PageCount = BytesToWrite / PAGE_SIZE;
	uint8_t SingleCount = BytesToWrite % PAGE_SIZE;

	while (PageCount--)
	{
		W25QXX_PageWrite(Addr, pData, PAGE_SIZE);
		Addr += PAGE_SIZE;
		pData += PAGE_SIZE;
	}
	W25QXX_PageWrite(Addr, pData, SingleCount);

}

void W25QXX_Read(uint32_t SectorAddr, uint8_t* pBuffer, uint32_t BytesToRead)
{
	ENABLE_W25QXX_CS;
	SPI_ReadWriteByte(&hspi1, 0x03);
	SPI_ReadWriteByte(&hspi1, SectorAddr >> 16);
	SPI_ReadWriteByte(&hspi1, SectorAddr >> 8);
	SPI_ReadWriteByte(&hspi1, SectorAddr);

	//HAL_SPI_Receive_DMA(&hspi1, pBuffer, Size * Count);

	while (BytesToRead--)
	{
		*(pBuffer++) = SPI_ReadWriteByte(&hspi1, DUMMY);
	}

	DISABLE_W25QXX_CS;
}

void W25QXX_PageWrite(uint32_t SectorAddr, uint8_t* pData, uint16_t Size)
{
	WriteEnable();
	ENABLE_W25QXX_CS;

	SPI_ReadWriteByte(&hspi1, 0x02);
	SPI_ReadWriteByte(&hspi1, SectorAddr >> 16);
	SPI_ReadWriteByte(&hspi1, SectorAddr >> 8);
	SPI_ReadWriteByte(&hspi1, SectorAddr);

	HAL_SPI_Transmit(&hspi1, pData, Size, 1000);

	DISABLE_W25QXX_CS;
	WaitForWriteEnd();
}

static void ReadID(void)
{
	ENABLE_W25QXX_CS;

	SPI_ReadWriteByte(&hspi1, 0x90);
	SPI_ReadWriteByte(&hspi1, 0x00);
	SPI_ReadWriteByte(&hspi1, 0x00);
	SPI_ReadWriteByte(&hspi1, 0x00);

	W25QXX_ID |= SPI_ReadWriteByte(&hspi1, DUMMY) << 8;
	W25QXX_ID |= SPI_ReadWriteByte(&hspi1, DUMMY);

	DISABLE_W25QXX_CS;
}

void W25QXX_SectorErase(uint32_t SectorAddr)
{
	WriteEnable();
	WaitForWriteEnd();

	ENABLE_W25QXX_CS;
	SPI_ReadWriteByte(&hspi1, 0x20);
	SPI_ReadWriteByte(&hspi1, SectorAddr >> 16);
	SPI_ReadWriteByte(&hspi1, SectorAddr >> 8);
	SPI_ReadWriteByte(&hspi1, SectorAddr);

	DISABLE_W25QXX_CS;
	WaitForWriteEnd();
}

void W25QXX_BlockErase(uint32_t SectorAddr)
{
	WriteEnable();
	WaitForWriteEnd();

	ENABLE_W25QXX_CS;
	SPI_ReadWriteByte(&hspi1, 0xD8);
	SPI_ReadWriteByte(&hspi1, SectorAddr >> 16);
	SPI_ReadWriteByte(&hspi1, SectorAddr >> 8);
	SPI_ReadWriteByte(&hspi1, SectorAddr);

	DISABLE_W25QXX_CS;
	WaitForWriteEnd();
}

static void PowerDown(void)
{
	ENABLE_W25QXX_CS;
	SPI_ReadWriteByte(&hspi1, 0xB9);
	DISABLE_W25QXX_CS;
	DelayUs(3);
}

static void WakeUp(void)
{
	ENABLE_W25QXX_CS;
	SPI_ReadWriteByte(&hspi1, 0xAB);
	DISABLE_W25QXX_CS;
	DelayUs(3);
}

static void WaitForWriteEnd(void)
{
	ENABLE_W25QXX_CS;
	SPI_ReadWriteByte(&hspi1, 0x05);
	while (SPI_ReadWriteByte(&hspi1, DUMMY) & 0x01);
	DISABLE_W25QXX_CS;
}

static void WriteEnable(void)
{
	ENABLE_W25QXX_CS;
	SPI_ReadWriteByte(&hspi1, 0x06);
	DISABLE_W25QXX_CS;
}