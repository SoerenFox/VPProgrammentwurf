#include "Authenticator.h"

#include <string.h>

#include "UARTModule.h"
#include "LEDModule.h"

#define KEY_IN_PROGRESS  0
#define KEY_DONE         1
#define KEY_TIMEOUT     -1

#define INIT_CHAR_NO 0
#define INIT_CHAR_YES 1

static void ResetKeyReception(Auth* pAuth);
static int32_t checkForInitChar(Auth* pAuth);
static int32_t checkForKey(Auth* pAuth);

static void copyAuthToRam(void);
static void decryptAuth(uint8_t key[], uint32_t keyLen);

int32_t authInitialize(Auth* pAuth) {
    if (!pAuth) return AUTH_INVALID_PTR;

    memset(pAuth, 0, sizeof(*pAuth)); // set all attributes to 0
    pAuth->state = AUTH_STATE_BOOTUP;
    pAuth->subState = AUTH_SUB_CHECK_FOR_A;

    return AUTH_OK;
}

int32_t authPrepareApp(Auth* pAuth) {
    if (!pAuth) return AUTH_INVALID_PTR;

    switch (pAuth->subState) {
        // CHECK_FOR_A
    	// Waiting for character 'A' (15s) long
        case AUTH_SUB_CHECK_FOR_A:
            uint32_t now = HAL_GetTick();

            // Startzeit einmalig setzen beim Eintritt
            if (pAuth->initStartTick == 0u) {
                pAuth->initStartTick = now;
                ledSetLED(LED1, LED_OFF); // D1 sicher aus
            }

            // Timeout 15s: wenn 'A' nicht kommt -> Failure
            if ((now - pAuth->initStartTick) >= TIMEOUT_INIT_15S_MS) {
                ledSetLED(LED1, LED_OFF);
                pAuth->state = AUTH_STATE_FAILURE;
                break;
            }

            // 'A' prüfen (nicht-blockierend)
            if (checkForInitChar(pAuth)) { // TODO
                // Wechsel zur Key-Receive-Phase
                ResetKeyReception(pAuth); // TODO
                pAuth->subState = AUTH_SUB_WAIT_FOR_KEY;
            }
            break;

        // WAIT_FOR_KEY
        // Waiting for Key with LEDs after (10/30/45s) and Timeout
        case AUTH_SUB_WAIT_FOR_KEY:
            int32_t keyStatus = checkForKey(pAuth); // TODO

            if (keyStatus == KEY_DONE) {
                // Key komplett empfangen -> weiter
                ledSetLED(LED1, LED_OFF); // D1 aus bevor wir weitergehen
                pAuth->subState = AUTH_SUB_DECRYPT_KEY;
            }
            else if (keyStatus == KEY_TIMEOUT) {
                ledSetLED(LED1, LED_OFF);
                pAuth->state = AUTH_STATE_FAILURE;
            }
            break;

        case AUTH_SUB_DECRYPT_KEY:
        	copyAuthToRam();
        	decryptAuth(pAuth->keyBuf, pAuth->keyLen);
            ledSetLED(LED1, LED_OFF);
            pAuth->state = AUTH_STATE_START_APP;
            break;
    }
    return AUTH_OK;
}

static void ResetKeyReception(Auth* pAuth) {
    pAuth->keyStartTick  = HAL_GetTick();
    pAuth->lastFlashTick = pAuth->keyStartTick;
    pAuth->keyLen = 0u;

    for (uint32_t i = 0; i < (MAX_KEY_LEN + 1u); i++) {
        pAuth->keyBuf[i] = 0u;
    }

    // LED D1 initial aus (wird nach 10s an / nach 30s blinkend)
    ledSetLED(LED1, LED_OFF);

    // init-wait timing nicht mehr nötig
    pAuth->initStartTick = 0u;
}

static int32_t checkForInitChar(Auth* pAuth) {
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

static int32_t checkForKey(Auth* pAuth) {
    uint32_t now = HAL_GetTick();
    uint32_t elapsed = now - pAuth->keyStartTick;

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
    }
    else if ((now - pAuth->lastFlashTick) >= FLASH_PERIOD_MS) {
        ledToggleLED(LED1);
        pAuth->lastFlashTick  = now;
    }

	int8_t hasChar = 0;
	uartHasData(&hasChar);

	if (hasChar) {
		uint8_t ch = 0;
		uint32_t receiveOK = uartReceiveData(&ch, 1);

		if (receiveOK == UART_ERR_OK) {
			if (ch == '\n') {
				return KEY_DONE;
			}
			else if (ch == '\r') {
				return KEY_IN_PROGRESS;
			}
			else if (pAuth->keyLen < MAX_KEY_LEN) {
				 pAuth->keyBuf[pAuth->keyLen++] = ch;
				 pAuth->keyBuf[pAuth->keyLen] = '\0';
			}
		}
	}
	return KEY_IN_PROGRESS;
}


/**
 * @brief Copies .auth section from Flash to RAM.
 */
static void copyAuthToRam(void) {
	uint8_t* src = &_sloadauth;
	uint8_t* dst = &_sauth;

	while (dst < &_eauth)
		*dst++ = *src++;
}

/**
 * @brief Runs same XOR encrypting logic of provided python code again.
 */
static void decryptAuth(uint8_t key[], uint32_t keyLen) {
    uint8_t* ptr = &_sauth;
    uint32_t length = (uint32_t)(&_eauth - &_sauth);

    for (uint32_t i = 0; i < length; i++)
    {
        ptr[i] ^= key[i % keyLen];
    }
}

/**
 * @brief Checks signature bytes and if correct, it calls the StartHandler() function of the Application.
 */
void verify(void) {
	const char* sig = (const char*)APP_SIGNATURE_ADDR;

	if (sig[0] == 'U' && sig[1] == 'M' && sig[2] == 'M' && sig[3] == 'S') {
		__disable_irq();

		StartHandler startApp = (StartHandler)*(uint32_t*)(APP_HANDLER_ADDR);

		startApp();
	}
	while(1) {}
}
