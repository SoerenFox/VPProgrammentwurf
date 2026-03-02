#include "Authenticator.h"

#include "stm32g4xx.h"

void verify(void) {

	const char* sig = (const char*)APP_SIGNATURE_ADDR;

	if (sig[0] == 'U' && sig[1] == 'M' && sig[2] == 'M' && sig[3] == 'S') {
		__disable_irq();

	    uint32_t *pStartApp = (uint32_t *)APP_HANDLER_ADDR;
	    StartHandler_t start = (StartHandler_t) *(pStartApp);
	    start();
	}
	while(1) {}
}
