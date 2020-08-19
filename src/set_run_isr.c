#include "set_run_isr.h"

#include <config.h>
#include <isr_comm.h>
#include <genPool.h>
#include <stdlib.h>

bool system_run = true;
uint32_t time_tick = 0;
uint32_t internal_error=0;


typedef struct TimedCall_t
{
    LinkBase_t node;
    uint32_t time;
    CbInstance_t cb;
    int32_t future;  // if 0, run once, <0 retrigger equal, else from current
} TimedCall_t;


TimedCall_t *timeQ = NULL;
TimedCall_t *addedTimeQ = NULL;

void User_Setup(void)
{
    timeQ = NULL;
    time_tick = 0;
}



STATIC void Do_Later(void);

void User_Loop(void)
{
    while(system_run)
    {
        /* user main loop */

        ProcessUser();
        ProcessReturnPoolObjects();
        time_tick++;
        Do_Later();
    }
}

STATIC Status_t retrigger_timer(Context_t context);

DefineStaticGenPool(futureHolders, TimedCall_t, 20, retrigger_timer);

STATIC Status_t _set_future_cb(CbInstance_t cb, uint32_t first, int32_t retrigger)
{
    TimedCall_t *holder = GenPool_allocate(futureHolders);
    if(holder)
    {
        holder->cb = cb;
        holder->time = first + time_tick;
        holder->future = retrigger;
        StackPush((LinkBase_t**)&addedTimeQ, (LinkBase_t*)holder);
        return Status_OK;
    }
    return Status_FULL;
}

Status_t Run_Later(CbInstance_t cb, uint32_t future)
{
    return _set_future_cb(cb, future, 0);

}

Status_t Run_Periodically(CbInstance_t cb, int32_t interval)
{
    return _set_future_cb(cb, abs(interval), interval);        

}

// before returning timer to pool, this callback can potentially 
// re-trigger the timer.
STATIC Status_t retrigger_timer(Context_t context)
{
    TimedCall_t *node = (TimedCall_t *)context.v_context;
    if(node->future)
    {
        if (node->future > 0)
        {
            node->time = (uint32_t)node->future + time_tick;
        }
        else
        {
            node->time += (uint32_t)(-(node->future));
        }
        StackPush((LinkBase_t**)&addedTimeQ, (LinkBase_t*)node);
        return Status_Interrupt;
    }
    return Status_OK;
}

// This is the timer queue processor, run all the functions past time.
STATIC void Do_Later(void)
{
    LinkBase_t *newQ = (LinkBase_t*)addedTimeQ;
    addedTimeQ = NULL;
    while(newQ) // if new items, push them on main stack
    {
        LinkBase_t *node = StackPop(&newQ);
        StackPush((LinkBase_t**)&timeQ, node);
    }
    // while there are items on the stack
    while(timeQ)
    {
        // pop, check time
        TimedCall_t *node = (TimedCall_t*)StackPop((LinkBase_t**)&timeQ);
        if((node->time - time_tick) & 0x80000000) // if passed time
        {
            if(node->cb.callback)
            {
                // if user function throws an interrupt. This is indication it wants
                // to cancel repeating calls                
                if(Status_Interrupt == node->cb.callback(node->cb.context))
                {
                    GenPool_extract_callback(node);  // this disables the retrigger callback
                }
            }
            if (Status_OK != GenPool_return(node)) internal_error++;
        }
        else
        {
            StackPush(&newQ, (LinkBase_t*)node);
        }
        
    }
    timeQ = (TimedCall_t*)newQ;
}

