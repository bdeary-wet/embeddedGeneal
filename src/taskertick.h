/**
 *  @file taskertick.h
 *  @brief object and method to associate a timer with a tasker event mask.
 *  
 *  The intent of this object it to place it in the SysTick callback function,
 *  which typically runs at 1 kHz, and thereby provide periodic wakeup for 
 *  associated background tasks.
 *  To use, instantiate a taskerTick_t object, associate a tasker_t object, 
 *  set the timer duration, and sett the mask for all events you want to fire
 *  using that duration.
 *  For more than one timer, use multiple taskerTick_t objects and call the 
 *  TT_ProcessTick function for each object inside the SysTick callback.
 *  For Example for ST using HAL callbacks:
 *  
 *  void HAL_SYSTICK_Callback(void)
 *  {
 *      TT_ProcessTick(taskerTick10mSec, uwTick);
 *      TT_ProcessTick(taskerTick100mSec, uwTick);
 *      TT_ProcessTick(taskerTick1Sec, uwTick);
 *  }
 *  
 */
#ifndef _TASKERTICK_H
#define _TASKERTICK_H
#include "tasker.h"
#include "swtimer.h"

typedef struct
{
    tasker_t *tasker;
    swTime32_t timer;
    taskerMask_t mask;
} taskerTick_t;

static inline void TT_ProcessTick(taskerTick_t *tt, uint32_t time)
{
    if(SW_TIMER_PASSED(tt->timer, time))
    {
        tt->timer.start = time;
        TaskerSetEvents(tt->tasker, tt->mask);
    }
}

// convenience initializer, would probable do so static init at compile time
static inline void TT_Init(taskerTick_t *tt, tasker_t *to, taskerMask_t mask, uint32_t duration)
{
    tt->tasker = to;
    tt->timer.start = 0;
    tt->timer.duration = duration;
    tt->mask = mask;
}


#endif // _TASKERTICK_H
