/**
 *  @file slimos.h
 *  @brief Slim OS is a single stack preemptive tasker.
 *  @details A preemptive tasker is almost an RTOS but uses only one stack
 *           so some features like priority elevation are not possible.  
 *  
 */


#ifndef _SLIMOS_H
#define _SLIMOS_H

#include "slimosuser.h"



#define TASKS 16

#if TASKS == 16
typedef uint_fast16_t  taskMask_t;
#elif TASKS == 32
typedef uint_fast32_t  taskMask_t;
#elif TASKS == 8
typedef uint_fast8_t  taskMask_t;
#else
#error TASKS must be 8, 16, or 32    
#endif


typedef struct 
{
    uintptr_t *queue;
    uint_fast8_t limit;
    uint_fast8_t in;
    uint_fast8_t out;

} taskWrapper_t;

typedef struct
{
    voidFunc_f *taskTable;  // pointer to the array of function pointers (tasks)
    taskWrapper_t **wrappers;
    taskMask_t activeMask;  // the active tasks
    taskMask_t pendingMask; // the pending tasks
    taskMask_t disable_mask;
    taskMask_t currentTask; // 
   
     
    
    
    
    
    
} slimos_t;

// The ISR function.  This should be the last line of the SystemTick ISR
void DoSlimOs(slimos_t *os);

// 











#endif // _SLIMOS_H
