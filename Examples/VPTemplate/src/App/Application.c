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
 * @brief Implementation file for main application (state machine)
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include <string.h>

#include "Application.h"
#include "Util/Global.h"
#include "Util/Log/printf.h"

#include "UARTModule.h"
#include "ButtonModule.h"
#include "LEDModule.h"

#include "Util/StateTable/StateTable.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE FUNCTIONS *****************************************************/
static int32_t onEntryInitialization(State_t* pState, int32_t eventID);
static int32_t onStatePreOperational(State_t* pState, int32_t eventID);
static int32_t onStateOperational(State_t* pState, int32_t eventID);
static int32_t onStateEmergency(State_t* pState, int32_t eventID);
static int32_t onEntryFailure(State_t* pState, int32_t eventID);

/***** PRIVATE VARIABLES *****************************************************/

/**
 * @brief List of State for the State Machine
 *
 * This list only constructs the state objects for each possible state
 * in the state machine. There are no transistions or events defined
 *
 */

static State_t gStateList[] =
{
    { APP_STATE_INITIALIZATION, onEntryInitialization, 	0, 						0, false },
    { APP_STATE_PREOPERATIONAL, 0, 						onStatePreOperational, 	0, false },
    { APP_STATE_OPERATIONAL,    0,    					onStateOperational,    	0, false },
    { APP_STATE_EMERGENCY,      0,      				onStateEmergency,      	0, false },
    { APP_STATE_FAILURE,        onEntryFailure,        	0,        				0, false },
};

/**
 * @brief Definition of the transistion table of the state machine. Each row
 * contains FROM_STATE_ID, TO_STATE_ID, EVENT_ID, Function Pointer Guard Function
 *
 * The last two members of a transistion row are only the initialization of dynamic
 * members used durin runtim
 */
static StateTableEntry_t gStateTableEntries[] =
{
    /* Initialization */
    { APP_STATE_INITIALIZATION, APP_STATE_PREOPERATIONAL, APP_EVT_INIT_DONE,              0, 0, 0 },
    { APP_STATE_INITIALIZATION, APP_STATE_FAILURE,        APP_EVT_ERROR,                  0, 0, 0 },

    /* PreOperational */
    { APP_STATE_PREOPERATIONAL, APP_STATE_OPERATIONAL,    APP_EVT_SWITCH_OPERATIONAL,     0, 0, 0 },

    /* Operational */
    { APP_STATE_OPERATIONAL,    APP_STATE_PREOPERATIONAL, APP_EVT_SWITCH_PRE_OPERATIONAL, 0, 0, 0 },
    { APP_STATE_OPERATIONAL,    APP_STATE_FAILURE,        APP_EVT_SENSOR_DEFECT,          0, 0, 0 },
    { APP_STATE_OPERATIONAL,    APP_STATE_EMERGENCY,      APP_EVT_TRIGGER_EMERGENCY,      0, 0, 0 },

    /* Emergency */
    { APP_STATE_EMERGENCY,      APP_STATE_OPERATIONAL,    APP_EVT_ALARM_RESET,            0, 0, 0 },
};

/**
 * @brief Global State Table instance
 *
 */
static StateTable_t gStateTable;


/***** PUBLIC FUNCTIONS ******************************************************/

int32_t applicationInit(void)
{
    gStateTable.pStateList = gStateList;
    gStateTable.stateCount = sizeof(gStateList) / sizeof(State_t);
    int32_t result = stateTableInitialize(&gStateTable, gStateTableEntries, sizeof(gStateTableEntries) / sizeof(StateTableEntry_t), APP_STATE_INITIALIZATION);

    return result;
}

int32_t applicationRunCyclic(void)
{
    int32_t result = stateTableRunCyclic(&gStateTable);
    return result;
}

int32_t applicationSendEvent(int32_t eventID)
{
    int32_t result = stateTableSendEvent(&gStateTable, eventID);
    return result;
}

int32_t applicationGetCurrentState(void)
{
    return gStateTable.currentStateID;
}


/***** PRIVATE FUNCTIONS *****************************************************/
static int32_t onEntryInitialization(State_t* pState, int32_t eventID)
{
    (void)pState; (void)eventID;

    /* TODO: init start (one-time):
       - init modules
       - start self-tests
       - start timers
       When init is COMPLETE, trigger event:
       applicationSendEvent(APP_EVT_INIT_DONE);

       If init FAILS:
       applicationSendEvent(APP_EVT_ERROR);
    */

    return 0;
}

/* Called cyclic while in PREOPERATIONAL (no onEntry used) */
static int32_t onStatePreOperational(State_t* pState, int32_t eventID)
{
    (void)pState; (void)eventID;

    /* TODO: waiting for enable / command:
       if (enable_command_received)
           applicationSendEvent(APP_EVT_SWITCH_OPERATIONAL);
    */

    return 0;
}

/* Called cyclic while in OPERATIONAL */
static int32_t onStateOperational(State_t* pState, int32_t eventID)
{
    (void)pState; (void)eventID;

    /* TODO: monitoring & checks:
       if (sensor_defect_detected)
           applicationSendEvent(APP_EVT_SENSOR_DEFECT);

       if (emergency_condition_detected)
           applicationSendEvent(APP_EVT_TRIGGER_EMERGENCY);

       if (manual_switch_to_preop)
           applicationSendEvent(APP_EVT_SWITCH_PRE_OPERATIONAL);
    */

    return 0;
}

/* Called cyclic while in EMERGENCY */
static int32_t onStateEmergency(State_t* pState, int32_t eventID)
{
    (void)pState; (void)eventID;

    /* TODO:
       - ensure emergency outputs are active (depending on your architecture you may want this in entry)
       - wait for reset condition / ack
       if (reset_condition)
           applicationSendEvent(APP_EVT_ALARM_RESET);
    */

    return 0;
}

/* Called once after transition into FAILURE */
static int32_t onEntryFailure(State_t* pState, int32_t eventID)
{
    (void)pState; (void)eventID;

    /* TODO: enter safe state, latch fault, log
       No transitions out of FAILURE in your table.
    */

    return 0;
}

