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



/***** PRIVATE CONSTANTS *****************************************************/

#define DUAL_GAS_TOL_PERCENT      10

#define PERCENTTOLERANCE 30 // Tolerance for filtered gasSensorsValues

/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/


void taskApp10ms()
{
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

	if (debounceButton(&gButtonSW1, buttonGetButtonStatus(BTN_SW1), HAL_GetTick()))
	{
		if (gButtonSW1.stableState)
		{
			applicationSendEvent(APP_EVT_SWITCH_STATE);
		}
	}

	if (debounceButton(&gButtonB1, buttonGetButtonStatus(BTN_B1), HAL_GetTick()))
	{
		if (gButtonB1.stableState)
		{
			applicationSendEvent(APP_EVT_ALARM_RESET);
		}
	}

	ledToggleLED(LED1);

}

void taskApp50ms()
{
	applicationRunCyclic();

	ledToggleLED(LED2);
}

void taskApp250ms()
{
	ledToggleLED(LED3);
}


/***** PRIVATE FUNCTIONS *****************************************************/
