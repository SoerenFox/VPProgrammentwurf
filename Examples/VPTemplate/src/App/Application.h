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
#include "stm32g4xx_hal.h"

#include "HardwareConfig.h"
#include "System.h"
#include "stm32g4xx.h"
#include "Util/StateTable/StateTable.h"

#include "Util/Global.h"
#include "Util/Log/printf.h"
#include "Util/Log/LogOutput.h"
#include "Util/Filter/Filter.h"

#include "UARTModule.h"
#include "ButtonModule.h"
#include "LEDModule.h"
#include "DisplayModule.h"
#include "ADCModule.h"
#include "TimerModule.h"
#include "Scheduler.h"
#include "GasSensor.h"
#include "WaterSensor.h"
#include "RadioConnect.h"

#include "GlobalObjects.h"


/***** CONSTANTS *************************************************************/
typedef struct _DebounceButton
{
    uint8_t rawState;
    uint8_t stableState;
    uint32_t lastChangeTick;
} DebounceButton;

extern EMAFilterData gEmaPot1;
extern EMAFilterData gEmaPot2;

extern GasSensor gGasSensor1;
extern GasSensor gGasSensor2;

extern DebounceButton gButtonSW1;
extern DebounceButton gButtonB1;

extern RadioConnect gRadioConnect;

/***** MACROS ****************************************************************/
#define STATE_OK 	0
#define STATE_SWITCH 1

#define DEBOUNCE_TIME_MS 	50
#define EMA_SCALE   		1000
#define EMA_ALPHA   		500   // 0.5
#define GASSENSORFACTOR 	204
#define SHORTDELAY 			20
#define FLASHINGTIMEMS 		250

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
	APP_EVT_ERROR,
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

int32_t onEntryInitialization(State_t* pState, int32_t eventID);
int32_t onStatePreOperational(State_t* pState, int32_t eventID);
int32_t onEntryOperational(State_t* pState, int32_t eventID);
int32_t onExitOperational(State_t* pState, int32_t eventID);
int32_t onStateEmergency(State_t* pState, int32_t eventID);
int32_t onExitEmergency(State_t* pState, int32_t eventID);
int32_t onEntryFailure(State_t* pState, int32_t eventID);

uint8_t debounceButton(DebounceButton *btn, uint8_t newRawState);

#endif
