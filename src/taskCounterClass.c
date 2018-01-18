#include "taskCounterClass.h"
#include <string.h>



tcHandle_t TCC_AssociateHandle(tcc_t *tcc, tcCounter_t volatile * myCounter,
                               intptr_t context)
{
    tcc->counter = myCounter;
    tcc->context = context;
    return (tcHandle_t)tcc;
}

void TCC_SignalUp(tcHandle_t task)
{
    if (task)
    {
        // do saturated inc
        tcCounter_t temp = *(tcc_t*)task->counter + 1;
        if (temp) *(tcc_t*)task->counter = temp;
        return;
    }
}

void TCC_SignalDown(tcHandle_t task)
{
    // do saturated dec
    if (task && *(tcc_t*)task->counter)
    {
        *(tcc_t*)task->counter--;
        return;
    }
}


tcCounter_t TCC_Test(tcHandle_t task) 
{
    if (task)
    {
        return *(tcc_t*)task->counter;
    }
    return 0;
}

tcCounter_t TC_TestAndClear(tcHandle_t task)
{
    if (task)
    {
        tcCounter_t temp = *(tcc_t*)task->counter;
        *(tcc_t*)task->counter = 0;
        return temp;
    }
    return 0;
}