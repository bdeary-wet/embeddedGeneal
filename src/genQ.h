#ifndef _GENQ_H
#define _GENQ_H

#include <stdint.h>
#include <stdbool.h>

#include <config.h>

/// incomplete def of a generic Q object.
typedef struct GenQ_t
{
    uint8_t * const end;// address of last object in buffer
    const size_t  objectSize; // size of the object    
    uint8_t *next;      // next entry point, modified only by write
    uint8_t *last;      // next source point, modified only by consumer
    void * const base_add;  // buffer base address
} GenQ_t;

/**
 * @brief macro for compile time definition of GenQ structure
 *  given an existing storage pointer of known type and length
 */
#define GenQDef(name, store, len) \
enum {store##_len=len}; \
GenQ_t name##_instance = (GenQ_t){ \
    .base_add=store, \
    .next=(uint8_t*)store, \
    .last=(uint8_t*)store, \
    .end=(uint8_t*)&store[store##_len - 1], \
    .objectSize=sizeof store[0] }; \
GenQ_t * const name = &name##_instance    

/**
 * @brief macro for defining a static queue of given type and length
 * 
 */
#define StaticGenQDef(name, type, len) \
enum {name##_len = len }; \
STATIC type name##_space[name##_len+1] = {0}; \
STATIC GenQ_t name##_instance = (GenQ_t){ \
    .base_add=name##_space, \
    .next=(uint8_t*)name##_space, \
    .last=(uint8_t*)name##_space, \
    .end=(uint8_t*)&name##_space[name##_len], \
    .objectSize=sizeof(type) }; \
STATIC GenQ_t * const name = &name##_instance


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
Status_t GenQ_Init(GenQ_t *q, void *buffer, uint16_t objectSize, uint16_t totalObjects);

void GenQ_Reset(GenQ_t *q);

/**
 *  @brief Copy a user object into the queue.
 *  
 *  @param [in] q   pointer to the queue object.
 *  @param [in] obj pointer to the user object to copy
 *  @return 0 if successful or non-zero if queue is full or for other errors
 *  
 *  @details Copies the user object into the queue if there is space available. 
 */
Status_t GenQ_Put(GenQ_t *q, void const *obj);

/**
 *  @brief Copy a user object out of the queue
 *  
 *  @param [in] q   pointer tot he queue object
 *  @param [out] obj pointer to location where object should be copied.
 *  @return 0 if successful or non-zero if queue is empty
 *  
 *  @details De-queues an object if available.      
 */
Status_t GenQ_Get(GenQ_t *q, void *obj);

/**
 *  @brief Test if data is available in the queue.
 *  
 *  @param [in] q pointer to the queue object
 *  @return non zero if data is available
 *  
 *  @details Tests if data is available in the queue.
 */
bool GenQ_HasData(GenQ_t *q);
bool GenQ_HasSpace(GenQ_t *q);

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
size_t GenQ_ObjectSize(GenQ_t *q);
#endif // _GENQ_H
