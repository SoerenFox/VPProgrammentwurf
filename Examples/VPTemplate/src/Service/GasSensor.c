#include "GasSensor.h"

// in ppm
#define MIN_SENSOR_VALUE 200
#define MAX_SENSOR_VALUE 10e3

// in µV
#define MIN_SENSOR_VOLTAGE 2.5e6
#define MAX_SENSOR_VOLTAGE 5e6

int32_t gasSesonsrInitialize(GasSensor* pSensor, uint32_t convFactor) {
	if (!pSensor) return SENSOR_INVALID_PTR;

	pSensor->sensorVoltage = 0;
	pSensor->conversionFactor = convFactor;

	return SENSOR_OK;
}

int32_t gasSensorSetSensorVoltage(GasSensor* pSensor, uint32_t sensorVoltage) {
	if (!pSensor) return SENSOR_INVALID_PTR;

	pSensor->sensorVoltage = sensorVoltage;

	return SENSOR_OK;
}

int32_t gasSensorGetSensorValue(GasSensor* pSensor) {
	if (!pSensor) return SENSOR_INVALID_PTR;

	int32_t value = (int32_t)(pSensor->sensorVoltage / pSensor->conversionFactor);

	if (value < MIN_SENSOR_VALUE || value > MAX_SENSOR_VALUE) return SENSOR_VALUE_INVALID;

	return value;
}

int32_t gasSensorGetSensorVoltage(GasSensor* pSensor) {
	if (!pSensor) return SENSOR_INVALID_PTR;

	if (pSensor->sensorVoltage < MIN_SENSOR_VOLTAGE|| pSensor->sensorVoltage > MAX_SENSOR_VOLTAGE) return SENSOR_VOLTAGE_INVALID;

	return pSensor->sensorVoltage;
}
// <|>
