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
#define AUTH_STATE_BOOTUP		1
#define AUTH_STATE_PREPARE_APP	2
#define AUTH_STATE_START_APP	3
#define AUTH_STATE_FAILURE		4

#define CHECK_FOR_A			0
#define WAIT_FOR_KEY		1
#define DECRYPT_KEY			2

#define INIT_CHAR_NO		0
#define INIT_CHAR_YES		1

#define TIMEOUT_INIT_15S_MS     15000	// ms

#define MAX_KEY_LEN             8
#define KEY_STAGE1_10S_MS       10000	// ms
#define KEY_STAGE2_30S_MS       30000	// ms
#define KEY_FAIL_45S_MS         45000	// ms
#define FLASH_PERIOD_MS			250 	// ms

// Return codes for checkForKey()
#define KEY_IN_PROGRESS         0
#define KEY_DONE                1
#define KEY_TIMEOUT             -1

static uint32_t authenticatorState = AUTH_STATE_BOOTUP;
static uint32_t prepareAppSubState = CHECK_FOR_A;
static bool gotValideInitChar = false;
static uint8_t  g_keyBuf[MAX_KEY_LEN + 1]; // +1 für \0
static uint32_t g_keyLen = 0;

static uint32_t g_initStartTick = 0;
static uint32_t g_keyStartTick  = 0;
static uint32_t g_lastFlashTick = 0;

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/



/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/
static int32_t initializePeripherals();
static int32_t prepareApp();
static int32_t checkForInitChar();
static int32_t checkForKey(void);
static void prepareApp_ResetKeyReception(void);

#define AUTH_KEY "VP2026"
char* key = AUTH_KEY;

/**
 * @brief Main function of System
 */
int main(void) {
	while(1) {
	// Pseudocode for states
	// make function for each
	// keine lokalen static valiables sondern globale
		switch (authenticatorState) {
			case AUTH_STATE_BOOTUP:
				__HAL_RCC_AHB1_FORCE_RESET();
				__HAL_RCC_AHB1_RELEASE_RESET();
				// Initialize the HAL
				HAL_Init();

				SystemClock_Config();

				// Initialize Peripherals
				initializePeripherals();

				ledSetLED(LED0, LED_ON);
				authenticatorState = AUTH_STATE_PREPARE_APP;
				break;

			case AUTH_STATE_PREPARE_APP:
				int newState = prepareApp();
				if(newState != AUTH_STATE_PREPARE_APP)
				{
					authenticatorState = newState;
				}
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

static int32_t prepareApp(void) {
    switch (prepareAppSubState) {
        // CHECK_FOR_A
    	// Waiting for character 'A' (15s) long
        case CHECK_FOR_A:
            uint32_t now = HAL_GetTick();

            // Startzeit einmalig setzen beim Eintritt
            if (g_initStartTick == 0u) {
                g_initStartTick = now;
                ledSetLED(LED1, LED_OFF); // D1 sicher aus
            }

            // Timeout 15s: wenn 'A' nicht kommt -> Failure
            if ((now - g_initStartTick) >= TIMEOUT_INIT_15S_MS) {
                ledSetLED(LED1, LED_OFF);
                return AUTH_STATE_FAILURE;
            }

            // 'A' prüfen (nicht-blockierend)
            if (checkForInitChar() == INIT_CHAR_YES) {
                gotValideInitChar = true;

                // Wechsel zur Key-Receive-Phase
                prepareApp_ResetKeyReception();
                prepareAppSubState = WAIT_FOR_KEY;
            }
            break;

        // WAIT_FOR_KEY
        // Waiting for Key with LEDs after (10/30/45s) and Timeout
        case WAIT_FOR_KEY:
            int32_t keyStatus = checkForKey();

            if (keyStatus == KEY_DONE) {
                // Key komplett empfangen -> weiter
                ledSetLED(LED1, LED_OFF); // D1 aus bevor wir weitergehen
                prepareAppSubState = DECRYPT_KEY;
            }
            else if (keyStatus == KEY_TIMEOUT) {
                ledSetLED(LED1, LED_OFF);
                return AUTH_STATE_FAILURE;
            } else {
                // KEY_IN_PROGRESS -> bleib hier
            }
            break;

        case DECRYPT_KEY:
        	copyAuthToRam();
        	decryptAuth();
            ledSetLED(LED1, LED_OFF);
            return AUTH_STATE_START_APP;

        default:
            break;
    }

    return AUTH_STATE_PREPARE_APP;
}

static void prepareApp_ResetKeyReception(void) {
    g_keyStartTick  = HAL_GetTick();
    g_lastFlashTick = g_keyStartTick;
    g_keyLen = 0u;

    for (uint32_t i = 0; i < (MAX_KEY_LEN + 1u); i++) {
        g_keyBuf[i] = 0u;
    }

    // LED D1 initial aus (wird nach 10s an / nach 30s blinkend)
    ledSetLED(LED1, LED_OFF);

    // init-wait timing nicht mehr nötig
    g_initStartTick = 0u;
}

static int32_t checkForKey(void) {
    uint32_t now = HAL_GetTick();
    uint32_t elapsed = now - g_keyStartTick;

    // 45s -> Failure
    if (elapsed >= KEY_FAIL_45S_MS) {
        return KEY_TIMEOUT;
    }

    // LED-Stufen: 0..10s OFF, 10..30s ON, ab 30s BLINK
    if (elapsed < KEY_STAGE1_10S_MS) {
        ledSetLED(LED1, LED_OFF);
    }
    else if (elapsed < KEY_STAGE2_30S_MS) {
        ledSetLED(LED1, LED_ON);
    } else {
    	if ((now - g_lastFlashTick) >= FLASH_PERIOD_MS) {
    		ledToggleLED(LED1);
    		g_lastFlashTick = now;
    	}
    }

    static char buffer[MAX_KEY_LEN] = "";
	int len = strlen(buffer);

	int8_t hasChar = 0;
	uartHasData(&hasChar);

	if (hasChar) {
		uint8_t ch = 0;
		uint32_t receiveOK = uartReceiveData(&ch, 1);

		if (receiveOK == UART_ERR_OK) {
			if (ch == '\n' && strcmp(buffer, key) == 0) {
				return KEY_DONE;
			}
			else if (ch == '\r') {
				return KEY_IN_PROGRESS;
			}
			else if (len < MAX_KEY_LEN) {
				 buffer[len] = ch;
				 buffer[len + 1] = '\0';
			}
		}

	}
	return KEY_IN_PROGRESS;
}

static int32_t checkForInitChar(void) {
    int8_t hasChar = 0;
    uartHasData(&hasChar);

    if (hasChar) {
        uint8_t ch = 0;
        uint32_t receiveOK = uartReceiveData(&ch, 1);

        // Checking for Input 'A' in Uart-Buffer
        if (receiveOK == UART_ERR_OK && ch == (uint8_t)'A') {
        	// Checking for other Inputs after 'A'
        	uartHasData(&hasChar);
        	if (hasChar == 1) {
        		// Other Input after 'A' in Uart-Buffer
        		return INIT_CHAR_NO;
        	}
        	// No other Input after 'A' in Uart-Buffer
        	return INIT_CHAR_YES;
        }
    }
    // No Input 'A' in Uart-Buffer
    return INIT_CHAR_NO;
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
