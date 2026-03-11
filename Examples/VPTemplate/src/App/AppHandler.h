/*
 * AppHandler.h
 *
 *  Created on: Mar 11, 2026
 *      Author: kali
 */

#ifndef SRC_APP_APPHANDLER_H_
#define SRC_APP_APPHANDLER_H_

#include "Application.h"

int32_t gasSensorHandler(GasSensor* gSensor1, GasSensor* gSensor2, EMAFilterData* gEMA1, EMAFilterData* gEMA2);
int32_t displayDashDash(uint32_t gCycleCounter);
int32_t waterSensorHandler(uint32_t gCycleCounter);
int32_t buttonHandler(DebounceButton* gButtonSW1, DebounceButton* gButtonB1);



#endif /* SRC_APP_APPHANDLER_H_ */
