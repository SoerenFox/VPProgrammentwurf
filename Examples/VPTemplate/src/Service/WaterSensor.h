/*
 * WaterSensor.h
 *
 *  Created on: Mar 10, 2026
 *      Author: kali
 */

#ifndef SRC_SERVICE_WATERSENSOR_H_
#define SRC_SERVICE_WATERSENSOR_H_

#include <stdint.h>
#include "UARTModule.h"

#define WATER_SENSOR_OK                 0
#define WATER_SENSOR_INVALID_PTR       -1
#define WATER_SENSOR_VALUE_INVALID     -2
#define WATER_SENSOR_UART_ERROR        -3
#define WATER_SENSOR_TIMEOUT           -4
#define WATER_SENSOR_CRC_ERROR         -5
#define WATER_SENSOR_COUNTER_ERROR     -6

#define WATER_SENSOR_PACKET_LENGTH      4
#define WATER_SENSOR_TIMEOUT_MS         1500

typedef struct _WaterSensor
{
    uint16_t sensorValueCm;         // Current water level in cm
    uint8_t lastPacketCounter;      // Last valid packet counter
    uint32_t lastReceiveTick;       // Tick of last valid packet

    uint8_t hasValidPacket;         // 0 = no valid packet received yet
    uint8_t sensorDefect;           // 1 = defect detected

    uint8_t rxBuffer[WATER_SENSOR_PACKET_LENGTH];
    uint8_t rxIndex;
} WaterSensor;

int32_t waterSensorInitialize(WaterSensor* pSensor);
int32_t waterSensorSetSensorValue(WaterSensor* pSensor, uint16_t sensorValue);
int32_t waterSensorGetSensorValue(WaterSensor* pSensor);

uint8_t waterSensorHasSensorDefect(WaterSensor* pSensor);
void waterSensorClearSensorDefect(WaterSensor* pSensor);

/**
 * @brief Cyclic 10ms function for receiving UART packets and checking timeout.
 *
 * @param pSensor       Pointer to sensor object
 * @param enabled       1 = UART handling active, 0 = disabled
 * @param currentTick   Current system tick in ms
 *
 * @return WATER_SENSOR_OK if no error occurred
 */
int32_t waterSensorCyclic10ms(WaterSensor* pSensor, uint8_t enabled, uint32_t currentTick);

#endif
