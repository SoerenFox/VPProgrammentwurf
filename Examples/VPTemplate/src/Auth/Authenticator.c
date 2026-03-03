#include "Authenticator.h"

#include "stm32g4xx.h"

/**
 * @brief Copies .auth section from Flash to RAM.
 */
void copyAuthToRam(void) {
	uint8_t* src = &_sloadauth;
	uint8_t* dst = &_sauth;

	while (dst < &_eauth)
		*dst++ = *src++;
}

/**
 * @brief Runs same XOR encrypting logic of provided python code again.
 */
void decryptAuthInRam(void)
{
    uint8_t* ptr = &_sauth;
    uint32_t length = (uint32_t)(&_eauth - &_sauth);

    const uint8_t key[] = AUTH_KEY;
    const uint32_t keyLen = sizeof(key) - 1; // exclude null terminator

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

		StartHandler_t startApp = (StartHandler_t)*(uint32_t*)(APP_HANDLER_ADDR);

		startApp();
	}
	while(1) {}
}
