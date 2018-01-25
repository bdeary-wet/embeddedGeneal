/**
 *  @file swtimers.h
 *  @brief Interface definition to software timer functions 
 *  
 *  
 *  
 *  
 */
 /** \addtogroup services
 *  @{
 */
#ifndef _SWTIMERS_H
#define _SWTIMERS_H
#include "gencmdef.h"
#include "taskService.h"
#include "swtimersupport.h"

#define FAST_PEND_DEPTH 8  // number of pending callbacks per channel -1


typedef void (*timerCallback)(intptr_t context);
typedef struct swtH_s swtHandle_t;
    
typedef  intptr_t swtFastHandle_t;



void SWT_OnSysTick(void);

void SWT_PendService(void);  // The pend isr function
void SWT_FastInit(void);     // The Init function for main
void SWT_ChanIsr(swtFastHandle_t fh); 

 
swtFastHandle_t SWT_FastTimerCount(
                    uint32_t *counter,
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    taskHandle_t task);

swtFastHandle_t SWT_FastTimerCallback(
                    timerCallback cb, 
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    intptr_t context);


swtHandle_t *SWT_SysTimerSignalTask(
                    taskHandle_t task, 
                    uint32_t timeInMicroSeconds,
                    uint32_t runCount); 
 
                  
swtHandle_t *SWT_SysTimerCallback(
            timerCallback cb, 
            uint32_t timeInMicroSeconds, 
            uint32_t runCount,
            intptr_t context);

swtHandle_t *SWT_SysTimerCount(
                    uint32_t *counter,
                    uint32_t timeInMicroSeconds,
                    uint32_t runCount,
                    taskHandle_t task);

            
/** @brief Get the storage requirements for one timer
 *  
 *  @return size in bytes of a timer.
 *  
 *  @details Used during setup to allocate space for system timer pools.
 */            
size_t SWT_sizeofSysTimer(void); 

size_t SWT_FastTimersAvailable(void);

            
/** @brief Assign memory for the system timer pool.
 *  
 *  @param [in] space     void pointer to allocated memory.
 *  @param [in] spaceSize size of allocated memory in bytes.
 *  
 *  @details Details
 */
int SWT_InitSysTimers(void *space, size_t spaceSize);



/** @brief Gets the hardware specific fast timer resolution in nanoseconds.
 *  
 *  @return resolution in nanoseconds 
 *  
 *  @details Details
 */
uint32_t SWT_FastTimerResolutionNs(void);

/// Convenience Macros for setting up the system using stack called from main
#define SWT_INIT_SYSTIMERS(timers) \
void *_systimerSpace = alloca(SWT_sizeofTimer() * timers);\
SWT_InitSysTimers(_systimerSpace, SWT_sizeofTimer() * timers)




/** @}*/

#endif // _SWTIMERS_H
