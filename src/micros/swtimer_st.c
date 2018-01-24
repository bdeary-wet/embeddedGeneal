#include "swtimer_st.h"
#include "swtimersupport.h"
#include "main.h"
#include "tim.h"


static const uint16_t ChanToken[4] =
{
    TIM_CHANNEL_1,
    TIM_CHANNEL_2,
    TIM_CHANNEL_3,
    TIM_CHANNEL_4
};


void SWT_MicroSetup(future_t *isrFuture, int chans)
{
    isrFuture[0].hwTarget = &htim3.Instance->CCR1;
    isrFuture[0].token = ChanToken[0];
    isrFuture[1].hwTarget = &htim3.Instance->CCR2;
    isrFuture[1].token = ChanToken[1];    
    isrFuture[2].hwTarget = &htim3.Instance->CCR3;
    isrFuture[2].token = ChanToken[2];    
    isrFuture[3].hwTarget = &htim3.Instance->CCR4;    
    isrFuture[3].token = ChanToken[3];    
}

// start for the first time based on counter
void SWT_start(future_t *fut)
{
    *(fut->hwTarget) = getFastCounter() + fut->duration;
    HAL_TIM_OC_Start_IT(&htim3, fut->token);
    fut->target = *(fut->hwTarget);
}

// start again based on last time
void SWT_recycle(future_t *fut)
{
    *(fut->hwTarget) = fut->target + fut->duration;
    HAL_TIM_OC_Start_IT(&htim3, fut->token);
    fut->target = *(fut->hwTarget);    
}

