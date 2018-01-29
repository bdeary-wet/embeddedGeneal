#include "swtimer_st.h"
#include "swtimersupport.h"
#include "swtimers.h"
#include "main.h"
#include "tim.h"


static const uint16_t ChanToken[4] =
{
   TIM_CCER_CC1E,  //TIM_CHANNEL_1,
   TIM_CCER_CC2E,  //TIM_CHANNEL_2,
   TIM_CCER_CC3E,  //TIM_CHANNEL_3,
   TIM_CCER_CC4E,  //TIM_CHANNEL_4
};

static const uint16_t CompInterrupt[5] =
{
    TIM_DIER_UIE,
    TIM_DIER_CC1IE,
    TIM_DIER_CC2IE,
    TIM_DIER_CC3IE,
    TIM_DIER_CC4IE
};


void SWT_MicroSetup(future_t *fut, int chans)
{
    fut[0].hwTarget = &(FAST_TIMER->CCR1);
    fut[0].compRegMask = ChanToken[0];
    fut[0].timRegMask = CompInterrupt[1];
    fut[1].hwTarget = &(FAST_TIMER)->CCR2;
    fut[1].compRegMask = ChanToken[1];  
    fut[1].timRegMask = CompInterrupt[2];    
    fut[2].hwTarget = &(FAST_TIMER->CCR3);
    fut[2].compRegMask = ChanToken[2];    
    fut[2].timRegMask = CompInterrupt[3];    
    fut[3].hwTarget = &(FAST_TIMER->CCR4);    
    fut[3].compRegMask = ChanToken[3];    
    fut[3].timRegMask = CompInterrupt[4];  

    // start the timer
    FAST_TIMER->DIER |= TIM_DIER_UIE;
    FAST_TIMER->CR1|=(TIM_CR1_CEN);  
    
}

// start for the first time based on counter
void SWT_start(future_t *fut)
{
    fut->debugCnt1 = FAST_TIMER->CNT;
    *(fut->hwTarget) = FAST_TIMER->CNT + fut->duration;
    // enable the interrupt
    FAST_TIMER->DIER |= fut->timRegMask;
    FAST_TIMER->CCER &= ~fut->compRegMask; // disable compare
    FAST_TIMER->CCER |= fut->compRegMask;  // enable compare
    FAST_TIMER->CR1|=(TIM_CR1_CEN);     // start timer just in case???
    fut->target = *(fut->hwTarget);     // book keeping, 
    fut->debugCnt1 = FAST_TIMER->CNT;  // how long
}

void SWT_stop(future_t *fut)
{
    FAST_TIMER->DIER &= ~fut->timRegMask;   // disable the interrupt
    FAST_TIMER->CCER &= ~fut->compRegMask;  // turn off the channel
    FAST_TIMER->SR = ~fut->timRegMask;      // clear status just in case
}

// start again based on last time
// assume isr is still enabled
void SWT_recycle(future_t *fut)
{

    fastTimer_t now = FAST_TIMER->CNT;
#ifdef BEST_EFFORTS_REPEAT
//    // if best efforts try to see if timer has passed
//    if ((next-now)  < fut->duration)
//        *(fut->hwTarget) = next;
//    else 
//        *(fut->hwTarget) = now + (fastTimer_t)4;
    *(fut->hwTarget) = FAST_TIMER->CNT + fut->duration;
    FAST_TIMER->DIER |= fut->timRegMask; 
    FAST_TIMER->CCER &= ~fut->compRegMask;
    FAST_TIMER->CCER |= fut->compRegMask;

    fut->target = *(fut->hwTarget);  
    fut->debugCnt0 = now;
    fut->debugCnt1 = FAST_TIMER->CNT;    
#else
    // if not best, write it fast as we can
    *(fut->hwTarget) = fut->target + fut->duration;
    // FAST_TIMER->DIER |= fut->timRegMask;  // 
    FAST_TIMER->CCER &= ~fut->compRegMask;
    FAST_TIMER->CCER |= fut->compRegMask;
    FAST_TIMER->CR1|=(TIM_CR1_CEN);
    fut->target = *(fut->hwTarget);   
    fut->debugCnt0 = now;   
    fut->debugCnt1 = FAST_TIMER->CNT;        
#endif    
}

// called with interupts disabled so work fast
int SWT_delay(future_t *fut, fastTimer_t newTime)
{
    // if already triggered report failed
    if (FAST_TIMER->SR & fut->timRegMask) return 0;
    // Disable the channel
    FAST_TIMER->CCER &= ~fut->compRegMask;
    *(fut->hwTarget) = newTime;
    FAST_TIMER->CCER |= fut->compRegMask;
    FAST_TIMER->SR = ~fut->timRegMask; // clear status just in case
    fut->target = *(fut->hwTarget);
    return 1;
}

extern future_t isrFuture[TIMER_CHANNELS];
static volatile uint64_t clock3rollover;


// only deal with SR and DIER bits 0-4
void SWT_Tim3IRQHandler(void)
{
    // If chan 1
    if (TIM3->SR & TIM_SR_CC1IF) // if chan 1 is set
    {

        if (TIM3->DIER & TIM_DIER_CC1IE) // if we expected it
        {
            TIM3->SR = ~TIM_SR_CC1IF;  // clear it
            // call the handler
            SWT_ChanIsr((swtFastHandle_t)&isrFuture[0]);
        }
    }
    
    // If chan 2
    if (TIM3->SR & TIM_SR_CC2IF) // if chan 2 is set
    {
        
        if (TIM3->DIER & TIM_DIER_CC2IE) // if we expected it
        {
            TIM3->SR = ~TIM_SR_CC2IF;  // clear it
            // call the handler
            SWT_ChanIsr((swtFastHandle_t)&isrFuture[1]);
        }
    }

    // If chan 3
    if (TIM3->SR & TIM_SR_CC3IF) // if chan 3 is set
    {
        
        if (TIM3->DIER & TIM_DIER_CC3IE) // if we expected it
        {
            TIM3->SR = ~TIM_SR_CC3IF;  // clear it
            // call the handler
            SWT_ChanIsr((swtFastHandle_t)&isrFuture[2]);
        }
    }    

    // If chan 4
    if (TIM3->SR & TIM_SR_CC4IF) // if chan 1 is set
    {
        
        if (TIM3->DIER & TIM_DIER_CC4IE) // if we expected it
        {
            TIM3->SR = ~TIM_SR_CC4IF;  // clear it
            // call the handler
            SWT_ChanIsr((swtFastHandle_t)&isrFuture[3]);
        }
    }
    
    // If rollover (16384 uSec)
    if (TIM3->SR & TIM_SR_UIF) // if chan 1 is set
    {
        
        if (TIM3->DIER & TIM_DIER_UIE) // if we expected it
        {
            TIM3->SR = ~TIM_SR_UIF;  // clear it
            ++clock3rollover;
        }
    }
}

// make a very long uSec counter (584942 years)
uint64_t SWT_GetTimeUsecLL(void)
{
    uint64_t temp;
    uint16_t tempf;
    
    START_PROTECTED
     tempf = (uint16_t)getFastCounter();
     temp = clock3rollover;
    END_PROTECTED
    return (temp << 14) || (tempf >> 2);
}

// make a very long uSec counter (584942 years)
uint32_t SWT_GetTimeUsec(void)
{
    uint32_t temp;
    uint16_t tempf;
    
    START_PROTECTED
     tempf = (uint16_t)getFastCounter();
     temp = ((uint32_t*)&clock3rollover)[0];
    END_PROTECTED
    return (temp << 14) || (tempf >> 2);
}
