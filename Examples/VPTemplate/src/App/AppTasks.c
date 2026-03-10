/******************************************************************************
 * @file AppTasks.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Implementation File for the application tasks
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "Application.h"
#include "AppTasks.h"
#include "Util/StackMonitor/StackMonitor.h"




/***** PRIVATE CONSTANTS *****************************************************/

#define DUAL_GAS_TOL_PERCENT      10

#define PERCENTTOLERANCE 50 // Tolerance for filtered gasSensorsValues

#define FLASH_PERIOD_MS 	250
#define MAX_DISPLAY_VALUE 	999
#define DISPLAYFACTOR		10

/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
static uint32_t gCycleCounter = 0;

/***** PUBLIC FUNCTIONS ******************************************************/


void taskApp10ms()
{
	if ((applicationGetCurrentState() != APP_STATE_OPERATIONAL))
	{
		if (gCycleCounter % 2 == 0)
			{
				displayShowDigit(LEFT_DISPLAY, 16);
			}
			else
			{
				displayShowDigit(RIGHT_DISPLAY, 16);
			}

			gCycleCounter++;
	} else
	{
		// GasSensors
		int32_t gasValue1 = gasSensorReadPpmValue(&gGasSensor1, ADC_INPUT0);
		int32_t gasValue2 = gasSensorReadPpmValue(&gGasSensor2, ADC_INPUT1);

		if (checkForValideADC(gasValue1, gasValue2))
		{
			applicationSendEvent(APP_EVT_ERROR);
		}

		int32_t pot1_filtered = filterEMA(&gEmaPot1, gasValue1);
		int32_t pot2_filtered = filterEMA(&gEmaPot2, gasValue2);

		if (isGasSensorMismatch(pot1_filtered, pot2_filtered, PERCENTTOLERANCE))
		{
			applicationSendEvent(APP_EVT_SENSOR_DEFECT);
		}

		int32_t errorValue = gasSensorOverPpmValue(pot1_filtered, pot2_filtered);

		if (errorValue == 2)
		{
			applicationSendEvent(APP_EVT_TRIGGER_EMERGENCY);
		}
		else if (errorValue == 1)
		{
			ledSetLED(LED1, LED_ON);
		}
		else ledSetLED(LED1, LED_OFF);


		radioConnectBufferToStruct(&gRadioConnect);

		// WaterSensor
		uint32_t cmValue = gRadioConnect.sensorValue;
		if (wasSensorCheckValue(cmValue))
		{
			applicationSendEvent(APP_EVT_ERROR);
		}

		errorValue = waterSensorOverCmValue(cmValue);

		if (errorValue == 3)
		{
			applicationSendEvent(APP_EVT_ERROR);
		}
		else if (errorValue == 2)
		{
			applicationSendEvent(APP_EVT_TRIGGER_EMERGENCY);
		}
		else if (errorValue == 1)
		{
			ledSetLED(LED1, LED_ON);
		}
		else ledSetLED(LED1, LED_OFF);

		if (gCycleCounter % 2 == 0)
		{
			if (cmValue > MAX_DISPLAY_VALUE) cmValue = MAX_DISPLAY_VALUE;
			displayShowDigit(LEFT_DISPLAY, (cmValue/(DISPLAYFACTOR * DISPLAYFACTOR)));
		}
		else
		{

			displayShowDigit(RIGHT_DISPLAY, (int32_t)(cmValue/DISPLAYFACTOR) % DISPLAYFACTOR);
		}
		gCycleCounter++;
	}

	if (debounceButton(&gButtonSW1, buttonGetButtonStatus(BTN_SW1)))
	{
		if (gButtonSW1.stableState)
		{
			applicationSendEvent(APP_EVT_SWITCH_STATE);
		}
	}

	if (debounceButton(&gButtonB1, buttonGetButtonStatus(BTN_B1)))
	{
		if (gButtonB1.stableState)
		{
			applicationSendEvent(APP_EVT_ALARM_RESET);
		}
	}

}

void taskApp50ms()
{
	applicationRunCyclic();
}

void taskApp250ms()
{
	if (isCorrupted())
	{
		applicationSendEvent(APP_EVT_STACK_CORRUPTION);
	}
}


/***** PRIVATE FUNCTIONS *****************************************************/
