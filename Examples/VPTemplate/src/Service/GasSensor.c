#include "GasSensor.h"

// in ppm
#define MIN_SENSOR_VALUE 200
#define MAX_SENSOR_VALUE 10000

#define MIN_SENSOR_VOLTAGE 500000	// in µV (0.5V)
#define MAX_SENSOR_VOLTAGE 2500000	// in µV (2.5V)

#define GETTOINT 100				// Factor to get dec to int

#define GAS_WARNING_THRESHOLD     3000
#define GAS_EMERGENCY_THRESHOLD   5000

#define GAS_WARNING_TIME_MS       5000
#define GAS_EMERGENCY_TIME_MS     3000

static uint32_t warningStartTick = 0;
static uint32_t emergencyStartTick = 0;

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

int32_t gasSensorReadPpmValue(GasSensor* pSensor, ADC_Channel_t adcChannel)
{
    uint32_t raw = adcReadChannel(adcChannel);

    gasSensorSetSensorVoltage(pSensor, raw);

    return gasSensorGetSensorValue(pSensor);
}

int32_t gasSensorOverPpmValue(int32_t filteredValue1, int32_t filteredValue2)
{
		uint32_t now = HAL_GetTick();
		/* Emergency >5000 ppm for 3 seconds */
	    if (filteredValue1 > GAS_EMERGENCY_THRESHOLD || filteredValue2 > GAS_EMERGENCY_THRESHOLD)
	    {
	        if (emergencyStartTick == 0)
	        {
	            emergencyStartTick = now;
	        }

	        if ((now - emergencyStartTick) >= GAS_EMERGENCY_TIME_MS)
	        {
	            return 2;
	        }
	    }
	    else
	    {
	        emergencyStartTick = 0;
	    }

	    /* Warning >3000 ppm for 5 seconds */
	    if (filteredValue1 > GAS_WARNING_THRESHOLD || filteredValue2 > GAS_WARNING_THRESHOLD)
	    {
	        if (warningStartTick == 0)
	        {
	            warningStartTick = now;
	        }

	        if ((now - warningStartTick) >= GAS_WARNING_TIME_MS)
	        {
	            return 1;
	        }
	    }
	    else
	    {
	        warningStartTick = 0;
	    }

	    return 0;
}

int32_t gasSensorResetThresholdTimers(uint32_t now)
{
    emergencyStartTick = now;
    warningStartTick = now;
}

