#ifndef KERNEL_H_
#define KERNEL_H_

void SchedulerInit(void);
void TaskInit(void (*pFun)(void),uint8_t TaskId);
void RunTask(uint8_t TaskId);

#endif
