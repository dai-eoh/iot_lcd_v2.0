/*
 * lcd_display.cpp
 *
 *  Created on: Jul 5, 2021
 *      Author: EoH.Dai
 */

#include "variable_lcd.hpp"
#include "lcd_display.hpp"



void LCD_CopyModbusConfig(ModbusRTU * _modbusRTU, ModbusConfig * _modbusConfig)
{
	for(int i = 0; i < NUM_MODBUS; i++)
	{
		_modbusRTU[i].addr = _modbusConfig[i].addr;
		_modbusRTU[i].func = _modbusConfig[i].func;
		_modbusRTU[i].regID = _modbusConfig[i].regId;
		_modbusRTU[i].length = _modbusConfig[i].length;
	}
}

void LCD_LoadConfigFromFlash(void)
{
	ExFlash_ReadData(&networkData, NETWORKCONFIG_ADDRESS, sizeof(networkData));
	ExFlash_ReadData(modbusConfig, MODBUSCONFIG_ADDRESS, sizeof(modbusConfig));
	LCD_CopyModbusConfig(modbusRTU ,modbusConfig);
	ExFlash_ReadData(sensorConfig, SENSORCONFIG_ADDRESS, sizeof(sensorConfig));
	ExFlash_ReadData(calibConfig, CALIBCONFIG_ADDRESS, sizeof(calibConfig));
	ExFlash_ReadData(alarmConfig, ALARMCONFIG_ADDRESS, sizeof(alarmConfig));
	if(!ExFlash_ReadData(&deviceConfig, DEVICECONFIG_ADDRESS, sizeof(deviceConfig)))
	{
		deviceConfig.slaveBaud = 1;
		deviceConfig.masterBaud = 1;
		deviceConfig.numConfig = 4;
		deviceConfig.brightness = 5;
		deviceConfig.sampleTime = 300;
		deviceConfig.displayTime = 10;
	}
	ExFlash_ReadData(advancedConfig, ADVANCEDCONFIG_ADDRESS, sizeof(advancedConfig));
	for(int i = 0; i < deviceConfig.numConfig; i++)
	{
		Kalman_SetProcessNoise(i, advancedConfig[i].filter);
	}
	if(!ExFlash_ReadData(&passwordConfig, PASSWORDCONFIG_ADDRESS, sizeof(passwordConfig)))
			passwordConfig.password = DEFAULT_PASSWORD;
}

void LCD_Init(void)
{
	HAL_TIM_Base_Start(TIMER_US);
	Buzzer_Control(deviceConfig.keypadBell);
	ST7920_SPI.setBackLight(deviceConfig.brightness);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	ST7920_SPI.init();// custom fillRect function and screen width and height values
//	RREFont.init(customRect, SCR_WD, SCR_HT);
//	RREFont.setBold(1);
//	RREFont.setFont(&rre_Bold13x20no);
//	RREFont.printStr(ALIGN_CENTER,0,(char*)"22.33");
//	ST7920_SPI.display(0);
	ST7920_SPI.setFont(defaulFont);
	ST7920_SPI.cls();
	ST7920_SPI.display(0);

}

void LCD_StartupPage(void)
{
	ST7920_SPI.cls();
	ST7920_SPI.display(0);
	ST7920_SPI.drawBitmap(EoH_Logo, (SCR_WD-WD_LOGO)/2 -1 , Y_LOGO);
	ST7920_SPI.drawRect(X_BAR, Y_BAR, SCR_WD-X_BAR*2, HT_BAR, 1);
	ST7920_SPI.printStr(X_BAR, Y_BAR - 9,(char*) "Loading...",false);
	for(int i = 1; i < 101; i++)
	{
		float percent = i*(SCR_WD-X_BAR*2-1)/100;
		ST7920_SPI.fillRect(X_BAR+1, Y_BAR+1, (uint8_t)percent, HT_BAR-2, 1);
		char strPercent[20] = {0};
		sprintf(strPercent, "%d", i);
		strcat(strPercent,(char*)"%");
		ST7920_SPI.printStr(X_BAR + SCR_WD-X_BAR*2 - 20, Y_BAR - 9,(char*) strPercent,true);
		ST7920_SPI.display(0);
	}
}

static void ConvertFloatToString(char * _buff, float _number, uint8_t _point)
{
	switch(_point)
	{
	case 0:
		sprintf(_buff, "%.0f", _number);
		break;
	case 1:
		sprintf(_buff, "%.1f", _number);
		break;
	case 2:
		sprintf(_buff, "%.2f", _number);
		break;
	case 3:
		sprintf(_buff, "%.3f", _number);
		break;
	default:
		break;
	}
}

static void PrintFloatExcept(uint8_t _xpos, uint8_t _ypos, float _number, uint8_t _point)
{
	char _buff[10] = {0};
	ConvertFloatToString(_buff, _number, _point);
	ST7920_SPI.printStrInvert(_xpos, _ypos ,(char*) _buff,true);
}

static void PrintFloat(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, float  _number, uint8_t _point, bool invert = true)
{
	char _buff[10] = {0};
	ConvertFloatToString(_buff, _number, _point);
	ST7920_SPI.deleteString(infoPage.xPosInvert[xValue], infoPage.yPos[yValue], infoPage.invertWide[xValue], OFFSET_INVERT);
	ST7920_SPI.printStrInvert(infoPage.xPosValue[xValue], infoPage.yPos[yValue] ,(char*) _buff,false);
	if(invert)
	{
		ST7920_SPI.invertString(infoPage.xPosInvert[xValue], infoPage.yPos[yValue], infoPage.invertWide[xValue], OFFSET_INVERT);
	}
}

static void PrintNumber(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, int16_t _number, bool invert = true, bool isPassword = false)
{
	char _buff[10] = {0};
	if(!isPassword)
		sprintf(_buff, "%d", _number);
	else
	{
		unsigned int num = _number; //for example
		unsigned int dig = 4;
		char _temp[4] = {0};
		while (dig--) {
			_temp[dig]=num%10;
		 num/=10;
		}
		sprintf( _buff, "%d %d %d %d", _temp[0], _temp[1], _temp[2], _temp[3]);
	}
	ST7920_SPI.deleteString(infoPage.xPosInvert[xValue], infoPage.yPos[yValue], infoPage.invertWide[xValue], OFFSET_INVERT);
	ST7920_SPI.printStrInvert(infoPage.xPosValue[xValue], infoPage.yPos[yValue] ,(char*) _buff,false);
	if(invert)
	{
		ST7920_SPI.invertString(infoPage.xPosInvert[xValue], infoPage.yPos[yValue], infoPage.invertWide[xValue], OFFSET_INVERT);
	}
}

static void PrintString(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage,const char * pString, bool invert = true)
{
	char _buff[10] = {0};
	sprintf(_buff, "%s", pString);
	ST7920_SPI.deleteString(infoPage.xPosInvert[xValue], infoPage.yPos[yValue], infoPage.invertWide[xValue], OFFSET_INVERT);
	ST7920_SPI.printStrInvert(infoPage.xPosValue[xValue], infoPage.yPos[yValue] ,(char*) _buff,false);
	if(invert)
	{
		ST7920_SPI.invertString(infoPage.xPosInvert[xValue], infoPage.yPos[yValue], infoPage.invertWide[xValue], OFFSET_INVERT);
	}
}

static void DisplayMainTitle(const char * const pString)
{
	ST7920_SPI.setFont(titleFont);
	uint16_t _xValue = (SCR_WD - ST7920_SPI.strWidth((char*)pString))/2;
	uint16_t _yValue = Y_MAIN_TITLE;
//	ST7920_SPI.drawRect(_xValue - 3, _yValue - 3 , ST7920_SPI.strWidth((char*)pString) + 5 , 8 + 5, 1);
	ST7920_SPI.drawLineHfast(0, SCR_WD - 1, Y_MAIN_TITLE + 8, 1);
	ST7920_SPI.printStr(_xValue, _yValue ,(char*) pString,false);
	ST7920_SPI.setFont(defaulFont);
}


static void DisplayValueModbusConfig(uint8_t _row, uint8_t _col ,InfoPage infoPage, const ModbusConfig * _modbusConfig)
{
	uint16_t *_pTemp = (uint16_t*) _modbusConfig;
	switch (_col)
	{
	case Addr:
	case Func:
	case RegID:
	case Leng:
	case Scale:
		PrintNumber(_col, _row, infoPage, _pTemp[_row * (infoPage.numCol) + _col], false);
		break;
	case Type:
		PrintString(_col, _row, infoPage, dataType[_pTemp[_row * (infoPage.numCol) + _col]], false);
		break;
	default:
		break;
	}
}

static void DisplayValueSensorConfig(uint8_t _row, uint8_t _col, InfoPage infoPage, const SensorConfig * _sensorConfig)
{
	uint32_t *_pTemp = (uint32_t*) _sensorConfig;
	uint8_t _point = sensorConfig[_row].numPoint;
	switch (_col)
	{
	case Name:
		PrintString(_col, _row, infoPage, nameSensor[(uint32_t) _pTemp[_row * (infoPage.numCol) + _col]], false);
		break;
	case Average:
	case Point:
		PrintNumber(_col, _row, infoPage, _pTemp[_row * (infoPage.numCol) + _col], false);
		break;
	case MaxValue:
	case MinValue:
		PrintFloat(_col, _row, infoPage, ((float*) _pTemp)[_row * (infoPage.numCol) + _col], _point, false);
		break;
	default:
		break;
	}

}

static void DisplayValueCalibConfig(uint8_t _row, uint8_t _col, InfoPage infoPage, const CalibConfig *_calibConfig)
{
	uint32_t *_pTemp = (uint32_t*) _calibConfig;
	uint8_t _point = sensorConfig[_row].numPoint;
	switch (_col)
	{
//	case Value:
	case Offset:
		PrintFloat(_col, _row, infoPage, ((float*) _pTemp)[_row * (infoPage.numCol) + _col], _point, false);
		break;
//	case Scale:
//		PrintNumber(_col, _row, infoPage, _pTemp[_row * (infoPage.numCol) + _col], false);
//		break;
	default:
		break;
	}

}

static void DisplayValueAlarmConfig(uint8_t _row, uint8_t _col, InfoPage infoPage, const AlarmConfig *_alarmConfig)
{
	uint32_t *_pTemp = (uint32_t*) _alarmConfig;
	uint8_t _point = sensorConfig[_row].numPoint;
	switch (_col)
	{
	case LowValue:
	case HighValue:
		PrintFloat(_col, _row, infoPage, ((float*) _pTemp)[_row * (infoPage.numCol) + _col], _point, false);
		break;
	default:
		break;
	}

}

static void DisplayValueAdvancedConfig(uint8_t _row, uint8_t _col, InfoPage infoPage,const AdvancedConfig * _advancedConfig)
{
	uint32_t *_pTemp = (uint32_t*) _advancedConfig;
	uint8_t _point = sensorConfig[_row].numPoint;
	switch (_col)
	{
//	case Value:
	case Filter:
		PrintFloat(_col, _row, infoPage, ((float*) _pTemp)[_row * (infoPage.numCol) + _col], _point, false);
		break;
//	case Scale:
//		PrintNumber(_col, _row, infoPage, _pTemp[_row * (infoPage.numCol) + _col], false);
//		break;
	default:
		break;
	}

}

static void DisplayValueConfig(InfoPage infoPage, uint8_t _pageNumber, uint8_t _subPage = SUBPAGE_1)
{
	uint8_t startRow = 0;
	uint8_t endRow = 0;
	//clear value page
	ST7920_SPI.fillRect(X_CLEAR_PAGE, Y_CLEAR_PAGE, SCR_WD - X_CLEAR_PAGE - 1, SCR_HT - Y_CLEAR_PAGE - 1, 0);
	if (_subPage == SUBPAGE_2)
	{
		startRow = ROW_PAGE;
		endRow = 2 * ROW_PAGE;
	}
	else if (_subPage == SUBPAGE_1)
	{
		startRow = 0;
		endRow = ROW_PAGE;
	}
	for (int i = startRow; i < endRow; i++)
	{
		uint8_t _xValue = X_SUB_TITLE + 2;
		uint8_t _yValue = infoPage.yPos[i];
		switch (_pageNumber)
		{
		case CALIB_CONFIG_PAGE:
		case ADVANCED_CONFIG_PAGE:
			ST7920_SPI.printStr(X_SUB_TITLE + 40, Y_SUB_TITLE, (char*) "Name", false);
			ST7920_SPI.printStr(_xValue + 40, _yValue, (char*) nameSensor[sensorConfig[i].name], true);
			break;
		case ALARM_CONFIG_PAGE:
			ST7920_SPI.printStr(X_SUB_TITLE + 2, Y_SUB_TITLE, (char*) "Name", false);
			ST7920_SPI.printStr(_xValue + 2, _yValue, (char*) nameSensor[sensorConfig[i].name], true);
			break;
		default:
			char _char[2] =
			{ 0 };
			sprintf(_char, "%d", i + 1);
			ST7920_SPI.printStr( X_SUB_TITLE, Y_SUB_TITLE, (char*) "Id", false);
			ST7920_SPI.printStr(_xValue, _yValue, (char*) _char, true);
			break;
		}

		for (int j = 0; j < infoPage.numCol; j++)
		{
			switch (_pageNumber)
			{
			case MODBUS_CONFIG_PAGE:
				DisplayValueModbusConfig(i, j, infoPage, modbusConfig);
				break;
			case SENSOSR_CONFIG_PAGE:
				DisplayValueSensorConfig(i, j, infoPage, sensorConfig);
				break;
			case CALIB_CONFIG_PAGE:
				DisplayValueCalibConfig(i, j, infoPage, calibConfig);
				break;
			case ALARM_CONFIG_PAGE:
				DisplayValueAlarmConfig(i, j, infoPage, alarmConfig);
				break;
			case ADVANCED_CONFIG_PAGE:
				DisplayValueAdvancedConfig(i, j, infoPage, advancedConfig);
				break;
			default:
				break;

			}
		}
	}
}

static void DisplayValueDevice(InfoPage infoPage)
{
	uint16_t * _pTemp = (uint16_t*) &deviceConfig;
	uint8_t _index;
	for (int i = 0; i < infoPage.numCol; i++)
	{
		for (int j = 0; j < infoPage.numRow; j++)
		{
			_index = infoPage.numRow * i + j;
			switch (_index)
			{
			case SlaveAddr:
			case NumConfig:
			case SampleTime:
			case DisplayTime:
			case Brightness:
				PrintNumber(i, j, infoPage, _pTemp[_index], false);
				break;
			case KeypadBell:
				PrintString(i, j, infoPage, titleKeypadBell[_pTemp[_index]], false);
				break;
			case SlaveBaud:
			case MasterBaud:
				PrintString(i, j, infoPage, deviceBaud[_pTemp[_index]], false);
				break;
			default:
				break;
			}
		}
	}
}

static void DisplayHealthy(InfoPage infoPage)
{
	uint8_t _index = 0;
	char _buff[10] = {0};
	for (int i = 0; i < infoPage.numCol; i++)
	{
		for (int j = 0; j < infoPage.numRow; j++)
		{
			_index = infoPage.numRow * i + j;
			switch (_index)
			{
			case Voltage:
				sprintf(_buff, "%0.2f", ((float)Network_GetVoltage()/100));
				strcat(_buff, "  V");
				PrintString(i, j, infoPage, _buff, false);
				break;
			case CPU_Temp:
				sprintf(_buff, "%0.2f", ((float)Network_GetCPUTemp()/100));
				strcat(_buff, " ~C");
				PrintString(i, j, infoPage, _buff, false);
				break;
			case CPU_Usage:
				sprintf(_buff, "%d", osGetCPUUsage());
				strcat(_buff, "   %");
				PrintString(i, j, infoPage, _buff, false);
				break;
			case ResetCode:
				PrintString(i, j, infoPage, ReasonResetStr[(uint8_t)lastResetReason], false);
				break;
			case ModbusFR:
				sprintf(_buff, "%d/%d",modbusFailrate, modbusTotal);
				PrintString(i, j, infoPage, _buff, false);
				break;
			case NetSignal:
				PrintNumber(i, j, infoPage,networkSignal, false);
				break;
			case NetStatus:
				sprintf(_buff, "%s-%d", autoSwitch ? "WIFI" : " SIM", networkStatus);
				PrintString(i, j, infoPage, _buff, false);
				break;
			case FirmVersion:
				PrintString(i, j, infoPage, "1.0", false);
				break;
			default:
				break;
			}
			memset(_buff, 0, sizeof(_buff));
		}
	}
}

static void DisplaySubTitleAndValueWriteConfig(const char ** pString, InfoPage infoPage)
{
	uint32_t * _pTemp = (uint32_t*) &writeConfig;
	uint8_t _col = 0;
	for(int i = 0; i < infoPage.numRow - 1; i++)
	{
		if(i >= ROW_PAGE)
			_col = 1;
		ST7920_SPI.printStr(infoPage.xPosTitle[_col], infoPage.yPos[i] ,(char*) pString[i]);
		switch(i)
		{
		case Address:
		case RegisterID:
		case ValueWrite:
			PrintNumber(_col, i, infoPage, _pTemp[i], false);
			break;
		case Function:
			PrintString(_col, i, infoPage, writeFunction[_pTemp[i]], false);
			break;
		case DataType:
			PrintString(_col, i, infoPage, dataTypeWCFG[_pTemp[i]], false);
			break;
		}
	}
	ST7920_SPI.drawLineVfast(SCR_WD/2, infoPage.yPos[0] - 2, SCR_HT - 1, 1);
	ST7920_SPI.setFont(titleFont);
	uint8_t _space = 5;
	uint8_t _xpos = (3*SCR_WD/4) - (WD_WRITE/2);
	uint8_t _ypos = (SCR_HT - Y_1ST_VALUE- HT_WRITE)/2 + Y_1ST_VALUE ;
	ST7920_SPI.printStr(_xpos + _space, _ypos + _space,(char*) "WRITE");
	ST7920_SPI.drawRect( _xpos, _ypos , WD_WRITE, HT_WRITE, 1);
	xWrite = _xpos;
	yWrite = _ypos;
	ST7920_SPI.setFont(defaulFont);
	ST7920_SPI.printStr(_xpos - 9 , _ypos + 22, (char*)"Response:", 0);
	xRes = _xpos + 51;
	yRes = _ypos + 23;


}


//#define CAL_POSITON
#ifdef CAL_POSITON
static void CalculateSubTitle(const char **  pString, InfoPage * infoPage)
{
	uint16_t _xValue = 20;
	uint16_t _yValue = Y_SUB_TITLE;
	uint8_t _spaceCol = 0;
	uint16_t _totalSpace = 0;
	for(int i = 0 ; i < infoPage->numCol; i++)
	{
		_totalSpace += (uint16_t) ST7920_SPI.strWidth((char*)pString[i]);
	}
	_spaceCol = (SCR_WD - _totalSpace - _xValue ) / (infoPage->numCol-1);
	for(int i = 0; i < infoPage->numCol; i++)
	{
		infoPage->xPosTitle[i] = _xValue;
		ST7920_SPI.printStr(_xValue, _yValue ,(char*) pString[i],false);
		_xValue += ST7920_SPI.strWidth((char*)pString[i]) + _spaceCol;
	}

	_xValue = X_SUB_TITLE + 2;
	_yValue = Y_SUB_TITLE + 2;
	_totalSpace = 8 * (infoPage->numRow -1);
	_spaceCol = (SCR_HT - _totalSpace - Y_SUB_TITLE - 8) / (infoPage->numRow );
	for(int i = 0; i < infoPage->numRow; i++)
	{
		char _char[2] = {0};
		sprintf(_char, "%d", i + 1);
		infoPage->yPos[i] = _yValue;
//		ST7920_SPI.printStr(_xValue, _yValue ,(char*) _char,false);
		_yValue += 8 + _spaceCol;
	}
}
#endif
static void DisplaySubTitle(const char **  pString, InfoPage infoPage)
{
	uint16_t _xValue = X_SUB_TITLE;
	uint16_t _yValue = Y_SUB_TITLE;
//	ST7920_SPI.printStr(_xValue, _yValue ,(char*)"Id",false);
	for(int i = 0; i < infoPage.numCol; i++)
	{
		_xValue = infoPage.xPosTitle[i];
		ST7920_SPI.printStr(_xValue, _yValue ,(char*) pString[i],false);
	}
}

static void DisplaySubTitleDevice(const char ** pString, InfoPage infoPage)
{
	for(int i = 0; i < infoPage.numCol; i++)
		for(int j = 0; j < infoPage.numRow; j++)
			ST7920_SPI.printStr(infoPage.xPosTitle[i], infoPage.yPos[j] ,(char*) pString[i*infoPage.numRow + j]);
	ST7920_SPI.drawLineVfast(SCR_WD/2, infoPage.yPos[0] - 2, SCR_HT - 1, 1);
}

static int8_t CalibRange(int16_t maxValue, int16_t minValue, int16_t value)
{
	if(value > maxValue)
		return minValue;
	if(value < minValue)
		return maxValue;
	return value;
}

static int8_t IsButton(uint8_t _index ,InfoPage _infoPage)
{
	for(int i = 0; i < _infoPage.numButton; i++)
	{
		if(_index == _infoPage.listButton[i])
			return i;
	}
	return -1;
}

static void InvertString(uint8_t _xpos, uint8_t _ypos, InfoPage _infoPage)
{
	int8_t _return = IsButton(_ypos, _infoPage);
	uint8_t _col = _xpos;
	if(_infoPage.rowPerCol == 0)
	{
		if(_ypos >= ROW_PAGE && !_infoPage.multipage)
			_col = 1;
	}
	else
	{
		if(_ypos >= _infoPage.rowPerCol && !_infoPage.multipage)
			_col = 1;
	}
	if( _return >= 0)
		ST7920_SPI.fillRect( _infoPage.xButton[_return] + 1, _infoPage.yButton[_return] + 1, _infoPage.wButton[_return] - 2 , _infoPage.hButton[_return] - 2, 2);
	else
	{
		ST7920_SPI.invertString(_infoPage.xPosInvert[_col], _infoPage.yPos[_ypos], _infoPage.invertWide[_xpos]);
	}

}


static void MoveCursor(InfoPage infoPage, KeyPad _key, uint8_t _pageNumber)
{
	uint8_t maxRow = infoPage.numRow - 1;
	uint8_t minRow = 0;

	InvertString(xCursor, yCursor , infoPage);

	switch((uint8_t)_key)
	{
	case KEY_UP:
		yCursor--;
		if(yCursor == -1 && infoPage.multipage)
			DisplayValueConfig(infoPage, _pageNumber, SUBPAGE_2);
		else if(yCursor == (ROW_PAGE - 1) && infoPage.multipage)
			DisplayValueConfig(infoPage, _pageNumber, SUBPAGE_1);
		yCursor = CalibRange(maxRow, minRow, yCursor);
		break;
	case KEY_DOWN:
		yCursor++;
		if(yCursor == ROW_PAGE  && infoPage.multipage)
			DisplayValueConfig(infoPage, _pageNumber, SUBPAGE_2);
		else if(yCursor == 2*ROW_PAGE && infoPage.multipage)
			DisplayValueConfig(infoPage, _pageNumber, SUBPAGE_1);
		yCursor = CalibRange(maxRow, minRow, yCursor);
		break;
	case KEY_LEFT:
		xCursor--;
		xCursor = CalibRange(infoPage.numCol - 1, 0, xCursor);
		break;
	case KEY_RIGHT:
		xCursor++;
		xCursor = CalibRange(infoPage.numCol - 1, 0, xCursor);
		break;
	default:
		break;
	}
	InvertString(xCursor, yCursor , infoPage);
	ST7920_SPI.display(0);
//	ST7920_SPI.invertString(infoPage.xPosInvert[xCursor], infoPage.yPos[yCursor % ROW_PAGE], infoPage.invertWide, OFFSET_INVERT);
}

static int32_t inRangeConfig(int32_t maxValue, int32_t minValue, int32_t value, bool flag)
{
	int32_t _temp = value;
	if(flag)
	{
		if (value > maxValue)
			_temp = maxValue;
		else if (value < minValue)
			_temp = minValue;
	}
	else
	{
		if(value > maxValue)
			_temp = minValue;
		else if (value < minValue)
			_temp = maxValue;
	}
	memset(numBuffer, 0, sizeof(numBuffer));
	sprintf(numBuffer, "%ld", _temp);
	return _temp;
}

static float inRangeConfigFloat(float maxValue, float minValue, float value, uint8_t point, bool flag)
{
	float _temp = value;
	if(flag)
	{
		if (value > maxValue)
			_temp = maxValue;
		else if (value < minValue)
			_temp = minValue;
	}
	else
	{
		if(value > maxValue)
			_temp = minValue;
		else if (value < minValue)
			_temp = maxValue;
	}
	memset(numBuffer, 0, sizeof(numBuffer));
	sprintf(numBuffer, "%ld", (int32_t)(roundf(_temp*pow(10, point))));
	return _temp;
}

static int32_t GetOldData(char * pString, int32_t value)
{
	if(strlen(pString))
		return atoi(pString);
	else
		return value;
}

static float GetOldDataFloat(char * pString, float _temp, uint16_t _pow)
{
	if(strlen(pString))
	{
		_temp = (float)atoi(pString);
		return _temp;
	}
	else
		return _temp*_pow;
}

static bool SensorConfigIsFloat(uint8_t _value)
{
	switch(_value)
	{
	case Name:
	case Point:
	case Average:
		return 0;
//	case Const:
	case MaxValue:
	case MinValue:
		return 1;
	}
	return 0;
}

static bool CalibConfigIsFloat(uint8_t _value)
{
	switch(_value)
	{
//	case Value:
	case Offset:
		return 1;
//	case Scale:
//		return 0;
	}
	return 0;
}

static bool AlarmConfigIsFloat(uint8_t _value)
{
	switch(_value)
	{
	case LowValue:
	case HighValue:
		return 1;
	}
	return 0;
}

static bool WriteConfigIsFloat(uint8_t _value)
{
	switch(_value)
	{
	case Address:
	case Function:
	case RegisterID:
	case DataType:
		return 0;
	case ValueWrite:
		if(writeConfig.dataType == FLOAT1 || writeConfig.dataType == FLOAT2)
			return 1;
		else
			return 0;
	}
	return 0;
}

static bool AdvancedConfigIsFloat(uint8_t _value)
{
	switch(_value)
	{
		case Filter:
			return 1;
	}
	return 0;
}

static void ChangeValueWriteConfig(uint8_t _type, char * _pStr, bool _flagMode, uint32_t _number, float _fNumber = 0.000f, uint8_t _point = 3)
{
	uint32_t _result;
	float _fResult;
	if(!_pStr)
		return;
	switch(_type)
	{
	case UINT16:
		_result = inRangeConfig(0xFFFF, 0, _number, _flagMode);
		sprintf(_pStr, "%d", (uint16_t)_result);
		break;
	case INT16:
		_result = inRangeConfig(32767, -32768, _number, _flagMode);
		sprintf(_pStr, "%d", (int16_t)_result);
		break;
	case BOOL:
		_result = inRangeConfig(OFF, ON, _number, _flagMode);
		strcpy(_pStr ,writeCoil[_result]);
		break;
		//Float - Big Endian
	case FLOAT1:
		//Float - Mid-Little Endian
	case FLOAT2:
		_fResult = inRangeConfigFloat(999.999f, 0.000f, _fNumber, _point, _flagMode);
		ConvertFloatToString(_pStr, _fResult, _point);
		break;
		//Uint32 - Big Endian
	case UINT32:
		_result = inRangeConfig(999999, 0, _number, _flagMode);
//		_result = convert2MidLittleEndian(_result);
		sprintf(_pStr, "%ld", (uint32_t)_result);
		break;
	default:
		break;
	}

}

static void WriteConfigHandle(void)
{
	uint8_t _flagQueue = 1;
	ST7920_SPI.deleteString(xRes, yRes, 4);
	ST7920_SPI.printStrInvert(xRes, yRes,(char*) "----");
	ST7920_SPI.display(0);
	osDelay(100);
	_flagQueue = writeConfig.func;
	osMessageQueuePut(writeConfigQueueHandle, &_flagQueue, 0U, 0U);
	while (osMessageQueueGet(responseWriteQueueHandle, &_flagQueue, NULL, 1) != osOK)
	{
		osDelay(10);
	}
	ST7920_SPI.setFont(writeFont);
	ST7920_SPI.deleteString(xRes, yRes, 4);
	if(_flagQueue)
		ST7920_SPI.printStrInvert(xRes, yRes,(char*) "OK");
	else
		ST7920_SPI.printStrInvert(xRes, yRes,(char*) "FAIL");
	ST7920_SPI.setFont(defaulFont);
	ST7920_SPI.display(0);
}

static bool EnterPasswordHandle(void)
{
	if(tempPassword == passwordConfig.password)
		return true;
	else
	{
		ST7920_SPI.setFont(defaulFont);
		ST7920_SPI.printStr(X_LOCK , Y_LOCK + 35, (char*)"Incorrect!!!", 0);
		ST7920_SPI.setFont(titleFont);
		ST7920_SPI.display(0);
		return false;
	}
}

static void ChangePasswordHandle(void)
{
	ST7920_SPI.setFont(defaulFont);
	ST7920_SPI.deleteString(X_CHANGE + 35, Y_CHANGE + 44, 13, 0);
	if(changePasswordConfig.currentPassword == passwordConfig.password)
	{
		ST7920_SPI.printStr(X_CHANGE - 21 , Y_CHANGE + 44, (char*)"Success!!!", 0);
		passwordConfig.password = changePasswordConfig.newPassword;
		ExFlash_WriteData(&passwordConfig, PASSWORDCONFIG_ADDRESS, sizeof(passwordConfig));
	}
	else
	{
		ST7920_SPI.printStr(X_CHANGE - 21 , Y_CHANGE + 44, (char*)"Incorrect!!!", 0);
	}
	ST7920_SPI.setFont(titleFont);
	ST7920_SPI.display(0);
}

static void ResetPasswordHandle(void)
{
	ST7920_SPI.setFont(defaulFont);
	ST7920_SPI.deleteString(X_CHANGE + 35, Y_CHANGE + 44, 13, 0);
	ST7920_SPI.printStr(X_CHANGE - 21 , Y_CHANGE + 44, (char*)"Reset!!!", 0);
	ST7920_SPI.setFont(titleFont);
	passwordConfig.password = 1;
	ExFlash_WriteData(&passwordConfig, PASSWORDCONFIG_ADDRESS, sizeof(passwordConfig));
	ST7920_SPI.display(0);
}

static void BCTurHandle(uint8_t _index)
{
	uint8_t _id = turCalibConfig.id - 1;
	ModbusCalib _modbusCalib = {0};
	_modbusCalib.type = NO_RESPONSE;
	_modbusCalib.command.addr = modbusRTU[_id].addr;
	_modbusCalib.command.func = 0x06;
	switch(_index)
	{
	case DecPointTurBC:
		_modbusCalib.command.regID = 0x0112;
		_modbusCalib.command.length = standardTurBC.point;
		break;
	case SensCalibTurBC:
		_modbusCalib.command.regID = 0x0113;
		_modbusCalib.command.length = standardTurBC.sensCalib;
		break;
	case SensCmdTurBC:
		_modbusCalib.type = TUR_RESPONSE;
		_modbusCalib.command.regID = 0x0114;
		if(standardTurBC.sensCmd == 0)
			_modbusCalib.command.length = 0x5300;
		else
			_modbusCalib.command.length = 0x5352;
		_modbusCalib.response.addr =  modbusRTU[_id].addr;
		_modbusCalib.response.func = 0x03;
		_modbusCalib.response.regID = 0x0114;
		_modbusCalib.response.length = 0x01;
		break;
	}
	osMessageQueuePut(modbusCalibQueueHandle, &_modbusCalib, 0U, 0U);
}

static void BCCloHandle(uint8_t _index)
{
	uint8_t _id = cloCalibConfig.id - 1;
	ModbusCalib _modbusCalib = {0};
	_modbusCalib.type = NO_RESPONSE;
	_modbusCalib.command.addr = modbusRTU[_id].addr;
	_modbusCalib.command.func = 0x06;
	switch(_index)
	{
	case SensPointClo:
		_modbusCalib.command.regID = 0x0112;
		_modbusCalib.command.length = standardCloBC.sensPoint;
		break;
	case SensCalibClo:
		_modbusCalib.command.regID = 0x0113;
		_modbusCalib.command.length = standardCloBC.sensCalib;
		break;
	case SensCmdClo:
		_modbusCalib.type = CLO_SENS_RESPONSE;
		_modbusCalib.command.regID = 0x0114;
		if(standardCloBC.sensCmd == 0)
			_modbusCalib.command.length = 0x5300;
		else
			_modbusCalib.command.length = 0x5352;
		_modbusCalib.response.addr =  modbusRTU[_id].addr;
		_modbusCalib.response.func = 0x03;
		_modbusCalib.response.regID = 0x0114;
		_modbusCalib.response.length = 0x01;
		break;
	case ZeroDecPointClo:
		_modbusCalib.command.regID = 0x0100;
		_modbusCalib.command.length = standardCloBC.zeroPoint;
		break;
	case ZeroCalibClo:
		_modbusCalib.command.regID = 0x0101;
		_modbusCalib.command.length = standardCloBC.zeroCalib;
		break;
	case ZeroCmdClo:
		_modbusCalib.type = CLO_ZERO_RESPONSE;
		_modbusCalib.command.regID = 0x0102;
		if(standardCloBC.zeroCmd == 0)
			_modbusCalib.command.length = 0x5A00;
		else
			_modbusCalib.command.length = 0x5A52;
		_modbusCalib.response.addr =  modbusRTU[_id].addr;
		_modbusCalib.response.func = 0x03;
		_modbusCalib.response.regID = 0x0102;
		_modbusCalib.response.length = 0x01;
		break;
	}
	osMessageQueuePut(modbusCalibQueueHandle, &_modbusCalib, 0U, 0U);
}

static void UpdateTurValueHandle(void)
{
	uint8_t _flag = 0;
	uint8_t _index = turCalibConfig.id - 1;

	if (osMessageQueueGet(displayStatusCalibHandle, &_flag, NULL, 1) == osOK)
	{
		if(_flag == _index)
		{
			if(processData[_index].failFlag)
				standardTurBCUpdate.turValue = processData[_index].value;
			else
				standardTurBCUpdate.turValue = 0.0f;
			PrintFloat(0, 4, bcTurCalibConfigPage, standardTurBCUpdate.turValue, sensorConfig[_index].numPoint, false);
			ST7920_SPI.display(0);
		}
	}
	ResponseCalib _responseCalib = {0};
	if(osMessageQueueGet(responseStatusCalibQueueHandle, &_responseCalib, NULL, 1) == osOK)
	{
		if(_responseCalib.type != TUR_RESPONSE)
			return;
		standardTurBCUpdate.sensStatus = _responseCalib.status;
		if(_responseCalib.flag)
		{
			PrintString(0, 3, bcTurCalibConfigPage, titlestatusTurCalib[_responseCalib.status], false);
		}
		else
		{
			PrintString(0, 3, bcTurCalibConfigPage, titlestatusTurCalib[2], false);
		}
		ST7920_SPI.display(0);
	}
}

static void UpdateCloValueHandle(void)
{
	uint8_t _flag = 0;
	uint8_t _index = cloCalibConfig.id - 1;

	if (osMessageQueueGet(displayStatusCalibHandle, &_flag, NULL, 1) == osOK)
	{
		if(_flag == _index )
		{
			if(processData[_index].failFlag)
				standardCloBCUpdate.cloValue = processData[_index].value;
			else
				standardCloBCUpdate.cloValue = 0.0f;
			ST7920_SPI.deleteString(xValueCloBC, bcCloCalibConfigPage.yPos[4], 6, OFFSET_INVERT);
			PrintFloatExcept(xValueCloBC, bcCloCalibConfigPage.yPos[4], standardCloBCUpdate.cloValue, sensorConfig[_index].numPoint);
			ST7920_SPI.display(0);
		}
	}
	ResponseCalib _responseCalib = {0};
	if(osMessageQueueGet(responseStatusCalibQueueHandle, &_responseCalib, NULL, 1) == osOK)
	{
		if(_responseCalib.type == CLO_ZERO_RESPONSE)
		{
			standardCloBCUpdate.zeroStatus = _responseCalib.status;
			if(_responseCalib.flag)
			{
				PrintString(0, 3, bcCloCalibConfigPage, titlestatusCloCalib[_responseCalib.status], false);
			}
			else
			{
				PrintString(0, 3, bcCloCalibConfigPage, titlestatusCloCalib[2], false);
			}
			ST7920_SPI.display(0);
		}
		else if(_responseCalib.type == CLO_SENS_RESPONSE)
		{
			standardCloBCUpdate.sensStatus = _responseCalib.status;
			if(_responseCalib.flag)
			{
				PrintString(1, 3, bcCloCalibConfigPage, titlestatusCloCalib[_responseCalib.status], false);
			}
			else
			{
				PrintString(1, 3, bcCloCalibConfigPage, titlestatusCloCalib[2], false);
			}
			ST7920_SPI.display(0);
		}
	}

}

static void UpdateValueCalibHandle(uint8_t _page)
{
	switch(_page)
	{
	case BCTUR_CALIBRATION_PAGE:
		UpdateTurValueHandle();
		break;
	case BCCLO_CALIBRATION_PAGE:
		UpdateCloValueHandle();
		break;
	}
}

static bool IsPageCalib(uint8_t _page)
{
	bool _flag = (_page == BCTUR_CALIBRATION_PAGE) || (_page == BCCLO_CALIBRATION_PAGE);
	return _flag;
}

static uint8_t ESC_Step0_Handle(uint8_t _page)
{
	if(_page == CHANGE_PASSWORD_PAGE)
		return ENTER_PASSWORD_PAGE;
	else if(_page == BCTUR_CALIBRATION_PAGE)
		return TUR_CALIBRATION_PAGE;
	else if(_page == BCCLO_CALIBRATION_PAGE)
		return CLO_CALIBRATION_PAGE;
	return MODBUS_VALUE_PAGE;
}

static int16_t ModbusConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue,IsFloatHandle _isFloatHandle)
{
	int16_t _number = 0;
	bool _flagMode = false;
	uint8_t _step = 2;
	int16_t * _pTemp = (int16_t*)modbusConfig;
	uint8_t _index = xValue + yValue * (infoPage.numCol);
	_number = GetOldData(numBuffer, _pTemp[_index]);
	switch (keyValue)
	{
	case KEY_LEFT:
//		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number--;
		_flagMode = false;
		break;
	case KEY_RIGHT:
//		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number++;
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		 _pTemp[_index] = _number;
		 LCD_CopyModbusConfig(modbusRTU, modbusConfig);
		 ExFlash_WriteData(modbusConfig, MODBUSCONFIG_ADDRESS, sizeof(modbusConfig));
		return _step;
	case KEY_CLR:
		_number = 0;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		break;
	default:
		return _step;
	}
	switch (xValue)
	{
	case Addr:
		_number = inRangeConfig(247, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case Func:
		_number = inRangeConfig(4, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case RegID:
		_number = inRangeConfig(999, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case Leng:
		_number = inRangeConfig(99, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case Scale:
		_number = inRangeConfig(3, -3, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case Type:
		_number = inRangeConfig(FLO2, UI16, _number, _flagMode);
		PrintString(xValue, yValue, infoPage, dataType[_number]);
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t SensorConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue, IsFloatHandle _isFloatHandle)
{
	int32_t _number = 0;
	bool _flagMode = false;
	uint8_t _point = sensorConfig[yValue].numPoint;
	uint16_t _pow = pow(10, _point);
	uint8_t _step = 2;
	float _fTemp = 0;
	int32_t * _pTemp = (int32_t*)sensorConfig;
	uint8_t _index = xValue + yValue * (infoPage.numCol);
	if(_isFloatHandle(xValue))
	{
		_fTemp = GetOldDataFloat(numBuffer, ((float*)_pTemp)[_index], _pow);
	}
	else
	{
		_number = GetOldData(numBuffer, _pTemp[_index]);
	}
	switch (keyValue)
	{
	case KEY_LEFT:
		if(_isFloatHandle(xValue))
		{
			_fTemp--;
			_fTemp = _fTemp/_pow;
		}
		else
		{
			_number--;
		}
		_flagMode = false;
		break;
	case KEY_RIGHT:
		if(_isFloatHandle(xValue))
		{
			_fTemp++;
			_fTemp = _fTemp/_pow;
		}
		else
		{
			_number++;
		}
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		if(_isFloatHandle(xValue))
		{
			((float*)_pTemp)[_index] = _fTemp/_pow;
		}
		else
		{
			_pTemp[_index] = _number;
		}
		if(xValue == Average)
		{
			processData[yValue].index = 0;
		}
		 ExFlash_WriteData(sensorConfig, SENSORCONFIG_ADDRESS, sizeof(sensorConfig));
		 if(xValue == Point)
		 {
				DisplayValueSensorConfig(yValue, MaxValue, infoPage, sensorConfig);
				DisplayValueSensorConfig(yValue, MinValue, infoPage, sensorConfig);
				ST7920_SPI.display(0);
		 }
		return _step;
	case KEY_CLR:
		_number = 0;
		_fTemp = 0.0f;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
//		DisplayValueSensorConfig(infoPage, sensorConfig);
		_fTemp = ((float*)_pTemp)[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		if(_isFloatHandle(xValue))
		{
			_fTemp = ((float)_number)/_pow;
		}
		break;
	default:
		return _step;
	}
	switch (xValue)
	{
	case Name:
		_number = inRangeConfig(6, 0, _number, _flagMode);
		PrintString(xValue, yValue, infoPage, nameSensor[_number]);
		break;
	case Point:
		_number = inRangeConfig(3, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case Average:
		_number = inRangeConfig(20, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case MaxValue:
		_fTemp = inRangeConfigFloat(1000.000f, 0.0f, _fTemp, _point,_flagMode);
		PrintFloat(xValue, yValue, infoPage, _fTemp, _point);
		break;
	case MinValue:
		_fTemp = inRangeConfigFloat(1000.000f, 0.0f, _fTemp, _point, _flagMode);
		PrintFloat(xValue, yValue, infoPage, _fTemp, _point);
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t CalibConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue, IsFloatHandle _isFloatHandle)
{
	int32_t _number = 0;
	bool _flagMode = false;
	uint8_t _point = sensorConfig[yValue].numPoint;;
	uint16_t _pow = pow(10, _point);
	uint8_t _step = 2;
	float _fTemp = 0;
	int32_t * _pTemp = (int32_t*)calibConfig;
	uint8_t _index = xValue + yValue * (infoPage.numCol);
	if(_isFloatHandle(xValue))
	{
		_fTemp = GetOldDataFloat(numBuffer, ((float*)_pTemp)[_index], _pow);
	}
	else
	{
		_number = GetOldData(numBuffer, _pTemp[_index]);
	}
	switch (keyValue)
	{
	case KEY_LEFT:
		if(_isFloatHandle(xValue))
		{
			_fTemp--;
			_fTemp = _fTemp/_pow;
		}
		else
		{
			_number--;
		}
		_flagMode = false;
		break;
	case KEY_RIGHT:
		if(_isFloatHandle(xValue))
		{
			_fTemp = GetOldDataFloat(numBuffer, ((float*)_pTemp)[_index], _pow);
			_fTemp++;
			_fTemp = _fTemp/_pow;
		}
		else
		{
			_number = GetOldData(numBuffer, _pTemp[_index]);
			_number++;
		}
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		if(_isFloatHandle(xValue))
		{
			((float*)_pTemp)[_index] = _fTemp/_pow;
		}
		else
		{
			_pTemp[_index] = _number;
		}
		 ExFlash_WriteData(calibConfig, CALIBCONFIG_ADDRESS, sizeof(calibConfig));
		return _step;
	case KEY_CLR:
		_number = 0;
		_fTemp = 0.0f;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
//		DisplayValueSensorConfig(infoPage, sensorConfig);
		_fTemp = ((float*)_pTemp)[_index];
		break;

	case KEY_ID:
		if(_isFloatHandle(xValue))
		{
			_fTemp = GetOldDataFloat(numBuffer, ((float*)_pTemp)[_index], _pow);
			_fTemp = - _fTemp/_pow;
		}
		else
		{
			_number = - GetOldData(numBuffer, _pTemp[_index]);
		}
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		if(_isFloatHandle(xValue))
		{
			_fTemp = ((float)_number)/_pow;
		}
		break;
	default:
		return _step;
	}
	switch (xValue)
	{
//	case Value:
//		_fTemp = inRangeConfigFloat(99.99, 0, _fTemp, _point,_flagMode);
//		PrintFloat(xValue, yValue, infoPage, _fTemp, 2);
//		break;
//	case Scale:
//		_number = inRangeConfig(3, -3, _number, _flagMode);
//		PrintNumber(xValue, yValue, infoPage, _number);
//		break;
	case Offset:
		_fTemp = inRangeConfigFloat(100.00f, -100.00f, _fTemp, _point,_flagMode);
		PrintFloat(xValue, yValue, infoPage, _fTemp, _point);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t AlarmConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue, IsFloatHandle _isFloatHandle)
{
	int32_t _number = 0;
	bool _flagMode = false;
	uint8_t _point = sensorConfig[yValue].numPoint;;
	uint16_t _pow = pow(10, _point);
	uint8_t _step = 2;
	float _fTemp = 0;
	int32_t * _pTemp = (int32_t*)alarmConfig;
	uint8_t _index = xValue + yValue * (infoPage.numCol);
	if(_isFloatHandle(xValue))
	{
		_fTemp = GetOldDataFloat(numBuffer, ((float*)_pTemp)[_index], _pow);
	}
	else
	{
		_number = GetOldData(numBuffer, _pTemp[_index]);
	}
	switch (keyValue)
	{
	case KEY_LEFT:
		if(_isFloatHandle(xValue))
		{
			_fTemp--;
			_fTemp = _fTemp/_pow;
		}
		else
		{
			_number--;
		}
		_flagMode = false;
		break;
	case KEY_RIGHT:
		if(_isFloatHandle(xValue))
		{
			_fTemp++;
			_fTemp = _fTemp/_pow;
		}
		else
		{
			_number++;
		}
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		if(_isFloatHandle(xValue))
		{
			((float*)_pTemp)[_index] =_fTemp/_pow;
		}
		else
		{
			_pTemp[_index] = _number;
		}
		 ExFlash_WriteData(alarmConfig, ALARMCONFIG_ADDRESS, sizeof(alarmConfig));
		return _step;
	case KEY_CLR:
		_number = 0;
		_fTemp = 0.0f;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		_fTemp = ((float*)_pTemp)[_index];
		break;

	case KEY_ID:
		if(_isFloatHandle(xValue))
		{
			_fTemp = GetOldDataFloat(numBuffer, ((float*)_pTemp)[_index], _pow);
			_fTemp = - _fTemp/_pow;
		}
		else
		{
			_number = - GetOldData(numBuffer, _pTemp[_index]);
		}
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		if(_isFloatHandle(xValue))
		{
			_fTemp = ((float)_number)/_pow;
		}
		break;
	default:
		return _step;
	}
	switch (xValue)
	{
	case LowValue:
	case HighValue:
		_fTemp = inRangeConfigFloat(1000.00f, 0.000f, _fTemp, _point,_flagMode);
		PrintFloat(xValue, yValue, infoPage, _fTemp, _point);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}


static int16_t DeviceConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue,IsFloatHandle _isFloatHandle)
{
	int16_t _number = 0;
	bool _flagMode = false;
	uint8_t _step = 2;
	uint16_t * _pTemp = (uint16_t*)&deviceConfig;
	uint8_t _index = yValue + xValue * infoPage.numRow;
	_number = GetOldData(numBuffer, _pTemp[_index]);
	switch (keyValue)
	{
	case KEY_LEFT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number--;
		_flagMode = false;
		break;
	case KEY_RIGHT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number++;
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		 if(_index == SlaveBaud)
			 ModbusSlave.changeBaudrate(_number);
		 else if(_index == SlaveAddr)
			 ModbusSlave.setUnitAddress(_number);
		 else if(_index == MasterBaud)
			 ModbusMaster.changeBaudrate(_number);
		 else if(_index == Brightness)
			 ST7920_SPI.setBackLight(_number);
		 else if(_index == NumConfig)
		 {
				_number = inRangeConfig(8, 1, _number, true);
				PrintNumber(xValue, yValue, infoPage, _number);
				ST7920_SPI.display(0);
		 }
		 else if(_index == SampleTime)
		 {
				_number = inRangeConfig(9999, 300, _number, true);
				PrintNumber(xValue, yValue, infoPage, _number);
				ST7920_SPI.display(0);
		 }
		 else if(_index == DisplayTime)
		 {
				_number = inRangeConfig(30, 5, _number, true);
				PrintNumber(xValue, yValue, infoPage, _number);
				ST7920_SPI.display(0);
		 }
		 else if(_index == KeypadBell)
		 {
			 Buzzer_Control(_number);
		 }
			_step = 1;
			memset(numBuffer, 0, sizeof(numBuffer));
			 _pTemp[_index] = _number;
		 ExFlash_WriteData(&deviceConfig, DEVICECONFIG_ADDRESS, sizeof(deviceConfig));
		return _step;
	case KEY_CLR:
		_number = 0;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		break;
	default:
		return _step;
	}
	switch (_index)
	{
	case SlaveAddr:
		_number = inRangeConfig(247, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case SlaveBaud:
	case MasterBaud:
		_number = inRangeConfig(3, 0, _number, _flagMode);
		PrintString(xValue, yValue, infoPage, deviceBaud[_number]);
		break;
	case NumConfig:
		_number = inRangeConfig(8, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case SampleTime:
		_number = inRangeConfig(9999, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case DisplayTime:
		_number = inRangeConfig(30, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case Brightness:
		_number = inRangeConfig(5, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case KeypadBell:
		_number = inRangeConfig(1, 0, _number, _flagMode);
		PrintString(xValue, yValue, infoPage, titleKeypadBell[_number]);
		break;
	default:
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t WriteConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue, IsFloatHandle _isFloatHandle)
{
	int32_t _number = 0;
	bool _flagMode = false;
	uint8_t _point = 3;
	uint16_t _pow = pow(10, _point);
	uint8_t _step = 2;
	float _fTemp = 0;
	uint32_t *_pTemp = (uint32_t*) &writeConfig;
	uint8_t _index = yValue;
	if(_isFloatHandle(xValue))
	{
		_fTemp = GetOldDataFloat(numBuffer, ((float*)_pTemp)[_index], _pow);
	}
	else
	{
		_number = GetOldData(numBuffer, _pTemp[_index]);
	}
	char _buf[20] =
	{ 0 };
	switch (keyValue)
	{
	case KEY_LEFT:
		if (_isFloatHandle(_index))
		{
			_fTemp--;
			_fTemp = _fTemp / _pow;
		}
		else
		{
			_number--;
		}
		_flagMode = false;
		break;
	case KEY_RIGHT:
		if (_isFloatHandle(_index))
		{
			_fTemp++;
			_fTemp = _fTemp / _pow;
		}
		else
		{
			_number++;
		}
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		if (_isFloatHandle(_index))
		{
			((float*) _pTemp)[_index] = _fTemp / _pow;
		}
		else
		{
			_pTemp[_index] = _number;
		}
		if(_index == DataType)
		{
			ChangeValueWriteConfig(_pTemp[DataType], _buf, _flagMode, 0, 0, 3);
			PrintString(1, ValueWrite, infoPage, _buf, false);
			ST7920_SPI.display(0);
		}
		memset(numBuffer, 0, sizeof(numBuffer));
		return _step;
	case KEY_CLR:
		_number = 0;
		_fTemp = 0.0f;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
//		DisplayValueSensorConfig(infoPage, sensorConfig);
		_fTemp = ((float*) _pTemp)[_index];
		break;

	case KEY_ID:
		if (_isFloatHandle(_index))
		{
			_fTemp = GetOldDataFloat(numBuffer, ((float*) _pTemp)[_index], _pow);
			_fTemp = -_fTemp / _pow;
		}
		else
		{
			_number = -GetOldData(numBuffer, _pTemp[_index]);
		}
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		if (_isFloatHandle(_index))
		{
			_fTemp = ((float) _number) / _pow;
		}
		break;
	default:
		return _step;
	}
	uint8_t _col = 0;
	if(_index >= ROW_PAGE)
		_col = 1;
	switch (_index)
	{
	case Address:
		_number = inRangeConfig(247, 0, _number, _flagMode);
		PrintNumber(_col, yValue, infoPage, _number);
		break;
	case Function:
		_number = inRangeConfig(FUNCTION_16, FUNCTION_5, _number, _flagMode);
		PrintString(_col, yValue, infoPage, writeFunction[_number]);
		break;
	case RegisterID:
		_number = inRangeConfig(999, 0, _number, _flagMode);
		PrintNumber(_col, yValue, infoPage, _number);
		break;
	case DataType:
		_number = inRangeConfig(FLOAT2, UINT16, _number, _flagMode);
		PrintString(_col, yValue, infoPage, dataTypeWCFG[_number]);
		break;
	case ValueWrite:
		ChangeValueWriteConfig(_pTemp[DataType], _buf, _flagMode, _number, _fTemp, 3);
		PrintString(_col, yValue, infoPage, _buf);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t AdvancedConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue, IsFloatHandle _isFloatHandle)
{
	int32_t _number = 0;
	bool _flagMode = false;
	uint8_t _point = 2;
	uint16_t _pow = pow(10, _point);
	uint8_t _step = 2;
	float _fTemp = 0;
	int32_t * _pTemp = (int32_t*)advancedConfig;
	uint8_t _index = xValue + yValue * (infoPage.numCol);
	if(_isFloatHandle(xValue))
	{
		_fTemp = GetOldDataFloat(numBuffer, ((float*)_pTemp)[_index], _pow);
	}
	else
	{
		_number = GetOldData(numBuffer, _pTemp[_index]);
	}
	switch (keyValue)
	{
	case KEY_LEFT:
		if(_isFloatHandle(xValue))
		{
			_fTemp--;
			_fTemp = _fTemp/_pow;
		}
		else
		{
			_number--;
		}
		_flagMode = false;
		break;
	case KEY_RIGHT:
		if(_isFloatHandle(xValue))
		{
			_fTemp++;
			_fTemp = _fTemp/_pow;
		}
		else
		{
			_number++;
		}
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		if(_isFloatHandle(xValue))
		{
			((float*)_pTemp)[_index] = _fTemp/_pow;
		}
		else
		{
			_pTemp[_index] = _number;
		}
		if(xValue == Filter)
		{
			Kalman_SetProcessNoise(yValue, _fTemp);
		}
		 ExFlash_WriteData(advancedConfig, ADVANCEDCONFIG_ADDRESS, sizeof(advancedConfig));
		return _step;
	case KEY_CLR:
		_number = 0;
		_fTemp = 0.0f;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
//		DisplayValueSensorConfig(infoPage, sensorConfig);
		_fTemp = ((float*)_pTemp)[_index];
		break;

	case KEY_ID:
		if(_isFloatHandle(xValue))
		{
			_fTemp = GetOldDataFloat(numBuffer, ((float*)_pTemp)[_index], _pow);
			_fTemp = - _fTemp/_pow;
		}
		else
		{
			_number = - GetOldData(numBuffer, _pTemp[_index]);
		}
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		if(_isFloatHandle(xValue))
		{
			_fTemp = ((float)_number)/_pow;
		}
		break;
	default:
		return _step;
	}
	switch (xValue)
	{
//	case Value:
//		_fTemp = inRangeConfigFloat(99.99, 0, _fTemp, _point,_flagMode);
//		PrintFloat(xValue, yValue, infoPage, _fTemp, 2);
//		break;
//	case Scale:
//		_number = inRangeConfig(3, -3, _number, _flagMode);
//		PrintNumber(xValue, yValue, infoPage, _number);
//		break;
	case Filter:
		_fTemp = inRangeConfigFloat(99.99f, 0.00f, _fTemp, _point,_flagMode);
		PrintFloat(xValue, yValue, infoPage, _fTemp, 2);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t EnterPasswordCallback(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue,IsFloatHandle _isFloatHandle)
{
	int32_t _number = 0;
	bool _flagMode = false;
	uint8_t _step = 2;
	int32_t * _pTemp = (int32_t*)&tempPassword;
	uint8_t _index = yValue + xValue * (infoPage.numRow);
	_number = GetOldData(numBuffer, _pTemp[_index]);
	switch (keyValue)
	{
	case KEY_LEFT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number--;
		_flagMode = false;
		break;
	case KEY_RIGHT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number++;
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		_pTemp[_index] = _number;
		memset(numBuffer, 0, sizeof(numBuffer));
		return _step;
	case KEY_CLR:
		_number = 0;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		break;
	default:
		return _step;
	}
	switch (_index)
	{
	case EnterPassword:
		_number = inRangeConfig(9999, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number, true, true);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t ChangePasswordCallback(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue,IsFloatHandle _isFloatHandle)
{
	int32_t _number = 0;
	bool _flagMode = false;
	uint8_t _step = 2;
	int32_t * _pTemp = (int32_t*)&changePasswordConfig;
	uint8_t _index = yValue + xValue * (infoPage.numRow);
	_number = GetOldData(numBuffer, _pTemp[_index]);
	switch (keyValue)
	{
	case KEY_LEFT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number--;
		_flagMode = false;
		break;
	case KEY_RIGHT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number++;
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		_pTemp[_index] = _number;
		memset(numBuffer, 0, sizeof(numBuffer));
		return _step;
	case KEY_CLR:
		_number = 0;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		break;
	default:
		return _step;
	}
	switch (_index)
	{
	case CurrentPassword:
	case NewPassword:
		_number = inRangeConfig(9999, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number, true, true);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t pHCalibConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue,IsFloatHandle _isFloatHandle)
{
	int16_t _number = 0;
	bool _flagMode = false;
	uint8_t _step = 2;
	int16_t * _pTemp = (int16_t*)&pHCalibConfig;
	uint8_t _index = yValue;
	_number = GetOldData(numBuffer, _pTemp[_index]);
	switch (keyValue)
	{
	case KEY_LEFT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number--;
		_flagMode = false;
		break;
	case KEY_RIGHT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number++;
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		 _pTemp[_index] = _number;
		return _step;
	case KEY_CLR:
		_number = 0;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		break;
	default:
		return _step;
	}
	switch (_index)
	{
	case IDpH:
		_number = inRangeConfig(8, 1, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t TurCalibConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue,IsFloatHandle _isFloatHandle)
{
	int16_t _number = 0;
	bool _flagMode = false;
	uint8_t _step = 2;
	int16_t * _pTemp = (int16_t*)&turCalibConfig;
	uint8_t _index = yValue;
	_number = GetOldData(numBuffer, _pTemp[_index]);
	switch (keyValue)
	{
	case KEY_LEFT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number--;
		_flagMode = false;
		break;
	case KEY_RIGHT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number++;
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		 _pTemp[_index] = _number;
		return _step;
	case KEY_CLR:
		_number = 0;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		break;
	default:
		return _step;
	}
	switch (_index)
	{
	case IDTur:
		_number = inRangeConfig(8, 1, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case TypeTur:
		_number = inRangeConfig(3, 0, _number, _flagMode);
		PrintString(xValue, yValue, infoPage, typeTur[_number]);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t BCTurCalibConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue,IsFloatHandle _isFloatHandle)
{
	int32_t _number = 0;
	bool _flagMode = false;
	uint8_t _step = 2;
	int32_t * _pTemp = (int32_t*)&standardTurBC;
	uint8_t _index = yValue;
	_number = GetOldData(numBuffer, _pTemp[_index]);
	switch (keyValue)
	{
	case KEY_LEFT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number--;
		_flagMode = false;
		break;
	case KEY_RIGHT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number++;
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		 _pTemp[_index] = _number;
		 BCTurHandle(_index);
		return _step;
	case KEY_CLR:
		_number = 0;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		break;
	default:
		return _step;
	}
	switch (_index)
	{
	case DecPointTurBC:
		_number = inRangeConfig(1, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case SensCalibTurBC:
		_number = inRangeConfig(10000, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case SensCmdTurBC:
		_number = inRangeConfig(1, 0, _number, _flagMode);
		PrintString(xValue, yValue, infoPage, titleSensCmdTurBC[_number]);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t CloCalibConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue,IsFloatHandle _isFloatHandle)
{
	int16_t _number = 0;
	bool _flagMode = false;
	uint8_t _step = 2;
	int16_t * _pTemp = (int16_t*)&cloCalibConfig;
	uint8_t _index = yValue;
	_number = GetOldData(numBuffer, _pTemp[_index]);
	switch (keyValue)
	{
	case KEY_LEFT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number--;
		_flagMode = false;
		break;
	case KEY_RIGHT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number++;
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		 _pTemp[_index] = _number;
		return _step;
	case KEY_CLR:
		_number = 0;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		break;
	default:
		return _step;
	}
	switch (_index)
	{
	case IDClo:
		_number = inRangeConfig(8, 1, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case TypeClo:
		_number = inRangeConfig(0, 0, _number, _flagMode);
		PrintString(xValue, yValue, infoPage, typeTur[_number]);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int16_t BCCloCalibConfigCallBack(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue,IsFloatHandle _isFloatHandle)
{
	int32_t _number = 0;
	bool _flagMode = false;
	uint8_t _step = 2;
	int32_t * _pTemp = (int32_t*)&standardCloBC;
	uint8_t _index = xValue*infoPage.numRow + yValue;
	_number = GetOldData(numBuffer, _pTemp[_index]);
	switch (keyValue)
	{
	case KEY_LEFT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number--;
		_flagMode = false;
		break;
	case KEY_RIGHT:
		_number = GetOldData(numBuffer, _pTemp[_index]);
		_number++;
		_flagMode = false;
		break;
	case KEY_ENTER: // save config
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		 _pTemp[_index] = _number;
		 BCCloHandle(_index);
		return _step;
	case KEY_CLR:
		_number = 0;
		memset(numBuffer, 0, sizeof(numBuffer));
		_flagMode = true;
		break;
	case KEY_ESC:
		_step = 1;
		memset(numBuffer, 0, sizeof(numBuffer));
		_number = _pTemp[_index];
		break;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
		keyValue += 39;
		_flagMode = true;
		strncat(numBuffer, (char*) &keyValue, 1);
		_number = atoi(numBuffer);
		break;
	default:
		return _step;
	}
	switch (_index)
	{
	case ZeroDecPointClo:
		_number = inRangeConfig(3, 1, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case ZeroCalibClo:
		_number = inRangeConfig(2000, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case ZeroCmdClo:
		_number = inRangeConfig(1, 0, _number, _flagMode);
		PrintString(xValue, yValue, infoPage, titleCmdCloBC[_number]);
		break;
	case SensPointClo:
		_number = inRangeConfig(3, 1, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case SensCalibClo:
		_number = inRangeConfig(2000, 0, _number, _flagMode);
		PrintNumber(xValue, yValue, infoPage, _number);
		break;
	case SensCmdClo:
		_number = inRangeConfig(1, 0, _number, _flagMode);
		PrintString(xValue, yValue, infoPage, titleCmdCloBC[_number]);
		break;
	}
	ST7920_SPI.display(0);
	return _step;
}

static int8_t HandleTask(int8_t _pageNumber, InfoPage infoPage, DisplayHandle _displayHandle, IsFloatHandle _isFloatHandle)
{
	int8_t _step = 0;
	xCursor = 0;
	yCursor = 0;
	KeyPad _valueKey = KEY_NONE;
	memset(numBuffer, 0, sizeof(numBuffer));
	memset(&writeConfig, 0, sizeof(writeConfig));
	memset(&changePasswordConfig, 0, sizeof(changePasswordConfig));
	memset(&standardCloBC, 0, sizeof(standardCloBC));
	memset(&standardTurBC, 0, sizeof(standardTurBC));
	osMessageQueueReset(displayStatusCalibHandle);
	osMessageQueueReset(responseCommandCalibHandle);
	tempPassword = 0;
	for (;;)
	{
		HAL_IWDG_Refresh(&hiwdg);
		_valueKey = (KeyPad) getKey();
		if (_valueKey != KEY_NONE)
		{
			if (_step == 0)
			{
				switch(_valueKey)
				{
				case KEY_ENTER:
					_step = 1;
					InvertString(xCursor, yCursor, infoPage);
					ST7920_SPI.display(0);
					break;
				case KEY_LEFT:
					if(IsPageCalib(_pageNumber))
						break;
					_pageNumber--;
					_pageNumber = CalibRange(ADVANCED_CONFIG_PAGE, MODBUS_CONFIG_PAGE, _pageNumber);
					return _pageNumber;
				case KEY_RIGHT:
					if(IsPageCalib(_pageNumber))
						break;
					_pageNumber++;
					_pageNumber = CalibRange(ADVANCED_CONFIG_PAGE, MODBUS_CONFIG_PAGE, _pageNumber);
					return _pageNumber;
				case KEY_ESC:
					return ESC_Step0_Handle(_pageNumber);
				case KEY_CLR:
					if(_pageNumber==CHANGE_PASSWORD_PAGE && checkResetPasswordButton())
						ResetPasswordHandle();
					break;
				default:
					break;
				}
			}
			else if (_step == 1)
			{
				switch ((uint8_t) _valueKey)
				{
				case KEY_UP:
				case KEY_DOWN:
				case KEY_LEFT:
				case KEY_RIGHT:
					MoveCursor(infoPage, _valueKey, _pageNumber);
					break;
				case KEY_ESC:
					_step = 0;
					InvertString(xCursor, yCursor, infoPage);
					ST7920_SPI.display(0);
					xCursor = 0;
					yCursor = 0;
					break;
				case KEY_ENTER:
					_step = 2;
					if( _pageNumber== WRITE_CONFIG_PAGE && yCursor== WriteButton)
					{
						WriteConfigHandle();
						_step = 1;
					}
					else if(_pageNumber == ENTER_PASSWORD_PAGE && yCursor == ChangePasswordButton)
						return CHANGE_PASSWORD_PAGE;
					else if(_pageNumber == CHANGE_PASSWORD_PAGE && yCursor == OKChangePasswordButton)
					{
						ChangePasswordHandle();
						_step = 1;
					}
					else if((_pageNumber == ENTER_PASSWORD_PAGE && yCursor == OKPasswordButton))
					{
						if(EnterPasswordHandle())
							return MODBUS_CONFIG_PAGE;
						else
							_step = 1;
					}
					else if(_pageNumber == pH_CALIBRATION_PAGE && yCursor == StartButtonpH)
						return pH7_CALIBRATION_PAGE;
					else if(_pageNumber == TUR_CALIBRATION_PAGE && yCursor== StartButtonTur)
					{
						uint8_t _flagPut = true;
						osMessageQueuePut(flagGetValueCalibHandle, &_flagPut, 0U, 0U);
						switch(turCalibConfig.type)
						{
						case BCTur:
							return BCTUR_CALIBRATION_PAGE;
						}
						_step = 1;
					}
					else if(_pageNumber == CLO_CALIBRATION_PAGE && yCursor== StartButtonTur)
					{
						uint8_t _flagPut = true;
						osMessageQueuePut(flagGetValueCalibHandle, &_flagPut, 0U, 0U);
						switch(cloCalibConfig.type)
						{
						case BCClo:
							return BCCLO_CALIBRATION_PAGE;
						}
						_step = 1;
					}
					break;
				default:
					break;
				}
			}
			else if (_step == 2)
			{
				_step = _displayHandle(xCursor, yCursor, infoPage, (uint8_t) _valueKey, _isFloatHandle);
			}
		}
		UpdateValueCalibHandle(_pageNumber);
		osDelay(10);

	}
}


static void DisplayTable(uint8_t _number)
{
	switch(_number)
	{
	case 0:
	case 1:
		break;
	case 2:
		ST7920_SPI.drawLineVfast(SCR_WD/2 - 1, 0, SCR_HT - 1 , 1);
		break;
	case 3:
		ST7920_SPI.drawLineVfast(SCR_WD/2 - 1, 0, SCR_HT - 1 , 1);
		ST7920_SPI.drawLineHfast(0, SCR_WD/2 - 1, SCR_HT/2 - 1, 1);
		break;
	case 4:
		ST7920_SPI.drawLineHfast(0, SCR_WD - 1, SCR_HT/2 - 1, 1);
		ST7920_SPI.drawLineVfast(SCR_WD/2 - 1, 0, SCR_HT - 1 , 1);
		break;
	default:
		break;
	}
}

static void DisplayBase(uint8_t _number, uint8_t _page, uint8_t _offset)
{
	DisplayTable(_number);
	uint8_t xposName, yposName, xposUnit, yposUnit;
	for(int i = 0 ; i < _number; i++)
	{
		switch(_number)
		{
		case 1:
			ST7920_SPI.setFont(valueFont2);
			xposName = xName_1[i];
			yposName = yName_1[i];
			xposUnit = xUnit_1[i];
			yposUnit = yUnit_1[i];
			break;
		case 2:
			ST7920_SPI.setFont(titleFont);
			xposName = xName_2[i];
			yposName = yName_2[i];
			xposUnit = xUnit_2[i];
			yposUnit = yUnit_2[i];
			break;
		case 3:
			ST7920_SPI.setFont(titleFont);
			xposName = xName_3[i];
			yposName = yName_3[i];
			xposUnit = xUnit_3[i];
			yposUnit = yUnit_3[i];
			break;
		case 4:
			ST7920_SPI.setFont(titleFont);
			xposName = xName_4[i];
			yposName = yName_4[i];
			xposUnit = xUnit_4[i];
			yposUnit = yUnit_4[i];
			break;
		default:
			break;
		}
		ST7920_SPI.deleteString(xposName, xposName, 4, 0);
		ST7920_SPI.printStr(xposName, yposName, (char*)nameSensor[sensorConfig[i + _offset].name], true);
		ST7920_SPI.deleteString(xposUnit, yposUnit, 4, 0);
		ST7920_SPI.printStrInvert(xposUnit, yposUnit, (char*)uintSensor[sensorConfig[i + _offset].name], true);
	}

}

static void LCD_DisplayArletModbus(uint8_t _number, uint8_t _page, uint8_t _offset)
{
	static bool _flag = true;
	uint32_t _curTime = HAL_GetTick() - preTimeAlert;
	bool _flagChange = false;
	if(_curTime > TIME_ALERT)
	{
//		ST7920_SPI.cls();
		DisplayBase(_number, _page, _offset);
		for(int i = 0 ; i < _number; i++)
		{
			bool _flagFail = !processData[i + _offset].failFlag;
			uint8_t _flagAlarm = processData[i + _offset].alarmStatus;
			_flagChange |= _flagFail;
			if(_flag)
			{
				if(_flagFail)
				{
					switch(_number)
					{
					case 1:
						ST7920_SPI.setFont(valueFont1);
						ST7920_SPI.deleteString(xValue_1[i], yValue_1[i], 5, OFFSET_INVERT);
						ST7920_SPI.fillRect(xAlert_1[i], yAlert_1[i], SIZE_ALERT, SIZE_ALERT, 0);
						ST7920_SPI.fillRect(xAlarm_1[i], yAlarm_1[i], W_ALARM, H_ALARM, 0);
						ST7920_SPI.drawBitmap(DisconnectIcon, xAlert_1[i], yAlert_1[i]);
						break;
					case 2:
						ST7920_SPI.setFont(valueFont2);
						ST7920_SPI.deleteString(xValue_2[i], yValue_2[i], 5, OFFSET_INVERT);
						ST7920_SPI.fillRect(xAlert_2[i], yAlert_2[i], SIZE_ALERT, SIZE_ALERT, 0);
						ST7920_SPI.fillRect(xAlarm_2[i], yAlarm_2[i], W_ALARM, H_ALARM, 0);
						ST7920_SPI.drawBitmap(DisconnectIcon, xAlert_2[i], yAlert_2[i]);
						break;
					case 3:
						ST7920_SPI.setFont(titleFont);
						if(i == 2)
							ST7920_SPI.setFont(valueFont2);
						ST7920_SPI.deleteString(xValue_3[i], yValue_3[i], 5, OFFSET_INVERT);
						ST7920_SPI.fillRect(xAlert_3[i], yAlert_3[i], SIZE_ALERT, SIZE_ALERT, 0);
						ST7920_SPI.fillRect(xAlarm_3[i], yAlarm_3[i], W_ALARM, H_ALARM, 0);
						ST7920_SPI.drawBitmap(DisconnectIcon, xAlert_3[i], yAlert_3[i]);
						break;
					case 4:
						ST7920_SPI.setFont(titleFont);
						ST7920_SPI.deleteString(xValue_4[i], yValue_4[i], 5, OFFSET_INVERT);
						ST7920_SPI.fillRect(xAlert_4[i], yAlert_4[i], SIZE_ALERT, SIZE_ALERT, 0);
						ST7920_SPI.fillRect(xAlarm_4[i], yAlarm_4[i], W_ALARM, H_ALARM, 0);
						ST7920_SPI.drawBitmap(DisconnectIcon, xAlert_4[i], yAlert_4[i]);
						break;
					}
				}
				else
				{
					if(_flagAlarm == LOW || _flagAlarm == HIGH)
					{
						uint8_t * _alarmSymbol;
						if(_flagAlarm == LOW)
							_alarmSymbol = (uint8_t*)Down_Button;
						else if(_flagAlarm == HIGH)
							_alarmSymbol = (uint8_t*)Up_Button;
						switch(_number)
						{
						case 1:
							ST7920_SPI.fillRect(xAlarm_1[i], yAlarm_1[i], W_ALARM, H_ALARM, 0);
							ST7920_SPI.drawBitmap(_alarmSymbol, xAlarm_1[i], yAlarm_1[i]);
							break;
						case 2:
							ST7920_SPI.fillRect(xAlarm_2[i], yAlarm_2[i], W_ALARM, H_ALARM, 0);
							ST7920_SPI.drawBitmap(_alarmSymbol, xAlarm_2[i], yAlarm_2[i]);
							break;
						case 3:
							ST7920_SPI.fillRect(xAlarm_3[i], yAlarm_3[i], W_ALARM, H_ALARM, 0);
							ST7920_SPI.drawBitmap(_alarmSymbol, xAlarm_3[i], yAlarm_3[i]);
							break;
						case 4:
							ST7920_SPI.fillRect(xAlarm_4[i], yAlarm_4[i], W_ALARM, H_ALARM, 0);
							ST7920_SPI.drawBitmap(_alarmSymbol, xAlarm_4[i], yAlarm_4[i]);
							break;
						}
					}
				}
			}
			else
			{
				if(_flagFail)
				{
					switch(_number)
					{
					case 1:
						ST7920_SPI.setFont(valueFont1);
						ST7920_SPI.deleteString(xValue_1[i], yValue_1[i], 5, OFFSET_INVERT);
						ST7920_SPI.fillRect(xAlert_1[i], yAlert_1[i], SIZE_ALERT, SIZE_ALERT, 0);
						break;
					case 2:
						ST7920_SPI.setFont(valueFont2);
						ST7920_SPI.deleteString(xValue_2[i], yValue_2[i], 5, OFFSET_INVERT);
						ST7920_SPI.fillRect(xAlert_2[i], yAlert_2[i], SIZE_ALERT, SIZE_ALERT, 0);
						break;
					case 3:
						ST7920_SPI.setFont(titleFont);
						if(i == 2)
							ST7920_SPI.setFont(valueFont2);
						ST7920_SPI.deleteString(xValue_3[i], yValue_3[i], 5, OFFSET_INVERT);
						ST7920_SPI.fillRect(xAlert_3[i], yAlert_3[i], SIZE_ALERT, SIZE_ALERT, 0);
						break;
					case 4:
						ST7920_SPI.setFont(titleFont);
						ST7920_SPI.deleteString(xValue_4[i], yValue_4[i], 5, OFFSET_INVERT);
						ST7920_SPI.fillRect(xAlert_4[i], yAlert_4[i], SIZE_ALERT, SIZE_ALERT, 0);
						break;
					}
				}
				else
				{
					switch(_number)
					{
					case 1:
						ST7920_SPI.fillRect(xAlarm_1[i], yAlarm_1[i], W_ALARM, H_ALARM, 0);
						break;
					case 2:
						ST7920_SPI.fillRect(xAlarm_2[i], yAlarm_2[i], W_ALARM, H_ALARM, 0);
						break;
					case 3:
						ST7920_SPI.fillRect(xAlarm_3[i], yAlarm_3[i], W_ALARM, H_ALARM, 0);
						break;
					case 4:
						ST7920_SPI.fillRect(xAlarm_4[i], yAlarm_4[i], W_ALARM, H_ALARM, 0);
						break;
					}
				}
			}

		}
//		if(_flagChange)
//			ST7920_SPI.display(0);
		ST7920_SPI.display(0);
		preTimeAlert = HAL_GetTick();
		_flag = !_flag;
	}


}

static void DisplayValue(uint8_t _number, uint8_t _page, uint8_t _offset)
{
	uint8_t _flagModbusQueue;
	uint8_t _xpos, _ypos;
	bool flagDisplay = false;
	if (osMessageQueueGet(modbusLCDQueueHandle, &_flagModbusQueue, NULL, 1) == osOK)
	{
		uint8_t _index = _flagModbusQueue;
		if(_page == SUBPAGE_1 && _index >= ROW_PAGE)
			return;
		else if(_page == SUBPAGE_2 && _index < ROW_PAGE)
			return;
//		ST7920_SPI.cls();
		DisplayBase(_number, _page, _offset);
		for(int i = 0; i < _number; i++)
		{
			bool _flagFail = processData[i + _offset].failFlag;
			flagDisplay |= _flagFail;
			if (_flagFail)
			{
				switch(_number)
				{
				case 1:
					ST7920_SPI.setFont(valueFont1);
					_xpos = xValue_1[i];
					_ypos = yValue_1[i];
					ST7920_SPI.fillRect(xAlert_1[i], yAlert_1[i], SIZE_ALERT, SIZE_ALERT, 0);
					break;
				case 2:
					ST7920_SPI.setFont(valueFont2);
					_xpos = xValue_2[i];
					_ypos = yValue_2[i];
					ST7920_SPI.fillRect(xAlert_2[i], yAlert_2[i], SIZE_ALERT, SIZE_ALERT, 0);
					break;
				case 3:
					ST7920_SPI.setFont(titleFont);
					if(i == 2)
						ST7920_SPI.setFont(valueFont2);
					_xpos = xValue_3[i];
					_ypos = yValue_3[i];
					ST7920_SPI.fillRect(xAlert_3[i], yAlert_3[i], SIZE_ALERT, SIZE_ALERT, 0);
					break;
				case 4:
					ST7920_SPI.setFont(titleFont);
					_xpos = xValue_4[i];
					_ypos = yValue_4[i];
					ST7920_SPI.fillRect(xAlert_4[i], yAlert_4[i], SIZE_ALERT, SIZE_ALERT, 0);
					break;
				default:
					break;
				}
				ST7920_SPI.deleteString(_xpos, _ypos, 5, OFFSET_INVERT);
				PrintFloatExcept(_xpos, _ypos, processData[i + _offset].value, sensorConfig[i + _offset].numPoint);
			}
		}
		if(flagDisplay)
			ST7920_SPI.display(0);
	}
}

void DisplayEnterPasswordBackground(void)
{
	ST7920_SPI.drawBitmap(Lock_Password, X_LOCK, Y_LOCK);
	ST7920_SPI.drawRect(X_LOCK + 30 , Y_LOCK + 6 , 100, 22, 1);
	ST7920_SPI.setFont(defaulFont);
	ST7920_SPI.printStr(X_LOCK + 46, Y_LOCK + 3,(char*) "Enter Password", true);
	ST7920_SPI.setFont(titleFont);
	ST7920_SPI.printStrInvert(X_LOCK + 95, Y_LOCK + 15,(char*) "0 0 0 0", false);
	ST7920_SPI.setFont(defaulFont);

	ST7920_SPI.drawRect(X_LOCK + 60 ,Y_LOCK + 33 , 34, 11, 1);
	ST7920_SPI.printStr(X_LOCK + 73 , Y_LOCK + 35, (char*)"OK", 0);

	ST7920_SPI.drawRect(X_LOCK + 97 ,Y_LOCK + 33 , 34, 11, 1);
	ST7920_SPI.printStr(X_LOCK + 99 , Y_LOCK + 35, (char*)"CHANGE", 0);
	ST7920_SPI.setFont(titleFont);
}

void DisplayChangePassword(void)
{
	ST7920_SPI.setFont(defaulFont);
	ST7920_SPI.drawRect(X_CHANGE - 21 , Y_CHANGE + 3 , 118, 36, 1);
	ST7920_SPI.printStr(X_CHANGE, Y_CHANGE,(char*) "Change Password", true);
	ST7920_SPI.printStr(X_CHANGE - 8, Y_CHANGE + 12,(char*) "Current", false);
	ST7920_SPI.printStr(X_CHANGE - 8, Y_CHANGE + 24,(char*) "New", false);
	ST7920_SPI.drawRect(X_CHANGE + 63 ,Y_CHANGE + 42 , 34, 11, 1);
	ST7920_SPI.printStr(X_CHANGE + 76 , Y_CHANGE + 44, (char*)"OK", 0);
	ST7920_SPI.setFont(titleFont);
	ST7920_SPI.printStrInvert(X_CHANGE + 79, Y_CHANGE + 12, (char*) "0 0 0 0", false);
	ST7920_SPI.printStrInvert(X_CHANGE + 79, Y_CHANGE + 24, (char*) "0 0 0 0", false);
//	ST7920_SPI.setFont(defaulFont);
}

#ifdef CAL_BUTTON
static void CalculateButton(const char ** _pTitle, InfoPage * _infoPage, uint8_t _offset)
{
	ST7920_SPI.setFont(defaulFont);
	for(int i = 0; i < _infoPage->numButton; i++)
	{
		_infoPage->wButton[i] = _offset*2 + ST7920_SPI.strWidth((char*)_pTitle[i]) + 1;
		_infoPage->hButton[i] = ST7920_SPI.cfont.ySize + _offset*2 + 1;
		ST7920_SPI.drawRect( _infoPage->xButton[i], _infoPage->yButton[i], _infoPage->wButton[i], _infoPage->hButton[i], 1);
		ST7920_SPI.printStr(_infoPage->xButton[i] + _offset + 1, _infoPage->yButton[i] + _offset + 1, (char*)_pTitle[i], false);
	}
}

#endif


static void DisplayValuePHCalib(InfoPage _infoPage)
{
	PrintNumber(IDpH, IDpH, _infoPage, pHCalibConfig.id, false);
}

static void DisplayValueTurCalib(InfoPage _infoPage)
{
	ST7920_SPI.setFont(defaulFont);
	for(int i = 0; i < _infoPage.numRow - _infoPage.numButton; i++)
	{
		switch(i)
		{
		case IDTur:
			PrintNumber(0, i, _infoPage, turCalibConfig.id, false);
			break;
		case TypeTur:
			PrintString(0, i, _infoPage, typeTur[turCalibConfig.type], false);
			break;
		}
	}
}

static void DisplayValueTurBCCalib(InfoPage _infoPage)
{
	for(int i = 0; i < _infoPage.numRow; i++)
	{
		switch(i)
		{
		case DecPointTurBC:
			PrintNumber(0, i, _infoPage, standardTurBC.point, false);
			break;
		case SensCalibTurBC:
			PrintNumber(0, i, _infoPage, standardTurBC.sensCalib, false);
			break;
		case SensCmdTurBC:
			PrintString(0, i, _infoPage, (char*)titleSensCmdTurBC[standardTurBC.sensCmd], false);
			break;
		}
	}
	PrintString(0, 3, _infoPage, (char*)"____", false);
	uint8_t _index = turCalibConfig.id - 1;
	if(processData[_index].failFlag)
		standardTurBCUpdate.turValue = processData[_index].value;
	PrintFloat(0, 4, _infoPage, standardTurBCUpdate.turValue, sensorConfig[_index].numPoint, false);
}

static void DisplayValueCloBCCalib(InfoPage _infoPage)
{
	for(int j = 0; j < _infoPage.numCol; j++)
	{
		for(int i = 0; i < _infoPage.numRow; i++)
		{
			uint8_t _index = i + j*_infoPage.numRow;
			switch(_index)
			{
			case ZeroDecPointClo:
				PrintNumber(j, i, _infoPage, standardCloBC.zeroPoint, false);
				break;
			case ZeroCalibClo:
				PrintNumber(j, i, _infoPage, standardCloBC.zeroCalib, false);
				break;
			case ZeroCmdClo:
				PrintString(j, i, _infoPage, (char*)titleCmdCloBC[standardCloBC.zeroCmd], false);
				break;
			case SensPointClo:
				PrintNumber(j, i, _infoPage, standardCloBC.sensPoint, false);
				break;
			case SensCalibClo:
				PrintNumber(j, i, _infoPage, standardCloBC.sensCalib, false);
				break;
			case SensCmdClo:
				PrintString(j, i, _infoPage, (char*)titleCmdCloBC[standardCloBC.sensCmd], false);
				break;
			}
		}
	}
	PrintString(0, 3, _infoPage, (char*)"____", false);
	PrintString(1, 3, _infoPage, (char*)"____", false);
	uint8_t _index = cloCalibConfig.id - 1;
	if(processData[_index].failFlag)
		standardCloBCUpdate.cloValue = processData[_index].value;
	ST7920_SPI.deleteString(xValueCloBC, _infoPage.yPos[4], 6, OFFSET_INVERT);
	PrintFloatExcept(xValueCloBC, _infoPage.yPos[4], standardCloBCUpdate.cloValue, sensorConfig[_index].numPoint);
}

static void DisplayBackgroundCalib(InfoPage _infoPage, const char ** _title, const char ** _titleButton)
{
	ST7920_SPI.setFont(defaulFont);
	uint8_t _offset = 2;
	for(int i = 0; i < _infoPage.numRow - _infoPage.numButton; i++)
	{
		ST7920_SPI.printStr(_infoPage.xPosValue[0] - 50, _infoPage.yPos[i],(char*) _title[i], false);
	}
	for(int i = 0; i < _infoPage.numButton; i++)
	{
		ST7920_SPI.drawRect( _infoPage.xButton[i], _infoPage.yButton[i], _infoPage.wButton[i], _infoPage.hButton[i], 1);
		ST7920_SPI.printStr(_infoPage.xButton[i] + 2 + _offset, _infoPage.yButton[i] + 4 , (char*)_titleButton[i], false);
	}
}

static void DisplayBackgroundSubpH(InfoPage _infoPage)
{
	const char *title[4] = {"Standard", "Current Value", "Status", "Progress"};
	ST7920_SPI.setFont(defaulFont);
	uint8_t _xTitle = _infoPage.xPosTitle[0];
	for(int i = 0; i < _infoPage.numRow; i++)
	{
		ST7920_SPI.printStr(_xTitle, _infoPage.yPos[i], (char*)title[i], false);
	}
	ST7920_SPI.drawProgressBar(xPHProgess, yPHProgess, wPHProgess, hPHProgess, 0);
}

static void DisplayBackgroundTur(InfoPage _infoPage)
{
	for(int i = 0 ; i < NumTitleBCTur; i++)
	{
		ST7920_SPI.printStr(_infoPage.xPosTitle[0], _infoPage.yPos[i], (char*)titleBCTur[i], false);
	}
}

static void DisplayBackgroundCloBC(InfoPage _infoPage)
{
	for(int i = 0; i < _infoPage.numCol; i++)
		for(int j = 0; j < _infoPage.numRow; j++)
			ST7920_SPI.printStr(_infoPage.xPosTitle[i], _infoPage.yPos[j] ,(char*) titleBCClo[i*_infoPage.numRow + j]);
	ST7920_SPI.drawLineVfast(SCR_WD/2, _infoPage.yPos[0] - 1,  _infoPage.yPos[3] + 9, 1);
	//custom
	ST7920_SPI.printStr(_infoPage.xPosTitle[0], _infoPage.yPos[3] ,(char*) titleBCClo[6]);
	ST7920_SPI.printStr(_infoPage.xPosTitle[1], _infoPage.yPos[3] ,(char*) titleBCClo[7]);
	ST7920_SPI.printStr(48, _infoPage.yPos[4] ,(char*) titleBCClo[8]);
}

static void UpdatePHCalibValue(void)
{
	for(int i = 0; i < 4; i++)
	{
		switch(i)
		{
		case Standard_PH:
			PrintFloat(0, i, subpPHCalibConfigPage, standardPH.standard, 2, false);
			break;
		case CurrentValue_PH:
			PrintFloat(0, i, subpPHCalibConfigPage, standardPH.currentValue, 2, false);
			break;
		case Status_PH:
			ST7920_SPI.deleteString(subpPHCalibConfigPage.xPosValue[0], subpPHCalibConfigPage.yPos[i], 9, 0);
			ST7920_SPI.printStrInvert(subpPHCalibConfigPage.xPosValue[0], subpPHCalibConfigPage.yPos[i], (char*)statusPHCalib[standardPH.status], false);
			break;
		case Progress_PH:
			ST7920_SPI.drawProgressBar(xPHProgess, yPHProgess, wPHProgess, hPHProgess, standardPH.progess);
			break;
		}
	}
	ST7920_SPI.display(0);
}

static bool SendPHCalibCommand(uint8_t _page)
{
	ModbusCalib _modbusCalib = {0};
	uint8_t _index = pHCalibConfig.id;
	_modbusCalib.type = NO_RESPONSE;
	_modbusCalib.command.addr = modbusRTU[_index].addr;
	_modbusCalib.command.func = 0x06;
	_modbusCalib.command.regID = 0x0D;
	switch(_page)
	{
	case pH4_CALIBRATION_PAGE:
		standardPH.standard = 4.00f;
		_modbusCalib.command.length = 0x02;
		DisplayMainTitle(TITLE_PH4);
		break;
	case pH10_CALIBRATION_PAGE:
		standardPH.standard = 10.00f;
		_modbusCalib.command.length = 0x02;
		DisplayMainTitle(TITLE_PH10);
		break;
		break;
	case pH7_CALIBRATION_PAGE:
		 standardPH.standard = 7.00f;
		 _modbusCalib.command.length = 0x01;
		DisplayMainTitle(TITLE_PH7);
		break;
	}
	UpdatePHCalibValue();
	uint8_t _flagPut = true;
	osMessageQueuePut(flagGetValueCalibHandle, &_flagPut, 0U, 0U);
	osMessageQueuePut(modbusCalibQueueHandle, &_modbusCalib, 0U, 0U);
	bool _flag = false;
	while(osMessageQueueGet(responseCommandCalibHandle, &_flag, NULL, 1) != osOK)
	{
		getKey();
		osDelay(10);
	}
	return _flag;
}

static void DisplayAlertStatusPHCalib(void)
{
	static uint32_t _timeComplete = HAL_GetTick();
	static bool _flagToggle = true;
	if(HAL_GetTick() - _timeComplete > TIME_ALERT)
	{
		if(!_flagToggle)
		{
			ST7920_SPI.deleteString(subpPHCalibConfigPage.xPosValue[0], subpPHCalibConfigPage.yPos[Status_PH], 9, 0);
			ST7920_SPI.printStrInvert(subpPHCalibConfigPage.xPosValue[0], subpPHCalibConfigPage.yPos[Status_PH], (char*)statusPHCalib[standardPH.status], false);
		}
		else
		{
			ST7920_SPI.deleteString(subpPHCalibConfigPage.xPosValue[0], subpPHCalibConfigPage.yPos[Status_PH], 9, 0);
		}
		ST7920_SPI.display(0);
		_flagToggle = !_flagToggle;
		_timeComplete = HAL_GetTick();
	}

}

static int8_t DisplaySubPHCalib(uint8_t _page)
{
	osMessageQueueReset(displayStatusCalibHandle);
	osMessageQueueReset(responseCommandCalibHandle);
	uint8_t _index = pHCalibConfig.id - 1;
	memset(&standardPH, 0, sizeof(standardPH));
	uint32_t _preTime = HAL_GetTick();
	bool _flagCommand = SendPHCalibCommand(_page);
	if(!_flagCommand)
	{
		standardPH.status = ERROR_PH;
		UpdatePHCalibValue();
	}
	for(;;)
	{
		osDelay(10);
		uint8_t _keyValue = getKey();
		if(_keyValue == KEY_ENTER)
		{
			switch(_page)
			{
			case pH7_CALIBRATION_PAGE:
				return pH10_CALIBRATION_PAGE;
			case pH10_CALIBRATION_PAGE:
				return pH4_CALIBRATION_PAGE;
			case pH4_CALIBRATION_PAGE:
				return pH_CALIBRATION_PAGE;
			}
		}
		else if(_keyValue == KEY_ESC)
			return pH_CALIBRATION_PAGE;
		if(!_flagCommand)
		{
			DisplayAlertStatusPHCalib();
			continue;
		}
		uint8_t _flagQueue = 0;
		if(osMessageQueueGet(displayStatusCalibHandle, &_flagQueue, 0U, 0U) == osOK)
		{
			if(_flagQueue == _index)
			{
				uint32_t _time = HAL_GetTick() - _preTime;
				if(_time > TIME_CALIB_PH)
				{
					standardPH.progess = 100;
					standardPH.status = ERROR_PH;
					_flagCommand = false;
					UpdatePHCalibValue();
					continue;
				}
				uint8_t _percent = (uint8_t)((float)_time * 100.00f/90000.00f);
				const ProcessData * _processData = processData;
				if(!_processData[_index].failFlag)
				{
					standardPH.progess = _percent;
					standardPH.status = ERROR_PH;
					_flagCommand = false;
				}
				else
				{
					standardPH.progess = _percent;
					standardPH.status = Running_PH;
					standardPH.currentValue = _processData[_index].value;
					float _abs = fabsf(standardPH.currentValue - standardPH.standard);
					if(_abs < 0.05f)
					{
						standardPH.status = Success_PH;
						_flagCommand = false;
					}
				}
				UpdatePHCalibValue();
			}
		}
	}
}

static int8_t LCD_DisplayValuePage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	uint8_t _numConfig = deviceConfig.numConfig;
	int8_t _page = SUBPAGE_1;
//	DisplayBase(_numConfig, _page);
//	ST7920_SPI.display(0);
	uint32_t _preTime = HAL_GetTick();
	bool _flagCheck = false;
	uint8_t _number = 0;
	uint8_t _offset = 0;
	for(;;)
	{
		osDelay(10);
		HAL_IWDG_Refresh(&hiwdg);
		uint8_t _keyValue = getKey();
		if(_numConfig > ROW_PAGE)
			_flagCheck = true;
		else
			_flagCheck = false;
		if((HAL_GetTick() - _preTime > (TIME_UNIT*deviceConfig.displayTime)) && _flagCheck)
		{
			if(_page == SUBPAGE_1)
				_page = SUBPAGE_2;
			else if(_page == SUBPAGE_2)
				_page = SUBPAGE_1;
			_preTime = HAL_GetTick();
			ST7920_SPI.cls();
		}
		if(_keyValue == KEY_SET)
			return ENTER_PASSWORD_PAGE;
		else if(_keyValue == KEY_ALARM)
			return HEALTHY_PAGE;
		else if(_keyValue == KEY_UP && _flagCheck)
		{
			_page--;
			_page = CalibRange(SUBPAGE_2, SUBPAGE_1, _page);
			_preTime = HAL_GetTick();
			ST7920_SPI.cls();
		}
		else if(_keyValue == KEY_DOWN && _flagCheck)
		{
			_page++;
			_page = CalibRange(SUBPAGE_2, SUBPAGE_1, _page);
			_preTime = HAL_GetTick();
			ST7920_SPI.cls();
		}
		switch(_numConfig)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			DisplayValue(_numConfig, _page, 0);
			LCD_DisplayArletModbus(_numConfig, _page, 0);
			break;
		case 5:
			if(_page == SUBPAGE_1)
			{
				_number = 3;
				_offset = 0;
			}
			else if(_page == SUBPAGE_2)
			{
				_number = 2;
				_offset = 3;
			}
			DisplayValue(_number, _page, _offset);
			LCD_DisplayArletModbus(_number, _page, _offset);
			break;
		case 6:
			if(_page == SUBPAGE_1)
			{
				_number = 3;
				_offset = 0;
			}
			else if(_page == SUBPAGE_2)
			{
				_number = 3;
				_offset = 3;
			}
			DisplayValue(_number, _page, _offset);
			LCD_DisplayArletModbus(_number, _page, _offset);
			break;
		case 7:
			if(_page == SUBPAGE_1)
			{
				_number = 4;
				_offset = 0;
			}
			else if(_page == SUBPAGE_2)
			{
				_number = 3;
				_offset = 4;
			}
			DisplayValue(_number, _page, _offset);
			LCD_DisplayArletModbus(_number, _page, _offset);
			break;
		case 8:
			if(_page == SUBPAGE_1)
			{
				_number = 4;
				_offset = 0;
			}
			else if(_page == SUBPAGE_2)
			{
				_number = 4;
				_offset = 4;
			}
			DisplayValue(_number, _page, _offset);
			LCD_DisplayArletModbus(_number, _page, _offset);
			break;
		}
	}
}


static int8_t LCD_ModbusConfigPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_MBCF);
	DisplaySubTitle(xTitleMBCF, modbusConfigPage);
	DisplayValueConfig(modbusConfigPage, _pageNumber);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, modbusConfigPage, ModbusConfigCallBack, nullptr);
}

static int8_t LCD_SensorConfigPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_SSCF);
//	CalculateSubTitle(xTitleSSCF, &sensorConfigPage);
	DisplaySubTitle(xTitleSSCF, sensorConfigPage);
	DisplayValueConfig(sensorConfigPage, _pageNumber);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, sensorConfigPage, SensorConfigCallBack, SensorConfigIsFloat);
}

static int8_t LCD_CalibConfigPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_CLCF);
	DisplaySubTitle(xTitleCLCF, calibConfigPage);
	DisplayValueConfig(calibConfigPage, _pageNumber);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, calibConfigPage, CalibConfigCallBack, CalibConfigIsFloat);
}

static int8_t LCD_AlarmConfigPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_ALCF);
	DisplaySubTitle(xTitleALCF, alarmConfigPage);
	DisplayValueConfig(alarmConfigPage, _pageNumber);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, alarmConfigPage, AlarmConfigCallBack, AlarmConfigIsFloat);
}

static int8_t LCD_DeviceConfigPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_DVCF);
	DisplaySubTitleDevice(xTitleDVCF, DeviceConfigPage);
	DisplayValueDevice(DeviceConfigPage);
//	CalculateSubTitle(nullptr, &DeviceConfigPage);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, DeviceConfigPage, DeviceConfigCallBack, nullptr);
}

static int8_t LCD_WriteConfigPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_WRCF);
	DisplaySubTitleAndValueWriteConfig(xTitleWRCF, WriteConfigPage);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, WriteConfigPage, WriteConfigCallBack, WriteConfigIsFloat);
}

static int8_t LCD_AdvancedConfigPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_AVCF);
	DisplaySubTitle(xTitleAVCF, advancedConfigPage);
	DisplayValueConfig(advancedConfigPage, _pageNumber);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, advancedConfigPage, AdvancedConfigCallBack, AdvancedConfigIsFloat);
}

static int8_t LCD_HealthyPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_HEATHY);
	DisplaySubTitleDevice(xTitleHealthy, healthyPage);
	static uint32_t _preTime = 0;
	for(;;)
	{
		uint8_t _keyValue = getKey();
		if(_keyValue == KEY_ESC)
			return MODBUS_VALUE_PAGE;
		if(HAL_GetTick() - _preTime > TIME_HEALTHY)
		{
			DisplayHealthy(healthyPage);
			_preTime = HAL_GetTick();
			ST7920_SPI.display(0);
		}
		osDelay(10);
	}
}

static int8_t LCD_EnterPasswordPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayEnterPasswordBackground();
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, PasswordConfigPage, EnterPasswordCallback, nullptr);
}

static int8_t LCD_ChangePasswordPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayChangePassword();
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, ChangePasswordPage, ChangePasswordCallback, nullptr);
}

static int8_t LCD_pHCalibPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_PH);
	DisplayBackgroundCalib( pHCalibConfigPage, titlePH, titlePHButton);
	DisplayValuePHCalib(pHCalibConfigPage);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, pHCalibConfigPage, pHCalibConfigCallBack, nullptr);
}

static int8_t LCD_subPHCalibPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayBackgroundSubpH(subpPHCalibConfigPage);
	return DisplaySubPHCalib(_pageNumber);
}

static int8_t LCD_TurCalibPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_TUR);
	DisplayBackgroundCalib(turCalibConfigPage, titleTur, titleTurButton);
	DisplayValueTurCalib(turCalibConfigPage);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, turCalibConfigPage, TurCalibConfigCallBack, nullptr);
}

static int8_t LCD_BCTurCalibPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_TUR);
	DisplayBackgroundTur(bcTurCalibConfigPage);
	DisplayValueTurBCCalib(bcTurCalibConfigPage);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, bcTurCalibConfigPage, BCTurCalibConfigCallBack, nullptr);
}

static int8_t LCD_CloCalibPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_CLO);
	DisplayBackgroundCalib(cloCalibConfigPage, titleClo, titleCloButton);
	DisplayValueTurCalib(cloCalibConfigPage);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, cloCalibConfigPage, CloCalibConfigCallBack, nullptr);
}

static int8_t LCD_BCCloCalibPage(int8_t _pageNumber)
{
	ST7920_SPI.cls();
	DisplayMainTitle(TITLE_CLO);
	DisplayBackgroundCloBC(bcCloCalibConfigPage);
	DisplayValueCloBCCalib(bcCloCalibConfigPage);
	ST7920_SPI.display(0);
	return HandleTask(_pageNumber, bcCloCalibConfigPage, BCCloCalibConfigCallBack, nullptr);
}

void LCD_SwitchContextPage(void)
{
	static int8_t _pageNumber = MODBUS_CONFIG_PAGE;
	switch(_pageNumber)
	{
	case MODBUS_VALUE_PAGE:
		_pageNumber = LCD_DisplayValuePage(_pageNumber);
		break;
	case MODBUS_CONFIG_PAGE:
		_pageNumber = LCD_ModbusConfigPage(_pageNumber);
		break;
	case CALIB_CONFIG_PAGE:
		_pageNumber = LCD_CalibConfigPage(_pageNumber);
		break;
	case SENSOSR_CONFIG_PAGE:
		_pageNumber = LCD_SensorConfigPage(_pageNumber);
		break;
	case ALARM_CONFIG_PAGE:
		_pageNumber = LCD_AlarmConfigPage(_pageNumber);
		break;
	case DEVICE_CONFIG_PAGE:
		_pageNumber = LCD_DeviceConfigPage(_pageNumber);
		break;
	case WRITE_CONFIG_PAGE:
		_pageNumber = LCD_WriteConfigPage(_pageNumber);
		break;
	case ADVANCED_CONFIG_PAGE:
		_pageNumber = LCD_AdvancedConfigPage(_pageNumber);
		break;
	case HEALTHY_PAGE:
		_pageNumber = LCD_HealthyPage(_pageNumber);
		break;
	case ENTER_PASSWORD_PAGE:
		_pageNumber = LCD_EnterPasswordPage(_pageNumber);
		break;
	case CHANGE_PASSWORD_PAGE:
		_pageNumber = LCD_ChangePasswordPage(_pageNumber);
		break;
	case pH_CALIBRATION_PAGE:
		_pageNumber = LCD_pHCalibPage(_pageNumber);
		break;
	case pH7_CALIBRATION_PAGE:
	case pH10_CALIBRATION_PAGE:
	case pH4_CALIBRATION_PAGE:
		_pageNumber = LCD_subPHCalibPage(_pageNumber);
		break;
	case TUR_CALIBRATION_PAGE:
		_pageNumber = LCD_TurCalibPage(_pageNumber);
		break;
	case BCTUR_CALIBRATION_PAGE:
		_pageNumber = LCD_BCTurCalibPage(_pageNumber);
		break;
	case CLO_CALIBRATION_PAGE:
		_pageNumber = LCD_CloCalibPage(_pageNumber);
		break;
	case BCCLO_CALIBRATION_PAGE:
		_pageNumber = LCD_BCCloCalibPage(_pageNumber);
		break;
	default:
		break;
	}
}
