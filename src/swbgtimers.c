#include "swbgtimers.h"

static swtBg_t *timerList;

// Do the entire list once, process any timers that have reached or passed match 
void SWT_Background(void)
{
    swtBg_t *pend = timerList;
    swtBg_t *next;
    swtBg_t **prev = &timerList;
    uint32_t time = GetBackgroundTimer();
    while(pend)
    {
        next = pend->next;
        if(SW_TIMER_PASSED(pend->timer, time))
        {
            if (pend->runCount)
            if (pend->runCount == 1)
            {
                *prev = next;
                pend->next = NULL;
            }
            else 
            {
                SW_TIMER_CYCLE(pend->timer);
            }
            
            if(pend->cb) pend->cb(pend->taskContext);
            else         TS_SignalTask(pend->taskContext);
        }
        pend = next;
    }
}

void SWT_BackgroundTimerTask(
            swtBg_t *swt, 
            taskHandle_t task, 
            uint32_t timeInMs,
            uint32_t runCount)
{
    swt->taskContext = task;
    swt->cb = NULL;
    swt->runCount = runCount;
    SW_TIMER_SET(swt->timer, timeInMs * BACKGROUND_TICKS_PER_MS, GetBackgroundTimer())
    swt->next = timerList;
    timerList = swt;
}

void SWT_BackgroundTimerCallback(
            swtBg_t *swt, 
            timerCallback cb, 
            uint32_t timeInMs,
            uint32_t runCount,
            intptr_t context)
{
    swt->taskContext = context;
    swt->cb = cb;
    swt->runCount = runCount;
    SW_TIMER_SET(swt->timer, (timeInMs * BACKGROUND_TICKS_PER_MS), GetBackgroundTimer())
    swt->next = timerList;
    timerList = swt;
}

void SWT_BackgroundTimersReset(void)
{
    swtBg_t *next = timerList;
    timerList = NULL;
    while(next)
    {
        swtBg_t *this = next->next;
        next->next = NULL;
        next = this;
    }
    return;
}

int SWT_IsTimerActive(swtBg_t *swt) {return swt->next != NULL;}
            
uint32_t GetBackgroundTimer(void)
{
    extern __IO uint32_t uwTick;
#ifdef TEST
    ++uwTick;
#endif    
    return uwTick;
}
