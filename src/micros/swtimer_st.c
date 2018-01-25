#include "swtimer_st.h"
#include "swtimersupport.h"
#include "swtimers.h"
#include "main.h"
#include "tim.h"


static const uint16_t ChanToken[4] =
{
    TIM_CHANNEL_1,
    TIM_CHANNEL_2,
    TIM_CHANNEL_3,
    TIM_CHANNEL_4
};

static const uint16_t CompInterrupt[4] =
{
    TIM_IT_CC1,
    TIM_IT_CC2,
    TIM_IT_CC3,
    TIM_IT_CC4
};


void SWT_MicroSetup(future_t *isrFuture, int chans)
{
    isrFuture[0].hwTarget = &htim3.Instance->CCR1;
    isrFuture[0].token = ChanToken[0];
    isrFuture[0].isrMask = CompInterrupt[0];
    isrFuture[1].hwTarget = &htim3.Instance->CCR2;
    isrFuture[1].token = ChanToken[1];  
    isrFuture[1].isrMask = CompInterrupt[1];    
    isrFuture[2].hwTarget = &htim3.Instance->CCR3;
    isrFuture[2].token = ChanToken[2];    
    isrFuture[2].isrMask = CompInterrupt[2];    
    isrFuture[3].hwTarget = &htim3.Instance->CCR4;    
    isrFuture[3].token = ChanToken[3];    
    isrFuture[3].isrMask = CompInterrupt[3];    
}

// start for the first time based on counter
void SWT_start(future_t *fut)
{
    fut->debugCnt1 = (fastTimer_t)htim3.Instance->CNT;
    *(fut->hwTarget) = getFastCounter() + fut->duration;
    htim3.Instance->DIER |= fut->isrMask;
    uint32_t tmp = TIM_CCER_CC1E << fut->token;
    htim3.Instance->CCER &= ~tmp;
    htim3.Instance->CCER |= tmp;
    htim3.Instance->CR1|=(TIM_CR1_CEN);
    fut->target = *(fut->hwTarget);
    fut->debugCnt1 = htim3.Instance->CNT;
}

// start again based on last time
void SWT_recycle(future_t *fut)
{
    fastTimer_t next = fut->target + fut->duration;
    fastTimer_t now = htim3.Instance->CNT;
#ifdef BEST_EFFORTS_REPEAT
    
    if ((next-now)  < fut->duration)
        *(fut->hwTarget) = next;
    else 
        *(fut->hwTarget) = now + (fastTimer_t)4;
    
    htim3.Instance->DIER |= fut->isrMask; 
    uint32_t tmp = TIM_CCER_CC1E << fut->token;
    htim3.Instance->CCER &= ~tmp;
    htim3.Instance->CCER |= tmp;
    htim3.Instance->CR1|=(TIM_CR1_CEN);
    fut->target = *(fut->hwTarget);  
    fut->debugCnt0 = now;
    fut->debugCnt1 = htim3.Instance->CNT;    
#else
    *(fut->hwTarget) = next;
    htim3.Instance->DIER |= fut->isrMask;
    uint32_t tmp = TIM_CCER_CC1E << fut->token;
    htim3.Instance->CCER &= ~tmp;
    htim3.Instance->CCER |= tmp;
    htim3.Instance->CR1|=(TIM_CR1_CEN);
    fut->target = *(fut->hwTarget);   
    fut->debugCnt0 = now;   
    fut->debugCnt1 = htim3.Instance->CNT;        
#endif    
}

// Interface to the fast software timer code using the 4 output compare
// registers of TIM3
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    extern future_t isrFuture[TIMER_CHANNELS];
    if (htim != &htim3) return;
    switch(htim->Channel)
    {
        case HAL_TIM_ACTIVE_CHANNEL_1:
        SWT_ChanIsr((swtFastHandle_t)&isrFuture[0]);
        break;
        case HAL_TIM_ACTIVE_CHANNEL_2:
        SWT_ChanIsr((swtFastHandle_t)&isrFuture[1]);        
        break;
        case HAL_TIM_ACTIVE_CHANNEL_3:
        SWT_ChanIsr((swtFastHandle_t)&isrFuture[2]);        
        break;
        case HAL_TIM_ACTIVE_CHANNEL_4:    
        SWT_ChanIsr((swtFastHandle_t)&isrFuture[3]);          
        break;
        case HAL_TIM_ACTIVE_CHANNEL_CLEARED:
        break;
    }
}
