/**
 *  @file taskCounterClass.h
 *  @brief Brief
 *  
 */
 /** \addtogroup classes
 *  @{
 */
#ifndef _TASKCOUNTERCLASS_H
#define _TASKCOUNTERCLASS_H
#include <stdint.h>
#include <stddef.h>

/// The system counter type is assumed to be 16 bits but can be changed
/// here globally if system is recompiled. 
typedef uint16_t tcCounter_t;

/// A system generated handle value
typedef intptr_t tcHandle_t;

/// actual underlying structure
typedef struct
{
    tcCounter_t volatile *counter; // pointer to a user counter
    intptr_t context;              // some user supplied context
} tcc_t;


/** @brief Associate a handle to a counter
 *  
 *  @param [in] myCounter Parameter_Description
 *  @return Return_Description
 *  
 *  @details Details
 */
tcHandle_t TCC_AssociateHandle(tcc_t *tcc, tcCounter_t volatile * myCounter,
                               intptr_t context);

/** @brief Signal the task (increment the counter)
 *  
 *  @param [in] taskNo The task counter to increment
 *  @return returns the associated context.
 *  
 *  @details Function to increment the counter
 */

intptr_t TCC_SignalUp(tcHandle_t taskNo);

/** @brief Signal the task (decrement the counter)
 *  
 *  @param [in] taskNo The task counter to increment
 *  @return returns the associated context.
 *  
 *  @details Function to increment the counter
 */
intptr_t TCC_SignalDown(tcHandle_t taskNo);

/** @brief Test the associated counter (return the value)
 *  
 *  @param [in] taskNo Parameter_Description
 *  @return Return_Description
 *  
 *  @details Details
 */
tcCounter_t TCC_Test(tcHandle_t task);


/** @brief Test and clear the associated counter
 *  
 *  @param [in] task Parameter_Description
 *  @return Return_Description
 *  
 *  @details Details
 */
tcCounter_t TCC_TestAndClear(tcHandle_t task);


/** @}*/

#endif // _TASKCOUNTER_H
