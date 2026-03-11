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
#include "stm32g4xx_hal.h"



/***** PRIVATE CONSTANTS *****************************************************/

/***** PRIVATE MACROS ********************************************************/

/***** Global TYPES *********************************************************/
EMAFilterData gEmaPot1;
EMAFilterData gEmaPot2;

GasSensor gGasSensor1;
GasSensor gGasSensor2;

DebounceButton gButtonSW1 = { BUTTON_RELEASED, BUTTON_RELEASED, 0};
DebounceButton gButtonB1  = { BUTTON_RELEASED, BUTTON_RELEASED, 0};

RadioConnect gRadioConnect;

// WaterSensor gWaterSensor;

/***** PRIVATE FUNCTIONS *****************************************************/
static int32_t initializePeripherals();

/***** PRIVATE VARIABLES *****************************************************/
static uint32_t lastToggle = 0;
/**
 * @brief List of State for the State Machine
 *
 * This list only constructs the state objects for each possible state
 * in the state machine. There are no transistions or events defined
 *
 */

State_t gStateList[] =
{
    { APP_STATE_INITIALIZATION, onEntryInitialization, 	0, 						0, 					false },
    { APP_STATE_PREOPERATIONAL, 0, 						0, 						0, 					false },
    { APP_STATE_OPERATIONAL,    onEntryOperational,		0,						onExitOperational, 	false },
    { APP_STATE_EMERGENCY,      0,      				onStateEmergency,      	onExitEmergency, 	false },
    { APP_STATE_FAILURE,        onEntryFailure,        	0,        				0, 					false },
};

/**
 * @brief Definition of the transistion table of the state machine. Each row
 * contains FROM_STATE_ID, TO_STATE_ID, EVENT_ID, Function Pointer Guard Function
 *
 * The last two members of a transistion row are only the initialization of dynamic
 * members used durin runtim
 */
StateTableEntry_t gStateTableEntries[] =
{
    /* Initialization */
    { APP_STATE_INITIALIZATION, APP_STATE_PREOPERATIONAL, APP_EVT_INIT_DONE,			0, 0, 0 },
    { APP_STATE_INITIALIZATION, APP_STATE_FAILURE,        APP_EVT_ERROR, 				0, 0, 0 },
	{ APP_STATE_INITIALIZATION, APP_STATE_FAILURE,        APP_EVT_STACK_CORRUPTION,		0, 0, 0 },

    /* PreOperational */
    { APP_STATE_PREOPERATIONAL, APP_STATE_OPERATIONAL,    APP_EVT_SWITCH_STATE,     	0, 0, 0 },
	{ APP_STATE_PREOPERATIONAL, APP_STATE_FAILURE,		  APP_EVT_STACK_CORRUPTION, 	0, 0, 0 },

    /* Operational */
    { APP_STATE_OPERATIONAL,    APP_STATE_PREOPERATIONAL, APP_EVT_SWITCH_STATE, 		0, 0, 0 },
    { APP_STATE_OPERATIONAL,    APP_STATE_FAILURE,        APP_EVT_SENSOR_DEFECT,        0, 0, 0 },
    { APP_STATE_OPERATIONAL,    APP_STATE_EMERGENCY,      APP_EVT_TRIGGER_EMERGENCY,    0, 0, 0 },
	{ APP_STATE_OPERATIONAL, 	APP_STATE_FAILURE,		  APP_EVT_STACK_CORRUPTION, 	0, 0, 0 },

    /* Emergency */
    { APP_STATE_EMERGENCY,      APP_STATE_OPERATIONAL,    APP_EVT_ALARM_RESET,          0, 0, 0 },
	{ APP_STATE_EMERGENCY, 		APP_STATE_FAILURE,		  APP_EVT_STACK_CORRUPTION, 	0, 0, 0 },
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

/**
 * @brief Initializes the used peripherals like GPIO,
 * ADC, DMA and Timer Interrupts
 *
 * @return Returns ERROR_OK if no error occurred
 */
static int32_t initializePeripherals()
{
    // Initialize UART used for Debug-Outputs
    uartInitialize(115200);

    // Initialize GPIOs for LED and 7-Segment output
	ledInitialize();
    displayInitialize();

    // Initialize GPIOs for Buttons
    buttonInitialize();

    // Initialize Timer, DMA and ADC for sensor measurements
    timerInitialize();
    adcInitialize();

    return ERROR_OK;
}


/***** PRIVATE FUNCTIONS *****************************************************/
int32_t onEntryInitialization(State_t* pState, int32_t eventID)
{
	// Initialize the System Clock
	SystemClock_Config();

	// Initialize Peripherals
	initializePeripherals();
	__enable_irq();

	filterInitEMA(&gEmaPot1, EMA_SCALE, EMA_ALPHA, true);
	filterInitEMA(&gEmaPot2, EMA_SCALE, EMA_ALPHA, true);

	gasSensorInitialize(&gGasSensor1, GASSENSORFACTOR);
	gasSensorInitialize(&gGasSensor2, GASSENSORFACTOR);

	// waterSensorInitialize(&gWaterSensor);

	HAL_Delay(SHORTDELAY);

	if (checkForValideADC(gasSensorReadPpmValue(&gGasSensor1, ADC_INPUT0), gasSensorReadPpmValue(&gGasSensor2, ADC_INPUT1)))
	{
		applicationSendEvent(APP_EVT_ERROR);
	}

	applicationSendEvent(APP_EVT_INIT_DONE);

    return STATE_OK;
}



int32_t onEntryOperational(State_t* pState, int32_t eventID)
{
	ledSetLED(LED0, LED_ON);
	return STATE_OK;
}

int32_t onExitOperational(State_t* pState, int32_t eventID)
{
	ledSetLED(LED0, LED_OFF);
	return STATE_OK;
}

/* Called cyclic while in EMERGENCY */
int32_t onStateEmergency(State_t* pState, int32_t eventID)
{
    (void)pState; (void)eventID;

    uint32_t now = HAL_GetTick();
    if ((now - lastToggle) >= FLASHINGTIMEMS)
		{
			ledToggleLED(LED1);
			lastToggle = now;
		}
    return STATE_OK;
}

int32_t onExitEmergency(State_t* pState, int32_t eventID)
{
	uint32_t now = HAL_GetTick();
	lastToggle = now;
	gasSensorResetThresholdTimers(now);
	waterSensorResetThresholdTimers(now);
	return STATE_OK;
}

/* Called once after transition into FAILURE */
int32_t onEntryFailure(State_t* pState, int32_t eventID)
{
	ledSetLED(LED0, LED_OFF);
	ledSetLED(LED1, LED_OFF);
	ledSetLED(LED2, LED_ON);
	ledSetLED(LED3, LED_OFF);
	if (eventID == APP_EVT_STACK_CORRUPTION)
	{
		ledSetLED(LED4, LED_OFF);
	} else ledSetLED(LED4, LED_ON);

    return STATE_OK;
}

uint8_t debounceButton(DebounceButton *btn, uint8_t newRawState)
{
	uint32_t now = HAL_GetTick();
    if (newRawState != btn->rawState)
    {
        btn->rawState = newRawState;
        btn->lastChangeTick = now;
    }

    if ((now - btn->lastChangeTick) >= DEBOUNCE_TIME_MS)
    {
        if (btn->stableState != btn->rawState)
        {
            btn->stableState = btn->rawState;
            return STATE_SWITCH; // state changed
        }
    }

    return STATE_OK;
}

