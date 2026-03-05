#ifndef _AUTHENTICATOR_H_
#define _AUTHENTICATOR_H_

#include <stdint.h>
#include <stdbool.h>

#define APP_SIGNATURE_ADDR 0x08010000 // address of signature segment
#define APP_HANDLER_ADDR 0x08010204 // vector table address + 4 for handler (its the second word in the vector table right after the initial stack pointer)

#define AUTH_KEY "VP2026"
#define MAX_KEY_LEN             8

#define AUTH_STATE_BOOTUP		1
#define AUTH_STATE_PREPARE_APP	2
#define AUTH_STATE_START_APP	3
#define AUTH_STATE_FAILURE		4

#define CHECK_FOR_A			0
#define WAIT_FOR_KEY		1
#define DECRYPT_KEY			2

typedef void (*StartHandler_t)(void);

typedef struct _Auth {
	uint32_t authenticatorState;
	uint32_t prepareAppSubState;
	bool gotValideInitChar;

//	static uint8_t  g_keyBuf[MAX_KEY_LEN + 1]; // +1 für \0
//	static uint32_t g_keyLen = 0;
//
//	static uint32_t g_initStartTick = 0;
//	static uint32_t g_keyStartTick  = 0;
//	static uint32_t g_lastFlashTick = 0;

} Auth;

extern uint8_t _sloadauth;
extern uint8_t _sauth;
extern uint8_t _eauth;

void authInitialize(Auth* pAuth);
void copyAuthToRam(void);
void decryptAuth(void);
void verify(void) __attribute__ ((section (".auth")));

#endif
