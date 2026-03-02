#ifndef _AUTHENTICATOR_H_
#define _AUTHENTICATOR_H_

#include <stdint.h>

#define APP_SIGNATURE_ADDR 0x08010000 // address of signature segment
#define APP_HANDLER_ADDR 0x08010204 // vector table adress + 4 for handler

typedef void (*StartHandler_t)(void);

void verify(void) __attribute__ ((section (".auth")));

#endif
