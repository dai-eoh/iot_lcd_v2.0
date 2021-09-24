/*
 * variable_lcd.h
 *
 *  Created on: Jul 6, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_VARIABLE_LCD_H_
#define INC_VARIABLE_LCD_H_
#include "define.hpp"
#include "logo.h"
#include "type_list.hpp"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "ButtonMatrix.h"
#include "ST7920_SPI.hpp"
#include "sub_function.hpp"
#include "bitmap_symbol.hpp"
#include "RREFont.h"
#include "Font/fonts_all.h"
#include "RRE_Font/fonts_rre.h"
#include "Kalman.h"
#include "network.hpp"
#include "cpu_utils.h"
#include <spi_flash.hpp>
#include "ModbusMaster.hpp"
#include "ModbusSlave.hpp"
//#define CAL_POSITON
#define TIMER_US 				&htim13

//DISPLAY TITLE
#define TITLE_MBCF 			"MODBUS CONFIG"
#define TITLE_SSCF			"SENSOR CONFIG"
#define TITLE_CLCF			"CALIB CONFIG"
#define TITLE_ALCF			"ALARM CONFIG"
#define TITLE_DVCF			"DEVICE CONFIG"
#define TITLE_WRCF			"WRITE CONIFG"
#define TITLE_AVCF			"ADVANCED CONFIG"
#define TITLE_HEATHY		"HEALTHY"

#define TITLE_PH				"pH CALIBRATION"
#define TITLE_PH7				"pH7 CALIBRATION"
#define TITLE_PH10			"pH10 CALIBRATION"
#define TITLE_PH4				"pH4 CALIBRATION"

#define TITLE_CLO				"CLO CALIBRATION"
#define TITLE_TUR				"TUR CALIBRATION"


#define Y_MAIN_TITLE 		0
#define Y_SUB_TITLE			(Y_MAIN_TITLE + 10)
#define X_SUB_TITLE			1
//DISPLAY VALUE MODBUS
#define X_1ST_NAME			2
#define Y_1ST_NAME			2
#define X_1ST_VALUE			65
#define Y_1ST_VALUE			12
#define X_1ST_UNIT			86
#define Y_1ST_UINIT			22
#define X_ALERT					38
#define Y_ALERT					3
#define SIZE_ALERT			25
#define W_ALARM					15
#define H_ALARM					8
// LOCK PASSSWORD
#define X_LOCK					30
#define Y_LOCK					12
#define LENGTH_PASSWORD	4
#define W_LOCK					30
#define H_LOCK					39
#define DEFAULT_PASSWORD 0001

#define X_CHANGE	58
#define Y_CHANGE	7
#define W_PASSWORD_BUTTON	34
#define H_PASSWORD_BUTTON	11
//
#define TIME_ALERT			700
#define TIME_UNIT			1000
//INVERT
#define WD_INVERT				5
#define OFFSET_INVERT		0
//PAGE
#define ROW_PAGE				4
#define X_CLEAR_PAGE		20
#define Y_CLEAR_PAGE		18
//WRITE
#define WD_WRITE				43
#define HT_WRITE				17
//HEALTHY
#define TIME_HEALTHY 1000
#define TIME_CALIB_PH 90000

extern ModbusRTU modbusRTU[NUM_MODBUS];
extern ProcessData processData[NUM_MODBUS];
extern NetworkData networkData;
extern ModbusMaster ModbusMaster;
extern ModbusSlave ModbusSlave;
extern uint8_t autoSwitch;
extern uint8_t networkStatus;
extern LastResetReason lastResetReason;
extern int8_t networkSignal;
extern uint8_t modbusTotal;
extern uint8_t modbusFailrate;



/*DISPLAY VALUE MODBUS*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t *defaulFont = (uint8_t*)PropPL5x7;
uint8_t *titleFont = (uint8_t*)c64enh;
uint8_t *valueFont2 = (uint8_t*)Term9x14PL;
uint8_t *valueFont1 = (uint8_t*)Chicago21x18;
uint8_t *writeFont = (uint8_t*)Small5x7PLBold;

ST7920_SPI ST7920_SPI;
RREFont    RREFont;
typedef enum PageNumber
{
	MODBUS_VALUE_PAGE = 0,
	MODBUS_CONFIG_PAGE,
	SENSOSR_CONFIG_PAGE,
	CALIB_CONFIG_PAGE,
	ALARM_CONFIG_PAGE,
	pH_CALIBRATION_PAGE,
	TUR_CALIBRATION_PAGE,
	CLO_CALIBRATION_PAGE,
	DEVICE_CONFIG_PAGE,
	WRITE_CONFIG_PAGE,
	ADVANCED_CONFIG_PAGE,
	HEALTHY_PAGE,
	ENTER_PASSWORD_PAGE,
	CHANGE_PASSWORD_PAGE,
	BCCLO_CALIBRATION_PAGE,
	BCTUR_CALIBRATION_PAGE,
	BQTUR_CALIBRATION_PAGE,
	TTSTUR_CALIBRATION_PAGE,
	UTTSTUR_CALIBRATION_PAGE,
	pH7_CALIBRATION_PAGE,
	pH10_CALIBRATION_PAGE,
	pH4_CALIBRATION_PAGE
}PageNumber;

InfoPage modbusValuePage =
{
		.numRow 			= 0,
		.numCol 			= 0,
		.xPosTitle 		= {23, 62, 99, 134, 170},
		.xPosInvert 	= {X_1ST_VALUE, X_1ST_VALUE + (SCR_WD/2) - 1, X_1ST_VALUE, X_1ST_VALUE + (SCR_WD/2)},
		.xPosValue		= {X_1ST_VALUE, X_1ST_VALUE + (SCR_WD/2) - 1, X_1ST_VALUE, X_1ST_VALUE + (SCR_WD/2)},
		.yPos 				= {Y_1ST_VALUE, Y_1ST_VALUE, Y_1ST_VALUE + (SCR_HT/2), Y_1ST_VALUE + (SCR_HT/2)},
		.invertWide   = {4, 4, 4, 4, 4, 4, 4, 4}
};
uint32_t preTimeAlert = 0;

//NUM_CONFIG = 4
uint8_t xName_4[4] = {X_1ST_NAME, X_1ST_NAME, X_1ST_NAME + (SCR_WD/2), X_1ST_NAME + (SCR_WD/2)};
uint8_t yName_4[4] = {Y_1ST_NAME, Y_1ST_NAME + (SCR_HT/2), Y_1ST_NAME, Y_1ST_NAME + (SCR_HT/2)};
uint8_t xUnit_4[4] = {X_1ST_UNIT, X_1ST_UNIT, X_1ST_UNIT + (SCR_WD/2), X_1ST_UNIT + (SCR_WD/2)};
uint8_t yUnit_4[4] = {Y_1ST_UINIT, Y_1ST_UINIT + (SCR_HT/2), Y_1ST_UINIT, Y_1ST_UINIT + (SCR_HT/2)};
uint8_t xAlert_4[4] = {X_ALERT, X_ALERT, X_ALERT + (SCR_WD/2), X_ALERT + (SCR_WD/2)};
uint8_t yAlert_4[4] = {Y_ALERT, Y_ALERT + (SCR_HT/2), Y_ALERT,  Y_ALERT + (SCR_HT/2)};
uint8_t xValue_4[4] = {X_1ST_VALUE, X_1ST_VALUE, X_1ST_VALUE + (SCR_WD/2), X_1ST_VALUE + (SCR_WD/2)};
uint8_t yValue_4[4] = {Y_1ST_VALUE, Y_1ST_VALUE + (SCR_HT/2), Y_1ST_VALUE, Y_1ST_VALUE + (SCR_HT/2)};

uint8_t xAlarm_4[4] = {5, 5, 101, 101};
uint8_t yAlarm_4[4] = {21, 53, 21, 53};
//NUM_CONFIG = 3
uint8_t xName_3[3] = {X_1ST_NAME, X_1ST_NAME, X_1ST_NAME + (SCR_WD/2)};
uint8_t yName_3[3] = {Y_1ST_NAME, Y_1ST_NAME + (SCR_HT/2) , Y_1ST_NAME};
uint8_t xUnit_3[3] = {X_1ST_UNIT, X_1ST_UNIT, X_1ST_UNIT + (SCR_WD/2)};
uint8_t yUnit_3[3] = {Y_1ST_UINIT, Y_1ST_UINIT + (SCR_HT/2), Y_1ST_UINIT + (SCR_HT/2)};
uint8_t xAlert_3[3] = {X_ALERT, X_ALERT , X_ALERT + (SCR_WD/2)};
uint8_t yAlert_3[3] = {Y_ALERT, Y_ALERT + (SCR_HT/2),  (SCR_HT - SIZE_ALERT)/2};
uint8_t xValue_3[3] = {X_1ST_VALUE, X_1ST_VALUE, X_1ST_VALUE + (SCR_WD/2)};
uint8_t yValue_3[3] = {Y_1ST_VALUE, Y_1ST_VALUE + (SCR_HT/2), (SCR_HT/2) - 5};

uint8_t xAlarm_3[3] = {5, 5, 101};
uint8_t yAlarm_3[3] = {21, 53, 53};
//NUM_CONFIG = 2
uint8_t xName_2[2] = {X_1ST_NAME, X_1ST_NAME + (SCR_WD/2)};
uint8_t yName_2[2] = {Y_1ST_NAME, Y_1ST_NAME};
uint8_t xUnit_2[2] = {X_1ST_UNIT, X_1ST_UNIT + (SCR_WD/2)};
uint8_t yUnit_2[2] = {Y_1ST_UINIT + (SCR_HT/2), Y_1ST_UINIT + (SCR_HT/2)};
uint8_t xAlert_2[2] = {X_ALERT, X_ALERT + (SCR_WD/2)};
uint8_t yAlert_2[2] = {(SCR_HT - SIZE_ALERT)/2, (SCR_HT - SIZE_ALERT)/2};
uint8_t xValue_2[2] = {X_1ST_VALUE, X_1ST_VALUE + (SCR_WD/2)};
uint8_t yValue_2[2] = {(SCR_HT/2) - 5, (SCR_HT/2) - 5};

uint8_t xAlarm_2[2] = {5, 101};
uint8_t yAlarm_2[2] = {53, 53};
//NUM_CONFIG = 1
uint8_t xName_1[1] = {X_1ST_NAME};
uint8_t yName_1[1] = {Y_1ST_NAME};
uint8_t xUnit_1[1] = {X_1ST_UNIT + (SCR_WD/2)};
uint8_t yUnit_1[1] = {Y_1ST_UINIT + (SCR_HT/2) - 6};
uint8_t xAlert_1[1] = {(SCR_WD-SIZE_ALERT)/2};
uint8_t yAlert_1[1] = { (SCR_HT - SIZE_ALERT)/2};
uint8_t xValue_1[1] = {SCR_WD - 24*3};
uint8_t yValue_1[1] = {(SCR_HT - 21)/2};

uint8_t xAlarm_1[1] = {5};
uint8_t yAlarm_1[1] = {53};
/*---------------------------------------------------------------------------------------------------------------------------------------*/
/*MODBUS CONFIG*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/

InfoPage modbusConfigPage =
{
		.numRow 			= 8,
		.numCol 			= 6,
		.xPosTitle 		= {21, 50, 80, 114, 139, 167},
		.xPosInvert 	= {34, 63, 94, 125, 152, 180},
		.xPosValue		= {34, 58, 94, 119, 148, 182},
		.yPos 				= {20, 31, 42, 53, 20, 31, 42, 53},
		.invertWide   = {3, 3, 3, 3, 3, 3},
		.multipage		= true
};

typedef enum InfoModbus
{
	Addr = 0,
	Func,
	RegID,
	Leng,
	Scale,
	Type
}InfoModbus;

typedef enum CommandKey
{
	KEY_NUMBER,
	KEY_UPDOWN
}CommandKey;

typedef enum UpDownCommand
{
	UP_COMMAND,
	DOWN_COMMAND
}UpDownCommand;

typedef enum SubPageNumber
{
	SUBPAGE_1 = 0,
	SUBPAGE_2
}SubPageNumber;

typedef bool(*IsFloatHandle)(uint8_t);
typedef int16_t(*DisplayHandle)(const uint8_t xValue, const uint8_t yValue, InfoPage infoPage, uint8_t keyValue, IsFloatHandle isFloatHandle);


ModbusConfig modbusConfig[NUM_MODBUS] = {0};
const char * xTitleMBCF[6] = {"Addr", "Func", "RegID", "Len", "Scal", "Type"};
int8_t xCursor, yCursor;
char numBuffer[10] = {0};
const char * dataType[10] = {"UI16", "IN16", "UI32", "FLO1", "FLO2"};

/*---------------------------------------------------------------------------------------------------------------------------------------*/
/*SENSOR CONFIG*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
const uint8_t numName = 7;
const uint8_t numTitle = 6;
InfoPage sensorConfigPage =
{
		.numRow 			= 8,
		.numCol 			= 5,
		.xPosTitle		= {20, 54, 89, 117, 161},
		.xPosInvert 	= {34, 69, 100, 145, 186},
		.xPosValue		= {33, 64, 95, 145, 186},
		.yPos 				= {20, 31, 42, 53, 20, 31, 42, 53},
		.invertWide   = {3, 3, 3, 7, 7},
		.multipage		= true

};

typedef enum InfoSensor
{
	Name=0,
	Point,
	Average,
	MaxValue,
	MinValue
}InfoSensor;
const char * xTitleSSCF[numTitle] = {"Name", "Point", "Ave", "MaxVal", "MinVal"};
const char * nameSensor[numName] = {"TEM", "HUM", "pH ", "CLO", "TUR", "FLO", "STA"};
const char * uintSensor[numName] = { "  ~C","   %","  pH" , "mg/L", " NTU", "L/mi", "    "};
SensorConfig sensorConfig[NUM_MODBUS] = {0};
/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*CALIBRATION*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
InfoPage calibConfigPage =
{
		.numRow 			= 8,
		.numCol 			= 1,
		.xPosTitle		= {123, 158, 164},
		.xPosInvert 	= {148, 182, 184},
		.xPosValue		= {146, 183, 183},
		.yPos 				= {20, 31, 42, 53, 20, 31, 42, 53},
		.invertWide   = {6,6},
		.multipage		= true
};

typedef enum InfoCalib
{
	Offset=0
}InfoCalib;

const char * xTitleCLCF[numTitle] = {"Offset"};
CalibConfig calibConfig[NUM_MODBUS] = {0};
/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*ALARM*/
InfoPage alarmConfigPage =
{
		.numRow 			= 8,
		.numCol 			= 2,
		.xPosTitle		= {64, 148, 164},
		.xPosInvert 	= {95, 181, 184},
		.xPosValue		= {96, 182, 183},
		.yPos 				= {20, 31, 42, 53, 20, 31, 42, 53},
		.invertWide   = {6,6},
		.multipage		= true
};

typedef enum InfoAlarm
{
	LowValue = 0,
	HighValue
}InfoAlarm;

const char * xTitleALCF[2] = {"LowValue", "HighValue"};
AlarmConfig alarmConfig[NUM_MODBUS] = {0};


/*---------------------------------------------------------------------------------------------------------------------------------------*/


/*DEVICE CONFIG*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
InfoPage DeviceConfigPage =
{
		.numRow 			= 4,
		.numCol 			= 2,
		.xPosTitle		= {2, 104},
		.xPosInvert 	= {85, 181},
		.xPosValue		= {85, 181},
		.yPos 				= {12, 25, 38, 51},
		.invertWide   = {4,4},
		.multipage		= false
};

typedef enum InfoDevice
{
	SlaveAddr = 0,
	SlaveBaud,
	MasterBaud,
	NumConfig,
	SampleTime,
	DisplayTime,
	Brightness,
	KeypadBell
}InfoDevice;
const char * xTitleDVCF[10] = {"SlaveAddr", "SlaveBaud", "MasterBaud", "NumConfig","SampleTime", "DisplayTime", "Brightness", "KeypadBell"};
const char * deviceBaud[10] = {"4800", "9600", "19200", "38400"};
const char * titleKeypadBell[2] = {"ON", "OFF"};
DeviceConfig deviceConfig = {0};
/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*WRITE CONFIG PAGE*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
typedef enum InfoWrite
{
	Address = 0,
	Function,
	RegisterID,
	DataType,
	ValueWrite,
	WriteButton
}InfoWrite;
InfoPage WriteConfigPage =
{
		.numRow 			= 6,
		.numCol 			= 1,
		.xPosTitle		= {2, 104},
		.xPosInvert 	= {85, 181},
		.xPosValue		= {85, 181},
		.yPos 				= {12, 25, 38, 51, 12},
		.invertWide   = {6,6},
		.multipage		= false,
		.numButton    = 1,
		.listButton   = {WriteButton},
		.xButton      = {123},
		.yButton			= {29},
		.wButton      = {WD_WRITE},
		.hButton			=	{HT_WRITE}
};

const char * xTitleWRCF[10] = {"Address", "Function", "RegisterID", "DataType", "Value"};
const char * dataTypeWCFG[10] = {"UINT16", "INT16", "UINT32", "BOOL", "FLOAT1", "FLOAT2",};
const char * writeCoil[2] = {"ON", "OFF"};
const char * writeFunction[3] = {"5", "6", "16"};
WriteConfig writeConfig = {0};
uint8_t xWrite, yWrite;
uint8_t xRes, yRes;
/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*ADVANCED CONFIG PAGE*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/

InfoPage advancedConfigPage =
{
		.numRow 			= 8,
		.numCol 			= 1,
		.xPosTitle		= {123, 158, 164},
		.xPosInvert 	= {141, 182, 184},
		.xPosValue		= {142, 183, 183},
		.yPos 				= {20, 31, 42, 53, 20, 31, 42, 53},
		.invertWide   = {4,4},
		.multipage		= true
};

typedef enum InfoAdvanced
{
	Filter = 0
}InfoAdvanced;

const char * xTitleAVCF[numTitle] = {"Filter"};
AdvancedConfig advancedConfig[NUM_MODBUS] = {0};
#endif /* INC_VARIABLE_LCD_H_ */

/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*HEALTHY PAGE*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
InfoPage healthyPage =
{
		.numRow 			= 4,
		.numCol 			= 2,
		.xPosTitle		= {2, 104},
		.xPosInvert 	= {85, 181},
		.xPosValue		= {88, 183},
		.yPos 				= {12, 25, 38, 51},
		.invertWide   = {6,6},
		.multipage		= false
};

typedef enum InfoHealthy
{
	Voltage = 0,
	CPU_Temp,
	CPU_Usage,
	ResetCode,
	ModbusFR,
	NetSignal,
	NetStatus,
	FirmVersion
}InfoHealthy;
const char * xTitleHealthy[8] = {"Voltage", "CPU-Temp", "CPU-Usage", "ResetCode", "ModbusFR", "NetSignal", "NetStatus", "FirmVer"};
const char * ReasonResetStr[8] = 	{"UNKN  ", "LOPW  ", "WWDG  ", "IWDG  ", "SOFT  ", "PWDN  ", "EPIN  ", "BROW  "};
/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*PASSWORD CONIFG PAGE*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
typedef enum InfoPassword
{
	EnterPassword = 0,
	OKPasswordButton,
	ChangePasswordButton
}InfoPassword;


InfoPage PasswordConfigPage =
{
		.numRow 			= 3,
		.numCol 			= 1,
		.xPosTitle		= {2, 104},
		.xPosInvert 	= {X_LOCK + 95},
		.xPosValue		= {X_LOCK + 95},
		.yPos 				= {Y_LOCK + 15},
		.invertWide   = {6},
		.multipage		= false,
		.numButton    = 2,
		.listButton   = {OKPasswordButton, ChangePasswordButton},
		.xButton      = {X_LOCK + 60, X_LOCK + 97},
		.yButton			= {Y_LOCK + 33, Y_LOCK + 33},
		.wButton      = {W_PASSWORD_BUTTON, W_PASSWORD_BUTTON},
		.hButton			=	{H_PASSWORD_BUTTON, H_PASSWORD_BUTTON}
};

PasswordConfig passwordConfig = {0};
bool flagPassword = false;
uint32_t tempPassword = 0;
/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*CHANGE PASSWORD PAGE*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
typedef enum ChangePasswordEnum
{
	CurrentPassword = 0,
	NewPassword,
	OKChangePasswordButton
}ChangePasswordEnum;

InfoPage ChangePasswordPage =
{
		.numRow 			= 3,
		.numCol 			= 1,
		.xPosTitle		= {2, 104},
		.xPosInvert 	= {X_CHANGE + 79},
		.xPosValue		= {X_CHANGE + 79},
		.yPos 				= {Y_CHANGE + 12, Y_CHANGE + 24},
		.invertWide   = {6},
		.multipage		= false,
		.numButton    = 1,
		.listButton   = {OKChangePasswordButton},
		.xButton      = {X_CHANGE + 63},
		.yButton			= {Y_CHANGE + 42},
		.wButton      = {W_PASSWORD_BUTTON},
		.hButton			=	{H_PASSWORD_BUTTON}
};

ChangePasswordConfig changePasswordConfig = {0};

/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*pH CALIB PAGE*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/

typedef enum pHCalibEnum
{
	IDpH = 0,
	StartButtonpH
}pHCalibEnum;

typedef enum TypePHCalib
{
	CalibpH7 = 0,
	CalibpH10,
	CalibpH4
}TypePHCalib;

typedef enum subPHCalibEnum
{
	Standard_PH = 0,
	CurrentValue_PH,
	Status_PH,
	Progress_PH
}subPHCalibEnum;

typedef enum StatePHCalib
{
	Waiting_PH = 0,
	Running_PH ,
	Success_PH,
	ERROR_PH
}StatePHCalib;

InfoPage pHCalibConfigPage =
{
		.numRow 			= 2,
		.numCol 			= 1,
		.xPosTitle		= {2},
		.xPosInvert 	= {117},
		.xPosValue		= {118},
		.yPos 				= {21},
		.invertWide   = {3},
		.multipage		= false,
		.numButton    = 1,
		.listButton   = {StartButtonpH},
		.xButton      = {65},
		.yButton			= {35},
		.wButton      = {62}, // calculate in code
		.hButton			=	{15}  // calculate in code
};

PHCalibConfig pHCalibConfig =
{
		.id = 1
};

InfoPage subpPHCalibConfigPage =
{
		.numRow 			= 4,
		.numCol 			= 1,
		.xPosTitle		= {46},
		.xPosInvert 	= {137},
		.xPosValue		= {137},
		.yPos 				= {15, 27, 39, 51},
		.invertWide   = {4, 4, 8, 8},
		.multipage		= false,

};

const char *titlePH[1] = {"ID pH"};
const char *titlePHButton[1] = {"START CALIB"};
const char *statusPHCalib[4] = {"Waiting","Running", "Success", "Error"};
StandardPH standardPH = {0};

const uint8_t xPHProgess = 91;
const uint8_t yPHProgess = 51;
const uint8_t wPHProgess = 51;
const uint8_t hPHProgess = 8;

/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*TUR CALIB PAGE*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
typedef enum TurCalibEnum
{
	IDTur = 0,
	TypeTur,
	StartButtonTur
}TurCalibEnum;

typedef enum TypeTurEnum
{
	BCTur=0,
	BQTur,
	TTSTur,
	UTTSTur
}TypeTurEnum;

InfoPage turCalibConfigPage =
{
		.numRow 			= 3,
		.numCol 			= 1,
		.xPosTitle		= {2},
		.xPosInvert 	= {117},
		.xPosValue		= {118},
		.yPos 				= {15, 29},
		.invertWide   = {4},
		.multipage		= false,
		.numButton    = 1,
		.listButton   = {StartButtonTur},
		.xButton      = {65},
		.yButton			= {42},
		.wButton      = {62}, // calculate in code
		.hButton			=	{15}  // calculate in code
};

TurCalibConfig turCalibConfig =
{
		.id = 1,
		.type = 0
};
const char *titleTur[2] = {"ID Tur", "Type"};
const char *titleTurButton[1] = {"START CALIB"};
const char *typeTur[4] = {"  BC", "  BQ", " TTS", "UTTS"};

/*BC TUR CALIB PAGE*/
InfoPage bcTurCalibConfigPage =
{
		.numRow 			= 3,
		.numCol 			= 1,
		.xPosTitle		= {46},
		.xPosInvert 	= {135},
		.xPosValue		= {137},
		.yPos 				= {11, 22, 33, 44, 55},
		.invertWide   = {6, 6},
		.multipage		= false
};

typedef enum TurBCCalibEnum
{
	DecPointTurBC=0,
	SensCalibTurBC,
	SensCmdTurBC
}TurBCCalibEnum;

const uint8_t NumTitleBCTur = 5;
const char *titleBCTur[NumTitleBCTur] = {"Dec Point", "Sens Calib", "Sens Cmd", "Sens Status", "Tur Value"};
const char *titleSensCmdTurBC[2] = {"SensCal", "Reset"};
const char *titlestatusTurCalib[3] = {"Running", "Success", "Error"};

StandardTurBC standardTurBC = {0};
StandardTurBCUpdate standardTurBCUpdate = {0};

/*---------------------------------------------------------------------------------------------------------------------------------------*/

/*TUR CALIB PAGE*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/
typedef enum CloCalibEnum
{
	IDClo = 0,
	TypeClo,
	StartButtonClo
}CloCalibEnum;

typedef enum TypeCloEnum
{
	BCClo=0,
	BQClo,
	TTSClo,
	UTTSClo
}TypeCloEnum;

InfoPage cloCalibConfigPage =
{
		.numRow 			= 3,
		.numCol 			= 1,
		.xPosTitle		= {2},
		.xPosInvert 	= {117},
		.xPosValue		= {118},
		.yPos 				= {15, 29},
		.invertWide   = {4},
		.multipage		= false,
		.numButton    = 1,
		.listButton   = {StartButtonClo},
		.xButton      = {65},
		.yButton			= {42},
		.wButton      = {62}, // calculate in code
		.hButton			=	{15}  // calculate in code
};

CloCalibConfig cloCalibConfig =
{
		.id = 1,
		.type = 0
};

const char *titleClo[2] = {"ID Tur", "Type"};
const char *titleCloButton[1] = {"START CALIB"};
const char *typeClo[4] = {"  BC", "  BQ", " TTS", "UTTS"};

/*BC CLO CALIB PAGE*/
InfoPage bcCloCalibConfigPage =
{
		.numRow 			= 3,
		.numCol 			= 2,
		.xPosTitle		= {2, 99},
		.xPosInvert 	= {85, 182},
		.xPosValue		= {87, 184},
		.yPos 				= {11, 22, 33, 44, 55},
		.invertWide   = {6, 6},
		.multipage		= false,
		.rowPerCol		= 3
};

typedef enum CloBCCalibEnum
{
	ZeroDecPointClo=0,
	ZeroCalibClo,
	ZeroCmdClo,
	SensPointClo,
	SensCalibClo,
	SensCmdClo
}CloBCCalibEnum;

const uint8_t NumTitleBCClo = 9;
const char *titleBCClo[NumTitleBCClo] = {"Dec Point", "Zero Calib", "Zero Cmd", "Dec Point", "Sens Calib", "Sens Cmd", "Zero Status", "Sens Status", "Clo Value"};
const char *titleCmdCloBC[2] = {"SensCal", "Reset"};
const char *titlestatusCloCalib[3] = {"Running", "Success", "Error"};

StandardCloBC standardCloBC = {0};
StandardCloBCUpdate standardCloBCUpdate = {0};
uint8_t xValueCloBC = 134;
/*---------------------------------------------------------------------------------------------------------------------------------------*/


