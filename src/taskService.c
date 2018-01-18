#include "taskService.h"
#include "taskServiceClass.h"

#ifdef TEST
taskMasterRecord_t *tmr;
#else
static taskMasterRecord_t *tmr;
#endif

void TS_Background(void)
{
    TSC_DoTasks(tmr);
}

int TS_Init(uint32_t *allocatedTableSpace, size_t sizeOfTableSpace)
{
    tmr = NULL;
    return TSC_Init(&tmr, allocatedTableSpace, sizeOfTableSpace);

}

// returns task number
taskHandle_t TS_AddTask(taskFunc_f task)
{
    return TSC_AddTask(tmr, task);
}

taskHandle_t TS_AddTaskWithQueue(taskFunc_f task, genQ_t *q)
{
    return TSC_AddTaskWithQueue(tmr, task, q);
}

int TS_SignalTask(taskHandle_t task)
{
    return TSC_SignalTask(tmr, task);
}

int TS_Put(taskHandle_t task, void const *val)
{
    return TSC_Put(tmr, task ,val);
}

int TS_Get( void *val)
{
    return TSC_Get(tmr, val);
}

int TS_Test(void)
{
    return TSC_Test(tmr);
}

void TS_Yield(void)
{
    TSC_Yield(tmr);
}

genQ_t *TS_GetQueueObject(uint8_t taskNo)
{
    return TSC_GetQueueObject(tmr, taskNo);
}

size_t TS_InitGetSize(int expectedTasks)
{
    return TSC_InitGetSize(expectedTasks);
}    