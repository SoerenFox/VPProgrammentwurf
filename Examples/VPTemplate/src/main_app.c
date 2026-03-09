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

	 applicationInit();

	 // Initialize Scheduler
	 schedInitialize(&gScheduler);
	 gScheduler.pGetHALTick = HAL_GetTick;
	 gScheduler.pTask_10ms = taskApp10ms;
	 gScheduler.pTask_50ms = taskApp50ms;
	 gScheduler.pTask_250ms = taskApp250ms;


	 while(1)
	 {
		 schedCycle(&gScheduler);
	 }


	//while (1)
    //
    //	int adcValue = adcReadChannel(ADC_INPUT0);
    //	gasSensorSetSensorVoltage(&gGasSensor1, adcValue);
    //	int32_t gasValue1 = gasSensorGetSensorValue(&gGasSensor1);
    //	outputLogf("Gas Sensor 1: %d\n\r", gasValue1);
    //
    //	adcValue = adcReadChannel(ADC_INPUT1);
    //	gasSensorSetSensorVoltage(&gGasSensor2, adcValue);
    //	int32_t gasValue2 = gasSensorGetSensorValue(&gGasSensor2);
    //	outputLogf("Gas Sensor 2: %d\n\r", gasValue2);
    //
    //	HAL_Delay(100);
    //

    //uint32_t lastRuntime = 0;

// while (1)
// {
// 	uint32_t currentTime = HAL_GetTick();
//
// 	if ((currentTime - lastRuntime) > 100) {
// 		ledToggleLED(LED0);
//
// 		lastRuntime = HAL_GetTick();
// 	}
// }
//
}

/***** PRIVATE FUNCTIONS *****************************************************/

