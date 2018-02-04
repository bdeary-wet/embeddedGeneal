/**
 *  @file swtimers.h
 *  @brief Interface definition to software timer functions 
 */
 /** \addtogroup services
 *  @{
 */
#ifndef _SWTIMERS_H
#define _SWTIMERS_H
#include "gencmdef.h"
//#include "taskService.h"
#include "swtimersupport.h"

#define FAST_PEND_DEPTH 8  // number of pending callbacks per channel -1


typedef void (*timerCallback)(intptr_t context);
typedef struct swtH_s *swtHandle_t;
    
typedef  intptr_t swtFastHandle_t;


// Call this from your SysTick ISR or a task triggered by the ISR.
void SWT_OnSysTick(void);

// call this function from your pendsv ISR
void SWT_PendService(void);  // The pend isr function
// Called from somewhere in main or application before the while(1) loop
void SWT_FastInit(void);     // The Init function for main
// Called from Inside highspeed timer isr
void SWT_ChanIsr(swtFastHandle_t fh); 


/** @brief Connect a user counter to a fast timer.
 *  
 *  @param [in] counter  Pointer to the user counter
 *  @param [in] timeInNs time between updates in ns
 *  @param [in] runCount 0 for continuous or the number of repeated timings
 *  @param [in] task     optional task to schedule on count
 *  @return the handle to the timer or NULL if no timer is available.
 *  
 *  @details Details
 */ 
swtFastHandle_t SWT_FastTimerCount(
                    uint32_t *counter,
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    intptr_t task);
                    
/** @brief COnnect a fast timer to a callback function.
 *  
 *  @param [in] cb       function pointer to callback
 *  @param [in] timeInNs time between call in ns
 *  @param [in] runCount 0 for continuous or the number of repeated timings
 *  @param [in] context  An optional object to send ans parameter of the callback
 *  @return handle to the timer or NULL if no timer is available.
 *  
 *  @details Details
 */
swtFastHandle_t SWT_FastTimerCallback(
                    timerCallback cb, 
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    intptr_t context);


swtHandle_t SWT_SysTimerCallback(
                    timerCallback cb, 
                    uint32_t timeInMilliseconds, 
                    uint32_t runCount,
                    intptr_t context);
 
                  
swtHandle_t SWT_SysTimerTask(
                    intptr_t task, 
                    uint32_t timeInMilliseconds,
                    uint32_t runCount);

swtHandle_t SWT_SysTimerCount(
                    int32_t *myCount,
                    int32_t timeInMilliseconds,
                    uint32_t countLimit);

/** @brief returns the constant value of timers configured in the system
 *  
 *  @return Return_Description
 *  
 *  @details Details
 */
size_t SWT_FastTimersAvailable(void);

            
/** @brief Function to initialize the system timers 
 *  
 *  @return Return_Description
 *  
 *  @details Details
 */
int SWT_InitSysTimers(void);



/** @brief Gets the hardware specific fast timer resolution in nanoseconds.
 *  
 *  @return resolution in nanoseconds 
 *  
 *  @details Details
 */
uint32_t SWT_FastTimerResolutionNs(void);




/** @}*/

#endif // _SWTIMERS_H
