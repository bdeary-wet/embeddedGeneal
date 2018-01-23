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
    if(!timerList) return;
    swtBg_t **prev = &timerList;    // points where the previous pointer is stored
    swtBg_t *pend = *prev;      // get the first item in the list
    uint32_t time = GetBackgroundTimer();
    while(pend)
    {
        int removed = 0;
        swtBg_t *next = pend->next;
        
        if(SW_TIMER_PASSED(pend->timer, time))
        {
            if (pend->runCount) // if counting down
            {
                pend->runCount--;
                if (pend->runCount == 0) // if counted to zero, remove from list
                {
                    pend->paused = 2; // stopped
                    *prev = next;     // remove from queue
                    pend->queued = 0; // mark dequeued
                    pend->check = NULL; // cleanup
                    removed = 1;   // mark removed
                }
                else // reset timer for next time
                {
                    SW_TIMER_CYCLE(pend->timer);
                }
            }
            else // else running continuous 
            {
                SW_TIMER_CYCLE(pend->timer);
            }
            // do callback or task call
            if(pend->cb) pend->cb(pend->taskContext);
            else         TS_SignalTask(pend->taskContext);
        }

        if (!removed) // if not removed update prev pointer
        {
            prev = &(pend->next);
        }
        pend = next;
    }
}

void SWT_BackgroundTimerTask(
            swtBg_t *swt, 
            taskHandle_t task, 
            uint32_t timeInMs,
            uint16_t runCount)
{
    swt->taskContext = task;
    swt->cb = NULL;
    swt->runCount = runCount;
    SW_TIMER_SET(swt->timer, timeInMs * BACKGROUND_TICKS_PER_MS, GetBackgroundTimer())
    swt->queued = 1;
    swt->paused = 0;  // future use
    // if user tries to reuse an already queued structure, don't re-queue it
    if(!swt->queued || swt->check != &timerList) 
    {        
        swt->next = timerList;
        timerList = swt;
        swt->check = &timerList;
    }
}

void SWT_BackgroundTimerCallback(
            swtBg_t *swt, 
            timerCallback cb, 
            uint32_t timeInMs,
            uint16_t runCount,
            intptr_t context)
{
    swt->taskContext = context;
    swt->cb = cb;
    swt->runCount = runCount;
    SW_TIMER_SET(swt->timer, (timeInMs * BACKGROUND_TICKS_PER_MS), GetBackgroundTimer())
    swt->queued = 1;
    swt->paused = 0;  // future use
    // if user tries to reuse an already queued structure, don't re-queue it
    if(!swt->queued || swt->check != &timerList)
    {
        swt->next = timerList;
        timerList = swt;
        swt->check = &timerList;
    }
}

void SWT_BackgroundTimersReset(void)
{
    swtBg_t *next = timerList;
    timerList = NULL;
    while(next && next->check == &timerList)
    {
        swtBg_t *this = next->next;
        next->next = NULL;
        next = this;
    }
    return;
}

int SWT_IsTimerActive(swtBg_t *swt) {return swt->queued;}
int SWT_IsTimerPaused(swtBg_t *swt) {return swt->paused == 1;}
int SWT_IsTimerStopped(swtBg_t *swt) {return swt->paused == 2;}
int SWT_IsTimerRunning(swtBg_t *swt) {return swt->paused == 0 && swt->queued;}

// pause the timer by storing current progress
int SWT_PauseTimer(swtBg_t *swt)
{
    swt->paused = 1; 
    // save how far we have advanced
    swt->timer.start =  GetBackgroundTimer() - swt->timer.start;
    return 0;
}

// stop the timer, will start from 0 if restarted
int SWT_StopTimer(swtBg_t *swt) {swt->paused = 2;     return 0;}

// restart from stop or pause
int SWT_RestartTimer(swtBg_t *swt)
{   
// if pause, restore prior progress
    if (swt->paused == 1)
    {
        // restore the prior state
        swt->timer.start = GetBackgroundTimer() - swt->timer.start;
    }
    else // restart from beginning
    {
        swt->timer.start = GetBackgroundTimer();
    }
    swt->paused = 0;
    return 0;
}


// wrapper around whatever background tick we want to use
uint32_t GetBackgroundTimer(void)
{
    extern __IO uint32_t *someCounter;
#ifdef TEST
    (*someCounter)++;
#endif    
    return *someCounter;
}
