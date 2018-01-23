#include "swtimer_st.h"





// Setup the timer2 isr system.
void InitFuture(void)
{
    memset(isrFuture, 0, sizeof(isrFuture));
    memset(bkgFuture, 0, sizeof(bkgFuture));
    
    isrFuture[0].hwTarget = &htim2.Instance->CCR1;
    isrFuture[1].hwTarget = &htim2.Instance->CCR2;
    isrFuture[2].hwTarget = &htim2.Instance->CCR3;
    isrFuture[3].hwTarget = &htim2.Instance->CCR4;
}


// The channel iSR handler
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    switch (htim->Channel)
    {
    case HAL_TIM_ACTIVE_CHANNEL_1:
        FutureIsr(0);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_2:
        FutureIsr(1);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_3:
        FutureIsr(2);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_4:
        FutureIsr(3);
        break;
    default:
        break;
    }
}
