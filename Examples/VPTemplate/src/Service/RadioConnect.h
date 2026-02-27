#ifndef _RADIO_CONNECT_H_
#define _RADIO_CONNECT_H_

#include  <stdint.h>

#define CONNECT_OK 0
#define CONNECT_INVALID_PTR -1
#define CONNECT_VALUE_INVALID -2

#define CONNECT_STRUCT_SIZE 7 // Structure size without checksum in bytes

#pragma pack(push, 1)
typedef struct _RadioConnect {
	uint8_t packetCounter;
	uint32_t sensorValue;
	uint16_t reserved;
	int8_t checksum;
} RadioConnect;
#pragma pack(pop)

int32_t radioConnectInitialize(RadioConnect* pRadioConnect);
int32_t radioConnectSetChecksum(RadioConnect* pRadioConnect);

#endif
