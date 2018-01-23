#include "swtimers.h"
#include "swtimer.h"
#include "gencmdef.h"
#include "taskService.h"
//#include "swtimer_micro.h"


typedef struct swtH_s 
{
    timerCallback cb;       ///< optional callback pointer
    uint32_t *counter;      ///< optional counter
    swTime32_t timer;       ///< timer structure
    intptr_t contextTask;   ///< holds context or task handle
    uint32_t runCount;      ///< 0 for continuous, else number of runs
    uint16_t active;        ///< active flag
    uint16_t spare;
} swtHandle_t;

typedef struct swtFH_s 
{
    timerCallback cb;       ///< optional callback pointer
    uint32_t *counter;      ///< optional counter
    swTime32_t timer;       ///< timer structure
    intptr_t contextTask;   ///< holds context or task handle
    uint32_t runCount;      ///< 0 for continuous, else number of runs
    uint16_t active;        ///< active flag
    uint16_t spare;
} swtFastHandle_t;

typedef enum
{
    FREE_HANDLE = 0,
    SETTING_UP = 1,
    TEAR_DOWN = 2,
    ACTIVE = 3,
} handleState_e;

uint32_t userSysTicks;
swtHandle_t *sysTimer;  // pointer to handle pool
size_t sysTimers;

swtHandle_t *fastTimer;  // pointer to handle pool
size_t fastTimers;

// take memory chunk and use it as handle pool
int SWT_InitSysTimers(void *space, size_t spaceSize)
{
    sysTimers = spaceSize/sizeof(swtHandle_t);
    sysTimer = (swtHandle_t*)space;
    return sysTimers;
}

// just get the next free handle from the pool or return NULL if none.
static swtHandle_t *getNextSys(void)
{
    swtHandle_t *out = NULL;
    for (int i=0; i < sysTimers || out; i++)
    {
        START_PROTECTED
        if (sysTimer[i].active == FREE_HANDLE)
        {
            out = &sysTimer[i];
            out->active = SETTING_UP;
        }
        END_PROTECTED
    }
    return out;
}

// let user know size of a handle at runtime
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
        swt->active = ACTIVE;
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
        swt->cb = NULL;
        swt->active = ACTIVE;
    }
    return swt;
}
                    
swtHandle_t *SWT_SysTimerCount(
                    uint32_t *counter,
                    uint32_t timeInMicroSeconds,
                    uint32_t runCount,
                    taskHandle_t task)
{
    swtHandle_t *swt = getNextSys();
    if (swt)
    {
        swt->contextTask = task;
        SW_TIMER_SET(swt->timer, timeInMicroSeconds * TICKS_PER_USEC, userSysTicks);
        swt->counter = counter;
        swt->runCount = runCount;
        swt->cb = NULL;
        swt->active = ACTIVE;
    }
    return swt;
}                   

// process a handle and return it to the pool if ready
static void sysProcess(swtHandle_t *swt)
{
    if(swt->active == ACTIVE) // only process if active
    {
        if (SW_TIMER_PASSED(swt->timer, userSysTicks))
        {
            // make local copy
            timerCallback cb = swt->cb;
            intptr_t contextTask = swt->contextTask;
            uint32_t *counter = swt->counter;
            // look if countdown
            if (swt->runCount == 1) 
            {
                swt->active = FREE_HANDLE;
            }
            else if (swt->runCount) swt->runCount--;
            // process actions
            if (counter) (*counter)++;
            if (cb)
            {
                cb(contextTask);
            }
            else if(contextTask)
            {
                TS_SignalTask(contextTask);
            }
        }
    }
}

// Call inside the system tick 
void SWT_OnSysTick(void)
{
    swtHandle_t *swt = sysTimer;
    for (int i=0; i<sysTimers; i++)
    {
        sysProcess(swt++);
    }
}


swtFastHandle_t *SWT_FastTimerCallback(
                    timerCallback cb, 
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    intptr_t context)
                    {return NULL;}                    
                    
swtFastHandle_t *SWT_FastTimerCount(
                    uint32_t *counter,
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    taskHandle_t task)
{
return NULL;
}

