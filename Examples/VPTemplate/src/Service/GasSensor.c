#include "GasSensor.h"

// in ppm
#define MIN_SENSOR_VALUE 200
#define MAX_SENSOR_VALUE 10e3

#define MIN_SENSOR_VOLTAGE 500000	// in µV (0.5V)
#define MAX_SENSOR_VOLTAGE 2500000	// in µV (2.5V)

#define GETTOINT 100				// Factor to get dec to int

int32_t gasSensorInitialize(GasSensor* pSensor, uint32_t convFactor) {
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

uint8_t checkForValideADC(int32_t value1, int32_t value2)
{
    if (value1 < 0 || value2 < 0)
    {
        return 1;   // invalid ADC value
    }

    return 0;       // values valid
}

uint8_t isGasSensorMismatch(int32_t filteredValue1, int32_t filteredValue2, uint32_t percent)
{
    if (filteredValue1 < filteredValue2)
    {
    	uint32_t diff = filteredValue2 - filteredValue1;
    	if ((diff * GETTOINT/filteredValue2) <= percent) return 0;

        return 1;   // invalid ADC value

    } else if (filteredValue2 < filteredValue1)
    {
    	uint32_t diff = filteredValue1 - filteredValue2;
    	if ((diff * GETTOINT/filteredValue1) <= percent) return 0;

    	return 1;   // invalid ADC value
    }

    return 0;       // values valid
}

