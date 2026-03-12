/******************************************************************************
 * @file main.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Main file for the VP Template project
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "System.h"

#include "HardwareConfig.h"
#include "TimerModule.h"
#include "Scheduler.h"

#include "AppTasks.h"
#include "Application.h"



/***** PRIVATE CONSTANTS *****************************************************/
const char signature[] __attribute__ ((section (".signature"))) = "UMMS";

/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
Scheduler gScheduler;            // Global Scheduler instance


/***** PUBLIC FUNCTIONS ******************************************************/

/**
 * @brief Main function of System
 */
int main(void)
{
	__HAL_RCC_AHB1_FORCE_RESET();
	__HAL_RCC_AHB1_RELEASE_RESET();

	// Initialize the HAL
	 HAL_Init();

	// Initialize the State Machine (needed to be able to initialize everything in the given state)
	 applicationInit(); // Sets Initailization State

	 // Initialize Scheduler
	 schedInitialize(&gScheduler);
	 gScheduler.pGetHALTick = HAL_GetTick;
	 // Define the cyclic tasks for the Scheduler
	 gScheduler.pTask_10ms = taskApp10ms;
	 gScheduler.pTask_50ms = taskApp50ms;
	 gScheduler.pTask_250ms = taskApp250ms;

	 applicationRunCyclic(); // Activates Initailization State

	 while(1)
	 {
		 schedCycle(&gScheduler);
	 }
}