#ifndef _TIMERCB_H
#define _TIMERCB_H
#include "swtimer.h"

typedef void (*timerCbCallback_f)(uintptr_t context);




typedef struct
{
    timerCbCallback_f callback;
    uintptr_t context;
    timerObj32_t timer;
    
} timerCb32_t;

// init the base object
void TC_Timer32Init(timerCb32_t *t, timer32_f timeBase);
// associate a callback function
void TC_Timer32Set(timerCb32_t *t, timerCbCallback_f cb, uintptr_t context);
// start the timer
void TC_Timer32Start(timerCb32_t *t, uint32_t duration);
// stop the timer
void TC_Timer32Stop(timerCb32_t *t);
// test timer and trigger the callback
bool TC_Timer32Test(timerCb32_t *t);


typedef struct
{
    timerCbCallback_f callback;
    uintptr_t context;
    timerObj16_t timer;
    
} timerCb16_t;

// init the base object
void TC_Timer16Init(timerCb16_t *t, timer16_f timeBase);
// associate a callback function
void TC_Timer16Set(timerCb16_t *t, timerCbCallback_f cb, uintptr_t context);
// start the timer
void TC_Timer16Start(timerCb16_t *t, uint16_t duration);
// stop the timer
void TC_Timer16Stop(timerCb16_t *t);
// test timer and trigger the callback
bool TC_Timer16Test(timerCb16_t *t);


#endif // _TIMERCB_H
