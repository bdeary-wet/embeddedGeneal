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
#include "gencmdef.h"
#include "swtimers.h"
#include "swtimer.h"


/// Timer callback function pointer prototype
typedef void (*timerCallback)(intptr_t context);
typedef swTime32_t swBgTimer_t;

/// User defined value
uint32_t GetBackgroundTimer(void);
#define BACKGROUND_TICKS_PER_MS 1000

/// The background timer object 
typedef struct swtBg_s swtBg_t;
typedef struct swtBg_s
{
    swtBg_t *next;      // linked list pointer;
    swBgTimer_t timer;
    timerCallback cb;   // if null, do task schedule else do callback
    intptr_t taskContext;   // task number or context based on cb 
    uint32_t runCount;  // down counter
} swtBg_t;

void SWT_Background(void);

void SWT_BackgroundTimerTask(
            swtBg_t *swt, 
            taskHandle_t task, 
            uint32_t timeInMs,
            uint32_t runCount);

void SWT_BackgroundTimerCallback(
            swtBg_t *swt, 
            timerCallback cb, 
            uint32_t timeInMs,
            uint32_t runCount,
            intptr_t context);    

void SWT_BackgroundTimersReset(void);

int SWT_IsTimerActive(swtBg_t *swt);


            
/** @}*/
#endif  // _SWBGTIMERS_H_