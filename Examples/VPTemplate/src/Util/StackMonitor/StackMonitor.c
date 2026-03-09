#include "StackMonitor.h"


/**
 * @brief Calculates amount of free bytes in stash.
 *
 * @return Returns amount of free bytes in stack.
 */
int32_t getFreeBytes() {
	uint32_t* end = (uint32_t*)&_bottom_of_stack;
	int32_t byteCtr = 0;

	while (*end++ == STACK_VALUE) byteCtr += 4;
	return byteCtr;
}

/**
 * @brief Calculates amount of used bytes in stash.
 *
 * @return Returns amount of used bytes in stack.
 */
int32_t getUsedBytes() {
	return (int32_t)&_size_of_stack - getFreeBytes();
}


/**
 * @brief Calculates percentage of used bytes in stash.
 *
 * @return Returns percentage of used bytes in stash as XX%.
 */
int32_t getUsage() {
	return (int32_t)(getUsedBytes() / (int32_t)&_size_of_stack);
}


/**
 * @brief Checks if stash is corrupted by checking for stack overflow.
 *
 * @return Returns if last stack byte is overwritten as bool.
 */
bool isCorrupted() {
	uint32_t* lastAddr = (uint32_t*)&_bottom_of_stack;
	return (*lastAddr != STACK_VALUE);
}
