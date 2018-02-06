#ifndef _SWTIMER_ST_H
#define _SWTIMER_ST_H

#define BEST_EFFORTS_REPEAT

#include "tim.h" // includes main.h for user defines for pins and peripherals 
#include "gencmdef.h"

#define FAST_TIMER      TIM3
#define TIMER_CHANNELS  4
typedef int16_t fastTimer_t;

#define T3SCALER ((0xffff/T3_NS_PER_TICK)+1)
#define NS_TO_FAST_CLOCKS(ns) (((ns)*T3SCALER)>>16)

static inline fastTimer_t getFastCounter(void)
{
    return (fastTimer_t)FAST_TIMER->CNT;
}

// device specific  
typedef struct
{
// Device independent variables, used in swTimers.c  
    objCallbackWrapper_t cbObj;
    uint32_t runCount;      // 0 for continious, else count down
    fastTimer_t duration;   // the repeat interval
// Device dependent variables, used in swTimer_st.c
    fastTimer_t target;     // the last target
    uint32_t compRegMask;   // control reg mask
    uint32_t timRegMask;    // isr and status mask
    fastTimer_t debugCnt0;
    fastTimer_t debugCnt1;
    uint32_t volatile *hwTarget; // where we write the data.
} future_t;

// pend the pendsv
static inline void pend_pendsv(void)
{
    SET_BIT(SCB->ICSR, SCB_ICSR_PENDSVSET_Msk);
}

// check if pendsv in pending
static inline int test_pendsv(void)
{
    return READ_BIT(SCB->ICSR, SCB_ICSR_PENDSVSET_Msk);
}

// clear pend on pendsv
static inline void clear_pendsv(void)
{
    SET_BIT(SCB->ICSR, SCB_ICSR_PENDSVCLR_Msk);
}


void SWT_Tim3IRQHandler(void);






#endif // _SWTIMER_ST_H
