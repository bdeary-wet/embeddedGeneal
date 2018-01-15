#ifndef _TASKSERVICE_H
#define _TASKSERVICE_H
#include <stdint.h>
#include <stddef.h>
#include "generalQueue.h"

typedef void (*taskFunc_f)(void);

typedef enum
{
    TS_OK,
    
} TS_ErrorCodes_e;

TS_ErrorCodes_e TS_InitService(int expectedTasks, uint32_t *space, size_t spaceLen);

int TS_AddTask(taskFunc_f task);

int TS_AddTaskWithQueue(taskFunc_f task, genQ_t *q);







#endif // _TASKSERVICE_H
