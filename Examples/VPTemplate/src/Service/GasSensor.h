#ifndef _GAS_SENSOR_H_
#define _GAS_SENSOR_H_

#include <stdint.h>
#include "ADCModule.h"
#include "stm32g4xx_hal.h"
#include "System.h"

#include "HardwareConfig.h"
#include "TimerModule.h"

#define SENSOR_OK 				 0
#define SENSOR_INVALID_PTR 		-1
#define SENSOR_VALUE_INVALID 	-2
#define SENSOR_VOLTAGE_INVALID 	-3
#define SENSOR_PPMVALUE_INVALID -4

#define WARNINGTRIGGER 1
#define EMERGENCYTRIGGER 2

typedef struct _GasSensor {
	// Sensor voltage in µV
	uint32_t sensorVoltage;

	// Conversion factor µV per ppm
	uint32_t conversionFactor;
} GasSensor;

int32_t	gasSensorInitialize(GasSensor* pSensor, uint32_t convFactor);
int32_t gasSensorSetSensorVoltage(GasSensor* pSensor, uint32_t sensorVoltage);

int32_t gasSensorGetSensorValue(GasSensor* pSensor);
int32_t gasSensorGetSensorVoltage(GasSensor* pSensor);

int8_t checkForValideADC(int32_t value1, int32_t value2);
int8_t isGasSensorMismatch(int32_t filteredValue1, int32_t filteredValue2, uint32_t percent);
int32_t gasSensorReadPpmValue(GasSensor* pSensor, ADC_Channel_t adcChannel);
int32_t calculateAvgPpmValue(int32_t filteredValue1, int32_t filteredValue2);
int32_t gasSensorOverPpmValue(int32_t avgValue);
void gasSensorResetThresholdTimers(uint32_t now);

#endif
