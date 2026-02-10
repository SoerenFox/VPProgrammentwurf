#ifndef _GAS_SENSOR_H_
#define _GAS_SENSOR_H_

#include <stdint.h>

#define SENSOR_OK 0
#define SENSOR_INVALID_PTR -1
#define SENSOR_VALUE_INVALID -2
#define SENSOR_VOLTAGE_INVALID -3

typedef struct _GasSensor {
	// Sensor voltage in µV
	uint32_t sensorVoltage;

	// Conversion factor µV per ppm
	uint32_t conversionFactor;
} GasSensor;

int32_t gasSesonsrInitialize(GasSensor* pSensor, uint32_t convFactor);
int32_t gasSensorSetSensorVoltage(GasSensor* pSensor, uint32_t sensorVoltage);

int32_t gasSensorGetSensorValue(GasSensor* pSensor);
int32_t gasSensorGetSensorVoltage(GasSensor* pSensor);

#endif
