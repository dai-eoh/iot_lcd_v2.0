#ifndef KALMAN_H__
#define KALMAN_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stdint.h"

#define NUM_KALMAN			10
#define SENSOR_NOISE  	10
#define ESTIMATED_ERROR 1023
#define INTIAL_VALUE   	0


void Kalman_SetProcessNoise(uint8_t _index, double _number);

void Kalman_SetConfig(double process_noise, double sensor_noise, double estimated_error, double intial_value, int index);

double Kalman_GetValue(double measurement, int index);

#ifdef __cplusplus
}
#endif

#endif
