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
#include "GasSensor.h"
#include "Util/Filter/Filter.h"

/***** PRIVATE CONSTANTS *****************************************************/

#define DUAL_GAS_TOL_PERCENT      10
#define EMA_SCALE   1000
#define EMA_ALPHA   500   // 0.5

/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
static EMAFilterData_t gEmaPot1;
static EMAFilterData_t gEmaPot2;

static GasSensor gGasSensor1;
static GasSensor gGasSensor2;

/***** PUBLIC FUNCTIONS ******************************************************/

void appTasksInit(void)
{
    filterInitEMA(&gEmaPot1, EMA_SCALE, EMA_ALPHA, true);
    filterInitEMA(&gEmaPot2, EMA_SCALE, EMA_ALPHA, true);

    gasSensorInitialize(&gGasSensor1, 204);
    gasSensorInitialize(&gGasSensor2, 204);
}


void taskApp10ms()
{
	int32_t pot1_raw = adcReadChannel(ADC_INPUT0);
	int32_t pot2_raw = adcReadChannel(ADC_INPUT1);
	int32_t gasValue1 = gasSensorSetSensorVoltage(&gGasSensor1, pot1_raw);
	int32_t gasValue2 = gasSensorSetSensorVoltage(&gGasSensor2, pot2_raw);

	int32_t pot1_filtered = filterEMA(&gEmaPot1, gasValue1);
	outputLogf("Gas Sensor 1: %d\n\r", pot1_filtered);
	int32_t pot2_filtered = filterEMA(&gEmaPot2, gasValue2);
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




