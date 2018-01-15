#include "future.h"
#include "timDev.h"
#include <string.h>
#include "tim.h"

#define TimDevGetTimerCount() (htim2.Instance->CNT)

static uint32_t refOffset; 

inline static bool IsTimeBkg(bkgCallback_t *bcb)
{
    return (TimDevGetTimerCount() - bcb->startTime) > bcb->duration;
}

// One per channel
static future_t isrFuture[4];

static const uint16_t ChanToken[4] =
{
    TIM_CHANNEL_1,
    TIM_CHANNEL_2,
    TIM_CHANNEL_3,
    TIM_CHANNEL_4
};


static bkgCallback_t bkgFuture[BACKGROUND_TIMERS];

uint32_t FutureGetRefTime(void)
{
    return (TimDevGetTimerCount()+refOffset)/TICKS_PER_US; // TODO confirm this is a shift not a divide.
}

void FutureSetRefTime(uint32_t ref)
{
    refOffset = ref*TICKS_PER_US - TimDevGetTimerCount();
}

// called in background loop at fastest rate
void FutureBackground(void)
{
    bkgCallback_t *bcb = bkgFuture;
    for(int i=0;i<DIM(bkgFuture);i++, bcb++)
    {
        // if time to fire callback
        if(bcb->callback && IsTimeBkg(bcb))
        {
            uint32_t pri = __get_PRIMASK();
            __set_PRIMASK(1);
            if(bcb->callback)
            {
                uintptr_t tmp = bkgFuture[i].context;
                timerCallback_t tmpCb = bkgFuture[i].callback;
                if (bcb->repeat < 0)
                {
                    
                }
                else if(bcb->repeat)
                {
                    bcb->repeat--;
                }
                
                if(bcb->repeat == 0)
                {
                    // release the object
                    bkgFuture[i].callback = NULL;
                }
                __set_PRIMASK(pri); // reenable interrupts    
                // call callback with the context
                tmpCb(tmp);
            }
            else __set_PRIMASK(pri); // reenable interrupts
        }
    }
}


// Function called specifically by channel that interrupted
void FutureIsr(int chan)
{
    // extract the context and callback
    uintptr_t tmp = isrFuture[chan].context;
    timerCallback_t tmpCb = isrFuture[chan].callback;
    // if callback was still active (no race to terminate)
    if (tmpCb) 
    {
        //HAL_TIM_OC_Stop_IT(&htim2, ChanToken[chan]); No need to stop, bug in HAL stops timer if no OC are enabled
        isrFuture[chan].callback = NULL; // release objec to pool
        tmpCb(tmp); // call function
    }
}


// add a callback to background list
bkgCallback_t* FutureCallbackBackground(uint32_t uSec, uintptr_t context, timerCallback_t callback)
{
    // for all available slots
    for(int i=0;i<DIM(bkgFuture);i++)
    {
        // disable interrupts to test and allocate object
        uint32_t pri = __get_PRIMASK();
        __set_PRIMASK(1);
        // if slot is empty, allocate it.
        if(NULL == bkgFuture[i].callback)
        {
            bkgFuture[i].callback = callback; // allocate slot
            __set_PRIMASK(pri); // reenable interrupts
            bkgFuture[i].context = context;
            bkgFuture[i].startTime = TimDevGetTimerCount();
            bkgFuture[i].duration = uSec;
            return &bkgFuture[i];
        }
        // reenable interupts before cycling to next object
        __set_PRIMASK(pri);
    }
    // if got here, no slots were available, return NULL
    return NULL;
}

// set new time for callback (could actually be sooner)
// delay is relative to current time.
bool DelayCallback(bkgCallback_t *bcb, uint32_t delay)
{
    if (!bcb->callback) return false;
    bcb->startTime = TimDevGetTimerCount();
    bcb->duration = delay;
    return true;
}

// kill potential callback, return true if callback was still active
bool KillCallback(bkgCallback_t *bcb)
{
    timerCallback_t tmp = bcb->callback;
    bcb->callback = NULL;
    return tmp != NULL;
}




future_t* FutureCallbackIsr(uint32_t uSec, uintptr_t context, timerCallback_t callback)
{
    uSec *= TICKS_PER_US;
    // look for a free channel
    for(int i=0;i<DIM(isrFuture);i++)
    {
        uint32_t pri = __get_PRIMASK();
        __set_PRIMASK(1);
        if(NULL == isrFuture[i].callback)
        {
            isrFuture[i].callback = callback;
            *isrFuture[i].hwTarget = TimDevGetTimerCount() + uSec;
            isrFuture[i].context = context;
            __set_PRIMASK(pri);
            HAL_TIM_OC_Start_IT(&htim2, ChanToken[i]);
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
    delay *= TICKS_PER_US;
    __set_PRIMASK(1);
    if(fut->callback)
    {
        uint32_t temp = TimDevGetTimerCount() + delay;
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
    if(fut->callback) 
    {
        fut->callback = NULL;
        __set_PRIMASK(pri);
        return true;
    }
    __set_PRIMASK(pri);
    return false;
}



// Setup the timer2 isr system.
void InitFuture(void)
{
    memset(isrFuture, 0, sizeof(isrFuture));
    memset(bkgFuture, 0, sizeof(bkgFuture));
    
    isrFuture[0].hwTarget = &htim2.Instance->CCR1;
    isrFuture[1].hwTarget = &htim2.Instance->CCR2;
    isrFuture[2].hwTarget = &htim2.Instance->CCR3;
    isrFuture[3].hwTarget = &htim2.Instance->CCR4;
}


// The channel iSR handler
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    switch (htim->Channel)
    {
    case HAL_TIM_ACTIVE_CHANNEL_1:
        FutureIsr(0);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_2:
        FutureIsr(1);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_3:
        FutureIsr(2);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_4:
        FutureIsr(3);
        break;
    default:
        break;
    }
}
