/*
 * ButtonMatrix.h
 *
 *  Created on: May 5, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_BUTTONMATRIX_H_
#define INC_BUTTONMATRIX_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"
#include "stdbool.h"

#define LIST_ROW_PIN 		{ROW0_Pin, ROW1_Pin, ROW2_Pin, ROW3_Pin}
#define LIST_COL_PIN		{COL0_Pin, COL1_Pin, COL2_Pin, COL3_Pin, COL4_Pin}
#define LIST_ROW_PORT		{ROW0_GPIO_Port, ROW1_GPIO_Port, ROW2_GPIO_Port, ROW3_GPIO_Port}
#define LIST_COL_PORT		{COL0_GPIO_Port, COL1_GPIO_Port, COL2_GPIO_Port, COL3_GPIO_Port, COL4_GPIO_Port}
#define NUM_ROW					4
#define NUM_COL					5
#define DEBOUNCE_TIME		300
#define BUZZER_TIMER		&htim3
#define BUZZER_CHANNEL	TIM_CHANNEL_4
#define BUZZER_TIME			10
#define BUZZER_ON				29
#define BUZZER_OFF			0
#define TIME_RESET			5000

typedef enum stateButton
{
	NO_BTN=0,
	UP_BTN,
	DOWN_BTN,
	RIGHT_BTN,
	LEFT_BTN,
	ENTER_BTN,
	ESC_BTN
}stateButton;

typedef enum KeyPad
{
	KEY_NONE=0,
	KEY_ESC,
	KEY_ALARM,
	KEY_DOWN,
	KEY_UP,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_SET,
	KEY_ENTER,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_ID,
	KEY_CLR
}KeyPad;


bool isPressed(void);
void scanColumn(uint8_t row);
uint8_t scanRow();
KeyPad getKey();
stateButton getStateButton();
void Buzzer_Init(void);
void Buzzer_Control(bool _flag);
uint8_t getKeyNumber(uint8_t keyValue);
bool checkResetPasswordButton(void);

#ifdef __cplusplus
}
#endif
#endif /* INC_BUTTONMATRIX_H_ */
