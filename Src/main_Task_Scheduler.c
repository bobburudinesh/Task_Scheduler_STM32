/*
 * main_Task_Scheduler.c
 *
 *  Created on: May 1, 2025
 *      Author: Dinesh Bobburu
 */


#include <stdint.h>

void enable_faults(void);

int main(void) {

	enable_faults();
	while(1);
	return 0;
}

void enable_faults(void) {
	uint32_t *pSCHSR = (uint32_t*)(0xE000ED24); // System Handler Control and State Register
	*pSCHSR |= (1 << 18);	// Usage fault enabled
	*pSCHSR |= (1 << 17);	// Bus fault enabled
	*pSCHSR |= (1 << 16);	// Mem fault enabled
}

void MemManage_Handler(void) {
	while(1);
}

void BusFault_Handler(void) {
	while(1);
}

void UsageFault_Handler(void) {
	while(1);
}
