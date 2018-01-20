#include "taskServiceClass.h"
#include "gencmdef.h"
#include <string.h>


/// One task entry in the table
typedef struct
{
    genQ_t *q;          // pointer to an optional queue else NULL &q = taskHandle
    taskFunc_f task;    // pointer to the function
    taskCnt_t scheduled; // scheduled counter
} taskTableEntry_t;

/// The current running state of the taskMasterRecord
typedef struct
{
    int16_t runningTask;        // the current running task or -1
    int16_t lowestYielding;     // temporary last 
} taskRunState_t;


typedef struct tmrStruct_s
{
    taskRunState_t runState;
    int16_t nextInTable;            // next available slot in table
    uint8_t ttLen;                  // max dim of jump table
    uint8_t spare;
    taskTableEntry_t ttable[];       // jump table C99
} taskMasterRecord_t;

// return needed size to store expected tasks
size_t TSC_InitGetSize(int expectedTasks)
{
    return sizeof(taskMasterRecord_t) +
           sizeof(taskTableEntry_t) * expectedTasks;
}


int TSC_Init(taskMasterRecord_t **tmrp, uint32_t *allocatedTableSpace, size_t sizeOfTableSpace)
{
    if(sizeOfTableSpace < 
       sizeof(taskMasterRecord_t) + sizeof(taskTableEntry_t)) return 0;
    size_t temp = (sizeOfTableSpace - sizeof(taskMasterRecord_t)) / 
                   sizeof(taskTableEntry_t);
                       
    *tmrp = NULL;               
    if(temp)
    {
        taskMasterRecord_t *tmr = (taskMasterRecord_t*)allocatedTableSpace;
        memset(allocatedTableSpace, 0, sizeOfTableSpace);
        tmr->nextInTable = 0;
        tmr->runState.runningTask = -1;
        tmr->runState.lowestYielding = 0; // 0 can't yield so 0 is good test state
        tmr->ttLen = temp;
        *tmrp = tmr;
    }
    return (int)temp;
}

taskHandle_t TSC_AddTask(taskMasterRecord_t *tmr, taskFunc_f task)
{
    if (tmr->nextInTable >= tmr->ttLen) return TSC_TableFull;
    taskTableEntry_t *te = &(tmr->ttable[tmr->nextInTable]);
    if (tmr->runState.lowestYielding == tmr->nextInTable) 
        tmr->runState.lowestYielding++;
    tmr->nextInTable++;
    te->task = task;
    te->q = NULL;
    te->scheduled = 0;
    return (taskHandle_t)te;
}

taskHandle_t TSC_AddTaskWithQueue(taskMasterRecord_t *tmr, taskFunc_f task, genQ_t *q)
{
    if (tmr->nextInTable >= tmr->ttLen) return TSC_TableFull;
    taskTableEntry_t *te = &(tmr->ttable[tmr->nextInTable]);
    if (tmr->runState.lowestYielding == tmr->nextInTable) 
        tmr->runState.lowestYielding++;
    tmr->nextInTable++;
    te->task = task;
    te->q = q;
    te->scheduled = 0;
    return (taskHandle_t)te;
}

int TSC_SignalTask(taskMasterRecord_t *tmr, taskHandle_t task)
{
    taskTableEntry_t *te = (taskTableEntry_t*)task;
    if (te < &tmr->ttable[0] || te > &tmr->ttable[tmr->nextInTable-1]) return TSC_TaskNotDefined;
    USAT_INC(te->scheduled);
    return TSC_OK;
}

// add to queue
int TSC_Put(taskMasterRecord_t *tmr, taskHandle_t task, void const *val)
{
    taskTableEntry_t *te = (taskTableEntry_t*)task;
    if (te < &tmr->ttable[0] || te > &tmr->ttable[tmr->nextInTable-1]) return TSC_TaskNotDefined;
    USAT_INC(te->scheduled);
    return GenQ_Put( te->q, val);
}

// get from queue
int TSC_Get(taskMasterRecord_t *tmr, void *val)
{
    if (tmr->runState.runningTask < 0) return TSC_IllegalCall;
    return GenQ_Get(tmr->ttable[tmr->runState.runningTask].q, val);
}

// test and clear the current running task schedule counter
int TSC_Test(taskMasterRecord_t *tmr)
{
    if (tmr->runState.runningTask < 0) return TSC_IllegalCall;
    int val = tmr->ttable[tmr->runState.runningTask].scheduled;
    tmr->ttable[tmr->runState.runningTask].scheduled = 0;
    return val;
}

// store the current state and reenter the loop
void TSC_Yield(taskMasterRecord_t *tmr)
{
    if (tmr->runState.runningTask <= 0) return;
    taskRunState_t state = tmr->runState;   // save current
    int inYield = state.lowestYielding != tmr->nextInTable;
    // set new limit on yield
    tmr->runState.lowestYielding = tmr->runState.runningTask; 
    tmr->runState.runningTask = -1;
    // reenter loop
    TSC_DoTasks(tmr);
    // restore state
    tmr->runState = state;
    if (!inYield) state.lowestYielding = tmr->nextInTable;
}

// convenience function to convert taskHandle to queue pointer.
genQ_t *TSC_GetQueueObject(taskMasterRecord_t *tmr, taskHandle_t task)
{
    taskTableEntry_t *te = (taskTableEntry_t*)task;
    if (te < &tmr->ttable[0] || te > &tmr->ttable[tmr->nextInTable-1]) return NULL;
    return te->q;
}

// The prioritized task loop
// run until no tasks need scheduling, restarting after every success
void TSC_DoTasks(taskMasterRecord_t *tmr)
{
    int again = 1;
    while (again)
    {
        again = 0;
        taskTableEntry_t *te = tmr->ttable;
        for (tmr->runState.runningTask = 0;
             tmr->runState.runningTask < tmr->runState.lowestYielding;
             tmr->runState.runningTask++, te++)
        {
           if (te->scheduled)
           {
               te->scheduled = 0;
               if (te->task) te->task();
               again = 1;
               break;
           }
        }
    }
}
