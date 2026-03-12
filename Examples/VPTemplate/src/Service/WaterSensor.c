/*
 * WaterSensor.c
 *
 *  Created on: Mar 10, 2026
 *      Author: kali
 */
#include "WaterSensor.h"

static uint32_t warningStartTick 	= 0; // This variable stores the tick count when the water level first exceeded the warning threshold
static uint32_t emergencyStartTick 	= 0; // This variable stores the tick count when the water level first exceeded the emergency threshold
static int lastInputTime;

// This function checks if the given cm value is valid and if we have received a new value within the defined timeout, otherwise it returns an error status
int32_t wasSensorCheckValue(uint32_t cmValue)
{
	uint32_t now = HAL_GetTick();

	radioConnectGetLastInputTime(&gRadioConnect, &lastInputTime);
	if (now - lastInputTime > WATER_TIMEOUT_TIME_MS)
	{
		return UART_ERR_RECEIVE;
	}

	if (cmValue == 0) return cmValue;

	if (cmValue < MIN_WATERSENSOR_VALUE || cmValue > MAX_WATERSENSOR_VALUE) return WATER_SENSOR_VALUE_INVALID;

	return cmValue;
}

// This function checks if the given cm value exceeds the defined thresholds for waterSensor with their specific time limits and returns the corresponding status
int32_t waterSensorOverCmValue(int32_t cmValue)
{
		uint32_t now = HAL_GetTick();

		// Emergency >300 cm for 5 seconds
	    if (cmValue > WATER_EMERGENCY_THRESHOLD)
	    {
	        if (emergencyStartTick == 0)
	        {
	            emergencyStartTick = now;
	        }

	        if ((now - emergencyStartTick) >= WATER_EMERGENCY_TIME_MS)
	        {
	            return EMERGENCYTRIGGER;
	        }
	    }
	    else
	    {
	        emergencyStartTick = 0;
	    }

	    // Warning >250 cm for 10 seconds
	    if (cmValue > WATER_WARNING_THRESHOLD)
	    {
	        if (warningStartTick == 0)
	        {
	            warningStartTick = now;
	        }

	        if ((now - warningStartTick) >= WATER_WARNING_TIME_MS)
	        {
	            return WARNINGTRIGGER;
	        }
	    }
	    else
	    {
	        warningStartTick = 0;
	    }

	    return WATER_SENSOR_OK;
}

// This function resets the timers for the defined thresholds and is used when we get out of the emergency state so we don't trigger an emergency immediately when we get back into the operational state
void waterSensorResetThresholdTimers(uint32_t now)
{
    emergencyStartTick = now;
    warningStartTick = now;
}


