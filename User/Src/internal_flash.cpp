/*
 * internal_flash.cpp
 *
 *  Created on: Sep 13, 2021
 *      Author: EoH.Dai
 */

#include "internal_flash.hpp"

//static uint32_t GetSector(uint32_t Address)
//{
//	uint32_t sector = 0;
//	if(Address >= 0x08000000 && Address < 0x08004000)
//		sector = FLASH_SECTOR_0;
//	else if(Address >= 0x08004000 && Address < 0x08008000)
//		sector = FLASH_SECTOR_1;
//	else if(Address >= 0x08008000 && Address < 0x0800C000)
//		sector = FLASH_SECTOR_2;
//	else if(Address >= 0x0800C000 && Address < 0x08010000)
//		sector = FLASH_SECTOR_3;
//	else if(Address >= 0x08010000 && Address < 0x08020000)
//		sector = FLASH_SECTOR_4;
//	else if(Address >= 0x08020000 && Address < 0x08040000)
//		sector = FLASH_SECTOR_5;
//	else if(Address >= 0x08040000 && Address < 0x08060000)
//		sector = FLASH_SECTOR_6;
//	else if(Address >= 0x08060000 && Address < 0x08080000)
//		sector = FLASH_SECTOR_7;
//	else if(Address >= 0x08080000 && Address < 0x080A0000)
//		sector = FLASH_SECTOR_8;
//	else if(Address >= 0x080A0000 && Address < 0x080C0000)
//		sector = FLASH_SECTOR_9;
//	else if(Address >= 0x080C0000 && Address < 0x080E0000)
//		sector = FLASH_SECTOR_10;
//	else if(Address >= 0x080E0000 && Address <= 0x080FFFFF)
//		sector = FLASH_SECTOR_11;
//
//	return sector;
//}

uint32_t InFlash_EraseSector(uint8_t _sector)
{
	uint32_t _sectorError;
	FLASH_EraseInitTypeDef _pEraseInit;
  HAL_FLASH_Unlock();

  _pEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
  _pEraseInit.Sector = _sector;
  _pEraseInit.NbSectors = 1;
  _pEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;

	if(HAL_FLASHEx_Erase(&_pEraseInit, &_sectorError) != HAL_OK)
	{
		return HAL_FLASH_GetError();
	}

	HAL_FLASH_Lock();
  return HAL_OK;
}

uint32_t InFlash_Write(uint32_t _address, uint32_t * _pData, uint16_t _length)
{
	HAL_FLASH_Unlock();
	for(int i = 0 ; (i < _length) &&  (_address <= (FLASH_END_ADDRESS - 4)); i++)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _address, _pData[i]) == HAL_OK)
		{
			if(*(__IO uint32_t*)_address != _pData[i])
			{
				return HAL_FLASH_GetError();
			}
		}
		else
		{
			return HAL_FLASH_GetError();
		}
		_address += 4;
	}
	HAL_FLASH_Lock();
	return HAL_OK;
}

void InFlash_Read(uint32_t _address, uint32_t * _pData, uint16_t _length)
{
	for(int i = 0; (i < _length) &&  (_address <= (FLASH_END_ADDRESS - 4)); i++)
	{
			_pData[i] = *(__IO uint32_t *)_address;
			_address += 4;
	}
}
