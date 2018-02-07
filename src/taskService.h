/**
 *  @file taskService.h
 *  @brief A background task service where a task is scheduled via task number
 *         and a task may be associated with a data queue.
 *  
 *  A set of user tasks are added to a task list at startup time and may
 *  optionally be associated with a queue so that a associated function may
 *  send data to the task and schedule it via a single call.
 *  
 *  The primary interface for associated functions require only the task number
 *  and the primary interface for the tasks are self referencing.
 *  
 *  scheduling task functions:
 *      TS_SignalTask   - schedules a task
 *      TS_Put          - enqueues a value and schedules the task
 *  
 *  processing task functions
 *      TS_Test         - Tests if task is re-scheduled
 *      TS_Get          - dequeues a value from associated queue
 *  
 *    
 */
/** @addtogroup services
 *  @{
 */
#ifndef _TASKSERVICE_H
#define _TASKSERVICE_H
#include <stdint.h>
#include <stddef.h>
#include "generalQueue.h"
#include "gencmdef.h"


/// Tasks are void functions
typedef void (*taskFunc_f)(void);
typedef intptr_t taskHandle_t;

/* 
/// @brief Task functions return TS_ErrorCodes if necessary else they return 0
typedef enum
{
    TS_BadParameter = -10,
    TS_TableFull = -9,
    TS_NoTable = -8,
    TS_TaskNotDefined = -7,
    TS_IllegalCall = -6,
    TS_QueueEmpty = -2,
    TS_QueueFull = -1,
    TS_OK = 0,
    
    
} TS_ErrorCodes_e; */

/**
 *  @brief Called prior to While Loop in background task to indicate to required
 *         space for the task table, returns value for allocation function to use.
 *  
 *  @param [in] expectedTasks The Maximum number of tasks to expect.
 *  @return The size in bytes needed to hold the task table.
 *  
 *  @details This function is called during startup to get the size of the
 *           dynamicly allocated task table. The allocated memory is then
 *           passed to the seconf setup function and the tasks are then
 *           added to the system.
 *  
 *  Copyright 2018
 */
size_t TS_InitGetSize(int expectedTasks);

/**
 *  @brief Brief
 *  
 *  @param [in] allocatedTableSpace pointer to allcated space
 *  @param [in] sizeOfTableSpace    size of allocated space in bytes
 *  @return Maximum number of tasks allowed
 *  
 *  @details function initializes the table space, sets tasks to zero and returns
 *           the Maximum number of tasks available to assign.
 *  
 *  Copyright 2018
 */
int TS_Init(uint32_t *allocatedTableSpace, size_t sizeOfTableSpace);


/**
 *  @brief Add a task to the table, eariler added tasks may have higher priority
 *         if a prioritized task runner is later selected.
 *  
 *  @param [in] task the function pointer of the task
 *  @return The assigned task number
 *  
 *  @details Details
 *  
 *  Copyright 2018
 */
taskHandle_t TS_AddTask(taskFunc_f task);

/**
 *  @brief Add a task to the table with an associated queue structure.
 *  
 *  @param [in] taskFn the function pointer of the task
 *  @param [in] q    a pointer to a prior initialized queue structure
 *  @return the assigned task number
 *  
 *  @details Adds a task to the task table with an associoated queue structure.
 *           Using the TS_PutQueue function below will schedule the task to
 *           run.
 *  
 *  Copyright 2018
 */
taskHandle_t TS_AddTaskWithQueue(taskFunc_f taskFn, genQ_t *q);

/** @brief Signal a task to run
 *  
 *  @param [in] task the assigned task number
 *  @return 0 or error code
 *  
 *  @details Details
 */
int TS_SignalTask(taskHandle_t task);
void TS_SignalTaskNoReturn(taskHandle_t task);

/** @brief Put a value into a task's queue and schedule the task
 *  
 *  @param [in] task the assigned task number
 *  @param [in] val    pointer to the value to copy to the queue
 *  @return 0 or error code
 *  
 *  @details copies a value into the queue associated with the task. 
 *           the size of the val object is previously defined when the
 *           task was associated with the queue.
 */
int TS_Put(taskHandle_t task, void const *val);

/** @brief Get the next value from the associated queue of the current running
 *         task.
 *  
 *  @param [out] val pointer to destination of copy.
 *  @return 0 or TS_QueueEmpty
 *  
 *  @details Returns the next value in the queue associated with the currently
 *           running task.
 */
int TS_Get(void *val);

/** @brief Tests to see if there is data in the queue associated with the 
 *         currently running task.
 *  
 *  @return TS_QueueEmpty or 0
 *  
 *  @details Details
 */
int TS_Test(void);

/** @brief Yield to higher priority task
 *  
 *  @details May be called from inside a task or it's function to yield
 *           run time back to a higher priority task. Processing will continue
 *           at the same point when returned.
 */
void TS_Yield(void);

/** @brief helper function to allow a function outside of a running task to
 *         dequeue data from the queue.
 *  
 *  @param [in] task the task number
 *  @return TS_QueueEmpty, 0 or error code
 *  
 *  @details Details
 */
genQ_t *TS_GetQueueObjectOutside(taskHandle_t task);


/** @brief The main background task processor.
 *  
 *  @return returns when there is no task to schedule
 *  
 *  @details Details
 */
void TS_Background(void);



/** @}*/




#endif // _TASKSERVICE_H
