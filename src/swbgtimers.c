#include "swbgtimers.h"
//#include "taskService.h"

//#ifdef TEST
swtBg_t *timerList;

//#else
//static swtBg_t *timerList;

//#endif    

uint32_t lastBgTime;
uint32_t maxBgUpdate;

#if defined( USE_GENDEF ) || defined( TEST )
extern intptr_t TS_SignalTask(intptr_t task);
static void sigWrapper(intptr_t task){TS_SignalTask(task);}
static objFunc_f taskFunc = sigWrapper;
#else
static objFunc_f taskFunc;
#endif


void SWBG_SetTaskCaller(objFunc_f userTaskFunc)
{
    taskFunc = userTaskFunc;
}

// Do the entire list once, process any timers that have reached or passed match 
void SWT_Background(void)
{
    // Do some runtime stats by keeping tract of the worst delay
    uint32_t time = GetBackgroundTimer();
    uint32_t delta = time - lastBgTime;
    if(!delta) return;  // unexpected tick
    // keep track of worst delay
    lastBgTime = time;
    if(delta > maxBgUpdate) maxBgUpdate = delta;    

    swtBg_t **prev = &timerList;    // points where the previous pointer is stored
    swtBg_t *pend = *prev;      // get the first item in the list

    while(pend)
    {
        int removed = 0;
        swtBg_t *next = pend->next;
        
        if(!pend->paused && SW_TIMER_PASSED(pend->timer, time))
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
            if(pend->cbObj.cb) pend->cbObj.cb(pend->cbObj.obj);
            else if (taskFunc) taskFunc(pend->cbObj.obj);
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
            intptr_t task, 
            uint32_t timeInMs,
            uint16_t runCount)
{
    swt->cbObj.obj = task;
    swt->cbObj.cb = NULL;
    swt->runCount = runCount;
    SW_TIMER_SET(swt->timer, timeInMs * BACKGROUND_TICKS_PER_MS, GetBackgroundTimer());
    swt->paused = 0;  // future use
    // if user tries to reuse an already queued structure, don't re-queue it
    if(!swt->queued || swt->check != &timerList) 
    {        
        swt->next = timerList;
        timerList = swt;
        swt->check = &timerList;
        swt->queued = 1;
    }
}

void SWT_BackgroundTimerCallback(
            swtBg_t *swt, 
            timerCallback cb, 
            uint32_t timeInMs,
            uint16_t runCount,
            intptr_t context)
{
    swt->cbObj.obj = context;
    swt->cbObj.cb = cb;
    swt->runCount = runCount;
    SW_TIMER_SET(swt->timer, (timeInMs * BACKGROUND_TICKS_PER_MS), GetBackgroundTimer());

    swt->paused = 0;  // future use
    // if user tries to reuse an already queued structure, don't re-queue it
    if(!swt->queued || swt->check != &timerList)
    {
        swt->next = timerList;
        timerList = swt;
        swt->check = &timerList;
        swt->queued = 1;
    }
}

// deallocate all the timers
void SWT_BackgroundTimersReset(void)
{
    swtBg_t *next = timerList;
    timerList = NULL;
    while(next && next->check == &timerList)
    {
        swtBg_t *this = next->next;
        next->queued = 0;
        next->paused = 0;
        next->next = NULL;
        next->check = NULL;
        next = this;
    }
    return;
}

int SWT_IsTimerActive(swtBg_t *swt) {return swt->queued;}
int SWT_IsTimerPaused(swtBg_t *swt) {return swt->paused == 1;}
int SWT_IsTimerStopped(swtBg_t *swt) {return swt->paused == 2 && swt->queued;}
int SWT_IsTimerRunning(swtBg_t *swt) {return swt->paused == 0 && swt->queued;}
int SWT_DidTimerFinish(swtBg_t *swt) {return swt->paused == 2 && !swt->queued;}


// pause the timer by storing current progress
int SWT_PauseTimer(swtBg_t *swt)
{
    if(!swt->queued) return -1;
    swt->paused = 1; 
    // save how far we have advanced
    swt->timer.start =  GetBackgroundTimer() - swt->timer.start;
    return 0;
}

// stop the timer, will start from 0 if restarted
int SWT_StopTimer(swtBg_t *swt)
{
    if(!swt->queued) return -1;
    swt->paused = 2;
    return 0;
}

// restart from stop or pause
int SWT_RestartTimer(swtBg_t *swt)
{   
    if(!swt->queued) return -1;
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


void SWT_BackgroundTimerKill(swtBg_t *swt)
{
    if(swt->queued)
    {
        swt->cbObj.obj = 0; // short circuit any pending calls best we can
        swt->cbObj.cb = NULL;
        swt->paused = 3;
        
        // find and remove from list
        swtBg_t **prev = &timerList;    // points where the previous pointer is stored
        swtBg_t *pend = *prev;      // get the first item in the list
        while(pend)
        {           
            if (pend == swt)
            {
                *prev = pend->next; // remove from queue
                pend->next = NULL;
                pend->queued = 0; // mark dequeued
                pend->check = NULL; // cleanup
                return;
            }
        }
    }
}

// Prevent timer from matching. No test for if it has already expired.
void SWT_BackgroundTimerDelay(swtBg_t *swt)
{
    swt->timer.start = GetBackgroundTimer();
}


// wrapper around whatever background tick we want to use
uint32_t GetBackgroundTimer(void)
{
    extern volatile uint32_t *sysCounter;
#ifdef TEST
    (*sysCounter)++;
#endif    
    return *sysCounter;
}
