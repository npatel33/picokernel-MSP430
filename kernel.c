#include<msp430.h>
#include<in430.h>
//#include<stdint.h>
#include "kernel.h"

//task control block(tcb) base address for task id 0.
#define TCB_BASE 0x03c0

//set task size to 64 bytes
#define TASK_SIZE 64

//save context  
#define SAVE_CONTEXT()    asm("push r0 \n"\
				  "push r2 \n"\
				  "push r4 \n"\
			      "push r5 \n"\
			      "push r6 \n"\
			      "push r7 \n"\
			      "push r8 \n"\
			      "push r9 \n"\
			      "push r10 \n"\
			      "push r11\n"\
			      "push r12 \n"\
			      "push r13 \n"\
			      "push r14 \n"\
			      "push r15\n")

//restore context
#define RESTORE_CONTEXT() asm("pop r15 \n"\
			      "pop r14 \n"\
			      "pop r13 \n"\
			      "pop r12 \n"\
			      "pop r11 \n"\
			      "pop r10 \n"\
			      "pop r9 \n"\
			      "pop r8 \n"\
			      "pop r7 \n"\
			      "pop r6 \n"\
			      "pop r5 \n"\
			      "pop r4 \n"\
			      "pop r2 \n"\
				  "pop r0 \n")
			   

#ifndef MAX_TASK
#define MAX_TASK 2
#endif

uint8_t CurrentTask=0; //track current running task
uint16_t Counter=0;

struct sTask Task[MAX_TASK];


void SchedulerInit(void){
	

	__disable_interrupt();			//disable global interrupt
	__nop();

	TACTL|=TACLR + MC_0;	//clear/reset and stop the timer A
	TACTL |= TASSEL_2 + ID_2;//Setting SMCLK as clock source
	TACCTL0 |= CCIE;	//enable compare interrupt
	TACCTL0 &= ~(CCIFG);	//clear compare interrupt flag
	TAR=0;			//Timer value set to 0
	TACCR0=60;		//Compare register value for 10 ms period
	//TACCR0=3000; //500ms
	//eint();			//Enable global interrupt

	TACTL |= MC_1;		//Start timer A in up mode


}


void SchedulerISR(void) __attribute__((interrupt (TIMER0_A0_VECTOR),naked));
/* scheduler with round robin algorithm */
void SchedulerISR(void){    //perform context switching
	

	//P1OUT ^= (1<<0);

	//P1OUT |= (1<<7);
	SAVE_CONTEXT(); //save current task context
	

	asm("mov R1,%[dest]":[dest]"=r"(Task[CurrentTask].prevStack)::"R1");
	
	CurrentTask++; //point to next task
	if(CurrentTask>=MAX_TASK){
		CurrentTask=0;
	}

	//load next task's stack pointer
	asm("mov %[src],R1"::[src]"r"(Task[CurrentTask].prevStack):"R1");
	
	//P1OUT &= ~(1<<7);
	RESTORE_CONTEXT();//restore next task's context

}

void TaskInit(void (*pFun)(void),uint16_t taskId){  //initialize tasks and their stack frame
	
	/*
	 * Store SP value to temp variable
	 */
	register uint16_t temp asm("R7");
	temp = __get_SP_register();

	/*
	 * Store argument in other registers because compiler pushes original
	 * argument registers R12 and R13 into stack after entering into function.
	 * we are manipulating stack for initialization so stack alignment can be lost
	 * and original R12 and R13 values will be inaccessible.
	 */
	register uint16_t id asm("R8") = (uint16_t)taskId;
	register uint16_t add asm("R9") = (uint16_t) pFun;
	register uint16_t loop asm("R10");

	/*
	 * Initialize task structure
	 */
	Task[taskId].id=id;
	Task[taskId].address = add;
	Task[taskId].TCBLoc= (uint16_t)(TCB_BASE-(TASK_SIZE*id));
	Task[taskId].prevStack = 0x3F6;

	/*
	 * Change Stack pointer to task TCB
	 */
	asm volatile("mov %[src],R1"::[src]"r"(Task[id].TCBLoc):"R1");


	/*
	 * Push program Counter,Status register and more 12 registers
	 */
	asm volatile("push %[src]"::[src]"r"(add):"R1");
	asm volatile("push #0x0008":::"R1");
	//initialize task stack frame with zero
	for(loop=0;loop<12;loop++){
	
		asm("push #0x0000");

	}
	
	/*
	 * task TCB current stack pointer value is saved
	 */
	asm volatile("mov R1,%[dst]":[dst]"=r"(Task[id].prevStack)::); //load sp

	/*
	 * Back to previous stack value
	 */
	__set_SP_register(temp);
}

void RunTask(uint8_t taskId){ //run a task


	
	CurrentTask=taskId; //set running task

	SchedulerInit(); //Initialize timer

    asm("mov %[src],r1"::[src]"m"(Task[taskId].prevStack)); //load sp

    RESTORE_CONTEXT(); //pop out task

}

