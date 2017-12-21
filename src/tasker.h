/**
 *  @file tasker.h
 *  @brief Simple set of tasker table functions to run tasks defined in a
 *         table whenever a corresponding event bit is sent. The task function
 *         is called with the event number passed to it.
 */
#ifndef _TASKER_H
#define _TASKER_H

#include <stdint.h>

#ifndef DIM
#define DIM(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

typedef uint32_t taskerMask_t;  // default is 32 bits
#define TASKER_MAX_TASKS (sizeof(taskerMask_t)*8)
#define CHECK_TASKER_TABLE(table) (DIM(table) > TASKER_MAX_TABLE)


// The prototype of a tasker function
typedef void (*taskerFunction_f)(int event);


typedef struct
{
    taskerFunction_f *table;
    unsigned int tableLen;
} taskerTable_t;

typedef struct
{
    taskerTable_t jumpTable;
    taskerMask_t event;  // pending event
    unsigned int next;
} tasker_t;

/** @brief convenience function to initialize the tasker object. 
 *  
 *  @param [in] to pointer to the tasker object
 *  @param [in] pointer to the task function jump table
 *  @param [in] tableLen The number of entries in the jump table
 *  
 */
void TaskerInit(tasker_t *to, taskerFunction_f *jumpTable, unsigned int tableLen);

/** @brief Set single event by number
 *  
 *  @param [in] to pointer to tasker object
 *  @param [in] eventNo The Event number to set in tasker object
 *  
 *  @details Allows you to set one event by event number (not event mask)
 *           Events outside the legal range can't be set. 
 */
void TaskerSetEvent(tasker_t *to, unsigned eventNo);

/** @brief Set one or more events by mask
 *  
 *  @param [in] to pointer to tasker object
 *  @param [in] mask The Events mask to set in the tasker object
 *  
 *  @details Allows you to set multiple events simultaneously by oring an event 
 *           mask.  No error checking is performed, don't set event outside of
 *           the legal mask.
 */
void TaskerSetEvents(tasker_t *to, taskerMask_t mask);

// The following functions represent different ways for the background code to
// operate on a tasker object. 
// The prioritized functions treat the tasks with lower position with higher
// priority and could result in blocking if higher priority tasks trigger
// themselves.
// The RoundRobin functions walk through the tasks in order lower to higher and
// give all tasks an equal opportunity to run.
// The "TillNone" functions continue to execute tasks as long as event bits 
// continue to be set.
// The other functions only execute one task per call.

/** @brief Executes tasks in order low to high, runs at most one task per call.
 *  
 *  @param [in] to pointer to tasker object
 *  @return returns the task number that was executed or 
 *                  -1 if no events where set.
 *  
 *  @details Walks through the task list low to high and executes the first 
 *           task it discovers with an event set. Automatically saves the 
 *           next task in the list to test and returns after running one task
 *           or traversing the list once.
 */
int TaskerRoundRobin(tasker_t *to);

/** @brief Executes tasks in priority order low to high, runs at most one task
 *         per call.
 *  
 *  @param [in] to pointer to tasker object
 *  @return returns the task number that was executed or 
 *                  -1 if no events where set.
 *  
 *  @details Walks through the task list low to high, always restarting at 0,
 *           and executes the first task it discovers with an event set. 
 *           Returns after running one task or traversing the list once.
 */
int TaskerPrioritized(tasker_t *to);

/** @brief Walks through the list low to high an executes all tasks with events
 *         set at the time of entry in order.
 *  
 *  @param [in] to pointer to tasker object
 *  @return returns the count of tasks that ran or 
 *                  -1 if no events where set.
 *  
 *  @details This function is intended to run a set of tasks based on some other
 *           event like a timer. The event mask is copied at entry and only those
 *           events are used to trigger tasks. 
 *           For example this function could be placed inside code that runs
 *           once every second, and will execute only those tasks that already
 *           had event bits set at the one second mark.
 */
int TaskerOnePass(tasker_t *to);

/** @brief Continually loops through the event list cyclically calling tasks 
 *         until no events remain set. 
 *  
 *  @param [in] to pointer to tasker object
 *  
 *  @details Continually runs until no events are set. Calls tasks cyclically. 
 */
void TaskerRoundRobinTillNone(tasker_t *to);

/** @brief Continually processes the event list always calling the highest 
 *         priority task until no events remain set. 
 *  
 *  @param [in] to Parameter_Description
 *  
 *  @details Continually runs until no events are set. Calls higher priority
 *           tasks before lower ones.
 */
void TaskerPrioritizedTillNone(tasker_t *to);

#endif // _TASKER_H
