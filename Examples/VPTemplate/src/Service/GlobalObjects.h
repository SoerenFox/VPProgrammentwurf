/******************************************************************************
 * @file GlobalObjects.h
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Header file for global objects used across different modules (based on
 * access functions)
 *
 *
 *****************************************************************************/
#ifndef _GLOBAL_OBJECT_H_
#define _GLOBAL_OBJECT_H_


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

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/


/***** TYPES *****************************************************************/
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

/***** PROTOTYPES ************************************************************/


#endif
