/*
 * esp32.cpp
 *
 *  Created on: Jul 22, 2021
 *      Author: EoH.Dai
 */

#include "esp32.hpp"
#include "network.hpp"
#include "type_list.hpp"


Esp32::Esp32():
_rssiWifi(0),
_enCheck(false),
_isDebug(false),
_interCount(0),
_isFirstInter(true),
_countCheck(0)
{
	memset(buffer, 0, sizeof(buffer));
	memset(ssidWifi, 0, sizeof(ssidWifi));
	memset(passWifi, 0, sizeof(passWifi));
	memset(ssidLast, 0, sizeof(ssidLast));
	memset(passLast, 0, sizeof(passLast));
	memset(rspCode, 0, sizeof(rspCode));
}

void Esp32::ESP(const char *data)
{
	SEND_UART(&ESP_UART, reinterpret_cast<uint8_t*>(const_cast<char*>(data)), strlen(data));
}

uint8_t Esp32::getWifiStatus(void)
{
	return _wifiStatus;
}

void Esp32::restart(void)
{
	HAL_GPIO_WritePin(WIFI_PW_GPIO_Port, WIFI_PW_Pin, GPIO_PIN_RESET);
	osDelay(1000);
	HAL_GPIO_WritePin(WIFI_PW_GPIO_Port, WIFI_PW_Pin, GPIO_PIN_SET);
	osDelay(1000);
}

ResultWifi Esp32::startSSLServer(const char* domain, bool isMux)
{
	if(isMux)
		waitAtResponse("AT+CIPMUX=1\r\n", "OK");
	waitAtResponse("AT+CIPCLOSE=0\r\n", "OK");
	ESP("AT+CIPSTART=0,\"TCP\",\"");
	ESP(domain);
	return waitAtResponse("\",80\r\n","OK");
}

ResultWifi Esp32::connectToWifi(const char* ssid, const char* pass)
{
	if(!strlen(ssid))
		return NEED_CONNECT;
	char wifiConfig[500] = {0};
	sprintf(wifiConfig, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pass);
	ResultWifi result = waitAtResponse(wifiConfig, "OK");
	if(result == SUCCESSFUL)
	{
		memset(ssidLast, 0, sizeof(ssidLast));
		memset(passLast, 0, sizeof(passLast));
		strcpy(ssidLast, ssid);
		strcpy(passLast, pass);
		_wifiStatus = 1;
	}
	return result;
}

void Esp32::getWifiConfig(NetworkData _networkData)
{
	if(!strlen(_networkData.ssid) && !strlen(_networkData.password))
		return;
	strcpy(ssidWifi, _networkData.ssid);
	strcpy(passWifi, _networkData.password);
	strcpy(ssidLast, _networkData.ssid);
	strcpy(passLast, _networkData.password);
}

ResultWifi Esp32::initNetworkWifi(const char* ssid, const char* password, const char * domain)
{
	waitAtResponse("AT\r\n", "OK");

	if(waitAtResponse("AT+RST\r\n", "ready") != SUCCESSFUL)
	{
		waitAtResponse("AT+CWMODE=1\r\n", "OK");
		if(connectToWifi(ssidWifi, passWifi) != SUCCESSFUL)
			if(connectToWifi(ssid, password) != SUCCESSFUL)
				return WIFI_ERROR;
	}
	if(waitAtResponse("AT+CIPMUX=1\r\n", "OK") != SUCCESSFUL) return WIFI_ERROR;

	if(waitAtResponse("ATE0\r\n", "OK") != SUCCESSFUL) return WIFI_ERROR;

	if(waitAtResponse("AT+CIFSR\r\n", "OK") != SUCCESSFUL) return WIFI_ERROR;

	if(waitAtResponse("AT+CWJAP?\r\n", "OK") != SUCCESSFUL) return WIFI_ERROR;
	_wifiStatus = 2;
	getTimeSNTP();

	startSSLServer(domain);
	return SUCCESSFUL;
}

bool Esp32::initWifi(const char* ssid, const char* password, const char * domain)
{
	//get ssid and password old
	restart();
	_wifiStatus = 0;
	HAL_UART_Receive_DMA(&ESP_UART, buffer, sizeof(buffer));
	if(initNetworkWifi(ssid, password, domain) == SUCCESSFUL)
		return true;
	return false;
}

bool Esp32::checkWifiConnected(const char * domain)
{
	waitAtResponse("AT+CWJAP?\r\n", "OK");
	if(_countCheck++ > 10)
	{
		char wifiConfig[500] = {0};
		sprintf(wifiConfig, "AT+CWLAP=\"%s\"\r\n", ssidLast);
		ResultWifi result = waitAtResponse(wifiConfig, "OK");
		if(result == WIFI_EXIST)
			connectToWifi(ssidLast, passLast);
		else
			if(result == FAIL_RESPONSE)
				restart();
		_countCheck = 0;
	}
	if(!_rssiWifi)
		return false;
	_countCheck = 0;
	if(waitAtResponse("AT+CIPMUX=1\r\n", "OK") != SUCCESSFUL) return false;
	if(waitAtResponse("ATE0\r\n", "OK") != SUCCESSFUL) return false;
	if(waitAtResponse("AT+CIFSR\r\n", "OK", NULL) != SUCCESSFUL) return false;
	startSSLServer(domain);
	return true;
}

void Esp32::getTimeSNTP()
{
	waitAtResponse("AT+CIPSNTPCFG=1,7,\"vn.pool.ntp.org\",\"time.google.com\"\r\n", "OK");
	for(int i = 0; i < 10; ++i)
	{
		osDelay(1000);
		waitAtResponse("AT+CIPSNTPTIME?\r\n", "OK");
		if(_time.sDate.Year)
		{
			HAL_RTC_SetTime(&hrtc, &_time.sTime, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &_time.sDate, RTC_FORMAT_BIN);
			break;
		}
	}
}

int8_t Esp32::parseTimeSNTP(char* str, clockTime& _timenow)
{
	if(!strlen(str))
		return -1;
	if(!strstr(str, "CIPSNTPTIME:"))
		return -1;
	std::string strTime(strstr(str, "CIPSNTPTIME:") + strlen("CIPSNTPTIME:"), 50);
	try
	{
		std::vector<std::string> ret_time;
		std::vector<std::string> ret_date;

		clearSpaceDouble(strTime);
		splitString(strTime, ret_date, ' ');

		if(ret_date.size() < 5)
			return -1;

		splitString(ret_date.at(3), ret_time, ':');

		_timenow.sDate.WeekDay = getWeekDay(ret_date.at(0));
		_timenow.sDate.Month = getMonth(ret_date.at(1));
		if(checkDigitBegin(ret_date.at(2)))
			_timenow.sDate.Date = stoi(ret_date.at(2));
		if(checkDigitBegin(ret_date.at(4)))
		{
			if(stoi(ret_date.at(4)) > 2000)
				_timenow.sDate.Year = stoi(ret_date.at(4)) - 2000;
		}

		if(ret_time.size() < 3)
			return -1;

		if(checkDigitBegin(ret_time.at(0)))
			_timenow.sTime.Hours = stoi(ret_time.at(0));
		if(checkDigitBegin(ret_time.at(1)))
			_timenow.sTime.Minutes = stoi(ret_time.at(1));
		if(checkDigitBegin(ret_time.at(2)))
			_timenow.sTime.Seconds = stoi(ret_time.at(2));

		return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}

uint8_t Esp32::getWeekDay(const std::string& str)
{
	if(str.empty())
		return 0;
	uint8_t _weekDay{1};
	if(str == "Sun")
		_weekDay = RTC_WEEKDAY_SUNDAY;
	if(str == "Mon")
		_weekDay = RTC_WEEKDAY_MONDAY;
	if(str == "Tue")
		_weekDay = RTC_WEEKDAY_TUESDAY;
	if(str == "Wed")
		_weekDay = RTC_WEEKDAY_WEDNESDAY;
	if(str == "Thu")
		_weekDay = RTC_WEEKDAY_THURSDAY;
	if(str == "Fri")
		_weekDay = RTC_WEEKDAY_FRIDAY;
	if(str == "Sat")
		_weekDay = RTC_WEEKDAY_SATURDAY;
	return _weekDay;
}

uint8_t Esp32::getMonth(const std::string& str)
{
	if(str.empty())
		return 0;
	uint8_t _month{1};
	if(str == "Jan")
		_month = RTC_MONTH_JANUARY;
	if(str == "Feb")
		_month = RTC_MONTH_FEBRUARY;
	if(str == "Mar")
		_month = RTC_MONTH_MARCH;
	if(str == "Apr")
		_month = RTC_MONTH_APRIL;
	if(str == "May")
		_month = RTC_MONTH_MAY;
	if(str == "Jun")
		_month = RTC_MONTH_JUNE;
	if(str == "Jul")
		_month = RTC_MONTH_JULY;
	if(str == "Aug")
		_month = RTC_MONTH_AUGUST;
	if(str == "Sep")
		_month = RTC_MONTH_SEPTEMBER;
	if(str == "Oct")
		_month = RTC_MONTH_OCTOBER;
	if(str == "Nov")
		_month = RTC_MONTH_NOVEMBER;
	if(str == "Dec")
		_month = RTC_MONTH_DECEMBER;
	return _month;
}

bool Esp32::checkDigitBegin(const std::string& str)
{
	if(str.empty())
		return false;

	if (isdigit(str.at(0)) == 0)
		return false;

	return true;
}

bool Esp32::clearSpaceDouble(std::string& str)
{
	if(str.empty())
		return false;

	while (true)
	{
		if (str.find("  ") == std::string::npos)
			break;
		str.erase(str.find("  "), 1);
	}
	if (str.at(str.size() - 1) == ' ')
		str.erase(str.end() - 1);

	return true;
}

unsigned int Esp32::splitString(const std::string& str_input, std::vector<std::string>& ret_list, char ch)
{
	if(str_input.empty())
		return -1;

	unsigned int pos = str_input.find(ch);
	unsigned int init_pos = 0;
	ret_list.clear();

	while (pos != std::string::npos)
	{
		std::string str_sub = str_input.substr(init_pos, pos - init_pos);// init_pos+1);
		ret_list.push_back(str_sub);
		init_pos = pos + 1;
		pos = str_input.find(ch, init_pos);
	}

	std::string last_sub = str_input.substr(init_pos, std::min(pos, str_input.size()) - init_pos + 1);
	ret_list.push_back(last_sub);
	return ret_list.size();
}

int8_t Esp32::readRSSI(char* str)
{
	if(!strlen(str))
		return 0;
	if(!strstr(str, "CWJAP:"))
		return 0;
	std::string strRssi(strstr(str, "CWJAP:") + strlen("CWJAP:"), 50);
	if(strRssi.find(",") == std::string::npos)
		return 0;
	try
	{
		std::vector<std::string> ret_arr;
		std::string chk_arr;
		splitString(strRssi, ret_arr, ',');
		if(ret_arr.size() > 3)
		{
			if(ret_arr.at(3).at(0) != '-')
				return 0;
			chk_arr = ret_arr.at(3).substr(1);
			if(checkDigit(chk_arr))
				return stoi(ret_arr.at(3));
		}
		return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}

uint16_t Esp32::lengthIPD(char* str)
{
	if(!strlen(str))
		return 0;
	try
	{
		if(!strstr(str, "+IPD,"))
			return 0;
		string _http(strstr(str, "+IPD,") + strlen("+IPD,"), 10);
		if (_http.find(",") != std::string::npos)
			_http = _http.substr(2);

		string _httplen;
		_httplen.assign(_http, 0, _http.find(":"));
		if(checkDigit(_httplen))
			return stoi(_httplen);
		else
			return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}


bool Esp32::checkDigit(const std::string& str)
{
	if(str.empty())
		return false;

	if (isdigit(str.at(0)) == 0)
		return false;

	return true;
}


int8_t Esp32::checkHTTP(const std::string& str, char* data, uint16_t& trueLen)
{
	if(str.empty())
		return -1;
	if(data == nullptr)
		return -1;
	try
	{
		string _http;
		if(str.find("HTTP/") == std::string::npos)
			return 0;

		_http.assign(str, str.find("HTTP/") + 9, 10);// , 3);
		_http.assign(_http, 0, _http.find(" "));
		if(checkDigit(_http))
			switch (stoi(_http.substr(0, 1))) {
			case 6:
				return -2;
				break;
			default: //200, 204, 500, 502
				return statusCodeHTTP(str, _http, data, trueLen);
				break;
			}
		return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}

int8_t Esp32::statusCodeHTTP(const std::string& str, const std::string& code, char* data, uint16_t& trueLen)
{
	if(str.empty())
		return 0;
	try
	{
		int8_t checkCon = checkConnection(str);
		switch (stoi(code)) {
		case 204:
		//case 429:
		case 500:
		case 502:
		case 503:
			return checkCon;
		default: //200
			if(str.find("Content-Length:") != std::string::npos)
			{
				if(checkCon == RSP_RECIPSTART)
				{
					contentHTTP(str, data, trueLen);
					return checkCon;
				}
				else
					return contentHTTP(str, data, trueLen);
			}
			else
				if(str.find("Content-Type: text/html") != std::string::npos)
				{
					if(checkCon == RSP_RECIPSTART)
					{
						contentTextHtml(str, data);
						return checkCon;
					}
					else
						return contentTextHtml(str, data);
				}
			break;
		}
		return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}

int8_t Esp32::checkConnection(const std::string& str)
{
	if(str.empty())
		return 0;
	try
	{
		string _connect;
		if(str.find("Connection:") == std::string::npos)
			return 0;

		_connect.assign(str, str.find("Connection:") + 12, 10);
		_connect.assign(_connect, 0, _connect.find("\r\n"));
		if(_connect == "close")
			return RSP_RECIPSTART;
		return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}

int8_t Esp32::contentTextHtml(const std::string& str, char* data)
{
	if(str.empty())
		return 0;
	try
	{
		string _http;
		if(str.find("\r\n\r\n") != std::string::npos)
		{
			_http.assign(str.substr(str.find("\r\n\r\n") + 4));
			if(_http.find("\r\n\r\n") != std::string::npos)
			{
				_http.assign(_http, 0, _http.find("\r\n\r\n"));
				memset(data, 0, strlen(data));
				memcpy(data, _http.c_str(), _http.size());
			}
		}
		return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}

int8_t Esp32::contentHTTP(const std::string& str, char* data, uint16_t& trueLen)
{
	if(str.empty())
		return 0;
	try
	{
		string _http;
		if(str.find("Content-Length:") == std::string::npos)
			return 0;

		_http.assign(str, str.find("Content-Length:") + 16, 10);
		_http.assign(_http, 0, _http.find("\r\n"));
		if(checkDigit(_http))
			switch (trueLen = stoi(_http)) {
			case 0:
				return 0;
				break;
			default:
				return responseHTTP(str, data, stoi(_http));
				break;
			}
		return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}

int8_t Esp32::responseHTTP(const std::string& str, char* data, uint16_t len)
{
	if(str.empty())
		return 0;
	try
	{
		string _http;
		if(str.find("\r\n\r\n") != std::string::npos)
		{
			_http.assign(str.substr(str.find("\r\n\r\n") + 4));
			_http.assign(_http, 0, _http.find("\r\n"));
			memset(data, 0, strlen(data));
			memcpy(data, _http.c_str(), _http.size());
		}
		if(strlen(data) != len)
			return -4;
		return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}

int8_t Esp32::getMoreDataHTTP(const char* str, char* data, uint16_t len, uint16_t& trueLen)
{
	if(!strlen(str))
		return 0;
	try
	{
		string _http;
		if(strstr(str, ":"))
		{
			_http.assign(strstr(str, ":") + 1);
			_http.assign(_http, 0, len);
			strcat(data, _http.c_str());
		}
		if(strlen(data) != trueLen)
			return -4;
		return 0;
	}
	catch(exception& e)
	{
		return 0;
	}
}

bool Esp32::requestGetESP(const char* domain, char* url, char* data)
{
	_wifiStatus = 3;
	memset(data, 0, strlen(data) + 1);
	memset(cipSend, 0, sizeof(cipSend));
	memset(finalUrl, 0, sizeof(finalUrl));

	strcat(finalUrl, "GET ");
	strcat(finalUrl, url);
	strcat(finalUrl, " HTTP/1.1\r\nHost: ");
	strcat(finalUrl, domain);
	strcat(finalUrl, "\r\nConnection: keep-alive\r\n\r\n");

	strcat(cipSend,"AT+CIPSEND=0,");
	sprintf(&cipSend[strlen(cipSend)], "%d", strlen(finalUrl));
	strcat(cipSend, "\r\n");

	int8_t reGet = 0;
	if(waitAtResponse(cipSend, ">"))
	{
		reGet = waitAtResponse(finalUrl, "+IPD,", data);
		switch (reGet) {
			case RECIPSTART:
				startSSLServer(domain);
			case WIFI_DICONNECT:
				return false;
			default:
				break;
		}
	}
	return true;
}

int8_t Esp32::requestPostESP(const char* domain, char* url, char* data)
{
	_wifiStatus = 3;
	memset(cipSend, 0, sizeof(cipSend));
	memset(finalUrl, 0, sizeof(finalUrl));

	strcat(finalUrl, "POST ");
	strcat(finalUrl, url);
	strcat(finalUrl, " HTTP/1.1\r\nHost: ");
	strcat(finalUrl, domain);
	strcat(finalUrl, "\r\nContent-Type: application/x-www-form-urlencoded");
	strcat(finalUrl, "\r\nContent-Length: ");
	sprintf(&finalUrl[strlen(finalUrl)], "%d", strlen(data) + 5);
	strcat(finalUrl, "\r\nConnection: keep-alive");
	strcat(finalUrl, "\r\n\r\ndata=");
	strcat(finalUrl, data);
	strcat(finalUrl, "\r\n");

	if(waitAtResponse("AT+CWJAP?\r\n", "OK") == WIFI_DICONNECT)
		return WIFI_DICONNECT;

	if(!_rssiWifi)
		return WIFI_SWITCH;

	strcat(cipSend,"AT+CIPSEND=0,");
	sprintf(&cipSend[strlen(cipSend)], "%d", strlen(finalUrl));
	strcat(cipSend, "\r\n");

	int8_t rePost = 0;
	memset(rspCode, 0, sizeof(rspCode));
	if(waitAtResponse(cipSend, ">"))
	{
		rePost = waitAtResponse(finalUrl, "+IPD,", rspCode);
		switch (rePost) {
			case RECIPSTART:
				startSSLServer(domain);
				break;
			case WIFI_DICONNECT:
				return WIFI_DICONNECT;
			default:
				break;
		}
	}
	else
	{
		return -1;
	}
	if(!strlen(rspCode))
		return 0;

	std::string chkCode;
	chkCode.assign(rspCode);
	if(checkDigit(chkCode))
		return stoi(chkCode);
	else
		return -9;
}

ResultWifi Esp32::waitAtResponse(const char* message, const char* messRsp, char* data, bool check)
{
	if(message == nullptr)
		return WIFI_ERROR;
	if(messRsp == nullptr)
		return WIFI_ERROR;

	HAL_IWDG_Refresh(&hiwdg);

	char* response = nullptr;
	uint16_t timeout = 0;
	int8_t statusRsp = 0;
	uint16_t lenRsp = 0;

	if (_enCheck)
	{
		if(strstr(reinterpret_cast<char*>(buffer), "WIFI DISCONNECT"))
		{
			_rssiWifi = 0;
			clearBuffer();
			return WIFI_DICONNECT;
		}
	}

	clearBuffer();
	ESP(message);
	while(!strstr(reinterpret_cast<char*>(buffer), messRsp))
	{
		osDelay(10);
		if(timeout++ >= MAX_TICK)
		{
			_countFail++;
			if(!strlen(reinterpret_cast<char*>(buffer)) && !strstr(message, "RST"))
			{
				_rssiWifi = 0;
				return WIFI_DICONNECT;
			}
			return FAIL_RESPONSE;
		}

		if(strstr(reinterpret_cast<char*>(buffer), "ALREADY CONNECTED"))
			return SUCCESSFUL;

		if(strstr(message, "CWJAP="))
		{
			timeout = 0;
			if(strstr(reinterpret_cast<char*>(buffer), "ERROR") || strstr(reinterpret_cast<char*>(buffer), "FAIL"))//+CWJAP:3 - wrong ssid, 4 - wrong pass for esp32, "FAIL" for esp8266
			{
				timeout = MAX_TICK;
			}
		}

		if (!strstr(message, "RST") && !strstr(message, "CWJAP="))
		{
			if(strstr(reinterpret_cast<char*>(buffer), "WIFI DISCONNECT") || strstr(reinterpret_cast<char*>(buffer), "ERROR") || strstr(reinterpret_cast<char*>(buffer), "DNS Fail"))
			{
				_rssiWifi = 0;
				return WIFI_DICONNECT;
			}
		}

		if (strstr(message, "CWJAP?") && strstr(reinterpret_cast<char*>(buffer), "No AP"))//only for esp8266
		{
			_rssiWifi = 0;
			timeout = MAX_TICK;
		}
	}
	osDelay(10);

	_countFail = 0;

	timeout = 0;
	if(strstr(message, "RST"))
	{
		if(strstr(reinterpret_cast<char*>(buffer), "DISCONNECT"))
		{
			while(!strstr(reinterpret_cast<char*>(buffer), "GOT IP")) //CONNECTED
			{
				osDelay(10);
				if(timeout++ > 500)
					return FAIL_RESPONSE;
			}
			return SUCCESSFUL;
		}
		return NEED_CONNECT;
	}

	if (strstr(message, "CIPMUX?"))
	{
		if(data != nullptr && strstr(reinterpret_cast<char*>(buffer), "CIPMUX:"))
		{
			response = strstr(reinterpret_cast<char*>(buffer), "CIPMUX:") + strlen("CIPMUX:");
			*data = response[0] == '1' ? 1 : 0;
		}
	}

	if(strstr(reinterpret_cast<const char*>(message), "CWJAP?"))
		_rssiWifi = readRSSI(reinterpret_cast<char*>(buffer));

//	if(strstr(message, "CIFSR"))
//		if(data != nullptr)
//			getIpGateway(reinterpret_cast<char*>(buffer), data);

	if(strstr(message, "CWLAP"))
	{
//		getListWifi(reinterpret_cast<char*>(buffer));
		return WIFI_EXIST;
	}

	if(strstr(message, "CIPSNTPTIME?"))
		parseTimeSNTP(reinterpret_cast<char*>(buffer), _time);

	if(strcmp(messRsp, "OK") == 0 || strcmp(messRsp, ">") == 0 || strcmp(messRsp, "ready") == 0)
		return SUCCESSFUL;

	if(strstr(message, "GET") || strstr(message, "POST"))
	{
		response = strstr(reinterpret_cast<char*>(buffer), "+IPD,");
		do
		{
			timeout = 0;
			uint16_t _lenIPD = lengthIPD(response);
			while(*(strstr(response, ":") + 1 + _lenIPD + 1) == '\0')
			{
				osDelay(10);
				if(timeout++ > 200)
				{
					_countFail++;
					return FAIL_RESPONSE;
				}
			}

			if(statusRsp == RSP_MORE_DATA)
				statusRsp = getMoreDataHTTP(response, data, _lenIPD, lenRsp);
			else
			{
				_strResponse.clear();
				_strResponse.assign(strstr(response, messRsp));
				statusRsp = checkHTTP(_strResponse, data, lenRsp);
			}
			switch (statusRsp) {
				case RSP_RECIPSTART:
					return RECIPSTART;
				case RSP_MORE_DATA:
					response = strstr(response, "+IPD,") + strlen("+IPD,");
					timeout = 0;
					while(!strstr(response, "+IPD,"))
					{
						osDelay(10);
						if(timeout++ > 200)
						{
							_countFail++;
							return FAIL_RESPONSE;
						}
					}
					response = strstr(response, "+IPD,");
					break;
				default:
					break;
			}
		} while(statusRsp == RSP_MORE_DATA);
	}
	return SUCCESSFUL;
}

void Esp32::clearBuffer(void)
{
	memset(buffer, 0, sizeof(buffer));
	__HAL_DMA_DISABLE(&ESP_DMA);
	__HAL_DMA_SET_COUNTER(&ESP_DMA, sizeof(buffer));
	__HAL_DMA_ENABLE(&ESP_DMA);
}

void Esp32::Esp32_IRQHandler_Error()
{
	memset(buffer, 0, sizeof(buffer));
	HAL_UART_DMAStop(&ESP_UART);
	HAL_UART_Receive_DMA(&ESP_UART, buffer, sizeof(buffer));
}
