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
/** \addtogroup classes
 *  @{
 */
#ifndef _TASKSERVICECLASS_H
#define _TASKSERVICECLASS_H
#include <stdint.h>
#include <stddef.h>
#include "generalQueue.h"
#include "generalDef.h"

/// Tasks are void functions
typedef void (*taskFunc_f)(void);

typedef intptr_t taskHandle_t;
typedef uint16_t taskCnt_t;
typedef struct tmrStruct_s taskMasterRecord_t;

/// @brief Task functions return TS_ErrorCodes if necessary else they return 0
typedef enum
{
    TSC_BadParameter = -10,
    TSC_TableFull = -9,
    TSC_NoTable = -8,
    TSC_TaskNotDefined = -7,
    TSC_IllegalCall = -6,
    TSC_QueueEmpty = -2,
    TSC_QueueFull = -1,
    TSC_OK = 0,
    
    
} TSC_ErrorCodes_e;

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
 */
size_t TSC_InitGetSize(int expectedTasks);

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
int TSC_Init(taskMasterRecord_t **tmr,
             uint32_t *allocatedTableSpace,
             size_t sizeOfTableSpace);


/**
 *  @brief Add a task to the table, eariler added tasks may have higher priority
 *         if a prioritized task runner is later selected.
 *  
 *  @param [in] task the function pointer of the task
 *  @return The assigned task number
 *  
 *  @details Details
 *  
 */
taskHandle_t TSC_AddTask(taskMasterRecord_t *tmr, taskFunc_f task);

/**
 *  @brief Add a task to the table with an associated queue structure.
 *  
 *  @param [in] task the function pointer of the task
 *  @param [in] q    a pointer to a prior initialized queue structure
 *  @return the assigned task number
 *  
 *  @details Adds a task to the task table with an associoated queue structure.
 *           Using the TS_PutQueue function below will schedule the task to
 *           run.
 */
taskHandle_t TSC_AddTaskWithQueue(taskMasterRecord_t *tmr, 
                                  taskFunc_f task, 
                                  genQ_t *q);

/** @brief Signal a task to run
 *  
 *  @param [in] taskNo the assigned task number
 *  @return 0 or error code
 *  
 *  @details Details
 */
int TSC_SignalTask(taskMasterRecord_t *tmr, taskHandle_t taskNo);

/** @brief Put a value into a task's queue and schedule the task
 *  
 *  @param [in] taskNo the assigned task number
 *  @param [in] val    pointer to the value to copy to the queue
 *  @return 0 or error code
 *  
 *  @details copies a value into the queue associated with the task. 
 *           the size of the val object is previously defined when the
 *           task was associated with the queue.
 */
int TSC_Put(taskMasterRecord_t *tmr, taskHandle_t task, void const *val);


/** @brief Get the next value from the associated queue of the current running
 *         task.
 *  
 *  @param [out] val pointer to destination of copy.
 *  @return 0 or TS_QueueEmpty
 *  
 *  @details Returns the next value in the queue associated with the currently
 *           running task.
 */
int TSC_Get(taskMasterRecord_t *tmr, void *val);

/** @brief Tests to see if there is data in the queue associated with the 
 *         currently running task.
 *  
 *  @return TS_QueueEmpty or 0
 *  
 *  @details Details
 */
int TSC_Test(taskMasterRecord_t *tmr);

/** @brief Yield to higher priority task
 *  
 *  @details May be called from inside a task or it's function to yield
 *           run time back to a higher priority task. Processing will continue
 *           at the same point when returned.
 */
void TSC_Yield(taskMasterRecord_t *tmr);

/** @brief helper function to allow a function outside of a running task to
 *         dequeue data from the queue.
 *  
 *  @param [in] task the task handle
 *  @return TS_QueueEmpty, 0 or error code
 *  
 *  @details Details
 */
genQ_t *TSC_GetQueueObject(taskMasterRecord_t *tmr, taskHandle_t task);


/** @brief The prioritized task processor.
 *  
 *  @return returns when there is no task to schedule
 *  
 *  @details Runs through the list of all tasks prioritized by entry,
 *           earlier run before later.
 */
void TSC_DoTasks(taskMasterRecord_t *tmr);


/** @}*/




#endif // _TASKSERVICECLASS_H
