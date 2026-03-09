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
#include "HardwareConfig.h"

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
