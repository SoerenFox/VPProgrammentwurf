/******************************************************************************
 * @file Application.h
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Header file for main application (state machine)
 *
 *
 *****************************************************************************/
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

/***** INCLUDES **************************************************************/
#include <stdint.h>

/***** CONSTANTS *************************************************************/
typedef struct _DebounceButton
{
    uint8_t rawState;
    uint8_t stableState;
    uint32_t lastChangeTick;
} DebounceButton;

/***** MACROS ****************************************************************/

/* ===== States ===== */
#define APP_STATE_INITIALIZATION     1
#define APP_STATE_PREOPERATIONAL     2
#define APP_STATE_OPERATIONAL        3
#define APP_STATE_EMERGENCY          4
#define APP_STATE_FAILURE            5

/* ===== Events ===== */
#define APP_EVT_INIT_DONE                1
#define APP_EVT_ERROR                    2
#define APP_EVT_SWITCH_STATE      		 3
#define APP_EVT_SENSOR_DEFECT            4
#define APP_EVT_TRIGGER_EMERGENCY        5
#define APP_EVT_ALARM_RESET              6

/* ===== API ===== */
int32_t applicationInit();
int32_t applicationRunCyclic();
int32_t applicationSendEvent(int32_t eventID);
int32_t applicationGetCurrentState();
uint8_t debounceButton(DebounceButton *btn, uint8_t newRawState, uint32_t currentTick);

#endif
