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

#include "Util/Global.h"
#include "Util/Log/printf.h"
#include "Util/Log/LogOutput.h"
#include "Util/Filter/Filter.h"

#include "UARTModule.h"
#include "ButtonModule.h"
#include "LEDModule.h"
#include "DisplayModule.h"
#include "ADCModule.h"
#include "TimerModule.h"
#include "Scheduler.h"

#include "GlobalObjects.h"

#include "GasSensor.h"

#include "AppTasks.h"


/***** PRIVATE CONSTANTS *****************************************************/
const char signature[] __attribute__ ((section (".signature"))) = "UMMS";

/***** PRIVATE MACROS ********************************************************/
#define STATE_INITIALIZATION     0
#define STATE_PRE_OPERATIONAL    1
#define STATE_OPERATIONAL        2
#define STATE_EMERGENCY          3
#define STATE_FAILURE            4

#define INIT_CHAR_NO	0
#define INIT_CHAR_YES	1


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
static Scheduler gScheduler;            // Global Scheduler instance


/***** PUBLIC FUNCTIONS ******************************************************/
static int32_t initializePeripherals();

/**
 * @brief Main function of System
 */
int main(void)
{
	__HAL_RCC_AHB1_FORCE_RESET();
	__HAL_RCC_AHB1_RELEASE_RESET();
	// Initialize the HAL
	 HAL_Init();

	 // Initialize the System Clock
	 SystemClock_Config();

	 // Initialize Peripherals
	 initializePeripherals();
	 __enable_irq();

	 // Initialize Scheduler
	 schedInitialize(&gScheduler);

	 appTasksInit();
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

    uint32_t lastRuntime = 0;

    while (1)
    {
    	uint32_t currentTime = HAL_GetTick();

    	if ((currentTime - lastRuntime) > 100) {
    		ledToggleLED(LED0);

    		lastRuntime = HAL_GetTick();
    	}
    }

}

/***** PRIVATE FUNCTIONS *****************************************************/

/**
 * @brief Initializes the used peripherals like GPIO,
 * ADC, DMA and Timer Interrupts
 *
 * @return Returns ERROR_OK if no error occurred
 */
static int32_t initializePeripherals()
{
    // Initialize UART used for Debug-Outputs
    uartInitialize(115200);

    // Initialize GPIOs for LED and 7-Segment output
	ledInitialize();
    displayInitialize();

    // Initialize GPIOs for Buttons
    buttonInitialize();

    // Initialize Timer, DMA and ADC for sensor measurements
    timerInitialize();
    adcInitialize();

    return ERROR_OK;
}
