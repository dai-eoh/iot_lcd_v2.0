/*
 * process.cpp
 *
 *  Created on: Jun 23, 2021
 *      Author: EoH.Dai
 */
#include <spi_flash.hpp>
#include "process.hpp"
#include "variable_process.hpp"
#include "lcd_display.hpp"



void ModbusSlave_Init()
{
	uint16_t _baud = ModbusSlave.changeBaudrate(deviceConfig.slaveBaud);
	ModbusSlave.setUnitAddress(deviceConfig.slaveAddr);
	ModbusSlave.begin(_baud, coilArray, registerArray);
}
void ModbusMaster_Init()
{
	ModbusMaster.begin();
	ModbusMaster.changeBaudrate(deviceConfig.masterBaud);
}

static void ModbusSlave_SetValueArrayRegister(uint8_t _number, uint32_t _value)
{
	switch(_number)
	{
	case 0:
		registerArray[30] =  (_value >> 16) & 0xFFFF;
		registerArray[31] =  _value & 0xFFFF;
		break;
	case 1:
		registerArray[40] =  (_value >> 16) & 0xFFFF;
		registerArray[41] =  _value & 0xFFFF;
		break;
	case 2:
		registerArray[50] =  (_value >> 16) & 0xFFFF;
		registerArray[51] =  _value & 0xFFFF;
		break;
	case 3:
		registerArray[60] =  (_value >> 16) & 0xFFFF;
		registerArray[61] =  _value & 0xFFFF;
		break;
	case 4:
		registerArray[20] =  (_value >> 16) & 0xFFFF;
		registerArray[21] =  _value & 0xFFFF;
		break;
	case 5:
		registerArray[22] =  (_value >> 16) & 0xFFFF;
		registerArray[23] =  _value & 0xFFFF;
		break;
	case 6:
		registerArray[0] =  (_value >> 16) & 0xFFFF;
		registerArray[1] =  _value & 0xFFFF;
		break;
	case 7:
		registerArray[100] =  _value & 0xFFFF;
		break;
	}
}

static void ModbusValueHandle(uint8_t _number, ProcessData *_processData)
{
	float _value = 0;
	uint32_t _valueModbus = 0;
	float _scale = pow(10, modbusConfig[_number].scale);
	float _offset = calibConfig[_number].offset;
	switch(modbusConfig[_number].dataType)
	{
	case IN16:
	case UI16:
		_valueModbus = ModbusMaster.getDataModbus(0);
		break;
	case UI32:
	case FLO1:
	case FLO2:
		_valueModbus = (ModbusMaster.getDataModbus(0)<<16) + ModbusMaster.getDataModbus(1);
		break;
	}
	uint8_t _avg = sensorConfig[_number].average;
	if(_avg > 1)
	{
		_processData->index++;
		pushDataToArray(_valueModbus, _processData->arrValue, _avg);
		if((_processData->index > 1 && _processData->index < _avg))
			return;
		if(_processData->index > _avg - 1)
		{
			_valueModbus = averageArray(_processData->arrValue, _avg);
			_processData->index = _avg;
		}
	}
	switch(modbusConfig[_number].dataType)
	{
	case IN16:
		_value = (float) (((int16_t)_valueModbus) *  _scale) + _offset;
		break;
	case UI16:
		_value = (float) (((uint16_t)_valueModbus) *  _scale) + _offset;
		break;
	case UI32:
		_value = (float) ((convertHexToEdian(_valueModbus, BIG_ENDIAN_)) * _scale) + _offset;
		break;
	case FLO1:
		_value = converHexToFloat(_valueModbus, BIG_ENDIAN_) * _scale + _offset;
		break;
	case FLO2:
		_value = converHexToFloat(_valueModbus, MIDLIT_ENDIAN) * _scale + _offset;
	default:
		break;
	}


	if(_value > sensorConfig[_number].maxValue)
		_value = sensorConfig[_number].maxValue;
	else if(_value < sensorConfig[_number].minValue)
		_value = sensorConfig[_number].minValue;
	if(advancedConfig[_number].filter > 0.001f)
	{
		_value = Kalman_GetValue(_value, _number);
	}
	if(_value < alarmConfig[_number].lowValue)
		_processData->alarmStatus = LOW;
	else if(_value > alarmConfig[_number].highValue)
		_processData->alarmStatus = HIGH;
	else
		_processData->alarmStatus = NORMAL;
	_processData->value =  _value;
	uint8_t _flagQueue = _number;
	uint32_t _valueNetwork = 0;
	uint8_t _pos = 0;
	for(int i = 0; i < _number; i++)
	{
		_pos += modbusConfig[i].length;
	}
//	_pos = _pos + _number;
	switch(modbusConfig[_number].dataType)
	{
	case IN16:
		_valueNetwork = (int16_t)(_value / _scale);
		valueModbus[_pos] = _valueNetwork;
		break;
	case UI16:
		_valueNetwork = (uint16_t)(_value / _scale);
		valueModbus[_pos] = _valueNetwork;
		break;
	case UI32:
		_value = (uint32_t)(_value  / _scale);
		_valueNetwork = (uint32_t) convertHexToEdian((uint32_t)_value, BIG_ENDIAN_);
		valueModbus[_pos] = ((uint32_t)_valueNetwork >> 16) & 0xFFFF;
		valueModbus[_pos + 1] = ((uint32_t)_valueNetwork) & 0xFFFF;
		break;
	case FLO1:
		_value = _value  / _scale;
		_valueNetwork = (uint32_t) convertHexToEdian(*((uint32_t*)&_value), BIG_ENDIAN_);
		valueModbus[_pos] = ((uint32_t)_valueNetwork >> 16) & 0xFFFF;
		valueModbus[_pos + 1] = ((uint32_t)_valueNetwork) & 0xFFFF;
		break;
	case FLO2:
		_value = _value  / _scale;
		_valueNetwork = (uint32_t) convertHexToEdian(*((uint32_t*)&_value), MIDLIT_ENDIAN);
		valueModbus[_pos] = ((uint32_t)_valueNetwork >> 16) & 0xFFFF;
		valueModbus[_pos + 1] = ((uint32_t)_valueNetwork) & 0xFFFF;
		break;
	}

	ModbusSlave_SetValueArrayRegister(_number, _valueNetwork);
	osMessageQueuePut(modbusLCDQueueHandle, &_flagQueue, 0U, 0U);
}

static bool WriteConfigHandle(ModbusRTU * _writeModbus, WriteConfig _writeConfig)
{
	_writeModbus->addr = _writeConfig.addr;
	_writeModbus->regID = _writeConfig.regID;
	if(_writeConfig.func == FUNCTION_5 && _writeConfig.dataType == BOOL)
	{
		_writeModbus->func = 5;
		if(_writeConfig.value == ON)
			_writeModbus->length = 0xFF00;
		else
			_writeModbus->length = 0x0000;
		return true;
	}
	else if(_writeConfig.func == FUNCTION_6)
	{
		_writeModbus->func = 6;
		if(_writeConfig.dataType == UINT16 || _writeConfig.dataType == INT16)
		{
			_writeModbus->length = (uint16_t)_writeConfig.value;
			return true;
		}
	}
	else if(_writeConfig.func == FUNCTION_16)
	{
		uint32_t _temp = 0;
		_writeModbus->func = 16;
		_writeModbus->length = 2;
		switch(_writeConfig.dataType)
		{
		case FLOAT1:
			_temp = convertHexToEdian(_writeConfig.value, BIG_ENDIAN_);
			break;
		case FLOAT2:
			_temp = convertHexToEdian(_writeConfig.value, MIDLIT_ENDIAN);
			break;
		case UINT32:
			_temp = _writeConfig.value;
			break;
		default:
			return false;
		}
		_writeModbus->extraArray[0] = (_temp >> 16) & 0xFFFF;
		_writeModbus->extraArray[1] = _temp & 0xFFFF;
		return true;
	}
	return false;
}

void ModbusSlave_Poll(void)
{
	ModbusSlave.poll();
}

void ModbusMaster_Poll(void)
{
	bool _flagModbus  = false;
	ModbusRTU _writeModbus = {0};
	static ModbusCalib _modbusCalib = {0};
	uint8_t _flagQueue;
	static uint32_t _preTime = 0;
	static bool _flagGetValueCalib = false;
	static uint32_t _timeResponse = 0;
	if(HAL_GetTick() - _preTime > (deviceConfig.sampleTime))
	{
		for(int i = 0; i < deviceConfig.numConfig; i++)
		{
			HAL_IWDG_Refresh(&hiwdg);
			_flagModbus = ModbusMaster.modbusMasterTransaction(&modbusRTU[i]);
			processData[i].failFlag = _flagModbus;
			_flagQueue = i;
			osDelay(20);
			if(_flagModbus)
				ModbusValueHandle(i, &processData[i]);
			else
				processData[i].index = 0;
			osMessageQueuePut(modbusNetworkQueueHandle, &_flagQueue, 0U, 0U);
			if (osMessageQueueGet(writeConfigQueueHandle, &_flagQueue, NULL, 1) == osOK)
			{
				_flagQueue = WriteConfigHandle(&_writeModbus, writeConfig);
				if(_flagQueue)
				{
					_flagQueue = ModbusMaster.modbusMasterTransaction(&_writeModbus);
					if(!_flagQueue)
						ModbusMaster.failRate--;
					ModbusMaster.modbusTotal--;
				}
				osMessageQueuePut(responseWriteQueueHandle, &_flagQueue, 0U, 0U);
			}


			if (osMessageQueueGet(modbusCalibQueueHandle, &_modbusCalib, NULL, 1) == osOK)
			{
				uint8_t _flag = ModbusMaster.modbusMasterTransaction(&_modbusCalib.command);
				osDelay(20);
				osMessageQueuePut(responseCommandCalibHandle, &_flag, 0U, 0U);
				if(_modbusCalib.type == TUR_RESPONSE || _modbusCalib.type == CLO_SENS_RESPONSE || _modbusCalib.type == CLO_ZERO_RESPONSE )
					_timeResponse = HAL_GetTick();
			}
			if(HAL_GetTick() - _timeResponse < TIME_CALIB)
			{
				ResponseCalib _responseCalib = {0};
				_responseCalib.type = _modbusCalib.type;
				_responseCalib.flag = ModbusMaster.modbusMasterTransaction(&_modbusCalib.response);
				_responseCalib.status = ModbusMaster.getDataModbus(0);
				osDelay(20);
				osMessageQueuePut(responseStatusCalibQueueHandle, &_responseCalib, 0U, 0U);
			}
			osMessageQueueGet(flagGetValueCalibHandle, &_flagGetValueCalib, NULL, 1);
			if(_flagGetValueCalib)
				osMessageQueuePut(displayStatusCalibHandle, &_flagQueue, 0U, 0U);
		}
		modbusTotal = ModbusMaster.modbusTotal;
		modbusFailrate = ModbusMaster.failRate;
		if(modbusTotal > 100)
		{
			modbusTotal = 0;
			modbusFailrate = 0;
			ModbusMaster.modbusTotal = 0;
			ModbusMaster.failRate = 0;
		}
		_preTime = HAL_GetTick();
	}

	if (osMessageQueueGet(fetchControlQueueHandle, &_flagQueue, NULL, 1) == osOK)
	{
		for(int i = 0; i < networkData.numControl; i++)
		{
			ModbusMaster.modbusMasterTransaction(&modbusControl[i]);
			osDelay(100);
		}
	}


}

void System_GetLastReason(void)
{
	lastResetReason =  getLastResetReason();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == huart1.Instance)
		ModbusMaster.Modbus_IRQHandler();
	else if(huart->Instance == huart4.Instance)
		ModbusSlave.Modbus_IRQHandler();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == MODBUS_MASTER.Instance)
		ModbusMaster.Modbus_IRQHandler_Error();
	else if(huart->Instance == MODBUS_SLAVE.Instance)
		ModbusSlave.Modbus_IRQHandler_Error();
	else if(huart->Instance == SIM_UART.Instance)
		Sim800C.SIM_IRQHandler_Error();
	else if(huart->Instance == ESP_UART.Instance)
		Esp32.Esp32_IRQHandler_Error();
}


