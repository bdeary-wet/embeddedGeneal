#ifndef _SLIMOSUSER_H
#define _SLIMOSUSER_H

// user settable to 8,16,or 32
#define OS_TASKS 16

typedef enum
{
    OS_Ok = 0,          ///< Call was successful 
    OS_BadTask,
    OS_NoTaskDefined,
    OS_QueueEmpty,      ///< GetQueue returned nothing
    OS_QueueFull,       ///< Queue was full, val not added
    OS_TaskBusy,        ///< Task is currently running
    OS_InternalError,
} OS_RetCode_t;

typedef void (*voidFunc_f)(void);

OS_RetCode_t OS_AddTask(voidFunc_f task);
OS_RetCode_t OS_AddTaskWithQueue(voidFunc_f task, uintptr_t *qBuf, size_t qLen);

/**
 *  @brief Enable a task.
 *  
 *  @param [in] taskNo - The System assigned task number 
 *  @return OS_RetCode_t
 *  
 *  @details Enables a task if it was previously disabled.
 *           Returns error if bad task number or no task exists for that number.
 */
OS_RetCode_t OS_EnableTask(uint8_t taskNo);

/**
 *  @brief Disable a task from running
 *  
 *  @param [in] taskNo - The system assigned task number.
 *  @return OS_Ok (0) if call is su
 *  
 *  @details Disables a previously enabled task.
 */
OS_RetCode_t OS_DisableTask(uint8_t taskNo);

/**
 *  @brief Signal the OS to schedule the task. 
 *  
 *  @param [in] taskNo - The system assigned task number.
 *  @return OS_Ok (0) if call is successful
 *  
 *  @details If task has a higher priority than current task, it will be scheduled
 *           immediately. If task has a lower priority it will be scheduled in
 *           the future, after the current task completes.
 */
OS_RetCode_t OS_TriggerTask(uint8_t taskNo);

/**
 *  @brief Signal the OS to  schedule the task and send a value to the tasks queue.
 *  
 *  @param [in] taskNo - The system assigned task number.
 *  @param [in] val    Description for val
 *  @return OS_Ok (0) if call is successful
 *  
 *  @details If task has a higher priority than current task, it will be scheduled
 *           immediately. If task has a lower priority it will be scheduled in
 *           the future, after the current task completes.
 */
OS_RetCode_t OS_TriggerTaskQueue(uint8_t taskNo, uintptr_t val);


/**
 *  @brief Brief description
 *  
 *  @param [in] val Description for val
 *  @return OS_Ok (0) if call is successful
 *  
 *  @details 
 */
OS_RetCode_t OS_GetQueue(uintptr_t *val);

/**
 *  @brief Get the currently running task number
 *  
 *  @return The currently running task.
 *  
 *  @details This returns the currently running task number which is useful for
 *           sending a return address to another task or ISR.
 *           If called from inside an ISR, will return the task currently schedule
 *           as a negative number.
 */
int OS_GetTaskNo(void);


#endif