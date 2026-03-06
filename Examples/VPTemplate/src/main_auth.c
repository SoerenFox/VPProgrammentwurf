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
 * @brief Main file for the VP Template Authenticator project
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "stm32g4xx_hal.h"
#include "System.h"
#include <stdbool.h>
#include <string.h>

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
#include "Auth/Authenticator.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
Auth gAuth;

/***** PUBLIC FUNCTIONS ******************************************************/
static int32_t initializePeripherals();

/**
 * @brief Main function of System
 */
int main(void) {
    if (authInitialize(&gAuth) != AUTH_OK) gAuth.state = AUTH_STATE_FAILURE;

	while(1) {
	// Pseudocode for states
	// make function for each
	// keine lokalen static valiables sondern globale
		switch (gAuth.state) {
			case AUTH_STATE_BOOTUP:
				__HAL_RCC_AHB1_FORCE_RESET();
				__HAL_RCC_AHB1_RELEASE_RESET();
				// Initialize the HAL
				HAL_Init();

				SystemClock_Config();

				// Initialize Peripherals
				initializePeripherals();

				ledSetLED(LED0, LED_ON);
				gAuth.state = AUTH_STATE_PREPARE_APP;
				break;

			case AUTH_STATE_PREPARE_APP:
				if (authPrepareApp(&gAuth) != AUTH_OK) gAuth.state = AUTH_STATE_FAILURE;
				break;

			case AUTH_STATE_START_APP:
				ledSetLED(LED0, LED_OFF);
				verify();
				break;

			case AUTH_STATE_FAILURE:
				ledSetLED(LED4, LED_ON);
				break;
		}
	}
}

/***** PRIVATE FUNCTIONS ******************************************A***********/

/**
 * @brief Initializes the used peripherals like GPIO,
 * ADC, DMA and Timer Interrupts
 *
 * @return Returns ERROR_OK if no error occurred
 */
static int32_t initializePeripherals() {
    // Initialize UART used for Debug-Outputs
    uartInitialize(115200);

    // Initialize GPIOs for LED and 7-Segment output
	ledInitialize();

    return ERROR_OK;
}
