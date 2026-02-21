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


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/
#define STATE_BOOTUP		1
#define STATE_PREPARE_APP	2
#define STATE_START_APP		3
#define STATE_FAILURE		4

#define WAIT_FOR_KEY		0
#define VALID_KEY			1
#define DECRYPT_KEY			2

#define INIT_CHAR_NO		0
#define INIT_CHAR_YES		1

#define TIMEOUT				0
#define TIMEOUT_10S_MS      10000 // ms
#define TIMEOUT_30S_MS      30000 // ms
#define FLASH_PERIOD_MS     250   // toggle every 250ms

static uint32_t authenticatorState = STATE_BOOTUP;
static uint32_t prepareAppSubState = WAIT_FOR_KEY;
static uint8_t gotValideInitChar = 0;

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/



/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/
static int32_t initializePeripherals();
static int32_t prepareApp();
static int32_t checkForInitChar();

/**
 * @brief Main function of System
 */
int main(void)
{
	while(1)
	{
	// Pseudocode for states
	// make function for each
	// keine lokalen static valiables sondern globale
		switch (authenticatorState)
		{
			case STATE_BOOTUP:
			{
				// Initialize the HAL
				HAL_Init();

				SystemClock_Config();

				// Initialize Peripherals
				initializePeripherals();

				authenticatorState = STATE_PREPARE_APP;
			}
			break;

			case STATE_PREPARE_APP:
			{
				int newState = prepareApp();
				if(newState != STATE_PREPARE_APP)
				{
					authenticatorState = newState;
				}
				/*// Read A
				if (gotValideInitChar == 0)
				{
					int32_t gotChar = checkForInitChar();
					if (gotChar == INIT_CHAR_YES)
					{
						gotValideInitChar = 1;
					}
				}else
				{
					// Read Key
				}*/
					/*if(timeout) {
						authenticatorState = STATE_FAILURE;
					}
				}

				if (key is valid)
				{
					// Copy Section
					// Decrypt
					if (copy & decrypt == true)
					{
						authenticatorState = STATE_START_APP;
					}
				}*/


			}
			break;

			case STATE_START_APP:
			{

			}
			break;

			case STATE_FAILURE:
			{
				ledSetLED(LED4, LED_ON);
			}
			break;


		}
	}
}

static int32_t prepareApp(void)
{
    // needed because gotKey must survive into DECRYPT_KEY
    static int32_t storedKey = 0;

    switch (prepareAppSubState)
    {
        case WAIT_FOR_KEY:
        {
            static uint32_t startTick = 0u;
            static uint32_t lastFlashTick = 0u;

            uint32_t now = HAL_GetTick();

            // start timing once when we enter WAIT_FOR_KEY
            if (startTick == 0u)
            {
                startTick = now;
                lastFlashTick = now;
                ledSetLED(LED1, GPIO_PIN_RESET); // OFF initially
            }

            uint32_t elapsed = now - startTick;

            // LED1 mapping while waiting
            if (elapsed < TIMEOUT_10S_MS)
            {
                ledSetLED(LED1, GPIO_PIN_RESET); // OFF
            }
            else if (elapsed < TIMEOUT_30S_MS)
            {
                ledSetLED(LED1, GPIO_PIN_SET);   // ON
            }
            else
            {
                // FLASHING after 30s
                if ((now - lastFlashTick) >= FLASH_PERIOD_MS)
                {
                    ledToggleLED(LED1);
                    lastFlashTick = now;
                }
            }

            // check init char
            if (gotValideInitChar == 0)
            {
                int32_t gotChar = checkForInitChar();
                if (gotChar == INIT_CHAR_YES)
                {
                    gotValideInitChar = 1;

                    // turn LED1 OFF before leaving WAIT_FOR_KEY
                    ledSetLED(LED1, GPIO_PIN_RESET);

                    // reset timing
                    startTick = 0u;
                    lastFlashTick = 0u;

                    prepareAppSubState = VALID_KEY;
                }
            }

            break;
        }

        case VALID_KEY:
        {
            int32_t gotKey = checkForKey();

            if (gotKey == TIMEOUT)
            {
                ledSetLED(LED1, GPIO_PIN_RESET); // safety: ensure OFF
                return STATE_FAILURE;
            }

            storedKey = gotKey;
            prepareAppSubState = DECRYPT_KEY;
            break;
        }

        case DECRYPT_KEY:
        {
            uint32_t decryptedKey = decryptKey(storedKey);
            (void)decryptedKey;

            ledSetLED(LED1, GPIO_PIN_RESET); // safety: ensure OFF
            return STATE_START_APP;
        }

        default:
            break;
    }

    return STATE_PREPARE_APP;
}

static int32_t checkForInitChar(void)
{
    int8_t hasChar = 0;
    uartHasData(&hasChar);

    if (hasChar == 1)
    {
        uint8_t pBuffer[2];
        uint32_t receiveOK = uartReceiveData(pBuffer, 1);

        if (receiveOK == UART_ERR_OK && pBuffer[0] == 'A')
            return INIT_CHAR_YES;
    }

    return INIT_CHAR_NO;
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
