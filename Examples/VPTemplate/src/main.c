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
#include "stm32g4xx_hal.h"
#include "System.h"

#include "HardwareConfig.h"

#include "Util/Global.h"
#include "Util/Log/printf.h"
#include "Util/Log/LogOutput.h"

#include "UARTModule.h"
#include "ButtonModule.h"
#include "LEDModule.h"
#include "DisplayModule.h"
#include "ADCModule.h"
#include "TimerModule.h"
#include "Scheduler.h"

#include "GlobalObjects.h"

#include "GasSensor.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/
static int32_t initializePeripherals();


/***** PRIVATE VARIABLES *****************************************************/
static Scheduler gScheduler;            // Global Scheduler instance

static GasSensor gGasSensor1;
static GasSensor gGasSensor2;


/***** PUBLIC FUNCTIONS ******************************************************/


/**
 * @brief Main function of System
 */
int main(void)
{
	// Initialize the HAL
	    HAL_Init();

	    // Initialize the System Clock
	    SystemClock_Config();

	    // Initialize Peripherals
	    initializePeripherals();

	    // Initialize Scheduler
	    schedInitialize(&gScheduler);

    gasSensorInitialize(&gGasSensor1, 204);
    gasSensorInitialize(&gGasSensor2, 204);

    while (1)
    {
    	int adcValue = adcReadChannel(ADC_INPUT0);
    	gasSensorSetSensorVoltage(&gGasSensor1, adcValue);
    	int32_t gasValue1 = gasSensorGetSensorValue(&gGasSensor1);
    	outputLogf("Gas Sensor 1: %d\n", gasValue1);

    	adcValue = adcReadChannel(ADC_INPUT0);
    	gasSensorSetSensorVoltage(&gGasSensor1, adcValue);
    	int32_t gasValue2 = gasSensorGetSensorValue(&gGasSensor1);
    	outputLogf("Gas Sensor 2: %d\n", gasValue2);

    	HAL_Delay(100);
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
