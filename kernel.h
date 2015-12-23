#ifndef KERNEL_TIME_SHARING_H_
#define KERNEL_TIME_SHARING_H_

void SchedulerInit(void);
void TaskInit(void (*pFun)(void),uint8_t TaskId);
void RunTask(uint8_t TaskId);

#endif
