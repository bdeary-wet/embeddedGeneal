#include "slimos.h"
#include "gencmddef.h"


static taskWrapper_t *wrapper[OS_TASKS];
static voidFunc_f taskTable[OS_TASKS];
uint8_t nextTask = 0;


// ------------------------------------------------- 
// Service functions
// -------------------------------------------------

static slimos_t *os;
static void switchTasks(void)
{
    
    
}

OS_RetCode_t OS_AddTask(voidFunc_f task)
{
    if (nextTask < OS_TASKS)
    {
        wrapper[nextTask] = NULL;
        taskTable[nextTask++] = task;
    }
    return OS_InternalError;
    
}

OS_RetCode_t OS_AddTaskWithQueue(voidFunc_f task, uintptr_t *qBuf, size_t qLen)
{
    if (nextTask < OS_TASKS)
    {
        wrapper[nextTask] = 
        taskTable[nextTask++] = task;
    }
    return OS_InternalError;
}


OS_RetCode_t OS_EnableTask(uint8_t taskNo)
{
    if (taskNo > OS_TASKS) return OS_BadTask;
    if (os->taskTable[taskNo] == NULL) return OS_NoTaskDefined;
    os->disable_mask &= ~(1 << taskNo);
    return OS_Ok;
}


OS_RetCode_t OS_DisableTask(uint8_t taskNo)
{
    if (taskNo > OS_TASKS) return OS_BadTask;
    if (os->taskTable[taskNo] == NULL) return OS_NoTaskDefined;
    os->disable_mask |= (1 << taskNo);
    return OS_Ok;
}


OS_RetCode_t OS_TriggerTask(uint8_t taskNo)
{
    if (taskNo > OS_TASKS) return OS_BadTask;
    if (os->taskTable[taskNo] == NULL) return OS_NoTaskDefined;
    os->pendingMask |= (1 << taskNo) & ~os->disable_mask;
    START_PROTECTED
    if(taskNo < os->currentTask) switchTasks();
    END_PROTECTED
    return OS_Ok;    
}


OS_RetCode_t OS_TriggerTaskQueue(uint8_t taskNo, uintptr_t val)
{
    if (taskNo > OS_TASKS) return OS_BadTask;
    if (os->taskTable[taskNo] == NULL) return OS_NoTaskDefined;
    if (os->
}


OS_RetCode_t OS_GetQueue(uintptr_t *val)
{
    
    
}


int OS_GetTaskNo(void)
{
    
    
}