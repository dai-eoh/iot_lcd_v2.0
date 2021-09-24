
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "define.hpp"
/* Exported types ------------------------------------------------------------*/
typedef enum {FULL = 0, EMPTY= 1} MemoryStatus;
/* Exported constants --------------------------------------------------------*/
#define LENTH(x)  (sizeof(x) / sizeof((x)[0]))
//SET ADDRESS ***************************************************************
#define CODE_SAVE_0 0x27
#define CODE_SAVE_1 0x10
#define PAGE_MAX		0x1000

#define NETWORKCONFIG_ADDRESS 	0x0000

#define MODBUSCONFIG_ADDRESS		0x1000

#define SENSORCONFIG_ADDRESS		0x2000

#define CALIBCONFIG_ADDRESS			0x3000

#define DEVICECONFIG_ADDRESS		0x4000

#define ADVANCEDCONFIG_ADDRESS	0x5000

#define ALARMCONFIG_ADDRESS			0x6000

#define PASSWORDCONFIG_ADDRESS	0x7000

#define FIRMWAREINFO_ADDRESS		0xF000

#define EXFLASH_APPLICATION_ADDRESS  0x200000

#define SIZE_FIRMWARE						0x40000

#define SIZE_SECTOR							0x1000
//***************************************************************************
#define RUNFIRMWARE_CODE			0xBB
#define UPDATEFIRMWARE_CODE		0xAA

//***************************************************************************
/* W25Qxx SPI Flash supported commands */
#define FLASH_CMD_WRITE          0x02  /* Write to Memory instruction */
#define FLASH_CMD_WRSR           0x01  /* Write Status Register instruction */
#define FLASH_CMD_WREN           0x06  /* Write enable instruction */
#define FLASH_CMD_WRDI           0x04
#define FLASH_CMD_READ           0x03  /* Read from Memory instruction */
#define FLASH_CMD_RDSR           0x05  /* Read Status Register instruction  */
#define FLASH_CMD_FTRD					 0x0B
#define FLASH_CMD_RDID           0x9F  /* Read identification */
#define FLASH_CMD_SE1            0x20	/* Sector Erase instruction */
#define FLASH_CMD_BE32           0x52	/* Block Erase 32MB instruction */
#define FLASH_CMD_BE64           0xD8	/* Block Erase 64MB instruction */
#define FLASH_CMD_CE             0xC7  /* Bulk Erase instruction */

#define FLASH_WIP_FLAG           0x01  /* Write In Progress (WIP) flag */

#define FLASH_DUMMY_BYTE         0xA5
#define FLASH_SPI_PAGESIZE       0x100

#define FLASH_W25Q64        	  0xEF4016

#define FLASH_SIZE				  		4 // Unit: MB
#define sFASH_SETMEMORY		      FLASH_SIZE*1024*1024 //
/* M25P FLASH SPI Interface pins  */  
#define ExFlash_SPI                           hspi3
/* Exported macro ------------------------------------------------------------*/
/* Select sFLASH: Chip Select pin low */
#define ExFlash_CS_LOW()       HAL_GPIO_WritePin(FLS_CS_GPIO_Port, FLS_CS_Pin, GPIO_PIN_RESET);
/* Deselect sFLASH: Chip Select pin high */
#define ExFlash_CS_HIGH()      HAL_GPIO_WritePin(FLS_CS_GPIO_Port, FLS_CS_Pin, GPIO_PIN_SET);

#define FLASH_NUM_CHECK					5
/* Exported functions ------------------------------------------------------- */


void ExFlash_Init(void);
uint32_t ExFlash_ReadID(void);

void ExFlash_EraseSector(uint32_t SectorAddr);
void ExFlash_EraseBlock32(uint32_t SectorAddr);
void ExFlash_EraseBlock64(uint32_t SectorAddr);
void ExFlash_EraseMB(uint32_t Addr, uint32_t NumMB);
void ExFlash_EraseChip(void);

void ExFlash_StartReadSequence(uint32_t ReadAddr);

void ExFlash_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
char ExFlash_ReadByte(void);
char ExFlash_SendByte(char byte);
void ExFlash_WriteEnable(void);
void ExFlash_WaitForWriteEnd(void);


void ExFlash_WriteBufferNum(void * pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
void ExFlash_ReadBufferNum(void * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

bool ExFlash_ReadData(void * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void ExFlash_WriteData(void * pBuffer, uint32_t WriteAddr, uint32_t num);

bool ExFlash_WriteBufferNumCheck(void * pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_FLASH_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
