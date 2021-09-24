/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC_AI2_Pin GPIO_PIN_0
#define ADC_AI2_GPIO_Port GPIOC
#define ADC_AI1_Pin GPIO_PIN_1
#define ADC_AI1_GPIO_Port GPIOC
#define SIM_KEY_Pin GPIO_PIN_2
#define SIM_KEY_GPIO_Port GPIOC
#define SIM_PW_Pin GPIO_PIN_3
#define SIM_PW_GPIO_Port GPIOC
#define SIM_TX_Pin GPIO_PIN_0
#define SIM_TX_GPIO_Port GPIOA
#define SIM_RX_Pin GPIO_PIN_1
#define SIM_RX_GPIO_Port GPIOA
#define WIFI_TX_Pin GPIO_PIN_2
#define WIFI_TX_GPIO_Port GPIOA
#define WIFI_RX_Pin GPIO_PIN_3
#define WIFI_RX_GPIO_Port GPIOA
#define WIFI_PW_Pin GPIO_PIN_4
#define WIFI_PW_GPIO_Port GPIOA
#define ADC_VIN_Pin GPIO_PIN_5
#define ADC_VIN_GPIO_Port GPIOA
#define ROW0_Pin GPIO_PIN_6
#define ROW0_GPIO_Port GPIOA
#define ROW1_Pin GPIO_PIN_7
#define ROW1_GPIO_Port GPIOA
#define ROW2_Pin GPIO_PIN_4
#define ROW2_GPIO_Port GPIOC
#define ROW3_Pin GPIO_PIN_5
#define ROW3_GPIO_Port GPIOC
#define COL0_Pin GPIO_PIN_0
#define COL0_GPIO_Port GPIOB
#define COL1_Pin GPIO_PIN_1
#define COL1_GPIO_Port GPIOB
#define COL2_Pin GPIO_PIN_2
#define COL2_GPIO_Port GPIOB
#define COL3_Pin GPIO_PIN_10
#define COL3_GPIO_Port GPIOB
#define COL4_Pin GPIO_PIN_11
#define COL4_GPIO_Port GPIOB
#define OUT1_Pin GPIO_PIN_12
#define OUT1_GPIO_Port GPIOB
#define OUT2_Pin GPIO_PIN_13
#define OUT2_GPIO_Port GPIOB
#define OUT3_Pin GPIO_PIN_14
#define OUT3_GPIO_Port GPIOB
#define OUT4_Pin GPIO_PIN_15
#define OUT4_GPIO_Port GPIOB
#define RS485_TX1_Pin GPIO_PIN_6
#define RS485_TX1_GPIO_Port GPIOC
#define RS485_RX1_Pin GPIO_PIN_7
#define RS485_RX1_GPIO_Port GPIOC
#define RS485_DE1_Pin GPIO_PIN_8
#define RS485_DE1_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_8
#define LED_R_GPIO_Port GPIOA
#define RS485_TX2_Pin GPIO_PIN_9
#define RS485_TX2_GPIO_Port GPIOA
#define RS485_RX2_Pin GPIO_PIN_10
#define RS485_RX2_GPIO_Port GPIOA
#define RS485_DE2_Pin GPIO_PIN_11
#define RS485_DE2_GPIO_Port GPIOA
#define LED_G_Pin GPIO_PIN_12
#define LED_G_GPIO_Port GPIOA
#define DBG_DIO_Pin GPIO_PIN_13
#define DBG_DIO_GPIO_Port GPIOA
#define DBG_CLK_Pin GPIO_PIN_14
#define DBG_CLK_GPIO_Port GPIOA
#define LED_B_Pin GPIO_PIN_15
#define LED_B_GPIO_Port GPIOA
#define FLS_SCK_Pin GPIO_PIN_10
#define FLS_SCK_GPIO_Port GPIOC
#define FLS_MISO_Pin GPIO_PIN_11
#define FLS_MISO_GPIO_Port GPIOC
#define FLS_MOSI_Pin GPIO_PIN_12
#define FLS_MOSI_GPIO_Port GPIOC
#define FLS_CS_Pin GPIO_PIN_2
#define FLS_CS_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_3
#define LCD_CS_GPIO_Port GPIOB
#define LCD_MOSI_Pin GPIO_PIN_4
#define LCD_MOSI_GPIO_Port GPIOB
#define LCD_SCLK1_Pin GPIO_PIN_5
#define LCD_SCLK1_GPIO_Port GPIOB
#define LCD_SCLK2_Pin GPIO_PIN_6
#define LCD_SCLK2_GPIO_Port GPIOB
#define LCD_BL_Pin GPIO_PIN_7
#define LCD_BL_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
