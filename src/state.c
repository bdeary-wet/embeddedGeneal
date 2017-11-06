#include "state.h"
#include <string.h>

static void doSwitchBookKeeping(sm_t *st)
{
    uint32_t time = st->getTime();      
    st->prev = st->current;         // save prev so next can detect timeouts
    st->prevStateDuration = time - st->stateStartTime; // save run time
    st->stateStartTime = time;                         // start new run time
    st->prevStateEntries = st->currentStateEntries;    // save entries
    st->currentStateEntries = 0;                       // start new entries
    st->currentTimeout = 0;
    st->timeoutCb = NULL;
    st->timedOut = 0;
}

// just do init side of a switch
static stRet_t initProcess(sm_t *st, stateFunc_f stateFunc)
{
    doSwitchBookKeeping(st);
    st->current = stateFunc;
    st->next = stateFunc;
    if (st->current) 
    {
        return st->current(st, SMP_INIT);
    }
    else 
    { 
        return SM_OFF;
    }
}

static stRet_t processError(sm_t *st)
{
    stRet_t rt = SM_ERR;
    st->errors++;
    if (st->onErr) // if we have an error trigger
    {
        if (st->onErr->callback) st->onErr->callback(st);
        // since this is error, we won't call cleanup on prev so do init
        rt = initProcess(st, st->onErr->trigState);
    }
    // else maybe do an init to state 0???
    else if (st->stateList)
    {
        rt = initProcess(st, st->stateList[0]);
    }
    else // else just set to OFF, return error
    {
        initProcess(st, NULL);
    }
    return rt;
}

// switch states, by calling tear down then init
// assumes next != current
static stRet_t processSwitch(sm_t *st)
{
    stRet_t rt = SM_ERR;
    // run any exit code
    if (st->current) 
    {
        // for now, the exit code is not acted upon 
        rt = st->current(st, SMP_EXIT);
    }
    doSwitchBookKeeping(st);
    // switch to new state
    st->current = st->next;
    // if new state was not set to OFF state, run init code
    if (st->current) 
    {
        rt = st->current(st, SMP_INIT);
    }
    else
    {
        rt = SM_OFF;
    }
    
    // if this is target (even if NULL)
    if (st->target) 
    {
        if (st->target->trigState == st->current)
        {
            rt = SM_AT_TARGET;
            if(st->target->callback) st->target->callback(st);
        }
    }
     
    return rt;
}


static stRet_t smProcessRun(sm_t *st)
{
    st->currentStateEntries++;
    // if timeout is pending, do special processing
    if (st->currentTimeout)
    {
        if ((st->getTime()-st->stateStartTime) >= st->currentTimeout)
        {
            st->timedOut = 1;       // set flag
            st->currentTimeout = 0; // turn off timeout
            if (st->timeoutCb) st->timeoutCb(st); // call special
            return st->current(st, SMP_TIMEOUT);    // tell state function
        }
    }
    
    return st->current(st, SMP_RUN);
}

// Do state transitions at beginning of new cycle
static stRet_t smProcessBefore(sm_t *st)
{
    stRet_t rt = SM_OK;
    
    // see if we need to start new state
    if (st->current != st->next) 
    {
        rt = processSwitch(st);
        if (rt == SM_ERR) 
            if (SM_ERR == processError(st)) return SM_ERR;
    }
    
    if (st->current) 
    {
        stRet_t rt2 = smProcessRun(st);
        if (rt2 == SM_ERR) 
            if (SM_ERR == processError(st)) return SM_ERR;
    }
    
    if (rt == SM_AT_TARGET) return rt;
    
    if (!st->current)
    {
        // if we look off and no other status report off
        if (rt == SM_OK) rt = SM_OFF;
    }

    return rt;
}

// Do state transitions at end of current cycle
static stRet_t smProcessAfter(sm_t *st)
{
    stRet_t rt;
    if (st->current) 
    {
        rt = smProcessRun(st);
    }
    else
    {
        rt = SM_OFF;
    }
    
    if (rt != SM_ERR)
    {
        // see if we need to start new state
        if (st->current != st->next) 
        {
            rt = processSwitch(st);
        }
    }
    
    // invoke default error handling
    if (rt == SM_ERR)
    {
        rt = processError(st);
    }
    
    if (!st->current)
    {
        // if we look off and no other status report off
        if (rt == SM_OK) rt = SM_OFF;
    }
    
    return rt;
}

stRet_t SmProcess(sm_t *st)
{
    if(st->doBefore) return smProcessBefore(st);
    else return smProcessAfter(st);
}

stRet_t SmInit(sm_t *st, 
            stateFunc_f *stateList, 
            size_t states,
            void *context,
            smTrigger_t *onErr, 
            smTrigger_t *target)
{
    if(!st || (!stateList && states)) return SM_BAD_PARAMETER;
    stRet_t rt = SM_OK;
    memset(st,0,sizeof(*st));
    st->stateList = stateList;
    st->states = states;
    st->context = context;
    st->onErr = onErr;
    st->target = target;
    st->getTime = SmTimeBase;
    st->stateStartTime = st->getTime();

    // finally set the init state and do init processing
    if (st->stateList)
        return initProcess(st, st->stateList[0]);
    else 
        return initProcess(st, NULL); // start in off state
}


 stRet_t SmSetNext(sm_t *st, uint32_t state)
 {
    if (state < st->states)
    {
        st->next = st->stateList[state];
        return SM_OK; 
    }
    else // state 0 is at least NULL (off state)
    {
        if (state) return SM_BAD_PARAMETER;
        st->next = NULL;
        return SM_OK; 
    }
 }
 
stRet_t SmSetTimeout(sm_t *st, uint32_t ticks, smTrigCb cb)
{
    st->currentTimeout = ticks;
    st->timeoutCb = cb;
    return SM_OK;
}