/**
 *  @file swtimers.h
 *  @brief Interface definition to software timer functions 
 *  
 */
#ifndef _SWTIMERS_H
#define _SWTIMERS_H
#include "taskCounter.h"

typedef void (*timerCallback)(intptr_t context);
typedef struct swtH_s swtHandle_t;

/** @brief Associate a fast (hardware) timer with a task counter.
 *  
 *  @param [in] timeInNs The timer duration in nanoseconds
 *  @param [in] taskNo   The previously defined taskNo
 *  @param [in] runCount Number of duration times to repeat before stopping,
 *                       where 0 indicates continuous operation.
 *  @return a software timer handle or NULL if no timers are available.
 *  
 *  @details 
 */
swtHandle_t *SWT_FastTimerCount(
                    uint32_t timeInNs, 
                    tcHandle_t taskNo,
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
                    uint32_t timeInNs, 
                    timerCallback cb, intptr_t context,
                    uint32_t runCount);                           

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
swtHandle_t *SWT_SysTimerCount(
                    uint32_t timeInMicroSeconds,
                    tcHandle_t task, 
                    uint32_t runCount);

/** @brief Brief
 *  
 *  @param [in] timeInMicroSeconds The timer duration in uSecs.
 *  @param [in] cb                 A callback function
 *  @param [in] context            A callback specific context value
 *  @param [in] runCount           Number of duration times to repeat before stopping,
 *                                 where 0 indicates continuous operation.
 *  @return a software timer handle or NULL if no timers are available.
 *  
 *  @details Details
 */                    
swtHandle_t *SWT_SysTimerCallback(
            uint32_t timeInMicroSeconds, 
            timerCallback cb, intptr_t context,
            uint32_t runCount);

            
/** @brief Get the storage requirements for one timer
 *  
 *  @return size in bytes of a timer.
 *  
 *  @details Used during setup to allocate space for the system timer pool.
 */            
size_t SWT_sizeofTimer(void);            
            
/** @brief Assign memory for the system timer pool.
 *  
 *  @param [in] space     void pointer to allocated memory.
 *  @param [in] spaceSize size of allocated memory in bytes.
 *  
 *  @details Details
 */
void SWT_InitSysTimers(void *space, size_t spaceSize);



#endif // _SWTIMERS_H
