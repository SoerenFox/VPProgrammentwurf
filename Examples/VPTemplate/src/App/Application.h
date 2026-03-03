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
#define APP_EVT_SWITCH_OPERATIONAL       3
#define APP_EVT_SWITCH_PRE_OPERATIONAL   4
#define APP_EVT_SENSOR_DEFECT            5
#define APP_EVT_TRIGGER_EMERGENCY        6
#define APP_EVT_ALARM_RESET              7

/* ===== API ===== */
int32_t applicationInit(void);
int32_t applicationRunCyclic(void);
int32_t applicationSendEvent(int32_t eventID);
int32_t applicationGetCurrentState(void);

#endif
