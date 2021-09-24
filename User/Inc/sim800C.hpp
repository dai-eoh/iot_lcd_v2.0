/*
 * Sim800C.hpp
 *
 *  Created on: Oct 29, 2020
 *      Author: EOH
 */

#ifndef INC_SIM800C_HPP_
#define INC_SIM800C_HPP_


#include "stdio.h"
#include "stdlib.h"
#include "main.h"
#include "cmsis_os.h"
#include "define.hpp"

#define PWK_On	HAL_GPIO_WritePin(SIM_KEY_GPIO_Port, SIM_KEY_Pin, GPIO_PIN_SET)
#define PWK_Off HAL_GPIO_WritePin(SIM_KEY_GPIO_Port, SIM_KEY_Pin, GPIO_PIN_RESET)

#define powerOn 	HAL_GPIO_WritePin(SIM_PW_GPIO_Port, SIM_PW_Pin, GPIO_PIN_SET)
#define powerOff	HAL_GPIO_WritePin(SIM_PW_GPIO_Port, SIM_PW_Pin, GPIO_PIN_RESET)
#define numberFailStepConn1 10
#define SIM_UART  huart4
#define SIM_DMA		hdma_uart4_rx
class Sim800C
{
public:
	Sim800C();
	virtual ~Sim800C() {};
	void init(void);
	uint8_t getStepConn(void);
	void resetCountFailStepConn1(void);
	void setStepConn(uint8_t num);
	void checkSimStatus(void);
	void checkSimStatusMain(void);
	void checkSignal(void);
	bool checkGPRSIfFail(void);
	void checkGPRS(void);
	void getTime(RTC_DateTypeDef* RTC_DateStruct, RTC_TimeTypeDef* RTC_TimeStruct);
	int8_t getSignalQuality(void);
	void requestGet(char* url, char* data);
	int requestPost(char* url, char* data);
	void restart(void);
	uint8_t sendSMS(char * phone, char * message);
	uint8_t waitForAtResponse(const char* message, const char* messRe, char* data);
	uint8_t countFailStepConn1;
	volatile uint8_t staCSQ[5] = {0};
	uint8_t statusM95 = 0;
	void SIM_IRQHandler_Error(void);
private:
	void GSM(const char* msg);
	void GSMB(uint8_t* msg, uint8_t len);
	void GSMC(uint8_t x);
	void GSM_hex(uint8_t data);
	void GSMH(uint8_t* data, uint32_t len);
	void resetBuffer(void);
	void parseDateTimeFromSim(char * data, RTC_DateTypeDef* RTC_DateStruct, RTC_TimeTypeDef* RTC_TimeStruct);
	void splitTimeBy(int * res, char * data, char splitChar);
	void atHttpGetResponse(char* data, uint16_t maxLength);

	uint8_t stepConn;
	uint32_t _baudRate = 0;
	uint16_t countBreak = 0;
	int ena;
	int debugSim;
	int debugSim1;
	int debugGPRS;
	int buf[50];
	int bufLen = 0;
	char dateStr[10], timeStr[10];
	uint8_t simEna;
	char buffer[1000];
};

#endif /* INC_SIM800C_HPP_ */
