#ifndef _AUTHENTICATOR_H_
#define _AUTHENTICATOR_H_

#include <stdint.h>
#include <stdbool.h>

#define AUTH_OK              0
#define AUTH_INVALID_PTR    -1

typedef enum {
    AUTH_STATE_BOOTUP = 1,
    AUTH_STATE_PREPARE_APP,
    AUTH_STATE_START_APP,
    AUTH_STATE_FAILURE
} AuthState;

typedef enum {
    AUTH_SUB_CHECK_FOR_A = 0,
    AUTH_SUB_WAIT_FOR_KEY,
    AUTH_SUB_DECRYPT_KEY
} AuthPrepareSubState;

#define MAX_KEY_LEN             8

#define APP_SIGNATURE_ADDR 0x08010000 // address of signature segment
#define APP_HANDLER_ADDR 0x08010204 // vector table address + 4 for handler (its the second word in the vector table right after the initial stack pointer)

#define TIMEOUT_INIT_15S_MS     15000u
#define KEY_STAGE1_10S_MS       10000u
#define KEY_STAGE2_30S_MS       30000u
#define KEY_FAIL_45S_MS         45000u
#define FLASH_PERIOD_MS         250u

typedef void (*StartHandler)(void);

extern uint8_t _sloadauth;
extern uint8_t _sauth;
extern uint8_t _eauth;

typedef struct {
    AuthState state;
    AuthPrepareSubState subState;

    uint32_t initStartTick;
    uint32_t keyStartTick;
    uint32_t lastFlashTick;

    uint8_t  keyBuf[MAX_KEY_LEN + 1u];
    uint32_t keyLen;
} Auth;

int32_t authInitialize(Auth* pAuth);
int32_t authPrepareApp(Auth* pAuth);

void verify(void) __attribute__ ((section (".auth")));

#endif
