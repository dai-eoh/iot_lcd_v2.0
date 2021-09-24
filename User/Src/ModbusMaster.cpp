/*
 * ModbusMaster.cpp
 *
 *  Created on: Apr 1, 2021
 *      Author: daitr
 */

#include "ModbusMaster.hpp"


ModbusMaster::ModbusMaster()
{

}

void ModbusMaster::begin(void)
{
	HAL_UART_Receive_DMA(&MODBUS_MASTER, _responseBuffer, sizeof(_responseBuffer));
}

uint8_t ModbusMaster::getModbusFail(void)
{
	return failRate;
}

uint8_t ModbusMaster::getModbusTotal(void)
{
	return modbusTotal;
}
uint16_t ModbusMaster::getDataModbus(int _number)
{
		return _dataBuffer[_number];
}

void ModbusMaster::changeBaudrate(uint8_t _number)
{
	switch(_number)
	{
	case 0:
		_baudRate = 4800;
		break;
	case 1:
		_baudRate = 9600;
		break;
	case 2:
		_baudRate = 19200;
		break;
	case 4:
		_baudRate = 38400;
		break;
	default:
		break;
	}
	failRate = 0;
	modbusTotal = 0;
	HAL_UART_Abort_IT(&MODBUS_MASTER);
	HAL_UART_DeInit(&MODBUS_MASTER);
	MODBUS_MASTER.Init.BaudRate = _baudRate;
	if (HAL_UART_Init(&MODBUS_MASTER) != HAL_OK)
	{
	    Error_Handler();
	}
	memset(_responseBuffer, 0, sizeof(_responseBuffer));
	HAL_UART_Receive_DMA(&MODBUS_MASTER, _responseBuffer, sizeof(_responseBuffer));
}

void ModbusMaster::clearResponseBuffer(void)
{
	memset(_responseBuffer, 0, sizeof(_responseBuffer));
	__HAL_DMA_DISABLE(&MODBUS_MASTER_DMA);
	__HAL_DMA_SET_COUNTER(&MODBUS_MASTER_DMA, sizeof(_responseBuffer));
	__HAL_DMA_ENABLE(&MODBUS_MASTER_DMA);
}

void ModbusMaster::sendCommand(uint8_t *string, uint8_t length)
{
	TRANS_MODBUS;
	while(HAL_UART_Transmit_IT(&huart1, string, length) != HAL_OK)
		;
}

uint16_t ModbusMaster::calculateCRC(uint8_t* buffer, uint8_t length)
{
	uint16_t crc = 0xFFFF;

	for (int pos = 0; pos < length; pos++) {
		crc ^= (uint16_t)buffer[pos]; 								// XOR byte into least sig. byte of crc

		for (int i = 8; i != 0; i--) {					// Loop over each bit 8 7 6 5 4 3 2 1
			if ((crc & 0x0001) != 0) { 						// If the LSB is set
 				crc >>= 1;
				crc ^= 0xA001;											// Shift right and XOR 0xA001
			}
			else
				crc >>= 1;
		}
	}
	return crc;
}

void ModbusMaster::getDataModbus(void)
{
	memset(_dataBuffer, 0, sizeof(_dataBuffer));
	int i = 0;
	switch(_function)
		{
			case ReadCoils:
			case ReadDiscreteInputs:
				// load bytes into word; response bytes are ordered L, H, L, H, ...
				for (i = 0; i < (_responseBuffer[2] >> 1); i++)
				{
					if (i < MaxBufferSize)
					{
						_dataBuffer[i] = word(_responseBuffer[2 * i + 4], _responseBuffer[2 * i + 3]);
					}

					_dataBufferLength = i;
				}

				// in the event of an odd number of bytes, load last byte into zero-padded word
				if (_responseBuffer[2] % 2)
				{
					if (i < MaxBufferSize)
					{
						_dataBuffer[i] = word(0, _responseBuffer[2 * i + 3]);
					}

					_dataBufferLength = i + 1;
				}
				break;

			case ReadInputRegisters:
			case ReadHoldingRegisters:
				// load bytes into word; response bytes are ordered H, L, H, L, ...
				for (i = 0; i < (_responseBuffer[2] >> 1); i++)
				{
					if (i < MaxBufferSize)
					{
						_dataBuffer[i] = word(_responseBuffer[2 * i + 3], _responseBuffer[2 * i + 4]);
					}

					_dataBufferLength = i+1;
				}
				break;
		}
}

void ModbusMaster::modbusFail(ModbusRTU* modbusRTU)
{
	failRate++;
	modbusRTU->failFlag = 1;
}

bool ModbusMaster::waitModbusResponse(void)
{
	uint32_t tickCounter = 0;
//	if(modbusTotal++ > 99)
//	{
//		failRate = 0;
//		modbusTotal = 0;
//	}
	modbusTotal++;
	while(_responseBuffer[0] != _slaveAddress || _responseBuffer[1] != _function)
	{
		tickCounter++;
		if(tickCounter > TickTimeout_1 )
		{
			return false;
		}
#ifdef USE_RTOS_MBRTU
		osDelay(10);
#else
		HAL_Delay(10);
#endif
	}
	tickCounter = 0;
	switch (_function)
	{
		case ReadCoils:
		case ReadDiscreteInputs:
		case ReadHoldingRegisters:
		case ReadInputRegisters:
			// 5 = slaveaddress + function + numreg + 2 byte crc
			while(__HAL_DMA_GET_COUNTER(&MODBUS_MASTER_DMA) > sizeof(_responseBuffer) - 5 - _responseBuffer[2])
			{
#ifdef USE_RTOS_MBRTU
				osDelay(10);
#else
				HAL_Delay(10);
#endif
				if(tickCounter++ > TickTimeout_2)
				{
					return false;
				}
			}
			break;
		default:
			while(__HAL_DMA_GET_COUNTER(&MODBUS_MASTER_DMA) > sizeof(_responseBuffer) - 8)
			{
#ifdef USE_RTOS_MBRTU
				osDelay(10);
#else
				HAL_Delay(10);
#endif
				if(tickCounter++ > TickTimeout_2)
				{
					return false;
				}
			}
			break;
	}
	return true;
}

bool ModbusMaster::checkCRCReceive(void)
{
	uint8_t _len;
	uint16_t CRC16;
	switch (_function)
	{
		case 5:
		case 6:
		case 15:
		case 16:
			_len = 6;
			break;
		default:
			_len = 3 + _responseBuffer[2];
			break;
	}
	if(_len < 4)
		return false;
	CRC16 = calculateCRC(_responseBuffer, _len);
	if(CRC16 == word(_responseBuffer[_len+1],_responseBuffer[_len]))
		return true;
	else
		return false;
}

bool ModbusMaster::modbusMasterTransaction(ModbusRTU* modbusRTU)
{
  uint8_t modbusADU[256];
  uint8_t modbusADUSize = 0;
  uint8_t Qty;
  bool statusMB = false;

  _slaveAddress = modbusRTU->addr;
  _function = modbusRTU->func;
  memset(_transmitBuffer, 0, sizeof(_transmitBuffer));
  memcpy(_transmitBuffer, modbusRTU->extraArray, sizeof(_transmitBuffer));  // assemble Modbus Request Application Data Unit
  modbusADU[modbusADUSize++] = _slaveAddress;
  modbusADU[modbusADUSize++] = _function;

  switch(_function)
  {
    case ReadCoils:
    case ReadDiscreteInputs:
    case ReadInputRegisters:
    case ReadHoldingRegisters:
    	_readAddress = modbusRTU->regID;
    	_readQty = modbusRTU->length;
      modbusADU[modbusADUSize++] = highByte(_readAddress);
      modbusADU[modbusADUSize++] = lowByte(_readAddress);
      modbusADU[modbusADUSize++] = highByte(_readQty);
      modbusADU[modbusADUSize++] = lowByte(_readQty);
      statusMB = true;
      break;
  }

  switch(_function)
  {
    case WriteSingleCoil:
    case WriteMultipleCoils:
    case WriteSingleRegister:
    case WriteMultipleRegisters:
    	_writeAddress = modbusRTU->regID;
    	_writeQty = modbusRTU->length;
      modbusADU[modbusADUSize++] = highByte(_writeAddress);
      modbusADU[modbusADUSize++] = lowByte(_writeAddress);
      statusMB = true;
      break;
  }

  switch(_function)
  {
    case WriteSingleCoil:
      modbusADU[modbusADUSize++] = highByte(_writeQty);
      modbusADU[modbusADUSize++] = lowByte(_writeQty);
      break;

    case WriteSingleRegister:
      modbusADU[modbusADUSize++] = highByte(_writeQty);
      modbusADU[modbusADUSize++] = lowByte(_writeQty);
      break;

    case WriteMultipleCoils:
      modbusADU[modbusADUSize++] = highByte(_writeQty);
      modbusADU[modbusADUSize++] = lowByte(_writeQty);
      Qty = (_writeQty % 8) ? ((_writeQty >> 3) + 1) : (_writeQty >> 3);
      modbusADU[modbusADUSize++] = Qty;
      for (int i = 0; i < Qty; i++)
      {
        switch(i % 2)
        {
          case 0: // i is even
            modbusADU[modbusADUSize++] = lowByte(_transmitBuffer[i >> 1]);
            break;

          case 1: // i is odd
            modbusADU[modbusADUSize++] = highByte(_transmitBuffer[i >> 1]);
            break;
        }
      }
      break;

    case WriteMultipleRegisters:
      modbusADU[modbusADUSize++] = highByte(_writeQty);
      modbusADU[modbusADUSize++] = lowByte(_writeQty);
      modbusADU[modbusADUSize++] = lowByte(_writeQty << 1);

      for (int i = 0; i < lowByte(_writeQty); i++)
      {
        modbusADU[modbusADUSize++] = highByte(_transmitBuffer[i]);
        modbusADU[modbusADUSize++] = lowByte(_transmitBuffer[i]);
      }
      break;
  }
  if(!statusMB)
  	return false;
  // append CRC

  _u16CRC = calculateCRC(modbusADU, modbusADUSize);
  modbusADU[modbusADUSize++] = lowByte(_u16CRC);
  modbusADU[modbusADUSize++] = highByte(_u16CRC);
  modbusADU[modbusADUSize] = 0;
  //transmit
  clearResponseBuffer();
  sendCommand(modbusADU, modbusADUSize);
  if(!waitModbusResponse())
  {
  	modbusFail(modbusRTU);
  	return false;
  }
  if(!checkCRCReceive())
  {
  	modbusFail(modbusRTU);
  	return false;
  }
  getDataModbus();
  modbusRTU -> failFlag = 0;
  return true;
}
void ModbusMaster::Modbus_IRQHandler(void)
{
	RECEI_MODBUS;
}
void ModbusMaster::Modbus_IRQHandler_Error(void)
{
//	failRate = 0;
//	modbusTotal = 0;
//	HAL_UART_Abort_IT(&MODBUS_MASTER);
//	HAL_UART_DeInit(&MODBUS_MASTER);
//	MODBUS_MASTER.Init.BaudRate = _baudRate;
//	if (HAL_UART_Init(&MODBUS_MASTER) != HAL_OK)
//	{
//	    Error_Handler();
//	}
//	memset(_responseBuffer, 0, sizeof(_responseBuffer));
//	HAL_UART_Receive_DMA(&MODBUS_MASTER, _responseBuffer, sizeof(_responseBuffer));
		memset(_responseBuffer, 0, sizeof(_responseBuffer));
		HAL_UART_DMAStop(&MODBUS_MASTER);
		HAL_UART_Receive_DMA(&MODBUS_MASTER, _responseBuffer, sizeof(_responseBuffer));
}
