/*
 * lcd_display.hpp
 *
 *  Created on: Jul 5, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_LCD_DISPLAY_HPP_
#define INC_LCD_DISPLAY_HPP_

#ifdef __cplusplus
extern "C" {
#endif
//Startup Page
#include "type_list.hpp"
#define WD_LOGO 				48
#define HT_LOGO
#define X_BAR						25
#define Y_BAR						48
#define HT_BAR					10
#define Y_LOGO 					2


void LCD_Init(void);
void LCD_LoadConfigFromFlash(void);
void LCD_StartupPage(void);
void LCD_CopyModbusConfig(ModbusRTU * _modbusRTU, ModbusConfig * _modbusConfig);
void LCD_SwitchContextPage(void);


#ifdef __cplusplus
}
#endif


#endif /* INC_LCD_DISPLAY_HPP_ */
