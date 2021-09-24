/*
 * define.hpp
 *
 *  Created on: Jul 5, 2021
 *      Author: EoH.Dai
 */

#ifndef DEFINE_HPP_
#define DEFINE_HPP_

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "main.h"
#include "cmsis_os.h"
#include "string.h"
#include "type_list.hpp"

#define NUM_MODBUS			8
#define SLAVE_ADDRESS		10
#define NUM_ADC					4
//LCD
#define LCD_PWM  				htim4
#define LCD_PWM_CHANNEL	TIM_CHANNEL_2
//RTOS
extern osMessageQueueId_t modbusLCDQueueHandle;
extern osMessageQueueId_t modbusNetworkQueueHandle;
extern osMessageQueueId_t changeNumConfigQueueHandle;
extern osMessageQueueId_t writeConfigQueueHandle;
extern osMessageQueueId_t responseWriteQueueHandle;
extern osMessageQueueId_t fetchControlQueueHandle;

extern osMessageQueueId_t modbusCalibQueueHandle;
extern osMessageQueueId_t responseCommandCalibHandle;
extern osMessageQueueId_t displayStatusCalibHandle;
extern osMessageQueueId_t responseStatusCalibQueueHandle;
extern osMessageQueueId_t flagGetValueCalibHandle;

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern I2C_HandleTypeDef hi2c1;

extern IWDG_HandleTypeDef hiwdg;

extern RTC_HandleTypeDef hrtc;

extern SPI_HandleTypeDef hspi3;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim13;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart6_rx;




#endif /* DEFINE_HPP_ */
