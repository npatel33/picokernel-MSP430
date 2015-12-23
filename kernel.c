#include<msp430g2553.h>
#include<stdint.h>
#include "kernel_time_sharing.h"
#include <legacymsp430.h>

//task control block(tcb) base address for task id 0.
#define TCB_BASE 0x03c0

//set task size to 64 bytes
#define TASK_SIZE 64

//save context  
#define SAVE_CONTEXT()    asm("pop r14 \n"\
			      "pop r15 \n"\
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
			      "reti \n")
			   
//#ifdef DefineMaxTask(MAX_TASK,Val) {uint8_t MAX_TASK;MAX_TASK=Val;}
#ifndef MAX_TASK
#define MAX_TASK 2
#endif

uint8_t CurrentTask=0; //track current running task
uint16_t Counter=0;

struct sTask{//Task Structure

	uint16_t Id;		//Task id
	uint16_t Address;	//Task Address
	uint16_t PrevStack;	//Task previous SP position
	uint16_t TCBLoc;	//TCB location

}Task[MAX_TASK];


void SchedulerInit(void){
	
	dint();			//disable global interrupt

	TACTL|=TACLR + MC_0;	//clear/reset and stop the timer A
	TACTL |= TASSEL_2 ;//Setting SMCLK as clock source
	TACCTL0 |= CCIE;	//enable compare interrupt
	TACCTL0 &= ~(CCIFG);	//clear compare interrupt flag
	TAR=0;			//Timer value set to 0
	TACCR0=100;		//Compare register value for 1 ms period
	//eint();			//Enable global interrupt
	TACTL |= MC_1;		//Start timer A in up mode

}
interrupt (TIMER0_A0_VECTOR) SchedulerISR(void){    //perform context switching
	
	P1OUT |= (1<<7);
	SAVE_CONTEXT(); //save current task context
	
	asm("mov r1,%[dest]":[dest]"=r"(Task[CurrentTask].PrevStack)::"r1");
	
	CurrentTask++; //point to next task
	if(CurrentTask>=MAX_TASK){
		CurrentTask=0;
	}

	//load next task's stack pointer
	asm("mov %[src],r1"::[src]"r"(Task[CurrentTask].PrevStack):"r1");
	
	P1OUT &= ~(1<<7);
	RESTORE_CONTEXT();//restore next task's context


}	

void TaskInit(void (*pFun)(void),uint8_t TaskId){  //initialize tasks and their stack frame
	
	uint8_t Loop;
	uint16_t temp;

	Task[TaskId].Id=TaskId;
	Task[TaskId].Address = (uint16_t)pFun;
	Task[TaskId].TCBLoc= (uint16_t)(TCB_BASE-(TASK_SIZE*TaskId));
	
	asm("mov r1,%[dst]":[dst]"=r"(temp)::"r1");
	asm("mov %[src],r1"::[src]"r"(Task[TaskId].TCBLoc):"r1");
	asm("push %[src]"::[src]"r"(Task[TaskId].Address));
	asm("push #0x0008");

	

	for(Loop=0;Loop<12;Loop++){
	
		asm("push #0x0000");

	}
	
	asm("mov r1,%[dst]":[dst]"=r"(Task[TaskId].PrevStack)::"r1"); //load sp
	asm("mov %[src],r1"::[src]"r"(temp):"r1");
	
}

void RunTask(uint8_t TaskId){ //run a task


	
	CurrentTask=TaskId; //set running task
	SchedulerInit();
        asm("mov %[src],r1"::[src]"r"(Task[TaskId].PrevStack)); //load sp
	RESTORE_CONTEXT(); //pop out task
}

