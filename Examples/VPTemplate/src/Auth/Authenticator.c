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
