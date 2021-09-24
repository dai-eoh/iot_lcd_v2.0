/*
 * process.hpp
 *
 *  Created on: Jun 23, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_PROCESS_HPP_
#define INC_PROCESS_HPP_
#ifdef __cplusplus
extern "C" {
#endif
#include "define.hpp"

void ModbusSlave_Init();
void ModbusMaster_Init();
void ModbusMaster_Poll(void);
void ModbusSlave_Poll(void);
void System_GetLastReason(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_PROCESS_HPP_ */
