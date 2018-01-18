/**
 *  @file swbgtimers.h
 *  @brief Interface and Class definition for background timers
 *  
 */
/** \addtogroup services
 *  @{
 */
#ifndef _SWBGTIMERS_H_
#define _SWBGTIMERS_H_
#include "swtimers.h"
#include "swtimer.h"

/// Timer callback function pointer prototype
typedef void (*timerCallback)(intptr_t context);

/// The background timer object 
typedef struct swtBg_s swtBg_t;
typedef struct swtBg_s
{
    swtBg_t *next;      // linked list pointer;
    swTime32_t timer;
    timerCallback cb;
    intptr_t taskContext;   // task number or context based on cb 
    uint32_t runCount;  // down counter
} swtBg_t;

                  
/** @brief Add a background timer to the slow timer list and attach a task.
 *  
 *  @param [in] swt      a user provided swtBg_t object
 *  @param [in] timeInMs duration time in milliseconds
 *  @param [in] task     the task number
 *  @param [in] runCount Number of duration times to repeat before stopping,
 *                       where 0 indicates continuous operation.
 *  
 *  @details Details
 */
void SWT_BackgroundTimerCount(
            swtBg_t *swt, 
            uint32_t timeInMs,
            tcHandle_t task, 
            uint32_t runCount);
/** @brief Brief
 *  
 *  @param [in] swt      a user provided swtBg_t object
 *  @param [in] timeInMs duration time in milliseconds
 *  @param [in] cb       A callback function
 *  @param [in] context  A callback specific context value
 *  @param [in] runCount Number of duration times to repeat before stopping,
 *                       where 0 indicates continuous operation.
 *  
 *  @details Details
 */
void SWT_BackgroundTimerCallback(
            swtBg_t *swt, 
            uint32_t timeInMs,
            timerCallback cb, intptr_t context,
            uint32_t runCount);            
/** @}*/
#endif  // _SWBGTIMERS_H_