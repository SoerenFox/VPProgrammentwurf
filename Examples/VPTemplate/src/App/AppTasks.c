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

#define PERCENTTOLERANCE 30 // Tolerance for filtered gasSensorsValues

#define FLASH_PERIOD_MS 250

/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
static uint32_t gCycleCounter = 0;
static uint32_t lastFlashTick = 0;

/***** PUBLIC FUNCTIONS ******************************************************/


void taskApp10ms()
{
	uint32_t now = HAL_GetTick();
	switch (applicationGetCurrentState())
	    {
	        case APP_STATE_PREOPERATIONAL:
	        	ledToggleLED(LED1);
	        	if (debounceButton(&gButtonSW1, buttonGetButtonStatus(BTN_SW1), HAL_GetTick()))
	        		{
	        			if (gButtonSW1.stableState)
	        			{
	        				applicationSendEvent(APP_EVT_SWITCH_STATE);
	        			}
	        		}
	            break;

	        case APP_STATE_OPERATIONAL:
	        	ledToggleLED(LED2);
	        	if (debounceButton(&gButtonSW1, buttonGetButtonStatus(BTN_SW1), HAL_GetTick()))
	        		{
	        			if (gButtonSW1.stableState)
	        			{
	        				applicationSendEvent(APP_EVT_SWITCH_STATE);
	        			}
	        		}

	        	int32_t gasValue1 = gasSensorReadPpmValue(&gGasSensor1, ADC_INPUT0);
				int32_t gasValue2 = gasSensorReadPpmValue(&gGasSensor2, ADC_INPUT1);

				if (checkForValideADC(gasValue1, gasValue2))
				{
					applicationSendEvent(APP_EVT_ERROR);
					return;
				}

				int32_t pot1_filtered = filterEMA(&gEmaPot1, gasValue1);
				int32_t pot2_filtered = filterEMA(&gEmaPot2, gasValue2);

				if (isGasSensorMismatch(pot1_filtered, pot2_filtered, PERCENTTOLERANCE))
				{
					applicationSendEvent(APP_EVT_SENSOR_DEFECT);
				}

				int32_t aboveValue = gasSensorOverPpmValue(pot1_filtered, pot2_filtered, now);

				if (aboveValue == 2)
				{
					applicationSendEvent(APP_EVT_TRIGGER_EMERGENCY);
				}
				else if (aboveValue == 1)
				{
					ledSetLED(LED1, LED_ON);
				}
				else ledSetLED(LED1, LED_OFF);

				// WaterSensor missing

	            break;

	        case APP_STATE_EMERGENCY:

	        	if ((now - lastFlashTick) >= FLASH_PERIOD_MS)
				{
					ledToggleLED(LED1);
					lastFlashTick  = now;
	        	}

	        	if (debounceButton(&gButtonB1, buttonGetButtonStatus(BTN_B1), HAL_GetTick()))
	        		{
	        			if (gButtonB1.stableState)
	        			{
	        				applicationSendEvent(APP_EVT_ALARM_RESET);
	        			}
	        		}
	            break;

	        case APP_STATE_INITIALIZATION:
	        case APP_STATE_FAILURE:
	        default:
	            break;
	    }

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
