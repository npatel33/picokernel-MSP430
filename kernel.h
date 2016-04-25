#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>

void SchedulerInit(void);
void TaskInit(void (*pFun)(void),uint16_t TaskId);
void RunTask(uint8_t TaskId);


struct sTask{//Task Structure

	uint16_t id;		//Task id
	uint16_t address;	//Task Address
	uint16_t prevStack;	//Task previous SP position
	uint16_t TCBLoc;	//TCB location

};

#endif
