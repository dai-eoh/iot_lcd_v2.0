/*
 * network.hpp
 *
 *  Created on: Jul 21, 2021
 *      Author: EoH.Dai
 */

#ifndef INC_NETWORK_HPP_
#define INC_NETWORK_HPP_
#ifdef __cplusplus
extern "C" {
#endif
#include "define.hpp"

void Network_Init(void);
void Network_Config(void);
void Network_Poll(void);
uint16_t Network_GetCPUTemp(void);
uint16_t Network_GetVoltage(void);
int8_t getSignalQuality(uint8_t _device);
void ADC_Start(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_NETWORK_HPP_ */
