#include "RadioConnect.h"

int32_t radioConnectInitialize(RadioConnect* pRadioConnect) {
	if (!pRadioConnect) return CONNECT_INVALID_PTR;

	pRadioConnect->reserved = 0xC0DE;

	return CONNECT_OK;
}

int32_t radioConnectSetChecksum(RadioConnect* pRadioConnect) {
	if (!pRadioConnect) return CONNECT_INVALID_PTR;

	int sum = 0;
	for (int i = 0; i < CONNECT_STRUCT_SIZE; i++) {
		sum += *(pRadioConnect >> i);
	}
	int lsb = (uint8_t)sum;
	pRadioConnect->checksum = (uint8_t)(lsb ^ 0xFF + 0x01); // TODO: get rid of magic numbers

	return CONNECT_OK;
}
