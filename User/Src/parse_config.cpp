/*
 * parse_config.cpp
 *
 *  Created on: Jul 22, 2021
 *      Author: EoH.Dai
 */
#include "parse_config.hpp"

static uint8_t countConfig = 0;

void actOneConfigSensorReadWrite(int * _params, int _length, ModbusConfig * _modbusConfig){
	/*
	params is an array with index
	0: sensor address
	1: database ID
	2: function
	3: sa1
	4: sa2
	5: len1
	6: len2
	7: extra
	*/


	_modbusConfig[countConfig].addr = _params[0];
//	modbusConfig.configId = _params[1];
	_modbusConfig[countConfig].func = _params[2];
	_modbusConfig[countConfig].regId = _params[3]*256 + _params[4];
	_modbusConfig[countConfig].length = _params[5]*256 + _params[6];
	countConfig++;

}


void parseOneConfigSensorReadWrite(char * _config, int _length, ModbusConfig * _modbusConfig) {

	char _temp[10];
	int _tempIndex = 0;
	int _tempConfigIndex = 0;
	int _tempConfigParam[20];

	for (int i = 0; i < _length; i++) {
		_temp[_tempIndex++] = _config[i];
		if (_config[i] == ',') {
			_temp[_tempIndex - 1] = 0;
			_tempConfigParam[_tempConfigIndex++] = atoi(_temp);
			_tempIndex = 0;
		}
	}
	actOneConfigSensorReadWrite(_tempConfigParam, _tempConfigIndex, _modbusConfig);
}


void processParseConfigSensorReadWrite(char * _config, int _length, ModbusConfig * _modbusConfig) {
	int _indexRead = 0;
	for(int i = 0; i < _length; i++) {
		if (_config[i] == '.') {
			parseOneConfigSensorReadWrite(_config + _indexRead, i - _indexRead, _modbusConfig);
			_indexRead = i + 1;
		}
	}
}

void processParseConfigWifi(char * _config, int _length, NetworkData * _networkData) {
	uint8_t _getSsidPass = 0;
	int _posSsid = 0, _posPass = 0;

	for (int i = 0; i < _length; i++) {
		if(_config[i] == ',') {
			_getSsidPass = 1;
			continue;
		}

		if (_getSsidPass == 0) {
			_networkData->ssid[_posSsid++] = _config[i];
		} else {
			_networkData->password[_posPass++] = _config[i];
		}
	}
	_networkData->ssid[_posSsid] = '\0';
	_networkData->password[_posPass] = '\0';
}

static void processParseConfig(uint8_t _part, char * _strConfig, uint8_t _length, ModbusConfig * _modbusConfig, NetworkData * _networkData, uint8_t _state)
{
	if(!strlen(_strConfig))
		return;
	countConfig = 0;
	switch(_part)
	{
	case CONFIG_ID:
		if(_state == CONTROL)
		{
			memcpy(_networkData->controlID, _strConfig, _length);
			_networkData->controlID[_length] = 0;
		}
		else if(_state == CONFIG)
		{
			memcpy(_networkData->configID, _strConfig, _length);
			_networkData->configID[_length] = 0;
		}
		break;
	case CONFIG_BAUD:
		_networkData->baudrate = atoi(_strConfig);
		break;
	case CONFIG_DELAY:
		break;
	case CONFIG_MODBUS:
		if(_state == CONTROL)
		{
			processParseConfigSensorReadWrite(_strConfig, _length, _modbusConfig);
			_networkData->numControl = countConfig;
		}
		else if(_state == CONFIG)
		{
			processParseConfigSensorReadWrite(_strConfig, _length, _modbusConfig);
			_networkData->numConfig = countConfig;
		}

		break;
	case CONFIG_WIFI:
		processParseConfigWifi(_strConfig, _length, _networkData);
		break;
	default:
		break;
	}
}

void parseConfig(char * _strConfig, ModbusConfig * _modbusConfig, NetworkData * _networkData, uint8_t _state)
{
	if(!strlen(_strConfig))
		return;
	uint16_t _length = strlen(_strConfig);
	uint8_t _part = 0, _start = 0;
	for (int i = 0; i < _length; i++) {
		if (_strConfig[i] == ';') {
			_part += 1;
			processParseConfig(_part, _strConfig + _start, i - _start, _modbusConfig, _networkData, _state);
			_start = i + 1;
		}
	}
}

