/**
 *  @file taskCounter.h
 *  @brief Interface for assigning a handle to an incrementing counter
 *  
 *  @details Simple association service. Associates a service provided handle to
 *           a user specified counter in memory. Interface provides for signaling
 *           via the handle which will increment the associated counter.
 *           Interface also includes tests for reading the counter and 
 *           reading the counter and simultaneously clearing it. 
 *           Counter is incremented using saturated counting so it 
 *           (won't roll over to zero).
 *  
 */
#ifndef _TASKCOUNTER_H
#define _TASKCOUNTER_H
#include <stdint.h>
#include <stddef.h>

/// @brief The system counter type is assumed to be 32 bits but can be changed
///        here globally if system is recompiled. 
typedef uint32_t TC_Counter_t;

/// @brief A system generated handle value
typedef intptr_t tcHandle_t;

/** @brief Setup the service for the system, allocate all handles needed.
 *  
 *  @param [in] space       pointer to memory block used by system
 *  @param [in] spaceSize   size of space in bytes
 *  
 *  @details Allocate space and initialize a task counter service.
 *           A 4 byte aligned buffer is provided to hold all future task counter
 *           handles. Space required is 4 bytes per handle.
 */
void TC_InitHandleService(void *space, size_t spaceSize);

/** @brief Associate a handle to a counter
 *  
 *  @param [in] myCounter Parameter_Description
 *  @return Return_Description
 *  
 *  @details Details
 */
tcHandle_t TC_AssociateHandle(TC_Counter_t volatile * myCounter);

void TC_SignalTask(tcHandle_t task);

uint32_t TC_Test(tcHandle_t task);
uint32_t TC_TestAndClear(tcHandle_t task);




#endif // _TASKCOUNTER_H
