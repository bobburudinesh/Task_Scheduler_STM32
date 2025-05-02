/*
 * main.h
 *
 *  Created on: May 2, 2025
 *      Author: Dinesh Bobburu
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include <stdio.h>


#define SRAM_START	0x20000000U					 // SRAM Start
#define SRAM_SIZE	((128) * (1024))
#define SRAM_END	((SRAM_START) + (SRAM_SIZE)) // SRAM_END

#define TASK_STACK_SIZE		1024U		// Task stack size
#define SCHEDULER_STACK_SIZE	1024U	// Scheduler stack size

#define MAX_TASKS	5	// Number of Tasks

#define	TASK_IDLE_STACK_START	SRAM_END	// Idle task stack start
#define TASK_1_STACK_START	(SRAM_END - (1 * TASK_STACK_SIZE))	//Task1 stack start
#define TASK_2_STACK_START	(SRAM_END - (2 * TASK_STACK_SIZE))	//Task2 stack start
#define TASK_3_STACK_START	(SRAM_END - (3 * TASK_STACK_SIZE))	//Task3 stack start
#define TASK_4_STACK_START	(SRAM_END - (4 * TASK_STACK_SIZE))	//Task4 stack start
#define SCHEDULER_STACK_START	(SRAM_END - (5 * TASK_STACK_SIZE))	//Scheduler stack start

#define XPSR_DEFAULT	0x01000000U  // Default value of XPSR to be stored during task creation
#define EXEC_RETURN	0xFFFFFFFD  //Exit return value to be stored in LR used for triggering exception exit

#define SYS_CLK		16000000U
#define TICK_HZ		1000U

#define INTERRUPT_ENABLE() do{__asm volatile("MOV R0,#0x0"); __asm volatile("MSR PRIMASK, R0");}while(0)
#define INTERRUPT_DISABLE() do{__asm volatile("MOV R0,#0x1"); __asm volatile("MSR PRIMASK, R0");}while(0)



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

void task_Delay(uint32_t	delay_in_ticks);


__attribute__((naked))void scheduler_Stack_Init(uint32_t stack_start);
__attribute((naked))void switch_sp_to_psp(void);

typedef struct {
	uint32_t	top_Of_Stack;		// Current top of the stack, latest PSP value
	uint32_t	block_count;		// The delay upto which task is blocked
	Task_States_t	task_State;				// current state of the task, of it is ready or blocked.
	void	    (*task_Handler)(void);	// Task handler for this task
}TCB_t;	// Task Control Block Data Structure


#endif /* MAIN_H_ */
