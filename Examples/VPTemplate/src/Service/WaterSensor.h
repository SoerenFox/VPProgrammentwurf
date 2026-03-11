/*
 * WaterSensor.h
 *
 *  Created on: Mar 10, 2026
 *      Author: kali
 */

#ifndef SRC_SERVICE_WATERSENSOR_H_
#define SRC_SERVICE_WATERSENSOR_H_

#include <stdint.h>
#include "UARTModule.h"
#include "stm32g4xx_hal.h"
#include "System.h"

#include "HardwareConfig.h"
#include "TimerModule.h"
#include "GasSensor.h"

#define WATER_SENSOR_OK                 0
#define WATER_SENSOR_INVALID_PTR       -1
#define WATER_SENSOR_VALUE_INVALID     -2

									// in cm
#define MIN_WATERSENSOR_VALUE			50
#define MAX_WATERSENSOR_VALUE 			1000
#define WATER_WARNING_THRESHOLD     250
#define WATER_EMERGENCY_THRESHOLD   300

									// in ms
#define WATER_WARNING_TIME_MS       10000
#define WATER_EMERGENCY_TIME_MS     5000
#define WATER_TIMEOUT_TIME_MS     	1500

int32_t wasSensorCheckValue(uint32_t cmValue);
int32_t waterSensorOverCmValue(int32_t cmValue);
int32_t waterSensorResetThresholdTimers(uint32_t now);


#endif
