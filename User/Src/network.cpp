/*
 * network.cpp
 *
 *  Created on: Jul 21, 2021
 *      Author: EoH.Dai
 */
#include "network.hpp"
#include "variable_network.hpp"

void ADC_Start(void)
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcValue, NUM_ADC);
}

uint16_t Network_GetCPUTemp(void)
{
	return (adcValue[0]*(3.3f/4095) - 0.76f)*100.f/0.0025 + 2500;
}
uint16_t Network_GetVoltage(void)
{
	return ((adcValue[1]*3.3f*20.2f)/(4095*2.2f))*100;
}

int8_t getSignalQuality(uint8_t _device)
{
	if(_device == SIM)
		networkSignal =  Sim800C.getSignalQuality();
	else
		networkSignal = Esp32._rssiWifi;
	return networkSignal;
}

static void fetchNewConfig(uint8_t _device)
{
	char _configURL[200] = {0};
	char _dataConfig[1024] = {0};
	strcat(_configURL, baseReadConfigUrl);
	strcat(_configURL, strlen(networkData.configID) ? networkData.configID : "1");
	if(_device == SIM)
		Sim800C.requestGet(_configURL, _dataConfig);
	else
		Esp32.requestGetESP(MAIN_DOMAIN, _configURL, _dataConfig);
	parseConfig(_dataConfig, modbusConfig, &networkData, CONFIG);
	//save wifi
	Esp32.getWifiConfig(networkData);
	//saveConfig & modbus
	ExFlash_WriteData(modbusConfig, MODBUSCONFIG_ADDRESS, sizeof(modbusConfig));
	ExFlash_WriteData(&networkData, NETWORKCONFIG_ADDRESS, sizeof(networkData));
	LCD_CopyModbusConfig(modbusRTU ,modbusConfig);
}

static void fetchNewControlWrite(uint8_t _device)
{
	char _configURL[200] = {0};
	char _dataConfig[1024] = {0};
	ModbusConfig _modbusControl[NUM_MODBUS] = {0};
	uint8_t _flagQueue = 1;
	strcat(_configURL, baseReadControlUrl);
	strcat(_configURL, strlen(networkData.controlID) ? networkData.configID : "1");
	if(_device == SIM)
		Sim800C.requestGet(_configURL, _dataConfig);
	else
		Esp32.requestGetESP(MAIN_DOMAIN, _configURL, _dataConfig);
	parseConfig(_dataConfig, _modbusControl, &networkData, CONTROL);
	ExFlash_WriteData(&networkData, NETWORKCONFIG_ADDRESS, sizeof(networkData));
	LCD_CopyModbusConfig(modbusControl ,_modbusControl);
	osMessageQueuePut(fetchControlQueueHandle, &_flagQueue, 0U, 0U);
}

static void uploadNewFirmware()
{
	firmwareStatus = UPDATE_FIRMWARE_CODE;
	InFlash_EraseSector(FLASH_SECTOR_11);
	InFlash_Write(INFLASH_INFO_FIRMWARE, &firmwareStatus, 1);
}

static void pushDataToServer(uint8_t _device)
{
	uint8_t _flagQueue = 0;
	char _dataModbus[200] = {0};
	char _dataPush[2048] = {0};
	char _ack[20] = {0};
	char _configURL[1024] = {0};
	uint8_t _pos = 0;
	if(osMessageQueueGet(modbusNetworkQueueHandle, &_flagQueue, NULL, 1) == osOK)
	{
		for(int i = 0; i < networkData.numConfig; i++)
		{
			for(int j = 0; j < modbusRTU[i].length; j++)
			{
				sprintf(&_dataModbus[strlen(_dataModbus)], "%04x", valueModbus[j + _pos]);
			}
			_pos += modbusRTU[i].length;
			sprintf(&_ack[strlen(_ack)], "%1d", modbusRTU[i].failFlag & 0x01);
		}
	}
//	if(!strlen(_dataModbus) && networkData.numConfig )
//		return;
	strcat(_dataPush, _dataModbus);
	uint16_t interTemp = (adcValue[0]*(3.3f/4095) - 0.76f)*100.f/0.0025 + 2500;
	strcat(_dataPush, "&it=");
	sprintf(&_dataPush[strlen(_dataPush)], "%04d", interTemp);
	strcat(_dataPush, "&t=");
	sprintf(&_dataPush[strlen(_dataPush)], "%d", 0);
	strcat(_dataPush, "&v=");
	sprintf(&_dataPush[strlen(_dataPush)], "%d", (adcValue[1]*24)/40);
	strcat(_dataPush, "&s=");
	sprintf(&_dataPush[strlen(_dataPush)], "%d", getSignalQuality(_device));
	strcat(_dataPush, "&mt=");
	sprintf(&_dataPush[strlen(_dataPush)], "%d", modbusTotal);
	strcat(_dataPush, "&mf=");
	sprintf(&_dataPush[strlen(_dataPush)], "%d", modbusFailrate);
	strcat(_dataPush, "&wfiu=");
	strcat(_dataPush, Esp32.ssidLast);
//	strcat(data_send_str, "&cpu=");
//	sprintf(&data_send_str[strlen(data_send_str)], "%d", _cpuLoad);
	strcat(_dataPush, "&ack=");
	strcat(_dataPush, _ack);

	strcat(_configURL, basePostDataUrl);
	strcat(_configURL, strlen(networkData.configID) ? networkData.configID : "1");
	strcat(_configURL, "&control_id=");
	strcat(_configURL, strlen(networkData.controlID) ? networkData.controlID : "1");
//	strcat(_configURL, "&bluetooth_config_id=");
//	strcat(_configURL, strlen(bluetoothConfig.readId) ? bluetoothConfig.readId : "1");

	if (interchangeCount++ > 99)
	{
		interchangeCount = 1;
		isFirstInterchange = 0;
	}
	strcat(_configURL, "&interchange_count=");
	sprintf(&_configURL[strlen(_configURL)], "%d", interchangeCount);

	// first run identify
	strcat(_configURL, "&is_first_interchange=");
	sprintf(&_configURL[strlen(_configURL)], "%d", isFirstInterchange);

//	if(!autoSwitch)
//		autoSwitch = Esp32.checkWifiConnected(MAIN_DOMAIN);
//	_device = autoSwitch;
	if(_device == SIM)
		responseInt = Sim800C.requestPost(_configURL, _dataPush);
	else
		responseInt = Esp32.requestPostESP(MAIN_DOMAIN, _configURL, _dataPush);
	switch (responseInt)
	{
	case -2:	// network error
		Sim800C.setStepConn(0);
		break;
	case -4:
		Esp32.restart();
		autoSwitch = 0;
		break;
	case -3:
	case -1:
		numberOfFailInterchange += 1;
		if (numberOfFailInterchange >= INTERCHANGE_FAIL_RATE)
		{
			numberOfFailInterchange = 0;
			if(autoSwitch)
				autoSwitch = Esp32.initWifi(SSID_DEFAULT, PASSWORD_DEFAULT, MAIN_DOMAIN);
			else
				Sim800C.restart();
		}
		break;

	case 1:
		fetchNewConfig(_device);
		break;

	case 2:
//		if (RTC_TimeStruct.Hours == 0 && RTC_TimeStruct.Minutes < 10)
//			storeReasonReset(RTC_TimeStruct, RTC_DateStruct, DAILY_RESET);
//		else
//			storeReasonReset(RTC_TimeStruct, RTC_DateStruct, MISS_COMMUNICATION_RESET);
//		resetChip();
		break;

//	case 3:
//		debugSendSMS = 1;
//		fetchAndSendSMS();
//		break;
	case 4:
		fetchNewControlWrite(_device);
		break;
	case 6:
		uploadNewFirmware();
		osDelay(100);
		resetChip();
		break;
//	case 6:
//		update_firmware();
//		osDelay(1000);
//		resetCpuAndSim();
//		break;

//	case 8:
//		fetchNewBluetoothConfig();
//		break;

//	case 9:
//		fetchAndDoAction();
//		break;

	default:
		break;
	}
}

static void initURL(void)
{

	memset(baseInfo, 0, sizeof(baseInfo));
	memset(baseReadConfigUrl, 0, sizeof(baseReadConfigUrl));
	memset(baseReadControlUrl, 0, sizeof(baseReadControlUrl));
	memset(basePostDataUrl, 0, sizeof(basePostDataUrl));
	memset(baseRegisterUrl, 0, sizeof(baseRegisterUrl));

	strcat(baseInfo, "&version=");
	strcat(baseInfo, VERSION);
	strcat(baseInfo, "&firm_version=");
	strcat(baseInfo, FIRMWARE_VERSION);
	strcat(baseInfo, "&board=");
	strcat(baseInfo, BOARD);
	strcat(baseInfo, "&format=json");

	strcat(baseReadConfigUrl, "http://");
	strcat(baseReadConfigUrl, MAIN_DOMAIN);
	strcat(baseReadConfigUrl, "/chip_manager/config?code=");
	strcat(baseReadConfigUrl, networkData.chipImei + 2); // ingore first 2 char
	strcat(baseReadConfigUrl, baseInfo);
	strcat(baseReadConfigUrl, "&id=");


	strcat(baseReadControlUrl, "http://");
	strcat(baseReadControlUrl, MAIN_DOMAIN);
	strcat(baseReadControlUrl, "/chip_manager/control?code=");
	strcat(baseReadControlUrl, networkData.chipImei + 2); // ingore first 2 char
	strcat(baseReadControlUrl, baseInfo);
	strcat(baseReadControlUrl, "&control_id=");


	strcat(basePostDataUrl, "http://");
	strcat(basePostDataUrl, MAIN_DOMAIN);
	strcat(basePostDataUrl, "/chip_manager/sensor_data?code=");
	strcat(basePostDataUrl, networkData.chipImei + 2);
	strcat(basePostDataUrl, baseInfo);
	strcat(basePostDataUrl, "&id=");

	strcat(baseRegisterUrl, "http://");
	strcat(baseRegisterUrl, MAIN_DOMAIN);
	strcat(baseRegisterUrl, "/chip_manager/register_chip?");
	strcat(baseRegisterUrl, baseInfo);
}

static bool checkImei(const char * _imei)
{
	if(strlen(_imei) < 3)
		return false;
	if(_imei[0] == 'I' && _imei[1] == 'M')
		return true;
	return false;
}


static void registerImei(uint8_t _device)
{
	char _imei[200] = {0};
	networkStatus = 4;
	if(_device == SIM)
		Sim800C.requestGet(baseRegisterUrl, _imei);
	else
		Esp32.requestGetESP(MAIN_DOMAIN, baseRegisterUrl, _imei);
	if(checkImei(_imei))
	{
		strcpy(networkData.chipImei, _imei);
		ExFlash_WriteData(&networkData, NETWORKCONFIG_ADDRESS, sizeof(networkData));
		initURL();
	}
}

void Network_Init(void)
{
	//get firmware status
	InFlash_Read(INFLASH_INFO_FIRMWARE, &firmwareStatus, 1);
	Sim800C.init();
//	Esp32.getWifiConfig(networkData);
//	autoSwitch = Esp32.initWifi(SSID_DEFAULT, PASSWORD_DEFAULT, MAIN_DOMAIN);

	autoSwitch = 0;
	initURL();
}

void Network_Poll(void)
{
	HAL_IWDG_Refresh(&hiwdg);
	if(HAL_GetTick() - preTimeNetWork < TICK_PERIOD)
	{
		osDelay(10);
		return;
	}
	preTimeNetWork = HAL_GetTick();
	if (autoSwitch)
	{
		networkStatus = Esp32.getWifiStatus();
		if (checkImei(networkData.chipImei))
			pushDataToServer(WIFI);
		else
			registerImei(WIFI);
	}
	else
	{
		stepConn = Sim800C.getStepConn();
		networkStatus = stepConn;
		switch (stepConn)
		{
		case CHECK_SIM:
			Sim800C.checkSimStatus();
			break;
		case CHECK_SIGNAL:
			Sim800C.checkSignal();
			break;
		case CHECK_GPRS:
			Sim800C.checkGPRS();
			break;
		case CHECK_SERVER:
			getSignalQuality(SIM);
			if (!Sim800C.checkGPRSIfFail())
				break;
			if (checkImei(networkData.chipImei))
				pushDataToServer(SIM);
			else
				registerImei(SIM);
			//push data
			break;
		}
	}
}
