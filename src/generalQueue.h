/**
 *  @file generalQueue.h
 *  @brief General purpose queue functions for sending data between
 *         tasks or between ISR functions and background functions.
 *  
 *  @details This uses the queue design of allowing 1 less than the allocated
 *           space to be queued. This design allows the functions to operate
 *           without the use of protected regions or device supported atomic
 *           operations.  
 *           This requires that the totalObjects parameter and allocated
 *           memory buffer must be one object size larger than required.
 *           This also implies that a Queue length of 1 is illegal. 
 */
#ifndef _GENERALQUEUE_H
#define _GENERALQUEUE_H
#include <stdint.h>


typedef struct genQ_s genQ_t;


/**
 *  @brief Initialize a queue object
 *  
 *  @param [in] q            pointer to the q object to initialize
 *  @param [in] buffer       pointer to the memory buffer used to hold the objects
 *                           being queued.
 *  @param [in] objectSize   The size in bytes of one object.
 *  @param [in] totalObjects The number of objects the queue will hold plus one.
 *  @return return 0 if successful 
 *  
 *  @details Initializes a queue object with a memory area and an object size.
 *           Sets the queue to empty.  
 *           Remember the queue must be sized one larger than needed and queue
 *           sizes of less than 2 are not allowed. 
 */
int GenQ_Init(genQ_t *q, void *buffer, uint16_t objectSize, uint16_t totalObjects);

/**
 *  @brief Copy a user object into the queue.
 *  
 *  @param [in] q   pointer to the queue object.
 *  @param [in] obj pointer to the user object to copy
 *  @return 0 if successful or non-zero if queue is full or for other errors
 *  
 *  @details Copies the user object into the queue if there is space available. 
 */
int GenQ_Put(genQ_t *q, void const *obj);

/**
 *  @brief Copy a user object out of the queue
 *  
 *  @param [in] q   pointer tot he queue object
 *  @param [out] obj pointer to location where object should be copied.
 *  @return 0 if successful or non-zero if queue is empty
 *  
 *  @details De-queues an object if available.      
 */
int GenQ_Get(genQ_t *q, void *obj);

/**
 *  @brief Test if data is available in the queue.
 *  
 *  @param [in] q pointer to the queue object
 *  @return non zero if data is available
 *  
 *  @details Tests if data is available in the queue.
 */
int GenQ_IsData(genQ_t *q);

/**
 *  @brief Get the configured object size of the queue.
 *  
 *  @param [in] q pointer tothe queue pbject
 *  @return The configure size of queued objects 
 *  
 *  @details Returns the original objectSize parameter used to initialize the queue.
 *           This is useful for intermediate functions that don't care about the
 *           contained object but must do some buffer operation on an entry,
 *           like copy it to another buffer or message structure.
 */
uint16_t GenQ_ObjectSize(genQ_t *q);



#endif // _GENERALQUEUE_H
