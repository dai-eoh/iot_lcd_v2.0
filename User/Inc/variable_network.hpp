/*
 * variable_network.hpp
 *
 *  Created on: Jul 21, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_VARIABLE_NETWORK_HPP_
#define INC_VARIABLE_NETWORK_HPP_

#include <spi_flash.hpp>
#include "define.hpp"
#include "sim800C.hpp"
#include "ModbusMaster.hpp"
#include "sub_function.hpp"
#include "parse_config.hpp"
#include "lcd_display.hpp"
#include "esp32.hpp"
#include "Kalman.h"
#include "cpu_utils.h"
#include "internal_flash.hpp"


#define VERSION							"5"
#define FIRMWARE_VERSION		"1.5"
#define BOARD								"STM32"
#define MAIN_DOMAIN					"nossl.iotwhynot.com"
#define INTERCHANGE_FAIL_RATE 20
#define TICK_PERIOD					1000
#define SSID_DEFAULT				"wifi_test"
#define PASSWORD_DEFAULT		"Eoh@2020"


typedef enum SimState
{
	CHECK_SIM = 0,
	CHECK_SIGNAL,
	CHECK_GPRS,
	CHECK_SERVER
}SimState;

typedef enum Device
{
	SIM=0,
	WIFI=1
}Device;

Sim800C	Sim800C;
Esp32 	Esp32;
uint16_t adcValue[NUM_ADC] = {0};
uint8_t stepConn = 0;
NetworkData networkData = {0};
char baseInfo[200] = {0};
char baseReadConfigUrl[200] = {0};
char baseReadControlUrl[200] = {0};
char basePostDataUrl[200] = {0};
char baseRegisterUrl[200] = {0};
uint8_t interchangeCount = 0;
uint8_t isFirstInterchange = 1;
int8_t  responseInt = 0;
uint8_t numberOfFailInterchange = 0;
uint8_t autoSwitch = 0;
uint8_t networkStatus = 0;
uint32_t preTimeNetWork = 0;
int8_t networkSignal = 0;
uint32_t firmwareStatus = 0;

extern ModbusMaster ModbusMaster;
extern ModbusRTU modbusRTU[NUM_MODBUS];
extern ModbusConfig modbusConfig[NUM_MODBUS];
extern ModbusRTU	modbusControl[NUM_MODBUS];
extern uint16_t valueModbus[NUM_MODBUS*2];
extern uint8_t modbusTotal;
extern uint8_t modbusFailrate;
#endif /* INC_VARIABLE_NETWORK_HPP_ */
