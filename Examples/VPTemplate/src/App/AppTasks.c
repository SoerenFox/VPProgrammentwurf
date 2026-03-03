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
#include "Util/Log/printf.h"
#include "Util/Log/LogOutput.h"
#include "Scheduler.h"
#include "AppTasks.h"
#include "UARTModule.h"
#include "ButtonModule.h"
#include "LEDModule.h"
#include "DisplayModule.h"
#include "ADCModule.h"
#include "TimerModule.h"
#include "Util/Filter/Filter.h"

/***** PRIVATE CONSTANTS *****************************************************/

#define DUAL_GAS_TOL_PERCENT      10

#define EMA_SCALE   1000
#define EMA_ALPHA   500   // entspricht 0.5


/***** PRIVATE MACROS ********************************************************/
static EMAFilterData_t gEmaPot1;
static EMAFilterData_t gEmaPot2;

static uint8_t gFilterInitialized = 0;

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/


void taskApp10ms()
{

	filterInitEMA(&gEmaPot1, EMA_SCALE, EMA_ALPHA, true);
	filterInitEMA(&gEmaPot2, EMA_SCALE, EMA_ALPHA, true);

	int32_t pot1_raw = adcReadChannel(ADC_INPUT0);
	int32_t pot2_raw = adcReadChannel(ADC_INPUT1);

	int32_t pot1_filtered = filterEMA(&gEmaPot1, pot1_raw);
	outputLogf("Gas Sensor 1: %d\n\r", pot1_filtered);
	int32_t pot2_filtered = filterEMA(&gEmaPot2, pot2_raw);
	outputLogf("Gas Sensor 2: %d\n\r", pot2_filtered);

	ledToggleLED(LED1);

}

void taskApp50ms()
{
	ledToggleLED(LED2);
}

void taskApp250ms()
{
	ledToggleLED(LED3);
}


/***** PRIVATE FUNCTIONS *****************************************************/




