#ifndef _SWTIMER_TEST_H
#define _SWTIMER_TEST_H

#define BEST_EFFORTS_REPEAT

#include "gencmdef.h"

#define TIMER_CHANNELS  4

#define T3_NS_PER_TICK 250
#define T3SCALER ((0xffff/T3_NS_PER_TICK)+1)
#define NS_TO_FAST_CLOCKS(ns) (((ns)*T3SCALER)>>16)

typedef int16_t fastTimer_t;
typedef struct
{
// Device independent variables, used in swTimers.c  
    objCallbackWrapper_t cbObj;   //
    uint32_t runCount;      // 0 for continious, else count down
    fastTimer_t duration;   // the repeat interval
// Device dependent variables or in this case unit-tests
    fastTimer_t target;     // the last target

} future_t;

fastTimer_t getFastCounter(void);
extern int pendSet;

// pend the pendsv
static inline void pend_pendsv(void)
{
    pendSet = 1;
}

// check if pendsv in pending
static inline int test_pendsv(void)
{
    return pendSet;
}

// clear pend on pendsv
static inline void clear_pendsv(void)
{
    pendSet = 0;
}



#endif // _SWTIMER_ST_H