#include "RadioConnect.h"

#include "UARTModule.h"
#include <string.h>

#define ONE_BYTE 1

#define CONNECT_STRUCT_SIZE 8 // Structure size
#define CONNECT_STRUCT_SIZE_WO_CRC 7// struct size w/ checksum

#define MIN_SIZE_FOR_RESERVED_BYTE 3
#define RESERVED_FIRST_BYTE_POS 3 // position in struct of 0xC0 in struct from behind
#define RESERVED_FIRST_BYTE 192 // 0xC0 in decimal
#define RESERVED_SECOND_BYTE_POS 2 // position in struct of 0xDE in struct from behind
#define RESERVED_SECOND_BYTE 222 // 0xDE in decimal

int32_t radioConnectInitialize(RadioConnect* pRadioConnect) {
	if (!pRadioConnect) return CONNECT_INVALID_PTR;

	pRadioConnect->reserved = 0xC0DE;

	return CONNECT_OK;
}

int32_t radioConnectSetChecksum(RadioConnect* pRadioConnect) {
	if (!pRadioConnect) return CONNECT_INVALID_PTR;

	uint8_t* bytes = (uint8_t*)pRadioConnect;

	int sum = 0;
	for (int i = 0; i < CONNECT_STRUCT_SIZE_WO_CRC; i++) {
		 sum += bytes[i];
	}
	int lsb = (uint8_t)sum;
	pRadioConnect->checksum = (uint8_t)((lsb ^ 0xFF) + 0x01); // TODO: get rid of magic numbers

	return CONNECT_OK;
}

/*
 * @brief Reads UART buffer and writes into pRadioConnect after syncing with struct format
 *
 * @params Radioconnect* as destination where to write message
 *
 * @return error status
 */
int32_t radioConnectBufferToStruct(RadioConnect* pRadioConnect) {
	if (!pRadioConnect) return CONNECT_INVALID_PTR;

	static uint8_t buffer[8];
	static int size = 0;
	int8_t hasChar = 0;
	uartHasData(&hasChar);

	if (hasChar) {
		uint8_t ch = 0;
		uint32_t receiveOK = uartReceiveData(&ch, ONE_BYTE);

		if (receiveOK == UART_ERR_OK) {
			buffer[size++] = ch;
			if (size >= MIN_SIZE_FOR_RESERVED_BYTE && buffer[size - RESERVED_FIRST_BYTE_POS] == RESERVED_FIRST_BYTE && buffer[size - RESERVED_SECOND_BYTE_POS] == RESERVED_SECOND_BYTE) {
				if (size != CONNECT_STRUCT_SIZE) {
					size = 0;
					return CONNECT_OK;
				}
				memcpy(pRadioConnect, buffer, sizeof(RadioConnect));
				size = 0;
			}
		}
	}
	return CONNECT_OK;
}
