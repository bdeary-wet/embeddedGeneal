#include "swtimers.h"
#include "swtimer.h"
#include "gencmdef.h"

typedef struct swtH_s 
{
    timerCallback cb;
    uint32_t *counter;
    swTime32_t timer;
    intptr_t contextTask;
    uint32_t runCount;
} swtHandle_t;

uint32_t userSysTicks;
swtHandle_t *sysTimer;
size_t sysTimers;

int SWT_InitSysTimers(void *space, size_t spaceSize)
{
    sysTimers = spaceSize/sizeof(swtHandle_t);
    sysTimer = (swtHandle_t*)space;
    return sysTimers;
}

static swtHandle_t *getNextSys(void)
{
    swtHandle_t *out = NULL;
    for (int i=0; i < sysTimers || out; i++)
    {
        START_PROTECTED
        if (!sysTimer[i].cb)
        {
            out = &sysTimer[i];
            out->cb = (void*)1;
        }
        END_PROTECTED
    }
    return out;
}

size_t  SWT_sizeofTimer(void)
{
    return sizeof(swtHandle_t);
}

swtHandle_t *SWT_SysTimerCallback(
            timerCallback cb, 
            uint32_t timeInMicroSeconds, 
            uint32_t runCount,
            intptr_t context)
{
    swtHandle_t *swt = getNextSys();
    if (swt)
    {
        swt->contextTask = context;
        SW_TIMER_SET(swt->timer, timeInMicroSeconds * TICKS_PER_USEC, userSysTicks);
        swt->counter = NULL;
        swt->runCount = runCount;
        swt->cb = cb;
    }
    return swt;
}
            
swtHandle_t *SWT_SysTimerTask(
                    taskHandle_t task, 
                    uint32_t timeInMicroSeconds,
                    uint32_t runCount)
{
    swtHandle_t *swt = getNextSys();
    if (swt)
    {
        swt->contextTask = task;
        SW_TIMER_SET(swt->timer, timeInMicroSeconds * TICKS_PER_USEC, userSysTicks);
        swt->counter = NULL;
        swt->runCount = runCount;
    }
    return swt;
}
                    
swtHandle_t *SWT_SysTimerCount(
                    uint32_t *counter,
                    uint32_t timeInMicroSeconds,
                    uint32_t runCount)
{
    swtHandle_t *swt = getNextSys();
    if (swt)
    {
        swt->contextTask = 0;
        SW_TIMER_SET(swt->timer, timeInMicroSeconds * TICKS_PER_USEC, userSysTicks);
        swt->counter = counter;
        swt->runCount = runCount;
    }
    return swt;
}                   

swtHandle_t *SWT_FastTimerCallback(
                    timerCallback cb, 
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    intptr_t context)
                    {return NULL;}                    
                    
swtHandle_t *SWT_FastTimerCount(
                    uint32_t *counter,
                    uint32_t timeInNs, 
                    uint32_t runCount)
                    {return NULL;}