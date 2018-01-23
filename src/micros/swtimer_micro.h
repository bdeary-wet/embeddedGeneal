/**
 *  @file swtimer_micro.h
 *  @brief header for micro specific interface for timers used to time
 *         software processes.
 *  
 */
#ifndef _SW_TIMER_MICRO_H_
#define _SW_TIMER_MICRO_H_

#include "gencmdef.h"


typedef uint32_t genReg_t;

typedef struct
{
    genReg_t *baseReg;      // root pointer to hardware
    genReg_t compare;       // next target
    genReg_t increment;     // for next 
    genReg_t spare;
} micro_timer_gen_t;



#endif  // _SW_TIMER_MICRO_H_
