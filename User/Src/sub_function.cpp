/*
 * sub_function.cpp
 *
 *  Created on: Jul 9, 2021
 *      Author: EoH.Dai
 */

#include "sub_function.hpp"

void strrev(char * pString)
{
	int i;
	int dim = strlen(pString);
	char l;

	for (i = 0; i < dim / 2; i++)
	{
		l = pString[i];
		pString[i] = pString[dim - i - 1];
		pString[dim - i - 1] = l;
	}
}

void resetChip(void)
{
	NVIC_SystemReset();
}

uint32_t convertHexToEdian(uint32_t _number, uint8_t _index)
{
	uint8_t _aByte, _bByte, _cByte, _dByte;
	uint32_t _result = 0;
	_aByte = (_number >> 24) & 0xFF;
	_bByte = (_number >> 16) & 0xFF;
	_cByte = (_number >> 8) & 0xFF;
	_dByte = _number & 0xFF;
	switch(_index)
	{
	case BIG_ENDIAN_:
		_result = _number;
		break;
	case LIT_ENDIAN:
		_result = (word(_dByte, _cByte) << 16) + word(_bByte, _aByte);
		break;
	case MIDBIG_ENDIAN:
		_result = (word(_bByte, _aByte) << 16) + word(_dByte, _cByte);
		break;
	case MIDLIT_ENDIAN:
		_result = (word(_cByte, _dByte) << 16) + word(_aByte, _bByte);
		break;
	}
	return _result;
}

float converHexToFloat(uint32_t _number, uint8_t _index)
{
	float _result;
	_number = convertHexToEdian( _number, _index);
	_result = *((float*)&_number);
	return _result;
}

void pushDataToArray(uint32_t value, uint32_t *_array, uint8_t _windowSize)
{
	for(int i = 1; i<_windowSize; i++)
		_array[i - 1] = _array[i];
	_array[_windowSize - 1] = value;
}

uint32_t averageArray(uint32_t *_array, uint8_t _windowSize)
{
	uint64_t _sum = 0;
	for(int i = 0; i<_windowSize; i++)
		_sum += _array[i];
	return (uint32_t)(_sum / _windowSize);
}

/// @brief      Obtain the STM32 system reset cause
/// @param      None
/// @return     The system reset cause
LastResetReason getLastResetReason(void)
{
	LastResetReason _lastResetReason;

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
    {
        _lastResetReason = RESET_CAUSE_LOW_POWER_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        _lastResetReason = RESET_CAUSE_WINDOW_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
    {
        _lastResetReason = RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        _lastResetReason = RESET_CAUSE_SOFTWARE_RESET; // This reset is induced by calling the ARM CMSIS `NVIC_SystemReset()` function!
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
    {
        _lastResetReason = RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
    {
        _lastResetReason = RESET_CAUSE_EXTERNAL_RESET_PIN_RESET;
    }
    // Needs to come *after* checking the `RCC_FLAG_PORRST` flag in order to ensure first that the reset cause is
    // NOT a POR/PDR reset. See note below.
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
    {
        _lastResetReason = RESET_CAUSE_BROWNOUT_RESET;
    }
    else
    {
        _lastResetReason = RESET_CAUSE_UNKNOWN;
    }

    // Clear all the reset flags or else they will remain set during future resets until system power is fully removed.
    __HAL_RCC_CLEAR_RESET_FLAGS();

    return _lastResetReason;
}
