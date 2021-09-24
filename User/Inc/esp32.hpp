/*
 * esp32.hpp
 *
 *  Created on: Jul 22, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_ESP32_HPP_
#define INC_ESP32_HPP_

#include "define.hpp"
#include <vector>
#include <algorithm>
#include <vector>
#include <functional>
#include <string>
#include "type_list.hpp"
#define MAX_BUFFER	4096
#define ESP_UART	huart2
#define ESP_DMA		hdma_usart2_rx
#define SEND_UART(huart, command, length)	while(HAL_UART_Transmit_IT(huart, command, length) != HAL_OK)
#define MAX_TICK  700


using namespace std;
typedef struct
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
}clockTime;

enum ResultWifi
{
	WIFI_SWITCH = -4,
	WIFI_DICONNECT = -3,
	WIFI_ERROR = -1,
	FAIL_RESPONSE = 0,
	SUCCESSFUL = 1,
	NEED_CONNECT = 2,
	WIFI_EXIST = 3,
	RECIPSTART = 4
};

enum statusRsp
{
	RSP_MORE_DATA = -4,
	RSP_RECIPSTART = -3,
	RSP_NONECT = -2,
	RSP_NODATA = -1
};

class Esp32
{
public:
	Esp32();
	~Esp32() {}
	bool initWifi(const char* ssid, const char* password, const char * domain);
	void getTimeSNTP(void);
	bool requestGetESP(const char* domain, char* url, char* data);
	int8_t requestPostESP(const char* domain, char* url, char* data);
	void Esp32_IRQHandler_Error(void);
	void restart(void);
	bool checkWifiConnected(const char * domain);
	char ssidLast[100];
	char passLast[100];
	void getWifiConfig(NetworkData _networkData);
	uint8_t getWifiStatus(void);
	clockTime _time;
	int16_t _rssiWifi;
private:
	uint8_t buffer[MAX_BUFFER];

	ResultWifi initNetworkWifi(const char* ssid, const char* password, const char * domain);
	ResultWifi connectToWifi(const char* ssid, const char* pass);
	ResultWifi startSSLServer(const char* domain, bool isMux = false);
	ResultWifi waitAtResponse(const char* message, const char* messRsp, char* data = nullptr, bool check = true);
	void ESP(const char *data);
	bool checkDigitBegin(const std::string& str);
	bool clearSpaceDouble(std::string& str);
	int8_t readRSSI(char* str);
	uint16_t lengthIPD(char* str);
	int8_t checkHTTP(const std::string& str, char* data, uint16_t& trueLen);
	int8_t statusCodeHTTP(const std::string& str, const std::string& code, char* data, uint16_t& trueLen);
	int8_t checkConnection(const std::string& str);
	int8_t contentTextHtml(const std::string& str, char* data);
	int8_t contentHTTP(const std::string& str, char* data, uint16_t& trueLen);
	int8_t responseHTTP(const std::string& str, char* data, uint16_t len);
	int8_t getMoreDataHTTP(const char* str, char* data, uint16_t len, uint16_t& trueLen);
	int8_t parseTimeSNTP(char* str, clockTime& _timenow);
	uint8_t getWeekDay(const std::string& str);
	uint8_t getMonth(const std::string& str);
	void clearBuffer(void);
	bool checkDigit(const std::string& str);
	unsigned int splitString(const std::string& str_input, std::vector<std::string>& ret_list, char ch);
	std::string _strResponse;
	bool _enCheck;
	bool _isDebug;
	uint8_t _countFail;
	uint8_t _interCount;
	bool _isFirstInter;
	uint8_t _countCheck;
	char ssidWifi[100];
	char passWifi[100];
	char cipSend[100];
	char finalUrl[1024];
	char rspCode[1024];
	uint8_t _wifiStatus = 0;
};


#endif /* INC_ESP32_HPP_ */
