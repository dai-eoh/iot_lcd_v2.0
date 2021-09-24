/*
 * ButtonMatrix.c
 *
 *  Created on: May 5, 2021
 *      Author: EoH.Dai
 */
#include "ButtonMatrix.h"
#include "define.hpp"




const GPIO_TypeDef* listRowPort[] = LIST_ROW_PORT;
const GPIO_TypeDef* listColPort[] = LIST_COL_PORT;
const uint16_t listRowPin[] = LIST_ROW_PIN;
const uint16_t listColPin[] = LIST_COL_PIN;
const KeyPad listKey[NUM_ROW*NUM_COL] = {KEY_5, KEY_6, KEY_ID,    KEY_LEFT,   \
																					 KEY_4, KEY_1, KEY_CLR,  	KEY_UP,     \
																					 KEY_9, KEY_2, KEY_ENTER, KEY_DOWN,   \
																					 KEY_8, KEY_3, KEY_RIGHT, 	KEY_ALARM,  \
																					 KEY_7, KEY_0, KEY_SET, 	KEY_ESC };

static uint8_t _col = 0;
static uint8_t _row = 0;
static uint64_t _tickButton = 0;
static uint64_t _tickBuzzer = 0;
static bool _flagButton = false;

void Buzzer_Init(void)
{
	HAL_TIM_PWM_Start(BUZZER_TIMER, BUZZER_CHANNEL);
	__HAL_TIM_SET_COMPARE(BUZZER_TIMER, BUZZER_CHANNEL, 0);
}

void Buzzer_Control(bool _flag)
{
	if(!_flag)
		HAL_TIM_PWM_Start(BUZZER_TIMER, BUZZER_CHANNEL);
	else
		HAL_TIM_PWM_Stop(BUZZER_TIMER, BUZZER_CHANNEL);
}

static void Buzzer_Set(bool flag)
{
	if(flag)
		__HAL_TIM_SET_COMPARE(BUZZER_TIMER, BUZZER_CHANNEL, BUZZER_ON);
	else
		__HAL_TIM_SET_COMPARE(BUZZER_TIMER, BUZZER_CHANNEL, BUZZER_OFF);
}


bool isPressed(void)
{
	for(int i =0 ; i < NUM_COL; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef*)listColPort[i], listColPin[i], GPIO_PIN_RESET);
	}
	for(int i = 0; i < NUM_ROW; i++)
	{
		if(HAL_GPIO_ReadPin((GPIO_TypeDef*)listRowPort[i], listRowPin[i]) == GPIO_PIN_RESET)
			return true;
	}
	return false;
}

void scanColumn(uint8_t row)
{
	for(int i =0 ; i < NUM_COL; i++)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef*)listColPort[i], listColPin[i], GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin((GPIO_TypeDef*)listColPort[row], listColPin[row], GPIO_PIN_RESET);
}

uint8_t scanRow()
{
	uint8_t row = 0;
	for(int i = 0; i < NUM_ROW; i++)
	{
		if(HAL_GPIO_ReadPin((GPIO_TypeDef*)listRowPort[i], listRowPin[i]) == GPIO_PIN_RESET)
		{
			row = i+1;
		}
	}
	return row;
}

KeyPad getKey()
{
	if((HAL_GetTick() - _tickBuzzer > BUZZER_TIME) && _flagButton)
	{
		_flagButton = false;
		Buzzer_Set(false);
	}
	if(isPressed())
	{
		Buzzer_Set(true);
		_tickBuzzer = HAL_GetTick();
		_flagButton = true;
		if(HAL_GetTick() - _tickButton < DEBOUNCE_TIME)
			return 0;
		else
		{
			_tickButton = HAL_GetTick();
		}
		for(int i = 0; i< NUM_COL; i++)
		{
			scanColumn(i);
			uint8_t _check = scanRow();
			if(_check > 0)
			{
				_row = _check;
				_col = i;
				return listKey[_col*4 + _row - 1];
			}
		}
	}
	return KEY_NONE;
}

uint8_t getKeyNumber(uint8_t keyValue)
{
	if(keyValue >= KEY_0 && keyValue <= KEY_9)
		return ( 39 + keyValue);
	else
		return 0;
}

stateButton getStateButton()
{
	stateButton _key = getKey();
	switch((uint8_t)_key)
	{
	case 1:
		return UP_BTN;
	case 2:
		return DOWN_BTN;
	case 3:
		return LEFT_BTN;
	case 4:
		return RIGHT_BTN;
	case 7:
		return ENTER_BTN;
	case 6:
		return ESC_BTN;
	}
	return NO_BTN;
}

bool checkResetPasswordButton(void)
{
	HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
	{
		osDelay(200);
		if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
		{
			uint32_t _preTime = HAL_GetTick();
			while(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
			{
				osDelay(10);
				uint32_t endTime = HAL_GetTick() - _preTime;
				if( endTime > TIME_RESET)
					return true;
			}
		}
	}
	return false;
}
