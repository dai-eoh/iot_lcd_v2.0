

/* Includes ------------------------------------------------------------------*/
#include <spi_flash.hpp>
#include "string.h"
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_ExFlash
  * @{
  */  

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t CheckMemory=0;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Erases the specified ExFlash sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void ExFlash_Init(void)
{
	ExFlash_CS_HIGH();
	HAL_Delay(100);
}

void ExExFlash_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  ExFlash_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();
  /*!< Send Sector Erase instruction */
  ExFlash_SendByte(FLASH_CMD_SE1);
  /*!< Send SectorAddr high nibble address byte */
  ExFlash_SendByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  ExFlash_SendByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  ExFlash_SendByte(SectorAddr & 0xFF);
  /*!< Deselect the ExFlash: Chip Select high */
  ExFlash_CS_HIGH();

  /*!< Wait the end of ExFlash writing */
  ExFlash_WaitForWriteEnd();
}

void ExFlash_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  ExFlash_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();
  /*!< Send Sector Erase instruction */
  ExFlash_SendByte(FLASH_CMD_SE1);
  /*!< Send SectorAddr high nibble address byte */
  ExFlash_SendByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  ExFlash_SendByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  ExFlash_SendByte(SectorAddr & 0xFF);
  /*!< Deselect the ExFlash: Chip Select high */
  ExFlash_CS_HIGH();

  /*!< Wait the end of ExFlash writing */
  ExFlash_WaitForWriteEnd();
}

void ExFlash_EraseBlock32(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  ExFlash_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();
  /*!< Send Sector Erase instruction */
  ExFlash_SendByte(FLASH_CMD_BE32);
  /*!< Send SectorAddr high nibble address byte */
  ExFlash_SendByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  ExFlash_SendByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  ExFlash_SendByte(SectorAddr & 0xFF);
  /*!< Deselect the ExFlash: Chip Select high */
  ExFlash_CS_HIGH();

  /*!< Wait the end of ExFlash writing */
  ExFlash_WaitForWriteEnd();
}

void ExFlash_EraseBlock64(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  ExFlash_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();
  /*!< Send Sector Erase instruction */
  ExFlash_SendByte(FLASH_CMD_BE64);
  /*!< Send SectorAddr high nibble address byte */
  ExFlash_SendByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  ExFlash_SendByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  ExFlash_SendByte(SectorAddr & 0xFF);
  /*!< Deselect the ExFlash: Chip Select high */
  ExFlash_CS_HIGH();

  /*!< Wait the end of ExFlash writing */
  ExFlash_WaitForWriteEnd();
}
//void ExFlash_EraseMB(uint32_t StartAddr, uint32_t NumMB)
//{
//	NumMB *= 64;
//	for(int i=0;i<NumMB;i++)
//	{
//		ExFlash_EraseBlock64(StartAddr);
//		StartAddr += 0x10000;
//	}
//}
/**
  * @brief  Erases the entire ExFlash.
  * @param  None
  * @retval None
  */
void ExFlash_EraseChip(void)
{
  /*!< Send write enable instruction */
  ExFlash_WriteEnable();

  /*!< Bulk Erase */
  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();
  /*!< Send Bulk Erase instruction  */
  ExFlash_SendByte(FLASH_CMD_CE);
  /*!< Deselect the ExFlash: Chip Select high */
  ExFlash_CS_HIGH();

  /*!< Wait the end of ExFlash writing */
  ExFlash_WaitForWriteEnd();
}

/**
  * @brief  Writes more than one byte to the ExFlash with a single WRITE cycle
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the ExFlash page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the ExFlash.
  * @param  WriteAddr: ExFlash's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the ExFlash, must be equal
  *         or less than "ExFlash_PAGESIZE" value.
  * @retval None
  */
void ExFlash_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the ExFlash */
  ExFlash_WriteEnable();

  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  ExFlash_SendByte(FLASH_CMD_WRITE);
  /*!< Send WriteAddr high nibble address byte to write to */
  ExFlash_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  ExFlash_SendByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  ExFlash_SendByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the ExFlash */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    ExFlash_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the ExFlash: Chip Select high */

  ExFlash_CS_HIGH();

  /*!< Wait the end of ExFlash writing */
  ExFlash_WaitForWriteEnd();

}

void ExFlash_WriteBufferNum(void * pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
  char NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  uint8_t * _temp = (uint8_t*)pBuffer;
  Addr = WriteAddr % FLASH_SPI_PAGESIZE;
  count = FLASH_SPI_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is ExFlash_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < ExFlash_PAGESIZE */
    {
      ExFlash_WritePage(_temp, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > ExFlash_PAGESIZE */
    {
      while (NumOfPage--)
      {
        ExFlash_WritePage(_temp, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr +=  FLASH_SPI_PAGESIZE;
        _temp += FLASH_SPI_PAGESIZE;
      }

      ExFlash_WritePage(_temp, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not ExFlash_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < ExFlash_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > ExFlash_PAGESIZE */
      {
        temp = NumOfSingle - count;

        ExFlash_WritePage(_temp, WriteAddr, count);
        WriteAddr +=  count;
        _temp += count;

        ExFlash_WritePage(_temp, WriteAddr, temp);
      }
      else
      {
        ExFlash_WritePage(_temp, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > ExFlash_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

      ExFlash_WritePage(_temp, WriteAddr, count);
      WriteAddr +=  count;
      _temp += count;

      while (NumOfPage--)
      {
        ExFlash_WritePage(_temp, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr +=  FLASH_SPI_PAGESIZE;
        _temp += FLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        ExFlash_WritePage(_temp, WriteAddr, NumOfSingle);
      }
    }
  }
}

/**
  * @brief  Reads a block of data from the ExFlash.
  * @param  pBuffer: pointer to the buffer that receives the data read from the ExFlash.
  * @param  ReadAddr: ExFlash's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the ExFlash.
  * @retval None
  */
void ExFlash_ReadBufferNum(void * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	uint8_t * _temp = (uint8_t*)pBuffer;
  /*!< Select the ExFlash: Chip Select low */

  ExFlash_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  ExFlash_SendByte(FLASH_CMD_FTRD);

  /*!< Send ReadAddr high nibble address byte to read from */
  ExFlash_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  ExFlash_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  ExFlash_SendByte(ReadAddr & 0xFF);

  ExFlash_SendByte(0);

  while(HAL_SPI_Receive_IT(&ExFlash_SPI, _temp, NumByteToRead) != HAL_OK);

  /*!< Deselect the ExFlash: Chip Select high */
  ExFlash_CS_HIGH();
}

/**
  * @brief  Reads ExFlash identification.
  * @param  None
  * @retval ExFlash identification
  */
uint32_t ExFlash_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();

  /*!< Send "RDID " instruction */
  ExFlash_SendByte(FLASH_CMD_RDID);

  /*!< Read a byte from the ExFlash */
  Temp0 = ExFlash_SendByte(FLASH_DUMMY_BYTE);

  /*!< Read a byte from the ExFlash */
  Temp1 = ExFlash_SendByte(FLASH_DUMMY_BYTE);

  /*!< Read a byte from the ExFlash */
  Temp2 = ExFlash_SendByte(FLASH_DUMMY_BYTE);

  /*!< Deselect the ExFlash: Chip Select high */
  ExFlash_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/**
  * @brief  Initiates a read data byte (READ) sequence from the ExFlash.
  *   This is done by driving the /CS line low to select the device, then the READ
  *   instruction is transmitted followed by 3 bytes address. This function exit
  *   and keep the /CS line low, so the ExFlash still being selected. With this
  *   technique the whole content of the ExFlash is read with a single READ instruction.
  * @param  ReadAddr: ExFlash's internal address to read from.
  * @retval None
  */
void ExFlash_StartReadSequence(uint32_t ReadAddr)
{
  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  ExFlash_SendByte(FLASH_CMD_READ);

  /*!< Send the 24-bit address of the address to read from -------------------*/
  /*!< Send ReadAddr high nibble address byte */
  ExFlash_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte */
  ExFlash_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte */
  ExFlash_SendByte(ReadAddr & 0xFF);
}

/**
  * @brief  Reads a byte from the SPI ExFlash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI ExFlash.
  */
char ExFlash_ReadByte(void)
{
  return (ExFlash_SendByte(FLASH_DUMMY_BYTE));
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
char ExFlash_SendByte(char byte)
{
  /*!< Loop while DR register in not empty */
	uint8_t _receive = 0;
	while(HAL_SPI_TransmitReceive_IT(&ExFlash_SPI,(uint8_t*) &byte, &_receive, 1) != HAL_OK);

	return _receive;
}

/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *         received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */

/**
  * @brief  Enables the write access to the ExFlash.
  * @param  None
  * @retval None
  */
void ExFlash_WriteEnable(void)
{
  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();

  /*!< Send "Write Enable" instruction */
  ExFlash_SendByte(FLASH_CMD_WREN);

  /*!< Deselect the ExFlash: Chip Select high */
  ExFlash_CS_HIGH();
}


/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the ExFlash's
  *         status register and loop until write operation has completed.
  * @param  None
  * @retval None
  */
void ExFlash_WaitForWriteEnd(void)
{
  char ExFlashstatus = 0;

  /*!< Select the ExFlash: Chip Select low */
  ExFlash_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  ExFlash_SendByte(FLASH_CMD_RDSR);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the ExFlash
    and put the value of the status register in ExFlash_Status variable */
    ExFlashstatus = ExFlash_SendByte(FLASH_DUMMY_BYTE);

  }
  while ((ExFlashstatus & FLASH_WIP_FLAG) == SET); /* Write in progress */

  /*!< Deselect the ExFlash: Chip Select high */
  ExFlash_CS_HIGH();
}

/**
  * @brief  Initializes the peripherals used by the SPI ExFlash driver.
  * @param  None
  * @retval None
  */


/**
  * @brief  DeInitializes the peripherals used by the SPI ExFlash driver.
  * @param  None
  * @retval None
  */

bool ExFlash_ReadData(void * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	uint8_t _temp[1024] = {0};
	if(NumByteToRead >= PAGE_MAX)
	{
		return false;
	}
	ExFlash_ReadBufferNum(_temp, ReadAddr, NumByteToRead + 3);

	if(_temp[NumByteToRead] == CODE_SAVE_0 && _temp[NumByteToRead + 1] == CODE_SAVE_1)
		memcpy((uint8_t*)pBuffer, _temp, NumByteToRead);
	else
		return false;
	return true;
}

void ExFlash_WriteData(void * pBuffer, uint32_t WriteAddr, uint32_t num)
{
	if(num >= PAGE_MAX)
	{
		return;
	}
	uint8_t * _temp = (uint8_t*)pBuffer;
	uint8_t _code[2] = {CODE_SAVE_0, CODE_SAVE_1};
	ExFlash_EraseSector(WriteAddr);
	ExFlash_WriteBufferNum(_temp,WriteAddr,num);
	ExFlash_WriteBufferNum(_code, WriteAddr + num, 2);
}

static bool CompareBufferNum(uint8_t * _pBuff1, uint8_t * _pBuff2, uint16_t _num)
{
	for(int i = 0; i < _num; i++)
	{
		if(_pBuff1[i] != _pBuff2[i])
			return false;
	}
	return true;
}

bool ExFlash_WriteBufferNumCheck(void * pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
	if(NumByteToWrite > 10240)
		return false;
	uint8_t _temp[10240] = {0};
	uint8_t _timesCheck = FLASH_NUM_CHECK;
	ExFlash_WriteBufferNum(pBuffer, WriteAddr, NumByteToWrite);
	do
	{
		HAL_Delay(10);
		ExFlash_ReadBufferNum(_temp, WriteAddr, NumByteToWrite);
		if(CompareBufferNum((uint8_t*)pBuffer, _temp, NumByteToWrite))
			return true;
	}
	while(_timesCheck--);
	return false;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
