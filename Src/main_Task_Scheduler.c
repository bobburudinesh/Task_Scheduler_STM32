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

#define INTERRUPT_ENABLE() do{__asm volatile("MOV R0,0x0"); __asm volatile("MSR PRIMASK, R0");}while(0)
#define INTERRUPT_DISABLE() do{__asm volatile("MOV R0,0x1"); __asm volatile("MSR PRIMASK, R0");}while(0)

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
void pend_PendSV(void);
void unblock_tasks(void);
void increment_Tick(void);
void get_next_task_to_run(void);

__attribute__((naked))void PendSV_Handler(void);
__attribute__((naked))void scheduler_Stack_Init(uint32_t stack_start);
__attribute((naked))void switch_sp_to_psp(void);

typedef struct {
	uint32_t	top_Of_Stack;		// Current top of the stack, latest PSP value
	uint32_t	block_count;		// The delay upto which task is blocked
	Task_States_t	task_State;				// current state of the task, of it is ready or blocked.
	void	    (*task_Handler)(void);	// Task handler for this task
}TCB_t;	// Task Control Block Data Structure

TCB_t user_Tasks[MAX_TASKS];	// These are the user tasks
uint8_t current_Task = 1;	// This variable holds current running task id
uint32_t global_Tick_Count = 0; // Global tick count to keep track of time elapsed using systick interupt
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

	user_Tasks[0].top_Of_Stack = (uint32_t)TASK_IDLE_STACK_START;
	user_Tasks[1].top_Of_Stack = (uint32_t)TASK_1_STACK_START;
	user_Tasks[2].top_Of_Stack = (uint32_t)TASK_2_STACK_START;
	user_Tasks[3].top_Of_Stack = (uint32_t)TASK_3_STACK_START;
	user_Tasks[4].top_Of_Stack = (uint32_t)TASK_4_STACK_START;

	for(int i = 0; i < MAX_TASKS; i++) {
		uint32_t *pPSP = (uint32_t*)user_Tasks[i].top_Of_Stack;
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
		user_Tasks[i].top_Of_Stack = (uint32_t)pPSP;	// Save the value of top of stack of that task(PSP)
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
	increment_Tick();	// increment global tick count
	unblock_tasks();	// check if there are any tasks to be unblocked once their delay count is completed.
	pend_PendSV();	// pend pendSV

}

void increment_Tick(void) {
	global_Tick_Count++;
}
void task_Delay(uint32_t	delay_in_ticks){
	INTERRUPT_DISABLE();	// disable the interrupts
	user_Tasks[current_Task].block_count = global_Tick_Count+delay_in_ticks; // task will be blocked for (global_Tick_Count+delay_in_ticks) ticks
	user_Tasks[current_Task].task_State = blocked;

	INTERRUPT_ENABLE();		// enable the interrupts
}

void pend_PendSV(void) {
	uint32_t *pICSR = (uint32_t*)0xE000ED04;
	*pICSR |= (1<<28); // Pend the pend SV
}

void unblock_tasks(void) {
	for(uint8_t i = 1; i<MAX_TASKS; i++) {
		if(user_Tasks[i].task_State != ready || user_Tasks[i].task_State != running) {
			if(global_Tick_Count == user_Tasks[i].block_count) {
				user_Tasks[i].task_State = ready;
			}
		}
	}
}

void get_next_task_to_run(void) {
	Task_States_t state;
	for(int i = 0; i< MAX_TASKS; i++) {
		current_Task++;
		current_Task = current_Task % MAX_TASKS;
		state = user_Tasks[i].task_State;
		if(state == ready && (current_Task != 0)){
			break;
		}
	}
	if(state == blocked) {
		current_Task = 0;
	}
}

__attribute__((naked))void PendSV_Handler(void) {
	// Get current running task PSP
	__asm volatile("PUSH {LR}");
	__asm volatile("MRS R0, PSP");
	// Push R4-R11 to the task stack to save context of this task
	__asm volatile("STMDB R0!,{R4-R11}");
	// Save this PSP to task's TCB top of stack variable
	__asm volatile("BL save_current_task_psp");

	// get the next task to run
	__asm volatile("BL get_next_task_to_run");
	// Get the PSP of this task
	__asm volatile("BL get_current_task_psp");
	// Restore the context of this new task
	__asm volatile("LDMIA R0!, {R4-R11}");
	// Save this task stack pointer value to PSP
	__asm volatile("BL save_current_task_psp");

	__asm volatile("POP {LR}");
	__asm volatile("BX LR");
}

// Stack pointer related functions START

__attribute__((naked))void scheduler_Stack_Init(uint32_t stack_start) {
	__asm volatile("MSR MSP, %0": :"r"(stack_start));
	__asm volatile("BX LR");
}

uint32_t get_current_task_psp(void) {
	return user_Tasks[current_Task].top_Of_Stack;
}
__attribute((naked))void switch_sp_to_psp(void) {
	__asm volatile("PUSH {LR}");
	__asm volatile("BL get_current_task_psp");
	__asm volatile("MSR PSP, R0");
	__asm volatile("MOV R0, #0x2");
	__asm volatile("MSR CONTROL, R0");
	__asm volatile("POP {LR}");
	__asm volatile("BX LR");
}

void save_current_task_psp(uint32_t current_stack_position) {
	user_Tasks[current_Task].top_Of_Stack = current_stack_position;
}



// Stack pointer related functions END
