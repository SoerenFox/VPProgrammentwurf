#ifndef _STACK_MONITOR_H_
#define _STACK_MONITOR_H_

#include "stdint.h"
#include <stdbool.h>

#define STACK_VALUE 0xDEADBEEF

extern int32_t _size_of_stack;
extern uint8_t _bottom_of_stack;

int32_t getFreeBytes();
int32_t getUsedBytes();
int32_t getUsage();
bool isCorrupted();

#endif
