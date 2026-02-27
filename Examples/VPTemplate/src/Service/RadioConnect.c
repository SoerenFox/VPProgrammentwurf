#include "RadioConnect.h"

int32_t radioConnectInitialize(RadioConnect* pRadioConnect) {
	if (!pRadioConnect) return CONNECT_INVALID_PTR;

	pRadioConnect->reserved = 0xC0DE;

	return CONNECT_OK;
}
