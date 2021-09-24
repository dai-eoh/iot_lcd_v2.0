/*
 * Sim800C.cpp
 *
 *  Created on: Oct 29, 2020
 *      Author: EOH
 */


#include "Sim800C.hpp"



Sim800C::Sim800C()
{

}
uint8_t Sim800C::getStepConn(void)
{
	return stepConn;
}

void Sim800C::setStepConn(uint8_t num)
{
	stepConn = num;
}

void Sim800C::resetCountFailStepConn1(void)
{
	countFailStepConn1 = 0;
}

void Sim800C::restart(void)
{
	stepConn = 0;
	countFailStepConn1 = 0;
  powerOff;
  osDelay(1500);
  powerOn;
  osDelay(500);
	PWK_On;
	osDelay(2000);
	PWK_Off;
	osDelay(2000);
}

void Sim800C::init(void)
{
	_baudRate = SIM_UART.Init.BaudRate;
  HAL_UART_Receive_DMA(&SIM_UART, (uint8_t*)buffer, sizeof(buffer));
}

void Sim800C::checkSimStatus(void)
{
	restart();
	while(!waitForAtResponse("AT\n", "SMS Ready", NULL))
	{
		osDelay(10);
		if(countBreak++ > 5)
		{
			countBreak = 0;
			return;
		}
	}
	waitForAtResponse("AT+CLTS=1;&W\n", "OK", NULL);
	stepConn = 1;

}

void Sim800C::checkSimStatusMain(void)
{
	countFailStepConn1 = 0;

	if(statusM95 == 0) {
		PWK_On;
		osDelay(2000);
		PWK_Off;
		osDelay(2000);
	}
	else
	{
		stepConn = 1;
		return;
	}

	while(!waitForAtResponse("AT\n", "SMS Ready", NULL))
	{
		osDelay(10);
		if(countBreak++ > 5)
		{
			statusM95 = 0;
			return;
		}
	}
	statusM95 = 1;
	waitForAtResponse("AT+CLTS=1;&W\n", "OK", NULL);

	if(statusM95 == 1)
		stepConn = 1;
}

void Sim800C::checkSignal(void)
{
	char data[1024] = {0};
	if(waitForAtResponse("AT+CREG?\n", "OK", data))
	{
		HAL_Delay(2000);
		if(atoi(data) == 1)
		{
			countFailStepConn1 = 0;
			stepConn = 2;
		}
	}

	countFailStepConn1++;
	if(countFailStepConn1 >= numberFailStepConn1)
		restart();
}

bool Sim800C::checkGPRSIfFail(void)
{
	char data[1024] = {0};

	if(waitForAtResponse("AT+CGATT?\n", "OK", data))
	{
		if(atoi(data) != 1)
		{
			stepConn = 2;
			return false;
		}
		else
			return true;
	}
	stepConn = 2;
	return false;
}

void Sim800C::checkGPRS(void)
{
	char data[1024] = {0};
	waitForAtResponse("AT+SAPBR=3,1,\"APN\",\"m-wap\"\n", "OK", NULL);
	waitForAtResponse("AT+SAPBR=1,1\n", "OK", NULL); //SMS Ready
	if(waitForAtResponse("AT+CGATT?\n", "OK", data))
	{
		if(atoi(data) == 1)
			stepConn = 3;
	}
	waitForAtResponse("AT+HTTPINIT\n", "OK", NULL);
	countFailStepConn1++;
	if(countFailStepConn1 >= numberFailStepConn1)
	{
		countFailStepConn1 = 0;
		stepConn = 1;
	}

	//waitForAtResponse("AT+HTTPSSL=1\n", "OK", NULL);
}

void Sim800C::getTime(RTC_DateTypeDef* RTC_DateStruct, RTC_TimeTypeDef* RTC_TimeStruct)
{
	char data[1024] = {0};
	if (waitForAtResponse("AT+CCLK?\n", "OK", data)) {
		parseDateTimeFromSim(data, RTC_DateStruct, RTC_TimeStruct);
	}
}

int8_t Sim800C::getSignalQuality(void)
{
	char _signal[1024] = {0};
	waitForAtResponse("AT+CSQ\n", "OK", (char*)_signal);
	return atoi(_signal);
}

void Sim800C::requestGet(char* url, char* data)
{
	memset(data, 0, strlen(data) + 1);
	char response[1024] = {0};

	waitForAtResponse("AT+HTTPPARA=\"CID\",1\n", "OK", NULL);

	GSM("AT+HTTPPARA=\"URL\",\"");
	GSM(url);
	waitForAtResponse("\"\n", "OK", NULL);

	if (waitForAtResponse("AT+HTTPACTION=0\n", "HTTPACTION:", response))
		atHttpGetResponse(data, atoi(response));
}

int Sim800C::requestPost(char* url, char* data)
{
	char response[1024] = {0};
	if(!waitForAtResponse("AT+CSQ\n", "OK", (char*)staCSQ))
	{
		return 0;
	}

	if(!waitForAtResponse("AT+HTTPPARA=\"CID\",1\n", "OK", NULL))
	{
		return 0;
	}

	GSM("AT+HTTPPARA=\"URL\",\"");
	GSM(url);
	if(!waitForAtResponse("\"\n", "OK", NULL))
	{
		return 0;
	}

	if (!waitForAtResponse("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\n", "OK", NULL))
	{
		return 0;
	}

	char length[20];
	sprintf(length, "%d", strlen(data) + 5);  // 5 is strlen("data=")

	GSM("AT+HTTPDATA=");
	GSM((char *)length);
	if (!waitForAtResponse(",5000\n", "DOWNLOAD", NULL))
	{
		return 0;
	}

	GSM("data=");
	GSM(data);
	if (!waitForAtResponse("\n", "OK", NULL))
	{
		return 0;
	}

	int16_t _value = 0;
	if (waitForAtResponse("AT+HTTPACTION=1\n", "HTTPACTION:", response)) {
		_value = atoi(response);
		if(_value > 0)
			atHttpGetResponse(response, _value);
		_value = atoi(response);
		return _value;
	} else {
		return -1;
	}
}

uint8_t Sim800C::sendSMS(char * phone, char * message)
{
	const char ctrlz = 26;
	waitForAtResponse("AT+CMGF=1\n", "OK", NULL);
	GSM("AT+CMGS=\"");
	GSM(phone);
	if (waitForAtResponse("\"\n", ">", NULL)) {

		GSM(message);
		waitForAtResponse(&ctrlz, "OK", NULL);
	}
	return 0;
}

void Sim800C::GSM(const char* msg)
{
	if(!strlen(msg))
		return;
	while(HAL_UART_Transmit_IT(&SIM_UART,(uint8_t*) msg, strlen(msg)) != HAL_OK);
}


void Sim800C::GSMH(uint8_t* data, uint32_t len)
{
	for(uint i=0; i<len; i++)
		GSM_hex(data[i]);
}

void Sim800C::resetBuffer(void)
{
	memset(buffer, 0, sizeof(buffer));
	__HAL_DMA_DISABLE(&SIM_DMA);
	__HAL_DMA_SET_COUNTER(&SIM_DMA, sizeof(buffer));
	__HAL_DMA_ENABLE(&SIM_DMA);
}

void Sim800C::parseDateTimeFromSim(char * data, RTC_DateTypeDef* RTC_DateStruct, RTC_TimeTypeDef* RTC_TimeStruct)
{
	char dateStr[10], timeStr[10];
	int date[10], time[10];

	for(int i = 0; i < 8; i++) {
		dateStr[i] = data[i];
		timeStr[i] = data[i+9];
	}

	splitTimeBy(date, dateStr, '/');
	splitTimeBy(time, timeStr, ':');

	RTC_DateStruct->Year = date[0];
	RTC_DateStruct->Month = date[1];
	RTC_DateStruct->Date = date[2];
	RTC_TimeStruct->Hours = time[0];
	RTC_TimeStruct->Minutes = time[1];
	RTC_TimeStruct->Seconds = time[2];
}

void Sim800C::splitTimeBy(int * res, char * data, char splitChar)
{
	int index = 0, indexTemp = 0;
	char temp[1024];

	for(uint i = 0; i < strlen(data) + 1; i++) {
		char ch = data[i];

		if (ch == splitChar || ch == 0) {
			res[index++] = atoi(temp);
			temp[0] = 0;
			indexTemp = 0;
		}
		else {
			temp[indexTemp++] = ch;
		}
	}
}

void Sim800C::atHttpGetResponse(char* data, uint16_t maxLength)
{
	char send[50] = {0};
	if(maxLength >= sizeof(buffer))
		return;
	sprintf(send, "AT+HTTPREAD=0,%d\n", maxLength);
	waitForAtResponse(send, "OK", data);
}

uint8_t Sim800C::waitForAtResponse(const char* message, const char* messRe, char* data)
{
	if(message == nullptr)
		return 0;
	if(messRe == nullptr)
		return 0;
	HAL_IWDG_Refresh(&hiwdg);
	char* response = nullptr;
	uint16_t timeout = 0;
	uint16_t lencpy = 0;
	resetBuffer();
	GSM(message);
	while(!strstr((char*) buffer, (char*)messRe)) {
		timeout++;
		osDelay(10);
		if(timeout > 500 && !strlen(buffer))
			return 0;
		if(timeout > 700)
			return 0;
		if(strstr(message, "AT\n"))
		{
			if(strstr(buffer, "NOT INSERTED"))
				return 0;
		}
		if (strstr(buffer, "POWER DOWN"))
			NVIC_SystemReset();
		if (strstr(buffer, "ERROR"))
			return 0;
//		if(strcmp(message, "AT\n") == 0)
//		{
//			response = buffer;
//			do
//			{
//				osDelay(1);
//				if(strstr(response, (char*)messRe))
//					break;
//				response += strlen(response) + 1;
//			} while (*(response));
//			if(strstr((char*) response, (char*)messRe))
//				break;
//		}
	}
	osDelay(10);
	if (strstr(message, "CREG?"))
	{
		if(data != NULL)
			strncpy(data, strstr(buffer,"CREG:") + strlen("CREG:") + 3, 1);
	}

	if (strstr(message, "CGATT?"))
	{
		if(data != NULL)
			strncpy(data, strstr(buffer,"CGATT:") + strlen("CGATT:") + 1, 1);
	}

	if (strstr(message, "CSQ"))
	{
		response = strstr(buffer, "CSQ:") + strlen("CSQ:") + 1;
		if(data != NULL)
		{
			lencpy = strlen(response) - strlen(strstr(response, ","));
			strncpy(data, response, lencpy);
		}
	}

	if (strstr(message, "HTTPACTION"))
	{
		response = strstr(buffer, "HTTPACTION:") + strlen("HTTPACTION:");
		while(*(response + 8) == '\0');
		response = strstr(response, ",") + 1;
		if (response[0] == '6') {
			sprintf(data, "%d", -2);
			return 1;
		}
		switch (atoi(response)) {
			case 204:
			case 500:
			case 502:
			case 503:
				sprintf(data, "%d", 0);
				return 1;
			default:
				break;
		}

		response = strstr(response, ",") + 1;
		if(data != NULL)
		{
			lencpy = strlen(response) - strlen(strstr(response, "\r\n"));
			strncpy(data, response, lencpy);
		}
	}

	if (strstr(message, "HTTPREAD"))
	{
		response = strstr(buffer, "HTTPREAD:");
		response = strstr(response, "\r\n") + 2;
		if(data != NULL)
		{
			lencpy = strlen(response) - strlen(strstr(response, "\r\n"));
			strncpy(data, response, lencpy);
		}
	}

	if (strstr(message, "CCLK?"))
	{
		response = strstr(buffer, "CCLK:") + strlen("CCLK:") + 2;
		if(data != NULL)
		{
			lencpy = strlen(response) - strlen(strstr(response, "+"));
			strncpy(data, response, lencpy);
		}
	}
	return 1;
}

void Sim800C::SIM_IRQHandler_Error(void)
{
//	HAL_UART_Abort_IT(&SIM_UART);
//	HAL_UART_DeInit(&SIM_UART);
//	SIM_UART.Init.BaudRate = _baudRate;
//	if (HAL_UART_Init(&SIM_UART) != HAL_OK) {
//	    Error_Handler();
//	}
	memset(buffer, 0, sizeof(buffer));
	HAL_UART_DMAStop(&SIM_UART);
	HAL_UART_Receive_DMA(&SIM_UART, (uint8_t*)buffer, sizeof(buffer));
}
