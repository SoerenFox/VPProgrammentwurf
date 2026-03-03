#ifndef _AUTHENTICATOR_H_
#define _AUTHENTICATOR_H_

#include <stdint.h>

#define AUTH_KEY "VP2026"
#define APP_SIGNATURE_ADDR 0x08010000 // address of signature segment
#define APP_HANDLER_ADDR 0x08010204 // vector table address + 4 for handler (its the second word in the vector table right after the initial stack pointer)

typedef void (*StartHandler_t)(void);

extern uint8_t _sloadauth;
extern uint8_t _sauth;
extern uint8_t _eauth;

void copyAuthToRam(void);
void decryptAuthInRam(void);
void verify(void) __attribute__ ((section (".auth")));

#endif
