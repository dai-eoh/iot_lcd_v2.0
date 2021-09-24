/*
 * ModbusSlave.cpp
 *
 *  Created on: May 4, 2021
 *      Author: daitr
 */

#include "ModbusSlave.hpp"

#define MODBUS_FRAME_SIZE 4
#define MODBUS_CRC_LENGTH 2

#define MODBUS_ADDRESS_INDEX 0
#define MODBUS_FUNCTION_CODE_INDEX 1
#define MODBUS_DATA_INDEX 2

#define MODBUS_BROADCAST_ADDRESS 0
#define MODBUS_ADDRESS_MIN 1
#define MODBUS_ADDRESS_MAX 247

#define MODBUS_HALF_SILENCE_MULTIPLIER 3
#define MODBUS_FULL_SILENCE_MULTIPLIER 7

#define highByte(x) ( (x) >> (8) ) 	 // keep upper 8 bits
#define lowByte(x) 	( (x) & (0xff) ) // keep lower 8 bits
#define word(h,l)   ((((h) & 0xff) << 8) + (l))
#define readUInt16(arr, index) word(arr[index], arr[index + 1])
#define readCRC(arr, length) word(arr[(length - MODBUS_CRC_LENGTH) + 1], arr[length - MODBUS_CRC_LENGTH])
#define GET_TICK		HAL_GetTick()*1000
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define bit(bitPosition) (1 << (bitPosition))
#define bitSet(x, bitPosition) ((x) |= (1 << (bitPosition)))
#define bitClear(x, bitPosition) ((x) &= ~(1 << (bitPosition)))
#define bitRead(x, bitPosition) (((x) >> bitPosition) & 1)
#define bitWrite(x, bitPosition, value) ((value) == 1 ? bitSet(x, bitPosition) : bitClear(x, bitPosition))

ModbusSlave::ModbusSlave()
{

}

void ModbusSlave::setUnitAddress(uint8_t unitAddress)
{
	if (unitAddress < MODBUS_ADDRESS_MIN || unitAddress > MODBUS_ADDRESS_MAX)
	{
		return;
	}
	_unitAddress = unitAddress;
}

void ModbusSlave::begin(uint16_t baudRate, bool *coilArray, uint16_t *regArray)
{
	_coilArray = coilArray;
	_regArray = regArray;
	if (baudRate > 19200)
	{
		_halfCharTimeInMicroSecond = 250; // 0.5T.
	}
	else
	{
		_halfCharTimeInMicroSecond = 5000000 / baudRate; // 0.5T.
	}

	// Set the last received time to 3.5T in the future to ignore request currently in the middle of transmission.
	_lastCommunicationTime = GET_TICK + (_halfCharTimeInMicroSecond * MODBUS_FULL_SILENCE_MULTIPLIER);
	HAL_UART_Receive_DMA(&MODBUS_SLAVE, _requestBuffer, sizeof(_requestBuffer));
	MODBUS_SLAVE_RECEIVE;
//	MODBUS_SLAVE_TRANSMIT;
//	HAL_UART_Transmit_IT(&MODBUS_SLAVE, (uint8_t*)"Hello", 5);
	// Sets the request buffer length to zero.

}

uint16_t ModbusSlave::changeBaudrate(uint8_t _number)
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
	HAL_UART_Abort_IT(&MODBUS_SLAVE);
	HAL_UART_DeInit(&MODBUS_SLAVE);
	MODBUS_SLAVE.Init.BaudRate = _baudRate;
	if (HAL_UART_Init(&MODBUS_SLAVE) != HAL_OK) {
	    Error_Handler();
	}
	memset(_requestBuffer, 0, sizeof(_requestBuffer));
	HAL_UART_Receive_DMA(&MODBUS_SLAVE, _requestBuffer, sizeof(_requestBuffer));
	return _baudRate;
}

bool ModbusSlave::isValidAdress(uint16_t address)
{
	return ((address > MODBUS_MAX_BUFFER)? false: true);
}

void ModbusSlave::clearResquestBuffer(void)
{
	_requestBufferLength = 0;
	memset(_requestBuffer, 0, sizeof(_requestBuffer));
	__HAL_DMA_DISABLE(&MODBUS_SLAVE_DMA);
	__HAL_DMA_SET_COUNTER(&MODBUS_SLAVE_DMA, sizeof(_requestBuffer));
	__HAL_DMA_ENABLE(&MODBUS_SLAVE_DMA);
}

void ModbusSlave::resetLength(void)
{
	__HAL_DMA_DISABLE(&MODBUS_SLAVE_DMA);
	__HAL_DMA_SET_COUNTER(&MODBUS_SLAVE_DMA, sizeof(_requestBuffer));
	__HAL_DMA_ENABLE(&MODBUS_SLAVE_DMA);
}
/**
 * Reads a coil state from input buffer.
 *
 * @param offset The offset from the first coil in the buffer.
 * @return The coil state from buffer (true / false).
 */
bool ModbusSlave::readCoilFromBuffer(int offset)
{
	if (_requestBuffer[MODBUS_FUNCTION_CODE_INDEX] == FC_WRITE_COIL)
	{
		if (offset == 0)
		{
			// (2 x coilAddress, 1 x value).
			return readUInt16(_requestBuffer, MODBUS_DATA_INDEX + 2) == COIL_ON;
		}
		return false;
	}
	else if (_requestBuffer[MODBUS_FUNCTION_CODE_INDEX] == FC_WRITE_MULTIPLE_COILS)
	{
		// (2 x firstCoilAddress, 2 x coilsCount, 1 x valueBytes, n x values).
		uint16_t index = MODBUS_DATA_INDEX + 5 + (offset / 8);
		uint8_t bitIndex = offset % 8;

		// Check the offset.
		if (index < _requestBufferLength - MODBUS_CRC_LENGTH)
		{
			return bitRead(_requestBuffer[index], bitIndex);
		}
	}
	return false;
}

/**
 * Reads a register value from input buffer.
 *
 * @param offset The offset from the first register in the buffer.
 * @return The register value from buffer.
 */
uint16_t ModbusSlave::readRegisterFromBuffer(int offset)
{
	if (_requestBuffer[MODBUS_FUNCTION_CODE_INDEX] == FC_WRITE_REGISTER)
	{
		if (offset == 0)
		{
			// (2 x coilAddress, 2 x value).
			return readUInt16(_requestBuffer, MODBUS_DATA_INDEX + 2);
		}
	}
	else if (_requestBuffer[MODBUS_FUNCTION_CODE_INDEX] == FC_WRITE_MULTIPLE_REGISTERS)
	{
		// (2 x firstRegisterAddress, 2 x registersCount, 1 x valueBytes, n x values).
		uint16_t index = MODBUS_DATA_INDEX + 5 + (offset * 2);

		// Check the offset.
		if (index < _requestBufferLength - MODBUS_CRC_LENGTH)
		{
			return readUInt16(_requestBuffer, index);
		}
	}
	return 0;
}

/**
 * Writes the coil state to the output buffer.
 *
 * @param offset The offset from the first coil in the buffer.
 * @param state The state to write into the buffer (true / false).
 */
uint8_t ModbusSlave::writeCoilToBuffer(int offset, bool state)
{
	// Check the function code.
	if (_requestBuffer[MODBUS_FUNCTION_CODE_INDEX] != FC_READ_DISCRETE_INPUT && _requestBuffer[MODBUS_FUNCTION_CODE_INDEX] != FC_READ_COILS)
	{
		return STATUS_ILLEGAL_DATA_ADDRESS;
	}

	// (1 x valueBytes, n x values).
	uint16_t index = MODBUS_DATA_INDEX + 1 + (offset / 8);
	uint8_t bitIndex = offset % 8;

	// Check the offset.
	if (index >= _responseBufferLength - MODBUS_CRC_LENGTH)
	{
		return STATUS_ILLEGAL_DATA_ADDRESS;
	}

	if (state)
	{
		bitSet(_responseBuffer[index], bitIndex);
	}
	else
	{
		bitClear(_responseBuffer[index], bitIndex);
	}

	return STATUS_OK;
}

/**
 * Writes the digital input to output buffer.
 *
 * @param offset The offset from the first input in the buffer.
 * @param state The state to write into the buffer (true / false).
 */
uint8_t ModbusSlave::writeDiscreteInputToBuffer(int offset, bool state)
{
	return writeCoilToBuffer(offset, state);
}

/**
 * Writes the register value to the output buffer.
 *
 * @param offset The offset from the first register in the buffer.
 * @param value The register value to write into buffer.
 */
uint8_t ModbusSlave::writeRegisterToBuffer(int offset, uint16_t value)
{
	// Check the function code.
	if (_requestBuffer[MODBUS_FUNCTION_CODE_INDEX] != FC_READ_HOLDING_REGISTERS && _requestBuffer[MODBUS_FUNCTION_CODE_INDEX] != FC_READ_INPUT_REGISTERS)
	{
		return STATUS_ILLEGAL_DATA_ADDRESS;
	}

	// (1 x valueBytes, n x values).
	uint16_t index = MODBUS_DATA_INDEX + 1 + (offset * 2);

	// Check the offset.
	if ((index + 2) > (_responseBufferLength - MODBUS_CRC_LENGTH))
	{
		return STATUS_ILLEGAL_DATA_ADDRESS;
	}

	_responseBuffer[index] = value >> 8;
	_responseBuffer[index + 1] = value & 0xFF;

	return STATUS_OK;
}

/**
 * Writes an uint8_t array to the output buffer.
 *
 * @param offset The offset from the first data register in the response buffer.
 * @param str The array to write into the response buffer.
 * @param length The length of the array.
 * @return STATUS_OK if succeeded, STATUS_ILLEGAL_DATA_ADDRESS if the data doesn't fit in the buffer.
 */
uint8_t ModbusSlave::writeArrayToBuffer(int offset, uint16_t *str, uint8_t length)
{
	// Index to start writing from (1 x valueBytes, n x values (offset)).
	uint8_t index = MODBUS_DATA_INDEX + 1 + (offset * 2);

	// Check if the array fits in the remaining space of the response.
	if ((index + (length * 2)) > _responseBufferLength - MODBUS_CRC_LENGTH)
	{
		// If not return an exception.
		return STATUS_ILLEGAL_DATA_ADDRESS;
	}

	for (int i = 0; i < length; i++)
	{
		_responseBuffer[index + (i * 2)] = str[i] >> 8;
		_responseBuffer[index + (i * 2) + 1] = str[i] & 0xFF;
	}

	return STATUS_OK;
}

uint8_t ModbusSlave::poll(void)
{
	if (!readRequest())
	{
		return 0;
	}

	// Prepare the output buffer.
	memset(_responseBuffer, 0, MODBUS_MAX_BUFFER);
	_responseBuffer[MODBUS_ADDRESS_INDEX] = _requestBuffer[MODBUS_ADDRESS_INDEX];
	_responseBuffer[MODBUS_FUNCTION_CODE_INDEX] = _requestBuffer[MODBUS_FUNCTION_CODE_INDEX];
	_responseBufferLength = MODBUS_FRAME_SIZE;

	if (!validateRequest())
	{
		clearResquestBuffer();
		return 0;
	}

	// Execute the incoming request and create the response.
	uint8_t status = createResponse();
	if(status != STATUS_OK)
	{
		clearResquestBuffer();
		return 0;
	}

	return writeResponse();
}


uint8_t ModbusSlave::writeResponse(void)
{
//	if(_unitAddress == MODBUS_BROADCAST_ADDRESS)
//		return 0;
	MODBUS_SLAVE_TRANSMIT;
  uint16_t crc = calculateCRC(_responseBuffer, _responseBufferLength - MODBUS_CRC_LENGTH);
  _responseBuffer[_responseBufferLength - MODBUS_CRC_LENGTH] = crc & 0xFF;
  _responseBuffer[(_responseBufferLength - MODBUS_CRC_LENGTH) + 1] = crc >> 8;
	while(HAL_UART_Transmit_IT(&MODBUS_SLAVE, _responseBuffer, _responseBufferLength)!=HAL_OK);
	clearResquestBuffer();
	return 1;
}

uint8_t ModbusSlave::createResponse()
{
	uint16_t firstAddress;
	uint16_t addressesLength;
	uint16_t requestUnitAddress = _requestBuffer[MODBUS_ADDRESS_INDEX];
	uint8_t status = STATUS_OK;
	// Match the function code with a callback and execute it and prepare the response buffer.
	switch (_requestBuffer[MODBUS_FUNCTION_CODE_INDEX])
	{
	case FC_READ_COILS:          // Read coils (digital out state).
	case FC_READ_DISCRETE_INPUT: // Read input state (digital in).
		// Reject broadcast read requests
		if (requestUnitAddress == MODBUS_BROADCAST_ADDRESS)
		{
			return STATUS_ILLEGAL_FUNCTION;
		}

		// Read the first address and the number of inputs.
		firstAddress = readUInt16(_requestBuffer, MODBUS_DATA_INDEX);
		addressesLength = readUInt16(_requestBuffer, MODBUS_DATA_INDEX + 2);
		if(!isValidAdress(firstAddress + addressesLength - 1))
			return STATUS_ADDRESS_OVERFLOW;
		// Calculate the length of the response data and add it to the length of the output buffer.
		_responseBuffer[MODBUS_DATA_INDEX] = (addressesLength / 8) + (addressesLength % 8 != 0);
		_responseBufferLength += 1 + _responseBuffer[MODBUS_DATA_INDEX];

		for (int i = 0; i < addressesLength; i++)
		{
			status = writeCoilToBuffer(i, _coilArray[i + firstAddress]);
		}
		return status;

	case FC_READ_HOLDING_REGISTERS: // Read holding registers (analog out state)
	case FC_READ_INPUT_REGISTERS:   // Read input registers (analog in)
		// Reject broadcast read requests
		if (requestUnitAddress == MODBUS_BROADCAST_ADDRESS)
		{
			return STATUS_ILLEGAL_FUNCTION;
		}

		// Read the first address and the number of inputs.
		firstAddress = readUInt16(_requestBuffer, MODBUS_DATA_INDEX);
		addressesLength = readUInt16(_requestBuffer, MODBUS_DATA_INDEX + 2);
		if(!isValidAdress(firstAddress + addressesLength - 1))
			return STATUS_ADDRESS_OVERFLOW;
		// Calculate the length of the response data and add it to the length of the output buffer.
		_responseBuffer[MODBUS_DATA_INDEX] = 2 * addressesLength;
		_responseBufferLength += 1 + _responseBuffer[MODBUS_DATA_INDEX];

		for (int i = 0; i < addressesLength; i++)
		{
			status = writeRegisterToBuffer(i, _regArray[firstAddress + i]);
		}
		return status;
	case FC_WRITE_COIL: // Write one coil (digital out).
		// Read the address.
		firstAddress = readUInt16(_requestBuffer, MODBUS_DATA_INDEX);
		if(!isValidAdress(firstAddress))
			return STATUS_ADDRESS_OVERFLOW;
		// Add the length of the response data to the length of the output.
		_responseBufferLength += 4;
		// Copy the parts of the request data that need to be in the response data.
		memcpy(_responseBuffer + MODBUS_DATA_INDEX, _requestBuffer + MODBUS_DATA_INDEX, _responseBufferLength - MODBUS_FRAME_SIZE);

		_coilArray[firstAddress] = readCoilFromBuffer(0);
		// Execute the callback and return the status code.
		return STATUS_OK;
	case FC_WRITE_REGISTER: // Write one holding register (analog out).
		// Read the address.
		firstAddress = readUInt16(_requestBuffer, MODBUS_DATA_INDEX);
		if(!isValidAdress(firstAddress))
			return STATUS_ADDRESS_OVERFLOW;
		// Add the length of the response data to the length of the output.
		_responseBufferLength += 4;
		// Copy the parts of the request data that need to be in the response data.
		memcpy(_responseBuffer + MODBUS_DATA_INDEX, _requestBuffer + MODBUS_DATA_INDEX, _responseBufferLength - MODBUS_FRAME_SIZE);
		_regArray[firstAddress] = readRegisterFromBuffer(0);
		// Execute the callback and return the status code.
		return STATUS_OK;
	case FC_WRITE_MULTIPLE_COILS: // Write multiple coils (digital out)
		// Read the first address and the number of outputs.
		firstAddress = readUInt16(_requestBuffer, MODBUS_DATA_INDEX);
		addressesLength = readUInt16(_requestBuffer, MODBUS_DATA_INDEX + 2);
		if(!isValidAdress(firstAddress + addressesLength - 1))
			return STATUS_ADDRESS_OVERFLOW;
		// Add the length of the response data to the length of the output.
		_responseBufferLength += 4;
		// Copy the parts of the request data that need to be in the response data.
		memcpy(_responseBuffer + MODBUS_DATA_INDEX, _requestBuffer + MODBUS_DATA_INDEX, _responseBufferLength - MODBUS_FRAME_SIZE);

		// Execute the callback and return the status code.
		for (int i = 0; i < addressesLength; i++)
		{
			_coilArray[firstAddress + i] = readCoilFromBuffer(i);
		}
		return STATUS_OK;
	case FC_WRITE_MULTIPLE_REGISTERS: // Write multiple holding registers (analog out).
		// Read the first address and the number of outputs.
		firstAddress = readUInt16(_requestBuffer, MODBUS_DATA_INDEX);
		addressesLength = readUInt16(_requestBuffer, MODBUS_DATA_INDEX + 2);
		if(!isValidAdress(firstAddress + addressesLength - 1))
			return STATUS_ADDRESS_OVERFLOW;
		// Add the length of the response data to the length of the output.
		_responseBufferLength += 4;
		// Copy the parts of the request data that need to be in the response data.
		memcpy(_responseBuffer + MODBUS_DATA_INDEX, _requestBuffer + MODBUS_DATA_INDEX, _responseBufferLength - MODBUS_FRAME_SIZE);

		// Execute the callback and return the status code.
		for (int i = 0; i < addressesLength; i++)
		{
			_regArray[firstAddress + i] = readRegisterFromBuffer(i);
		}
		return STATUS_OK;
	default:
		return STATUS_ILLEGAL_FUNCTION;
	}
}

bool ModbusSlave::relevantAddress(uint8_t unitAddress)
{
	// Every device should listen to broadcast messages,
	// keep the check it local, since we provide the unitAddress
	if (unitAddress == MODBUS_BROADCAST_ADDRESS)
	{
		return true;
	}

	// Iterate over all the slaves and check if it listens to the given address, if so return true.
	if (_unitAddress == unitAddress)
	{
		return true;
	}

	return false;
}

bool ModbusSlave::validateRequest()
{

	// Check that the message was addressed to us
	if (!relevantAddress(_requestBuffer[MODBUS_ADDRESS_INDEX]))
	{
		return false;
	}
	// The minimum buffer size (1 x Address, 1 x Function, n x Data, 2 x CRC).
	uint16_t expected_requestBufferSize = MODBUS_FRAME_SIZE;
	bool report_illegal_function = false;

	// Check the validity of the data based on the function code.
	switch (_requestBuffer[MODBUS_FUNCTION_CODE_INDEX])
	{
	case FC_READ_COILS:             // Read coils (digital read).
	case FC_READ_DISCRETE_INPUT:    // Read input state (digital read).
	case FC_READ_HOLDING_REGISTERS: // Read holding registers (analog read).
	case FC_READ_INPUT_REGISTERS:   // Read input registers (analog read).
		// Broadcast is not supported, so ignore this request.
		if (_requestBuffer[MODBUS_ADDRESS_INDEX] == MODBUS_BROADCAST_ADDRESS)
		{
			return false;
		}
		// Add bytes to expected request size (2 x Index, 2 x Count).
		expected_requestBufferSize += 4;
		break;

	case FC_WRITE_COIL:     // Write coils (digital write).
	case FC_WRITE_REGISTER: // Write regosters (digital write).
		// Add bytes to expected request size (2 x Index, 2 x Count).
		expected_requestBufferSize += 4;
		break;

	case FC_WRITE_MULTIPLE_COILS:
	case FC_WRITE_MULTIPLE_REGISTERS:
		// Add bytes to expected request size (2 x Index, 2 x Count, 1 x Bytes).
		expected_requestBufferSize += 5;
		if (_requestBufferLength >= expected_requestBufferSize)
		{
			// Add bytes to expected request size (n x Bytes).
			expected_requestBufferSize += _requestBuffer[6];
		}
		break;

	default:
		// Unknown function code.
		report_illegal_function = true;
	}

	// If the received data is smaller than what we expect, ignore this request.
	if (_requestBufferLength < expected_requestBufferSize)
	{
		return false;
	}

	// Check the crc, and if it isn't correct ignore the request.
	uint16_t crc = readCRC(_requestBuffer, _requestBufferLength);
	if (calculateCRC(_requestBuffer, _requestBufferLength - MODBUS_CRC_LENGTH) != crc)
	{
		return false;
	}

	// report_illegal_function after the CRC check, cheaper
	if (report_illegal_function)
	{
		return false;
	}

	// Set the length to be read from the request to the calculated expected length.
	_requestBufferLength = expected_requestBufferSize;

	return true;
}

uint16_t ModbusSlave::calculateCRC(uint8_t *buffer, int length)
{
	int i, j;
	uint16_t crc = 0xFFFF;
	uint16_t tmp;

	// Calculate the CRC.
	for (i = 0; i < length; i++)
	{
		crc = crc ^ buffer[i];

		for (j = 0; j < 8; j++)
		{
			tmp = crc & 0x0001;
			crc = crc >> 1;
			if (tmp)
			{
				crc = crc ^ 0xA001;
			}
		}
	}

	return crc;
}

bool ModbusSlave::readRequest()
{
	// Read one data packet and report when it's received completely.
	static uint8_t preLength = 0;
	uint16_t length = sizeof(_requestBuffer) - __HAL_DMA_GET_COUNTER(&MODBUS_SLAVE_DMA);
	if (length != preLength)
	{
		_flagEn = true;
		_requestBufferLength += length;
		_lastCommunicationTime = GET_TICK;
		// If we are still reading but no data has been received for 1.5T then this request message is complete.
	}
	if (((GET_TICK - _lastCommunicationTime) > (_halfCharTimeInMicroSecond * MODBUS_HALF_SILENCE_MULTIPLIER)) && _flagEn)
	{
		_flagEn = false;
		preLength = 0;
		// Stop the reading to allow new messages to be read.
		if(_requestBufferLength >= 8 && _requestBufferLength < MODBUS_MAX_BUFFER )
		{
			return true;
		}
		else
		{
			clearResquestBuffer();
			return false;
		}

	}
	preLength = length;
	return false;
}

void ModbusSlave::Modbus_IRQHandler(void)
{
	MODBUS_SLAVE_RECEIVE;
}
void ModbusSlave::Modbus_IRQHandler_Error(void)
{
//	HAL_UART_Abort_IT(&MODBUS_SLAVE);
//	HAL_UART_DeInit(&MODBUS_SLAVE);
//	MODBUS_SLAVE.Init.BaudRate = _baudRate;
//	if (HAL_UART_Init(&MODBUS_SLAVE) != HAL_OK)
//	{
//	    Error_Handler();
//	}
//	memset(_requestBuffer, 0, sizeof(_requestBuffer));
//	HAL_UART_Receive_DMA(&MODBUS_SLAVE, _requestBuffer, sizeof(_requestBuffer));
	memset(_requestBuffer, 0, sizeof(_requestBuffer));
	HAL_UART_DMAStop(&MODBUS_SLAVE);
	HAL_UART_Receive_DMA(&MODBUS_SLAVE, _requestBuffer, sizeof(_requestBuffer));
}
