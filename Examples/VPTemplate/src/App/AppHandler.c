/*
 * AppHandler.c
 *
 *  Created on: Mar 11, 2026
 *      Author: kali
 */
#include "AppHandler.h"

#define PERCENTTOLERANCE 	50 // Tolerance for filtered gasSensorsValues
#define FLASH_PERIOD_MS 	250
#define MAX_DISPLAY_VALUE 	999
#define DISPLAYFACTOR		10
#define DISPLAYSWITCH		2
#define DASH				16

#define BUTTON_OK 			0
#define BUTTON_ERR			-1




int32_t displayDashDash(uint32_t gCycleCounter)
{
	if (gCycleCounter % DISPLAYSWITCH == 0)
	{
		displayShowDigit(LEFT_DISPLAY, DASH);
	}
	else
	{
		displayShowDigit(RIGHT_DISPLAY, DASH);
	}
	return WATER_SENSOR_OK;
}

int32_t gasSensorHandler(GasSensor* gSensor1, GasSensor* gSensor2, EMAFilterData* gEMA1, EMAFilterData* gEMA2)
{
	// GasSensors
	int32_t gasValue1 = gasSensorReadPpmValue(gSensor1, ADC_INPUT0);
	int32_t gasValue2 = gasSensorReadPpmValue(gSensor2, ADC_INPUT1);

	if (checkForValideADC(gasValue1, gasValue2))
	{
		applicationSendEvent(APP_EVT_ERROR);
	}

	int32_t pot1_filtered = filterEMA(gEMA1, gasValue1);
	int32_t pot2_filtered = filterEMA(gEMA2, gasValue2);

	if (isGasSensorMismatch(pot1_filtered, pot2_filtered, PERCENTTOLERANCE))
	{
		applicationSendEvent(APP_EVT_SENSOR_DEFECT);
	}

	int32_t errorValue = gasSensorOverPpmValue(pot1_filtered, pot2_filtered);

	if (errorValue == EMERGENCYTRIGGER)
	{
		applicationSendEvent(APP_EVT_TRIGGER_EMERGENCY);
	}
	else if (errorValue == WARNINGTRIGGER)
	{
		ledSetLED(LED1, LED_ON);
	}
	else ledSetLED(LED1, LED_OFF);

	return SENSOR_OK;
}

int32_t waterSensorHandler(uint32_t gCycleCounter)
{
	uint32_t cmValue = gRadioConnect.sensorValue;
	if (wasSensorCheckValue(cmValue))
	{
		applicationSendEvent(APP_EVT_ERROR);
	}

	uint32_t errorValue = waterSensorOverCmValue(cmValue);

	// if (1500ms no input)

	if (errorValue == EMERGENCYTRIGGER)
	{
		applicationSendEvent(APP_EVT_TRIGGER_EMERGENCY);
	}
	else if (errorValue == WARNINGTRIGGER)
	{
		ledSetLED(LED1, LED_ON);
	}
	else ledSetLED(LED1, LED_OFF);

	if (gCycleCounter % DISPLAYSWITCH == 0)
	{
		if (cmValue > MAX_DISPLAY_VALUE) cmValue = MAX_DISPLAY_VALUE;
		displayShowDigit(LEFT_DISPLAY, (cmValue/(DISPLAYFACTOR * DISPLAYFACTOR)));
	}
	else
	{

		displayShowDigit(RIGHT_DISPLAY, (int32_t)(cmValue/DISPLAYFACTOR) % DISPLAYFACTOR);
	}
	return WATER_SENSOR_OK;
}

int32_t buttonHandler(DebounceButton* gButtonSW1, DebounceButton* gButtonB1)
{
	if (debounceButton(gButtonSW1, buttonGetButtonStatus(BTN_SW1)))
	{
		if (!(gButtonSW1->stableState)) return BUTTON_ERR;
		if (gButtonSW1->stableState)
		{
			applicationSendEvent(APP_EVT_SWITCH_STATE);
		}
	}

	if (debounceButton(gButtonB1, buttonGetButtonStatus(BTN_B1)))
	{
		if (!(gButtonB1->stableState)) return BUTTON_ERR;
		if (gButtonB1->stableState)
		{
			applicationSendEvent(APP_EVT_ALARM_RESET);
		}
	}
	return BUTTON_OK;
}
