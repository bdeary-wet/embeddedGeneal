#ifndef _SWTIMERSUPPORT_H_
#define _SWTIMERSUPPORT_H_

#ifdef TEST
#include "swtimer_test.h"
#else
// The supported micro, change for you project
#include "swtimer_st.h"
#endif

#define TICKS_PER_USEC 4

// device specific interface. to be writen for each supported micro
void SWT_MicroSetup(future_t *fut, int chans);
void SWT_start(future_t *fut);
void SWT_stop(future_t *fut);
void SWT_recycle(future_t *fut); 
int SWT_delay(future_t *fut, fastTimer_t newTime);
uint64_t SWT_GetTimeUsecLL(void);
uint32_t SWT_GetTimeUsec(void);

#endif
