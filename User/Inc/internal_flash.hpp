/*
 * internal_flash.hpp
 *
 *  Created on: Sep 13, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_INTERNAL_FLASH_HPP_
#define INC_INTERNAL_FLASH_HPP_

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define FLASH_END_ADDRESS        ((uint32_t)0x080FFFFF)
#define INFLASH_APPLICATION_ADDRESS			 0x08020000

#define INFLASH_INFO_FIRMWARE						 0x080E000C //sector 11

uint32_t InFlash_EraseSector(uint8_t _sector);
uint32_t InFlash_Write(__IO uint32_t _address, uint32_t * _pData, uint16_t _length);
void InFlash_Read(uint32_t _address, uint32_t * _pData, uint16_t _length);



#ifdef __cplusplus
}
#endif

#endif /* INC_INTERNAL_FLASH_HPP_ */
