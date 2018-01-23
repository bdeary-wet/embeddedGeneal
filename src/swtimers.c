#include "swtimers.h"
#include "swtimer.h"
#include "gencmdef.h"
#include "taskService.h"
#include "main.h"
#include "tim.h"
#include <string.h>
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

//----------------------------------------------------------------------------//

// Fast timer area, high priority interrupt will queue a callback for the 
// pendsv isr service to process.

static inline uint16_t getFastCounter(void)
{
    return htim3.Instance->CNT;
}

static objFuncQueue_t cbBuf[32];
static genQ_t pendsvQueue;

objFuncQueue_t *cbPending[4];

// This is the PendSv service routine, justs pulls callbacks out of the 
// queue and calls them.  The PendSv is assumed to run at near the lowest
// priority.

uint32_t rependSvCnt;
uint32_t resetCallbackCnt;
void SWT_PendService(void)
{
    objFuncQueue_t entry;
    // run for as long as there are things in queue
    while (!GenQ_Get(&pendsvQueue, &entry))
    {
        // clear pending as long as we have data in queue, note the re-pend
        if (READ_BIT(SCB->ICSR, SCB_ICSR_PENDSVSET_Msk))
        {
            SET_BIT(SCB->ICSR, SCB_ICSR_PENDSVCLR_Msk);
            ++rependSvCnt;
        }
        // if it looks valid, call it
        if(entry.cb) entry.cb(entry.obj);
        else ++resetCallbackCnt;
    }
}





static const uint16_t ChanToken[4] =
{
    TIM_CHANNEL_1,
    TIM_CHANNEL_2,
    TIM_CHANNEL_3,
    TIM_CHANNEL_4
};

future_t isrFuture[4];

void SWT_FastInit(void)
{
    ARRAY_TO_Q(cbBuf, pendsvQueue);
    memset(isrFuture, 0, sizeof(isrFuture));

    isrFuture[0].hwTarget = &htim3.Instance->CCR1;
    isrFuture[1].hwTarget = &htim3.Instance->CCR2;
    isrFuture[2].hwTarget = &htim3.Instance->CCR3;
    isrFuture[3].hwTarget = &htim3.Instance->CCR4;    
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

// On interrupt, copy the callback onto the pending queue and pend the pendsv
void SWT_ChanIsr(int chan)
{
    if(isrFuture[chan].cbObj.cb)
    {
        GenQ_Put(&pendsvQueue, &isrFuture[chan].cbObj);
        isrFuture[chan].cbObj.cb = NULL;
        SET_BIT(SCB->ICSR, SCB_ICSR_PENDSVSET_Msk);
    }
}


future_t* FutureCallbackIsr(uint32_t uSec, uintptr_t context, objFunc_f callback)
{
    uSec *= T3_COUNTS_PER_USEC;
    // look for a free channel
    for(int i=0;i<DIM(isrFuture);i++)
    {
        uint32_t pri = __get_PRIMASK();
        __set_PRIMASK(1);
        if(NULL == isrFuture[i].cbObj.cb)
        {
            isrFuture[i].cbObj.cb = callback;
            *isrFuture[i].hwTarget = getFastCounter() + uSec;
            isrFuture[i].cbObj.obj = (void*)context;
            __set_PRIMASK(pri);
            HAL_TIM_OC_Start_IT(&htim3, ChanToken[i]);
            isrFuture[i].target = *isrFuture[i].hwTarget; // book keeping
            return &isrFuture[i];
        }
        __set_PRIMASK(pri);
    }
    return NULL;    
}


// Delay exiting callback set time from now
bool FutureReset(future_t *fut, uint32_t delay)
{
    uint32_t pri = __get_PRIMASK();
    delay *= T3_COUNTS_PER_USEC;
    __set_PRIMASK(1);
    if(fut->cbObj.cb)
    {
        uint32_t temp = getFastCounter() + delay;
        *(fut->hwTarget) = temp;
        __set_PRIMASK(pri);
        fut->target = temp;  // redundent in background , book keeping in isr
        return true;
    }
    __set_PRIMASK(pri);
    return false;
    
}
// Kill the future callback
bool FutureKill(future_t *fut)
{
    uint32_t pri = __get_PRIMASK();
    __set_PRIMASK(1);
    if(fut->cbObj.cb) 
    {
        fut->cbObj.cb = NULL;
        __set_PRIMASK(pri);
        return true;
    }
    __set_PRIMASK(pri);
    return false;
}






