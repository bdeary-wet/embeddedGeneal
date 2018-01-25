#ifndef _SWTIMER_ST_H
#define _SWTIMER_ST_H

#define BEST_EFFORTS_REPEAT

#include "tim.h" // includes main.h for user defines for pins and peripherals 
#include "../gencmdef.h"

#define FAST_TIMER      htim3
#define TIMER_CHANNELS  4
typedef uint16_t fastTimer_t;

#define T3SCALER ((0xffff/T3_NS_PER_COUNT)+1)
#define NS_TO_FAST_CLOCKS(ns) (((ns)*T3SCALER)>>16)

static inline uint16_t getFastCounter(void)
{
    return htim3.Instance->CNT;
}

// device specific 
typedef struct
{
    objFuncQueue_t cbObj;
    uint32_t runCount;      // 0 for continious, else count down
    fastTimer_t duration;   // the repeat interval
    fastTimer_t target;     // the last target
    uint16_t token;
    uint32_t isrMask;
    fastTimer_t debugCnt0;
    fastTimer_t debugCnt1;
    uint32_t volatile *hwTarget; // where we write the data.
} future_t;

void SWT_MicroSetup(future_t *isrFuture, int chans);

void SWT_start(future_t *fut);
void SWT_recycle(future_t *fut); 





#endif // _SWTIMER_ST_H
