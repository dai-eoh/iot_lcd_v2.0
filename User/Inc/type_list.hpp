/*
 * type_list.hpp
 *
 *  Created on: Jul 5, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_TYPE_LIST_HPP_
#define INC_TYPE_LIST_HPP_
#include "stdint.h"
#include "stdbool.h"

struct RRE_Font {
  uint8_t type;
  uint8_t wd;
  uint8_t ht;
  uint8_t firstCh;
  uint8_t lastCh;
  const uint8_t *rects;
  const uint16_t *offs;
};

typedef enum FloatType
{
	BIG_ENDIAN_ = 0,
	LIT_ENDIAN,
	MIDBIG_ENDIAN,
	MIDLIT_ENDIAN
}FloatType;


/// @brief  Possible STM32 system reset causes
typedef enum LastResetReason
{
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_LOW_POWER_RESET,
    RESET_CAUSE_WINDOW_WATCHDOG_RESET,
    RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
    RESET_CAUSE_SOFTWARE_RESET,
    RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
    RESET_CAUSE_EXTERNAL_RESET_PIN_RESET,
    RESET_CAUSE_BROWNOUT_RESET,
} LastResetReason;

///
typedef enum FirmwareCode
{
	UPDATE_FIRMWARE_CODE = 0x10,
	RUN_FIRMWARE_CODE = 0x20
}FirmwareCode;


typedef enum DataTypeEnum
{
	UI16 = 0,
	IN16,
	UI32,
	FLO1,
	FLO2
}DataTypeEnum;
///
typedef enum DataTypeWCFG
{
	UINT16 = 0,
	INT16,
	UINT32,
	BOOL,
	FLOAT1,
	FLOAT2
}DataTypeWCFG;

typedef enum WriteCoil
{
	ON = 0,
	OFF
}WriteCoil;

typedef enum WriteFunction
{
	FUNCTION_5 = 0,
	FUNCTION_6,
	FUNCTION_16
}WriteFunction;

typedef enum AlarmStatus
{
	NORMAL = 0,
	LOW,
	HIGH
}AlarmStatus;

typedef enum CalibResponseStatus
{
	NO_RESPONSE,
	CLO_ZERO_RESPONSE,
	CLO_SENS_RESPONSE,
	PH_RESPONSE,
	TUR_RESPONSE
}CalibResponseStatus;

typedef struct InfoPage
{
	uint8_t numRow;
	uint8_t numCol;
	uint8_t xPosTitle[10];
	uint8_t xPosInvert[10];
	uint8_t xPosValue[10];
	uint8_t yPos[10];
	uint8_t invertWide[10];
	bool    multipage;
	uint8_t   rowPerCol;
	uint8_t numButton;
	uint8_t listButton[10];
	uint8_t xButton[10];
	uint8_t yButton[10];
	uint8_t wButton[10];
	uint8_t hButton[10];
}InfoPage;

typedef struct ModbusConfig
{
	uint16_t addr;
	uint16_t func;
	uint16_t regId;
	uint16_t length;
	int16_t  scale;
	uint16_t dataType;

}ModbusConfig;

typedef struct SensorConfig
{
	uint32_t name;
	uint32_t numPoint;
	uint32_t average;
//	float filterConst;
	float maxValue;
	float minValue;
}SensorConfig;

typedef struct CalibConfig
{
	float   offset;
}CalibConfig;

typedef struct AlarmConfig
{
	float lowValue;
	float highValue;
}AlarmConfig;

typedef struct ModbusRTU
{
	uint8_t addr;
	uint8_t func;
	uint16_t regID;
	uint16_t length;
	bool failFlag;
	uint16_t extraArray[64];
}ModbusRTU;

typedef struct ProcessData
{
	float value;
	uint8_t index;
	uint32_t arrValue[25];
	bool failFlag;
	uint8_t alarmStatus;
}ProcessData;

typedef struct AlertModbus
{
	uint8_t index;
	uint8_t value;
}AlertModbus;

typedef struct NetworkData
{
	char chipImei[20];
	char configID[10];
	char controlID[10];
	uint8_t delay;
	uint8_t numConfig;
	uint8_t numControl;
	uint32_t baudrate;
	char ssid[50];
	char password[50];
}NetworkData;

typedef struct DeviceConfig
{
	uint16_t slaveAddr;
	uint16_t slaveBaud;
	uint16_t masterBaud;
	uint16_t numConfig;
	uint16_t sampleTime;
	uint16_t displayTime;
	uint16_t brightness;
	uint16_t keypadBell;
}DeviceConfig;

typedef struct WriteConfig
{
	uint32_t addr;
	uint32_t func;
	uint32_t regID;
	uint32_t dataType;
	uint32_t value;
}WriteConfig;

typedef struct AdvancedConfig
{
	float filter;
}AdvancedConfig;

typedef struct PasswordConfig
{
	uint32_t password;
}PasswordConfig;

typedef struct ChangePasswordConfig
{
	uint32_t currentPassword;
	uint32_t newPassword;
}ChangePasswordConfig;

typedef struct PHCalibConfig
{
	uint16_t id ;
}PHCalibConfig;

typedef struct StandardPH
{
	float standard;
	float currentValue;
	uint8_t status;
	uint8_t progess;
}StandardPH;

typedef struct TurCalibConfig
{
	uint16_t id;
	uint16_t type;
}TurCalibConfig;

typedef struct StandardTurBC
{
	uint32_t point;
	uint32_t sensCalib;
	uint32_t sensCmd;
}StandardTurBC;

typedef struct StandardTurBCUpdate
{
	uint8_t sensStatus;
	float turValue;
}StandardTurBCUpdate;

typedef struct ModbusCalib
{
	uint8_t type;
	ModbusRTU command;
	ModbusRTU response;
}ModbusCalib;

typedef struct ResponseCalib
{
	uint8_t type;
	uint8_t status;
	bool flag;
}ResponseCalib;

typedef struct CloCalibConfig
{
	uint16_t id;
	uint16_t type;
}CloCalibConfig;

typedef struct StandardCloBC
{
	uint32_t zeroPoint;
	uint32_t zeroCalib;
	uint32_t zeroCmd;
	uint32_t sensPoint;
	uint32_t sensCalib;
	uint32_t sensCmd;
}StandardCloBC;

typedef struct StandardCloBCUpdate
{
	uint8_t zeroStatus;
	uint8_t sensStatus;
	float cloValue;
}StandardCloBCUpdate;

#endif /* INC_TYPE_LIST_HPP_ */
