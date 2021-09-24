/*
 * variable_process.hpp
 *
 *  Created on: Jul 15, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_VARIABLE_PROCESS_HPP_
#define INC_VARIABLE_PROCESS_HPP_

#include <string.h>
#include <math.h>
#include "sim800C.hpp"
#include "main.h"
#include "define.hpp"
#include "type_list.hpp"
#include "ModbusMaster.hpp"
#include "ModbusSlave.hpp"
#include "sub_function.hpp"
#include "esp32.hpp"
#include "Kalman.h"

#define TIME_CALIB 5000

extern ModbusConfig modbusConfig[NUM_MODBUS];
extern ModbusRTU	modbusControl[NUM_MODBUS];
extern CalibConfig  calibConfig[NUM_MODBUS];
extern SensorConfig sensorConfig[NUM_MODBUS];
extern AlarmConfig	alarmConfig[NUM_MODBUS];
extern AdvancedConfig advancedConfig[NUM_MODBUS];
extern DeviceConfig deviceConfig;
extern WriteConfig writeConfig;
extern NetworkData networkData;
extern Sim800C	Sim800C;
extern Esp32		Esp32;


ModbusMaster ModbusMaster;
ModbusSlave  ModbusSlave;
uint16_t valueModbus[NUM_MODBUS*2] = {0};
ModbusRTU modbusRTU[NUM_MODBUS] = {0};
ModbusRTU	modbusControl[NUM_MODBUS] = {0};
float processValue[NUM_MODBUS] = {0};
ProcessData processData[NUM_MODBUS] = { 0 };
uint16_t registerArray[MODBUS_MAX_BUFFER] = {0};
bool coilArray[MODBUS_MAX_BUFFER] = {0};
LastResetReason lastResetReason = RESET_CAUSE_UNKNOWN;

uint8_t modbusTotal = 0;
uint8_t modbusFailrate = 0;
#endif /* INC_VARIABLE_PROCESS_HPP_ */
