#ifndef _TASKSERVICE_H
#define _TASKSERVICE_H
#include <stdint.h>
#include <stddef.h>
#include "generalQueue.h"

/// Tasks are void functions
typedef void (*taskFunc_f)(void);

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
    
    
} TS_ErrorCodes_e;

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
int TS_AddTask(taskFunc_f task);

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
 *  
 *  Copyright 2018
 */
int TS_AddTaskWithQueue(taskFunc_f task, genQ_t *q);


int TS_SignalTask(uint8_t taskNo);

int TS_Put(uint8_t taskNo, void const *val);

int TS_Get(void *val);
int TS_Test(void);


genQ_t *TS_GetQueueObjectOutside(uint8_t taskNo);







#endif // _TASKSERVICE_H
