#include "taskCounter.h"
#include <string.h>

#ifdef TEST
#define __Testable 
#else
#define __Testable static
#endif

#define satuinc(val) do{if(!(++(val))) --(val);}while(0)

__Testable TC_Counter_t volatile **counterPointer;
__Testable size_t counters;
__Testable size_t allocated;

static uint32_t misuse;

void TC_InitHandleService(void *space, size_t spaceSize)
{
    memset(space, 0, spaceSize); // clear the space
    counterPointer = space;
    counters = spaceSize / sizeof(counterPointer);
    allocated = 0;
    misuse = 0;
}

tcHandle_t TC_AssociateHandle(TC_Counter_t volatile * myCounter)
{
    if (allocated >= counters) return 0;
    tcHandle_t handle = ++allocated;
    counterPointer[handle-1] = myCounter;
    return handle;
}

void TC_SignalTask(tcHandle_t task)
{
    if (task && task <= allocated)
    {
        // do saturated inc
        TC_Counter_t temp = *counterPointer[task-1] + 1;
        if (temp) *counterPointer[task-1] = temp;
        return;
    }
    satuinc(misuse);
}

TC_Counter_t TC_Test(tcHandle_t task) 
{
    if (task && task <= allocated)
    {
        return *counterPointer[task-1];
    }
    satuinc(misuse);
    return 0;
}

TC_Counter_t TC_TestAndClear(tcHandle_t task)
{
    if (task && task <= allocated)
    {
        TC_Counter_t temp = *counterPointer[task -1];
        *counterPointer[task-1] = 0;
        return temp;
    }
    satuinc(misuse);    
    return 0;
}

uint32_t TC_misuse(void)
{
    uint32_t temp = misuse;
    misuse = 0;
    return temp;
}