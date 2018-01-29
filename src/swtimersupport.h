#ifndef _SWTIMERSUPPORT_H_
#define _SWTIMERSUPPORT_H_



#ifdef TEST
typedef struct
{
// Device independent variables, used in swTimers.c  
    objCallbackWrapper_t cbObj;   //
    uint32_t runCount;      // 0 for continious, else count down
    fastTimer_t duration;   // the repeat interval
// Device dependent variables or in this case unit-tests
    fastTimer_t target;     // the last target

} future_t;
#else
// The supported micro, change for you project
#include "swtimer_st.h"
#endif

#define TICKS_PER_USEC 4;

// device specific interface. to be writen for each supported micro
void SWT_MicroSetup(future_t *fut, int chans);
void SWT_start(future_t *fut);
void SWT_stop(future_t *fut);
void SWT_recycle(future_t *fut); 
int SWT_delay(future_t *fut, fastTimer_t newTime);
uint64_t SWT_GetTimeUsecLL(void);
uint32_t SWT_GetTimeUsec(void);

#endif
