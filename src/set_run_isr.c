#include "set_run_isr.h"

#include <config.h>
#include <isr_comm.h>
#include <genPool.h>
#include <stdlib.h>
#include <assert.h>

bool system_run = true;
uint32_t time_tick = 0;
uint32_t timer_internal_error=0;
uint32_t task_internal_error=0;

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
STATIC void Run_Tasks(void);

void User_Loop(void)
{
    while(system_run)
    {
        /* user main loop order here is intentional */
        ProcessUser();  // hard user tasks always run, may trigger isr stuff
        ProcessReturnPoolObjects(); // possible isr deferred processing
        Do_Later();     // timer driven event or task
        Run_Tasks();    // queueable tasks which may include things from timer driven
        time_tick++;    // till we have an actual timer
    }
}

STATIC Status_t retrigger_timer(Context_t context);
STATIC Status_t requeue_task(Context_t context);

DefineStaticGenPool(taskHolders, CbInstance_t, TASK_RUNNERS, requeue_task);
DefineStaticGenQ(taskQ1, CbInstance_t*, TASK_RUNNERS);
DefineStaticGenQ(taskQ2, CbInstance_t*, TASK_RUNNERS);
GenQ_t * const taskQs[2] = {&taskQ1_instance, &taskQ2_instance};
STATIC int taskQ=0;
DefineStaticGenPool(futureHolders, TimedCall_t, FUTURE_RUNNERS, retrigger_timer);

Status_t Run_Task(CbInstance_t cb)
{
    if(cb.callback)
    {
        CbInstance_t *holder = GenPool_allocate(taskHolders); // allocate with default requeue
        if(holder)
        {
            *holder = cb;
            assert(Status_OK == GenQ_Put(taskQs[taskQ&1], &holder));
            return Status_OK;
        }
        else
        {
            return Status_FULL;
        }
    }
    else
    {
        return Status_Param;  // bad parameter
    }
    
}

Status_t Run_Task_Once(CbInstance_t cb)
{
    if(cb.callback)
    {
        CbInstance_t *holder = GenPool_allocate_with_callback(taskHolders, NULL, (Context_t){0});
        if(holder)
        {
            *holder = cb;
            assert(Status_OK == GenQ_Put(taskQs[taskQ&1], &holder));
            return Status_OK;
        }
        else
        {
            return Status_FULL;
        }
    }
    else
    {
        return Status_Param;  // bad parameter
    }
}

STATIC void Run_Tasks(void)
{
    int from = taskQ;
    taskQ = (taskQ + 1)&1; // setup requeue location
    
    CbInstance_t *task;
    while(Status_OK == GenQ_Get(taskQs[from],&task))
    {
        if(task->callback) // if function attached call it
        {
            // if user task signals not Ok, remove it from possible retrigger
            if(Status_OK != task->callback(task->context))
            {
                GenPool_extract_callback(task); 
            }
        }

        // try to send it back to the pool, but may be requeued by callback in mete data
        if(GenPool_return(task)) task_internal_error++;
    }
}

// This is a special callback attached as meta data to the task pool object
// it requeue the task and prevents the pool return
STATIC Status_t requeue_task(Context_t context)
{
    CbInstance_t *holder = (CbInstance_t*)context.v_context;
    if(holder)
    {
        if(Status_OK == GenQ_Put(taskQs[taskQ], &holder))
        {
            return Status_Interrupt; // cancel the pool return
        }
        task_internal_error++; // The task was dropped unexpectedly 
    }
    // if got here, return Ok which will cause the pool item to be returned.
    return Status_OK;  
}

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
    static uint32_t last_time = 0xffff;
    if(time_tick != last_time)
    {
        last_time = time_tick;
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
                if (Status_OK != GenPool_return(node)) timer_internal_error++;
            }
            else
            {
                StackPush(&newQ, (LinkBase_t*)node);
            }
            
        }
        timeQ = (TimedCall_t*)newQ;
    }
}

