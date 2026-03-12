/******************************************************************************
 * @file Scheduler.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Implementation of the cooperative scheduler with a  pre-defined set
 * of cyclic "task slots"
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "Scheduler.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/
#define HAL_TICK_VALUE_10MS     10      //!< Number of HAL Ticks used for 10ms Tasks
#define HAL_TICK_VALUE_50MS     50      //!< Number of HAL Ticks used for 50ms Tasks
#define HAL_TICK_VALUE_250MS    250     //!< Number of HAL Ticks used for 250ms Tasks

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/


int32_t schedInitialize(Scheduler* pScheduler)
{
    if (!pScheduler) return SCHED_ERR_INVALID_PTR;

    uint32_t now = 0;

    // Set all three needed schedulers to the current HAL tick value to start with a defined state
    pScheduler->halTick_10ms   = now;
    pScheduler->halTick_50ms   = now;
    pScheduler->halTick_250ms  = now;

    return SCHED_ERR_OK;
}

static void runSlot(uint32_t now, uint32_t period, uint32_t* pLast, CyclicFunction task)
{
    // Overflow-safe: unsigned subtraction
    if ((uint32_t)(now - *pLast) >= period)
    {
		*pLast = now;
		if (task) task();

    }
}

int32_t schedCycle(Scheduler* pScheduler)
{
    if (!pScheduler) return SCHED_ERR_INVALID_PTR;
    if (!pScheduler->pGetHALTick) return SCHED_ERR_INVALID_PTR;

    uint32_t now = pScheduler->pGetHALTick();

    // Check each slot if it is due for execution and runs task if so
    runSlot(now, HAL_TICK_VALUE_10MS,   &pScheduler->halTick_10ms,   pScheduler->pTask_10ms);
    runSlot(now, HAL_TICK_VALUE_50MS,   &pScheduler->halTick_50ms,   pScheduler->pTask_50ms);
    runSlot(now, HAL_TICK_VALUE_250MS,  &pScheduler->halTick_250ms,  pScheduler->pTask_250ms);

    return SCHED_ERR_OK;
}


/***** PRIVATE FUNCTIONS *****************************************************/
