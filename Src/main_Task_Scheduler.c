/*
 * main_Task_Scheduler.c
 *
 *  Created on: May 1, 2025
 *      Author: Dinesh Bobburu
 */


#include <stdint.h>


#define SRAM_START	0x20000000U					 // SRAM Start
#define SRAM_SIZE	(128 * 1024)
#define SRAM_END	((SRAM_START) + (SRAM_SIZE)) // SRAM_END

#define TASK_STACK_SIZE		1024U		// Task stack size
#define SCHEDULER_STACK_SIZE	1024U	// Scheduler stack size

#define MAX_TASKS	5	// Number of Tasks

#define	TASK_IDLE_STACK_START	SRAM_END	// Idle task stack start
#define TASK_1_STACK_START	(SRAM_END - (1 * 1024))	//Task1 stack start
#define TASK_2_STACK_START	(SRAM_END - (2 * 1024))	//Task2 stack start
#define TASK_3_STACK_START	(SRAM_END - (3 * 1024))	//Task3 stack start
#define TASK_4_STACK_START	(SRAM_END - (4 * 1024))	//Task4 stack start
#define SCHEDULER_STACK_START	(SRAM_END - (5 * 1024))	//Scheduler stack start

#define XPSR_DEFAULT	0x01000000U  // Default value of XPSR to be stored during task creation
#define EXEC_RETURN	0xFFFFFFFDU  //Exit return value to be stored in LR used for triggering exception exit

#define SYS_CLK		16000000U
#define TICK_HZ		1000

//#define TASK_STATE_READY		1
//#define TASK_STATE_RUNNING
//#define	TASK_STATE_BLOCKED
//#define	TASK_STATE_SUSPENDED


typedef enum {
	ready = 1,
	running = 2,
	blocked = 3,
	suspended = 4
}Task_States_t;

void enable_Faults(void);
void task_1_Handler(void);
void task_1_Handler(void);
void task_2_Handler(void);
void task_3_Handler(void);
void task_4_Handler(void);

void tasks_Init(void);
void enable_Sys_Tick_Timer(void);


typedef struct {
	uint32_t	*top_Of_Stack;		// Current top of the stack, latest PSP value
	uint32_t	block_count;		// The delay upto which task is blocked
	Task_States_t	task_State;				// current state of the task, of it is ready or blocked.
	void	    (*task_Handler)(void);	// Task handler for this task
}TCB_t;	// Task Control Block Data Structure

TCB_t user_Tasks[MAX_TASKS];

int main(void) {

	enable_Faults();
	tasks_Init();
	enable_Sys_Tick_Timer();
	while(1);
	return 0;
}

void enable_Faults(void) {
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

// Task Handlers START

void task_Idle_Handler(void) {
	// Task Handler for Idle Task
	while(1) {

	}
}


void task_1_Handler(void) {
	// Task Handler for Task 1
	while(1) {

	}
}

void task_2_Handler(void) {
	// Task Handler for Task 2
	while(1) {

	}
}

void task_3_Handler(void) {
	// Task Handler for Task 3
	while(1) {

	}
}

void task_4_Handler(void) {
	// Task Handler for Task 4
	while(1) {

	}
}

// Task Handlers END


void tasks_Init(void) {

	user_Tasks[0].task_State = ready;
	user_Tasks[1].task_State = ready;
	user_Tasks[2].task_State = ready;
	user_Tasks[3].task_State = ready;
	user_Tasks[4].task_State = ready;

	user_Tasks[0].task_Handler = task_Idle_Handler;
	user_Tasks[1].task_Handler = task_1_Handler;
	user_Tasks[2].task_Handler = task_2_Handler;
	user_Tasks[3].task_Handler = task_3_Handler;
	user_Tasks[4].task_Handler = task_4_Handler;

	user_Tasks[0].top_Of_Stack = (uint32_t*)TASK_IDLE_STACK_START;
	user_Tasks[1].top_Of_Stack = (uint32_t*)TASK_1_STACK_START;
	user_Tasks[2].top_Of_Stack = (uint32_t*)TASK_2_STACK_START;
	user_Tasks[3].top_Of_Stack = (uint32_t*)TASK_3_STACK_START;
	user_Tasks[4].top_Of_Stack = (uint32_t*)TASK_4_STACK_START;

	for(int i = 0; i < MAX_TASKS; i++) {
		uint32_t *pPSP = user_Tasks[i].top_Of_Stack;
		pPSP--;
		*pPSP = XPSR_DEFAULT;	// XPSR = default XPSR value

		pPSP--;
		*pPSP = (uint32_t)user_Tasks[i].task_Handler; // PC = Task handler for that task

		pPSP--;
		*pPSP = EXEC_RETURN;	// LR = Exec return value

		for(uint8_t j = 4; j<12; j++) {	// Initializing registers R4-R11 to 0
			pPSP--;
			*pPSP = 0;
		}
		user_Tasks[i].top_Of_Stack = pPSP;	// Save the value of top of stack of that task(PSP)
	}
}

void enable_Sys_Tick_Timer(void) {
	uint32_t *pSCSR = (uint32_t*)0xE000E010; // SysTick Control and Status Register
	uint32_t *pSRVR = (uint32_t*)0xE000E014 ; // SysTick Reload Value Register

	uint32_t tick_count = (SYS_CLK/TICK_HZ)-1;

	*pSRVR &= ~(0x00FFFFFF);
	*pSRVR |= tick_count;	// Load tick count in auto reload register

	*pSCSR |= (1<<2);   // set processor clock as clock source
	*pSCSR |= (1<<1);	// Enable Asset to rise interrupt after count down to zero
	*pSCSR |= (1<<0);	// Enable SysTick timer

}

void SysTick_Handler(void) {

}
