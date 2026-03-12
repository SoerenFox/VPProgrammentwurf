/*
 * AppHandler.c
 *
 *  Created on: Mar 11, 2026
 *      Author: kali
 */
#include "AppHandler.h"
#include <stdbool.h>

#define FLASH_PERIOD_MS 	250

#define MAX_DISPLAY_VALUE 	999
#define DISPLAYFACTOR		10
#define DISPLAYSWITCH		2
#define DASH				16

// #define TESTVALUE		1000

static bool gGasWarning = false;
static bool gWaterWarning = false;

// Function to show "--" on the display while not in operational state
// Requires gCycleCounter to switch between left and right display
void displayDashDash(uint32_t gCycleCounter)
{
	if (gCycleCounter % DISPLAYSWITCH == 0)
	{
		displayShowDigit(LEFT_DISPLAY, DASH);
	}
	else
	{
		displayShowDigit(RIGHT_DISPLAY, DASH);
	}
}

// Function to handle the gas sensor logic, including reading values, filtering, checking for mismatches and triggering warnings or emergencies
int32_t gasSensorHandler(GasSensor* gSensor1, GasSensor* gSensor2, EMAFilterData* gEMA1, EMAFilterData* gEMA2)
{
	// GasSensors
	int32_t gasValue1 = gasSensorReadPpmValue(gSensor1, ADC_INPUT0);
	int32_t gasValue2 = gasSensorReadPpmValue(gSensor2, ADC_INPUT1);

	if (checkForValideADC(gasValue1, gasValue2) == SENSOR_PPMVALUE_INVALID)
	{
		applicationSendEvent(APP_EVT_SENSOR_DEFECT);
		return STATETBL_ERR_OK;
	}

	int32_t pot1_filtered = filterEMA(gEMA1, gasValue1);
	int32_t pot2_filtered = filterEMA(gEMA2, gasValue2);

	if (isGasSensorMismatch(pot1_filtered, pot2_filtered) == SENSOR_PPMVALUE_INVALID)
	{
		applicationSendEvent(APP_EVT_SENSOR_DEFECT);
		return STATETBL_ERR_OK;
	}

	int32_t errorValue = gasSensorOverPpmValue(calculateAvgPpmValue(pot1_filtered, pot2_filtered));

	if (errorValue == EMERGENCYTRIGGER)
	{
		applicationSendEvent(APP_EVT_TRIGGER_EMERGENCY);
		return STATETBL_ERR_OK;
	}
	else if (errorValue == WARNINGTRIGGER)
	{
		gGasWarning = true;
	}
	else
	{
		gGasWarning = false;
	}

	/* LED Logik */
	if (gGasWarning || gWaterWarning)
	{
		ledSetLED(LED1, LED_ON);
	}
	else
	{
		ledSetLED(LED1, LED_OFF);
	}

	return SENSOR_OK;
}

// Function to handle the water sensor logic, including reading values, checking for validity, triggering warnings or emergencies and updating the display
int32_t waterSensorHandler(uint32_t gCycleCounter)
{
	uint32_t cmValue = gRadioConnect.sensorValue; // Tested with TESTVALUE

	if (wasSensorCheckValue(cmValue) == (WATER_SENSOR_VALUE_INVALID || UART_ERR_RECEIVE))
	{
		applicationSendEvent(APP_EVT_SENSOR_DEFECT);
		return STATETBL_ERR_OK;
	}

	uint32_t errorValue = waterSensorOverCmValue(cmValue);

	// if (1500ms no input)

	if (errorValue == EMERGENCYTRIGGER)
	{
		applicationSendEvent(APP_EVT_TRIGGER_EMERGENCY);
		return STATETBL_ERR_OK;
	}
	else if (errorValue == WARNINGTRIGGER)
	{
		gWaterWarning = true;
	}
	else
	{
		gWaterWarning = false;
	}

	/* LED Logik */
	if (gGasWarning || gWaterWarning)
	{
		ledSetLED(LED1, LED_ON);
	}
	else
	{
		ledSetLED(LED1, LED_OFF);
	}

	if (cmValue > MAX_DISPLAY_VALUE)
	{
		cmValue = MAX_DISPLAY_VALUE;
	}

	if (gCycleCounter % DISPLAYSWITCH == 0)
	{
		displayShowDigit(LEFT_DISPLAY, (cmValue/(DISPLAYFACTOR * DISPLAYFACTOR)));
	}
	else
	{
		int32_t new = (cmValue/DISPLAYFACTOR);
		displayShowDigit(RIGHT_DISPLAY, (int32_t)(new) % DISPLAYFACTOR);
	}
	return WATER_SENSOR_OK;
}

// Function to handle button inputs, including debouncing and triggering events based on button states
int32_t buttonHandler(DebounceButton* gButtonSW1, DebounceButton* gButtonB1)
{
	if (debounceButton(gButtonSW1, buttonGetButtonStatus(BTN_SW1)))
	{
		if (!(gButtonSW1->stableState)) return BUTTON_ERR;
		if (gButtonSW1->stableState)
		{
			applicationSendEvent(APP_EVT_SWITCH_STATE);
			return STATETBL_ERR_OK;
		}
	}

	if (debounceButton(gButtonB1, buttonGetButtonStatus(BTN_B1)))
	{
		if (!(gButtonB1->stableState)) return BUTTON_ERR;
		if (gButtonB1->stableState)
		{
			applicationSendEvent(APP_EVT_ALARM_RESET);
			return STATETBL_ERR_OK;
		}
	}
	return BUTTON_OK;
}
