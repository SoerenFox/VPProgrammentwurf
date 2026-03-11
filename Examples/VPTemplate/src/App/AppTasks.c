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
#include "AppTasks.h"

/***** PRIVATE CONSTANTS *****************************************************/
#define FLASH_PERIOD_MS 	250
#define MAX_DISPLAY_VALUE 	999
#define DISPLAYFACTOR		10
#define DISPLAYSWITCH		2
#define DASH				16

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
		displayDashDash(gCycleCounter);
		gCycleCounter++;
	} else
	{
		// GasSensor
		gasSensorHandler(&gGasSensor1, &gGasSensor2, &gEmaPot1, &gEmaPot2);

		if (radioConnectBufferToStruct(&gRadioConnect) == (CONNECT_INVALID_PTR || CONNECT_SENSOR_DEFECT))
		{
			applicationSendEvent(APP_EVT_SENSOR_DEFECT);
		}

		// WaterSensor
		waterSensorHandler(gCycleCounter);
		gCycleCounter++;
	}

	// Buttons
	if (buttonHandler(&gButtonSW1, &gButtonB1) == BUTTON_ERR)
	{
		applicationSendEvent(APP_EVT_SENSOR_DEFECT);
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
