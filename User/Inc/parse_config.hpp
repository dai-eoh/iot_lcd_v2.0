/*
 * parse_config.hpp
 *
 *  Created on: Jul 22, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_PARSE_CONFIG_HPP_
#define INC_PARSE_CONFIG_HPP_
#ifdef __cplusplus
extern "C" {
#endif
#include "type_list.hpp"
#include "define.hpp"

typedef enum ParseConfig
{
	CONFIG_ID = 1,
	CONFIG_BAUD,
	CONFI_ROW,
	CONFIG_DELAY,
	CONFIG_MODBUS,
	CONFIG_ALARM,
	CONFIG_PHONE,
	CONFIG_ISWIFI,
	CONFIG_WIFI
}ParseConfig;

typedef enum StateConfig
{
	CONTROL,
	CONFIG
}StateConfig;
void parseConfig(char * _strConfig, ModbusConfig * _modbusConfig, NetworkData * _networkData, uint8_t _state);


#ifdef __cplusplus
}
#endif
#endif /* INC_PARSE_CONFIG_HPP_ */
