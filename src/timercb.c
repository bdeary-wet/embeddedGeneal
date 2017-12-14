#include "timercb.h"
#include <stddef.h>

// init the base object
void TC_Timer32Init(timerCb32_t *t, timer32_f timeBase)
{
    SW_Timer32Init(&(t->timer), timeBase);
    t->callback = NULL;
    
}
// associate a callback function
void TC_Timer32Set(timerCb32_t *t, timerCbCallback_f cb, uintptr_t context)
{
    t->callback = cb;
    t->context = context;
}
// start the timer
void TC_Timer32Start(timerCb32_t *t, uint32_t duration)
{
    SW_SetTimer32(&(t->timer), duration);
}
// stop the timer
void TC_Timer32Stop(timerCb32_t *t)
{
    SW_ResetTimer32(&(t->timer));
}

// test the timer, disable the timer and trigger the callback if ready.
// return true if callback was called
bool TC_Timer32Test(timerCb32_t *t)
{
    if (SW_Timer32Passed(&(t->timer)))
    {
        SW_ResetTimer32(&(t->timer));
        t->callback(t->context);
        return true;
    }
    return false;
}


// init the base object
void TC_Timer16Init(timerCb16_t *t, timer16_f timeBase)
{
    SW_Timer16Init(&(t->timer), timeBase);
    t->callback = NULL;
    
}
// associate a callback function
void TC_Timer16Set(timerCb16_t *t, timerCbCallback_f cb, uintptr_t context)
{
    t->callback = cb;
    t->context = context;
}
// start the timer
void TC_Timer16Start(timerCb16_t *t, uint16_t duration)
{
    SW_SetTimer16(&(t->timer), duration);
}
// stop the timer
void TC_Timer16Stop(timerCb16_t *t)
{
    SW_ResetTimer16(&(t->timer));
}

// test the timer, disable the timer and trigger the callback if ready.
// return true if callback was called
bool TC_Timer16Test(timerCb16_t *t)
{
    if (SW_Timer16Passed(&(t->timer)))
    {
        SW_ResetTimer16(&(t->timer));
        t->callback(t->context);
        return true;
    }
    return false;
}