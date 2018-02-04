#include "swtimers.h"
//#include "swtimer.h"
#include "gencmdef.h"
//#include "taskService.h"
#include "generalQueue.h"
#include <string.h>

#if defined( USE_GENDEF ) || defined( TEST )
extern intptr_t TS_SignalTask(intptr_t task);
static objFunc_f taskFunc = (objFunc_f)TS_SignalTask;
#else
static objFunc_f taskFunc;
#endif

void SWT_SetTaskCaller(objFunc_f userTaskFunc)
{
    taskFunc = userTaskFunc;
}

static void onRelease(intptr_t const obj);

typedef struct swtH_s 
{
    objCallbackWrapper_t cbObj; ///< q-able callback structure or task handle
    uint32_t downCount;     ///< actual timer, init this last
    uint32_t runCount;      ///< 0 for continuous, else number of runs
    uint32_t reloadVal;      ///< For continuous timer operation
    int32_t direction;
} swt_t;

// called when timer ticks down to 0
// returns true if timer still allocated
static int doTimerTerm(swt_t *timer)
{
    // if multiple or continuous operation reload timer
    if(timer->runCount != 1)
    {
        if (timer->runCount) timer->runCount--;
        timer->downCount = timer->reloadVal;
        return 1;
    }
    else // return timer to the pool.
    {
        GenPool_Return(timer); // dequeue the timer
        return 0; // timer deallocated
    }
}

static genPool_t *swtTimerPool;
size_t sysTimers = SW_TIMERS;


// take memory chunk and use it as handle pool
int SWT_InitSysTimers(void)
{
    GenPoolAllocateWithCallback(swtTimerPool, swt_t, SW_TIMERS, onRelease);
    return SW_TIMERS;
}

// start timer with callback 
swtHandle_t SWT_SysTimerCallback(
            objFunc_f cb, 
            uint32_t timeInMilliseconds, 
            uint32_t runCount,
            intptr_t context)
{
    swt_t *swt = GenPool_Get(swtTimerPool);
    if (swt)
    {
        swt->reloadVal = timeInMilliseconds;
        swt->runCount = runCount;
        swt->cbObj.cb = cb;
        swt->cbObj.obj = context;
        swt->downCount = timeInMilliseconds; // set last
    }
    return swt;
}
            
            
// use the taskFunc to callback the task scheduler            
swtHandle_t SWT_SysTimerTask(
                    intptr_t task, 
                    uint32_t timeInMilliseconds,
                    uint32_t runCount)
{
    if (!taskFunc) return NULL;
    return SWT_SysTimerCallback(taskFunc,timeInMilliseconds,runCount,task);
}
        

// attach a timer to a user timer        
swtHandle_t SWT_SysTimerCount(
                    int32_t *myCount,
                    int32_t timeInMilliseconds,
                    uint32_t countLimit)
{
    swt_t *swt = GenPool_Get(swtTimerPool);
    if (swt)
    {
        swt->cbObj.obj = (intptr_t)myCount;
        swt->cbObj.cb = NULL;
        swt->reloadVal = timeInMilliseconds;
        swt->runCount = countLimit;
        if(timeInMilliseconds < 0)
        {
            swt->direction = -1;
            swt->downCount = -timeInMilliseconds;  // must be last
        }
        else
        {
            swt->direction = 1;
            swt->downCount = timeInMilliseconds;  // must be last
        }
    }
    return swt;
}     

uint32_t SWT_GetCount(swtHandle_t timer)
{
    return timer->downCount;
}



// callback buffer timerIsr -> pendsv
static objCallbackWrapper_t cbBuf[FAST_PEND_DEPTH*TIMER_CHANNELS];  
static genQ_t pendsvQueue;        // queue for callback buffer

void runTimers(intptr_t const obj);

// Call inside the system tick 
void SWT_OnSysTick(void)
{
#ifdef HIGHSPEED  // if triggered by some other timer
    static objCallbackWrapper_t run;
    if(!run.cb){ run.cb = runTimers; run.obj = (intptr_t)swtTimerPool; }
    GenQ_Put(&pendsvQueue, &run); // queue the process
#else // Timer is systeick                  
    runTimers((intptr_t)swtTimerPool);
#endif
}

//----------------------------------------------------------------------------//

// Fast timer area, high priority interrupt will queue a callback for the 
// pendsv isr service to process.





// This is the PendSv service routine, justs pulls callbacks out of the 
// queue and calls them.  The PendSv is assumed to run at near the lowest
// priority.



uint32_t rependSvCnt;
uint32_t resetCallbackCnt;

void SWT_PendService(void)
{
    objCallbackWrapper_t entry;
    // run for as long as there are things in queue
    while (!GenQ_Get(&pendsvQueue, &entry))
    {
        // clear pending as long as we have data in queue, note the re-pend
        if (test_pendsv())
        {
            clear_pendsv();
            ++rependSvCnt;  // keep track of repends to get an idea about backlog
        }
        // if it looks valid, call it
        if(entry.cb) entry.cb(entry.obj);
        else ++resetCallbackCnt;  // keep track of empty callbacks
    }
}

// global fast handles
future_t isrFuture[TIMER_CHANNELS];

void SWT_FastInit(void)
{
    ARRAY_TO_Q(cbBuf, pendsvQueue);
    memset(isrFuture, 0, sizeof(isrFuture));
    SWT_MicroSetup(isrFuture, TIMER_CHANNELS);
}


swtFastHandle_t SWT_FastTimerCallback(
                    timerCallback cb, 
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    intptr_t context)
{
    // look for a free channel
    for(int i=0;i<DIM(isrFuture);i++)
    {
        uint32_t pri = __get_PRIMASK();
        __set_PRIMASK(1);
        if(NULL == isrFuture[i].cbObj.cb)
        {
            // claim the handle
            isrFuture[i].cbObj.cb = cb;
            __set_PRIMASK(pri);
            // do general design stuff
            isrFuture[i].duration = NS_TO_FAST_CLOCKS(timeInNs);
            isrFuture[i].runCount = runCount;
            isrFuture[i].cbObj.obj = context;
            // do hardware specific stuff
            __set_PRIMASK(1);
            SWT_start(&isrFuture[i]);   
            __set_PRIMASK(pri);
            return (swtFastHandle_t)&isrFuture[i]; // return the handle
        }
        __set_PRIMASK(pri);
    }
    return (swtFastHandle_t)NULL;    
}


                    
                    
swtFastHandle_t SWT_FastTimerCount(
                    uint32_t *counter,
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    intptr_t task)
{
return (swtFastHandle_t)NULL;
}

// On interrupt, copy the callback onto the pending queue and pend the pendsv
void SWT_ChanIsr(swtFastHandle_t fa)
{
    future_t *fup = (future_t*)fa;
    if(fup->runCount != 1)
    {
        SWT_recycle(fup);
        if (fup->runCount) fup->runCount--;
        // send callback to processor isr
        GenQ_Put(&pendsvQueue, &(fup->cbObj));
    }
    else
    {
        // send callback to processor isr
        GenQ_Put(&pendsvQueue, &(fup->cbObj));
        // free the resource
        SWT_stop(fup);
        fup->cbObj.cb = NULL;
    }
    // schedule the processor isr
    pend_pendsv();
}


// Delay exiting callback set time from now
bool FutureReset(future_t *fut, uint32_t delay)
{
    uint32_t pri = __get_PRIMASK();
    delay = NS_TO_FAST_CLOCKS(delay);
    __set_PRIMASK(1);
    // if still active
    if(fut->cbObj.cb)
    {
        
        int val = SWT_delay(fut, (fastTimer_t)(getFastCounter() + delay)); 
        __set_PRIMASK(pri);
        return val ;
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
        SWT_stop(fut);
        __set_PRIMASK(pri);
        return true;
    }
    __set_PRIMASK(pri);
    return false;
}



// on active function, process a swt_t
static void countDown(intptr_t const obj)
{
    swt_t *timer = (swt_t*)obj;
    
    // if counter is running
    if(timer->downCount)
    {
        // if counter is expiring
        if(1 == timer->downCount--)
        {
            // if we have a callback
            if(timer->cbObj.cb) {
                // queue fails
                if (GenQ_IsSpace(&pendsvQueue))
                {
                    if (doTimerTerm(timer))
                    {
#ifdef HIGHSPEED        // if triggered by some other timer
                        GenQ_Put(&pendsvQueue, &(timer->cbObj)); // queue the cb
#else                   // if triggered in background call locally.
                        timer->cbObj.cb(timer->cbObj.obj);
#endif
                    }
                }
                else // can't queue the callback to delay
                {
                    timer->downCount=1;
                }
            }
            else if (timer->cbObj.obj) // not callback by user's counter
            {
                (*((int32_t*)timer->cbObj.obj)) += timer->direction;
                doTimerTerm(timer);
            }
        }
    }
}

static void onRelease(intptr_t const obj)
{
    swt_t *timer = (swt_t*)obj;
#ifdef HIGHSPEED        // if triggered by some other timer
    GenQ_Put(&pendsvQueue, &(timer->cbObj)); // queue the cb
#else                   // if triggered in background call locally.
    timer->cbObj.cb(timer->cbObj.obj);
#endif
    
}

// This function prototype is compatible with object callbacks
// and can be queued to the pendsv
void runTimers(intptr_t const obj)
{
    genPool_t const *tp = (genPool_t const *)obj;
    Pool_OnEachActive(tp,countDown);
}



