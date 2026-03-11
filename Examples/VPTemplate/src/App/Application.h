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
#include "GlobalObjects.h"


/***** CONSTANTS *************************************************************/

/***** MACROS ****************************************************************/
#define EMA_SCALE   		1000
#define EMA_ALPHA   		500   // 0.5
#define GASSENSORFACTOR 	204

#define SHORTDELAY 			20	// in ms
#define FLASHINGTIMEMS 		250	// in ms
#define DEBOUNCE_TIME_MS 	50	// in ms

/* ===== States ===== */
typedef enum
{
	APP_STATE_INITIALIZATION = 1,
	APP_STATE_PREOPERATIONAL,
	APP_STATE_OPERATIONAL,
	APP_STATE_EMERGENCY,
	APP_STATE_FAILURE
} AppState;

/* ===== Events ===== */
typedef enum
{
	APP_EVT_INIT_DONE = 1,
	APP_EVT_SWITCH_STATE,
	APP_EVT_SENSOR_DEFECT,
	APP_EVT_TRIGGER_EMERGENCY,
	APP_EVT_ALARM_RESET,
	APP_EVT_STACK_CORRUPTION
} AppEvent;

/* ===== API ===== */
int32_t applicationInit();
int32_t applicationRunCyclic();
int32_t applicationSendEvent(int32_t eventID);
int32_t applicationGetCurrentState();

int32_t onEntryInitialization();
int32_t onStatePreOperational();
int32_t onEntryOperational();
int32_t onExitOperational();
int32_t onStateEmergency();
int32_t onExitEmergency();
int32_t onEntryFailure(State_t* pState, int32_t eventID);

uint8_t debounceButton(DebounceButton *btn, uint8_t newRawState);

#endif
