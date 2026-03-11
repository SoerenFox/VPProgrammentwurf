/*
 * WaterSensor.c
 *
 *  Created on: Mar 10, 2026
 *      Author: kali
 */
#include "WaterSensor.h"

static uint32_t warningStartTick 	= 0;
static uint32_t emergencyStartTick 	= 0;
static uint32_t timeoutStartTick	= 0;


int32_t wasSensorCheckValue(uint32_t cmValue) {

	if (cmValue == 0) return cmValue;

	if (cmValue < MIN_WATERSENSOR_VALUE || cmValue > MAX_WATERSENSOR_VALUE) return WATER_SENSOR_VALUE_INVALID;

	return cmValue;
}

int32_t waterSensorOverCmValue(int32_t cmValue)
{
		uint32_t now = HAL_GetTick();
//		if (cmValue == 0)
//		{
//			if (timeoutStartTick == 0)
//			{
//				timeoutStartTick = now;
//			}
//
//			if ((now - timeoutStartTick) >= WATER_EMERGENCY_TIME_MS)
//			{
//				return 3;
//			}
//		}

		/* Emergency >300 cm for 5 seconds */
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

	    /* Warning >250 cm for 10 seconds */
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

int32_t waterSensorResetThresholdTimers(uint32_t now)
{
    emergencyStartTick = now;
    warningStartTick = now;
    timeoutStartTick = now;
    return WATER_SENSOR_OK;
}


