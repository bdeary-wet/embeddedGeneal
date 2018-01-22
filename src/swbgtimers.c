#include "swbgtimers.h"
#include "taskService.h"

#ifdef TEST
swtBg_t *timerList;
#else
static swtBg_t *timerList;
#endif    

// Do the entire list once, process any timers that have reached or passed match 
void SWT_Background(void)
{
    swtBg_t **prev = &timerList;
    swtBg_t *pend = *prev;
    uint32_t time = GetBackgroundTimer();
    while(pend)
    {
        int remove = 0;
        if(SW_TIMER_PASSED(pend->timer, time))
        {
            if (pend->runCount) // if counting down
            {
                pend->runCount--;
                if (pend->runCount == 0) // if counted to zero, remove from list
                {
                    remove = 1;
                }
                else // reset timer for next time
                {
                    SW_TIMER_CYCLE(pend->timer);
                }
            }
            else // else running continious 
            {
                SW_TIMER_CYCLE(pend->timer);
            }
            // do callback or task call
            if(pend->cb) pend->cb(pend->taskContext);
            else         TS_SignalTask(pend->taskContext);
        }

        if (remove)
        {
            pend = pend->next;  // next in list
            *prev = &(pend->next); // upate prev's pointer
        }
        else
        {
            prev = &(pend->next);
            pend = pend->next;
        }
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
