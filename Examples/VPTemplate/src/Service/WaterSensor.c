/*
 * WaterSensor.c
 *
 *  Created on: Mar 10, 2026
 *      Author: kali
 */
#include "WaterSensor.h"

#include "WaterSensor.h"

static uint8_t waterSensorCalculateCRC(const uint8_t* pData, uint8_t length);
static int32_t waterSensorHandlePacket(WaterSensor* pSensor, uint32_t currentTick);

int32_t waterSensorInitialize(WaterSensor* pSensor)
{
    if (pSensor == 0)
    {
        return WATER_SENSOR_INVALID_PTR;
    }

    pSensor->sensorValueCm = 0u;
    pSensor->lastPacketCounter = 0u;
    pSensor->lastReceiveTick = 0u;
    pSensor->hasValidPacket = 0u;
    pSensor->sensorDefect = 0u;
    pSensor->rxIndex = 0u;

    for (uint8_t i = 0u; i < WATER_SENSOR_PACKET_LENGTH; i++)
    {
        pSensor->rxBuffer[i] = 0u;
    }

    return WATER_SENSOR_OK;
}

int32_t waterSensorSetSensorValue(WaterSensor* pSensor, uint16_t sensorValue)
{
    if (pSensor == 0)
    {
        return WATER_SENSOR_INVALID_PTR;
    }

    pSensor->sensorValueCm = sensorValue;
    return WATER_SENSOR_OK;
}

int32_t waterSensorGetSensorValue(WaterSensor* pSensor)
{
    if (pSensor == 0)
    {
        return WATER_SENSOR_INVALID_PTR;
    }

    return (int32_t)pSensor->sensorValueCm;
}

uint8_t waterSensorHasSensorDefect(WaterSensor* pSensor)
{
    if (pSensor == 0)
    {
        return 1u;
    }

    return pSensor->sensorDefect;
}

void waterSensorClearSensorDefect(WaterSensor* pSensor)
{
    if (pSensor == 0)
    {
        return;
    }

    pSensor->sensorDefect = 0u;
}

int32_t waterSensorCyclic10ms(WaterSensor* pSensor, uint8_t enabled, uint32_t currentTick)
{
    if (pSensor == 0)
    {
        return WATER_SENSOR_INVALID_PTR;
    }

    /* UART reception only active in Operational state */
    if (enabled == 0u)
    {
        pSensor->rxIndex = 0u;
        pSensor->lastReceiveTick = currentTick;
        return WATER_SENSOR_OK;
    }

    /* Read all currently available bytes, but only one packet size max per cycle */
    for (uint8_t i = 0u; i < WATER_SENSOR_PACKET_LENGTH; i++)
    {
        int8_t hasData = 0;
        uint8_t rxByte = 0u;

        if (uartHasData(&hasData) != UART_ERR_OK)
        {
            pSensor->sensorDefect = 1u;
            return WATER_SENSOR_UART_ERROR;
        }

        if (hasData == 0)
        {
            break;
        }

        if (uartReceiveData(&rxByte, 1) != UART_ERR_OK)
        {
            pSensor->sensorDefect = 1u;
            return WATER_SENSOR_UART_ERROR;
        }

        if (pSensor->rxIndex < WATER_SENSOR_PACKET_LENGTH)
        {
            pSensor->rxBuffer[pSensor->rxIndex] = rxByte;
            pSensor->rxIndex++;
        }

        if (pSensor->rxIndex >= WATER_SENSOR_PACKET_LENGTH)
        {
            int32_t result = waterSensorHandlePacket(pSensor, currentTick);
            pSensor->rxIndex = 0u;

            if (result != WATER_SENSOR_OK)
            {
                return result;
            }
        }
    }

    /* Timeout check: no valid packet for > 1500 ms */
    if ((currentTick - pSensor->lastReceiveTick) > WATER_SENSOR_TIMEOUT_MS)
    {
        pSensor->sensorDefect = 1u;
        return WATER_SENSOR_TIMEOUT;
    }

    return WATER_SENSOR_OK;
}

static int32_t waterSensorHandlePacket(WaterSensor* pSensor, uint32_t currentTick)
{
    uint8_t receivedCounter;
    uint8_t expectedCounter;
    uint8_t receivedCRC;
    uint8_t calculatedCRC;
    uint16_t receivedValue;

    receivedCounter = pSensor->rxBuffer[0];
    receivedValue = ((uint16_t)pSensor->rxBuffer[1] << 8) | (uint16_t)pSensor->rxBuffer[2];
    receivedCRC = pSensor->rxBuffer[3];

    calculatedCRC = waterSensorCalculateCRC(pSensor->rxBuffer, WATER_SENSOR_PACKET_LENGTH - 1u);

    if (receivedCRC != calculatedCRC)
    {
        pSensor->sensorDefect = 1u;
        return WATER_SENSOR_CRC_ERROR;
    }

    if (pSensor->hasValidPacket != 0u)
    {
        expectedCounter = (uint8_t)(pSensor->lastPacketCounter + 1u);

        if (receivedCounter != expectedCounter)
        {
            pSensor->sensorDefect = 1u;
            return WATER_SENSOR_COUNTER_ERROR;
        }
    }

    pSensor->sensorValueCm = receivedValue;
    pSensor->lastPacketCounter = receivedCounter;
    pSensor->lastReceiveTick = currentTick;
    pSensor->hasValidPacket = 1u;

    return WATER_SENSOR_OK;
}

static uint8_t waterSensorCalculateCRC(const uint8_t* pData, uint8_t length)
{
    uint8_t crc = 0u;

    for (uint8_t i = 0u; i < length; i++)
    {
        crc ^= pData[i];
    }

    return crc;
}
