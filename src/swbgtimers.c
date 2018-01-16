#include "swbgtimers.h"



static swtBg_t *bgTimers;


void SWT_BackgroundTimerCount(
            swtBg_t *swt, 
            uint32_t timeInMs,
            tcHandle_t task, 
            uint32_t runCount)
            {}   


void SWT_BackgroundTimerCallback(
            swtBg_t *swt, 
            uint32_t timeInMs,
            timerCallback cb, intptr_t context,
            uint32_t runCount)
            {}            
            
