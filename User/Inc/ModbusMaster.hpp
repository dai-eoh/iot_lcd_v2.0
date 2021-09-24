/*
 * ModbusMaster.hpp
 *
 *  Created on: Apr 1, 2021
 *      Author: daitr
 */

/*
 * ModbusMaster.hpp
 *
 *  Created on: Apr 1, 2021
 *      Author: daitr
 */

#ifndef INC_MODBUSRTUMASTER_HPP_
#define INC_MODBUSRTUMASTER_HPP_

#include "main.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "define.hpp"
#include "type_list.hpp"

#define USE_RTOS_MBRTU
#define MODBUS_MASTER			huart6
#define MODBUS_MASTER_DMA hdma_usart6_rx

#define highByte(x) ( (x) >> (8) ) 	 // keep upper 8 bits
#define lowByte(x) 	( (x) & (0xff) ) // keep lower 8 bits
#define word(h,l)   ((((h) & 0xff) << 8) + (l))
//#define USE_RTOS_MBRTU 1
#define TRANS_MODBUS		HAL_GPIO_WritePin(RS485_DE1_GPIO_Port, RS485_DE1_Pin, GPIO_PIN_SET)
#define RECEI_MODBUS		HAL_GPIO_WritePin(RS485_DE1_GPIO_Port, RS485_DE1_Pin, GPIO_PIN_RESET)
#ifdef USE_RTOS_MBRTU
#include "cmsis_os.h"
#endif


class ModbusMaster
{
  public:
    uint8_t failRate = 0;
    uint8_t modbusTotal = 0;
    ModbusMaster();
    void begin(void);
    uint16_t getDataModbus(int _number);
    void     clearDataModbus();
    uint8_t getModbusFail(void);
    uint8_t getModbusTotal(void);
    bool modbusMasterTransaction(ModbusRTU* modbusRTU);
    void changeBaudrate(uint8_t _number);
    void Modbus_IRQHandler(void);
    void Modbus_IRQHandler_Error(void);
  private:
    uint32_t _baudRate = 9600;
    uint8_t  _slaveAddress;                                         ///< Modbus slave (1..255) initialized in begin()
    uint8_t  _function;
    static const uint8_t MaxBufferSize                = 64;   ///< size of response/transmit buffers
    uint16_t _readAddress;                                    ///< slave register from which to read
    uint16_t _readQty;                                        ///< quantity of words to read
    uint8_t _responseBuffer[MaxBufferSize];               ///< buffer to store Modbus slave response; read via GetResponseBuffer()
    uint16_t _dataBuffer[MaxBufferSize/2];
    uint16_t _writeAddress;                                   ///< slave register to which to write
    uint16_t _writeQty;                                       ///< quantity of words to write
    uint16_t _transmitBuffer[MaxBufferSize];               ///< buffer containing data to transmit to Modbus slave; set via SetTransmitBuffer()
    uint8_t _transmitBufferIndex;
    uint16_t _transmitBufferLength;
    uint8_t _responseBufferIndex;
    uint8_t _dataBufferLength;
    uint16_t _u16CRC;
    // Modbus function codes for bit access
    static const uint8_t ReadCoils                  = 0x01; ///< Modbus function 0x01 Read Coils
    static const uint8_t ReadDiscreteInputs         = 0x02; ///< Modbus function 0x02 Read Discrete Inputs
    static const uint8_t WriteSingleCoil            = 0x05; ///< Modbus function 0x05 Write Single Coil
    static const uint8_t WriteMultipleCoils         = 0x0F; ///< Modbus function 0x0F Write Multiple Coils

    // Modbus function codes for 16 bit access
    static const uint8_t ReadHoldingRegisters       = 0x03; ///< Modbus function 0x03 Read Holding Registers
    static const uint8_t ReadInputRegisters         = 0x04; ///< Modbus function 0x04 Read Input Registers
    static const uint8_t WriteSingleRegister        = 0x06; ///< Modbus function 0x06 Write Single Register
    static const uint8_t WriteMultipleRegisters     = 0x10; ///< Modbus function 0x10 Write Multiple Registers

    // Modbus timeout [milliseconds]
    static const uint16_t TickTimeout_1          = 150; ///< Modbus timeout [milliseconds] * 10
    static const uint16_t TickTimeout_2          = 50; ///< Modbus timeout [milliseconds] * 10
    // master function that conducts Modbus transactions
    uint16_t calculateCRC(uint8_t* buffer, uint8_t length);
    bool waitModbusResponse(void);
    bool checkCRCReceive(void);
    void modbusFail(ModbusRTU* modbusRTU);
    void getDataModbus(void);
    void clearResponseBuffer(void);
    void sendCommand(uint8_t *string, uint8_t length);
};

#endif /* INC_MODBUSRTUMASTER_HPP_ */

