#include "RadioConnect.h"

#include "UARTModule.h"
#include "stm32g4xx_hal.h"
#include <string.h>

#define ONE_BYTE 1

#define CONNECT_STRUCT_SIZE 8 // Structure size
#define CONNECT_STRUCT_SIZE_WO_CRC 7// struct size w/ checksum

#define MIN_SIZE_FOR_RESERVED_BYTE 3
#define RESERVED_FIRST_BYTE_POS 3 // position in struct of 0xC0 in struct from behind
#define RESERVED_FIRST_BYTE 192 // 0xC0 in decimal
#define RESERVED_SECOND_BYTE_POS 2 // position in struct of 0xDE in struct from behind
#define RESERVED_SECOND_BYTE 222 // 0xDE in decimal

#define CHECKSUM_COMPLEMENT 0xFF
#define CHECKSUM_INCREMENT 0x01

static int lastReceived;
static uint8_t previousDataCount = 0;
static uint8_t hasPreviousData = 0;

static int8_t calculateChecksum(RadioConnect* pRadioConnect);
static int32_t verifyReceived(RadioConnect* pRadioConnect);

/*
 * @brief Reads UART buffer and writes into pRadioConnect after syncing with struct format
 *
 * @params Radioconnect* as destination where to write message
 *
 * @return error status
 */
int32_t radioConnectBufferToStruct(RadioConnect* pRadioConnect) {
	if (!pRadioConnect) return CONNECT_INVALID_PTR;

	static uint8_t buffer[CONNECT_STRUCT_SIZE];
	static int size = 0;
	int8_t hasChar = 0;
	uartHasData(&hasChar);

	if (hasChar) {
		uint8_t ch = 0;
		uint32_t receiveOK = uartReceiveData(&ch, ONE_BYTE);

		if (receiveOK == UART_ERR_OK) {
			if (size >= CONNECT_STRUCT_SIZE) return CONNECT_SENSOR_DEFECT; // buffer overflow
			buffer[size++] = ch;
			if (size >= MIN_SIZE_FOR_RESERVED_BYTE && buffer[size - RESERVED_FIRST_BYTE_POS] == RESERVED_FIRST_BYTE && buffer[size - RESERVED_SECOND_BYTE_POS] == RESERVED_SECOND_BYTE) {
				if (size != CONNECT_STRUCT_SIZE) {
					size = 0;
					return CONNECT_OK;
				}
				memcpy(pRadioConnect, buffer, sizeof(RadioConnect));
				size = 0;
				lastReceived = HAL_GetTick();
				return verifyReceived(pRadioConnect);
			}
		}
	}
	return CONNECT_OK;
}


int32_t radioConnectGetLastInputTime(RadioConnect* pRadioConnect, int* lastInputTime) {
	if (!pRadioConnect || !lastInputTime) return CONNECT_INVALID_PTR;

	*lastInputTime = lastReceived;

	return CONNECT_OK;
}

static int8_t calculateChecksum(RadioConnect* pRadioConnect) {
	uint8_t* bytes = (uint8_t*)pRadioConnect;

	int sum = 0;
	for (int i = 0; i < CONNECT_STRUCT_SIZE_WO_CRC; i++) {
		sum += bytes[i];
	}
	uint8_t lsb = (uint8_t)sum;
	return (int8_t)((lsb ^ CHECKSUM_COMPLEMENT) + CHECKSUM_INCREMENT);
}



static int32_t verifyReceived(RadioConnect* pRadioConnect) {
	if (pRadioConnect->checksum != calculateChecksum(pRadioConnect)) return CONNECT_SENSOR_DEFECT;
	if (!hasPreviousData) {
			previousDataCount = pRadioConnect->packetCounter;
			hasPreviousData = 1;
			return CONNECT_OK;
	}

	if (pRadioConnect->packetCounter != (uint8_t)(previousDataCount + 1)) return CONNECT_SENSOR_DEFECT;

	previousDataCount = pRadioConnect->packetCounter;
	return CONNECT_OK;
}



Final Review then please
