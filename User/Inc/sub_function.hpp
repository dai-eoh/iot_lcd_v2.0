/*
 * sub_function.hpp
 *
 *  Created on: Jul 9, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_SUB_FUNCTION_HPP_
#define INC_SUB_FUNCTION_HPP_
#ifdef __cplusplus
extern "C" {
#endif
#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include "main.h"
#include "define.hpp"
#include "ModbusMaster.hpp"


void strrev(char * pString);
void resetChip(void);
uint32_t convertHexToEdian(uint32_t _number, uint8_t _index);
float converHexToFloat(uint32_t _number, uint8_t _index);
void pushDataToArray(uint32_t value, uint32_t *_array, uint8_t _windowSize);
uint32_t averageArray(uint32_t *_array, uint8_t _windowSize);
LastResetReason getLastResetReason(void);

#ifdef __cplusplus

}
#endif
#endif /* INC_SUB_FUNCTION_HPP_ */
