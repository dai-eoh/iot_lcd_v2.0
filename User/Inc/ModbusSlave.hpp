/*
 * ModbusSlave.hpp
 *
 *  Created on: May 4, 2021
 *      Author: daitr
 */

#ifndef INC_MODBUSSLAVE_HPP_
#define INC_MODBUSSLAVE_HPP_

#include "main.h"
#include "define.hpp"
#include "string.h"

#define MODBUS_MAX_BUFFER 256
#define MODBUS_INVALID_UNIT_ADDRESS 255
#define MODBUS_DEFAULT_UNIT_ADDRESS 1
#define MODBUS_SLAVE	huart1
#define MODBUS_SLAVE_DMA	hdma_usart1_rx
#define MODBUS_SLAVE_TRANSMIT	HAL_GPIO_WritePin(RS485_DE2_GPIO_Port, RS485_DE2_Pin, GPIO_PIN_SET)
#define MODBUS_SLAVE_RECEIVE	HAL_GPIO_WritePin(RS485_DE2_GPIO_Port, RS485_DE2_Pin, GPIO_PIN_RESET)

enum
{
  FC_INVALID = 0,
  FC_READ_COILS = 1,
  FC_READ_DISCRETE_INPUT = 2,
  FC_READ_HOLDING_REGISTERS = 3,
  FC_READ_INPUT_REGISTERS = 4,
  FC_WRITE_COIL = 5,
  FC_WRITE_REGISTER = 6,
  FC_WRITE_MULTIPLE_COILS = 15,
  FC_WRITE_MULTIPLE_REGISTERS = 16
};

enum
{
  CB_READ_COILS = 0,
  CB_READ_DISCRETE_INPUTS,
  CB_READ_HOLDING_REGISTERS,
  CB_READ_INPUT_REGISTERS,
  CB_WRITE_COILS,
  CB_WRITE_HOLDING_REGISTERS,
  CB_MAX
};

enum
{
  COIL_OFF = 0x0000,
  COIL_ON = 0xff00
};

enum
{
  STATUS_OK = 0,
  STATUS_ILLEGAL_FUNCTION,
  STATUS_ILLEGAL_DATA_ADDRESS,
  STATUS_ILLEGAL_DATA_VALUE,
  STATUS_SLAVE_DEVICE_FAILURE,
  STATUS_ACKNOWLEDGE,
  STATUS_SLAVE_DEVICE_BUSY,
  STATUS_NEGATIVE_ACKNOWLEDGE,
  STATUS_MEMORY_PARITY_ERROR,
  STATUS_GATEWAY_PATH_UNAVAILABLE,
  STATUS_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND,
	STATUS_ADDRESS_OVERFLOW
};

class ModbusSlave
{
	public:
		ModbusSlave();
	  void setUnitAddress(uint8_t unitAddress);
	  uint8_t poll();
	  void begin(uint16_t baudRate, bool* coilArray, uint16_t* regArray);
    void Modbus_IRQHandler(void);
    void Modbus_IRQHandler_Error(void);
    void clearResquestBuffer(void);
    uint16_t changeBaudrate(uint8_t _number);
	private:
	  uint8_t _unitAddress = MODBUS_DEFAULT_UNIT_ADDRESS;
	  uint32_t _baudRate = 9600;
	  uint16_t _halfCharTimeInMicroSecond;
	  uint64_t _lastCommunicationTime;

	  uint8_t _requestBuffer[MODBUS_MAX_BUFFER] = {0};
	  uint16_t _requestBufferLength = 0;
	  bool _isRequestBufferReading = false;

	  uint8_t _responseBuffer[MODBUS_MAX_BUFFER] = {0};
	  uint16_t _responseBufferLength = 0;
	  bool _isResponseBufferWriting = false;
	  uint16_t _responseBufferWriteIndex = 0;

	  uint64_t _totalBytesSent = 0;
	  uint64_t _totalBytesReceived = 0;

	  bool* _coilArray = nullptr;
	  uint16_t* _regArray = nullptr;;
		bool _flagEn = false;
	  bool relevantAddress(uint8_t unitAddress);
	  bool readRequest();
	  bool validateRequest();
	  uint8_t createResponse();
	  uint8_t executeCallback(uint8_t slaveAddress, uint8_t callbackIndex, uint16_t address, uint16_t length);
	  uint16_t reportException(uint8_t exceptionCode);
	  uint16_t calculateCRC(uint8_t *buffer, int length);


	  bool readCoilFromBuffer(int offset);
	  uint16_t readRegisterFromBuffer(int offset);
	  uint8_t writeExceptionStatusToBuffer(int offset, bool status);
	  uint8_t writeCoilToBuffer(int offset, bool state);
	  uint8_t writeDiscreteInputToBuffer(int offset, bool state);
	  uint8_t writeRegisterToBuffer(int offset, uint16_t value);
	  uint8_t writeArrayToBuffer(int offset, uint16_t *str, uint8_t length);
	  void resetLength(void);
	  uint8_t writeResponse(void);
	  bool isValidAdress(uint16_t address);
};
#endif /* INC_MODBUSSLAVE_HPP_ */
