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
#include "taskService.h"

#define TICKS_PER_USEC 4;

typedef void (*timerCallback)(intptr_t context);
typedef struct swtH_s swtHandle_t;


                    
swtHandle_t *SWT_FastTimerCount(
                    uint32_t *counter,
                    uint32_t timeInNs, 
                    uint32_t runCount);                    
     

/** @brief Associate a fast (hardware) timer with a callback
 *  
 *  @param [in] timeInNs The timer duration in nanoseconds
 *  @param [in] cb       A callback function
 *  @param [in] context  A callback specific context value
 *  @param [in] runCount Number of duration times to repeat before stopping,
 *                       where 0 indicates continuous operation.
 *  @return a software timer handle or NULL if no timers are available.
 *  
 *  @details Details
 */     
swtHandle_t *SWT_FastTimerCallback(
                    timerCallback cb, 
                    uint32_t timeInNs, 
                    uint32_t runCount,
                    intptr_t context);                           

/** @brief Associate a time event from the system tick with a task counter
 *  
 *  @param [in] timeInMicroSeconds The timer duration in uSec.
 *  @param [in] taskNo   The previously defined taskNo
 *  @param [in] runCount Number of duration times to repeat before stopping,
 *                       where 0 indicates continuous operation.
 *  @return a software timer handle or NULL if no timers are available.
 *  
 *  @details Details
 */
swtHandle_t *SWT_SysTimerSignalTask(
                    taskHandle_t task, 
                    uint32_t timeInMicroSeconds,
                    uint32_t runCount); 
 
swtHandle_t *SWT_SysTimerCount(
                    uint32_t *counter,          ///< [in/out] the counter to increment
                    uint32_t timeInMicroSeconds,///< [in] the interval in uSec.
                    uint32_t runCount);         ///< 0=continuous, else count down

                  
swtHandle_t *SWT_SysTimerCallback(
            timerCallback cb, 
            uint32_t timeInMicroSeconds, 
            uint32_t runCount,
            intptr_t context);

            
/** @brief Get the storage requirements for one timer
 *  
 *  @return size in bytes of a timer.
 *  
 *  @details Used during setup to allocate space for system timer pools.
 */            
size_t SWT_sizeofTimer(void); 
size_t SWT_FastTimersAvailable(void);           
            
/** @brief Assign memory for the system timer pool.
 *  
 *  @param [in] space     void pointer to allocated memory.
 *  @param [in] spaceSize size of allocated memory in bytes.
 *  
 *  @details Details
 */
int SWT_InitSysTimers(void *space, size_t spaceSize);
int SWT_InitFastTimers(int needed);


/// Convenience Macros for setting up the system using stack called from main
#define SWT_INIT_SYSTIMERS(timers) \
void *_systimerSpace = alloca(SWT_sizeofTimer() * timers);\
SWT_InitSysTimers(_systimerSpace, SWT_sizeofTimer() * timers)




/** @}*/

#endif // _SWTIMERS_H
