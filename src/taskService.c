#include "taskService.h"
#include "generalDef.h"


typedef struct
{
    taskFunc_f task;
    genQ_t *q;
    uint16_t scheduled; // scheduled counter
} taskTableEntry_t;


// return needsize to store expected tasks
size_t TS_InitGetSize(int expectedTasks)
{
    return expectedTasks * sizeof(taskTableEntry_t);
}

static taskTableEntry_t *ttable;
static uint8_t ttLen;
static int16_t nextInTable = -1;
static int16_t runningTask = -1;

int TS_Init(uint32_t *allocatedTableSpace, size_t sizeOfTableSpace)
{
    ttable = (taskTableEntry_t*)allocatedTableSpace;
    nextInTable = 0;
    runningTask = -1;
    ttLen = sizeOfTableSpace/sizeof(taskTableEntry_t);
    return(ttLen);
}

int TS_AddTask(taskFunc_f task)
{
    if (nextInTable >= ttLen) return TS_TableFull;
    if (nextInTable < 0 || !ttable) return TS_NoTable;
    ttable[nextInTable].task = task;
    ttable[nextInTable].q = NULL;
    ttable[nextInTable].scheduled = 0;
    return nextInTable++;    
}


int TS_AddTaskWithQueue(taskFunc_f task, genQ_t *q)
{
    if (nextInTable >= ttLen) return TS_TableFull;
    if (nextInTable < 0 || !ttable) return TS_NoTable;
    ttable[nextInTable].task = task;
    ttable[nextInTable].q = q;
    ttable[nextInTable].scheduled = 0;
    return nextInTable++;     
}


int TS_SignalTask(uint8_t taskNo)
{
    if (taskNo >= nextInTable) return TS_TaskNotDefined;
    ttable[taskNo].scheduled++;
}


int TS_Put(uint8_t taskNo, void const *val)
{
    if (taskNo >= nextInTable) return TS_TaskNotDefined;
    ttable[taskNo].scheduled++;
    return GenQ_Put(ttable[taskNo].q, val);
}

int TS_Get(void *val)
{
    if (runningTask < 0) return TS_IllegalCall;
    return GenQ_Get(ttable[runningTask].q, val);
}

int TS_Test(void)
{
    if (runningTask < 0) return TS_IllegalCall;
    int val = ttable[runningTask].scheduled;
    ttable[runningTask].scheduled = 0;
    return val;
}


genQ_t *TS_GetQueueObjectOutside(uint8_t taskNo)
{
    if (taskNo >= nextInTable) return NULL;
    return ttable[taskNo].q;
}
