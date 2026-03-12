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
#include "Application.h"

/***** PRIVATE CONSTANTS *****************************************************/

/***** PRIVATE MACROS ********************************************************/

/***** Global TYPES *********************************************************/

// WaterSensor gWaterSensor;

/***** PRIVATE FUNCTIONS *****************************************************/
static int32_t initializePeripherals();
static int32_t onEntryInitialization();
static int32_t onStatePreOperational();
static int32_t onEntryOperational();
static int32_t onExitOperational();
static int32_t onStateEmergency();
static int32_t onExitEmergency();
static int32_t onEntryFailure(State_t* pState, int32_t eventID);

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
    { APP_STATE_INITIALIZATION, APP_STATE_FAILURE,        APP_EVT_SENSOR_DEFECT, 		0, 0, 0 },
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

// As required by the requirements, the system shall start in the Initialization state and automatically transition to the PreOperational state after initialization is done. This is implemented by sending the APP_EVT_INIT_DONE event at the end of this function, which triggers the transition to the PreOperational state.
staticint32_t onEntryInitialization()
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


    // Here the application can check if the sensors are working by reading the ADC values and checking if they are valid. If not, it can send the APP_EVT_SENSOR_DEFECT event to transition to the Failure state.
	HAL_Delay(SHORTDELAY);
    // The short delay is needed to ensure that the ADC has finished its first conversion after initialization before reading the values for the first time. This is necessary because the ADC might return invalid values if read immediately after initialization.
	if (checkForValideADC(gasSensorReadPpmValue(&gGasSensor1, ADC_INPUT0), gasSensorReadPpmValue(&gGasSensor2, ADC_INPUT1)))
	{
		applicationSendEvent(APP_EVT_SENSOR_DEFECT);
	}

	applicationSendEvent(APP_EVT_INIT_DONE);

    return STT_NONE_EVENT;
}

// Turns on LED0 when entering the Operational state and turns it off when exiting the Operational state. 
static int32_t onEntryOperational()
{
	ledSetLED(LED0, LED_ON);
	return STT_NONE_EVENT;
}

int32_t onExitOperational()
{
	ledSetLED(LED0, LED_OFF);
	return STT_NONE_EVENT;
}

// Called cyclic while in EMERGENCY to toggle LED1 with a defined flashing time. The function also resets the threshold timers of the sensors to ensure that they do not trigger a sensor defect event after exiting the emergency state.
static int32_t onStateEmergency()
{
    uint32_t now = HAL_GetTick();
    if ((now - lastToggle) >= FLASHINGTIMEMS)
		{
			ledToggleLED(LED1);
			lastToggle = now;
		}
    return STT_NONE_EVENT;
}

static int32_t onExitEmergency()
{
	uint32_t now = HAL_GetTick();
	lastToggle = now;
	gasSensorResetThresholdTimers(now);
	waterSensorResetThresholdTimers(now);
	return STT_NONE_EVENT;
}

// Called once after transition into FAILURE state to turn on LED2 to indicate that the system is in a failure state. Additionally, LED4 is turned on if the failure was caused by a sensor defect and turned off if the failure was caused by stack corruption.
static int32_t onEntryFailure(State_t* pState, int32_t eventID)
{
	ledSetLED(LED0, LED_OFF);
	ledSetLED(LED1, LED_OFF);
	ledSetLED(LED2, LED_ON);
	ledSetLED(LED3, LED_OFF);
	if (eventID == APP_EVT_STACK_CORRUPTION)
	{
		ledSetLED(LED4, LED_OFF);
	} else ledSetLED(LED4, LED_ON);

    return STT_NONE_EVENT;
}

// This function is used to debounce the buttons. It takes a pointer to a DebounceButton struct and the new raw state of the button as input. It updates the raw state and stable state of the button based on the debounce time and returns STT_NEW_EVENT if the stable state has changed, otherwise it returns STT_NONE_EVENT.
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
            return STT_NEW_EVENT; // state changed
        }
    }

    return STT_NONE_EVENT;
}

