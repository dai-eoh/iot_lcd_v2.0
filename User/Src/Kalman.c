#include "Kalman.h"

double q[NUM_KALMAN]; //process noise covariance
double r[NUM_KALMAN]; //measurement noise covariance
double x[NUM_KALMAN]; //value
double p[NUM_KALMAN]; //estimation error covariance
double k[NUM_KALMAN]; //kalman gain



void Kalman_SetConfig(double process_noise, double sensor_noise, double estimated_error, double intial_value, int index)
{
    q[index] = process_noise;
    r[index] = sensor_noise;
    p[index] = estimated_error;
    x[index] = intial_value;
}


double Kalman_GetValue(double measurement, int index)
{
    /* Updates and gets the current measurement value */
    //prediction update
    //omit x = x
    p[index] = p[index] + q[index];

    //measurement update
    k[index] = p[index] / (p[index] + r[index]);
    x[index] = x[index] + k[index] * (measurement - x[index]);
    p[index] = (1 - k[index]) * p[index];

    return x[index];
}

void Kalman_SetProcessNoise(uint8_t _index, double _number)
{
	if(_number < 0.001f)
		return;
	Kalman_SetConfig(_number, SENSOR_NOISE, ESTIMATED_ERROR, INTIAL_VALUE, _index);
}
