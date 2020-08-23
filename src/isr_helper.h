#ifndef ISR_HELPER_H
#define ISR_HELPER_H
#include <isr_abstraction.h>

/**
 * @brief Helper function to run a user function protected from 
 *  interrupts
 * 
 * @param cb - the function to run in a CbInstance wrapper
 */
Status_t Run_Protected(CbInstance_t cb);


/**
 * @brief Helper function to run user function with elevated 
 *   protection from interrupts.
 * 
 * @param isr_mask 
 * @param cb 
 * @return STATIC_INLINE 
 */
Status_t Run_Privileged(isr_state_t isr_mask, CbInstance_t cb);
#endif // ISR_HELPER_H
